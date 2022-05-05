import Foundation
import MetalKit
import MetalPerformanceShaders


public struct IntegerPairKey<T:Hashable> : Hashable {
   public let v1 : T
   public let v2 : T
}

public typealias SweepLines = [ IntegerPairKey<Int16> : [DepthPeelingResult] ]

class DepthPeeler {

    let device:          MTLDevice
    let meshCoordinates: ExtractedCoordinatesFromMTKMesh
    var colorTexture:    MTLTexture?
    let gridInfo:        GridInfo
    let epsilon:         Float

    var vertexBuffer:              MTLBuffer?
    var maskBuffer:                MTLBuffer?
    var rayBuffer:                 MTLBuffer?

    var intersectionBuffer:        MTLBuffer?
    var depthPeelingResultsBuffer: MTLBuffer?
    var normalBuffer:              MTLBuffer?
    var textureUVsBuffer:          MTLBuffer?

    var uniformRayIntersector:        UniformRayIntersector
    var accelerationStructure:        MPSTriangleAccelerationStructure?
    var intersector:                  MPSRayIntersector?
    var initializeRaysPipeline:       MTLComputePipelineState?
    var checkAndUpdateRaysPipeline:   MTLComputePipelineState?


    /// initializes the depth peeler for the specified MTLMesh with the specified configuration
    ///
    /// - parameter device: Metal device
    /// - parameter useExternalShader: Default is False (Framework's own shader is compiled at runtime). Set it to true if your App provides the shader code such as for debugging.
    /// - parameter meshCoordinates: coordinates info from the 3D model to be scanned
    /// - parameter texture: (optional) if color should be sampled, specify the corresponding texture here.
    /// - parameter gridInfo: spec of the 3d grid in which the 3D model is scanned.
    /// - parameter epsilon: perturbation added when the ray position is updated for the next peeling to avoid detecting the same intersection due to numerical errors.
    ///
    /// mtkMesh must contain the following attributes: `MDLVertexAttributePosition`, `MDLVertexAttributeNormal`.
    ///
    /// mtkMesh must contain the following attributes if texture is specified: `MDLVertexAttributeTextureCoordinate`.
    /// mtkMesh.subMeshes must be of `MTLPrimitiveType.triangle`.
    public init(

        device:            MTLDevice,
        useExternalShader: Bool,
        meshCoordinates:   ExtractedCoordinatesFromMTKMesh,
        texture:           MTLTexture?,
        defaultColor:      SIMD3<Float>?,
        gridInfo:          GridInfo,
        epsilon:           Float

    ) {
        self.device           = device
        self.meshCoordinates  = meshCoordinates
        self.colorTexture     = texture
        self.epsilon          = epsilon
        self.gridInfo         = gridInfo

        uniformRayIntersector = UniformRayIntersector()

        arrangeMetalBuffers()
        
        accelerationStructure = MPSTriangleAccelerationStructure( device: device )
        accelerationStructure!.vertexBuffer  = vertexBuffer
        accelerationStructure!.maskBuffer    = maskBuffer
        accelerationStructure!.triangleCount = meshCoordinates.vertices.count / 3;
        accelerationStructure!.rebuild()
        
        intersector = MPSRayIntersector(device: device)
        intersector!.rayDataType          = .originMaskDirectionMaxDistance
        intersector!.rayStride            = MemoryLayout<Ray>.stride
        intersector!.rayMaskOptions       = .primitive
        intersector!.intersectionDataType = .distancePrimitiveIndexCoordinates
        
        uniformRayIntersector.extentMin      = self.gridInfo.extentMin
        uniformRayIntersector.extentMax      = self.gridInfo.extentMax
        uniformRayIntersector.gridDimension  = self.gridInfo.dimensionUInt32()
        uniformRayIntersector.castDirection  = CastDirection.CD_INVALID.rawValue
        uniformRayIntersector.epsilon        = epsilon

        if defaultColor != nil {

            uniformRayIntersector.defaultColor = defaultColor!
        } else {

            uniformRayIntersector.defaultColor = SIMD3<Float>( 0.5, 0.5, 0.5 )
        }

        var library : MTLLibrary?
        if useExternalShader {

            library = device.makeDefaultLibrary()
            if library == nil {
                print ("cannot make default library")
                return
            }
        } else {
            do {
                library = try device.makeLibrary(source: DepthPeelerShadersSourceCode, options: nil)
            } catch {
                fatalError("cannot compile metal code from the baked string.")
            }
        }
        
        let initializeRaysDescriptor = MTLComputePipelineDescriptor()
        initializeRaysDescriptor.threadGroupSizeIsMultipleOfThreadExecutionWidth = true
        initializeRaysDescriptor.computeFunction = library!.makeFunction( name: "initializeRays" )

        let functionConstants = MTLFunctionConstantValues()
        var property = texture != nil
        functionConstants.setConstantValue( &property, type: .bool, index: 0 )

        let checkAndUpdateRaysDescriptor = MTLComputePipelineDescriptor()
        checkAndUpdateRaysDescriptor.threadGroupSizeIsMultipleOfThreadExecutionWidth = true

        do {

            initializeRaysPipeline
                = try device.makeComputePipelineState( descriptor: initializeRaysDescriptor, options: [], reflection: nil )

            checkAndUpdateRaysDescriptor.computeFunction = try library!.makeFunction(
                name:           "checkIntersectionAndUpdateRays",
                constantValues: functionConstants
            )

            checkAndUpdateRaysPipeline
                = try device.makeComputePipelineState(descriptor: checkAndUpdateRaysDescriptor, options: [], reflection: nil)

        } catch {

            print ("cannot make pipeline state")
            return
        }
    }

