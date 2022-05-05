#ifndef _MAKENA_MANIFOLD_HPP_
#define _MAKENA_MANIFOLD_HPP_

#include <memory>
#include <array>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <exception>
#include <stdexcept>
#include <cmath>

#include "di_base.hpp"
#include "primitives.hpp"
#include "quaternion.hpp"
#include "loggable.hpp"

#ifdef UNIT_TESTS
#include "gtest/gtest_prod.h"
#endif


/**
 * @file manifold.hpp
 *
 * @reference
 *
 */
namespace Makena {

using namespace std;
using namespace Wailea;


/* Predicates for the geometric features to indicate degeneracy */
enum predicate {
    NONE,

    MAYBE_COINCIDENT,
    MAYBE_COLINEAR,
    MAYBE_COPLANAR,
    MAYBE_FLAT,
    INCOMPLETE_MANIFOLD,

    ON_POINT1,
    ON_POINT2,
    ON_POINT3,
    BETWEEN_1_AND_2,

    FLAT_TRIANGLE_1_AND_3,
    FLAT_TRIANGLE_1_AND_2,
    FLAT_TRIANGLE_2_AND_3,
    FLAT_TETRAHEDRON_123,
    FLAT_TETRAHEDRON_234,
    FLAT_TETRAHEDRON_341,
    FLAT_TETRAHEDRON_412,
    FLAT_TETRAHEDRON_QUAD,

    VORONOI_ON_VERTEX_1,
    VORONOI_ON_VERTEX_2,
    VORONOI_ON_VERTEX_3,
    VORONOI_ON_VERTEX_4,
    VORONOI_OVER_VERTEX_1,
    VORONOI_OVER_VERTEX_2,
    VORONOI_OVER_VERTEX_3,
    VORONOI_OVER_VERTEX_4,
    VORONOI_ON_EDGE_1_2,
    VORONOI_ON_EDGE_2_3,
    VORONOI_ON_EDGE_3_1,
    VORONOI_ON_EDGE_1_4,
    VORONOI_ON_EDGE_2_4,
    VORONOI_ON_EDGE_3_4,
    VORONOI_OVER_EDGE_1_2,
    VORONOI_OVER_EDGE_2_3,
    VORONOI_OVER_EDGE_3_1,
    VORONOI_OVER_EDGE_1_4,
    VORONOI_OVER_EDGE_2_4,
    VORONOI_OVER_EDGE_3_4,
    VORONOI_INSIDE_TRIANGLE,
    VORONOI_INSIDE_TRIANGLE_1_3_2,
    VORONOI_INSIDE_TRIANGLE_1_2_4,
    VORONOI_INSIDE_TRIANGLE_2_3_4,
    VORONOI_INSIDE_TRIANGLE_1_4_3,
    VORONOI_OVER_TRIANGLE_1_3_2,
    VORONOI_OVER_TRIANGLE_1_2_4,
    VORONOI_OVER_TRIANGLE_2_3_4,
    VORONOI_OVER_TRIANGLE_1_4_3,
    VORONOI_INSIDE_TETRAHEDRON,

    IF_ACTIVE,
    IF_PROCESSED,

    IF_VERTEX_VERTEX,
    IF_VERTEX_EDGE,
    IF_VERTEX_FACE,
    IF_VERTEX_INTERIOR,
    IF_EDGE_VERTEX,
    IF_EDGE_EDGE,
    IF_EDGE_FACE,
    IF_EDGE_INTERIOR,
    IF_FACE_VERTEX,
    IF_FACE_EDGE,
    IF_FACE_FACE,
    IF_FACE_INTERIOR,
    IF_INTERIOR_VERTEX,
    IF_INTERIOR_EDGE,
    IF_INTERIOR_FACE,
    IF_INTERIOR_INTERIOR,

    IF_VERTEX,
    IF_EDGE,
    IF_FACE,
    IF_INTERIOR,

    IF_FRONT_OF_PLANE,
    IF_ON_PLANE,
    IF_BACK_OF_PLANE,

    IF_FACE_BOUNDARY,
    IF_FACE_POLYTOPE_1,
    IF_FACE_POLYTOPE_2,
    IF_EDGE_BOUNDARY_INTERIOR,
    IF_EDGE_POLYTOPE_1_INTERIOR,
    IF_EDGE_POLYTOPE_2_INTERIOR,
    IF_EDGE_BOUNDARY_PROPER,
    IF_EDGE_BOUNDARY_ON_POLYTOPE_1,
    IF_EDGE_BOUNDARY_ON_POLYTOPE_2,
    IF_EDGE_DEGENERATE_EMPTY_POLYTOPE_1,
    IF_EDGE_DEGENERATE_EMPTY_POLYTOPE_2,

    IF_VERTEX_DEGENERATE_EMPTY_POLYTOPE_1,
    IF_VERTEX_DEGENERATE_EMPTY_POLYTOPE_2,

    IF_POLYTOPE_1,
    IF_POLYTOPE_2,
    IF_BOUNDARY,

    PRED_END
};


class Vertex;
class HalfEdge;
class Edge;
class Face;
class Manifold;

/** @brief the following are substitue to raw pointers.
 *         use of raw pointers are discouraged but use of shared_ptr
 *         is clumsy and vulnerable to memory leak due ot ciarcular references.
 *         The weak_ptr has to be promoted to shared_ptr by lock() for every
 *         reference. Some experiments on clang-802.0.42 on MacBook Pro shows
 *         using iterators is as fast as using raw pointers in general.
 */
using VertexIt   = list<unique_ptr<Vertex  > >::iterator;
using HalfEdgeIt = list<unique_ptr<HalfEdge> >::iterator;
using EdgeIt     = list<unique_ptr<Edge    > >::iterator;
using FaceIt     = list<unique_ptr<Face    > >::iterator;
using ManifoldIt = list<unique_ptr<Manifold> >::iterator;


class Vertex {

  public:

    inline Vec3& pLCS();
    inline Vec3  pGCS(const Mat3x3& rot);
    inline Vec3  pGCS(const Mat3x3& rot, const Vec3& CoM);
    inline Vec3  pGCS(const double scale, const Mat3x3& rot, const Vec3& CoM);

    inline const Vec3& nLCS() const;
    inline Vec3  nGCS(const Mat3x3&rot) const;

    inline virtual ~Vertex();
    inline const list<HalfEdgeIt>& halfEdges();

    inline void resetGen();
    inline void updateGen(unsigned long newGen);
    inline unsigned long gen();
    inline bool isGenUpToDate(unsigned long gen);

    inline long id();

    inline void   IFsetDot(const double& dot);
    inline double IFdot() const;

    inline void   IFreset();
    inline void   IFsetActive();
    inline bool   IFisActive() const;
    inline void   IFsetProcessed();
    inline bool   IFisProcessed() const;

    inline void   IFincrement();
    inline long   IFcnt() const;

    long          userUtil;

  private:

    inline Vertex(const Vec3& p);

    inline void pushHalfEdgesCCW(const EdgeIt& e);

    HalfEdgeIt nextIncompleteHalfEdge(HalfEdgeIt& hit);

    inline void setNormalFromIncidentFaces();

    inline void setId(long id);


    /** @brief coordinates in LCS of the manifold
     *         in the initial orientation
     */
    Vec3                          mPointLCS;

    /** @brief vertex normal in the initial orientation
     */
    Vec3                          mNormalLCS;

    /** @brief incident half edges in the counter-clockwise ordering
     *         facing the vertex from outside
     */
    list<HalfEdgeIt>              mIncidentHalfEdges;

    /** @brief iterator version of 'this'. */
    VertexIt                      mBackIt;

    /** @brief used in the conflict graph in findConvexHull() */
    Undirected::node_list_it_t    mVertexConflict;

    long                          mId;

    unsigned long                 mGeneration;

    /** @brief a map from face to a count. Used to find a face from
     *         three vertices.
     */
    map<long,long>                mFaceCounts;

    /** @brief mark to indicate this vertex's degree is 2 and hence
     *         must be removed.
     */
    bool                          mToBeRemoved;

    /** @brief back pointer to Manifold::mVerticesToBeRemoved */
    list<VertexIt>::iterator      mBackItVTBR;


    /** @brief temporary dot product value stored for IntersectionFinder
     */
    double                        mIFdot;

    /** @brief temporary data storage used by IntersectionFinder
     */
    enum predicate                mIFflags;

    /** @brief temporary counter stored for IntersectionFinder
     */
    long                          mIFcnt;

