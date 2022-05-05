import Foundation
import MetalKit

class StructureOfArraysForMDLMesh {

    public var vertices:            [ SIMD3<Float> ]?
    public var normals:             [ SIMD3<Float> ]?
    public var tangents:            [ SIMD3<Float> ]?
    public var bitangents:          [ SIMD3<Float> ]?
    public var textureCoordinates:  [ SIMD2<Float> ]?
    public var colors:              [ SIMD3<Float> ]?
    public var arrayIndices:        [ Int32 ] = []
    public var numVertices:         Int = 0

    init(
        useVertices:           Bool,
        useNormals:            Bool,
        useTangents:           Bool,
        useBitangents:         Bool,
        useTextureCoordinates: Bool,
        useColors:             Bool
    ) {
        if useVertices {
            vertices = []
        }
        if useNormals {
            normals = []
        }
        if useTangents {
            tangents = []
        }
        if useBitangents {
            bitangents = []
        }
        if useTextureCoordinates {
            textureCoordinates = []
        }
        if useColors {
            colors = []
        }
    }

    func updateNumVertices() {

        numVertices = 0
        if let vertices = vertices {
            numVertices = max( numVertices, vertices.count )
        }
        if let normals = normals {
            numVertices = max( numVertices, normals.count )
        }
        if let textureCoordinates = textureCoordinates {
            numVertices = max( numVertices, textureCoordinates.count )
        }
        if let colors = colors {
            numVertices = max( numVertices, colors.count )
        }
        if let tangents = tangents {
            numVertices = max( numVertices, tangents.count )
        }
        if let bitangents = bitangents {
            numVertices = max( numVertices, bitangents.count )
        }
    }