    /// scan the model along the specified direction and generate sweep lines
    ///
    /// - parameter direction: direction along which the model is scannedl.
    ///
    /// - returns dictionary ( KV-pairs ): Key is the integer grid coordinates (ex. if the direction is CD_X_POSITIVE, the key (i,j) indicates the yz position in the grid.)
    /// Value is the list of scanned restuls along the direction. The results are in `struct DepthPeelingResult`.
    ///
    public func peelDepths( direction: CastDirection ) -> SweepLines? {
    
        uniformRayIntersector.castDirection  = direction.rawValue

        let queue = self.device.makeCommandQueue()

        // initlizes rayBuffer
        performInitializeRays( commandQueue: queue!, direction: direction )

        var sweepLines : SweepLines =  [:]

        var needsUpdate = true

        while needsUpdate {

            performRayIntersector( commandQueue: queue!, direction: direction )

            performCheckAndUpdateRays( commandQueue: queue!, direction: direction )

            updateSweepLinesFromOnePeeledDepth( direction: direction, sweepLines: &sweepLines, needsUpdate: &needsUpdate )
        }

        return sweepLines
    }

    // MARK: private functions

    func updateSweepLinesFromOnePeeledDepth( direction : CastDirection, sweepLines : inout SweepLines,  needsUpdate: inout Bool ) {
    
        let uv = findUVResolutions( direction : direction )
    
        let depthPeelingResultsBufferRawPointer = depthPeelingResultsBuffer?.contents()

        let depthPeelingResultsBufferTypedPointer = depthPeelingResultsBufferRawPointer?.bindMemory(

            to:       DepthPeelingResult.self,
            capacity: MemoryLayout<DepthPeelingResult>.stride * uv!.u * uv!.v
        )

        let depthPeelingResultsBufferBufferedPointer = UnsafeBufferPointer(

            start: depthPeelingResultsBufferTypedPointer,
            count: uv!.u * uv!.v
        )

        needsUpdate = false

        for i in 0 ..< uv!.u {
        
            for j in 0 ..< uv!.v {

                let result = depthPeelingResultsBufferBufferedPointer[ j * uv!.u + i ]

                if ( result.resultType != DepthPeelingResultType.DPRT_NONE.rawValue ) {

                    let xy = IntegerPairKey<Int16>( v1: Int16(i), v2: Int16(j) )

                    if sweepLines[xy] == nil {
                        sweepLines[xy] = []
                    }
                    sweepLines[xy]!.append( result )
                    needsUpdate = true
                }
            }
        }
    }

    func findUVResolutions( direction : CastDirection ) -> ( u: Int, v: Int )? {

        var resolution_u : Int
        var resolution_v : Int

        switch direction {
        
          case .CD_X_POSITIVE:
            resolution_u = Int(self.gridInfo.dimension.y)
            resolution_v = Int(self.gridInfo.dimension.z)
            break;

          case .CD_X_NEGATIVE:
            resolution_u = Int(self.gridInfo.dimension.y)
            resolution_v = Int(self.gridInfo.dimension.z)
            break;

          case .CD_Y_POSITIVE:
            resolution_u = Int(self.gridInfo.dimension.z)
            resolution_v = Int(self.gridInfo.dimension.x)
            break;

          case .CD_Y_NEGATIVE:
            resolution_u = Int(self.gridInfo.dimension.z)
            resolution_v = Int(self.gridInfo.dimension.x)
            break;

          case .CD_Z_POSITIVE:
            resolution_u = Int(self.gridInfo.dimension.x)
            resolution_v = Int(self.gridInfo.dimension.y)
            break;

          case .CD_Z_NEGATIVE:
            resolution_u = Int(self.gridInfo.dimension.x)
            resolution_v = Int(self.gridInfo.dimension.y)
            break;

          default:
            return nil
        }
        return (u: resolution_u, v: resolution_v)
    }

    func findThreadConfiguration( direction : CastDirection ) -> ( threadsPerThreadgroup: MTLSize, threadgroupsPerGrid: MTLSize )
    {
        let threadsPerThreadgroup = MTLSizeMake( 16, 16, 1 )

        let uv = findUVResolutions( direction : direction )
        if uv == nil {
            return ( threadsPerThreadgroup: MTLSizeMake(0,0,0), threadgroupsPerGrid: MTLSizeMake(0,0,0) )
        }

        let threadgroupsPerGrid = MTLSizeMake(
            (uv!.u + threadsPerThreadgroup.width  - 1) / threadsPerThreadgroup.width,
            (uv!.v + threadsPerThreadgroup.height - 1) / threadsPerThreadgroup.height,
            1
        )
        return ( threadsPerThreadgroup: threadsPerThreadgroup, threadgroupsPerGrid: threadgroupsPerGrid )
    }

