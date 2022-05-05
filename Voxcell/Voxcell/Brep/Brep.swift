import Foundation

public class Vertex : ListElem {

    public var outgoingHalfEdges = WeakList1()
    
    public var point = SIMD3<Float>(0,0,0)
    
    init(_ v : SIMD3<Float>) {
        point = v
    }
}

public class HalfEdge : ListElemWithWeakChain2 {

    public weak var src : Vertex?
    public weak var dst : Vertex?

    public weak var prevHalfEdge  : HalfEdge?
    public weak var nextHalfEdge  : HalfEdge?
    public weak var buddyHalfEdge : HalfEdge?

    public weak var face : Face?
}

public class Face : ListElem {

    public var halfEdges = WeakList2()

    public var normal    = SIMD3<Float>(0,0,0)
    public var tangent   = SIMD3<Float>(0,0,0)
    public var bitangent = SIMD3<Float>(0,0,0)

    public var textureCoordinates : [SIMD2<Float>]?
}


public class Brep {

    // Brep is the owner of all the features.
    public var vertices  = OwnerList()
    public var halfEdges = OwnerList()
    public var faces     = OwnerList()

    /// Constructs a boundary representation or Brep from the lists of vertices and faces
    ///
    ///
    /// - parameter vertices: This array contains the points in 3-space. The indices to this array correspond to the vertex ID.
    /// - parameter verticesIndexAroundFacesCCW: This array contains the incident faces in their IDs in counterclockwise. The indices to this array correspond to the vertex ID.
    /// - parameter faceNormals: This array contains the normals of the faces. The indices to this array correspond to the face ID.
    /// - parameter facesIndexAroundVerticesCCW: This array contains the incident vertices in their IDs in counterclockwise. The indices to this array correspond to the face ID.
    ///
    /// - returns: A Brep that represents the convex hull with features interconnected.
    ///
    /// **Example parameter values for a cuboid**
    ///```
    ///  vertices[0]: (0,0,0)
    ///  vertices[1]: (0,0,1)
    ///  vertices[2]: (0,1,0)
    ///  vertices[3]: (0,1,1)
    ///  vertices[4]: (1,0,0)
    ///  vertices[5]: (1,0,1)
    ///  vertices[6]: (1,1,0)
    ///  vertices[7]: (1,1,1)
    ///
    ///  facesIndexAroundVerticesCCW[0]: [1,5,2]
    ///  facesIndexAroundVerticesCCW[1]: [1,2,4]
    ///  facesIndexAroundVerticesCCW[2]: [1,3,5]
    ///  facesIndexAroundVerticesCCW[3]: [1,4,3]
    ///  facesIndexAroundVerticesCCW[4]: [0,2,5]
    ///  facesIndexAroundVerticesCCW[5]: [0,4,2]
    ///  facesIndexAroundVerticesCCW[6]: [0,5,3]
    ///  facesIndexAroundVerticesCCW[7]: [0,3,4]
    ///
    ///  faceNormals[0]: ( 1, 0, 0)
    ///  faceNormals[1]: (-1, 0, 0)
    ///  faceNormals[2]: ( 0,-1, 0)
    ///  faceNormals[3]: ( 0, 1, 0)
    ///  faceNormals[4]: ( 0, 0, 1)
    ///  faceNormals[5]: ( 0, 0,-1)
    ///
    ///  verticesIndexAroundFacesCCW[0]: [4, 6, 7, 5]  //(1,0,0)->(1,1,0)->(1,1,1)->(1,0,1)
    ///  verticesIndexAroundFacesCCW[1]: [0, 1, 3, 2]  //(0,0,0)->(0,0,1)->(0,1,1)->(0,1,0)
    ///  verticesIndexAroundFacesCCW[2]: [0, 4, 5, 1]  //(0,0,0)->(1,0,0)->(1,0,1)->(0,0,1)
    ///  verticesIndexAroundFacesCCW[3]: [2, 3, 7, 6]  //(0,1,0)->(0,1,1)->(1,1,1)->(1,1,0)
    ///  verticesIndexAroundFacesCCW[4]: [1, 5, 7, 3]  //(0,0,1)->(1,0,1)->(1,1,1)->(0,1,1)
    ///  verticesIndexAroundFacesCCW[5]: [0, 2, 6, 4]  //(0,0,0)->(0,1,0)->(1,1,0)->(1,0,0)
    ///```
    ///
    static public func constructFrom(

        vertices :                         [ SIMD3<Float>   ],
        facesIndexAroundVerticesCCW :      [ [Int]          ],
        faceNormals :                      [ SIMD3<Float>   ],
        faceTangents :                     [ SIMD3<Float>   ],
        faceBitangents :                   [ SIMD3<Float>   ],
        verticesIndexAroundFacesCCW :      [ [Int]          ],
        textureCoordinatesAroundFacesCCW : [ [SIMD2<Float>] ]?

    ) -> Brep {
    
        let m = Brep()

        // Temporary lists during the construction
        var verticesArray  : [ Vertex   ] = []
        var facesArray     : [ Face     ] = []
        var halfEdgesArray : [ HalfEdge ] = []

        // Construct Vertex List
        for vertex in vertices {

            let v = Vertex( vertex )
            verticesArray.append( v )
            m.vertices.insert( v, before: nil )
        }

        // Constcut Face List and HalfEdges
        for ( faceIndex, vertexIndices ) in verticesIndexAroundFacesCCW.enumerated() {

            // Process the first Vertex and HalfEdge

            let f = Face()

            f.normal    = faceNormals[faceIndex]
            f.tangent   = faceTangents[faceIndex]
            f.bitangent = faceBitangents[faceIndex]

            facesArray.append(f)
            
            m.faces.insert( f, before: nil )
            let i0   = vertexIndices[ 0 ]
            let v0   = verticesArray[ i0 ]
            let he0  = HalfEdge()
            halfEdgesArray.append(he0)
            he0.face = f
            he0.src  = v0
            f.halfEdges.insert( he0, before: nil )
            m.halfEdges.insert( he0, before: nil )
            var phe = he0
            for fi in 1 ..< vertexIndices.count {

                // Process the second Vertex onwards

                let i = vertexIndices[ fi ]
                let v = verticesArray[ i ]
                let he = HalfEdge()
                halfEdgesArray.append(he)
                he.face = f
                he.src = v
                f.halfEdges.insert( he, before: nil )
                m.halfEdges.insert( he, before: nil )
                phe.dst = v
                phe.nextHalfEdge = he
                he.prevHalfEdge = phe
                phe = he
            }
            phe.dst = v0
            phe.nextHalfEdge = he0
            he0.prevHalfEdge = phe
            
            f.textureCoordinates = textureCoordinatesAroundFacesCCW?[ faceIndex ]
        }

        // Patch-up buddies of HalfEdges
        for i1 in 0 ..< halfEdgesArray.count {

            let he1 = halfEdgesArray[i1]

            if he1.buddyHalfEdge == nil {

                for i2 in i1 ..< halfEdgesArray.count {

                    let he2 = halfEdgesArray[i2]
                
                    if he1.src === he2.dst && he1.dst === he2.src {

                        he1.buddyHalfEdge = he2
                        he2.buddyHalfEdge = he1
                        break
                    }
                }
            }
        }

        // Construct incident halfEdge list for each vertex
        for i in 0 ..< verticesArray.count {

            let v = verticesArray[ i ]
            let faceIndices = facesIndexAroundVerticesCCW[ i ]
            for faceIndex in faceIndices {
                let face = facesArray[faceIndex]

                for listElem in face.halfEdges {

                    let he = listElem as! HalfEdge

                    if he.src === v {

                        v.outgoingHalfEdges.insert( he, before: nil )
                        break
                    }
                }
            }
        }

        return m
    }
}
