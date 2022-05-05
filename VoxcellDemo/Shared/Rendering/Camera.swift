import Foundation

import simd

class Camera {
  
    var position:    SIMD3<Float> = SIMD3<Float>( 0.0, 0.0, 0.0 )
    var rotation:    SIMD3<Float> = SIMD3<Float>( 0.0, 0.0, 0.0 )
    var scale:       SIMD3<Float> = SIMD3<Float>( 1.0, 1.0, 1.0 )

    var fov:         Float = ( 70.0 / 180.0 ) * Float.pi
    var aspect:      Float =   1.0
    var near:        Float =   0.001
    var far:         Float = 100.0

    var minDistance: Float =  0.5
    var maxDistance: Float = 40.0
    var target:      SIMD3<Float> = SIMD3<Float>( 0.0, 0.5, 0.0 )

    var distance:      Float = 2.0
    var previousScale: Float = 0.0

    var projectionMatrix: float4x4 {

        return float4x4( projectionFov: fov,
                         near:          near,
                         far:           far,
                         aspect:        aspect      )
    }
  
    var viewMatrix: float4x4 {

        let translateMatrix = float4x4( translation: [ target.x, target.y, target.z - distance ] )

        let rotateMatrix = float4x4( rotationYXZ: [ -1.0 * rotation.x, rotation.y, 0.0 ] )
                                              
        let matrix = ( rotateMatrix * translateMatrix ).inverse

        position = rotateMatrix.upperLeft * -matrix.columns.3.xyz

        return matrix
    }
     
    func zoom( scale: Float ) {

        let sensitivity: Float = 0.05
        distance -= ( scale * sensitivity )
    }

    func rotate(delta: SIMD2<Float>) {
    
       let sensitivity: Float = 0.005

        rotation.y += ( delta.x * sensitivity )

        while rotation.y < 0.0 {
            rotation.y += ( 2.0 * Float.pi )
        }
        while rotation.y >  2.0 * Float.pi  {
            rotation.y -= ( 2.0 * Float.pi )
        }

        rotation.x += ( delta.y * sensitivity )
        while rotation.x < 0.0 {
            rotation.x += ( 2.0 * Float.pi )
        }
        while rotation.x >  2.0 * Float.pi  {
            rotation.x -= ( 2.0 * Float.pi )
        }
    }
}
