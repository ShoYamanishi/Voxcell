import Foundation
import MetalKit

extension MDLVertexDescriptor {

    func hasAttribute(
        name:             String,
        ofTypes:          [MDLVertexFormat]

    ) -> Bool {

        guard let attribute = attributeNamed( name )
        else {
            return false
        }

        return ofTypes.reduce( false ) { ret, e in
            if e == attribute.format {
                return true
            }
            return ret
        }
    }

    func hasNormals() -> Bool {
        return hasAttribute(
            name:    MDLVertexAttributeNormal,
            ofTypes: [ MDLVertexFormat.float3, MDLVertexFormat.float4 ]
        )
    }

    func hasTangents() -> Bool {
        return hasAttribute(
            name:    MDLVertexAttributeTangent,
            ofTypes: [ MDLVertexFormat.float3, MDLVertexFormat.float4 ]
        )
    }

    func hasBitangents() -> Bool {
        return hasAttribute(
            name:    MDLVertexAttributeBitangent,
            ofTypes: [ MDLVertexFormat.float3, MDLVertexFormat.float4 ]
        )
    }

    func hasTextureCoordinates() -> Bool {
        return hasAttribute(
            name:    MDLVertexAttributeTextureCoordinate,
            ofTypes: [ MDLVertexFormat.float2 ]
        )
    }

    func hasColors() -> Bool {
        return hasAttribute(
            name:    MDLVertexAttributeColor,
            ofTypes: [ MDLVertexFormat.float3, MDLVertexFormat.float4 ]
        )
    }
}
