//
//  StructsSharedWithShaders.swift
//  Voxell
//
//  Created by Shoichiro Yamanishi on 01.05.22.
//

import Foundation
import MetalKit

// MARK: **Conversion Rules**
//
// | Metal              | Swift              | Size | Alignment |
// | ------------------ | ------------------ | ----:| ---------:|
// | bool               |                    |    1 |         1 |
// | char               |                    |    1 |         1 |
// | short              |                    |    2 |         2 |
// | enum               | enum : Int32       |    4 |         4 |
// | int                | Int32              |    4 |         4 |
// | float              | Float              |    4 |         4 |
// | ------------------ | ------------------ | ----:| ---------:|
// | float2             | SIMD2<Float>       |    8 |         8 |
// | float3             | SIMD4<Float>       |   16 |        16 | <= NOT SIMD3<Float>
// | float4             | SIMD4<Float>       |   16 |        16 |
// | packed_float2      |                    |    8 |         4 |
// | packed_float3      |                    |   12 |         4 |
// | packed_float4      |                    |   16 |         4 |
// | ------------------ | ------------------ | ----:| ---------:|
// | float3x3           | float3x3           |   48 |        16 |
// | float4x4           | float4x4           |   64 |        16 |

// MARK: Indices
public enum FunctionConstantIndex : Int {

    case VertexBufferHasNormals            = 0
    case VertexBufferHasTangents           = 1
    case VertexBufferHasBitangents         = 2
    case VertexBufferHasTextureCoordinates = 3
    case VertexBufferHasColors             = 4
  
    case TextureBaseColor                  = 5
    case TextureTangentSpaceNormal         = 6
    case TextureRoughness                  = 7
}

public enum TextureIndex : Int {

    case BaseColor          = 0
    case TangentSpaceNormal = 1
    case Roughness          = 2
}

public enum BufferIndex : Int {

    // 0-9 reserved for vertex buffers (stage_in)

    case SceneUniformVertex       = 10
    case PerInstanceUniformVertex = 11
    case SceneUniformFragment     = 12
    case LightsFragment           = 13
    case MaterialFragment         = 14
}

// MARK: Structures for Vertex Shaders

public struct UniformsPerSceneSharedWithVertexShaders {

    var viewMatrix:       float4x4
    var projectionMatrix: float4x4

    public init( viewMatrix: float4x4, projectionMatrix: float4x4 ) {
        self.viewMatrix       = viewMatrix
        self.projectionMatrix = projectionMatrix
    }

    public init() {
        viewMatrix       = matrix_identity_float4x4
        projectionMatrix = matrix_identity_float4x4
    }
}

public struct UniformsPerInstanceSharedWithVertexShaders {

    public var modelMatrix:  matrix_float4x4
    public var normalMatrix: matrix_float3x3

    public init ( modelMatrix: matrix_float4x4, normalMatrix: matrix_float3x3 ) {
        self.modelMatrix  = modelMatrix
        self.normalMatrix = normalMatrix
    }

    public init () {
        modelMatrix  = matrix_identity_float4x4
        normalMatrix = matrix_identity_float3x3
    }
}

// MARK: Structures for Fragment Shaders

public struct UniformsPerSceneSharedWithFragmentShaders {

    var cameraPosition: SIMD3<Float>
    var lightCount:     UInt32
}

public enum LightType : UInt32 {

    case Unused       = 0
    case Sunlight     = 1
    case Spotlight    = 2
    case Pointlight   = 3
    case Ambientlight = 4
}

public struct LightSharedWithFragmentShaders {


    var position:        SIMD4<Float>
    var color:           SIMD4<Float>
    var specularColor:   SIMD4<Float>
    var intensity:       Float
    var attenuation:     SIMD4<Float>
    var lightType:       UInt32 /* LightType */
    var coneAngle:       Float
    var coneDirection:   SIMD4<Float>
    var coneAttenuation: Float
    
    init() {

        position        = SIMD4<Float>( 0.0, 0.0, 0.0, 1.0 )
        color           = SIMD4<Float>( 1.0, 1.0, 1.0, 1.0 )
        specularColor   = SIMD4<Float>( 1.0, 1.0, 1.0, 1.0 )
        intensity       = 0.4
        attenuation     = SIMD4<Float>( 1.0, 0.0, 0.0, 1.0 )
        lightType       = LightType.Sunlight.rawValue
        coneAngle       = 0.0
        coneDirection   = SIMD4<Float>( 0.0, 0.0, 0.0, 1.0 )
        coneAttenuation = 0.0
    }
}

public struct MaterialFragmentBytesSharedWithFragmentShaders {

    var baseColor:        SIMD4<Float>
    var specular:         SIMD4<Float>
    var roughness:        SIMD4<Float>
    var specularExponent: Float
    var ambientOcclusion: SIMD4<Float>
    var metallic:         Float
    var opacity:          Float

    public init ( mdlMaterial : MDLMaterial ) {

        baseColor        = SIMD4<Float>( mdlMaterial.baseColor( ifNotFound: SIMD3<Float>( 0.8, 0.8, 0.8 ) ), w: 1.0 )
        specular         = SIMD4<Float>( mdlMaterial.specular ( ifNotFound: SIMD3<Float>( 0.8, 0.8, 0.8 ) ), w: 1.0 )
        roughness        = SIMD4<Float>( mdlMaterial.roughness( ifNotFound: SIMD3<Float>( 0.0, 0.0, 0.0 ) ), w: 1.0 )
        specularExponent = mdlMaterial.specularExponent( ifNotFound: 0.0 )
        ambientOcclusion = SIMD4<Float>( 0.0, 0.0, 0.0, 1.0 )
        metallic         = 0.0
        opacity          = mdlMaterial.opacity( ifNotFound: 0.0 )
    }
}
