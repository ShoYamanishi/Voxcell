import simd

extension Float {

    var radiansToDegrees: Float {

        (self / Float.pi) * 180.0
    }

    var degreesToRadians: Float {

      (self / 180.0) * Float.pi
    }
}

// MARK: - float4

extension float4x4 {

    // MARK: - Translate
    init( translation: SIMD3<Float> ) {

        let matrix = float4x4(
            [           1.0,           0.0,           0.0,         0.0 ],
            [           0.0,           1.0,           0.0,         0.0 ],
            [           0.0,           0.0,           1.0,         0.0 ],
            [ translation.x, translation.y, translation.z,         1.0 ]
        )

        self = matrix
    }
  
    // MARK: - Scale
    init( scaling: SIMD3<Float> ) {

        let matrix = float4x4(
            [ scaling.x,       0.0,       0.0,      0.0 ],
            [       0.0, scaling.y,       0.0,      0.0 ],
            [       0.0,       0.0, scaling.z,      0.0 ],
            [       0.0,       0.0,       0.0,      1.0 ]
        )
        self = matrix
    }

    init( scaling: Float ) {

        self = matrix_identity_float4x4
        columns.3.w = 1 / scaling
    }
  
    // MARK: - Rotate
    init( rotationX angle: Float ) {

        let matrix = float4x4(
            [       1.0,         0.0,        0.0,        0.0 ],
            [       0.0,  cos(angle), sin(angle),        0.0 ],
            [       0.0, -sin(angle), cos(angle),        0.0 ],
            [       0.0,         0.0,        0.0,        1.0 ]
        )
        self = matrix
    }
  
    init( rotationY angle: Float ) {

        let matrix = float4x4(
            [ cos(angle),       0.0,-1.0*sin(angle),       0.0 ],
            [        0.0,       1.0,            0.0,       0.0 ],
            [ sin(angle),       0.0,     cos(angle),       0.0 ],
            [        0.0,       0.0,            0.0,       1.0 ]
        )
        self = matrix
    }
  
    init( rotationZ angle: Float ) {

        let matrix = float4x4(
            [     cos(angle),   sin(angle),        0.0,        0.0 ],
            [-1.0*sin(angle),   cos(angle),        0.0,        0.0 ],
            [            0.0,          0.0,        1.0,        0.0 ],
            [            0.0,          0.0,        0.0,        1.0 ]
        )
        self = matrix
    }
  
    init( rotation angle: SIMD3<Float> ) {

        let rotationX = float4x4( rotationX: angle.x )
        let rotationY = float4x4( rotationY: angle.y )
        let rotationZ = float4x4( rotationZ: angle.z )

        self = rotationX * rotationY * rotationZ
    }
  
    init( rotationYXZ angle: SIMD3<Float> ) {

        let rotationX = float4x4( rotationX: angle.x )
        let rotationY = float4x4( rotationY: angle.y )
        let rotationZ = float4x4( rotationZ: angle.z )

        self = rotationY * rotationX * rotationZ
    }
  
    // MARK: - Identity
    static func identity() -> float4x4 {
        matrix_identity_float4x4
    }
  
    // MARK: - Upper left 3x3
    var upperLeft: float3x3 {

        let x = columns.0.xyz
        let y = columns.1.xyz
        let z = columns.2.xyz

        return float3x3(columns: (x, y, z))
    }
  
    // MARK: - Left handed projection matrix
    init( projectionFov fov: Float, near: Float, far: Float, aspect: Float, lhs: Bool = true ) {

        let y = 1 / tan(fov * 0.5)
        let x = y / aspect
        let z = lhs ? far / (far - near) : far / (near - far)
        let X = SIMD4<Float>(   x,    0.0,  0.0,  0.0)
        let Y = SIMD4<Float>( 0.0,      y,  0.0,  0.0)
        let Z = lhs ? SIMD4<Float>( 0.0,  0.0,            z,  1.0 ) : SIMD4<Float>( 0.0,  0.0,         z, -1.0 )
        let W = lhs ? SIMD4<Float>( 0.0,  0.0,  -1.0*near*z,  0.0 ) : SIMD4<Float>( 0.0,  0.0,  z * near,  0.0 )

        self.init()
        columns = ( X, Y, Z, W )
    }
  
    // MARK: - left-handed LookAt
    init( eye: SIMD3<Float>, center: SIMD3<Float>, up: SIMD3<Float> ) {

        let z = normalize( center - eye )
        let x = normalize( cross(up, z) )
        let y = cross( z, x )
    
        let X = SIMD4<Float>( x.x, y.x, z.x, 0.0 )
        let Y = SIMD4<Float>( x.y, y.y, z.y, 0.0 )
        let Z = SIMD4<Float>( x.z, y.z, z.z, 0.0 )
        let W = SIMD4<Float>( -1.0 * dot(x, eye), -1.0 * dot(y, eye), -1.0 * dot(z, eye), 1.0 )
    
        self.init()
        columns = (X, Y, Z, W)
    }
  
    // MARK: - Orthographic matrix
    init(
        orthoLeft left: Float,
        right:  Float,
        bottom: Float,
        top:    Float,
        near:   Float,
        far:    Float
    ) {
        let X = SIMD4<Float>( 2.0 / (right - left), 0.0, 0.0, 0.0 )
        let Y = SIMD4<Float>( 0.0, 2.0 / (top - bottom), 0.0, 0.0 )
        let Z = SIMD4<Float>( 0.0, 0.0, 1.0 / (far - near), 0.0 )
        let W = SIMD4<Float>(
                    (left + right) / (left - right),
                    (top + bottom) / (bottom - top),
                    near / (near - far),
                    1.0
                )
        self.init()

        columns = ( X, Y, Z, W )
    }
  
    // MARK: - convert double4x4 to float4x4
    init( _ m: matrix_double4x4 ) {

        self.init()
        let matrix: float4x4 = float4x4( SIMD4<Float>( m.columns.0 ),
                                         SIMD4<Float>( m.columns.1 ),
                                         SIMD4<Float>( m.columns.2 ),
                                         SIMD4<Float>( m.columns.3 )
                                       )
        self = matrix
    }
}

// MARK: - float3x3
extension float3x3 {

    init( normalFrom4x4 matrix: float4x4 ) {
        self.init()
        columns = matrix.upperLeft.inverse.transpose.columns
    }
}

// MARK: - float4

extension SIMD4 where Scalar == Float {

    var xyz: SIMD3<Float> {
        get {
            SIMD3<Float>( x, y, z )
        }
        set {
            x = newValue.x
            y = newValue.y
            z = newValue.z
        }
    }
  
    // convert from double4
    init( _ d: SIMD4<Double> ) {
        self.init()
        self = [Float(d.x), Float(d.y), Float(d.z), Float(d.w)]
    }

    // convert from simd3
    init( _ v: SIMD3<Float>, w: Float ) {
        self.init()
        self = [ v.x, v.y, v.z, w ]
    }
}