    func generateInterleavedArrays(

       vertexDescriptor: MDLVertexDescriptor,
       floatArrays:      inout [[Float]]
    ) {
        floatArrays = []

        var numFloatsPerVertex : [Int]     = []

        updateNumVertices()

        StructureOfArraysForMDLMesh.allocateFloatArraysToBeCopiedToMTLBuffers(
            vertexDescriptor   : vertexDescriptor,
            floatArrays        : &floatArrays,
            numFloatsPerVertex : &numFloatsPerVertex,
            numVertices        : numVertices
        )

        let extractedAttributes = ExtractedVertexAttributes( vertexDescriptor: vertexDescriptor )

        if extractedAttributes.point.hasAttribute {

            if normals != nil {
                StructureOfArraysForMDLMesh.populateArrayOneAttribute(

                    linearInputArray3:  vertices!,
                    floatArrays:        &floatArrays,
                    numFloatsPerVertex: numFloatsPerVertex,
                    attributes:         extractedAttributes.point
                )
            } else {
                StructureOfArraysForMDLMesh.populateArrayOneAttribute(

                    defaultValue3:      SIMD3<Float>(0,0,0),
                    numVertices:        numVertices,
                    floatArrays:        &floatArrays,
                    numFloatsPerVertex: numFloatsPerVertex,
                    attributes:         extractedAttributes.point
                )
            }
        }

        if extractedAttributes.normal.hasAttribute {
            if normals != nil {
                StructureOfArraysForMDLMesh.populateArrayOneAttribute(

                    linearInputArray3:  normals!,
                    floatArrays:        &floatArrays,
                    numFloatsPerVertex: numFloatsPerVertex,
                    attributes:         extractedAttributes.normal
                )
            } else {
                StructureOfArraysForMDLMesh.populateArrayOneAttribute(

                    defaultValue3:      SIMD3<Float>(0,0,0),
                    numVertices:        numVertices,
                    floatArrays:        &floatArrays,
                    numFloatsPerVertex: numFloatsPerVertex,
                    attributes:         extractedAttributes.normal
                )
            }
        }

        if extractedAttributes.tangent.hasAttribute {
            if tangents != nil {
                StructureOfArraysForMDLMesh.populateArrayOneAttribute(

                    linearInputArray3:  tangents!,
                    floatArrays:        &floatArrays,
                    numFloatsPerVertex: numFloatsPerVertex,
                    attributes:         extractedAttributes.tangent
                )
            } else {
                StructureOfArraysForMDLMesh.populateArrayOneAttribute(

                    defaultValue3:      SIMD3<Float>(0,0,0),
                    numVertices:        numVertices,
                    floatArrays:        &floatArrays,
                    numFloatsPerVertex: numFloatsPerVertex,
                    attributes:         extractedAttributes.tangent
                )
            }
        }

        if extractedAttributes.bitangent.hasAttribute {
            if bitangents != nil {
                StructureOfArraysForMDLMesh.populateArrayOneAttribute(

                    linearInputArray3:  bitangents!,
                    floatArrays:        &floatArrays,
                    numFloatsPerVertex: numFloatsPerVertex,
                    attributes:         extractedAttributes.bitangent
                )
            } else {
                StructureOfArraysForMDLMesh.populateArrayOneAttribute(

                    defaultValue3:      SIMD3<Float>(0,0,0),
                    numVertices:        numVertices,
                    floatArrays:        &floatArrays,
                    numFloatsPerVertex: numFloatsPerVertex,
                    attributes:         extractedAttributes.tangent
                )
            }
        }

        if extractedAttributes.textureCoordinate.hasAttribute {
            if textureCoordinates != nil {
                StructureOfArraysForMDLMesh.populateArrayOneAttribute(

                    linearInputArray2:  textureCoordinates!,
                    floatArrays:        &floatArrays,
                    numFloatsPerVertex: numFloatsPerVertex,
                    attributes:         extractedAttributes.textureCoordinate
                )
            } else {
                StructureOfArraysForMDLMesh.populateArrayOneAttribute(

                    defaultValue2:      SIMD2<Float>(0,0),
                    numVertices:        numVertices,
                    floatArrays:        &floatArrays,
                    numFloatsPerVertex: numFloatsPerVertex,
                    attributes:         extractedAttributes.tangent
                )
            }
        }

        if extractedAttributes.color.hasAttribute {
            if colors != nil {

                StructureOfArraysForMDLMesh.populateArrayOneAttribute(

                    linearInputArray3:  colors!,
                    floatArrays:        &floatArrays,
                    numFloatsPerVertex: numFloatsPerVertex,
                    attributes:         extractedAttributes.color
                )
            } else {
                StructureOfArraysForMDLMesh.populateArrayOneAttribute(

                    defaultValue3:      SIMD3<Float>(0,0,0),
                    numVertices:        numVertices,
                    floatArrays:        &floatArrays,
                    numFloatsPerVertex: numFloatsPerVertex,
                    attributes:         extractedAttributes.tangent
                )
            
            }
        }
    }

    
    static func allocateFloatArraysToBeCopiedToMTLBuffers(

        vertexDescriptor   : MDLVertexDescriptor,
        floatArrays        : inout [[Float]],
        numFloatsPerVertex : inout [Int],
        numVertices        : Int

    ) {
        let layouts = vertexDescriptor.layouts as! [MDLVertexBufferLayout]

        for layout in layouts {

            floatArrays.append([])
            numFloatsPerVertex.append( layout.stride / MemoryLayout<Float>.stride )
        }
        
        for i in 0 ..< layouts.count {
            for _ in 0 ..< numFloatsPerVertex[i] * numVertices {
                floatArrays[i].append(0.0)
            }
        }
    }
    
    static func assignOneAttributeToArray(

        floatArrays:        inout [[Float]],
        numFloats:          [Int],
        vertexIndex:        Int,
        extractedAttribute: ExtractedVertexAttribute,
        vector4:            SIMD4<Float>

    ) -> Void {

        let layoutIndex = extractedAttribute.bufferIndex
        let offset      = extractedAttribute.floatOffset
        let baseIndex   = vertexIndex * numFloats[ layoutIndex ] + offset

        floatArrays[ layoutIndex ][ baseIndex     ] = vector4.x
        floatArrays[ layoutIndex ][ baseIndex + 1 ] = vector4.y
        floatArrays[ layoutIndex ][ baseIndex + 2 ] = vector4.z
        if extractedAttribute.format == MDLVertexFormat.float4 {
            floatArrays[ layoutIndex ][ baseIndex + 3 ] = vector4.w
        }
    }

    static func assignOneAttributeToArray(

        floatArrays:        inout [[Float]],
        numFloats:          [Int],
        vertexIndex:        Int,
        extractedAttribute: ExtractedVertexAttribute,
        vector3:            SIMD3<Float>

    ) -> Void {

        let layoutIndex = extractedAttribute.bufferIndex
        let offset      = extractedAttribute.floatOffset
        let baseIndex   = vertexIndex * numFloats[ layoutIndex ] + offset

        floatArrays[ layoutIndex ][ baseIndex     ] = vector3.x
        floatArrays[ layoutIndex ][ baseIndex + 1 ] = vector3.y
        floatArrays[ layoutIndex ][ baseIndex + 2 ] = vector3.z
        if extractedAttribute.format == MDLVertexFormat.float4 {
            floatArrays[ layoutIndex ][ baseIndex + 3 ] = 1.0
        }
    }

