import Foundation

extension Brep {
    
    static func constructFeaturesFromManifoldObjc(_ mObjc: ManifoldObjc ) -> Brep {

        let verticesPointer : UnsafePointer< SIMD3<Float> >? = mObjc.vertices()
        let verticesArray = Array( UnsafeBufferPointer(start: verticesPointer, count: mObjc.numVertices()) )

        var facesIndexAroundVerticesCCW: [[Int]] = []
        for i in 0 ..< mObjc.numVertices() {
            let faceIndexPointer : UnsafePointer< Int >? = mObjc.facesIndexAroundVertice( for: i )
            let faceIndexArray : [Int] = Array( UnsafeBufferPointer(
                                             start: faceIndexPointer,
                                             count: mObjc.numFacesIndexAroundVertice(for: i)
                                         ) )
            facesIndexAroundVerticesCCW.append(faceIndexArray)
        }

        let faceNormalsPointer : UnsafePointer< SIMD3<Float> >? = mObjc.faceNormals()
        let faceNormalsArray = Array( UnsafeBufferPointer(start: faceNormalsPointer, count: mObjc.numFaceNormals()) )

        let faceTangentsPointer : UnsafePointer< SIMD3<Float> >? = mObjc.faceTangents()
        let faceTangentsArray = Array( UnsafeBufferPointer(start: faceTangentsPointer, count: mObjc.numFaceTangents()) )

        let faceBitangentsPointer : UnsafePointer< SIMD3<Float> >? = mObjc.faceBitangents()
        let faceBitangentsArray = Array( UnsafeBufferPointer(start: faceBitangentsPointer, count: mObjc.numFaceBitangents()) )

        var verticesIndexAroundFacesCCW: [[Int]] = []
        for i in 0 ..< mObjc.numFaceNormals() {
            let vertexIndexPointer : UnsafePointer< Int >? = mObjc.verticesIndexAroundFace( for: i )
            let vertexIndexArray : [Int] = Array( UnsafeBufferPointer(
                                             start: vertexIndexPointer,
                                             count: mObjc.numVerticesIndexAroundFace(for: i)
                                         ) )
            verticesIndexAroundFacesCCW.append(vertexIndexArray)
        }

        var textureCoordinatesAroundFacesCCW : [[SIMD2<Float>]] = []
        for i in 0 ..< mObjc.numFaceNormals() {
            let textureCoordinatesPointer : UnsafePointer< SIMD2<Float> >? = mObjc.textureCoordinatesAroundFace( for: i )
            let textureCoordinatesArray : [SIMD2<Float>] = Array( UnsafeBufferPointer(
                                             start: textureCoordinatesPointer,
                                             count: mObjc.numVerticesIndexAroundFace(for: i)
                                         ) )
            textureCoordinatesAroundFacesCCW.append(textureCoordinatesArray)
        }

        let m = Brep.constructFrom(
            vertices :                         verticesArray,
            facesIndexAroundVerticesCCW :      facesIndexAroundVerticesCCW,
            faceNormals :                      faceNormalsArray,
            faceTangents :                     faceTangentsArray,
            faceBitangents :                   faceBitangentsArray,
            verticesIndexAroundFacesCCW :      verticesIndexAroundFacesCCW,
            textureCoordinatesAroundFacesCCW : textureCoordinatesAroundFacesCCW
        )

        return m
    }
}
