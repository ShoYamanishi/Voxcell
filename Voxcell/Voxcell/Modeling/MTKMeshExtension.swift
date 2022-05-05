import Foundation
import MetalKit

extension MTKMesh {

    // MARK: public functions

    /// extract the points (positions)  in the MTKMesh to an array.
    ///
    /// It extracts the points, whose  attribute is `MDLVertexAttributePosition` to an array of `SIMD3<Float>`.
    /// - returns: array of points in `SIMD3<Float>`
    func getPositions() -> [ SIMD3<Float> ] {

        return getSIMD3FloatArray(name: MDLVertexAttributePosition)
    }

    /// extract the normals  in the MTKMesh to an array.
    ///
    /// It extracts the normals whose  attribute is `MDLVertexAttributeNormal` to an array of `SIMD3<Float>`.
    /// - returns: array of points in `SIMD3<Float>`
    func getNormals() -> [ SIMD3<Float> ] {

        return getSIMD3FloatArray(name: MDLVertexAttributeNormal)
    }

    /// extract the texture coordinates in the MTKMesh to an array.
    ///
    /// It extracts the normals whose  attribute is `MDLVertexAttributeTextureCoordinate` to an array of `SIMD2<Float>`.
    /// - returns: array of points in `SIMD2<Float>`
    func getTextureCoordinates() -> [ SIMD2<Float> ] {

        return getSIMD2FloatArray(name: MDLVertexAttributeTextureCoordinate)
    }

    /// extract the indices into the vertex buffers for the triangles to an array of array of 3 UInt32s.
    ///
    ///  - returns: Array of Array of 3 UInt32. Each inner array represent one triangle.
    func getTriangleIndices() -> [[UInt32]] {

        var outputArray: [[UInt32]] = []

        for submesh in self.submeshes {

            if submesh.primitiveType == .triangle {

                if submesh.indexType == .uint16  {

                    let uint16Array = submesh.indexBuffer.buffer.contents().assumingMemoryBound(to: UInt16.self)
                    let count = submesh.indexBuffer.buffer.length / MemoryLayout<UInt16>.size
                    for i in 0 ..< (count / 3) {

                        outputArray.append( [
                            UInt32(uint16Array[i*3  ]),
                            UInt32(uint16Array[i*3+1]),
                            UInt32(uint16Array[i*3+2])
                        ] )
                    }

                } else if submesh.indexType == .uint32 {

                    let uint32Array = submesh.indexBuffer.buffer.contents().assumingMemoryBound(to: UInt32.self)
                    let count = submesh.indexBuffer.buffer.length / MemoryLayout<UInt32>.size
                    for i in 0 ..< (count / 3) {

                        outputArray.append( [
                            uint32Array[i*3  ],
                            uint32Array[i*3+1],
                            uint32Array[i*3+2]
                        ] )
                    }
                }
            } else {
                fatalError()
            }
        }

        return outputArray
    }

    /// prints contents of vertexBuffers nd submeshes of this MTKMesh
    ///
    /// - parameter numElementsToShow : maximum number of elements in the arrays to print
    ///
    /// This can be useful for debugging.
    func printInfo( numElementsToShow: Int ) {

        print ( "Float.size: ", MemoryLayout<Float>.size )

        for ( index, vertexBuffer ) in self.vertexBuffers.enumerated() {
            print ( "vertexBuffer[", index, "]")
            print ( "    length: ", vertexBuffer.length )
            print ( "    offset: ", vertexBuffer.offset )

            let floatArray = vertexBuffer.buffer.contents().assumingMemoryBound(to: Float.self)
            for i in 0 ..< numElementsToShow {
                print ("    ", i, floatArray[i])
            }
        }

        for ( index, submesh ) in submeshes.enumerated() {
            print ( "submesh[", index, "]")
            print ( "    indexType (uint16 = 0, uint32 = 1: ", submesh.indexType.rawValue )
            print ( "    indexCount: ", submesh.indexCount )
            print ( "    primitiveType (triangle = 3): ", submesh.primitiveType.rawValue )
            if submesh.indexType == .uint16 {
                let int16Array = submesh.indexBuffer.buffer.contents().assumingMemoryBound(to: Int16.self)
                for i in 0 ..< numElementsToShow {
                    print("    ", i, int16Array[i])
                }
            } else if submesh.indexType == .uint32 {
                let int32Array = submesh.indexBuffer.buffer.contents().assumingMemoryBound(to: Int32.self)
                for i in 0 ..< numElementsToShow {
                    print("    ", i, int32Array[i])
                }
            }

        }
    }

