import Foundation
import MetalKit

class RenderUtil {

    static func defaultVertexDescriptorPositionNormalTextureCoordinateTangentBitangent() -> MDLVertexDescriptor {
  
        let layout0 = MDLVertexBufferLayout()
        layout0.stride =   MemoryLayout<SIMD3<Float>>.stride
                         + MemoryLayout<SIMD3<Float>>.stride
                         + MemoryLayout<SIMD2<Float>>.stride
                         + MemoryLayout<SIMD3<Float>>.stride
                         + MemoryLayout<SIMD3<Float>>.stride

        let attribute0         = MDLVertexAttribute()
        attribute0.name        = MDLVertexAttributePosition
        attribute0.format      = MDLVertexFormat.float3
        attribute0.bufferIndex = 0
        attribute0.offset      = 0

        let attribute1         = MDLVertexAttribute()
        attribute1.name        = MDLVertexAttributeNormal
        attribute1.format      = MDLVertexFormat.float3
        attribute1.bufferIndex = 0
        attribute1.offset      = MemoryLayout<SIMD3<Float>>.stride

        let attribute2         = MDLVertexAttribute()
        attribute2.name        = MDLVertexAttributeTextureCoordinate
        attribute2.format      = MDLVertexFormat.float2
        attribute2.bufferIndex = 0
        attribute2.offset      = MemoryLayout<SIMD3<Float>>.stride + MemoryLayout<SIMD3<Float>>.stride

        let attribute3         = MDLVertexAttribute()
        attribute3.name        = MDLVertexAttributeTangent
        attribute3.format      = MDLVertexFormat.float3
        attribute3.bufferIndex = 0
        attribute3.offset      =   MemoryLayout<SIMD3<Float>>.stride
                                 + MemoryLayout<SIMD3<Float>>.stride
                                 + MemoryLayout<SIMD2<Float>>.stride

        let attribute4         = MDLVertexAttribute()
        attribute4.name        = MDLVertexAttributeBitangent
        attribute4.format      = MDLVertexFormat.float3
        attribute4.bufferIndex = 0
        attribute4.offset      =   MemoryLayout<SIMD3<Float>>.stride
                                 + MemoryLayout<SIMD3<Float>>.stride
                                 + MemoryLayout<SIMD2<Float>>.stride
                                 + MemoryLayout<SIMD3<Float>>.stride

        let vertexDescriptor = MDLVertexDescriptor()
        vertexDescriptor.layouts    = [ layout0 ]
        vertexDescriptor.attributes = [ attribute0, attribute1, attribute2, attribute3, attribute4 ]

        return vertexDescriptor
    }

    static func defaultVertexDescriptorPositionNormalTextureCoordinate() -> MDLVertexDescriptor {
  
        let layout0 = MDLVertexBufferLayout()
        layout0.stride =   MemoryLayout<SIMD3<Float>>.stride
                         + MemoryLayout<SIMD3<Float>>.stride
                         + MemoryLayout<SIMD2<Float>>.stride

        let attribute0         = MDLVertexAttribute()
        attribute0.name        = MDLVertexAttributePosition
        attribute0.format      = MDLVertexFormat.float3
        attribute0.bufferIndex = 0
        attribute0.offset      = 0

        let attribute1         = MDLVertexAttribute()
        attribute1.name        = MDLVertexAttributeNormal
        attribute1.format      = MDLVertexFormat.float3
        attribute1.bufferIndex = 0
        attribute1.offset      = MemoryLayout<SIMD3<Float>>.stride

        let attribute2         = MDLVertexAttribute()
        attribute2.name        = MDLVertexAttributeTextureCoordinate
        attribute2.format      = MDLVertexFormat.float2
        attribute2.bufferIndex = 0
        attribute2.offset      = MemoryLayout<SIMD3<Float>>.stride + MemoryLayout<SIMD3<Float>>.stride

        let vertexDescriptor = MDLVertexDescriptor()
        vertexDescriptor.layouts    = [ layout0 ]
        vertexDescriptor.attributes = [ attribute0, attribute1, attribute2 ]

        return vertexDescriptor
    }

    static func defaultVertexDescriptorPositionNormalColor() -> MDLVertexDescriptor {
  
        let layout0 = MDLVertexBufferLayout()
        layout0.stride =   MemoryLayout<SIMD3<Float>>.stride
                         + MemoryLayout<SIMD3<Float>>.stride
                         + MemoryLayout<SIMD3<Float>>.stride

        let attribute0         = MDLVertexAttribute()
        attribute0.name        = MDLVertexAttributePosition
        attribute0.format      = MDLVertexFormat.float3
        attribute0.bufferIndex = 0
        attribute0.offset      = 0

        let attribute1         = MDLVertexAttribute()
        attribute1.name        = MDLVertexAttributeNormal
        attribute1.format      = MDLVertexFormat.float3
        attribute1.bufferIndex = 0
        attribute1.offset      = MemoryLayout<SIMD3<Float>>.stride

        let attribute2         = MDLVertexAttribute()
        attribute2.name        = MDLVertexAttributeColor
        attribute2.format      = MDLVertexFormat.float3
        attribute2.bufferIndex = 0
        attribute2.offset      = MemoryLayout<SIMD3<Float>>.stride + MemoryLayout<SIMD3<Float>>.stride

        let vertexDescriptor = MDLVertexDescriptor()
        vertexDescriptor.layouts    = [ layout0 ]
        vertexDescriptor.attributes = [ attribute0, attribute1, attribute2 ]

        return vertexDescriptor
    }

    static func defaultVertexDescriptorPositionColor() -> MDLVertexDescriptor {
  
        let layout0 = MDLVertexBufferLayout()
        layout0.stride =   MemoryLayout<SIMD3<Float>>.stride
                         + MemoryLayout<SIMD3<Float>>.stride

        let attribute0         = MDLVertexAttribute()
        attribute0.name        = MDLVertexAttributePosition
        attribute0.format      = MDLVertexFormat.float3
        attribute0.bufferIndex = 0
        attribute0.offset      = 0

        let attribute1         = MDLVertexAttribute()
        attribute1.name        = MDLVertexAttributeColor
        attribute1.format      = MDLVertexFormat.float3
        attribute1.bufferIndex = 0
        attribute1.offset      = MemoryLayout<SIMD3<Float>>.stride

        let vertexDescriptor = MDLVertexDescriptor()
        vertexDescriptor.layouts    = [ layout0 ]
        vertexDescriptor.attributes = [ attribute0, attribute1 ]

        return vertexDescriptor
    }

    static func defaultMaterial( color: SIMD3<Float> ) -> MDLMaterial {

        let scatteringFunction = MDLScatteringFunction()
        let material = MDLMaterial( name: "Brep", scatteringFunction: scatteringFunction )

        material.setProperty( MDLMaterialProperty(
                                  name:     "baseColor",
                                  semantic: .baseColor,
                                  float3:   color
                            ) )

        material.setProperty( MDLMaterialProperty(
                                  name:     "specular",
                                  semantic: .specular,
                                  float3:   vector_float3( 0.8, 0.8 ,0.8 )
                            ) )

        material.setProperty( MDLMaterialProperty(
                                  name:     "specularComponent",
                                  semantic: .specularExponent ,
                                  float:    255.0
                            ) )

        material.setProperty( MDLMaterialProperty(
                                  name:     "opacity",
                                  semantic: .opacity ,
                                  float:    0.2
                            ) )
        return material
    }
}
