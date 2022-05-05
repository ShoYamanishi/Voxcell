import Foundation
import MetalKit

extension MDLMesh {

    static func generateFromStructureOfArrays(

        device:           MTLDevice,
        vertexDescriptor: MDLVertexDescriptor,
        material:         MDLMaterial,
        structureOfArrays:   StructureOfArraysForMDLMesh

    ) -> MDLMesh? {

        var floatArrays : [[Float]] = []

        structureOfArrays.generateInterleavedArrays( vertexDescriptor: vertexDescriptor, floatArrays: &floatArrays )

        let mdlMesh = MDLMesh.generateMDLMeshFromInterleavedArrays(

            device:           device,
            vertexDescriptor: vertexDescriptor,
            material:         material,
            floatArrays:      floatArrays,
            arrayIndices:     structureOfArrays.arrayIndices,
            numVertices:      structureOfArrays.numVertices
        )

        return mdlMesh
    }

    static func generateMDLMeshFromInterleavedArrays(

        device:           MTLDevice,
        vertexDescriptor: MDLVertexDescriptor,
        material:         MDLMaterial,
        floatArrays:      [[Float]],
        arrayIndices:     [ Int32 ],
        numVertices:      Int

    ) -> MDLMesh {

        let allocator = MTKMeshBufferAllocator(device: device)

        var buffers : [MTKMeshBuffer] = []

        for floatArray in floatArrays {

            buffers.append( floatArray.withUnsafeBufferPointer()
                { ptr in
                    let data = Data( buffer: ptr )
                    return allocator.newBuffer( with: data, type: MDLMeshBufferType.vertex ) as! MTKMeshBuffer
                }
            )
        }

        let bufferIndices : MTKMeshBuffer = arrayIndices.withUnsafeBufferPointer()
        { ptr in
            let data = Data( buffer: ptr )
            return allocator.newBuffer( with: data, type: MDLMeshBufferType.index ) as! MTKMeshBuffer

        }

        let bufferIndicesMDL = MDLSubmesh(
            indexBuffer:  bufferIndices,
            indexCount:   arrayIndices.count,
            indexType:    MDLIndexBitDepth.uint32,
            geometryType: MDLGeometryType.triangles,
            material:     material
        )

        let mdlMesh = MDLMesh(

            vertexBuffers: buffers,
            vertexCount:   numVertices,
            descriptor:    vertexDescriptor,
            submeshes:     [ bufferIndicesMDL ]
        )

        return mdlMesh
    }
}
