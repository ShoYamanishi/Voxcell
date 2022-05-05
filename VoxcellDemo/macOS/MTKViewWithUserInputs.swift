import Foundation
import MetalKit


// TODO: Fill in later

class MTKViewWithUserInputs : MTKView {

    var delete : UserInputDelegate?

    override var acceptsFirstResponder: Bool {
        return true
    }

    override func acceptsFirstMouse(for event: NSEvent?) -> Bool {
        return true
    }
  
    override func keyDown( with event: NSEvent ) {}

    override func keyUp(with event: NSEvent) {}

    override func mouseMoved(with event: NSEvent) {
        let screenFrame = NSScreen.main?.frame ?? .zero
        var rect = frame
        frame = convert(rect, to: nil)
        rect = window?.convertToScreen(rect) ?? rect
        CGWarpMouseCursorPosition(NSPoint(x: (rect.origin.x + bounds.midX),
                                          y: (screenFrame.height - rect.origin.y - bounds.midY) ))
    }
  
    override func mouseDown(with event: NSEvent) {}
    override func mouseUp(with event: NSEvent) {}
    override func mouseDragged(with event: NSEvent) {}
    override func rightMouseDown(with event: NSEvent) {}
    override func rightMouseDragged(with event: NSEvent) {}
    override func rightMouseUp(with event: NSEvent) {}
    override func scrollWheel(with event: NSEvent) {}
}
