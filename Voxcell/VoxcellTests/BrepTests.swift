import XCTest

class BrepTests: XCTestCase {

    func testBrepConstructEmpty() throws {
    
        let m_01 = Brep()
        XCTAssertNil( m_01.vertices.head, "m_01.vertices.head is wrong" )
        XCTAssertNil( m_01.vertices.tail, "m_01.vertices.tail is wrong" )
        XCTAssertNil( m_01.halfEdges.head, "m_01.halfEdges.head is wrong" )
        XCTAssertNil( m_01.halfEdges.tail, "m_01.halfEdges.tail is wrong" )
        XCTAssertNil( m_01.faces.head, "m_01.faces.head is wrong" )
        XCTAssertNil( m_01.faces.tail, "m_01.faces.tail is wrong" )
    }

    func testBrepCuboid() throws {
        
        let vertices : [ SIMD3<Float> ] = [
            SIMD3<Float>(0,0,0),
            SIMD3<Float>(0,0,1),
            SIMD3<Float>(0,1,0),
            SIMD3<Float>(0,1,1),
            SIMD3<Float>(1,0,0),
            SIMD3<Float>(1,0,1),
            SIMD3<Float>(1,1,0),
            SIMD3<Float>(1,1,1)
        ]

        let verticesIndexAroundFacesCCW : [ [Int] ] = [
            [4, 6, 7, 5],  //(1,0,0)->(1,1,0)->(1,1,1)->(1,0,1)
            [0, 1, 3, 2],  //(0,0,0)->(0,0,1)->(0,1,1)->(0,1,0)
            [0, 4, 5, 1],  //(0,0,0)->(1,0,0)->(1,0,1)->(0,0,1)
            [2, 3, 7, 6],  //(0,1,0)->(0,1,1)->(1,1,1)->(1,1,0)
            [1, 5, 7, 3],  //(0,0,1)->(1,0,1)->(1,1,1)->(0,1,1)
            [0, 2, 6, 4]   //(0,0,0)->(0,1,0)->(1,1,0)->(1,0,0)
        ]

        let faceNormals : [ SIMD3<Float> ] = [
            SIMD3<Float>( 1, 0, 0),
            SIMD3<Float>(-1, 0, 0),
            SIMD3<Float>( 0,-1, 0),
            SIMD3<Float>( 0, 1, 0),
            SIMD3<Float>( 0, 0, 1),
            SIMD3<Float>( 0, 0,-1)
        ]

        let faceTangents : [ SIMD3<Float> ] = [
            SIMD3<Float>( 0, 1, 0),
            SIMD3<Float>( 0,-1, 0),
            SIMD3<Float>( 0, 0,-1),
            SIMD3<Float>( 0, 0, 1),
            SIMD3<Float>( 1, 0, 0),
            SIMD3<Float>(-1, 0, 0)
        ]

        let faceBitangents : [ SIMD3<Float> ] = [
            SIMD3<Float>( 0, 0, 1),
            SIMD3<Float>( 0, 0,-1),
            SIMD3<Float>(-1, 0, 0),
            SIMD3<Float>( 1, 0, 0),
            SIMD3<Float>( 0, 1, 0),
            SIMD3<Float>( 0,-1, 0)
        ]


        let facesIndexAroundVerticesCCW : [ [Int] ] = [
            [1,5,2],
            [1,2,4],
            [1,3,5],
            [1,4,3],
            [0,2,5],
            [0,4,2],
            [0,5,3],
            [0,3,4]
        ]

        let m_01 = Brep.constructFrom(
            vertices:                    vertices,
            facesIndexAroundVerticesCCW: facesIndexAroundVerticesCCW,
            faceNormals:                 faceNormals,
            faceTangents:                faceTangents,
            faceBitangents:              faceBitangents,
            verticesIndexAroundFacesCCW: verticesIndexAroundFacesCCW,
            textureCoordinatesAroundFacesCCW: nil
        )

        var vl : [Vertex] = []
        var vl_hel : [[HalfEdge]] = []
        for vr in m_01.vertices {

            let v = vr as! Vertex
            vl.append(v)

            var hel : [HalfEdge] = []
            for her in v.outgoingHalfEdges {
                let he = her as! HalfEdge
                hel.append(he)
            }
            vl_hel.append(hel)

        }
        XCTAssertEqual( vl.count, 8, "numVertices is wrong")

        var fl : [Face] = []
        var fl_hel : [[HalfEdge]] = []
        for fr in m_01.faces {
            let f = fr as! Face
            fl.append(f)
            var hel : [HalfEdge] = []
            for her in f.halfEdges {
                let he = her as! HalfEdge
                hel.append(he)
            }
            fl_hel.append(hel)

        }
        XCTAssertEqual( fl.count, 6, "numFaces is wrong")

        var hel : [HalfEdge] = []
        for he in m_01.halfEdges {
            hel.append( he as! HalfEdge )
        }
        XCTAssertEqual( hel.count, 24, "numFaces is wrong")

        var he_0_1 : HalfEdge?
        var he_1_0 : HalfEdge?
        var he_0_2 : HalfEdge?
        var he_2_0 : HalfEdge?
        var he_0_4 : HalfEdge?
        var he_4_0 : HalfEdge?
        var he_1_3 : HalfEdge?
        var he_3_1 : HalfEdge?
        var he_1_5 : HalfEdge?
        var he_5_1 : HalfEdge?
        var he_2_3 : HalfEdge?
        var he_3_2 : HalfEdge?
        var he_2_6 : HalfEdge?
        var he_6_2 : HalfEdge?
        var he_3_7 : HalfEdge?
        var he_7_3 : HalfEdge?
        var he_4_5 : HalfEdge?
        var he_5_4 : HalfEdge?
        var he_4_6 : HalfEdge?
        var he_6_4 : HalfEdge?
        var he_5_7 : HalfEdge?
        var he_7_5 : HalfEdge?
        var he_6_7 : HalfEdge?
        var he_7_6 : HalfEdge?

        for he in hel {
            if he.src === vl[0] && he.dst === vl[1] {
                he_0_1 = he
            } else if he.src === vl[1] && he.dst === vl[0] {
                he_1_0 = he
            } else if he.src === vl[0] && he.dst === vl[2] {
                he_0_2 = he
            } else if he.src === vl[2] && he.dst === vl[0] {
                he_2_0 = he
            } else if he.src === vl[0] && he.dst === vl[4] {
                he_0_4 = he
            } else if he.src === vl[4] && he.dst === vl[0] {
                he_4_0 = he
            } else if he.src === vl[1] && he.dst === vl[3] {
                he_1_3 = he
            } else if he.src === vl[3] && he.dst === vl[1] {
                he_3_1 = he
            } else if he.src === vl[1] && he.dst === vl[5] {
                he_1_5 = he
            } else if he.src === vl[5] && he.dst === vl[1] {
                he_5_1 = he
            } else if he.src === vl[2] && he.dst === vl[3] {
                he_2_3 = he
            } else if he.src === vl[3] && he.dst === vl[2] {
                he_3_2 = he
            } else if he.src === vl[2] && he.dst === vl[6] {
                he_2_6 = he
            } else if he.src === vl[6] && he.dst === vl[2] {
                he_6_2 = he
            } else if he.src === vl[3] && he.dst === vl[7] {
                he_3_7 = he
            } else if he.src === vl[7] && he.dst === vl[3] {
                he_7_3 = he
            } else if he.src === vl[4] && he.dst === vl[5] {
                he_4_5 = he
            } else if he.src === vl[5] && he.dst === vl[4] {
                he_5_4 = he
            } else if he.src === vl[4] && he.dst === vl[6] {
                he_4_6 = he
            } else if he.src === vl[6] && he.dst === vl[4] {
                he_6_4 = he
            } else if he.src === vl[5] && he.dst === vl[7] {
                he_5_7 = he
            } else if he.src === vl[7] && he.dst === vl[5] {
                he_7_5 = he
            } else if he.src === vl[6] && he.dst === vl[7] {
                he_6_7 = he
            } else if he.src === vl[7] && he.dst === vl[6] {
                he_7_6 = he
            }
        }

        XCTAssertEqual(vl[0].point, SIMD3<Float>(0,0,0), "vertex 0 is wrong")
        XCTAssertEqual(vl_hel[0].count, 3, "vertex 0's number of outgoing half edges is wrong")
        XCTAssertIdentical(vl_hel[0][0], he_0_1, "vertex 0's half edge 0 is wrong")
        XCTAssertIdentical(vl_hel[0][1], he_0_2, "vertex 0's half edge 1 is wrong")
        XCTAssertIdentical(vl_hel[0][2], he_0_4, "vertex 0's half edge 2 is wrong")

        XCTAssertEqual(vl[1].point, SIMD3<Float>(0,0,1), "vertex 1 is wrong")
        XCTAssertEqual(vl_hel[1].count, 3, "vertex 1's number of outgoing half edges is wrong")
        XCTAssertIdentical(vl_hel[1][0], he_1_3, "vertex 1's half edge 0 is wrong")
        XCTAssertIdentical(vl_hel[1][1], he_1_0, "vertex 1's half edge 1 is wrong")
        XCTAssertIdentical(vl_hel[1][2], he_1_5, "vertex 1's half edge 2 is wrong")

        XCTAssertEqual(vl[2].point, SIMD3<Float>(0,1,0), "vertex 2 is wrong")
        XCTAssertEqual(vl_hel[2].count, 3, "vertex 2's number of outgoing half edges is wrong")
        XCTAssertIdentical(vl_hel[2][0], he_2_0, "vertex 2's half edge 0 is wrong")
        XCTAssertIdentical(vl_hel[2][1], he_2_3, "vertex 2's half edge 1 is wrong")
        XCTAssertIdentical(vl_hel[2][2], he_2_6, "vertex 2's half edge 2 is wrong")

        XCTAssertEqual(vl[3].point, SIMD3<Float>(0,1,1), "vertex 3 is wrong")
        XCTAssertEqual(vl_hel[3].count, 3, "vertex 3's number of outgoing half edges is wrong")
        XCTAssertIdentical(vl_hel[3][0], he_3_2, "vertex 3's half edge 0 is wrong")
        XCTAssertIdentical(vl_hel[3][1], he_3_1, "vertex 3's half edge 1 is wrong")
        XCTAssertIdentical(vl_hel[3][2], he_3_7, "vertex 3's half edge 2 is wrong")

        XCTAssertEqual(vl[4].point, SIMD3<Float>(1,0,0), "vertex 4 is wrong")
        XCTAssertEqual(vl_hel[4].count, 3, "vertex 4's number of outgoing half edges is wrong")
        XCTAssertIdentical(vl_hel[4][0], he_4_6, "vertex 4's half edge 0 is wrong")
        XCTAssertIdentical(vl_hel[4][1], he_4_5, "vertex 4's half edge 1 is wrong")
        XCTAssertIdentical(vl_hel[4][2], he_4_0, "vertex 4's half edge 2 is wrong")
        
        XCTAssertEqual(vl[5].point, SIMD3<Float>(1,0,1), "vertex 5 is wrong")
        XCTAssertEqual(vl_hel[5].count, 3, "vertex 5's number of outgoing half edges is wrong")
        XCTAssertIdentical(vl_hel[5][0], he_5_4, "vertex 5's half edge 0 is wrong")
        XCTAssertIdentical(vl_hel[5][1], he_5_7, "vertex 5's half edge 1 is wrong")
        XCTAssertIdentical(vl_hel[5][2], he_5_1, "vertex 5's half edge 2 is wrong")
        
        XCTAssertEqual(vl[6].point, SIMD3<Float>(1,1,0), "vertex 6 is wrong")
        XCTAssertEqual(vl_hel[6].count, 3, "vertex 6's number of outgoing half edges is wrong")
        XCTAssertIdentical(vl_hel[6][0], he_6_7, "vertex 6's half edge 0 is wrong")
        XCTAssertIdentical(vl_hel[6][1], he_6_4, "vertex 6's half edge 1 is wrong")
        XCTAssertIdentical(vl_hel[6][2], he_6_2, "vertex 6's half edge 2 is wrong")

        XCTAssertEqual(vl[7].point, SIMD3<Float>(1,1,1), "vertex 7 is wrong")
        XCTAssertEqual(vl_hel[7].count, 3, "vertex 7's number of outgoing half edges is wrong")
        XCTAssertIdentical(vl_hel[7][0], he_7_5, "vertex 7's half edge 0 is wrong")
        XCTAssertIdentical(vl_hel[7][1], he_7_6, "vertex 7's half edge 1 is wrong")
        XCTAssertIdentical(vl_hel[7][2], he_7_3, "vertex 7's half edge 2 is wrong")

        XCTAssertEqual(fl[0].normal, SIMD3<Float>( 1, 0, 0), "face 0's normal is wrong")
        XCTAssertEqual(fl_hel[0].count, 4, "face 0's number of half edges is wrong")
        XCTAssertIdentical(fl_hel[0][0], he_4_6, "face 0's half edge 0 is wrong")
        XCTAssertIdentical(fl_hel[0][1], he_6_7, "face 0's half edge 1 is wrong")
        XCTAssertIdentical(fl_hel[0][2], he_7_5, "face 0's half edge 2 is wrong")
        XCTAssertIdentical(fl_hel[0][3], he_5_4, "face 0's half edge 3 is wrong")

        XCTAssertEqual(fl[1].normal, SIMD3<Float>(-1, 0, 0), "face 0's normal is wrong")
        XCTAssertEqual(fl_hel[1].count, 4, "face 1's number of half edges is wrong")
        XCTAssertIdentical(fl_hel[1][0], he_0_1, "face 1's half edge 0 is wrong")
        XCTAssertIdentical(fl_hel[1][1], he_1_3, "face 1's half edge 1 is wrong")
        XCTAssertIdentical(fl_hel[1][2], he_3_2, "face 1's half edge 2 is wrong")
        XCTAssertIdentical(fl_hel[1][3], he_2_0, "face 1's half edge 3 is wrong")

        XCTAssertEqual(fl[2].normal, SIMD3<Float>( 0,-1, 0), "face 0's normal is wrong")
        XCTAssertEqual(fl_hel[2].count, 4, "face 2's number of half edges is wrong")
        XCTAssertIdentical(fl_hel[2][0], he_0_4, "face 2's half edge 0 is wrong")
        XCTAssertIdentical(fl_hel[2][1], he_4_5, "face 2's half edge 1 is wrong")
        XCTAssertIdentical(fl_hel[2][2], he_5_1, "face 2's half edge 2 is wrong")
        XCTAssertIdentical(fl_hel[2][3], he_1_0, "face 2's half edge 3 is wrong")

        XCTAssertEqual(fl[3].normal, SIMD3<Float>( 0, 1, 0), "face 0's normal is wrong")
        XCTAssertEqual(fl_hel[3].count, 4, "face 3's number of half edges is wrong")
        XCTAssertIdentical(fl_hel[3][0], he_2_3, "face 3's half edge 0 is wrong")
        XCTAssertIdentical(fl_hel[3][1], he_3_7, "face 3's half edge 1 is wrong")
        XCTAssertIdentical(fl_hel[3][2], he_7_6, "face 3's half edge 2 is wrong")
        XCTAssertIdentical(fl_hel[3][3], he_6_2, "face 3's half edge 3 is wrong")
        
        XCTAssertEqual(fl[4].normal, SIMD3<Float>( 0, 0, 1), "face 0's normal is wrong")
        XCTAssertEqual(fl_hel[4].count, 4, "face 4's number of half edges is wrong")
        XCTAssertIdentical(fl_hel[4][0], he_1_5, "face 4's half edge 0 is wrong")
        XCTAssertIdentical(fl_hel[4][1], he_5_7, "face 4's half edge 1 is wrong")
        XCTAssertIdentical(fl_hel[4][2], he_7_3, "face 4's half edge 2 is wrong")
        XCTAssertIdentical(fl_hel[4][3], he_3_1, "face 4's half edge 3 is wrong")

        XCTAssertEqual(fl[5].normal, SIMD3<Float>( 0, 0,-1), "face 0's normal is wrong")
        XCTAssertEqual(fl_hel[5].count, 4, "face 5's number of half edges is wrong")
        XCTAssertIdentical(fl_hel[5][0], he_0_2, "face 5's half edge 0 is wrong")
        XCTAssertIdentical(fl_hel[5][1], he_2_6, "face 5's half edge 1 is wrong")
        XCTAssertIdentical(fl_hel[5][2], he_6_4, "face 5's half edge 2 is wrong")
        XCTAssertIdentical(fl_hel[5][3], he_4_0, "face 5's half edge 3 is wrong")

        XCTAssertIdentical(he_0_1!.src, vl[0], "half edge 0-1's src vertex is wrong")
        XCTAssertIdentical(he_0_1!.dst, vl[1], "half edge 0-1's dst vertex is wrong")
        XCTAssertIdentical(he_0_1!.prevHalfEdge,  he_2_0, "half edge 0-1's prev half edge is wrong")
        XCTAssertIdentical(he_0_1!.nextHalfEdge,  he_1_3, "half edge 0-1's next half edge is wrong")
        XCTAssertIdentical(he_0_1!.buddyHalfEdge, he_1_0, "half edge 0-1's buddy half edge is wrong")
        XCTAssertIdentical(he_0_1!.face, fl[1], "half edge 0-1's face is wrong")

        XCTAssertIdentical(he_1_0!.src, vl[1], "half edge 1-0's src vertex is wrong")
        XCTAssertIdentical(he_1_0!.dst, vl[0], "half edge 1-0's dst vertex is wrong")
        XCTAssertIdentical(he_1_0!.prevHalfEdge,  he_5_1, "half edge 1-0's prev half edge is wrong")
        XCTAssertIdentical(he_1_0!.nextHalfEdge,  he_0_4, "half edge 1-0's next half edge is wrong")
        XCTAssertIdentical(he_1_0!.buddyHalfEdge, he_0_1, "half edge 1-0's buddy half edge is wrong")
        XCTAssertIdentical(he_1_0!.face, fl[2], "half edge 1-0's face is wrong")

        XCTAssertIdentical(he_0_2!.src, vl[0], "half edge 0-2's src vertex is wrong")
        XCTAssertIdentical(he_0_2!.dst, vl[2], "half edge 0-2's dst vertex is wrong")
        XCTAssertIdentical(he_0_2!.prevHalfEdge,  he_4_0, "half edge 0-2's prev half edge is wrong")
        XCTAssertIdentical(he_0_2!.nextHalfEdge,  he_2_6, "half edge 0-2's next half edge is wrong")
        XCTAssertIdentical(he_0_2!.buddyHalfEdge, he_2_0, "half edge 0-2's buddy half edge is wrong")
        XCTAssertIdentical(he_0_2!.face, fl[5], "half edge 0-2's face is wrong")

        XCTAssertIdentical(he_2_0!.src, vl[2], "half edge 2-0's src vertex is wrong")
        XCTAssertIdentical(he_2_0!.dst, vl[0], "half edge 2-0's dst vertex is wrong")
        XCTAssertIdentical(he_2_0!.prevHalfEdge,  he_3_2, "half edge 2-0's prev half edge is wrong")
        XCTAssertIdentical(he_2_0!.nextHalfEdge,  he_0_1, "half edge 2-0's next half edge is wrong")
        XCTAssertIdentical(he_2_0!.buddyHalfEdge, he_0_2, "half edge 2-0's buddy half edge is wrong")
        XCTAssertIdentical(he_2_0!.face, fl[1], "half edge 2-0's face is wrong")

        XCTAssertIdentical(he_0_4!.src, vl[0], "half edge 0-4's src vertex is wrong")
        XCTAssertIdentical(he_0_4!.dst, vl[4], "half edge 0-4's dst vertex is wrong")
        XCTAssertIdentical(he_0_4!.prevHalfEdge,  he_1_0, "half edge 0-4's prev half edge is wrong")
        XCTAssertIdentical(he_0_4!.nextHalfEdge,  he_4_5, "half edge 0-4's next half edge is wrong")
        XCTAssertIdentical(he_0_4!.buddyHalfEdge, he_4_0, "half edge 0-4's buddy half edge is wrong")
        XCTAssertIdentical(he_0_4!.face, fl[2], "half edge 0-4's face is wrong")

        XCTAssertIdentical(he_4_0!.src, vl[4], "half edge 4-0's src vertex is wrong")
        XCTAssertIdentical(he_4_0!.dst, vl[0], "half edge 4-0's dst vertex is wrong")
        XCTAssertIdentical(he_4_0!.prevHalfEdge,  he_6_4, "half edge 4-0's prev half edge is wrong")
        XCTAssertIdentical(he_4_0!.nextHalfEdge,  he_0_2, "half edge 4-0's next half edge is wrong")
        XCTAssertIdentical(he_4_0!.buddyHalfEdge, he_0_4, "half edge 4-0's buddy half edge is wrong")
        XCTAssertIdentical(he_4_0!.face, fl[5], "half edge 4-0's face is wrong")

        XCTAssertIdentical(he_1_3!.src, vl[1], "half edge 1-3's src vertex is wrong")
        XCTAssertIdentical(he_1_3!.dst, vl[3], "half edge 1-3's dst vertex is wrong")
        XCTAssertIdentical(he_1_3!.prevHalfEdge,  he_0_1, "half edge 1-3's prev half edge is wrong")
        XCTAssertIdentical(he_1_3!.nextHalfEdge,  he_3_2, "half edge 1-3's next half edge is wrong")
        XCTAssertIdentical(he_1_3!.buddyHalfEdge, he_3_1, "half edge 1-3's buddy half edge is wrong")
        XCTAssertIdentical(he_1_3!.face, fl[1], "half edge 1-3's face is wrong")

        XCTAssertIdentical(he_3_1!.src, vl[3], "half edge 3-1's src vertex is wrong")
        XCTAssertIdentical(he_3_1!.dst, vl[1], "half edge 3-1's dst vertex is wrong")
        XCTAssertIdentical(he_3_1!.prevHalfEdge,  he_7_3, "half edge 3-1's prev half edge is wrong")
        XCTAssertIdentical(he_3_1!.nextHalfEdge,  he_1_5, "half edge 3-1's next half edge is wrong")
        XCTAssertIdentical(he_3_1!.buddyHalfEdge, he_1_3, "half edge 3-1's buddy half edge is wrong")
        XCTAssertIdentical(he_3_1!.face, fl[4], "half edge 3-1's face is wrong")

        XCTAssertIdentical(he_1_5!.src, vl[1], "half edge 1-5's src vertex is wrong")
        XCTAssertIdentical(he_1_5!.dst, vl[5], "half edge 1-5's dst vertex is wrong")
        XCTAssertIdentical(he_1_5!.prevHalfEdge,  he_3_1, "half edge 1-5's prev half edge is wrong")
        XCTAssertIdentical(he_1_5!.nextHalfEdge,  he_5_7, "half edge 1-5's next half edge is wrong")
        XCTAssertIdentical(he_1_5!.buddyHalfEdge, he_5_1, "half edge 1-5's buddy half edge is wrong")
        XCTAssertIdentical(he_1_5!.face, fl[4], "half edge 1-5's face is wrong")

        XCTAssertIdentical(he_5_1!.src, vl[5], "half edge 5-1's src vertex is wrong")
        XCTAssertIdentical(he_5_1!.dst, vl[1], "half edge 5-1's dst vertex is wrong")
        XCTAssertIdentical(he_5_1!.prevHalfEdge,  he_4_5, "half edge 5-1's prev half edge is wrong")
        XCTAssertIdentical(he_5_1!.nextHalfEdge,  he_1_0, "half edge 5-1's next half edge is wrong")
        XCTAssertIdentical(he_5_1!.buddyHalfEdge, he_1_5, "half edge 5-1's buddy half edge is wrong")
        XCTAssertIdentical(he_5_1!.face, fl[2], "half edge 5-1's face is wrong")

        XCTAssertIdentical(he_2_3!.src, vl[2], "half edge 2-3's src vertex is wrong")
        XCTAssertIdentical(he_2_3!.dst, vl[3], "half edge 2-3's dst vertex is wrong")
        XCTAssertIdentical(he_2_3!.prevHalfEdge,  he_6_2, "half edge 2-3's prev half edge is wrong")
        XCTAssertIdentical(he_2_3!.nextHalfEdge,  he_3_7, "half edge 2-3's next half edge is wrong")
        XCTAssertIdentical(he_2_3!.buddyHalfEdge, he_3_2, "half edge 2-3's buddy half edge is wrong")
        XCTAssertIdentical(he_2_3!.face, fl[3], "half edge 2-3's face is wrong")

        XCTAssertIdentical(he_3_2!.src, vl[3], "half edge 3-2's src vertex is wrong")
        XCTAssertIdentical(he_3_2!.dst, vl[2], "half edge 3-2's dst vertex is wrong")
        XCTAssertIdentical(he_3_2!.prevHalfEdge,  he_1_3, "half edge 3-2's prev half edge is wrong")
        XCTAssertIdentical(he_3_2!.nextHalfEdge,  he_2_0, "half edge 3-2's next half edge is wrong")
        XCTAssertIdentical(he_3_2!.buddyHalfEdge, he_2_3, "half edge 3-2's buddy half edge is wrong")
        XCTAssertIdentical(he_3_2!.face, fl[1], "half edge 3-2's face is wrong")

        XCTAssertIdentical(he_2_6!.src, vl[2], "half edge 2-6's src vertex is wrong")
        XCTAssertIdentical(he_2_6!.dst, vl[6], "half edge 2-6's dst vertex is wrong")
        XCTAssertIdentical(he_2_6!.prevHalfEdge,  he_0_2, "half edge 2-6's prev half edge is wrong")
        XCTAssertIdentical(he_2_6!.nextHalfEdge,  he_6_4, "half edge 2-6's next half edge is wrong")
        XCTAssertIdentical(he_2_6!.buddyHalfEdge, he_6_2, "half edge 2-6's buddy half edge is wrong")
        XCTAssertIdentical(he_2_6!.face, fl[5], "half edge 2-6's face is wrong")

        XCTAssertIdentical(he_6_2!.src, vl[6], "half edge 6-2's src vertex is wrong")
        XCTAssertIdentical(he_6_2!.dst, vl[2], "half edge 6-2's dst vertex is wrong")
        XCTAssertIdentical(he_6_2!.prevHalfEdge,  he_7_6, "half edge 6-2's prev half edge is wrong")
        XCTAssertIdentical(he_6_2!.nextHalfEdge,  he_2_3, "half edge 6-2's next half edge is wrong")
        XCTAssertIdentical(he_6_2!.buddyHalfEdge, he_2_6, "half edge 6-2's buddy half edge is wrong")
        XCTAssertIdentical(he_6_2!.face, fl[3], "half edge 6-2's face is wrong")

        XCTAssertIdentical(he_3_7!.src, vl[3], "half edge 3-7's src vertex is wrong")
        XCTAssertIdentical(he_3_7!.dst, vl[7], "half edge 3-7's dst vertex is wrong")
        XCTAssertIdentical(he_3_7!.prevHalfEdge,  he_2_3, "half edge 3-7's prev half edge is wrong")
        XCTAssertIdentical(he_3_7!.nextHalfEdge,  he_7_6, "half edge 3-7's next half edge is wrong")
        XCTAssertIdentical(he_3_7!.buddyHalfEdge, he_7_3, "half edge 3-7's buddy half edge is wrong")
        XCTAssertIdentical(he_3_7!.face, fl[3], "half edge 3-7's face is wrong")

        XCTAssertIdentical(he_7_3!.src, vl[7], "half edge 7-3's src vertex is wrong")
        XCTAssertIdentical(he_7_3!.dst, vl[3], "half edge 7-3's dst vertex is wrong")
        XCTAssertIdentical(he_7_3!.prevHalfEdge,  he_5_7, "half edge 7-3's prev half edge is wrong")
        XCTAssertIdentical(he_7_3!.nextHalfEdge,  he_3_1, "half edge 7-3's next half edge is wrong")
        XCTAssertIdentical(he_7_3!.buddyHalfEdge, he_3_7, "half edge 7-3's buddy half edge is wrong")
        XCTAssertIdentical(he_7_3!.face, fl[4], "half edge 7-3's face is wrong")

        XCTAssertIdentical(he_4_5!.src, vl[4], "half edge 4-5's src vertex is wrong")
        XCTAssertIdentical(he_4_5!.dst, vl[5], "half edge 4-5's dst vertex is wrong")
        XCTAssertIdentical(he_4_5!.prevHalfEdge,  he_0_4, "half edge 4-5's prev half edge is wrong")
        XCTAssertIdentical(he_4_5!.nextHalfEdge,  he_5_1, "half edge 4-5's next half edge is wrong")
        XCTAssertIdentical(he_4_5!.buddyHalfEdge, he_5_4, "half edge 4-5's buddy half edge is wrong")
        XCTAssertIdentical(he_4_5!.face, fl[2], "half edge 4-5's face is wrong")

        XCTAssertIdentical(he_5_4!.src, vl[5], "half edge 5-4's src vertex is wrong")
        XCTAssertIdentical(he_5_4!.dst, vl[4], "half edge 5-4's dst vertex is wrong")
        XCTAssertIdentical(he_5_4!.prevHalfEdge,  he_7_5, "half edge 5-4's prev half edge is wrong")
        XCTAssertIdentical(he_5_4!.nextHalfEdge,  he_4_6, "half edge 5-4's next half edge is wrong")
        XCTAssertIdentical(he_5_4!.buddyHalfEdge, he_4_5, "half edge 5-4's buddy half edge is wrong")
        XCTAssertIdentical(he_5_4!.face, fl[0], "half edge 5-4's face is wrong")

        XCTAssertIdentical(he_4_6!.src, vl[4], "half edge 4-6's src vertex is wrong")
        XCTAssertIdentical(he_4_6!.dst, vl[6], "half edge 4-6's dst vertex is wrong")
        XCTAssertIdentical(he_4_6!.prevHalfEdge,  he_5_4, "half edge 4-6's prev half edge is wrong")
        XCTAssertIdentical(he_4_6!.nextHalfEdge,  he_6_7, "half edge 4-6's next half edge is wrong")
        XCTAssertIdentical(he_4_6!.buddyHalfEdge, he_6_4, "half edge 4-6's buddy half edge is wrong")
        XCTAssertIdentical(he_4_6!.face, fl[0], "half edge 4-6's face is wrong")

        XCTAssertIdentical(he_6_4!.src, vl[6], "half edge 6-4's src vertex is wrong")
        XCTAssertIdentical(he_6_4!.dst, vl[4], "half edge 6-4's dst vertex is wrong")
        XCTAssertIdentical(he_6_4!.prevHalfEdge,  he_2_6, "half edge 6-4's prev half edge is wrong")
        XCTAssertIdentical(he_6_4!.nextHalfEdge,  he_4_0, "half edge 6-4's next half edge is wrong")
        XCTAssertIdentical(he_6_4!.buddyHalfEdge, he_4_6, "half edge 6-4's buddy half edge is wrong")
        XCTAssertIdentical(he_6_4!.face, fl[5], "half edge 6-4's face is wrong")

        XCTAssertIdentical(he_5_7!.src, vl[5], "half edge 5-7's src vertex is wrong")
        XCTAssertIdentical(he_5_7!.dst, vl[7], "half edge 5-7's dst vertex is wrong")
        XCTAssertIdentical(he_5_7!.prevHalfEdge,  he_1_5, "half edge 5-7's prev half edge is wrong")
        XCTAssertIdentical(he_5_7!.nextHalfEdge,  he_7_3, "half edge 5-7's next half edge is wrong")
        XCTAssertIdentical(he_5_7!.buddyHalfEdge, he_7_5, "half edge 5-7's buddy half edge is wrong")
        XCTAssertIdentical(he_5_7!.face, fl[4], "half edge 5-7's face is wrong")

        XCTAssertIdentical(he_7_5!.src, vl[7], "half edge 7-5's src vertex is wrong")
        XCTAssertIdentical(he_7_5!.dst, vl[5], "half edge 7-5's dst vertex is wrong")
        XCTAssertIdentical(he_7_5!.prevHalfEdge,  he_6_7, "half edge 7-5's prev half edge is wrong")
        XCTAssertIdentical(he_7_5!.nextHalfEdge,  he_5_4, "half edge 7-5's next half edge is wrong")
        XCTAssertIdentical(he_7_5!.buddyHalfEdge, he_5_7, "half edge 7-5's buddy half edge is wrong")
        XCTAssertIdentical(he_7_5!.face, fl[0], "half edge 7-5's face is wrong")

        XCTAssertIdentical(he_6_7!.src, vl[6], "half edge 6-7's src vertex is wrong")
        XCTAssertIdentical(he_6_7!.dst, vl[7], "half edge 6-7's dst vertex is wrong")
        XCTAssertIdentical(he_6_7!.prevHalfEdge,  he_4_6, "half edge 6-7's prev half edge is wrong")
        XCTAssertIdentical(he_6_7!.nextHalfEdge,  he_7_5, "half edge 6-7's next half edge is wrong")
        XCTAssertIdentical(he_6_7!.buddyHalfEdge, he_7_6, "half edge 6-7's buddy half edge is wrong")
        XCTAssertIdentical(he_6_7!.face, fl[0], "half edge 6-7's face is wrong")

        XCTAssertIdentical(he_7_6!.src, vl[7], "half edge 7-6's src vertex is wrong")
        XCTAssertIdentical(he_7_6!.dst, vl[6], "half edge 7-6's dst vertex is wrong")
        XCTAssertIdentical(he_7_6!.prevHalfEdge,  he_3_7, "half edge 7-6's prev half edge is wrong")
        XCTAssertIdentical(he_7_6!.nextHalfEdge,  he_6_2, "half edge 7-6's next half edge is wrong")
        XCTAssertIdentical(he_7_6!.buddyHalfEdge, he_6_7, "half edge 7-6's buddy half edge is wrong")
        XCTAssertIdentical(he_7_6!.face, fl[3], "half edge 7-6's face is wrong")
    }
}