    /** @brief temporary counter stored for IntersectionFinder
     */
    long                          mIFcomponentId;
    long                          mIFcomponentIdaux;

  friend class Manifold;
  friend std::unique_ptr<Vertex>
         std::make_unique<Vertex, const Vec3&>(const Vec3&);
  friend std::ostream& operator<<(std::ostream& os, const Vertex& V);
  friend class IntersectionDecomposer;

};

inline std::ostream& operator<<(std::ostream& os, const Vertex& V)
{
    os << "ID: "     << V.mId        << "\t";
    os << "Point: "  << V.mPointLCS  <<  "\t";
    os << "Normal: " << V.mNormalLCS <<  "\n";
    return os;
}


class HalfEdge {

  public:


    /** @brief check the state of this  half edge relative to
     *         either of the next or the previous indicent half edge.
     *
     *  @param he : (in) the peer HalfEdge
     *
     *  @return NONE :             Not colinear
     *          MAYBE_COINCIDENT : One of the half edges is miniscule.
     *          MAYBE_COLINEAR   : Two edges are colinear.
     */
    enum predicate isColinear(const HalfEdgeIt& he);

    inline virtual ~HalfEdge();

    inline VertexIt src() const;

    inline VertexIt dst() const;

    inline HalfEdgeIt buddy() const;

    inline EdgeIt edge() const;

    inline FaceIt face() const;

    inline HalfEdgeIt prev() const;
    inline HalfEdgeIt next() const;

    inline const Vec2& textureUVsrc() const;
    inline const Vec2& textureUVdst() const;

  private:

    inline HalfEdge();

    inline void setVerticesAndEdge(
                 const VertexIt& vSrc, const VertexIt& vDst, const EdgeIt& e);


    /** @brief previous half edge along the face */
    HalfEdgeIt                            mPrev;

    /** @brief predicate relative to the previous half edge */
    enum predicate                        mPrevPred;

    /** @brief next half edge along the face */
    HalfEdgeIt                            mNext;

    /** @brief predicate relative to the next half edge */
    enum predicate                        mNextPred;

    /** @brief source vertex */
    VertexIt                              mSrc;

    /** @brief iterator into the source vertex's edge list */
    list<HalfEdgeIt>::const_iterator      mSrcBackIt;

    /** @brief destination vertex */
    VertexIt                              mDst;

    /** @brief iterator into the destination vertex's edge list */
    list<HalfEdgeIt>::const_iterator      mDstBackIt;

    /** @brief incident face */
    FaceIt                                mFace;

    /** @brief iterator into the incident face's half edge list */
    list<HalfEdgeIt>::const_iterator      mFaceBackIt;

    /** @brief Edge that owns this half edge */
    EdgeIt                                mParent;

    /** @brief the other half edge */
    HalfEdgeIt                            mBuddy;

    /** @brief iterator version of 'this'. */
    HalfEdgeIt                            mBackIt;

    /** @brief termporary flag used to mark that the incident faces
     *         should be merged into one.
     */
    bool                                  mToBeMerged;

    /** @brief ID coordinates for the texture.*/
    Vec2                                  mTextureUVsrc;
    Vec2                                  mTextureUVdst;

  friend class Vertex;
  friend class Edge;
  friend class Face;
  friend class Manifold;
  friend std::unique_ptr<HalfEdge> std::make_unique<HalfEdge>();

};


class Edge {

  public:
    inline virtual ~Edge();

    inline const Vec3& nLCS() const;
    inline Vec3  nGCS(const Mat3x3&rot) const;

    inline HalfEdgeIt he1();

    inline HalfEdgeIt he2();

    /** @brief used by GJK algorithm */
    bool   mDegenerate;

    /** @brief returns ID pair of the incident vertices
     *         such that the first is less than the second.
     */
    inline pair<long, long> id();

  private:

    inline Edge();

    inline void setNormalFromIncidentFaces();

    /** @brief ID of this edge */
    long           mId;

    /** @brief one half edge */
    HalfEdgeIt     mHe1;

    /** @brief the other half edge */
    HalfEdgeIt     mHe2;

    /** @brief predicate between two incident vertices
     *         MAYBE_COINCIDENT if they are too close.
     */
    enum predicate mPredVertices;

    /** @brief predicate between two incident faces */
    enum predicate mPredFaces;

    /** @brief iterator version of 'this'. */
    EdgeIt         mBackIt;

    /** @brief vertex normal in the initial orientation
     */
    Vec3           mNormalLCS;

    /** @brief binary counter to find an edge that has only one valid face.
     *         used in findCircumference().
     *  @remark invariant: it must be always even outside findCircumference().
     */
    long           mOddCnt;

    /** @brief marks this edge for removal to merge two incident faces into
     *         one.
     */
    bool           mToBeRemoved;


    /** @brief temporary flag. This edge has been found for processing.*/
    bool           mFound;


    /** @brief back pointer into Manifold::mEdgesToBeRemoved where
     *         the edges to be removed are chained.
     */
    list<EdgeIt>::iterator mBackItETBR;

    /** @brief temporary data storage used by IntersectionFinder
     */
    enum predicate                mIFflags;

    /** @brief temporary counter stored for IntersectionFinder
     */
    long                          mIFcnt;

    /** @brief temporary variables stored for IntersectionFinder
     */
    long                          mIFcomponentId;
    long                          mIFcomponentIdaux;

  friend class HalfEdge;
  friend class Vertex;
  friend class Face;
  friend class Manifold;
  friend class IntersectionFinder;
  friend std::unique_ptr<Edge> std::make_unique<Edge>();
  friend class IntersectionDecomposer;

};


class Face {

  public:

    inline virtual ~Face();

    /** @brief utility function to find the area of a triangle */
    inline static double triangularArea(
                               const Vec3& v1, const Vec3& v2, const Vec3& v3);


    /** @brief utility function to find a normalized normal of the triangle
     *         whose vertices are p1->p2->p3->p1 in counter-clockwise ordering.
     *
     *  @param p1 , p2 , p3 (in): vertex coordinates
     *
     *  @param pred (out): predicate found
     *
     *  @return normalized normal
     */
    inline static Vec3 findNormal(
         const Vec3& p1, const Vec3& p2, const Vec3& p3, enum predicate& pred);


    /** @brief find the surrounding halfEdges from the circularly ordered
     *         list of Edges around a face.
     *
     *  @param edges (in): circular list of edges
     *
     *  @return circular list of half edges
     */
    static list<HalfEdgeIt> halfEdgesFromEdges(const list<EdgeIt>& edges);


    /** @brief check the state of this face relative to
     *         one of the indicent faces.
     *
     *  @param f : (in) the incident face
     *
     *  @return NONE :             Not colinear
     *          MAYBE_COINCIDENT : the face is miniscule
     *          MAYBE_COPLANAR   : Two faces are coplanar within the given
     *                             tolerance
     */
    inline enum predicate isCoplanar(const FaceIt& f);


    /** @brief returns the area of this face assuming it is convex */
    inline double areaIfConvex();


    /** @brief returns true if this face is facing the given point
     *         in the manifold. Used in findConvexHull()
     *
     *  @param pred (out): MAYBE_COPLANAR if the point is coplanar
     *                     NONE otherwise
     *
     *  @return true if the point is facing this face.
     */
    inline bool isFacing(const Vec3& p, enum predicate& pred,const double eps);


    /** @brief returns true if the point is coplanar to this face.
     */
    inline bool isCoplanar(const Vec3& p);

    /** @brief integer ID of this manifold. */
    inline long  id() const;


    inline const Vec3& nLCS() const;

    inline Vec3  nGCS(const Mat3x3&rot) const;

    inline list<HalfEdgeIt>& halfEdges();

    inline long textureID() const;

    inline bool isIncident(VertexIt vit);

    inline bool isIncident(EdgeIt eit);

    long          userUtil;

  private:

    inline Face();

    inline void setId(long id);

    /** @brief integer ID of this manifold. */
    long                                   mId;

    /** @brief normalized normal in the original orientation of the manifold */
    Vec3             mNormalLCS;

    /** @brief hald edge chain along the face in counter-clockwise ordering */
    list<HalfEdgeIt> mIncidentHalfEdges;

    /** @brief prdicate of the shape of this face */
    enum predicate   mPred;

    /** @brief iterator version of 'this'. */
    FaceIt           mBackIt;

    /** @brief used in the conflict graph in findConvexHull() */
    Undirected::node_list_it_t   mFaceConflict;

    /** @brief texture ID for this face. */
    long             mTextureID;

    /** @brief temporary flag to indicate the face is facing the current vertex.
     *         Used to find the convex hull.
     */
    bool             mToBeMerged;

