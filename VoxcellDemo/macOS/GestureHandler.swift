//
//  GestureHandler.swift
//  Voxell (macOS)
//
//  Created by Shoichiro Yamanishi on 02.05.22.
//

import Foundation
import Cocoa
import simd

class GestureHandler: NSObject {

    var gesturePreviousScale: CGFloat = 1.0
    var delegate: GentureHandlerDelegate?

    func addGestureRecognizers( to view: NSView ) {
    
        let tap   = NSClickGestureRecognizer( target: self, action: #selector( handleTap(sender:) ) )
        view.addGestureRecognizer(tap)
    
        let pan   = NSPanGestureRecognizer( target: self, action: #selector( handlePan(sender:) ) )
        view.addGestureRecognizer(pan)
    
        let pinch = NSMagnificationGestureRecognizer( target: self, action: #selector( handlePinch(sender:) ) )
        view.addGestureRecognizer(pinch)
    }
  
    @objc func handleTap( sender: NSClickGestureRecognizer ) {
        if sender.state == .ended {
            let p = sender.location( in: sender.view )
            delegate?.handleTap( location: SIMD2<Float>( x: Float(p.x), y: Float(p.y)) )
        }
    }
  
    @objc func handlePan( sender: NSPanGestureRecognizer ) {
    
        let translation = sender.translation( in: sender.view )
        let delta = SIMD2<Float>( Float(translation.x), Float(translation.y) )
        delegate?.handlePan( delta: delta )
        sender.setTranslation(.zero, in: sender.view)
    }

    @objc func handlePinch( sender: NSMagnificationGestureRecognizer ) {
        delegate?.handlePinch( scale: Float(sender.magnification) )
    }
}
