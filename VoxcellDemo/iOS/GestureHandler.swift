import Foundation
import MetalKit


class GestureHandler: NSObject {

    var gesturePreviousScale: CGFloat = 1.0
    var delegate: GentureHandlerDelegate?

    func addGestureRecognizers( to view: UIView ) {
    
        let tap   = UITapGestureRecognizer( target: self, action: #selector( handleTap(sender:) ) )
        view.addGestureRecognizer(tap)
    
        let pan   = UIPanGestureRecognizer( target: self, action: #selector( handlePan(sender:) ) )
        view.addGestureRecognizer(pan)
    
        let pinch = UIPinchGestureRecognizer( target: self, action: #selector( handlePinch(sender:) ) )
        view.addGestureRecognizer(pinch)
    }
  
    @objc func handleTap( sender: UITapGestureRecognizer ) {
        if sender.state == .ended {
            let p = sender.location( in: sender.view )
            delegate?.handleTap( location: SIMD2<Float>( x: Float(p.x), y: Float(p.y)) )
        }
    }
  
    @objc func handlePan( sender: UIPanGestureRecognizer ) {
        let translation = sender.translation( in: sender.view )
        let delta = SIMD2<Float>( Float(translation.x), Float(-translation.y) )
        delegate?.handlePan( delta: delta )
        sender.setTranslation(.zero, in: sender.view)
    }

    @objc func handlePinch( sender: UIPinchGestureRecognizer ) {

        let sensitivity: Float = 20

        let scale = Float( sender.scale - self.gesturePreviousScale ) * sensitivity
    
        if sender.state == .ended {

            self.gesturePreviousScale = 1.0

        } else {

            self.gesturePreviousScale = sender.scale
        }

        delegate?.handlePinch( scale: scale )
    }
}