    /** @brief temporary data storage used by IntersectionFinder
     */
    enum predicate                mIFflags;

    /** @brief temporary counter stored for IntersectionFinder
     */
    long                          mIFcnt;

    /** @brief temporary counter stored for IntersectionFinder
     */
    long                          mIFcomponentId;


  friend class Manifold;
  friend std::unique_ptr<Face> std::make_unique<Face>();
  friend class IntersectionDecomposer;

};


class FrontierElem;
class FaceConflict;
class VertexConflict;

class Manifold : public Loggable {

  public:

    inline Manifold(std::ostream& logStream = std::cerr);
    inline virtual ~Manifold();

    /** @brief reset this manifold to the initial empty state.
     */
    inline void clear();

    /** @brief constructs a tetrahedron (3-simplex) based on the given 4 points
     *         in LCS in an arbitrary ordering.
     *
     *
     *  @param p1 -4  (in): point 1-4 in LCS.
     *
     *
     *                y
     *                ^
     *                |
     *                *2
     *               /|\
     *               ||  \
     *              / |    \
     *              | |      \
     *             /  |        \
     *             |  *---------*---------> x
     *            /  /4     __/ 1
     *            | /    __/
     *            //  __/
     *           |/__/
     *           //
     *         3*
     *         /
     *        /
     *       z
     *
     */
    void construct3Simplex(
        const Vec3& p1,
        const Vec3& p2,
        const Vec3& p3,
        const Vec3& p4,
        const long  ind1=0,
        const long  ind2=1,
        const long  ind3=2,
        const long  ind4=3
    );

    /** @brief constructs a cuboid (rectangular solid) based on the given 8
     *         points in LCS in the specific ordering.
     *
     *                   Y
     *                   ^
     *                   |
     *                   |
     *                   |backUpperLeft         backUpperRight
     *                   *---------------------*
     *                  /|                    /|
     *                 / |                   / |
     *                /  |                  /  |
     *               *---+-----------------*   |
     * frontUpperLeft|   |  frontUpperRight|   |
     *               |   |                 |   |
     *               |   *-----------------+---*--------->X
     *               |  /backLowerLeft     |  /backLowerRight
     *               | /                   | /
     *               |/                    |/
     *               *---------------------*
     *              /frontLowerLeft         frontLowerRight
     *             /
     *            /
     *           Z
     */
    void constructCuboid(
        const Vec3& frontLowerLeft,
        const Vec3& frontUpperLeft,
        const Vec3& frontUpperRight,
        const Vec3& frontLowerRight,
        const Vec3& backLowerLeft,
        const Vec3& backUpperLeft,
        const Vec3& backUpperRight,
        const Vec3& backLowerRight
    );


    /** @brief constructs the convex hull of the given points as a manifold.
     *         It uses a randomized algorithm whose expected running time
     *         is O(n*log(n)).
     *
     *  @param points  (in):  the points.
     *
     *  @param pred    (out): predicate to specify any degeneracy found.
     *
     *  @param epsilon (in):  numerical margin used for predicates.
     *
     *  @reference "Computational Geometry Algorithms and APplications"
     *             M. de Berg, M. van Kreveld, M. Overmars, and O.Schwartzkopf
     *             2nd Ed, Springer 2000, ISBN 3-540-65620-0
     */
    void findConvexHull(
        vector<Vec3>&   points,
        vector<long>&   indices,
        enum predicate& pred,
        const double    epsilon = EPSILON_SQUARED
    );

    void findConvexHull(
        vector<Vec3>&   points,
        enum predicate& pred,
        const double    epsilon = EPSILON_SQUARED
    );

    inline EdgeIt findEdge(const VertexIt& vit1, const VertexIt& vit2);

    inline FaceIt findFace(const VertexIt& vit1, const VertexIt& vit2);

    inline FaceIt findFace(
             const VertexIt& vit1, const VertexIt& vit2, const VertexIt& vit3);

    inline long id() const;

    inline void setId(long id);

    inline vector<Vec3> getPointsLCS();

    inline vector<Vec3> getFaceNormalsOriginal();

    inline pair<VertexIt, VertexIt> vertices();

    inline pair<EdgeIt,   EdgeIt>   edges();

    inline pair<FaceIt,   FaceIt>   faces();

    inline VertexIt vertexIt(long id);
    inline EdgeIt   edgeIt  (const pair<long,long>& id);
    inline FaceIt   faceIt  (long id);

    /** @brief reset generations of all the Vertices to zero.
     */
    inline void resetGen();

    class Martialled {
      public:
        long                            mId;
        std::map<long, Vec3>            mPoints;
        std::map<long, Vec3>            mNormals;
        std::map<long, vector<long> >   mIncidentVertices;
        std::map<pair<long,long>, Vec3> mEdgeNormals;
        std::map<long, Vec3>            mFaceNormals;
        std::map<long, vector<long> >   mFaceVertices;
    };

    /** @brief export the manifold structure.
     */
    Martialled exportData();

    void importData(Martialled& m);

    /** @brief parses the input text stream and generates
     *         the martialled data structure to be imported.
     */
    static Martialled parseTextData(std::istream& is);

    static void emitText(Martialled& M, std::ostream& os);

    void logContents(
        enum LogLevel lvl,
        const char*   _file,
        const int     _line
    );

    void logConflictGraph(
        enum LogLevel lvl,
        const char*   _file,
        const int     _line
    );

    void logVertexConflict(
        enum LogLevel   lvl,
        const char*     _file,
        const int       _line,
        VertexConflict& VC
    );

  private:

    /** @brief convenience function to generate a face of polygon from its
     *         surrounding edges ordered counter-clockwise order.
     */
    inline FaceIt makePolygon (const list<EdgeIt>& edges);


    /** @brief convenience function to generate a face of polygon from its
     *         surrounding half edges ordered counter-clockwise order.
     */
    FaceIt makePolygon (const list<HalfEdgeIt>& halfEdges);


    /** @brief convenience function to generate a face of triangle from its
     *         surrounding edges ordered counter-clockwise order and the
     *         normal is pointing toward you.
     */
    inline FaceIt makeTriangle(
             const HalfEdgeIt& e1, const HalfEdgeIt& e2, const HalfEdgeIt& e3);


    /** @brief convenience function to generate a face of triangle from its
     *         surrounding edges ordered counter-clockwise order.
     */
    inline FaceIt makeTriangle(
             const EdgeIt& e1, const EdgeIt& e2, const EdgeIt& e3);

    /** @brief convenience function to generate a face of quadrilateral from
     *         its surrounding edges ordered counter-clockwise order.
     *
     */
    inline FaceIt makeQuad(
       const EdgeIt& e1, const EdgeIt& e2, const EdgeIt& e3, const EdgeIt& e4);


    /** @brief find the closed cycle of half edges for the set of connected
     *         faces.
     *
     *   'f' indicates            The cyclic half edges
     *   the set of faces         found.
     *
     *    *---*--*---*            *---*--*---*
     *    |\f |  |f / \           |           \
     *    | \ |f | /  |           |           |
     *    |f \|  |/ f |           |           |
     *    *-- *--*----*           *           *
     *    |f  |f/\  f |           |           |
     *    |   |/ f|   |     ==>   |           |
     *    *---*---*---*           *           *
     *    |f /|   | f/            |          /
     *    | / | f | /             |         /
     *    |/ f|   |/              |        /
     *    *---*---*               *---*---*
     */
    vector<HalfEdgeIt> findCircumference(vector<FaceIt>& faces, bool& abort);


    /** @brief helper function to findCircumference() */
    HalfEdgeIt findNextBoundaryHalfEdge(HalfEdgeIt startIt);


    /** @brief adds a series of circularly arranged triangles centered at the
     *         given point and around the circularly ordered half edges.
     *
     *            he3
     *        <----------         <----------
     *       |           ^       |\         /^
     *       |           |       |  \     /  |
     *       |           |       |    \ /    |
     *       |     *     |  ==>  |     *     |
     *       |   center  |       |    /c\    |
     *       |           |he2    |  /     \  |
     *    he4|           |       |/         \|
     *        ---------->         ---------->
     *            he1
     *
     *   @param halfEdges   (in): list of half edges ordered clockwise if you
     *                            are looking at them from outside of the
     *                            manifold from the center point.
     *
     *   @param p           (in): center point of the fan in LCS.
     *
     *   @return the new center vertex created.
     */
    VertexIt makeCircularFan(
        const vector<HalfEdgeIt>& halfEdges,
        const Vec3&               p,
        const long                id = -1
    );