    func  performInitializeRays( commandQueue: MTLCommandQueue, direction: CastDirection ) {

        guard let commandBuffer = commandQueue.makeCommandBuffer() else {
            print ("cannot make command buffer")
            return
        }

        let encoder = commandBuffer.makeComputeCommandEncoder()
        encoder?.setBytes( &uniformRayIntersector, length: MemoryLayout<UniformRayIntersector>.stride, index: 0 )
        encoder?.setBuffer(rayBuffer, offset: 0, index: 1)
        encoder?.setComputePipelineState( initializeRaysPipeline! )
        let threadConfig = findThreadConfiguration( direction: direction )

        encoder?.dispatchThreadgroups(
            threadConfig.threadgroupsPerGrid,
            threadsPerThreadgroup: threadConfig.threadsPerThreadgroup
        )
        encoder?.memoryBarrier( scope: .buffers )
        encoder?.endEncoding()

        commandBuffer.commit()
        commandBuffer.waitUntilCompleted()
        
    }

    func performRayIntersector( commandQueue: MTLCommandQueue, direction: CastDirection ) {
        
        guard let commandBuffer = commandQueue.makeCommandBuffer() else {
            print ("cannot make command buffer")
            return
        }
        let uv = findUVResolutions( direction: direction )

        intersector!.encodeIntersection(

                commandBuffer:            commandBuffer,
                intersectionType:         .nearest,
                rayBuffer:                rayBuffer!,
                rayBufferOffset:          0,
                intersectionBuffer:       intersectionBuffer!,
                intersectionBufferOffset: 0,
                rayCount:                 uv!.u * uv!.v,
                accelerationStructure:    accelerationStructure!
        )

        commandBuffer.commit()
        commandBuffer.waitUntilCompleted()
    }

    func performCheckAndUpdateRays( commandQueue: MTLCommandQueue, direction: CastDirection ) {

        guard let commandBuffer = commandQueue.makeCommandBuffer() else {
            print ("cannot make command buffer")
            return
        }

        let encoder = commandBuffer.makeComputeCommandEncoder()
        
        encoder?.setBytes( &uniformRayIntersector, length: MemoryLayout<UniformRayIntersector>.stride, index: 0 )
        encoder?.setBuffer( rayBuffer,                   offset: 0, index: 1 )
        encoder?.setBuffer( intersectionBuffer,          offset: 0, index: 2 )
        encoder?.setBuffer( depthPeelingResultsBuffer,   offset: 0, index: 3 )
        encoder?.setBuffer( normalBuffer,                offset: 0, index: 4 )
        encoder?.setBuffer( textureUVsBuffer,            offset: 0, index: 5 )

        encoder?.setTexture( colorTexture, index: 0 )

        encoder?.setComputePipelineState( checkAndUpdateRaysPipeline! )

        let threadConfig = findThreadConfiguration( direction: direction )

        encoder?.dispatchThreadgroups(
            threadConfig.threadgroupsPerGrid,
            threadsPerThreadgroup: threadConfig.threadsPerThreadgroup
        )
        encoder?.memoryBarrier( scope: .buffers )
        encoder?.endEncoding()

        commandBuffer.commit()
        commandBuffer.waitUntilCompleted()
    }
    
    func arrangeMetalBuffers() -> Void {

        let gridDimensionMax = Int( max( max( gridInfo.dimension.x, gridInfo.dimension.y ), gridInfo.dimension.z ) )

        vertexBuffer = device.makeBuffer(
            bytes:   &meshCoordinates.vertices,
            length:  meshCoordinates.vertices.count * MemoryLayout<SIMD3<Float>>.stride,
            options: .storageModeShared
        )

        maskBuffer = device.makeBuffer(
            bytes:   &meshCoordinates.masks,
            length:  meshCoordinates.masks.count * MemoryLayout<Int32>.stride,
            options: .storageModeShared
        )

        rayBuffer = device.makeBuffer(
            length: MemoryLayout<Ray>.stride * gridDimensionMax * gridDimensionMax,
            options: .storageModeShared
        )

        intersectionBuffer = device.makeBuffer(
            length:  MemoryLayout<Intersection>.stride * gridDimensionMax * gridDimensionMax,
            options: .storageModeShared
        )

        depthPeelingResultsBuffer = device.makeBuffer(
            length: MemoryLayout<DepthPeelingResult>.stride * gridDimensionMax * gridDimensionMax,
            options: .storageModeShared
        )

        normalBuffer = device.makeBuffer(
            bytes:   &meshCoordinates.normals,
            length:  meshCoordinates.normals.count * MemoryLayout<SIMD3<Float>>.stride,
            options: .storageModeShared
        )

        textureUVsBuffer = device.makeBuffer(
            bytes:   &meshCoordinates.textureUVs,
            length:  meshCoordinates.textureUVs.count * MemoryLayout<SIMD2<Float>>.stride,
            options: .storageModeShared
        )
    }
}
