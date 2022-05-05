#include "manifold.hpp"
/**
 * @file manifold_convex_hull.cpp
 *
 *  @reference "Computational Geometry Algorithms and Applications"
 *             M. de Berg, M. van Kreveld, M. Overmars, and O.Schwartzkopf
 *             2nd Ed, Springer 2000, ISBN 3-540-65620-0
 */
namespace Makena {

using namespace Wailea;


/* @class FrontierElem
 *
 * @brief temporarily holds the visible vertices to the two faces adjacent 
 *        to the edge specified by the half edge.
 *        This is used to transfer the visible vertices from the old face
 *        to the new one around the frontier.
 */
class FrontierElem {
  public:

    /** @brief the half edge */
    HalfEdgeIt                         mHeit;

    /** @brief the set of visible vertices to the faces incident to
     *         the half edge.
     */
    vector<Undirected::node_list_it_t> mFacingVertices;

};


/** @class FaceConflict
 *
 *  @brief represents a face of the current manifold  in the conflict graph
 */
class FaceConflict : public Directed::DiNode {

  public:
    inline FaceConflict(const FaceIt& it):mFace(it){;}
    inline FaceIt face(){ return mFace; }
  private:
    FaceIt mFace;
};


/** @class VertexConflict
 *
 *  @brief represents a vertex to be tested in the conflict graph
 */
class VertexConflict : public Directed::DiNode {

  public:
    inline VertexConflict(const Vec3& p, const long id):
               mFound(false),mP(p),mId(id){;}
    inline const Vec3& p() { return mP; }
    inline const long  id(){ return mId; }
    /** @brief temporary flag used to avoid doubly adding
     *         the same vertex to the list when two or more faces are merged.
     */
    long         mFound;