    // MARK: private functions

    func vertexBufferFor( _ name: String, format: MDLVertexFormat ) -> MTKMeshBuffer? {
    
        guard let attribute = self.vertexDescriptor.attributeNamed( name )
                , attribute.format == format
        else {
            return nil
        }
        return self.vertexBuffers[ attribute.bufferIndex ]
    }

    func offsetInVertexBufferFor( _ name: String, format: MDLVertexFormat ) -> Int {
    
        guard let attribute = self.vertexDescriptor.attributeNamed( name )
                , attribute.format == format
        else {
            return 0
        }
        return attribute.offset
    }

    func strideInVertexBufferFor( _ name: String, format: MDLVertexFormat ) -> Int {
    
        guard let attribute = self.vertexDescriptor.attributeNamed( name )
                , attribute.format == format
        else {
            return 0
        }
        return ( self.vertexDescriptor.layouts[ attribute.bufferIndex ] as! MDLVertexBufferLayout ).stride
    }

    func getSIMD3FloatArray(name: String) -> [ SIMD3<Float> ] {

        // first, try .float3
        let buffer3 = vertexBufferFor ( name, format: MDLVertexFormat.float3 )
        if buffer3 != nil {
            let buffer      = buffer3!.buffer
            let offset      = offsetInVertexBufferFor ( name, format: MDLVertexFormat.float3 )
            let stride      = strideInVertexBufferFor( name, format: MDLVertexFormat.float3 )
            let numVertices = Int(buffer.length / stride)
            var outputArray : [ SIMD3<Float> ] = []

            for i in 0 ..< numVertices {

                let floatArray = ( buffer.contents() + i * stride + offset ).assumingMemoryBound( to: Float.self )
            
                outputArray.append( SIMD3<Float>(x:floatArray[0], y:floatArray[1], z:floatArray[2] ) )
            }

            return outputArray

        }

        // second, try .float4
        let buffer4 = vertexBufferFor ( name, format: MDLVertexFormat.float4 )

        if buffer4 != nil {
            let buffer      = buffer4!.buffer
            let offset      = offsetInVertexBufferFor ( name, format: MDLVertexFormat.float4 )
            let stride      = strideInVertexBufferFor( name, format: MDLVertexFormat.float4 )
            let numVertices = Int(buffer.length / stride)
            var outputArray : [ SIMD3<Float> ] = []

            for i in 0 ..< numVertices {

                let floatArray = ( buffer.contents() + i * stride + offset ).assumingMemoryBound( to: Float.self )
                let w = floatArray[3]
                if w == 0.0 {

                    // the w element is 0. It can happen to affine vectors such as normals, tangents, and bitangents.
                    outputArray.append( SIMD3<Float>(x:floatArray[0], y:floatArray[1], z:floatArray[2] ) )
                }
                else {
                    // the w element is nonzero. It can happen to affine points.
                    outputArray.append( SIMD3<Float>(x:floatArray[0] / w, y:floatArray[1] / w, z:floatArray[2] / w ) )
                }
            }

            return outputArray
        }
        return []
    }

    func getSIMD2FloatArray(name: String) -> [ SIMD2<Float> ] {

        let buffer = vertexBufferFor ( name, format: MDLVertexFormat.float2 )
        if buffer == nil {
            return []
        }
        let buffer2     = buffer!.buffer
        let offset      = offsetInVertexBufferFor ( name, format: MDLVertexFormat.float2 )
        let stride      = strideInVertexBufferFor( name, format: MDLVertexFormat.float2 )
        let numVertices = Int(buffer2.length / stride)
        var outputArray : [ SIMD2<Float> ] = []

        for i in 0 ..< numVertices {

            let floatArray = ( buffer2.contents() + i * stride + offset ).assumingMemoryBound( to: Float.self )
            
            outputArray.append( SIMD2<Float>( x:floatArray[0], y:floatArray[1] ) )
        }
        return outputArray
    }

    func getSIMDFloatArrayCount(name: String, format: MDLVertexFormat ) -> Int {

        let vBuffer = vertexBufferFor ( name, format: format )
        if vBuffer == nil {
            return 0
        }
        let buffer = vBuffer!.buffer
        let stride      = strideInVertexBufferFor( name, format: format )
        return Int(buffer.length / stride)
    }

    func getNumPositions( format: MDLVertexFormat ) -> Int {
        return getSIMDFloatArrayCount( name: MDLVertexAttributePosition, format: format )
    }
}
