import Foundation
import MetalKit

public class VolumeSampler {

    public var device: MTLDevice

    public var meshCoordinates: ExtractedCoordinatesFromMTKMesh?

    public var sweepLinesXY: SweepLines?
    public var sweepLinesYZ: SweepLines?
    public var sweepLinesZX: SweepLines?

    public var defaultColor: SIMD3<Float>

    /// information about the 3D grid
    /// The volume is represented by the given 3D grid, whose cells are cubes.
    /// It is represented by two coordinate system: one geometric system in Float, and one integer system.
    /// The relationship between the two is described by the following example.
    ///  Ex. `gridInfo.extentMin = ( 2.0, -5.0, 10.0 ), extentMax = ( 7.0, 5.0, 15.0 ), pitch = 1.0
    ///  Then `gridInfo.dimension = [5, 10, 5],
    ///  and    `[i,j,k]=[0,0,0]` corresponds to `(x,y,z)=(2.0, -5.0, 10.0)`, and `[i,j,k]=[5,10.5]` corresponds to `(x,y,z)=(7.0, 5.0, 15.0)`
    public var gridInfo:     GridInfo?


    /// 3D volume bitmap. 1 (set) represents a mass at the grid cell, 0(not set) represents void.
    public var volumeBitmap: VolumeBitmap?

    /// sample volume for the given 3D object represented by MTKMesh
    /// - parameter device: Metal device
    /// - parameter useExternalShader: Default is False (Framework's own shader is compiled at runtime). Set it to true if your App provides the shader code such as for debugging.
    /// - parameter mdlMesh: the 3D model to be scanned
    /// - parameter texture: associated texture for the 3d model (optional)
    /// - parameter defaultColor: color used if the texture is not given (optional)
    /// - parameter pitch: the grid pitch of the grid in which the 3D models is scanned.
    /// - parameter epsilon: perturbation added when the ray position is updated for the next peeling to avoid detecting the same intersection due to numerical errors.
    public init(
        device:            MTLDevice,
        useExternalShader: Bool,
        mdlMesh:           MDLMesh,
        texture:           MTLTexture?,
        defaultColor:      SIMD3<Float>?,
        pitch:             Float,
        epsilon:           Float
    ) {
        self.device = device

        meshCoordinates = ExtractedCoordinatesFromMTKMesh( mdlMesh: mdlMesh, device: device )

        gridInfo = GridInfo.generateGridInfoFrom( arrayOfPoints: meshCoordinates!.vertices, pitch: pitch )

        if defaultColor != nil {
            self.defaultColor = defaultColor!
        } else {
            self.defaultColor = SIMD3<Float>( 0.5, 0.5, 0.5 )
        }

        let depthPeeler = DepthPeeler(
            device:            device,
            useExternalShader: useExternalShader,
            meshCoordinates:   meshCoordinates!,
            texture:           texture,
            defaultColor:      defaultColor,
            gridInfo:          gridInfo!,
            epsilon:           epsilon
        )

        sweepLinesYZ = depthPeeler.peelDepths( direction: .CD_X_NEGATIVE )
        sweepLinesZX = depthPeeler.peelDepths( direction: .CD_Y_NEGATIVE )
        sweepLinesXY = depthPeeler.peelDepths( direction: .CD_Z_NEGATIVE )

        volumeBitmap = VolumeBitmap( gridInfo: gridInfo! )
        volumeBitmap!.updateAlongZFrom( sweepLinesAlongZ: sweepLinesXY! )
        volumeBitmap!.updateAlongXFrom( sweepLinesAlongX: sweepLinesYZ! )
        volumeBitmap!.updateAlongYFrom( sweepLinesAlongY: sweepLinesZX! )
    }
    