    /** @brief removes the specified faces from this manifold.
     *
     *  @param  faces (in): a set of faces that form a closed continuous
     *                      surface (2-manifold).
     *
     *  @remark if an edge has lost both of its incident faces, then it will
     *          be removed, and subsequently if a vertex has lost all of its
     *          incident edges, then it will be removed from the graph.
     */
    void removeFaces(vector<FaceIt>& faces);


    /** @brief removes the specified edge from this manifold.
     *
     *  @param  e (in): the edge removed
     *
     *  @remark if a vertex has lost all of its incident edges, then it will
     *          be removed from the graph.
     */
    void removeEdge(EdgeIt& e);

    /** @brief creates and adds a new vertex to the manifold for the given
     *         point in LCS.
     *
     *  @param    p (in): the point in LCS
     *
     *  @return the iterator to the vertex created.
     */
    inline VertexIt makeVertex(const Vec3& p, const long id = -1);


    /** @brief creates and adds a new edge for the given incident vertices.
     *
     *  @param    v1 (in): the incident vertex 1.
     *
     *  @param    v2 (in): the incident vertex 2.
     *
     *  @return the iterator to the edge created.
     */
    inline EdgeIt   makeEdge(const VertexIt& v1, const VertexIt& v2);


    /** @brief set the normals of vertices and edges from their incident
     *         face normals.
     */
    inline void setNormalsForVerticesAndEdges();


    /** @brief split a line into fields by the specified delimiter.
     */
    static std::vector<std::string> splitLine(const string&txt, const char&ch);

    static std::vector<std::string> processLine(std::istream& is);

    /** @brief constructs Vertex::mFaceCounts and Manifold::mVertexPairToEdge*/
    void constructHelperMaps();

    /** @brief set the 2D texture coordinates for the faces.
     *         The 2D texture coordinates are set to HalfEdge::mTextureUVsrc
     *         and HalfEdge::mTextureUVdst.
     *         The application can later scale and rotate them for each face
     *         to suit their needs.
     */
    void constructDefaultTextureCoordinates();

    /** @brief find the face incident to the given three vertices */
    FaceIt findFace(VertexIt& v1, VertexIt& v2, VertexIt& v3, bool& found);


    FaceIt findFaceOrdered(
                    VertexIt& v1, VertexIt& v2, VertexIt& v3, bool& found);



    /** @brief subroutine for findConvexHull()
     *
     *         it performns principal component analysis on the given points
     *         and find 4 extremal points to make a good initial 3-simplex
     *         as the starting manifold to find the convex hull.
     *
     *  @param points (in): points in 3 space. There must be at least 4 points.
     *
     *  @param index1 (out): index into points for 1st extremal point
     *
     *  @param index2 (out): index into points for 2nd extremal point
     *
     *  @param index3 (out): index into points for 3rd extremal point
     *
     *  @param index4 (out): index into points for 4th extremal point
     *
     *  @return predicate found during the analysis.
     */
    enum predicate analyzePoints(vector<Vec3>& points,
                  size_t& index1,size_t& index2,size_t& index3,size_t& index4);


    /** @brief subroutine for findConvexHull()
     *
     *         creates the initial bi-partite conflict graph between the faces
     *         of the initial 3-simplex and the given points for the convex
     *         hull finding algorithm. The graph is created into
     *         mConflictGraph.
     *
     *  @param  points   (in):  the set of points in LCS.
     *
     *  @param  vertices (out): set of vertices in the conflict graph that
     *                          represents the given points.
     */
    void createInitialConflictGraph(
        vector<Vec3>&                       points,
        vector<long>&                       indices,
        vector<Undirected::node_list_it_t>& vertices
    );


    /** @brief subroutine for findConvexHull()
     *
     *         It finds the set of faces that are visible to the given vertex
     *         based on the conflicg graph.
     *         It performs an extra check: If the vertex is too close to
     *         one of the features of a face, then it is considered degenerate
     *         and return true (abort).
     *
     *  @param  vc       (in):       the target vertex to be tested.
     *
     *  @param  conflictFaces (out): the set of visible faces.
     *
     *  @return true  : abort. The vertex too close to a face.
     *          false : continue processing the vertex.
     */
    bool findVisibleFaces(
        VertexConflict& vc,
        vector<FaceIt>& conflictFaces
    );


    /** @brief subroutine for findConvexHull()
     *
     *         It checks if the set of faces visible to the target vertex
     *         form a closed compact 2-manifold (i.e., the faces are all
     *         connected by an edge and they have no holes but one closed
     *         boundary.
     *         If so, then it finds the bounddary (circumference) half edges
     *         facing inward of the manifold (toward the vertex).
     *         For each half edge, it creates FrontierElem, which contains
     *         the set of visible vertices from the incident face of the half
     *         edge and the face of the buddy (the opposite half edge).
     *         This vertex set is a superset of of the vertices visible from
     *         the new face (triangle) to be created next by the half edge
     *         and the target vertex.
     *         Then it creates a circular fan of triangles around the the
     *         boundary half edges with the center as the target vertex.
     *
     *  @param  p             (in): the target vertex to be tested.
     *  @param  conflictFaces (in): the set of visible faces.
     *  @param  frontier      (out):the set of FrontierElem for the boundary
     *                              half edges.
     *  @param  abort         (out):the set of faces are not closed or compact
     *
     *  @return the center vertex for the circular fan.
     */
    VertexIt updateFaces(
        const Vec3&           p,
        const long            id,
        vector<FaceIt>&       conflictFaces,
        vector<FrontierElem>& frontier,
        bool&                 abort
    );


    /** @brief subroutine for findConvexHull()
     *
     *         It creates new face nodes in the conflict graph for the faces
     *         newly created. The new faces are discovered from the boundary
     *         half edges. The pointers to the boundary half edges are stored
     *         in the FrontierElems in the parameter frontier.
     *
     *  @param  frontier (in/out):: the target vertex to be tested.
     */
    void updateConflictGraph(vector<FrontierElem>& frontier);


    /** @brief subroutine for findConvexHull()
     *
     *         It removes all the nodes and edges from mConflictGraph.
     *         This is part of the clearning up process of findConvexHull.
     */
    void clearConflictGraph();


    /** @brief subroutine for findConvexHull()
     *
     *         It tests if the target vertex is too close to one of the edges
     *         of the given face, which is supposed to be visible from the
     *         vertex.
     *         This is to avoid degenerate (super thin or miniscule)
     *
     *  @param  pTest (in): the target vertex
     *
     *  @param  fit (in): the face whose edges are tested.
     *
     *  @return true : the vertex is too close to an edge.
     *          false: otherwise
     */
    bool vertexIsTooCloseToFace(const Vec3& pTest, FaceIt fit);


    /** @brief subroutine for findConvexHull()
     *
     *         tests if two adjacent faces are in parallel or facing toward
     *         each other, in which case those two should be merged.
     *
     *  @param    e (in): the incident edge between two faces to be tested
     *
     *            v2
     *             *
     *  ^        /| |         ^        ^
     *  |         | |         |        |
     *  |         | |         |        |
     *  +  f1  he1|e|he2  f2  +        |
     * n1         | |        n2       v12
     *            | |
     *            | |/
     *             *
     *             v1
     *
     *      n1xn2
     *    ^   ^   ^                  ^     ^
     *     \  |  /                    \   /
     *    n1\ | / n2                 n2\ /n1
     *       \|/                        |
     *                                  v n1xn2
     *
     *      convex                    concave
     *  The two faces are          The two faces are
     *     facing away             facing each other
     *
     *  @return true  : if two are parallel or concave, and should be merged.
     *          false : the two faces are convex.
     */
    bool areIncidentFacesParallelOrConcave(EdgeIt e);


    /** @brief subroutine for findConvexHull()
     *
     *         tests if the angle of two adjacent half edges of a face
     *         is less than pi or not.
     *
     *
     *             ....--*                        ...---*<-----+
     *                   ^                                     |
     *             .     |                        .            |
     *             .     |heCW                    .    *------>*
     *             |     |                        .    ^ heCW
     *             v     |                        |    |
     *             *---->*                        |    |heCCW
     *              heCCW                         *----+
     *
     *       The face is convex.              The face is not convex.
     *     The angle the two half             The angle the two half
     *     edges make is less than pi.        edges make is greater than pi.
     *
     *  @return   true   : if the angle is less than or nearly 180.
     *            false  : if the angle is greater than 180 with some margin.
     */
    bool isNewFaceAreawiseConvex(
        HalfEdgeIt  heCW,
        HalfEdgeIt  heCCW,
        const Vec3& n
    );


