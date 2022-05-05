import Foundation
import MetalKit

extension Brep {

    /// Constructs a MDLMesh per MDLVertexDescriptor.
    ///
    /// - parameter device:  The Metal device used
    /// - parameter type:    geometry type. Any of `.triangles`, `.lines`, or `.points`
    /// - parameter vertexDescriptor: spec for the buffer in layouts and attributes. If  nil, the following default values are used.
    /// - parameter material: spec for the material properties. If nil, the following default values are used.
    /// - parameter color: RGBA for the mesh to be generated. If the vertex descriptor contains an attribute `MDLVertexAttributeColor`, then this value is used.
    ///
    /// **Acceptable values for MDLVertexDescriptor**
    ///
    /// *type == .triangles*
    /// - `MDLVertexAttributePosition` (`float3` or `float4`) if `float4` is specified `w` is set to 1.
    /// - `MDLVertexAttributeNormal` (`float3` or `float4`) if `float4` is specified `w` is set to 0.
    /// - `MDLVertexAttributeTextureCoordinate` (`float2`)
    /// - `MDLVertexAttributeTangent` (`float3` or `float4`) if `float4` is specified `w` is set to 0.
    /// - `MDLVertexAttributeBitangent` (`float3` or `float4`) if `float4` is specified `w` is set to 0.
    /// - `MDLVertexAttributeColor` (`float3` or `float4`) RGB or RGBA
    ///
    /// *type == .lines or .points*
    ///
    /// - `MDLVertexAttributePosition` (`float3` or `float4`) if `float4` is specified `w` is set to 1.
    /// - `MDLVertexAttributeColor` (`float3` or `float4`) RGB or RGBA
    /// - `MDLVertexAttributeTextureCoordinate` (`float2`)
    ///
    /// **The default MDLVertexDescriptor**
    /// ```
    /// layout[0].stride =   MemoryLayout<SIMD3<Float>>.stride // point
    ///                    + MemoryLayout<SIMD3<Float>>.stride // normal
    ///                    + MemoryLayout<SIMD4<Float>>.stride // color
    ///
    /// attribute[0].name        = MDLVertexAttributePosition
    /// attribute[0].format      = MDLVertexFormat.float3
    /// attribute[0].bufferIndex = 0
    /// attribute[0].offset      = 0
    ///
    /// attribute[1].name        = MDLVertexAttributeNormal
    /// attribute[1].format      = MDLVertexFormat.float3
    /// attribute[1].bufferIndex = 1
    /// attribute[1.]offset      = MemoryLayout<SIMD3<Float>>.stride
    ///
    /// attribute[2].name        = MDLVertexAttributeColor
    /// attribute[2].format      = MDLVertexFormat.float4
    /// attribute[2].bufferIndex = 1
    /// attribute[2.]offset      = MemoryLayout<SIMD3<Float>>.stride + MemoryLayout<SIMD3<Float>>.stride
    /// ```
    ///
    /// **The default MDLMaterial**
    /// ```
    /// scatteringFunction: MDLScatteringFunction()
    /// name: 'Brep'
    /// MDLMaterialProperty( name: "baseColor",         semantic: .baseColor,        float3: color or vector_float3(0.5, 0.5 ,0.5) )
    /// MDLMaterialProperty( name: "specular",          semantic: .specular,         float3: vector_float3(0.8, 0.8 ,0.8) )
    /// MDLMaterialProperty( name: "specularComponent", semantic: .specularExponent, float:  255.0) )
    /// MDLMaterialProperty( name: "opacity",           semantic: .opacity,          float: 0.2) )
    /// ```
    ///
    /// - returns: A MDLMesh of this Brep.
    ///
    public func generateMDLMesh(

        device:           MTLDevice,
        type:             MDLGeometryType,
        vertexDescriptor: MDLVertexDescriptor?,
        material:         MDLMaterial?,
        color:            SIMD3<Float>?
        
    ) -> MDLMesh? {

        var vertexDescriptorCleaned : MDLVertexDescriptor
        var materialCleaned         : MDLMaterial
        var colorCleaned            : SIMD3<Float>

        (vertexDescriptorCleaned, materialCleaned, colorCleaned)
            = Brep.checkForSanityAndArrangeInputsForMDLMesh(
                            type:             type,
                            vertexDescriptor: vertexDescriptor,
                            material:         material,
                            color:            color
              )

        let structureOfArrays = populateStructureOfArraysToBeCopiedToMTLBuffers(
            vertexDescriptor:    vertexDescriptorCleaned,
            color:               colorCleaned
        )

        let mdlMesh = MDLMesh.generateFromStructureOfArrays(
            device:              device,
            vertexDescriptor:    vertexDescriptorCleaned,
            material:            materialCleaned,
            structureOfArrays:      structureOfArrays
        )

        return mdlMesh
    }

