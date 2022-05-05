import Foundation
import MetalKit

struct ExtractedVertexAttribute {
    let hasAttribute: Bool
    let bufferIndex : Int
    let floatOffset : Int
    let format      : MDLVertexFormat
}

struct ExtractedVertexAttributes {

    let point             : ExtractedVertexAttribute
    let normal            : ExtractedVertexAttribute
    let tangent           : ExtractedVertexAttribute
    let bitangent         : ExtractedVertexAttribute
    let textureCoordinate : ExtractedVertexAttribute
    let color             : ExtractedVertexAttribute

    init (vertexDescriptor: MDLVertexDescriptor) {

        let attributes = vertexDescriptor.attributes as! [MDLVertexAttribute]

        point             = ExtractedVertexAttributes.extract( attributes: attributes, name: MDLVertexAttributePosition )
        normal            = ExtractedVertexAttributes.extract( attributes: attributes, name: MDLVertexAttributeNormal )
        tangent           = ExtractedVertexAttributes.extract( attributes: attributes, name: MDLVertexAttributeTangent )
        bitangent         = ExtractedVertexAttributes.extract( attributes: attributes, name: MDLVertexAttributeBitangent )
        textureCoordinate = ExtractedVertexAttributes.extract( attributes: attributes, name: MDLVertexAttributeTextureCoordinate )
        color             = ExtractedVertexAttributes.extract( attributes: attributes, name: MDLVertexAttributeColor )
    }
    
    static func extract( attributes : [MDLVertexAttribute], name: String ) -> ExtractedVertexAttribute {

        for attr in attributes {
            if attr.name == name {
                return ExtractedVertexAttribute(
                    hasAttribute: true,
                    bufferIndex:  attr.bufferIndex,
                    floatOffset:  attr.offset / MemoryLayout<Float>.stride,
                    format:       attr.format
                )
            }
        }
        return ExtractedVertexAttribute( hasAttribute: false, bufferIndex: 0, floatOffset: 0, format: MDLVertexFormat.invalid )
    }
}