    /** @brief subroutine for findConvexHull()
     *
     *         find and the set of points in the vertices of the conflict
     *         graph that may face the new face to be created at each of the
     *         half edges along the frontier.
     *
     *  @param  halfEdges  (in): circular list of half edges that represent
     *                           the frontier.
     *
     *  @return list of FrontierElems, each of which contains the frontier
     *          half edge and its associated points that may face the new face.
     */
    vector<FrontierElem> makeFrontier(vector<HalfEdgeIt>& halfEdges);


    /** @brief subroutine for findConvexHull()
     *         subroutine for removeEdgeAndMergeFaces()
     *
     *         it marks the vertices that should be subsequently merged
     *         as a result of merging two faces at an edge.
     *         Merging two adjacent faces removes the common indicent
     *         edge (s,t), which in turn may make s or t degree 2 or 1..
     *
     *         If a vertex s becomes degree 2, and there are {v,s} and {s,w},
     *         then we should remove s and make {v,w}. we mark s and {v,w}
     *         as to-be-removed. Removal of s and addition of {v,w} may
     *         make a new face of 2-cycle {(v,w),(w,v}, which should be removed
     *         In this case, we mark {v,w} as to-be-removed as well.
     *
     *         If a vertex s becomes degree 1, and there is {v,s}, then
     *         then we should remove s and {v,s}. We immediately remove s and
     *         {v,s}. Removal of {v,s} will make v degree 2 or 1.
     *         In case of degree 2, we mark v as to-be-removed.
     *         In case of degree 1, we remove v and {w, v} immediately.
     *         This in turn may make w degree 2 or 1. The same cycle is
     *         repeated untill there are no more vertices or edges to be
     *         processed.
     *
     *  @param  vit (in): to be filled
     *
     */
    void removeEdgeAndMergeFacesCheckForUpdate(VertexIt vit);


    /** @brief subroutine for findConvexHull()
     *
     *         It removes the edges chained on mEdgesToBeRemoved, and merges
     *         the incident faces at those edges.
     *         It also handles the side effects as a consequence of removing
     *         edges as described in the comments of
     *         removeEdgeAndMergeFacesCheckForUpdate().
     */
    bool removeEdgeAndMergeFaces(EdgeIt eit);


    /** @brief subroutine for findConvexHull()
     *
     *         It removes the vertices of degree 2. Removal of such vertices
     *         may create faces of 2-cycles on either or both sides.
     *         Such faces must be merged to their incident faces.
     *         Care must be taken on which adjacent incident face to choose
     *         for merging. The face with more incident half edges must be
     *         chosen.
     */
    void removeVertexAndMergeEdges(VertexIt vit);


    /** @brief subroutine for findConvexHull()
     *
     *         It merges the connected faces arranged as in a simple path
     *         or cycle into one with associated face conflicts in the
     *         conflict graph.
     *
     *  @param faces (in): ordered list of consecutive faces to be merged.
     */
    void mergeConsecutiveFaces(vector<FaceIt>& faces);


    /** @brief subroutine for findConvexHull()
     *
     *         It handles the co-planar edges, non-convex polygons,
     *         vertex of degree 2, and 2-cycle faces after an update
     *         around the frontier half edges.
     *
     *         1. It scans the newly created circular fan around the
     *            frontier half edges and the center vertex, check for
     *            concavity, and merge consecutively concave or coplanar
     *            faces into one. If any of the merged faces become concave,
     *            then the entire fan becomes a single convex polygon and
     *            the center vertex is removed.
     *            If the center vertex becomes degree 2, then it is removed.
     *
     *         2. It checks for anomalies around the frontier edges in terms
     *            of adjacent concave face pairs, deg2 & deg1 vertices, and
     *            2-cycle faces.
     *
     *         3. As a result, the updated manifold should be proper:
     *            closed compact tri-connected convex 3-manifold without
     *            2-cycles.
     *
     *  @param  center          (in): center vertex of the frontier
     *
     */
    void checkAndMergeFacesCounterClockwise(VertexIt center);


    /** @brief subroutine for findConvexHull()
     *
     *         Starting from the two adjacent coplanar faces incident to the
     *         edge specified by eit, collect all the coplanar faces reachable
     *         from those two.
     *
     *  @param eit  (in): the starting edge whose adjacent faces are coplanar.
     */
    vector<FaceIt> gatherConnectedFaces(EdgeIt eit);


    /** @brief subroutine for findConvexHull()
     *
     *  @param   halfEdges       (in):  the ordered list of half edges for
     *                                  the new face ordered counter clockwise
     *
     *  @param   additionalFaces (out): set of coplanar adjacent faces found.
     *
     *  @return  true  : aborted as it has found an ill-condition:
     *                   the face (polygon) specified by halfEdges has
     *                   a dent (concavity) but the adjacent face is not
     *                   coplanar. It should not happen and exit.
     *
     *           false : no anomalies detected.
     */
    bool checkForConcavity(
        vector<HalfEdgeIt>& halfEdges,
        vector<FaceIt>&     additionalFaces
    );

    /** @brief subroutine for findConvexHull()
     *
     *         Scan the inner edges of the connected faces specified by
     *         'faces' and check if they are marked (mToBeRemoved) and
     *         chained to mEdgesToBeRemoved. If it finds such an edge, it
     *         removes it from mEdgesToBeRemoved. Those inner edges are
     *         removed entirely when the faces are replaces with a single new
     *         face.
     *
     * @param  faces     (in): set of connected faces.
     *
     * @param  halfEdges (in): circumference of the connected faces. Those
     *                         specify the boundary edges as opposed to the
     *                         inner edges.
     */
    void findInnerEdgesAndRemoveFromChain(
        vector<FaceIt>     faces,
        vector<HalfEdgeIt> halfEdges
    );


    /** @brief integer ID of this manifold. */
    long                                   mId;

    /** @brief Vertices in this manifold */
    list<unique_ptr<Vertex> >              mVertices;

    /** @brief Edges in this manifold */
    list<unique_ptr<Edge> >                mEdges;

    /** @brief HalfEdges in this manifold */
    list<unique_ptr<HalfEdge> >            mHalfEdges;

    /** @brief Faces in this manifold */
    list<unique_ptr<Face> >                mFaces;

    /** @brief Number of faces in this manifold */
    long                                   mNumFaces;

    /** @brief predicate on the shape of this manifold */
    enum predicate                         mPred;

    /** @brief iterator version 'this'. */
    ManifoldIt                             mBackIt;

    /** @brief conflict graph used to find the convex hull. */
    Directed::DiGraph                      mConflictGraph;

    /** @brief next number to be assigned to a newly created feature */
    long                                   mNextIdForFeatures;

    /** @brief a map from a vertex pair to incident edge */
    map<pair<long,long>,EdgeIt>            mVertexPairToEdge;

    /** @brief a map from a vertex ID to VertexIt */
    map<long,VertexIt>                     mVertexIdToVertex;

    /** @brief a map from a vertex ID to EdgeIt */
    map<pair<long,long>,EdgeIt>            mEdgeIdToEdge;

    /** @brief a map from a face ID to FaceIt */
    map<long,FaceIt>                       mFaceIdToFace;

    /** @brief used as the margin to test predicates
     *         when convex hull is generated.
     */
    double                                 mEpsilonCHMargin;

    /** @brief the edges to be removed are chained here.
     *         Used to merge two coplanar faces and remove 2-cycle faces.
     */
    list<EdgeIt>                           mEdgesToBeRemoved;

    /** @brief the vertices to be removed are chained here.
     *         Used to remove deg-2 vertices.
     */
    list<VertexIt>                         mVerticesToBeRemoved;


#ifdef UNIT_TESTS

    /*
     * =============================================================
     *  Following functions are used for step-by-step processing in
     *  test_visualizer_manifold_convex_hull
     * =============================================================
     */

  public:


    /** @brief prints the number of FaceConflicts and VertexConflicts in
     *         mConflictGraph
     */
    void debugNumNodes();

    /** @brief prints the number of FaceConflicts incident to the given
     *         VertexConflict.
     *
     *  @param  nit (in): the VertecConflict to be examined
     */
    void debugNumFaces(Undirected::node_list_it_t nit);


    /** @brief constructs the initial 3-simplex and the conflict graph
     *
     *  @param points (in) points for which the convex hull is to be found
     */
    void debugFindConvexHullStep1(vector<Vec3>& points);