    func populateStructureOfArraysToBeCopiedToMTLBuffers(
        vertexDescriptor:    MDLVertexDescriptor,

        color:               SIMD3<Float>
    ) -> StructureOfArraysForMDLMesh {

        let extractedAttributes = ExtractedVertexAttributes( vertexDescriptor: vertexDescriptor )

        let structureOfArrays = StructureOfArraysForMDLMesh(

            useVertices:           extractedAttributes.point.hasAttribute,
            useNormals:            extractedAttributes.normal.hasAttribute,
            useTangents:           extractedAttributes.tangent.hasAttribute,
            useBitangents:         extractedAttributes.bitangent.hasAttribute,
            useTextureCoordinates: extractedAttributes.textureCoordinate.hasAttribute,
            useColors:             extractedAttributes.color.hasAttribute
        )

        var baseIndex : Int = 0

        for fr in faces {

            let f = fr as! Face
            var vertexIndex : Int = 0

            for her in f.halfEdges {

                let he = her as! HalfEdge
                let p = he.src!

                if extractedAttributes.point.hasAttribute {
                    structureOfArrays.vertices!.append(p.point)
                }

                if extractedAttributes.normal.hasAttribute {
                    structureOfArrays.normals!.append(f.normal)
                }

                if extractedAttributes.tangent.hasAttribute {
                    structureOfArrays.tangents!.append(f.tangent)
                }

                if extractedAttributes.bitangent.hasAttribute {
                    structureOfArrays.bitangents!.append(f.bitangent)
                }

                if extractedAttributes.color.hasAttribute {
                    structureOfArrays.colors!.append(color)
                }
                vertexIndex += 1
            }

            let numVerticesPerFace = vertexIndex

            if extractedAttributes.textureCoordinate.hasAttribute {
                let hasTextureUVs : Bool =    ( f.textureCoordinates != nil )
                                           && ( f.textureCoordinates!.count == vertexIndex )

                let dummyUV = SIMD2<Float>(x: 0.0, y: 0.0)

                for i in 0 ..< numVerticesPerFace {

                   structureOfArrays.textureCoordinates!.append(hasTextureUVs ? f.textureCoordinates![i] : dummyUV)
                }
            }

            for i in 2 ..< numVerticesPerFace {
            
                structureOfArrays.arrayIndices.append( Int32(baseIndex) )
                structureOfArrays.arrayIndices.append( Int32(baseIndex) + Int32( i - 1 ) )
                structureOfArrays.arrayIndices.append( Int32(baseIndex) + Int32( i ) )
            }

            baseIndex += numVerticesPerFace
        }
        structureOfArrays.numVertices = baseIndex
        
        return structureOfArrays
    }

    static func checkForSanityAndArrangeInputsForMDLMesh(

        type:             MDLGeometryType,
        vertexDescriptor: MDLVertexDescriptor?,
        material:         MDLMaterial?,
        color:            SIMD3<Float>?

    ) -> ( MDLVertexDescriptor,MDLMaterial,SIMD3<Float> )
    {
        var vertexDescriptorCleaned  : MDLVertexDescriptor
        var materialCleaned          : MDLMaterial
        var colorCleaned             : SIMD3<Float>
    
        if vertexDescriptor == nil {
            vertexDescriptorCleaned = RenderUtil.defaultVertexDescriptorPositionNormalColor()
        }
        else {

            vertexDescriptorCleaned = vertexDescriptor!

            if !Brep.isVertexDescriptorGood( type: type, vertexDescriptor: vertexDescriptorCleaned ) {
                fatalError()
            }
        }

        if color == nil {
            colorCleaned = SIMD3<Float>(0.5, 0.5, 0.5)
        }
        else {
            colorCleaned = color!
        }

        if material == nil {

            materialCleaned = RenderUtil.defaultMaterial(
                                        color: SIMD3<Float>( x: colorCleaned.x,
                                                             y: colorCleaned.y,
                                                             z: colorCleaned.z ) )
        }
        else {
            materialCleaned = material!
        }

        return ( vertexDescriptorCleaned, materialCleaned, colorCleaned )
    }

    static func isVertexDescriptorGood( type: MDLGeometryType, vertexDescriptor: MDLVertexDescriptor ) -> Bool {
    
        if type == .triangles {

            for attr in vertexDescriptor.attributes as! [MDLVertexAttribute] {

                if    attr.name == MDLVertexAttributePosition
                   || attr.name == MDLVertexAttributeNormal
                   || attr.name == MDLVertexAttributeTangent
                   || attr.name == MDLVertexAttributeBitangent
                   || attr.name == MDLVertexAttributeColor {

                    if    attr.format == MDLVertexFormat.float3
                       || attr.format == MDLVertexFormat.float4 {
                    }
                    else {
                        return false
                    }

                } else if attr.name == MDLVertexAttributeTextureCoordinate {
                    if attr.format == MDLVertexFormat.float2 {
                    }
                    else {
                        return false
                    }
                } else {
                    return false
                }
            }

        } else if type == .lines || type == .points {

            for attr in vertexDescriptor.attributes as! [MDLVertexAttribute] {

                if    attr.name == MDLVertexAttributePosition
                   || attr.name == MDLVertexAttributeColor {
                    if    attr.format == MDLVertexFormat.float3
                       || attr.format == MDLVertexFormat.float4 {
                    }
                    else {
                        return false
                    }
                } else {
                    return false
                }
            }
        } else {
            return false
        }

        return true
    }
}