    /// generates MDLMesh of the voxelized object in a composition of quads.
    /// - parameter vertexDescriptor: vertex descriptor to describe the memory layouts(optional)
    /// - parameter material: material used for MDLSubmeshes in MDLMesh
    /// - parameter useTextureColors: true if you want to use the sampled colors from the texture.
    /// - returns MDLMesh generated
    public func generateMDLMesh(

        vertexDescriptor: MDLVertexDescriptor?,
        material:         MDLMaterial?,
        useTextureColors: Bool

    ) -> MDLMesh? {

        var vd: MDLVertexDescriptor
        var m:  MDLMaterial

        if vertexDescriptor == nil {
            vd = RenderUtil.defaultVertexDescriptorPositionNormalColor()
        } else {
            vd = vertexDescriptor!
        }

        if material == nil {
            m = RenderUtil.defaultMaterial( color: defaultColor )
        } else {
            m = material!
        }

        let attributes = ExtractedVertexAttributes( vertexDescriptor: vd )

        let helper = QuadGenerationHelper(
            volumeSampler:              self,
            generatePoints:             attributes.point.hasAttribute,
            generateNormals:            attributes.normal.hasAttribute,
            generateTangents:           attributes.tangent.hasAttribute,
            generateBitangents:         attributes.bitangent.hasAttribute,
            generateTextureCoordinates: attributes.textureCoordinate.hasAttribute,
            generateColors:             attributes.color.hasAttribute,
            useTextureColors:           useTextureColors
        )

        let mdlMesh = MDLMesh.generateFromStructureOfArrays(
            device:              device,
            vertexDescriptor:    vd,
            material:            m,
            structureOfArrays:   helper.structureOfArrays
        )

        return mdlMesh
    }
    
    /// shrinks the size of the grid by one grid pitch along all the three axes.
    ///
    /// Assume the current extent in the x-axis is [-3.0, 5.0] and the pitch is 1.0.
    /// The number of cells along the x-axis  (dimension of x) is 8. After the shrinking,
    /// The extent will be [-2.5, 4.5], and the dimension becomes 7. The pitch does not change.
    /// Conceptually, the points that corresponded to the vertices of the cubes become
    ///  center point of the cubes in the new shrunk grid.
    /// The bit in the new grid is set if the bits in all the 8 surrounding cells are set (except for the boundary cells).
    /// for example the bit at the integer coordinate (3,4,5) in the new grid is set only if the bits in the following 8
    /// integer coordinates in the old grid are set:
    /// (3,4,5), (4,4,5), (3,5,5), (4,5,5), (3,4,6), (4,4,6), (3,5,6), (4,5,6).
    /// This can be useful for collision detectors where it is desirable for the collision spheres to be completely inside the
    /// original 3D object.
    public func shrinkByHalfStep() -> Void {
        volumeBitmap!.shrinkByHalfStep()
    }

    /// finds the integer coordinates of the cells that are at the boundary of the voxelized volume.
    ///
    /// - returns array of integer coordinates in the grid.
    ///
    /// A cell is at the boundary if not all of its 6 orthogonally adjacent cells are set.
    public func findShell() -> [SIMD3<Int32>] {
        return volumeBitmap!.findShell()
    }

    /// finds the 3D spatial coordinates of the center point of the cells that are at the boundary of the voxelized volume.
    ///
    /// - returns array of geometric coordinates in the grid.
    ///
    /// This is useful for generating collision spheres of the 3D object for collision detection
    /// A cell is at the boundary if not all of its 6 orthogonally adjacent cells are set.
    public func findShellCenterPoints() -> [SIMD3<Float>] {
        return volumeBitmap!.findShellCenterPoints()
    }

    /// finds the total mass by sampling based on the volume represented by the bitmap.
    /// - parameter densityPerUnitVolume
    /// - returns Mass
    public func findMass( densityPerUnitVolume: Float ) -> Float {
        return volumeBitmap!.findMass( densityPerUnitVolume: densityPerUnitVolume )
    }

    /// finds the center of mass by sampling based on the volume represented by the bitmap.
    /// - returns CoM in the 3D spatial coordinates.
    public func findCenterOfMass() -> SIMD3<Float> {
        return volumeBitmap!.findCenterOfMass()
    }

    /// finds the 3x3 inertia tensor by sampling based on the volume represented by the bitmap.
    /// - returns intertia tensor
    public func findInertiaTensor( densityPerUnitVolume density: Float ) -> float3x3 {
        return volumeBitmap!.findInertiaTensor( densityPerUnitVolume: density )
    }
}