    /** @brief returns the vertices of the conflict graph for the points
     */
    vector<Undirected::node_list_it_t> debugFindConvexHullVertices();


    /** @brief initializes the iteration for the given vertex, and find the
     *         frontier.
     *
     *  @return false if the given vertex does not have any facing faces.
     *          true if a frontier is found.
     */
    bool debugFindConvexHullLoopStep1(Undirected::node_list_it_t vcit);

    /** @brief removes the faces inside the frontier
     *
     *  @return false if the given point is degenerate and absorbed in the
     *          current frontier.
     *          true if the faces are removed from the convex hull
     */
    bool debugFindConvexHullLoopStep2();


    /** @brief create new faces along the frontier and the current point as
     *         their center and update the conflict graph
     */
    void debugFindConvexHullLoopStep3();


    /** @brief terminates the function of findConvexHull()
     *         It removes all the nodes from mConflictGraph
     */
    void debugFindConvexHullTerm();


    /** @brief returns the point for the current vertex to be examined.
     *
     *  @param vcit (in): the current vertex
     *
     *  @return point of the vertex.
     */
    Vec3 debugCurrentPoint(Undirected::node_list_it_t vcit);

    /** @brief generates vertices and colors for the lines along the
     *         current frontier for Open GL's GL_LINES.
     *
     *  @param color (in): color of the lines
     *
     *  @param vertices (out): array of points for GL_LINES
     *
     *  @param colors   (out): array of colors for GL_LINES
     */
    void debugFindConvexHullHalfEdges(
        Vec3&               color,
        vector<Vec3>&       vertices,
        vector<Vec3>&       colors
    );

    /** @brief generates arrays for vertices, colors, and normals
     *         for the faces of the current convex hull.
     *         Those are for OpenGL's GL_TRIANGLES
     *
     *  @param color  (in):  color of this manifold
     *
     *  @param vertices (out): vertices of triangles
     *
     *  @param colors   (out): colors of triangles
     *
     *  @param normals  (out): normals of triangles
     */
    void makeOpenGLVerticesColorsNormalsForTriangles(
        Vec3&         color,
        vector<Vec3>& vertices,
        vector<Vec3>& colors,
        vector<Vec3>& normals,
        bool          bothSides
    );

    /** @brief generates arrays for vertices, colors, and normals
     *         for the faces of the current convex hull.
     *         Those are for OpenGL's GL_TRIANGLES
     *
     *  @param color  (in):  color of this manifold
     *
     *  @param vertices (out): vertices of triangles
     *
     *  @param colors   (out): colors of triangles
     *
     *  @param normals  (out): normals of triangles
     *
     *  @param rot      (in): rotation matrix to GCS
     *
     *  @param CoM      (in): CoM in GCS
     */
    void makeOpenGLVerticesColorsNormalsForTriangles(
        Vec3&         color,
        vector<Vec3>& vertices,
        vector<Vec3>& colors,
        vector<Vec3>& normals,
        bool          bothSides,
        const Mat3x3& rot,
        const Vec3&   CoM
    );

    void makeOpenGLVerticesColorsNormalsForTriangles(
        Vec3&         color,
        vector<Vec3>& vertices,
        vector<Vec3>& colors,
        vector<Vec3>& normals,
        bool          bothSides,
        const double& scaling,
        const Mat3x3& rot,
        const Vec3&   CoM
    );


    void makeOpenGLVerticesColorsNormalsForTriangles(
        Vec3&          color,
        float          alpha,
        vector<Vec3>&  vertices,
        vector<Vec3>&  colors,
        vector<float>& alphas,
        vector<Vec3>&  normals,
        bool           bothSides
    );



    /** @brief generates arrays for vertices, colors, and normals
     *         for the faces of the current convex hull.
     *         Those are for OpenGL's GL_TRIANGLES
     *
     *  @param color    (in):  color of this manifold
     *
     *  @param alpha    (in):  alpha of the manifold.
     *
     *  @param vertices (out): vertices of triangles
     *
     *  @param colors   (out): colors of triangles
     *
     *  @param alphas   (out): alphas of triangles
     *
     *  @param normals  (out): normals of triangles
     *
     *  @param rot      (in):  rotation matrix to GCS
     *
     *  @param CoM      (in):  CoM in GCS
     */
    void makeOpenGLVerticesColorsNormalsForTriangles(
        Vec3&          color,
        float          alpha,
        vector<Vec3>&  vertices,
        vector<Vec3>&  colors,
        vector<float>& alphas,
        vector<Vec3>&  normals,
        bool           bothSides,
        const Mat3x3&  rot,
        const Vec3&    CoM
    );

    void makeOpenGLVerticesColorsNormalsForTriangles(
        Vec3&          color,
        float          alpha,
        vector<Vec3>&  vertices,
        vector<Vec3>&  colors,
        vector<float>& alphas,
        vector<Vec3>&  normals,
        bool           bothSides,
        const double&  scaling,
        const Mat3x3&  rot,
        const Vec3&    CoM
    );

    void makeOpenGLVerticesColorsNormalsForTriangles(
        FaceIt        fit,
        Vec3&         color,
        float         alpha,
        vector<Vec3>& vertices,
        vector<Vec3>& colors,
        vector<float>&alphas,
        vector<Vec3>& normals,
        bool          bothSides,
        const Mat3x3& rot,
        const Vec3&   CoM
    );

    void makeOpenGLVerticesColorsNormalsForTriangles(
        FaceIt        fit,
        Vec3&         color,
        float         alpha,
        vector<Vec3>& vertices,
        vector<Vec3>& colors,
        vector<float>&alphas,
        vector<Vec3>& normals,
        bool          bothSides,
        const double& scaling,
        const Mat3x3& rot,
        const Vec3&   CoM
    );

    /** @brief generates arrays for vertices, and colors
     *         for the lines of the given circular half edges.
     *         Those are for OpenGL's GL_LINES
     *
     *  @param halfEdges (in): circular list of half edges for the frontier
     *
     *  @param color  (in):  color of this manifold
     *
     *  @param vertices (out): vertices of the half edges
     *
     *  @param colors   (out): colors of the half edges
     */
    static void makeOpenGLVerticesColorsForLines(
        vector<HalfEdgeIt>& halfEdges,
        Vec3&               color,
        vector<Vec3>&       vertices,
        vector<Vec3>&       colors
    );


    static void makeOpenGLVerticesColorsForLines(
        vector<HalfEdgeIt>& halfEdges,
        Vec3&               color,
        vector<Vec3>&       vertices,
        vector<Vec3>&       colors,
        const Mat3x3&       rot,
        const Vec3&         CoM
    );

    static void makeOpenGLVerticesColorsForLines(
        vector<HalfEdgeIt>& halfEdges,
        Vec3&               color,
        vector<Vec3>&       vertices,
        vector<Vec3>&       colors,
        const double&       scaling,
        const Mat3x3&       rot,
        const Vec3&         CoM
    );


    /** @brief generates arrays for vertices, and colors
     *         for the given points.
     *         Those are for OpenGL's GL_POINTS
     *
     *  @param halfEdges (in): circular list of half edges for the frontier
     *
     *  @param color  (in):  color of this manifold
     *
     *  @param vertices (out): vertices of the points
     *
     *  @param colors   (out): colors of the points
     */
    static void makeOpenGLVerticesColorsForPoints(
        vector<Vec3>& points,
        Vec3&         color,
        vector<Vec3>& vertices,
        vector<Vec3>& colors
    );



    void makeOpenGLVerticesColorsForTriangleWireFrame(
        Vec3&         color,
        vector<Vec3>& vertices,
        vector<Vec3>& colors
    );


