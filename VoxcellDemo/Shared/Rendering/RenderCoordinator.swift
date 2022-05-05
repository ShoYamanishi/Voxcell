import Foundation
import MetalKit

protocol GentureHandlerDelegate {

    func handleTap   ( location: SIMD2<Float> )
    func handlePan   ( delta:    SIMD2<Float> )
    func handlePinch ( scale:    Float        )
}

// TODO: to be filled. macOS only 
protocol UserInputDelegate {

}

class RenderCoordinator: NSObject, MTKViewDelegate, GentureHandlerDelegate, UserInputDelegate {

    var device:            MTLDevice!
    var commandQueue:      MTLCommandQueue!
    var colorPixelFormat:  MTLPixelFormat!
    var depthStencilState: MTLDepthStencilState!
    var worldManager:      WorldManager?

    var previousFrameSize: CGSize // to detect screen dimension change. SwiftUI does not always call mtkView().

    init( worldManager: WorldManager, device : MTLDevice ) {

        self.worldManager = worldManager
        self.device = worldManager.device
        guard
            let commandQueue = device.makeCommandQueue()
        else {
            fatalError("GPU not available")
        }
        self.commandQueue = commandQueue

        self.depthStencilState = RenderCoordinator.buildDepthStencilState( device: device )!
        self.previousFrameSize = CGSize( width: 0.0, height: 0.0 )

        super.init()
    }

    /// 2nd part of init. It must wait until device and the pixel format become available in MTKView
    func createPipelineDescriptors( colorPixelFormat: MTLPixelFormat ) {
        worldManager?.createPipelineDescriptors( colorPixelFormat: colorPixelFormat)
    }
    
    static func buildDepthStencilState( device: MTLDevice ) -> MTLDepthStencilState? {

        let descriptor = MTLDepthStencilDescriptor()
        descriptor.depthCompareFunction = .less
        descriptor.isDepthWriteEnabled = true
        return device.makeDepthStencilState( descriptor: descriptor )
    }
    
    // MARK: - MTKViewDelegate
    
    func mtkView(_ view: MTKView, drawableSizeWillChange size: CGSize ) {

        self.colorPixelFormat = view.colorPixelFormat
        worldManager?.updateScreenSizes( size: size )
    }

    func draw( in view: MTKView ) {

        if previousFrameSize != view.frame.size  {
            previousFrameSize  = view.frame.size
            self.mtkView( view, drawableSizeWillChange: view.frame.size )
        }

        worldManager?.updateWorld()

        guard
            let descriptor    = view.currentRenderPassDescriptor,
            let commandBuffer = commandQueue.makeCommandBuffer(),
            let encoder = commandBuffer.makeRenderCommandEncoder( descriptor: descriptor )
        else {
            return
        }
        encoder.setDepthStencilState( depthStencilState )

        worldManager?.encode( encoder: encoder )

        encoder.endEncoding()

        guard let drawable = view.currentDrawable
        else {
            return
        }
        commandBuffer.present( drawable )
        commandBuffer.commit()
    }

    // MARK: - GentureHandlerDelegate

    func handleTap   ( location: SIMD2<Float> ) { worldManager?.handleTap   ( location: location ) }

    func handlePan   ( delta:    SIMD2<Float> ) { worldManager?.handlePan   ( delta:    delta    ) }
      
    func handlePinch ( scale:    Float        ) { worldManager?.handlePinch ( scale:    scale    ) }
}
