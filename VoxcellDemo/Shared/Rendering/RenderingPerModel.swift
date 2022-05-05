import Foundation
import MetalKit

class RenderingPerModel {

    let vertexBuffers:             [ MTKMeshBuffer ]
    let numPerInstanceUniforms:    Int
    var drawCalls:                 [ RenderingPerDrawCall ]
    let perInstanceUniformsBuffer: MTLBuffer?
    let samplerState:              MTLSamplerState?

    var colorPixelFormat:          MTLPixelFormat

    public init(

        device:                 MTLDevice,
        textureStorage:         TextureStorage,
        mdlMesh:                MDLMesh,
        doAlphaBlending:        Bool,
        numPerInstanceUniforms: Int

    ) {
        self.drawCalls              = []
        self.numPerInstanceUniforms = numPerInstanceUniforms
        self.colorPixelFormat       = .invalid
        self.samplerState           = RenderingPerModel.buildSamplerState( device: device )
        perInstanceUniformsBuffer = device.makeBuffer(
            length:  numPerInstanceUniforms * MemoryLayout<UniformsPerInstanceSharedWithVertexShaders>.stride,
            options: .storageModeShared
        )

        do {
            let mtkMesh = try MTKMesh( mesh: mdlMesh, device: device )

            self.vertexBuffers = mtkMesh.vertexBuffers

            for ( i, mtkSubmesh ) in mtkMesh.submeshes.enumerated()  {

                let mdlSubmesh : MDLSubmesh = mdlMesh.submeshes![i] as! MDLSubmesh

                let oneDrawCall = RenderingPerDrawCall(

                    device:           device,
                    textureStorage:   textureStorage,
                    vertexDescriptor: mdlMesh.vertexDescriptor,
                    mdlMaterial:      mdlSubmesh.material!,
                    doAlphaBlending:  doAlphaBlending,
                    numInstances:     numPerInstanceUniforms,
                    mtkSubmesh:       mtkSubmesh
                )
                drawCalls.append( oneDrawCall )

            }
        } catch {
            fatalError()
        }
    }

    public func createPipelineDescriptors( colorPixelFormat: MTLPixelFormat ) {

        self.colorPixelFormat = colorPixelFormat

        for dc in drawCalls {

            dc.createPipelineDescriptors( colorPixelFormat: colorPixelFormat )
        }
    }

    public func updatePerInstanceUniforms( uniforms : [UniformsPerInstanceSharedWithVertexShaders] ) {
    
        let rawPointer = perInstanceUniformsBuffer?.contents()
        rawPointer!.copyMemory( from: uniforms, byteCount: MemoryLayout<UniformsPerInstanceSharedWithVertexShaders>.stride * numPerInstanceUniforms )
    }

    public func encode( encoder :  MTLRenderCommandEncoder ) {

        encoder.setVertexBuffer(
             perInstanceUniformsBuffer,
             offset: 0,
             index:  BufferIndex.PerInstanceUniformVertex.rawValue
        )

        encoder.setFragmentBuffer(
             perInstanceUniformsBuffer,
             offset: 0,
             index:  BufferIndex.PerInstanceUniformVertex.rawValue
        )

        encoder.setFragmentSamplerState( samplerState, index: 0 )

        for ( index, vertexBuffer ) in vertexBuffers.enumerated() {

            encoder.setVertexBuffer( vertexBuffer.buffer, offset: 0, index: index )
        }

        for drawCall in drawCalls {

            drawCall.encode( encoder : encoder )
        }
    }
    
    static func buildSamplerState( device: MTLDevice ) -> MTLSamplerState? {

        let descriptor = MTLSamplerDescriptor()

        descriptor.sAddressMode  = .repeat
        descriptor.tAddressMode  = .repeat
        descriptor.mipFilter     = .linear
        descriptor.maxAnisotropy = 8

        let samplerState = device.makeSamplerState( descriptor: descriptor )
        
        return samplerState
    }
}