    void makeOpenGLVerticesColorsForTriangleWireFrame(
        Vec3&         color,
        vector<Vec3>& vertices,
        vector<Vec3>& colors,
        const double& scaling,
        const Mat3x3& rot,
        const Vec3&   CoM
    );

#endif //UNIT_TESTS

};


inline Vertex::Vertex(const Vec3& p):mPointLCS(p),mToBeRemoved(false){;}


inline Vertex::~Vertex(){;}


inline Vec3& Vertex::pLCS(){ return mPointLCS; }


inline Vec3 Vertex::pGCS(const Mat3x3& rot)
{
    return rot * mPointLCS;
}

inline Vec3 Vertex::pGCS(const Mat3x3& rot, const Vec3& CoM)
{
    return (rot * mPointLCS) + CoM;
}

inline Vec3 Vertex::pGCS(
    const double  scale,
    const Mat3x3& rot,
    const Vec3&   CoM
) {
    return (rot * (mPointLCS * scale)) + CoM;
}

inline const Vec3& Vertex::nLCS() const
{
    return mNormalLCS;
}

inline Vec3  Vertex::nGCS(const Mat3x3&rot) const
{
    return rot * mNormalLCS;
}


inline const list<HalfEdgeIt>& Vertex::halfEdges(){return mIncidentHalfEdges;}


inline void Vertex::pushHalfEdgesCCW(const EdgeIt& e) {

    auto& he1 = *((*e)->mHe1);
    auto& he2 = *((*e)->mHe2);

    if (he1->mSrc == mBackIt)  {

        he2->mDstBackIt = mIncidentHalfEdges.insert(
                                         mIncidentHalfEdges.end(), (*e)->mHe2);


        he1->mSrcBackIt = mIncidentHalfEdges.insert(
                                         mIncidentHalfEdges.end(), (*e)->mHe1);

    }
    else {

        he1->mDstBackIt = mIncidentHalfEdges.insert(
                                         mIncidentHalfEdges.end(), (*e)->mHe1);


        he2->mSrcBackIt = mIncidentHalfEdges.insert(
                                         mIncidentHalfEdges.end(), (*e)->mHe2);

    }
}


inline void Vertex::setNormalFromIncidentFaces()
{
    Vec3 sumNormals(0.0, 0.0, 0.0);

    for (auto heit : mIncidentHalfEdges) {
        if ((*heit)->mSrc == mBackIt) {
            sumNormals += (*((*heit)->mFace))->nLCS();
        }
    }

    mNormalLCS = sumNormals;
    mNormalLCS.normalize();
}

inline void   Vertex::resetGen()
    { mGeneration = 0; }

inline void   Vertex::updateGen(unsigned long newGen)
    { mGeneration = newGen; }

inline unsigned long Vertex::gen()
    { return mGeneration; }

inline bool   Vertex::isGenUpToDate(unsigned long gen)
    { return mGeneration >= gen; }

inline long   Vertex::id()
    { return mId; }

inline void   Vertex::setId(long id)
    { mId = id; }

inline void   Vertex::IFsetDot(const double& dot)
    { mIFdot = dot; }

inline double Vertex::IFdot() const
    { return mIFdot; }

inline void   Vertex::IFreset()
    { mIFflags = NONE; mIFcnt = 0;}

inline void   Vertex::IFsetActive()
    { mIFflags = IF_ACTIVE; }

inline bool   Vertex::IFisActive() const
    { return mIFflags == IF_ACTIVE; }

inline void   Vertex::IFsetProcessed()
    { mIFflags = IF_PROCESSED; }

inline bool   Vertex::IFisProcessed() const
    { return mIFflags == IF_PROCESSED; }

inline void   Vertex::IFincrement()
    { mIFcnt++; }

inline long   Vertex::IFcnt() const
    { return mIFcnt; }

inline HalfEdge::HalfEdge():
    mPrevPred(NONE), mNextPred(NONE), mToBeMerged(false){;}

inline void HalfEdge::setVerticesAndEdge(
                   const VertexIt& vSrc, const VertexIt& vDst, const EdgeIt& e)
{
    mSrc = vSrc;
    mDst = vDst;
    mParent = e;
}

HalfEdge::~HalfEdge(){;}

inline VertexIt HalfEdge::src() const { return mSrc; }

inline VertexIt HalfEdge::dst() const { return mDst; }

inline HalfEdgeIt HalfEdge::prev() const { return mPrev; }

inline HalfEdgeIt HalfEdge::next() const { return mNext; }

inline HalfEdgeIt HalfEdge::buddy() const { return mBuddy; }

inline enum predicate HalfEdge::isColinear(const HalfEdgeIt& he)
{
    if ( ((*mParent)->mPredVertices == MAYBE_COINCIDENT) ||
         ((*((*he)->mParent))->mPredVertices == MAYBE_COINCIDENT)    ) {

        return MAYBE_COINCIDENT;

    }

    auto d1 = (*mDst)->pLCS() - (*mSrc)->pLCS();
    auto d2 = (*((*he)->mDst))->pLCS() - (*((*he)->mSrc))->pLCS();

    auto cr = d1.cross(d2);

    if (cr.norm2() < EPSILON_ANGLE) {
        return MAYBE_COLINEAR;
    }

    return NONE;
}

inline EdgeIt HalfEdge::edge() const { return mParent; }

inline FaceIt HalfEdge::face() const { return mFace; }

inline const Vec2& HalfEdge::textureUVsrc() const { return mTextureUVsrc;}

inline const Vec2& HalfEdge::textureUVdst() const { return mTextureUVdst;}

inline Edge::Edge():
    mPredVertices(NONE),
    mPredFaces(NONE),
    mOddCnt(0),
    mToBeRemoved(false),
    mFound(false){;}

inline Edge::~Edge(){;}


inline const Vec3& Edge::nLCS() const
{
    return mNormalLCS;
}


inline Vec3  Edge::nGCS(const Mat3x3&rot) const
{
    return rot * mNormalLCS;
}


inline HalfEdgeIt Edge::he1() { return mHe1; }


inline HalfEdgeIt Edge::he2() { return mHe2; }


inline void Edge::setNormalFromIncidentFaces()
{
    auto& v1 = (*((*mHe1)->mFace))->nLCS();
    auto& v2 = (*((*mHe2)->mFace))->nLCS();
    mNormalLCS = v1 + v2;
    mNormalLCS.normalize();
}


inline pair<long, long> Edge::id()
{
    auto id1 = (*((*mHe1)->src()))->id();
    auto id2 = (*((*mHe1)->dst()))->id();
    if (id1<id2) {
        return make_pair(id1, id2);
    }
    else {
        return make_pair(id2, id1);
    }
}


inline double Face::triangularArea(
                                const Vec3& v1, const Vec3& v2, const Vec3& v3)
{
    auto d1 = v2 - v1;
    auto d2 = v3 - v1;
    auto cr = d1.cross(d2);
    return 0.5 * fabs(cr.norm2());
}


inline Vec3 Face::findNormal(
          const Vec3& p1, const Vec3& p2, const Vec3& p3, enum predicate& pred)
{
    Vec3 v23(p3 - p2);
    Vec3 v21(p1 - p2);
    Vec3 n = v23.cross(v21);
    if (n.norm2() < EPSILON_ANGLE) {
        pred = MAYBE_COLINEAR;
    }
    else {
        pred = NONE;
    }
    n.normalize();
    return n;
}



inline Face::Face():mPred(NONE),mToBeMerged(false){;}

inline void Face::setId(long id){mId = id;}

inline Face::~Face(){;};


inline double Face::areaIfConvex()
{
    double total = 0.0;
    auto& v1 = (*((*(*(mIncidentHalfEdges.begin())))->mSrc))->pLCS();

    auto eit1 = mIncidentHalfEdges.begin();
    eit1++;
    auto eit2 = eit1;
    eit2++;

    for (; eit2 != mIncidentHalfEdges.end(); eit1++,eit2++) {
        auto& v2 = (*((*(*eit1))->mSrc))->pLCS();
        auto& v3 = (*((*(*eit1))->mDst))->pLCS();
        total += triangularArea(v1, v2, v3);
    }
    return total;
}


inline enum predicate Face::isCoplanar(const FaceIt& f)
{
    if (mPred == MAYBE_FLAT || (*f)->mPred == MAYBE_FLAT ) {
        return MAYBE_COINCIDENT;
    }
    else {
        auto c = mNormalLCS.cross((*f)->mNormalLCS);
        if (c.norm2() < EPSILON_ANGLE) {
            return MAYBE_COPLANAR;
        }
        else {
            return NONE;
        }
    }
}


inline bool Face::isFacing(
                     const Vec3& p, enum predicate& pred, const double eps)
{
    // Find the furthest incident vertex.
    double distMax = 0.0;
    Vec3   maxP;
    for (auto heit : mIncidentHalfEdges) {
        auto& curP = (*((*heit)->mSrc))->pLCS();
        Vec3 v = p - curP;
        double sqDist = v.squaredNorm2();
        if (distMax < sqDist) {
            distMax = sqDist;
            maxP = curP;
        }
    }

    Vec3 d = p - maxP;

    // p is facing this face if dot product of d and the normal is positive
    double vertDist = d.dot(mNormalLCS);

    if ( fabs(vertDist) < eps) {
        pred = MAYBE_COPLANAR;
    }
    else {
        pred = NONE;
    }
    return vertDist > 0.0;
}


inline long Face::id() const { return mId; }


inline const Vec3& Face::nLCS() const
{
    return mNormalLCS;
}


inline Vec3  Face::nGCS(const Mat3x3&rot) const
{
    return rot * mNormalLCS;
}

inline list<HalfEdgeIt>& Face::halfEdges() { return mIncidentHalfEdges; }

inline long Face::textureID() const { return mTextureID; }


inline bool Face::isIncident(VertexIt vit)
{
    for (auto he : mIncidentHalfEdges) {
        if ((*he)->src()==vit ||(*he)->dst()==vit) {
            return true;
        }
    }
    return false;
}


inline bool Face::isIncident(EdgeIt eit)
{
    for (auto he : mIncidentHalfEdges) {
        if ((*he)->edge()==eit) {
            return true;
        }
    }
    return false;
}


inline Manifold::Manifold(std::ostream& logStream):
    Loggable(logStream),
    mNumFaces(0),
    mPred(NONE),
    mNextIdForFeatures(0),
    mEpsilonCHMargin(EPSILON_SQUARED*100.0){;}


inline Manifold::~Manifold() {;}


inline void Manifold::clear() {
    mVertices.clear();
    mEdges.clear();
    mHalfEdges.clear();
    mFaces.clear();
    const auto& nPair = mConflictGraph.nodes();
    std::vector<Wailea::Undirected::node_list_it_t> gNodes;
    for (auto nit = nPair.first; nit != nPair.second; nit++) {
        gNodes.push_back(nit);
    }
    for (auto nit : gNodes) {
        mConflictGraph.removeNode(*(*nit));
    }
    mNumFaces = 0;
    mNextIdForFeatures = 0;
    mPred = NONE;
}


inline long Manifold::id()const { return mId; }

inline void Manifold::setId(long id){ mId = id; }

inline EdgeIt Manifold::findEdge(const VertexIt& vit1, const VertexIt& vit2)
{
    for (auto he : (*vit1)->mIncidentHalfEdges) {
        if ((*he)->mSrc == vit2 || (*he)->mDst == vit2) {
            return (*he)->mParent;
        }
    }
    return  mEdges.end();
}

class FaceItComp {
  public:
    inline bool operator() (const FaceIt& a, const FaceIt& b) const
                                             {return (*a)->id() < (*b)->id();}
};

inline FaceIt Manifold::findFace(const VertexIt& vit1, const VertexIt& vit2)
{
    set<FaceIt, FaceItComp> faceSet;

    for (auto he : (*vit1)->mIncidentHalfEdges) {
        faceSet.insert((*he)->mFace);
    }

    for (auto he : (*vit2)->mIncidentHalfEdges) {
        if (faceSet.find((*he)->mFace)!= faceSet.end()) {;
            return (*he)->mFace;
        }
    }
    return  mFaces.end();
}

inline FaceIt Manifold::findFace(
             const VertexIt& vit1, const VertexIt& vit2, const VertexIt& vit3)
{
    set<FaceIt, FaceItComp> faceSet1;

    for (auto he : (*vit1)->mIncidentHalfEdges) {
        faceSet1.insert((*he)->mFace);
    }
    set<FaceIt, FaceItComp> faceSet2;
    for (auto he : (*vit2)->mIncidentHalfEdges) {
        if (faceSet1.find((*he)->mFace)!= faceSet1.end()) {;
            faceSet2.insert((*he)->mFace);
        }
    }

    for (auto he : (*vit3)->mIncidentHalfEdges) {
        if (faceSet2.find((*he)->mFace)!= faceSet2.end()) {;
            return (*he)->mFace;
        }
    }
    return  mFaces.end();
}

inline FaceIt Manifold::makeTriangle(
              const HalfEdgeIt& e1, const HalfEdgeIt& e2, const HalfEdgeIt& e3)
{

    list<HalfEdgeIt> edges;

    edges.push_back(e1);
    edges.push_back(e2);
    edges.push_back(e3);

    return makePolygon(edges);

}

inline FaceIt Manifold::makeTriangle(
              const EdgeIt& e1, const EdgeIt& e2, const EdgeIt& e3)
{
    list<EdgeIt> edges;

    edges.push_back(e1);
    edges.push_back(e2);
    edges.push_back(e3);

    return makePolygon(edges);
}

inline FaceIt Manifold::makeQuad(
       const EdgeIt& e1, const EdgeIt& e2, const EdgeIt& e3, const EdgeIt& e4)
{
    list<EdgeIt> edges;

    edges.push_back(e1);
    edges.push_back(e2);
    edges.push_back(e3);
    edges.push_back(e4);

    return makePolygon(edges);
}


inline VertexIt Manifold::makeVertex(const Vec3& p, const long id) {
    auto vp = make_unique<Vertex>(p);
    auto vit = mVertices.insert(mVertices.end(), std::move(vp));
    if (id == -1) {
        (*vit)->setId(mNextIdForFeatures++);
    }
    else {
        (*vit)->setId(id);
    }
    (*vit)->mBackIt = vit;
    return vit;
}


inline EdgeIt Manifold::makeEdge(const VertexIt& v1, const VertexIt& v2) {

    auto ep = make_unique<Edge>();
    auto eit = mEdges.insert(mEdges.end(), std::move(ep));
    (*eit)->mBackIt = eit;

    auto hep1 = make_unique<HalfEdge>();
    hep1->setVerticesAndEdge(v1, v2, eit);
    auto heit1 = mHalfEdges.insert(mHalfEdges.end(), std::move(hep1));

    auto hep2 = make_unique<HalfEdge>();
    hep2->setVerticesAndEdge(v2, v1, eit);
    auto heit2 = mHalfEdges.insert(mHalfEdges.end(), std::move(hep2));

    (*heit1)->mBackIt = heit1;
    (*heit2)->mBackIt = heit2;

    (*heit1)->mBuddy = heit2;
    (*heit2)->mBuddy = heit1;

    (*heit1)->mFace = mFaces.end();
    (*heit2)->mFace = mFaces.end();

    (*heit1)->mParent = eit;
    (*heit2)->mParent = eit;

    (*eit)->mHe1 = heit1;
    (*eit)->mHe2 = heit2;

    Vec3 v2v1 = (*v1)->pLCS() - (*v2)->pLCS();
    if (v2v1.squaredNorm2() < EPSILON_SQUARED) {
        (*eit)->mPredVertices = MAYBE_COINCIDENT;
    }
    return eit;
}


inline FaceIt Manifold::makePolygon (const list<EdgeIt>& edges) {

    auto halfEdges = Face::halfEdgesFromEdges(edges);
    return makePolygon(halfEdges);
}


inline vector<Vec3> Manifold::getPointsLCS()
{
    vector<Vec3> points;
    for (auto& v : mVertices) {
        points.push_back(v->pLCS());
    }
    return points;
}


inline vector<Vec3> Manifold::getFaceNormalsOriginal()
{
    vector<Vec3> normals;
    for (auto& f : mFaces) {
        normals.push_back(f->nLCS());
    }
    return normals;
}


inline pair<VertexIt, VertexIt> Manifold::vertices()
{
    std::pair<VertexIt,VertexIt> p(mVertices.begin(), mVertices.end());
    return p;
}


inline pair<EdgeIt, EdgeIt>   Manifold::edges()
{
    std::pair<EdgeIt, EdgeIt> p(mEdges.begin(), mEdges.end());
    return p;
}


inline pair<FaceIt, FaceIt>   Manifold::faces()
{
    std::pair<FaceIt, FaceIt> p(mFaces.begin(), mFaces.end());
    return p;
}

inline VertexIt Manifold::vertexIt(long id)
{
    return mVertexIdToVertex[id];
}

inline EdgeIt   Manifold::edgeIt  (const pair<long,long>& id)
{
    return mEdgeIdToEdge[id];
}

inline FaceIt Manifold::faceIt  (long id)
{
    return mFaceIdToFace[id];
}

inline void Manifold::setNormalsForVerticesAndEdges()
{
    auto vPair = vertices();
    for (auto vit = vPair.first; vit != vPair.second; vit++) {
        (*vit)->setNormalFromIncidentFaces();
    }

    auto ePair = edges();
    for (auto eit = ePair.first; eit != ePair.second; eit++) {
        (*eit)->setNormalFromIncidentFaces();
    }
}


inline void Manifold::resetGen()
{
    auto vPair = vertices();
    for (auto vit = vPair.first; vit != vPair.second; vit++) {
        (*vit)->resetGen();
    }
}


}// namespace Makena


#endif/*_MAKENA_MANIFOLD_HPP_*/