    static func assignOneAttributeToArray(

        floatArrays:        inout [[Float]],
        numFloats:          [Int],
        vertexIndex:        Int,
        extractedAttribute: ExtractedVertexAttribute,
        vector2:            SIMD2<Float>

    ) -> Void {

        let layoutIndex = extractedAttribute.bufferIndex
        let offset      = extractedAttribute.floatOffset
        let baseIndex   = vertexIndex * numFloats[ layoutIndex ] + offset

        floatArrays[ layoutIndex ][ baseIndex     ] = vector2.x
        floatArrays[ layoutIndex ][ baseIndex + 1 ] = vector2.y
    }

    static func populateArrayOneAttribute(

        linearInputArray4:  [SIMD4<Float>],
        floatArrays:        inout [[Float]],
        numFloatsPerVertex: [Int],
        attributes:         ExtractedVertexAttribute

    ) {
        for ( vertexIndex, vec4 ) in linearInputArray4.enumerated() {

            assignOneAttributeToArray(
                floatArrays:        &floatArrays,
                numFloats:          numFloatsPerVertex,
                vertexIndex:        vertexIndex,
                extractedAttribute: attributes,
                vector4:            vec4
            )
        }
    }

    static func populateArrayOneAttribute(

        linearInputArray3:  [SIMD3<Float>],
        floatArrays:        inout [[Float]],
        numFloatsPerVertex: [Int],
        attributes:         ExtractedVertexAttribute

    ) {
        for ( vertexIndex, vec3 ) in linearInputArray3.enumerated() {

            assignOneAttributeToArray(
                floatArrays:        &floatArrays,
                numFloats:          numFloatsPerVertex,
                vertexIndex:        vertexIndex,
                extractedAttribute: attributes,
                vector3:            vec3
            )
        }
    }

    static func populateArrayOneAttribute(

        linearInputArray2:  [SIMD2<Float>],
        floatArrays:        inout [[Float]],
        numFloatsPerVertex: [Int],
        attributes:         ExtractedVertexAttribute

    ) {
        for ( vertexIndex, vec2 ) in linearInputArray2.enumerated() {

            assignOneAttributeToArray(
                floatArrays:        &floatArrays,
                numFloats:          numFloatsPerVertex,
                vertexIndex:        vertexIndex,
                extractedAttribute: attributes,
                vector2:            vec2
            )
        }
    }

    static func populateArrayOneAttribute(

        defaultValue4:      SIMD4<Float>,
        numVertices:        Int,
        floatArrays:        inout [[Float]],
        numFloatsPerVertex: [Int],
        attributes:         ExtractedVertexAttribute
    ) {
        for vertexIndex in 0 ..< numVertices {

            assignOneAttributeToArray(
                floatArrays:        &floatArrays,
                numFloats:          numFloatsPerVertex,
                vertexIndex:        vertexIndex,
                extractedAttribute: attributes,
                vector4:            defaultValue4
            )
        }
    
    }

    static func populateArrayOneAttribute(

        defaultValue3:      SIMD3<Float>,
        numVertices:        Int,
        floatArrays:        inout [[Float]],
        numFloatsPerVertex: [Int],
        attributes:         ExtractedVertexAttribute
    ) {
        for vertexIndex in 0 ..< numVertices {

            assignOneAttributeToArray(
                floatArrays:        &floatArrays,
                numFloats:          numFloatsPerVertex,
                vertexIndex:        vertexIndex,
                extractedAttribute: attributes,
                vector3:            defaultValue3
            )
        }
    
    }

    static func populateArrayOneAttribute(

        defaultValue2:      SIMD2<Float>,
        numVertices:        Int,
        floatArrays:        inout [[Float]],
        numFloatsPerVertex: [Int],
        attributes:         ExtractedVertexAttribute
    ) {
        for vertexIndex in 0 ..< numVertices {

            assignOneAttributeToArray(
                floatArrays:        &floatArrays,
                numFloats:          numFloatsPerVertex,
                vertexIndex:        vertexIndex,
                extractedAttribute: attributes,
                vector2:            defaultValue2
            )
        }
    
    }

}
