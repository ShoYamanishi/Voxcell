import Foundation
import MetalKit

class RenderingPerScene {

    public var models: [ RenderingPerModel ] = []
    
    let maxNumLights:           Int
    var numLights:              Int
    let uniformsBufferVertex:   MTLBuffer?
    let uniformsBufferFragment: MTLBuffer?
    let uniformsBufferLights:   MTLBuffer?

    public init(
        device:       MTLDevice,
        maxNumLights: Int
    ) {
        self.maxNumLights = maxNumLights
        self.numLights    = 0

        uniformsBufferVertex = device.makeBuffer(
            length:  MemoryLayout<UniformsPerSceneSharedWithVertexShaders>.stride,
            options: .storageModeShared
        )

        uniformsBufferFragment = device.makeBuffer(
            length:  MemoryLayout<UniformsPerSceneSharedWithFragmentShaders>.stride,
            options: .storageModeShared
        )
        
        uniformsBufferLights = device.makeBuffer(
            length:  maxNumLights * MemoryLayout<LightSharedWithFragmentShaders>.stride,
            options: .storageModeShared
        )
    }

    public func updateLightsAndCamera( camera: Camera, lights: inout [ LightSharedWithFragmentShaders ] ) {

        var uniformVertex = UniformsPerSceneSharedWithVertexShaders(
            viewMatrix:       camera.viewMatrix,
            projectionMatrix: camera.projectionMatrix
        )

        let rawPointerVertex = uniformsBufferVertex?.contents()
        rawPointerVertex!.copyMemory(
            from:      &uniformVertex,
            byteCount: MemoryLayout<UniformsPerSceneSharedWithVertexShaders>.stride * numLights
        )

        numLights = max(maxNumLights, lights.count)

        let rawPointerLights = uniformsBufferLights?.contents()
        rawPointerLights!.copyMemory(
            from:      lights,
            byteCount: MemoryLayout<LightSharedWithFragmentShaders>.stride * numLights
        )


        var uniformFragment = UniformsPerSceneSharedWithFragmentShaders(
            cameraPosition: camera.position,
            lightCount:     UInt32(numLights)
        )

        let rawPointerFragment = uniformsBufferFragment?.contents()
        rawPointerFragment!.copyMemory(
            from:      &uniformFragment,
            byteCount: MemoryLayout<UniformsPerSceneSharedWithFragmentShaders>.stride * numLights
        )
    }

    public func encode( encoder :  MTLRenderCommandEncoder ) {

        encoder.setVertexBuffer(
             uniformsBufferVertex,
             offset: 0,
             index:  BufferIndex.SceneUniformVertex.rawValue
        )

        encoder.setFragmentBuffer(
             uniformsBufferFragment,
             offset: 0,
             index:  BufferIndex.SceneUniformFragment.rawValue
        )

        encoder.setFragmentBuffer(
             uniformsBufferLights,
             offset: 0,
             index:  BufferIndex.LightsFragment.rawValue
        )

        for model in models {

            model.encode( encoder : encoder )
        }
    }
}