  private:
    const Vec3   mP;
    const long   mId;

};



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
 *  @reference "Computational Geometry Algorithms and Applications"
 *             M. de Berg, M. van Kreveld, M. Overmars, and O.Schwartzkopf
 *             2nd Ed, Springer 2000, ISBN 3-540-65620-0
 */
void Manifold::findConvexHull(
    vector<Vec3>&   points,
    enum predicate& pred,
    const double    epsilon
) {
    vector<long> indices;
    for (long i = 0; i < points.size(); i++) {
        indices.push_back(i);
    }

    findConvexHull(points, indices, pred, epsilon);
}


void Manifold::findConvexHull(
    vector<Vec3>&   points,
    vector<long>&   indices,
    enum predicate& pred,
    const double    epsilon
) {
    mEpsilonCHMargin = epsilon;

    log(INFO, __FILE__, __LINE__, "findConvexHull() BEGIN");

    if (points.size() < 4) {
        pred = MAYBE_FLAT;
        return;
    }

    size_t index1, index2, index3,  index4;

    pred = analyzePoints(points, index1, index2, index3, index4);
    if (pred != NONE) {
        return;
    }

    // Find the initial 3-simplex from the 4 points
    construct3Simplex(
        points [index1], points [index2], points [index3], points [index4],
        indices[index1], indices[index2], indices[index3], indices[index4]
    );

    log(INFO, __FILE__, __LINE__, "Initial 3-simplex");
    logContents(INFO, __FILE__, __LINE__);

    // Remove the 4 points from the list, and generate conflict graph nodes.
    vector<Vec3> pointsReduced;
    vector<long> indicesReduced;
    for (size_t i = 0; i < points.size(); i++) {
        if (i != index1 && i != index2 && i != index3 && i != index4) {
            pointsReduced.push_back (points [i]);
            indicesReduced.push_back(indices[i]);
        }
    }

    // For each face find conflicts.
    vector<Undirected::node_list_it_t> vertices;

    createInitialConflictGraph(pointsReduced, indicesReduced, vertices);

    logConflictGraph(INFO, __FILE__, __LINE__);

    for (auto vcit : vertices) {

        auto& vc = dynamic_cast<VertexConflict&>(*(*vcit));

        log(INFO, __FILE__, __LINE__, "Start of loop.");
        logVertexConflict(INFO, __FILE__, __LINE__, vc);

        if (vc.degreeIn() > 0) {

            vector<FaceIt>       conflictFaces;

            auto abort = findVisibleFaces(vc, conflictFaces);

            if (!abort) {

                vector<FrontierElem> frontier;

                auto vp = updateFaces(
                              vc.p(), vc.id(), conflictFaces, frontier, abort);

                if (!abort) {

                    updateConflictGraph(frontier);

                    checkAndMergeFacesCounterClockwise(vp);
                }
            }
        }

        mConflictGraph.removeNode(vc);

        log(INFO, __FILE__, __LINE__, "End of loop");
        logContents(INFO, __FILE__, __LINE__);
        logConflictGraph(INFO, __FILE__, __LINE__);
    }

    // Tidy up
    clearConflictGraph();

    setNormalsForVerticesAndEdges();

    constructHelperMaps();

}


/** @brief it performns principal component analysis on the given points
 *         and find 4 extremal points to make a good initial 3-simplex
 *         as the starting manifold to find the convex hull.
 *
 *  @param points (in): points in 3 space. There must be at least 4 points.
 *
 *  @param index1 (out): index into points for the extremal point
 *                       along the principal direction
 *
 *  @param index2 (out): index into points for the other extremal point
 *                       along the principal direction
 *
 *  @param index3 (out): index into points for the extremal point perpendicular
 *                       to the direction defined by 1st and 2nd points.
 *
 *  @param index4 (out): index into points for the extremal point perpendicular
 *                       to the direction defined by 1st, 2nd and 3rd points
 *
 *  @return predicate found during the analysis.
 */
enum predicate Manifold::analyzePoints(
    vector<Vec3>& points,
    size_t      & index1,
    size_t      & index2,
    size_t      & index3,
    size_t      & index4 
) {
    Vec3   mean;
    for(auto& p : points) {
        mean += p;
    }
    mean.scale(1.0/points.size());
    Vec3 variance(0.0, 0.0, 0.0);
    for(auto& p : points) {
        Vec3 d = p - mean;
        Vec3 v(d[1]*d[1], d[2]*d[2], d[3]*d[3]); 
        variance += v;
    }
    variance.normalize();

    // Find two extremal points along the 1st principal axis. 
    Vec3   ax1       = variance;
    double xMin      = ax1.dot(points[0]);
    double xMax      = xMin;
    long   xMinIndex = 0;
    long   xMaxIndex = 0;

    for (long i = 1; i < points.size(); i++) {
        auto& p = points[i];
        double dot = ax1.dot(p);
        if (xMin > dot) {
            xMin      = dot;
            xMinIndex = i;
        }
        if (xMax < dot) {
            xMax      = dot;
            xMaxIndex = i;
        }
    }

    const Vec3   p1  = points[xMinIndex];
    const Vec3   p2  = points[xMaxIndex];

    // Find the extremal point perpendicular to the direction of (p1, p2).
    const Vec3   ax2 = p2 - p1;
    double       yAbsMax = 0.0;
    long         yAbsMaxIndex = -1;
    bool         yAbsMaxFound = false;

    for (size_t i = 0; i < points.size(); i++) {
        if (i != xMinIndex && i != xMaxIndex) {
 
            auto& p = points[i];
            auto  v1 = p - p1;
            auto  v2 = p - p2;

            if ( v1.squaredNorm2() < EPSILON_SQUARED || 
                 v2.squaredNorm2() < EPSILON_SQUARED   ) {
                continue;
            }

            double sinDist = ax2.cross(v1).squaredNorm2();
            if (!yAbsMaxFound) {
                yAbsMaxFound = true;
                yAbsMax      = sinDist;
                yAbsMaxIndex = i;
            }
            else if (sinDist > yAbsMax) {
                yAbsMax      = sinDist;
                yAbsMaxIndex = i;
            }
        }
    }

    if (!yAbsMaxFound) {
        // Ill-shaped. All the points are coincident to either xMin or xMax.
        log(WARNING, __FILE__, __LINE__, "All points on an edge.");
        return MAYBE_COLINEAR;
    }

    // Find the extremal point along the normal of (p1, p2, p3).
    const Vec3   p3  = points[yAbsMaxIndex];
    const Vec3   ax3 = ax2.cross(p3 - p1);
    double       zAbsMax = 0.0;
    long         zAbsMaxIndex = -1;
    bool         zAbsMaxFound = false;

    for (size_t i = 0; i < points.size(); i++) {
        if (i != xMinIndex && i != xMaxIndex && i != yAbsMaxIndex) {
 
            auto& p = points[i];
            auto v1 = p - p1;
            auto v2 = p - p2;
            auto v3 = p - p3;

            if ( v1.squaredNorm2() < EPSILON_SQUARED || 
                 v2.squaredNorm2() < EPSILON_SQUARED ||
                 v3.squaredNorm2() < EPSILON_SQUARED   ) {
                continue;
            }

            double dot = fabs(ax3.dot(v1));
            if (!zAbsMaxFound) {
                zAbsMaxFound = true;
                zAbsMax      = dot;
                zAbsMaxIndex = i;
            }
            else if (dot > zAbsMax) {
                zAbsMax      = dot;
                zAbsMaxIndex = i;
            }
        }
    }

    if (!zAbsMaxFound) {
        // Ill-shaped. All the points are coincident to xMin, xMax or yAbsMax.
        log(WARNING, __FILE__, __LINE__, "All points on a plane.");
        return MAYBE_COPLANAR;
    }

    index1 = xMinIndex;
    index2 = xMaxIndex;
    index3 = yAbsMaxIndex;
    index4 = zAbsMaxIndex; 

    return NONE;
}


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
void Manifold::createInitialConflictGraph(
    std::vector<Vec3>&                       points,
    std::vector<long>&                       indices,
    std::vector<Undirected::node_list_it_t>& vertices
) {

    for (auto fit =  mFaces.begin(); fit != mFaces.end(); fit++) {
        auto fcp = make_unique<FaceConflict>(fit);
        auto& fc = mConflictGraph.addNode(std::move(fcp));
        (*fit)->mFaceConflict = fc.backIt();

    }

    for (long i = 0; i < points.size(); i++) {
        auto& p   = points[i];
        auto& id  = indices[i];
        auto vcit = mConflictGraph.nodes().second;

        for (auto fit =  mFaces.begin(); fit != mFaces.end(); fit++) {

            auto& fcit = (*fit)->mFaceConflict;
            auto& fc   = dynamic_cast<FaceConflict&>(*(*fcit));

            enum predicate pred;
            if ((*fit)->isFacing(p, pred, mEpsilonCHMargin)) {
                if (pred == NONE) {
                    if (vcit == mConflictGraph.nodes().second) {
                        auto vcp = make_unique<VertexConflict>(p, id);
                        vcit = mConflictGraph.addNode(std::move(vcp)).backIt();
                        vertices.push_back(vcit);
                    }

                    auto& vc = dynamic_cast<VertexConflict&>(*(*vcit));
                    auto  ep = make_unique<Directed::DiEdge>();
                    mConflictGraph.addEdge(std::move(ep), fc, vc);
                }
            }
        }
    }
}


bool Manifold::findVisibleFaces(
    VertexConflict& vc,
    vector<FaceIt>& conflictFaces
) {
    auto iPair = vc.incidentEdgesIn();

    bool abort = false;

    for (auto iit = iPair.first; iit != iPair.second; iit++) {

        auto& e = dynamic_cast<Directed::DiEdge&>(*(*(*iit)));

        auto& fc = dynamic_cast<FaceConflict&>(e.adjacentNode(vc));

        if (vertexIsTooCloseToFace(vc.p(), fc.face())) {
            log(INFO, __FILE__, __LINE__,
                "Aborting. Point is too close to face [%d]",
                (*(fc.face()))->id()                         );
            abort = true;
            break;
        }
        conflictFaces.push_back(fc.face());
    }
    return abort;
}


bool Manifold::vertexIsTooCloseToFace(const Vec3& pTest, FaceIt fit)
{
    for (auto& he : (*fit)->halfEdges()) {

        const Vec3   pSrc     = (*((*he)->src()))->pLCS();
        const Vec3   pDst     = (*((*he)->dst()))->pLCS();
        const Vec3   pSrcTest = pTest - pSrc;
        const double sqDist   = pSrcTest.squaredNorm2();

        if (sqDist < mEpsilonCHMargin) {
            log(INFO, __FILE__, __LINE__, 
                         "Vertex too close to [%d]", (*((*he)->src()))->id());
            return true;
        }

        const Vec3 v12     = pDst  - pSrc;
        const Vec3 v1t     = pTest - pSrc;
        Vec3 v12norm       = v12;
        v12norm.normalize();
        Vec3 v1tnorm = v1t;
        v1tnorm.normalize();
        const Vec3 cr      = v12norm.cross(v1tnorm);

        if ( (cr.squaredNorm2()  < mEpsilonCHMargin)   &&
             (v12.dot(v1t)       > 0.0)                &&
             (v1t.squaredNorm2() < v12.squaredNorm2())    ) {

            log(INFO, __FILE__, __LINE__, "Vertex too close to (%d,%d)", 
                             (*((*he)->src()))->id(),(*((*he)->dst()))->id());
            return true;
        }
    }

    return false;
}


VertexIt Manifold::updateFaces(
    const Vec3&           p,
    const long            id,
    vector<FaceIt>&       conflictFaces,
    vector<FrontierElem>& frontier,
    bool&                 abort
) {
    vector<HalfEdgeIt> frontierHalfEdges
                                    = findCircumference(conflictFaces, abort);
    if (abort) {
        log(INFO, __FILE__, __LINE__, "Aborting.");
        return mVertices.end();
    }

    // Temporarily save the conflict graph info for the faces being deleted
    // to FrontierElems.
    frontier = makeFrontier(frontierHalfEdges);

    for (auto& cf : conflictFaces) {

        auto& fcit = (*cf)->mFaceConflict;
        auto& fc = dynamic_cast<FaceConflict&>(*(*fcit));
        mConflictGraph.removeNode(fc);

    }

    removeFaces(conflictFaces);

    return makeCircularFan(frontierHalfEdges, p, id);
}


vector<FrontierElem> Manifold::makeFrontier(vector<HalfEdgeIt>& halfEdges)
{

    vector<FrontierElem> elements;

    for (auto& he : halfEdges) {

        FrontierElem fe;

        fe.mHeit = he;

        auto& fcit1 = (*((*he)->mFace))->mFaceConflict;
        auto& fc1   = dynamic_cast<FaceConflict&>(*(*fcit1));
        auto iPair1 = fc1.incidentEdgesOut();

        for (auto iit = iPair1.first; iit != iPair1.second; iit++) {

            auto& e   = dynamic_cast<Directed::DiEdge&>(*(*(*iit)));
            auto& vc  = dynamic_cast<VertexConflict&>(e.adjacentNode(fc1));
            vc.mFound = true;

            fe.mFacingVertices.push_back(vc.backIt());

        }

        auto& heBuddy = (*he)->mBuddy;
        auto& fcit2   = (*((*heBuddy)->mFace))->mFaceConflict;
        auto& fc2     = dynamic_cast<FaceConflict&>(*(*fcit2));
        auto iPair2   = fc2.incidentEdgesOut();

        for (auto iit = iPair2.first; iit != iPair2.second; iit++) {

            auto& e  = dynamic_cast<Directed::DiEdge&>(*(*(*iit)));
            auto& vc = dynamic_cast<VertexConflict&>(e.adjacentNode(fc2));
            if (!vc.mFound) {
                fe.mFacingVertices.push_back(vc.backIt());
            }
        }

        // Reset mFound.
        for (auto iit = iPair1.first; iit != iPair1.second; iit++) {

            auto& e   = dynamic_cast<Directed::DiEdge&>(*(*(*iit)));
            auto& vc  = dynamic_cast<VertexConflict&>(e.adjacentNode(fc1));
            vc.mFound = false;
        }

        elements.push_back(std::move(fe));

    }

    return elements;
}


void Manifold::updateConflictGraph(vector<FrontierElem>& frontier)
{

    for(auto& fe : frontier) {

        // (*fe.mHeit)->mFace points the new face created.
        auto& f  = (*fe.mHeit)->mFace;
        auto fcp = make_unique<FaceConflict>(f);
        auto& fc = mConflictGraph.addNode(std::move(fcp));

        (*f)->mFaceConflict = fc.backIt();

        for (auto& vcit : fe.mFacingVertices) {

            auto& vc = dynamic_cast<VertexConflict&>(*(*vcit));
            enum predicate pred;
            if ((*f)->isFacing(vc.p(),pred,mEpsilonCHMargin)){

                if (pred == NONE) {

                    auto ep = make_unique<Directed::DiEdge>();
                    mConflictGraph.addEdge(std::move(ep), fc, vc);

                }
            }
        }
    }
}


void Manifold::checkAndMergeFacesCounterClockwise(VertexIt center)
{
    mEdgesToBeRemoved.clear();
    mVerticesToBeRemoved.clear();

    for (auto hePillar : (*center)->mIncidentHalfEdges) {
        if ((*hePillar)->mSrc == center) {
            auto heCircum = (*hePillar)->mNext;

            auto ePillar = (*hePillar)->mParent;

            if ((*ePillar)->mPredFaces == MAYBE_COPLANAR  ||
                areIncidentFacesParallelOrConcave(ePillar)  ) {
                if (!(*ePillar)->mToBeRemoved) {
                    (*ePillar)->mToBeRemoved = true;
                    (*ePillar)->mBackItETBR = mEdgesToBeRemoved.insert(
                                          mEdgesToBeRemoved.begin(), ePillar);
                }
            }

            auto eCircum = (*heCircum)->mParent;
            if ((*eCircum)->mPredFaces == MAYBE_COPLANAR  ||
                areIncidentFacesParallelOrConcave(eCircum)  ) {
                if (!(*eCircum)->mToBeRemoved) {
                    (*eCircum)->mToBeRemoved = true;
                    (*eCircum)->mBackItETBR = mEdgesToBeRemoved.insert(
                                          mEdgesToBeRemoved.begin(), eCircum);
                }
            }
        }
    }

    while (!mEdgesToBeRemoved.empty() || !mVerticesToBeRemoved.empty()) {

        if (!mEdgesToBeRemoved.empty()) {
            auto eit = *(mEdgesToBeRemoved.begin());
            mEdgesToBeRemoved.pop_front();
            (*eit)->mToBeRemoved = false;
            auto abort = removeEdgeAndMergeFaces(eit);
            if (abort) {
                auto v1 = (*((*eit)->he1()))->src();
                auto v2 = (*((*eit)->he1()))->dst();
                log(INFO, __FILE__, __LINE__, "Edge removal aborted. (%d,%d)", 
                                              (*v1)->id(),  (*v2)->id()      );

            }            
        }

        if (!mVerticesToBeRemoved.empty()) {
            auto vit = *(mVerticesToBeRemoved.begin());
            mVerticesToBeRemoved.pop_front();
            (*vit)->mToBeRemoved = false;
            removeVertexAndMergeEdges(vit);
        }
    }
}


/** @brief tests if two adjacent faces are in parallel or facing toward each
 *         other, in which case those two should be merged.
 *  @return true  : if two are parallel or concave, and should be merged.
 *          false : the two faces are convex.
 */
bool Manifold::areIncidentFacesParallelOrConcave(EdgeIt e)
{
    //            v2
    //             *
    //  ^        /| |         ^        ^
    //  |         | |         |        |
    //  |         | |         |        |
    //  +  f1  he1|e|he2  f2  +        |
    // n1         | |        n2       v12
    //            | |
    //            | |/
    //             *
    //            v1
    auto he1 = (*e)->he1();
    auto he2 = (*e)->he2();
    auto v1  = (*he1)->src();
    auto v2  = (*he2)->src();
    auto f1  = (*he1)->mFace;
    auto f2  = (*he2)->mFace;

    const auto n1  = (*f1)->nLCS();
    const auto n2  = (*f2)->nLCS();
    const auto cr  = n1.cross(n2);
    const auto v12 = (*v2)->pLCS() - (*v1)->pLCS();

    //
    //    n1xn2
    //  ^   ^   ^        ^     ^
    //   \  |  /          \   /
    //  n1\ | / n2       n2\ /n1
    //     \|/              |
    //                      v n1xn2
    //
    //    convex         concave
    //    ------         -------

    return v12.dot(cr) <= mEpsilonCHMargin;
}


bool Manifold::isNewFaceAreawiseConvex(
    HalfEdgeIt  heCW,
    HalfEdgeIt  heCCW,
    const Vec3& n
) {

    if (heCCW == heCW) {
        return false;
    }

    //
    //          * pCW
    //          ^
    //          |heCW          /
    //   heCCW  |             / n (face normal)
    //  *<------* pCenter    v
    // pCCW    /
    //        / vCW x vCCW
    //       v
    //
    auto pCenter = (*((*heCCW)->src()))->pLCS();
    auto pCW     = (*((*heCW) ->dst()))->pLCS();
    auto pCCW    = (*((*heCCW)->dst()))->pLCS();
    auto vCW     = pCW  - pCenter;
    auto vCCW    = pCCW - pCenter;

    auto cr = vCW.cross(vCCW);
    cr.normalize();

    // The condition on convexity is relaxed to include parallel edges.
    return n.dot(cr) > -1.0 * std::min(0.1, mEpsilonCHMargin*100.0);

}


void Manifold::mergeConsecutiveFaces(vector<FaceIt>& faces)
{
    vector<Wailea::Undirected::node_list_it_t> vertices;

    for (auto fit : faces) {

        auto& FC =dynamic_cast<FaceConflict&>(*(*((*fit)->mFaceConflict)));

        auto ePair = FC.incidentEdgesOut();
        for (auto eit = ePair.first; eit != ePair.second; eit++) {
            auto& E  = dynamic_cast<Wailea::Directed::DiEdge&>(*(*(*eit)));
            auto& VC = dynamic_cast<VertexConflict&>(E.adjacentNode(FC));

            if (!VC.mFound) {
                VC.mFound = true;
                vertices.push_back(VC.backIt());
            }
        }
        mConflictGraph.removeNode(FC);
    }

    // Reset the flag.
    for (auto vit : vertices) {
        auto& VC = dynamic_cast<VertexConflict&>(*(*vit));
        VC.mFound = false;
    }

    bool abortIgnored;
    vector<HalfEdgeIt> halfEdges = findCircumference(faces, abortIgnored);
    list<HalfEdgeIt>   halfEdgesList(halfEdges.begin(),halfEdges.end());
    removeFaces(faces);

    auto fit = makePolygon(halfEdgesList);

    for (auto heit : halfEdges) {
        auto fit    = (*heit)->face();
        auto hBuddy = (*heit)->mBuddy;
        auto fBuddy = (*hBuddy)->face();
        (*((*heit)->mParent))->mPredFaces = (*fit)->isCoplanar(fBuddy);
    }

    auto fcp = make_unique<FaceConflict>(fit);
    auto& FC = mConflictGraph.addNode(std::move(fcp));
    (*fit)->mFaceConflict = FC.backIt();

    for (auto v: vertices) {

        auto& VC = dynamic_cast<VertexConflict&>(*(*v));
        enum predicate pred;
        if ((*fit)->isFacing(VC.p(), pred, mEpsilonCHMargin)) {
            if (pred == NONE) {
                auto  ep = make_unique<Directed::DiEdge>();
                mConflictGraph.addEdge(std::move(ep), FC, VC);
            }
        }
    }
}


bool Manifold::checkForConcavity(
    vector<HalfEdgeIt>& halfEdges,
    vector<FaceIt>&     additionalFaces
) {
    additionalFaces.clear();

    auto fit   = (*(halfEdges[0]))->face();
    auto n     = (*fit)->nLCS();
    for (long i = 0; i < halfEdges.size(); i++) {

        auto index1 = i;
        auto index2 = (i + 1)%halfEdges.size();
        auto he1    = halfEdges[index1];
        auto he2    = halfEdges[index2];
        auto v1     = (*he1)->mSrc;
        auto v2     = (*he2)->mSrc;
        auto v3     = (*he2)->mDst;
        auto p1     = (*v1)->pLCS();
        auto p2     = (*v2)->pLCS();
        auto p3     = (*v3)->pLCS();
        auto v12    = p2 - p1;
        auto v23    = p3 - p2;
        v12.normalize();
        v23.normalize();
        auto cr     = v12.cross(v23);
        if (n.dot(cr) < -1.0 * std::min(0.1, mEpsilonCHMargin*100.0)) {

            // Concavity found
            auto e1      = (*he1)->mParent;
            auto e2      = (*he2)->mParent;
            auto hBuddy1 = (*he1)->mBuddy;
            auto hBuddy2 = (*he2)->mBuddy;
            auto f1      = (*hBuddy1)->face();
            auto f2      = (*hBuddy2)->face();

            if ( ((*e1)->mPredFaces == MAYBE_COPLANAR) &&
                 ((*e2)->mPredFaces == MAYBE_COPLANAR)    ) {
                if (!(*f1)->mToBeMerged) {
                    (*f1)->mToBeMerged = true;
                    additionalFaces.push_back(f1);
                }
                if (!(*f2)->mToBeMerged) {
                    (*f2)->mToBeMerged = true;
                    additionalFaces.push_back(f2);
                }
            }
            else {
                for (auto fit : additionalFaces) {
                    (*fit)->mToBeMerged = false;
                }
                log(INFO, __FILE__, __LINE__, 
                       "Edges dented but the adjacent face is not coplanar "
                       "Vertices: (%d -> %d -> %d), Faces: %d | (%d, %d)",
                       (*v1)->id(), (*v2)->id(), (*v3)->id(),
                       (*fit)->id(), (*f1)->id(), (*f2)->id()              );
                return true;
            }
        }
    }

    for (auto fit : additionalFaces) {
        (*fit)->mToBeMerged = false;
    }

    return false;

}


void Manifold::findInnerEdgesAndRemoveFromChain(
    vector<FaceIt>     faces,
    vector<HalfEdgeIt> halfEdges
) {
    for (auto he : halfEdges) {
        auto eit = (*he)->mParent;
        (*eit)->mFound = true;
    }

    for (auto fit : faces) {   
        for (auto he : (*fit)->halfEdges()) {
            auto eit = (*he)->mParent;
            if (!(*eit)->mFound) {
                if ((*eit)->mToBeRemoved) {
                    (*eit)->mToBeRemoved = false;
                    mEdgesToBeRemoved.erase((*eit)->mBackItETBR);
                }
            }
        }
    }

    for (auto he : halfEdges) {
        auto eit = (*he)->mParent;
        (*eit)->mFound = false;
    }
}

bool Manifold::removeEdgeAndMergeFaces(EdgeIt eit)
{

    vector<FaceIt> faces = gatherConnectedFaces(eit);
    vector<HalfEdgeIt> halfEdges;
    bool abort = false;
    while (true) {

        halfEdges = findCircumference(faces, abort);
        if (abort) {
            log(INFO, __FILE__, __LINE__, "Aborting");
            return true;
        }
        vector<FaceIt> newFaces;
        abort = checkForConcavity(halfEdges, newFaces);
        if (abort) {
            log(INFO, __FILE__, __LINE__, "Aborting");
            return true;
        }
        if (newFaces.empty()) {
            break;
        }
        faces.insert(faces.end(), newFaces.begin(), newFaces.end());
    }
    findInnerEdgesAndRemoveFromChain(faces, halfEdges);

    mergeConsecutiveFaces(faces);

    for (auto he : halfEdges) {
        auto vit = (*he)->src();
        removeEdgeAndMergeFacesCheckForUpdate(vit);
    }
    return false;
}


vector<FaceIt> Manifold::gatherConnectedFaces(EdgeIt eit)
{
    FaceIt f1 = (*((*eit)->he1()))->face();
    FaceIt f2 = (*((*eit)->he2()))->face();
    list<FaceIt>    Q;
    vector<FaceIt>  faces;
    Q.push_back(f1);
    (*f1)->mToBeMerged = true;
    Q.push_back(f2);
    (*f2)->mToBeMerged = true;

    while (!Q.empty()) {
        FaceIt fit = *(Q.begin());
        Q.pop_front();
        faces.push_back(fit);
        for (auto& heit : (*fit)->halfEdges()) {
            auto eit     = (*heit)->mParent;
            if ((*eit)->mToBeRemoved) {
                auto heBuddy = (*heit)->buddy();
                auto fBuddy  = (*heBuddy)->face();

                if (!(*fBuddy)->mToBeMerged) {
                    (*fBuddy)->mToBeMerged = true;
                    Q.push_back(fBuddy);
                }
            }        
        }
    }

    for (auto fit : faces) {
        (*fit)->mToBeMerged = false;
    }
    return faces;
}


void Manifold::removeEdgeAndMergeFacesCheckForUpdate(VertexIt vit)
{

    if ((*vit)->halfEdges().size()==4) {
        // Degree 2.
        (*vit)->mBackItVTBR =
              mVerticesToBeRemoved.insert(mVerticesToBeRemoved.begin(), vit);
    }
    else if ((*vit)->halfEdges().size()==2) {

        // Degree 1.
        bool needUpdate;
        auto vCur   = vit;
        auto heCur  = *((*vCur)->halfEdges().begin());
        auto eCur   = (*heCur)->mParent;
        auto vAdj   = ((*heCur)->src()==vCur)?(*heCur)->dst():(*heCur)->src();
        auto degAdj = (*vAdj)->halfEdges().size();
        if (degAdj == 6) {
            // vAdj will become deg2 after removal.
            (*vAdj)->mBackItVTBR =
               mVerticesToBeRemoved.insert(mVerticesToBeRemoved.begin(), vAdj);
            (*vAdj)->mToBeRemoved = true;
            needUpdate = false;
        }
        else if (degAdj == 4) {
            if (!(*vAdj)->mToBeRemoved) {
                log(ERROR, __FILE__, __LINE__, 
                                "Vertex not chained yet. [%d]", (*vAdj)->id());
            }
            // vAdj will become deg1 after removal.
            mVerticesToBeRemoved.erase((*vAdj)->mBackItVTBR);
            (*vAdj)->mToBeRemoved = false;
            needUpdate = true;
        }
        else {
            needUpdate = false;
        }

        if ((*eCur)->mToBeRemoved) {
            mEdgesToBeRemoved.erase((*eCur)->mBackItETBR);
        }

        removeEdge(eCur);

        while (needUpdate) {
            vCur   = vAdj;
            heCur  = *((*vCur)->halfEdges().begin());
            eCur   = (*heCur)->mParent;
            vAdj   = ((*heCur)->src()==vCur)?(*heCur)->dst():(*heCur)->src();
            degAdj = (*vAdj)->halfEdges().size();
            if (degAdj == 6) {
                // vAdj will become deg2 after removal.
                (*vAdj)->mBackItVTBR = mVerticesToBeRemoved.insert(
                                           mVerticesToBeRemoved.begin(), vAdj);
                (*vAdj)->mToBeRemoved = true;
                needUpdate = false;
            }
            else if (degAdj == 4) {
                if (!(*vAdj)->mToBeRemoved) {
                    log(ERROR, __FILE__, __LINE__, 
                                "Vertex not chained yet. [%d]", (*vAdj)->id());
                }
                // vAdj will become deg1 after removal.
                mVerticesToBeRemoved.erase((*vAdj)->mBackItVTBR);
                (*vAdj)->mToBeRemoved = false;
                needUpdate = true;
            }
            else {
                needUpdate = false;
            }

            if ((*eCur)->mToBeRemoved) {
                mEdgesToBeRemoved.erase((*eCur)->mBackItETBR);
            }
            removeEdge(eCur);
        }
    }
}


void Manifold::removeVertexAndMergeEdges(VertexIt v2)
{
    //                   f1                                   f1
    //
    //        he1       he2       he3                   he1       he3
    //      ------->  =======>  ------->              ------->  ------->
    //    v1       (v2)       v3            ===>    v1        v3
    //      <-------  <=======  <-------              <------   <-------
    //        he6  deg2 he5       he4                   he6       he4
    //
    //                   f2                                   f2

    auto heit   = *((*v2)->halfEdges().begin());
    auto he2    = (*heit)->src()==v2?heit:(*heit)->buddy();
    auto he1    = (*he2)->mPrev;
    auto he3    = (*he2)->mNext;
    auto he5    = (*he2)->buddy();
    auto he4    = (*he5)->mPrev;
    auto he6    = (*he5)->mNext;
    auto v1     = (*he1)->src();
    auto v3     = (*he2)->dst();
    auto f1     = (*he2)->face();
    auto f2     = (*he5)->face();

    auto v3Att  = (*he4)->mDstBackIt;

    if (v1==v3) {
        // v1==v3. f1 or f2 is a 2-cycle
        auto eit = (*he1)->mParent;
        if (!(*eit)->mToBeRemoved) {
            (*eit)->mBackItETBR =
            mEdgesToBeRemoved.insert(mEdgesToBeRemoved.begin(), eit);
            (*eit)->mToBeRemoved = true;
        }
        (*v2)->mToBeRemoved = false;

        return ;
    }

    // remove v2, he2, and he5.
    (*f2)->mIncidentHalfEdges.erase((*he5)->mFaceBackIt);
    (*f1)->mIncidentHalfEdges.erase((*he2)->mFaceBackIt);
    (*v3)->mIncidentHalfEdges.erase((*he5)->mSrcBackIt);
    (*v3)->mIncidentHalfEdges.erase((*he2)->mDstBackIt);

    auto eRemoved = (*he5)->mParent;
    mHalfEdges.erase(he5);
    mHalfEdges.erase(he2);
    mEdges.erase(eRemoved);
    (*he1)->mDstBackIt =(*v3)->mIncidentHalfEdges.insert(v3Att, he1);
    (*he1)->mDst = v3;
    (*he6)->mSrcBackIt =(*v3)->mIncidentHalfEdges.insert(v3Att, he6);
    (*he6)->mSrc = v3;
    mVertices.erase(v2);

    (*he6)->mPrev = he4;
    (*he4)->mNext = he6;

    (*he1)->mNext = he3;
    (*he3)->mPrev = he1;

    if ((*f1)->halfEdges().size()==2) {
        //       eit2
        //       he3
        //     <------
        //   v1  f1   v3
        //     ------>
        //       he1
        //       eit1
        auto eit1 = (*he1)->mParent;
        auto eit2 = (*he3)->mParent;
        if (!(*eit2)->mToBeRemoved) {
            (*eit2)->mBackItETBR =
                    mEdgesToBeRemoved.insert(mEdgesToBeRemoved.begin(), eit2);
            (*eit2)->mToBeRemoved = true;
        }
        else if  (!(*eit1)->mToBeRemoved) {
            (*eit1)->mBackItETBR =
                    mEdgesToBeRemoved.insert(mEdgesToBeRemoved.begin(), eit1);
            (*eit1)->mToBeRemoved = true;
        }
        else {
            log(ERROR, __FILE__, __LINE__, 
                    "Edge (%d, %d) already chained", (*v1)->id(), (*v3)->id());
        }
    }

    if (f1 != f2 && (*f2)->halfEdges().size()==2) {
        //       eit2
        //       he6
        //     <------
        //   v1  f2   v3
        //     ------>
        //       he4
        //       eit1
        auto eit1 = (*he4)->mParent;
        auto eit2 = (*he6)->mParent;
        if (!(*eit2)->mToBeRemoved) {
            (*eit2)->mBackItETBR =
                    mEdgesToBeRemoved.insert(mEdgesToBeRemoved.begin(), eit2);
            (*eit2)->mToBeRemoved = true;
        }
        else if  (!(*eit1)->mToBeRemoved) {
            (*eit1)->mBackItETBR =
                    mEdgesToBeRemoved.insert(mEdgesToBeRemoved.begin(), eit1);
            (*eit1)->mToBeRemoved = true;
        }
        else {
            log(ERROR, __FILE__, __LINE__, 
                    "Edge (%d, %d) already chained", (*v1)->id(), (*v3)->id());
        }
    }
}



void Manifold::clearConflictGraph()
{

    vector<Undirected::node_list_it_t> nodesToBeRemoved;

    for (auto nit = mConflictGraph.nodes().first;
                                 nit != mConflictGraph.nodes().second; nit++) {

        nodesToBeRemoved.push_back(nit);
    }

    for (auto nit : nodesToBeRemoved) {

        auto& N = dynamic_cast<Directed::DiNode&>(*(*nit));

        mConflictGraph.removeNode(N);
    }
}


void Manifold::logContents(
    enum LogLevel lvl,
    const char*   _file,
    const int     _line
) {
    if (mLogLevel>0 && mLogLevel>=lvl) {

        // Vertices
        mLogStream << "Vertices\n";
        for (auto& vit : mVertices) {
            mLogStream << "    P: " << (vit)->id() << "\t";
            mLogStream << (vit)->pLCS() << "\t";
            mLogStream << "N: " << (vit)->nLCS() << "\n";

        }

        // Faces
        mLogStream << "\nFaces\n";
        for (auto& fit : mFaces) {
            mLogStream << "    " << (fit)->id() << "\t";
            mLogStream << "N: " << (fit)->nLCS() << "\t";
            bool start = true;
            for (auto& heit : (fit)->halfEdges()) {
                if (start) {
                    start = false;
                }
                else{
                    mLogStream << " ";
                }
                mLogStream << (*((*heit)->mSrc))->id();
            }
            mLogStream << "\n";
        }

    }
}


void Manifold::logConflictGraph(
    enum LogLevel lvl,
    const char*   _file,
    const int     _line
) {
    if (mLogLevel>0 && mLogLevel>=lvl) {
       
        mLogStream << "ConflictGraph:\n";
        mLogStream << "VertexConflicts\n";
        for (auto nit  = mConflictGraph.nodes().first;
                  nit != mConflictGraph.nodes().second;
                  nit++                                  ) {
            auto& D = dynamic_cast<Wailea::Directed::Node&>(*(*nit));
            if (typeid(D)==typeid(VertexConflict&)) {
                auto& N = dynamic_cast<VertexConflict&>(*(*nit));
                mLogStream << "    P: " << N.p() << "\t";
                bool start = true;
                auto ePair = N.incidentEdgesIn();
                for (auto eit = ePair.first; eit != ePair.second; eit++) {
                    auto& E  = dynamic_cast<Wailea::Directed::DiEdge&>(
                                                                 *(*(*eit)));
                    auto& F = dynamic_cast<FaceConflict&>(E.adjacentNode(N));
                    auto  fit = F.face();
                    if (start) {
                        start = false;
                    }
                    else{
                        mLogStream << " ";
                    }
                    mLogStream << (*fit)->id();
                }
                 mLogStream << "\n";
            }
        }
    }
}


void Manifold::logVertexConflict(
    enum LogLevel   lvl,
    const char*     _file,
    const int       _line,
    VertexConflict& N
) {
    if (mLogLevel>0 && mLogLevel>=lvl) {
        mLogStream << "VC: ";
        mLogStream << N.p() << "\t";
        mLogStream << N.id() << "\t";
        bool start = true;
        auto ePair = N.incidentEdgesIn();
        for (auto eit = ePair.first; eit != ePair.second; eit++) {
            auto& E  = dynamic_cast<Wailea::Directed::DiEdge&>(*(*(*eit)));
            auto& F = dynamic_cast<FaceConflict&>(E.adjacentNode(N));
            auto  fit = F.face();
            if (start) {
                start = false;
            }
            else{
                mLogStream << " ";
            }
            mLogStream << (*fit)->id();
        }
        mLogStream << "\n";
    }
}


#ifdef UNIT_TESTS


static vector<Vec3>                       debug_pointsReduced;
static vector<long>                       debug_indicesReduced;
static vector<Undirected::node_list_it_t> debug_vertices;
static Undirected::node_list_it_t         debug_vcit;
static vector<FaceIt>                     debug_conflictFaces;
static vector<HalfEdgeIt>                 debug_frontierHalfEdges;
static vector<FrontierElem>               debug_frontier;
static predicate                          debug_pred;

// Set the points and find the initial 3-simplex.
void Manifold::debugFindConvexHullStep1(vector<Vec3>& points)
{
    debug_pointsReduced.clear();
    debug_indicesReduced.clear();
    debug_vertices.clear();
    debug_conflictFaces.clear();
    debug_frontierHalfEdges.clear();
    debug_frontier.clear();

    log(INFO, __FILE__, __LINE__, "findConvexHull() BEGIN");

    if (points.size() < 4) {
        debug_pred = MAYBE_FLAT;
        return;
    }

    size_t index1;
    size_t index2;
    size_t index3;
    size_t index4;

    debug_pred = analyzePoints(points, index1, index2, index3, index4);
    if (debug_pred != NONE) {
        return;
    }

    // Find the initial 3-simplex from the 4 points
    construct3Simplex(
               points[index1], points[index2], points[index3], points[index4]);

    log(INFO, __FILE__, __LINE__, "Initial 3-simplex");
    logContents(INFO, __FILE__, __LINE__);

    // Remove the 4 points from the list, and generate conflict graph nodes.
    for (size_t i = 0; i < points.size(); i++) {
        if (i != index1 && i != index2 && i != index3 && i != index4) {
            debug_pointsReduced.push_back(points[i]);
            debug_indicesReduced.push_back(i);
        }
    }

    // For each face find conflicts.

    createInitialConflictGraph(
                   debug_pointsReduced, debug_indicesReduced, debug_vertices);

    logConflictGraph(INFO, __FILE__, __LINE__);
}


// Get the list of initial vertices to be examined.
vector<Undirected::node_list_it_t> Manifold::debugFindConvexHullVertices()
{
    return debug_vertices;
}


void Manifold::debugNumFaces(Undirected::node_list_it_t nit)
{
     auto& N = dynamic_cast<VertexConflict&>(*(*nit));
     cerr << "Num Faces: " << N.degreeIn() << "\n";
}

void Manifold::debugNumNodes()
{
    long numFaces = 0;
    long numVertices = 0;
    for (auto nit = mConflictGraph.nodes().first;
          nit != mConflictGraph.nodes().second; nit++) {
        if (typeid(*nit)==typeid(FaceConflict*)) {
            numFaces++;
        }
        else {
            numVertices++;
        }
    }
    cerr << "number of faces: " << numFaces << "\n";
    cerr << "number of vertices: " << numVertices << "\n";
}


// Loop. Set the vertex to be examined.
bool Manifold::debugFindConvexHullLoopStep1(
                              Undirected::node_list_it_t  vcit)
{
    debug_conflictFaces.clear();
    debug_frontierHalfEdges.clear();
    debug_frontier.clear();
    debug_vcit = vcit;
    debug_pred = NONE;
    auto& vc = dynamic_cast<VertexConflict&>(*(*vcit));

    log(INFO,__FILE__, __LINE__, "Start of loop.");
    logVertexConflict(INFO,__FILE__, __LINE__, vc);


    if (vc.degreeIn() == 0) {

        mConflictGraph.removeNode(vc);
        log(INFO,__FILE__, __LINE__, "End of loop");
        logContents(INFO, __FILE__, __LINE__);
        logConflictGraph(INFO, __FILE__, __LINE__);
        return false;

    }

    auto abort = findVisibleFaces(vc, debug_conflictFaces);

    if (abort) {

        mConflictGraph.removeNode(vc);
        log(INFO,__FILE__, __LINE__, "End of loop");
        logContents(INFO, __FILE__, __LINE__);
        logConflictGraph(INFO, __FILE__, __LINE__);
        return false;
    }

    debug_frontierHalfEdges = findCircumference(debug_conflictFaces, abort);

    if (abort) {

        mConflictGraph.removeNode(vc);
        log(INFO,__FILE__, __LINE__, "End of loop");
        logContents(INFO, __FILE__, __LINE__);
        logConflictGraph(INFO, __FILE__, __LINE__);
        return false;
    }

    return true;

}


void Manifold::debugFindConvexHullHalfEdges(
    Vec3&               color,
    vector<Vec3>&       vertices,
    vector<Vec3>&       colors
) {
    makeOpenGLVerticesColorsForLines(
        debug_frontierHalfEdges,
        color,
        vertices,
        colors
    );
}

// Loop. Set the vertex to be examined.
bool Manifold::debugFindConvexHullLoopStep2()
{

    //auto& vc = dynamic_cast<VertexConflict&>(*(*debug_vcit));

    debug_frontier = makeFrontier(debug_frontierHalfEdges);

    for (auto& cf : debug_conflictFaces) {

        auto& fcit = (*cf)->mFaceConflict;
        auto& fc = dynamic_cast<FaceConflict&>(*(*fcit));

        mConflictGraph.removeNode(fc);

    }

    removeFaces(debug_conflictFaces);

    return true;
}


void Manifold::debugFindConvexHullLoopStep3()
{
    auto& vc = dynamic_cast<VertexConflict&>(*(*debug_vcit));

    auto vp = makeCircularFan(debug_frontierHalfEdges, vc.p(), vc.id());

    updateConflictGraph(debug_frontier);

    checkAndMergeFacesCounterClockwise(vp);

    mConflictGraph.removeNode(vc);

    log(INFO,__FILE__, __LINE__, "End of loop");
    logContents(INFO, __FILE__, __LINE__);
    logConflictGraph(INFO, __FILE__, __LINE__);

}

Vec3 Manifold::debugCurrentPoint(
                        Undirected::node_list_it_t vcit)
{
    auto& vc = dynamic_cast<VertexConflict&>(*(*vcit));
    return vc.p();
}


void Manifold::debugFindConvexHullTerm()
{
    // Tidy up
    clearConflictGraph();

    setNormalsForVerticesAndEdges();

    constructHelperMaps();
}



#endif // UNIT_TESTS


}// namespace Makena
