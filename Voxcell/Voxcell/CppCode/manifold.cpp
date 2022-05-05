#include <map>
#include "manifold.hpp"


#ifdef UNIT_TESTS
#include "gtest/gtest_prod.h"
#endif


/**
 * @file manifold.cpp
 *
 * @reference
 *
 */
namespace Makena {

using namespace std;

HalfEdgeIt Vertex::nextIncompleteHalfEdge(HalfEdgeIt& h)
{
    auto hit = (*h)->mDstBackIt;
    auto hitVccw = hit;
    if (hitVccw == mIncidentHalfEdges.begin()) {
        hitVccw = mIncidentHalfEdges.end();
    }
    hitVccw--;

    while (hitVccw != hit ) {
        if ((*((*(*hitVccw))->mParent))->mOddCnt % 2 != 0) {
            break;
        }

        if (hitVccw == mIncidentHalfEdges.begin()) {
            hitVccw = mIncidentHalfEdges.end();
        }
        hitVccw--;
    }

    return *hitVccw;
}


list<HalfEdgeIt> Face::halfEdgesFromEdges(const list<EdgeIt>& edges)
{
    list<HalfEdgeIt> halfEdges;

    if (edges.size() ==0) {

        return halfEdges;

    }
    if (edges.size() ==1) {

        halfEdges.push_back( (*(*(edges.begin())))->mHe1 );
        halfEdges.push_back( (*(*(edges.begin())))->mHe2 );
        return halfEdges;

    }

    // Find the initial vertex.
    auto eit = edges.begin();
    auto he11Src = (*((*(*eit))->mHe1))->mSrc;
    auto he12Src = (*((*(*eit))->mHe2))->mSrc;
    eit++;
    auto he21Src = (*((*(*eit))->mHe1))->mSrc;
    auto he22Src = (*((*(*eit))->mHe2))->mSrc;

    VertexIt vSrc;
    if (he11Src == he21Src || he11Src == he22Src) {
        vSrc = he12Src;
    }
    else {
        vSrc = he11Src;
    }

    for (auto& eit : edges) {
        auto he = ((*((*eit)->mHe1))->mSrc==vSrc)?(*eit)->mHe1:(*eit)->mHe2;
        halfEdges.push_back(he);
        vSrc = (*he)->mDst;
    }

    return halfEdges;
}


FaceIt Manifold::makePolygon (const list<HalfEdgeIt>& halfEdges) {

    auto fp = make_unique<Face>();
    fp->setId(mNextIdForFeatures++);
    auto fit = mFaces.insert(mFaces.end(), std::move(fp));
//    (*fit)->mId     = mNumFaces++;
    (*fit)->mBackIt = fit;

    if (halfEdges.size() < 2) {

        return fit;
    }

    for (auto& heit : halfEdges) {
        (*heit)->mFaceBackIt = (*fit)->mIncidentHalfEdges.insert(
                                      (*fit)->mIncidentHalfEdges.end(), heit);
    }    

    HalfEdgeIt hitPrev;
    size_t edgeIndex = 0;
    vector<Vec3> points;
    for (auto& hit : (*fit)->mIncidentHalfEdges) {
        points.push_back((*((*hit)->mSrc))->pLCS());
        (*hit)->mFace = fit;

        if (edgeIndex > 0) {
            (*hit)->mPrev = hitPrev;
            (*hitPrev)->mNext = hit;

            auto pred = (*hit)->isColinear(hitPrev);
            (*hit)->mPrevPred     = pred;
            (*hitPrev)->mNextPred = pred;
        }

        hitPrev = hit;
        edgeIndex++;

    }
    HalfEdgeIt hitFirst = *((*fit)->mIncidentHalfEdges.begin());
    (*hitFirst)->mPrev = hitPrev;
    (*hitPrev)->mNext  = hitFirst;

    auto pred = (*hitFirst)->isColinear(hitPrev);
    (*hitFirst)->mPrevPred = pred;
    (*hitPrev)->mNextPred  = pred;

    // Following is to find the face normal.

    (*fit)->mPred = NONE;
    if (points.size() == 3) {
        enum predicate pred;
        (*fit)->mNormalLCS = 
                       Face::findNormal(points[0], points[1], points[2], pred);
        if (pred != NONE) {
            (*fit)->mPred = MAYBE_FLAT;
        }
    }
    else {

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
            // Ill-shaped.
            enum predicate pred;
            log(WARNING, __FILE__, __LINE__, "All points on an edge.");
            (*fit)->mNormalLCS = 
                       Face::findNormal(points[0], points[1], points[2], pred);
            if (pred!=NONE) {
                (*fit)->mPred = MAYBE_FLAT;
            }
        }

        long ind1 = std::min(xMinIndex, std::min(xMaxIndex, yAbsMaxIndex));
        long ind3 = std::max(xMinIndex, std::max(xMaxIndex, yAbsMaxIndex));
        long ind2;
        if ( (ind1 == xMinIndex && ind3 == xMaxIndex) ||
             (ind3 == xMinIndex && ind1 == xMaxIndex)    ) {
            ind2 = yAbsMaxIndex;
        }
        else if ( (ind1 == xMinIndex && ind3 == yAbsMaxIndex) ||
                  (ind3 == xMinIndex && ind1 == yAbsMaxIndex)    ) {
            ind2 = xMaxIndex;
        }
        else {
            ind2 = xMinIndex;
        }
        enum predicate pred;        
        
        (*fit)->mNormalLCS = 
              Face::findNormal(points[ind1], points[ind2], points[ind3], pred);
        if (pred != NONE) {
            (*fit)->mPred = MAYBE_FLAT;
        }
    }

    return fit;
}


HalfEdgeIt Manifold::findNextBoundaryHalfEdge(HalfEdgeIt startIt)
{
    HalfEdgeIt heit    = (*startIt)->mNext;
    HalfEdgeIt buddyit = (*heit)->mBuddy;
    while ((*buddyit)->mToBeMerged && heit != startIt) {
        heit    = (*buddyit)->mNext;
        buddyit = (*heit)->mBuddy;
    }
    return heit;

}

vector<HalfEdgeIt> Manifold::findCircumference(
    vector<FaceIt>& faces, 
    bool&           abort
) {
    vector<HalfEdgeIt> boundaryHalfEdges;

    for (auto& fit : faces) {
        for (auto& heit : (*fit)->mIncidentHalfEdges) {
            (*heit)->mToBeMerged = true;
        }
    }

    long numBoundaryHalfEdges = 0;
    HalfEdgeIt heStart = mHalfEdges.end();
    for (auto& fit : faces) {
        for (auto& heit : (*fit)->mIncidentHalfEdges) {
            if ((*heit)->mToBeMerged && !(*((*heit)->mBuddy))->mToBeMerged) {
                numBoundaryHalfEdges++;
                if (heStart == mHalfEdges.end()) {
                    heStart = heit;
                }
            }                
        }
    }
    if (numBoundaryHalfEdges < 3) {
        abort = true;

        for (auto& fit : faces) {
            for (auto& heit : (*fit)->mIncidentHalfEdges) {
                (*heit)->mToBeMerged = false;
            }
        }
        return boundaryHalfEdges;
    }

    boundaryHalfEdges.push_back(heStart);
    HalfEdgeIt heit = findNextBoundaryHalfEdge(heStart);


    while (heit!=heStart && boundaryHalfEdges.size() <= numBoundaryHalfEdges) {
        boundaryHalfEdges.push_back(heit);
        heit = findNextBoundaryHalfEdge(heit);        
    }
    for (auto& fit : faces) {
        for (auto& heit : (*fit)->mIncidentHalfEdges) {
            (*heit)->mToBeMerged = false;
        }
    }

    if (boundaryHalfEdges.size() < numBoundaryHalfEdges) {
        boundaryHalfEdges.clear();
        abort = true;
    }
    else {
        abort = false;
    }
    return boundaryHalfEdges;

}


void Manifold::removeFaces(vector<FaceIt>& faces)
{
    vector<EdgeIt> edgesToBeRemoved;

    for (auto& fit : faces) {

        for (auto& hit : (*fit)->mIncidentHalfEdges) {

            (*hit)->mFace = mFaces.end();

            if ( (*((*hit)->mBuddy))->mFace == mFaces.end() ) {

                edgesToBeRemoved.push_back((*hit)->mParent);;

            }

        }

        mFaces.erase(fit);

    }

    for (auto eit : edgesToBeRemoved) {

//        removeEdge(std::move(eit));
        removeEdge(eit);

    }

}


void Manifold::removeEdge(EdgeIt& e)
{
    //
    //    ---                             --->
    //       \                           /
    //        \            he2          /
    //         ->    -------------->  --
    //            v2                v1
    //         --    <--------------  <-
    //        /            he1          \
    //       /                           \
    //    <--                             ---
    //

    auto v1 = (*((*e)->mHe1))->mSrc;

    if ((*((*e)->mHe1))->mPrev != (*e)->mHe2) {

        auto hePrev = (*((*e)->mHe1))->mPrev;
        auto heNext = (*((*e)->mHe2))->mNext;

        (*hePrev)->mNext = heNext;
        (*heNext)->mPrev = hePrev;

        auto pred = (*hePrev)->isColinear(heNext);
        (*hePrev)->mNextPred = pred;
        (*heNext)->mPrevPred = pred;

        (*v1)->mIncidentHalfEdges.erase( (*((*e)->mHe1))->mSrcBackIt );
        (*v1)->mIncidentHalfEdges.erase( (*((*e)->mHe2))->mDstBackIt );

    }
    else {
        mVertices.erase(v1);
    }

    auto v2 = (*((*e)->mHe1))->mDst;

    if ((*((*e)->mHe1))->mNext != (*e)->mHe2) {

        auto heNext = (*((*e)->mHe1))->mNext;
        auto hePrev = (*((*e)->mHe2))->mPrev;

        (*heNext)->mPrev = hePrev;
        (*hePrev)->mNext = heNext;

        auto pred = (*hePrev)->isColinear(heNext);
        (*hePrev)->mNextPred = pred;
        (*heNext)->mPrevPred = pred;

        (*v2)->mIncidentHalfEdges.erase( (*((*e)->mHe1))->mDstBackIt );
        (*v2)->mIncidentHalfEdges.erase( (*((*e)->mHe2))->mSrcBackIt );

    }
    else {
        mVertices.erase(v2);
    }

    mHalfEdges.erase((*e)->mHe1);
    mHalfEdges.erase((*e)->mHe2);
    (*e)->mHe1 = mHalfEdges.end();
    (*e)->mHe2 = mHalfEdges.end();
    mEdges.erase(e);

}


void Manifold::constructCuboid(
    const Vec3& pFLL,
    const Vec3& pFUL,
    const Vec3& pFUR,
    const Vec3& pFLR,
    const Vec3& pBLL,
    const Vec3& pBUL,
    const Vec3& pBUR,
    const Vec3& pBLR
) {
    /*
     * Naming: {Front/Back}{Lower/Upper}{Left/Right}
     *                   Y
     *                   ^
     *                   |
     *                   |
     *                   |vBUL     eBUX       vBUR
     *                   *---------------------*
     *                  /|                    /|
     *             eXUL/ |eBXL           eXUR/ |
     *                /  |                  /  |
     *               *---+-----------------*   |eBXR
     *           vFUL|   |    eFUX     vFUR|   |
     *               |   |         eBLX    |   |
     *               |   *-----------------+---*--------->X
     *               |  /vBLL              |  /vBLR
     *           eFXL| /eXLL           eFXR| /eXLR
     *               |/                    |/
     *               *---------------------*
     *              /vFLL    eFLX          vFLR
     *             /
     *            /
     *           Z
     */
    auto vFLL = makeVertex(pFLL);
    auto vFUL = makeVertex(pFUL);
    auto vFUR = makeVertex(pFUR);
    auto vFLR = makeVertex(pFLR);
    auto vBLL = makeVertex(pBLL);
    auto vBUL = makeVertex(pBUL);
    auto vBUR = makeVertex(pBUR);
    auto vBLR = makeVertex(pBLR);

    auto eFLX = makeEdge(vFLL, vFLR);
    auto eFXR = makeEdge(vFLR, vFUR);
    auto eFUX = makeEdge(vFUR, vFUL);
    auto eFXL = makeEdge(vFUL, vFLL);

    auto eXLR = makeEdge(vFLR, vBLR);
    auto eXUR = makeEdge(vFUR, vBUR);
    auto eXUL = makeEdge(vFUL, vBUL);
    auto eXLL = makeEdge(vFLL, vBLL);

    auto eBLX = makeEdge(vBLL, vBLR);
    auto eBXR = makeEdge(vBLR, vBUR);
    auto eBUX = makeEdge(vBUR, vBUL);
    auto eBXL = makeEdge(vBUL, vBLL);

    /*auto fFront = */makeQuad(eFLX, eFXR, eFUX, eFXL);
    /*auto fBack  = */makeQuad(eBLX, eBXL, eBUX, eBXR);
    /*auto fLower = */makeQuad(eFLX, eXLL, eBLX, eXLR);
    /*auto fTop   = */makeQuad(eFUX, eXUR, eBUX, eXUL);
    /*auto fRight = */makeQuad(eXLR, eBXR, eXUR, eFXR);
    /*auto fLeft  = */makeQuad(eXLL, eFXL, eXUL, eBXL);

    (*vFLL)->pushHalfEdgesCCW(eFLX);
    (*vFLL)->pushHalfEdgesCCW(eFXL);
    (*vFLL)->pushHalfEdgesCCW(eXLL);

    (*vFLR)->pushHalfEdgesCCW(eXLR);
    (*vFLR)->pushHalfEdgesCCW(eFXR);
    (*vFLR)->pushHalfEdgesCCW(eFLX);

    (*vFUR)->pushHalfEdgesCCW(eFXR);
    (*vFUR)->pushHalfEdgesCCW(eXUR);
    (*vFUR)->pushHalfEdgesCCW(eFUX);

    (*vFUL)->pushHalfEdgesCCW(eFXL);
    (*vFUL)->pushHalfEdgesCCW(eFUX);
    (*vFUL)->pushHalfEdgesCCW(eXUL);

    (*vBLL)->pushHalfEdgesCCW(eXLL);
    (*vBLL)->pushHalfEdgesCCW(eBXL);
    (*vBLL)->pushHalfEdgesCCW(eBLX);

    (*vBLR)->pushHalfEdgesCCW(eXLR);
    (*vBLR)->pushHalfEdgesCCW(eBLX);
    (*vBLR)->pushHalfEdgesCCW(eBXR);

    (*vBUR)->pushHalfEdgesCCW(eXUR);
    (*vBUR)->pushHalfEdgesCCW(eBXR);
    (*vBUR)->pushHalfEdgesCCW(eBUX);

    (*vBUL)->pushHalfEdgesCCW(eXUL);
    (*vBUL)->pushHalfEdgesCCW(eBUX);
    (*vBUL)->pushHalfEdgesCCW(eBXL);

    constructHelperMaps();
}


void Manifold::construct3Simplex(
    const Vec3& cp1,  const Vec3& cp2,  const Vec3& cp3,  const Vec3& cp4,
    const long  ind1, const long  ind2, const long  ind3, const long ind4 
) {
    // Check the orientation of the 3 points against the rest.
    auto v12 = cp2 - cp1;
    auto v13 = cp3 - cp1;
    auto v14 = cp4 - cp1;
    auto c12_13 = v12.cross(v13);

    bool reversed = v14.dot(c12_13) < 0.0;

    // Create a triangle from 3 points
    auto v1 = makeVertex(cp1, ind1);
    auto v2 = makeVertex(reversed?cp3:cp2, reversed?ind3:ind2);
    auto v3 = makeVertex(reversed?cp2:cp3, reversed?ind2:ind3);
    auto v4 = makeVertex(cp4, ind4);

    auto e12 = makeEdge(v1, v2);
    auto e13 = makeEdge(v1, v3);
    auto e14 = makeEdge(v1, v4);
    auto e23 = makeEdge(v2, v3);
    auto e24 = makeEdge(v2, v4);
    auto e34 = makeEdge(v3, v4);

    auto f132 = makeTriangle(e13, e23, e12);

    auto f143 = makeTriangle(e14, e34, e13);

    auto f124 = makeTriangle(e12, e24, e14);

    auto f234 = makeTriangle(e23, e34, e24);

    (*v1)->pushHalfEdgesCCW(e12);
    (*v1)->pushHalfEdgesCCW(e14);
    (*v1)->pushHalfEdgesCCW(e13);

    (*v2)->pushHalfEdgesCCW(e23);
    (*v2)->pushHalfEdgesCCW(e24);
    (*v2)->pushHalfEdgesCCW(e12);

    (*v3)->pushHalfEdgesCCW(e13);
    (*v3)->pushHalfEdgesCCW(e34);
    (*v3)->pushHalfEdgesCCW(e23);

    (*v4)->pushHalfEdgesCCW(e14);
    (*v4)->pushHalfEdgesCCW(e24);
    (*v4)->pushHalfEdgesCCW(e34);

    // Find predicates among the faces.
    (*e12)->mPredFaces = (*f132)->isCoplanar(f124);
    (*e13)->mPredFaces = (*f132)->isCoplanar(f143);
    (*e14)->mPredFaces = (*f124)->isCoplanar(f143);
    (*e23)->mPredFaces = (*f132)->isCoplanar(f234);
    (*e24)->mPredFaces = (*f124)->isCoplanar(f234);
    (*e34)->mPredFaces = (*f234)->isCoplanar(f143);

    setNormalsForVerticesAndEdges();

    constructHelperMaps();
}


/** @brief adds a series of circularly arranged triangles centered at a point
 *         and around the circularly ordered half edges.
 *
 *        <----------         <----------
 *       |           ^       |\         /^
 *       |           |       |  \     /  |
 *       |           |       |    \ /    |
 *       |     *     |  ==>  |     *     |
 *       |   center  |       |    /c\    |
 *       |           |       |  /     \  |
 *       |           |       |/         \|
 *        ---------->         ---------->
 *
 *   @param halfEdges  (in): list of half edges ordered clockwise if you are
 *                           looking at them from outside of the manifold
 *                           (from the center point.)
 *
 *   @param p          (in): center point of the fan in LCS.
 *
 *   @return the new center vertex created.
 */
VertexIt Manifold::makeCircularFan(
    const vector<HalfEdgeIt>& halfEdges,
    const Vec3&               p,
    const long                id
) {
    auto vCenter = makeVertex(p, id);

    HalfEdgeIt hPrev = mHalfEdges.end();
    HalfEdgeIt hLast = mHalfEdges.end();

    for (size_t i = 0; i < halfEdges.size(); i++) {

        //            vBase
        // *<-----------*<-----------*
        // |  hBase2  /| |  hBase1   ^
        // |           | |           |
        // |     hStem2| |hStem1     |
        //  \          | |          /
        //   \         | |/        /
        //    \_______> * ________/
        //           vCenter  hPrev

        auto hBase2  = halfEdges[i];
        auto hBase1  = (*hBase2)->mPrev;
        auto vBase   = (*hBase2)->mSrc;

        auto  eStem   = makeEdge(vBase, vCenter);

        auto hStem1  = ((*((*eStem)->mHe1))->mDst == vCenter)?
                                                 (*eStem)->mHe1:(*eStem)->mHe2;
        auto hStem2  = ((*((*eStem)->mHe1))->mDst == vCenter)?
                                                 (*eStem)->mHe2:(*eStem)->mHe1;

        // Create a face.
        if (i > 0) {

             // Create a triangle with hBase1->hStem1->hPrev.
            auto fit = makeTriangle(hBase1, hStem1, hPrev);

            auto fBuddy = (*((*hBase1)->mBuddy))->mFace;

            (*((*hBase1)->mParent))->mPredFaces = (*fit)->isCoplanar(fBuddy);

            //if (i>1) {
            //
            //  fBuddy = (*((*hPrev)->mBuddy))->mFace;
            //  (*((*hPrev)->mParent))->mPredFaces=(*fit)->isCoplanar(fBuddy);
            //
            //}

        }

        if (i==halfEdges.size()-1) {

            // Create the first triangle with hBase2->hLast->hStem2
            auto fit = makeTriangle(hBase2, hLast, hStem2);

            auto fBuddy = (*((*hBase2)->mBuddy))->mFace;

            (*((*hBase2)->mParent))->mPredFaces = (*fit)->isCoplanar(fBuddy);

            fBuddy = (*((*hLast)->mBuddy))->mFace;

            (*((*hLast)->mParent))->mPredFaces = (*fit)->isCoplanar(fBuddy);

            //fBuddy = (*((*hStem2)->mBuddy))->mFace;

            //(*((*hStem2)->mParent))->mPredFaces = (*fit)->isCoplanar(fBuddy);

        }

        // Insert hStem1 & 2 to vBase.
        (*hStem2)->mDstBackIt = (*vBase)->mIncidentHalfEdges.insert(
                                                (*hBase1)->mDstBackIt, hStem2);

        (*hStem1)->mSrcBackIt = (*vBase)->mIncidentHalfEdges.insert(
                                                (*hBase1)->mDstBackIt, hStem1);

        // Insert hStem1 & 2 to vCenter.
        (*hStem1)->mDstBackIt = (*vCenter)->mIncidentHalfEdges.insert(
                               (*vCenter)->mIncidentHalfEdges.end(), hStem1);

        (*hStem2)->mSrcBackIt = (*vCenter)->mIncidentHalfEdges.insert(
                               (*vCenter)->mIncidentHalfEdges.end(), hStem2);

        hPrev = hStem2;

        if (i == 0) {

            hLast = hStem1;

        }
    }
    return vCenter;
}


Manifold::Martialled Manifold::exportData() {

    Martialled M;

    M.mId = mId;

    for (auto vit = mVertices.begin(); vit != mVertices.end(); vit++) {

        M.mPoints [(*vit)->mId] = (*vit)->mPointLCS;
        M.mNormals[(*vit)->mId] = (*vit)->mNormalLCS;
        vector<long> ids;
        for (auto& heit : (*vit)->mIncidentHalfEdges) {
            if ((*heit)->mSrc == vit) {
                ids.push_back((*((*heit)->mDst))->mId);
            }
        }
        M.mIncidentVertices[(*vit)->mId] = std::move(ids);
    }

    for (auto eit = mEdges.begin(); eit != mEdges.end(); eit++) {
        auto& heit1 = (*eit)->mHe1;
        auto& heit2 = (*eit)->mHe2;
        M.mEdgeNormals[make_pair((*((*heit1)->mSrc))->mId, 
                                 (*((*heit2)->mSrc))->mId) ]
                       = (*eit)->mNormalLCS;
    }

    for (auto fit = mFaces.begin(); fit != mFaces.end(); fit++) {
        M.mFaceNormals[(*fit)->mId] = (*fit)->mNormalLCS;
        vector<long> ids;
        for (auto heit : (*fit)->mIncidentHalfEdges) {
            ids.push_back((*((*heit)->mSrc))->mId);
        }
        M.mFaceVertices[(*fit)->mId] = std::move(ids);
    }

    return M;
}



void Manifold::importData(Martialled& M) {

    mVertices.clear();
    mEdges.clear();
    mHalfEdges.clear();
    mFaces.clear();
    mNumFaces          = 0;
    mPred              = NONE;
    mNextIdForFeatures = 0;
    mId                = M.mId;

    std::map<long, VertexIt> vertices;
    for (auto pit = M.mPoints.begin(); pit != M.mPoints.end(); pit++) {
        const auto  id = pit->first;
        const auto& p  = pit->second;

        auto vp = make_unique<Vertex>(p);
        auto vit = mVertices.insert(mVertices.end(), std::move(vp));
        (*vit)->setId(id);
        (*vit)->mBackIt    = vit;
        (*vit)->mNormalLCS = M.mNormals[pit->first];
        (*vit)->mNormalLCS.normalize();
        vertices[id]=vit;
        mNextIdForFeatures = max(mNextIdForFeatures,id+1);
    }

    std::map< pair<long,long>, EdgeIt > edges;

    for (auto enit = M.mEdgeNormals.begin(); enit != M.mEdgeNormals.end();
                                                                      enit++) {

        auto& v1 = vertices[enit->first.first];
        auto& v2 = vertices[enit->first.second];
        auto& n  = enit->second;

        auto ep = make_unique<Edge>();
        auto eit = mEdges.insert(mEdges.end(), std::move(ep));
        (*eit)->mBackIt = eit;
        edges[enit->first] = eit;
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

        (*eit)->mNormalLCS = n;
        (*eit)->mNormalLCS.normalize();

        mNumFaces++;
    }

    for (auto fvit = M.mFaceVertices.begin(); fvit != M.mFaceVertices.end();
                                                                      fvit++) {
        long prev = fvit->second[fvit->second.size()-1];

        list<HalfEdgeIt> halfEdges;

        for (auto vn : fvit->second) {

            auto vit = vertices[prev];
            auto k1 = make_pair( prev, vn   );
            auto k2 = make_pair( vn,   prev );

            EdgeIt eit;

            if (edges.find(k1)!=edges.end()) {
                eit = edges[k1];
            }
            else {
                eit = edges[k2];
            }

            if (vit == (*((*eit)->he1()))->src()) {
                halfEdges.push_back((*eit)->he1());
            }
            else {
                halfEdges.push_back((*eit)->he2());
            }
            prev = vn;

        }
        auto fit = makePolygon(halfEdges);
        (*fit)->mNormalLCS = M.mFaceNormals[fvit->first];

    }

    for (auto& iit : M.mIncidentVertices) {

        auto& vit = vertices[iit.first];     
        auto& ids = iit.second;

        for (auto vid : ids) {

            auto k1 = make_pair( vid,       iit.first );
            auto k2 = make_pair( iit.first, vid        );
            EdgeIt eit;
            if (edges.find(k1)!=edges.end()) {
                eit = edges[k1];
            }
            else {
                eit = edges[k2];
            }
            if (vit == (*((*eit)->he1()))->src()) {
                (*vit)->mIncidentHalfEdges.push_back((*eit)->he2());
                (*vit)->mIncidentHalfEdges.push_back((*eit)->he1());
            }
            else {
                (*vit)->mIncidentHalfEdges.push_back((*eit)->he1());
                (*vit)->mIncidentHalfEdges.push_back((*eit)->he2());
            }
        }
    }
    constructHelperMaps();
}


Manifold::Martialled Manifold::parseTextData(std::istream& is)
{
    long            lineNumber = 0;
    bool            error      = false;
    const std::string ID       = "ID";
    const std::string VERTICES = "VERTICES";
    const std::string EDGES    = "EDGES";
    const std::string FACES    = "FACES";

    const std::string ERR_ID       = "Manifold::importData(Error ID)";
    const std::string ERR_VERTICES = "Manifold::importData(Error VERTICES)";
    const std::string ERR_EDGES    = "Manifold::importData(Error EDGES)";
    const std::string ERR_FACES    = "Manifold::importData(Error FACES)";
    const std::string ERR_DEFAULT  = "Manifold::importData(Error DEFAULT)";

    enum parseState {
        INIT,
        IN_ID,
        IN_VERTICES,
        IN_EDGES,
        IN_FACES,
        END
    } state = INIT;

    
    Martialled M;

    while (!is.eof() && !error) {

        std::vector<std::string> fields = processLine(is);
        lineNumber++;
        
        if (fields.size()==0) { continue; }
        if (fields.size()==1) {
            if (fields[0].compare     (0, ID.size(), ID)==0) {
                state = IN_ID;
                continue;
            }
            else if (fields[0].compare(0, VERTICES.size(), VERTICES)==0) {
                state = IN_VERTICES;
                continue;
            }
            else if (fields[0].compare(0, EDGES.size(),    EDGES)==0) {
                state = IN_EDGES;
                continue;
            }
            else if (fields[0].compare(0, FACES.size(),    FACES)==0) {
                state = IN_FACES;
                continue;
            }
        }
        switch (state) {
          case IN_ID:
            {
                if (fields.size()!=1) { 
                    throw std::logic_error(ERR_ID);
                }
                M.mId = std::stol(fields[0]); // id
            }
            break;

          case IN_VERTICES:
            {
                if (fields.size()<7) { 
                    throw std::logic_error(ERR_VERTICES);
                }
                long   id = std::stol(fields[0]); // id
                double px = std::stof(fields[1]); // point x
                double py = std::stof(fields[2]); // point y
                double pz = std::stof(fields[3]); // point z 
                double nx = std::stof(fields[4]); // normal x
                double ny = std::stof(fields[5]); // normal y
                double nz = std::stof(fields[6]); // normal z 
                Vec3 p(px, py, pz), n(nx, ny, nz);
                M.mPoints[id]  = p;
                M.mNormals[id] = n;
                vector <long> ids;
                for (long i = 7; i < fields.size(); i++) {
                    long vid = std::stol(fields[i]);
                    ids.push_back(vid);
                }
                M.mIncidentVertices[id] = std::move(ids);

            }
            break;
          case IN_EDGES:
            {
                if (fields.size()!=5) { 
                    throw std::logic_error(ERR_EDGES);
                }
                long  id1 = std::stol(fields[0]); // id
                long  id2 = std::stol(fields[1]); // id
                double nx = std::stof(fields[2]); // normal x
                double ny = std::stof(fields[3]); // normal y
                double nz = std::stof(fields[4]); // normal z 
                Vec3 n(nx, ny, nz);
                M.mEdgeNormals[make_pair(id1,id2)] = n;
            }
            break;
          case IN_FACES:
            {
                if (fields.size()<4) { 
                    throw std::logic_error(ERR_FACES);
                }
                long   id = std::stol(fields[0]); // id
                double nx = std::stof(fields[1]); // normal x
                double ny = std::stof(fields[2]); // normal y
                double nz = std::stof(fields[3]); // normal z 
                Vec3 n(nx, ny, nz);
                M.mFaceNormals[id] = n;
                vector <long> ids;
                for (long i = 4; i < fields.size(); i++) {
                    long vid = std::stol(fields[i]);
                    ids.push_back(vid);
                }
                M.mFaceVertices[id] = std::move(ids);
            }
            break;
          default:
            throw std::logic_error(ERR_DEFAULT);
            break;
        }
    }

    return M;

}


std::vector<std::string> Manifold::processLine(std::istream& is)
{
    std::vector<std::string> fields;
    std::string line;
    std::getline(is, line);
    if (line.empty())      { return fields; }
    if (line.at(0) == '#') { return fields; }
    if ((line[line.size()-1] == '\n')||(line[line.size()- 1] == '\r')) {
        line.erase(line.size() - 1);
    }
    if ((line[line.size()-1] == '\n')||(line[line.size()- 1] == '\r')) {
        line.erase(line.size() - 1);
    }
    if (line.find_first_not_of(' ') == std::string::npos) { return fields; }
    return splitLine(line, '\t');
}


std::vector<std::string> Manifold::splitLine(const string& txt, const char& ch)
{
    auto   pos        = txt.find( ch );
    size_t initialPos = 0;
    std::vector<std::string> strs;
    while( pos != std::string::npos && initialPos < txt.size()) {
        if (pos > initialPos) {
            strs.push_back( txt.substr( initialPos, pos - initialPos) );
        }
        initialPos = pos + 1;
        if (initialPos < txt.size()) {
            pos = txt.find( ch, initialPos );
        }
    }
    if(initialPos < txt.size()) {
        strs.push_back( txt.substr( initialPos, txt.size() - initialPos) );
    }
    return strs;
}


void Manifold::emitText(Martialled& M, std::ostream& os)
{

    os << "ID\n" << M.mId << "\n";
    os << "VERTICES\n";
    os << "#ID\tPoint LCS\tNormal LCS\t Incident Vertex 1\t"
       << "Incident Vertex 2...\tIncident Vertex N\n";

    for (auto pit : M.mPoints) {
        auto id = pit.first;
        auto& p = pit.second;
        auto& n = M.mNormals[id];
        os << id    << "\t";
        os << p.x() << "\t";
        os << p.y() << "\t";
        os << p.z() << "\t";
        os << n.x() << "\t";
        os << n.y() << "\t";
        os << n.z();
        for (auto& iid : M.mIncidentVertices[id]) {
            os << "\t";
            os << iid;
        }
        os << "\n";     
    }

    os << "EDGES\n";
    os << "#Vertex 1\tVertex 2\tNormal LCS\n";

    for (auto& pit : M.mEdgeNormals) {
        os << pit.first.first << "\t";
        os << pit.first.second << "\t";
        os << pit.second.x()   << "\t";
        os << pit.second.y()   << "\t";
        os << pit.second.z()   << "\t";
        os << "\n";     
    }

    os << "FACES\n";
    os << "#ID\tNormal LCS\t Vertex 1\tVertex 2...\tVertex X\n";

    for (auto& pit : M.mFaceNormals) {
        auto& n = pit.second;
        os << pit.first << "\t";
        os << n.x() << "\t";
        os << n.y() << "\t";
        os << n.z();
        auto& ids = M.mFaceVertices[pit.first];
        for (auto& id : ids) {
            os << "\t";
            os << id;
        }
        os << "\n";     
    }
}


void Manifold::constructHelperMaps()
{
    mVertexIdToVertex.clear();
    for (auto vit = mVertices.begin(); vit != mVertices.end(); vit++) {
        (*vit)->mFaceCounts.clear();
        for (auto heit : (*vit)->mIncidentHalfEdges) {
            if ((*heit)->dst()==vit) {
                (*vit)->mFaceCounts[(*(*heit)->face())->id()] = 1;
            }
        }
        mVertexIdToVertex[(*vit)->id()] = vit;
    }

    mEdgeIdToEdge.clear();
    mVertexPairToEdge.clear();
    for (auto eit = mEdges.begin(); eit != mEdges.end(); eit++) {
        auto heit = (*eit)->he1();
        long id1 = (*(*heit)->src())->id();
        long id2 = (*(*heit)->dst())->id();
        if (id1 > id2){
            swap(id1,id2);
        }
        auto p = make_pair(id1, id2);
        mEdgeIdToEdge[p] = eit;
        mVertexPairToEdge[p]=eit;
    }

    mFaceIdToFace.clear();
    for (auto fit = mFaces.begin(); fit != mFaces.end(); fit++) {
        mFaceIdToFace[(*fit)->id()]=fit;
    }

}


FaceIt Manifold::findFace(
    VertexIt& v1,
    VertexIt& v2,
    VertexIt& v3,
    bool&     found
) {
    auto size1 = (*v1)->mFaceCounts.size();
    auto size2 = (*v2)->mFaceCounts.size();
    auto size3 = (*v3)->mFaceCounts.size();


    if (size1 > size2 && size1 > size3) {
        return findFaceOrdered(v1, v2, v3, found);
    }
    if (size2 > size1 && size2 > size3) {
        return findFaceOrdered(v2, v3, v1, found);
    }
    else {
        return findFaceOrdered(v3, v1, v2, found);
    }
}


FaceIt Manifold::findFaceOrdered(
    VertexIt& v1,
    VertexIt& v2,
    VertexIt& v3,
    bool&     found
) {
    for (auto& pair : (*v1)->mFaceCounts) {
        pair.second = 1;
    }
    for (auto& pair : (*v2)->mFaceCounts) {
        auto it = (*v1)->mFaceCounts.find(pair.first);
        if (it != (*v1)->mFaceCounts.end()) {
            it->second++;
        }
    }
    for (auto& pair : (*v3)->mFaceCounts) {
        auto it = (*v1)->mFaceCounts.find(pair.first);
        if (it != (*v1)->mFaceCounts.end()) {
            if (it->second==2) {
                found = true;
                return mFaceIdToFace[it->first];
            }
        }
    }

    found = false;
    return mFaces.end();
}


void Manifold::constructDefaultTextureCoordinates()
{
    for (auto fit = mFaces.begin(); fit != mFaces.end(); fit++) {
        const Vec3& normal = (*fit)->nLCS();
        Vec3 west = normal.cross(Vec3(0.0, 1.0, 0.0));
        if (west.squaredNorm2() < EPSILON_SQUARED) {
            // (0,1,0) is almost parallel to the face normal.
            // Use (1,0,0) instead.
            west = normal.cross(Vec3(1.0, 0.0, 0.0));
        }
        const Vec3 north = west.cross(normal);
        // (north, west, normal) forms a right-hand coordinate system.
        Mat3x3 rotMat(north, west, normal);
        rotMat.transposeInPlace();
        for (auto heit : (*fit)->halfEdges()) {

            auto vSrc = (*heit)->src();
            auto pSrc = (*vSrc)->pGCS(rotMat);
            (*heit)->mTextureUVsrc.setX(pSrc.x());
            (*heit)->mTextureUVsrc.setY(pSrc.y());

            auto vDst = (*heit)->dst();
            auto pDst = (*vDst)->pGCS(rotMat);
            (*heit)->mTextureUVdst.setX(pDst.x());
            (*heit)->mTextureUVdst.setY(pDst.y());

        }
        (*fit)->mTextureID = 0;
    }

}


#ifdef UNIT_TESTS


void Manifold::makeOpenGLVerticesColorsNormalsForTriangles(
    Vec3&         color,
    vector<Vec3>& vertices,
    vector<Vec3>& colors,
    vector<Vec3>& normals,
    bool          bothSides
) {
    for (auto& fp : mFaces) {

        Vec3 baseP;
        Vec3 prevP;
        size_t index = 0;

        for (auto& heit : fp->mIncidentHalfEdges) {

            if (index == 0) {

                baseP = (*((*heit)->mSrc))->mPointLCS;

            }
            else if (index == 1) {

                prevP = (*((*heit)->mSrc))->mPointLCS;

            }
            else {

                auto curP = (*((*heit)->mSrc))->mPointLCS;


                if (bothSides) {

                    // Draw the flip side as well
                    vertices.push_back(baseP);
                    vertices.push_back(curP);
                    vertices.push_back(prevP);


                    colors.push_back(color);
                    colors.push_back(color);
                    colors.push_back(color);

                    Vec3 n2 = fp->mNormalLCS;

                    n2.scale(-1.0);
                    normals.push_back(n2);
                    normals.push_back(n2);
                    normals.push_back(n2);

                }

                vertices.push_back(baseP);
                vertices.push_back(prevP);
                vertices.push_back(curP);

                colors.push_back(color);
                colors.push_back(color);
                colors.push_back(color);

                normals.push_back(fp->mNormalLCS);
                normals.push_back(fp->mNormalLCS);
                normals.push_back(fp->mNormalLCS);

                prevP = curP;

            }

            index++;

        }

    }

}



void Manifold::makeOpenGLVerticesColorsNormalsForTriangles(
    Vec3&         color,
    float         alpha,
    vector<Vec3>& vertices,
    vector<Vec3>& colors,
    vector<float>&alphas,
    vector<Vec3>& normals,
    bool          bothSides
) {

    for (auto& fp : mFaces) {

        Vec3 baseP;
        Vec3 prevP;
        size_t index = 0;

        for (auto& heit : fp->mIncidentHalfEdges) {

            if (index == 0) {
                baseP = (*((*heit)->mSrc))->pLCS();
            }
            else if (index == 1) {
                prevP = (*((*heit)->mSrc))->pLCS();
            }
            else {
                auto curP = (*((*heit)->mSrc))->pLCS();

                if (bothSides) {

                    // Draw the flip side as well
                    vertices.push_back(baseP);
                    vertices.push_back(curP);
                    vertices.push_back(prevP);


                    colors.push_back(color);
                    colors.push_back(color);
                    colors.push_back(color);
                    alphas.push_back(alpha);
                    alphas.push_back(alpha);
                    alphas.push_back(alpha);

                    Vec3 n2 = fp->nLCS();
                    n2.scale(-1.0);
                    normals.push_back(n2);
                    normals.push_back(n2);
                    normals.push_back(n2);

                }

                vertices.push_back(baseP);
                vertices.push_back(prevP);
                vertices.push_back(curP);

                colors.push_back(color);
                colors.push_back(color);
                colors.push_back(color);
                alphas.push_back(alpha);
                alphas.push_back(alpha);
                alphas.push_back(alpha);

                normals.push_back(fp->nLCS());
                normals.push_back(fp->nLCS());
                normals.push_back(fp->nLCS());

                prevP = curP;
            }
            index++;
        }
    }
}


void Manifold::makeOpenGLVerticesColorsNormalsForTriangles(
    Vec3&         color,
    vector<Vec3>& vertices,
    vector<Vec3>& colors,
    vector<Vec3>& normals,
    bool          bothSides,
    const Mat3x3& rot,
    const Vec3&   CoM
) {
    makeOpenGLVerticesColorsNormalsForTriangles(    
        color,
        vertices,
        colors,
        normals,
        bothSides,
        1.0,
        rot,
        CoM );
}

void Manifold::makeOpenGLVerticesColorsNormalsForTriangles(
    Vec3&         color,
    vector<Vec3>& vertices,
    vector<Vec3>& colors,
    vector<Vec3>& normals,
    bool          bothSides,
    const double& scaling,
    const Mat3x3& rot,
    const Vec3&   CoM
) {

    for (auto& fp : mFaces) {

        Vec3 baseP;
        Vec3 prevP;
        size_t index = 0;

        for (auto& heit : fp->mIncidentHalfEdges) {

            if (index == 0) {
                baseP = (*((*heit)->mSrc))->pGCS(scaling, rot, CoM);
            }
            else if (index == 1) {
                prevP = (*((*heit)->mSrc))->pGCS(scaling, rot, CoM);
            }
            else {
                auto curP = (*((*heit)->mSrc))->pGCS(scaling, rot, CoM);

                if (bothSides) {

                    // Draw the flip side as well
                    vertices.push_back(baseP);
                    vertices.push_back(curP);
                    vertices.push_back(prevP);


                    colors.push_back(color);
                    colors.push_back(color);
                    colors.push_back(color);

                    Vec3 n2 = fp->nGCS(rot);
                    n2.scale(-1.0);
                    normals.push_back(n2);
                    normals.push_back(n2);
                    normals.push_back(n2);
                }

                vertices.push_back(baseP);
                vertices.push_back(prevP);
                vertices.push_back(curP);

                colors.push_back(color);
                colors.push_back(color);
                colors.push_back(color);

                normals.push_back(fp->nGCS(rot));
                normals.push_back(fp->nGCS(rot));
                normals.push_back(fp->nGCS(rot));

                prevP = curP;
            }

            index++;

        }

    }

}



void Manifold::makeOpenGLVerticesColorsNormalsForTriangles(
    Vec3&         color,
    float         alpha,
    vector<Vec3>& vertices,
    vector<Vec3>& colors,
    vector<float>&alphas,
    vector<Vec3>& normals,
    bool          bothSides,
    const Mat3x3& rot,
    const Vec3&   CoM
) {
    makeOpenGLVerticesColorsNormalsForTriangles(
        color,
        alpha,
        vertices,
        colors,
        alphas,
        normals,
        bothSides,
        1.0,
        rot,
        CoM );
}



void Manifold::makeOpenGLVerticesColorsNormalsForTriangles(
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
) {

    for (auto& fp : mFaces) {

        Vec3 baseP;
        Vec3 prevP;
        size_t index = 0;

        for (auto& heit : fp->mIncidentHalfEdges) {

            if (index == 0) {
                baseP = (*((*heit)->mSrc))->pGCS(scaling, rot, CoM);
            }
            else if (index == 1) {
                prevP = (*((*heit)->mSrc))->pGCS(scaling, rot, CoM);
            }
            else {
                auto curP = (*((*heit)->mSrc))->pGCS(scaling, rot, CoM);

                if (bothSides) {

                    // Draw the flip side as well
                    vertices.push_back(baseP);
                    vertices.push_back(curP);
                    vertices.push_back(prevP);


                    colors.push_back(color);
                    colors.push_back(color);
                    colors.push_back(color);
                    alphas.push_back(alpha);
                    alphas.push_back(alpha);
                    alphas.push_back(alpha);

                    Vec3 n2 = fp->nGCS(rot);
                    n2.scale(-1.0);
                    normals.push_back(n2);
                    normals.push_back(n2);
                    normals.push_back(n2);

                }

                vertices.push_back(baseP);
                vertices.push_back(prevP);
                vertices.push_back(curP);

                colors.push_back(color);
                colors.push_back(color);
                colors.push_back(color);
                alphas.push_back(alpha);
                alphas.push_back(alpha);
                alphas.push_back(alpha);

                normals.push_back(fp->nGCS(rot));
                normals.push_back(fp->nGCS(rot));
                normals.push_back(fp->nGCS(rot));

                prevP = curP;
            }

            index++;

        }

    }

}


void Manifold::makeOpenGLVerticesColorsNormalsForTriangles(
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
) {
    makeOpenGLVerticesColorsNormalsForTriangles(
        fit,
        color,
        alpha,
        vertices,
        colors,
        alphas,
        normals,
        bothSides,
        1.0,
        rot,
        CoM
    );
}


void Manifold::makeOpenGLVerticesColorsNormalsForTriangles(
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
) {

    Vec3 baseP;
    Vec3 prevP;
    size_t index = 0;

    for (auto& heit : (*fit)->mIncidentHalfEdges) {

        if (index == 0) {
            baseP = (*((*heit)->mSrc))->pGCS(scaling, rot, CoM);
        }
        else if (index == 1) {
            prevP = (*((*heit)->mSrc))->pGCS(scaling, rot, CoM);
        }
        else {
            auto curP = (*((*heit)->mSrc))->pGCS(scaling, rot, CoM);

            if (bothSides) {

                // Draw the flip side as well
                vertices.push_back(baseP);
                vertices.push_back(curP);
                vertices.push_back(prevP);


                colors.push_back(color);
                colors.push_back(color);
                colors.push_back(color);
                alphas.push_back(alpha);
                alphas.push_back(alpha);
                alphas.push_back(alpha);

                Vec3 n2 = (*fit)->nGCS(rot);
                n2.scale(-1.0);
                normals.push_back(n2);
                normals.push_back(n2);
                normals.push_back(n2);

            }

            vertices.push_back(baseP);
            vertices.push_back(prevP);
            vertices.push_back(curP);

            colors.push_back(color);
            colors.push_back(color);
            colors.push_back(color);
            alphas.push_back(alpha);
            alphas.push_back(alpha);
            alphas.push_back(alpha);

            normals.push_back((*fit)->nGCS(rot));
            normals.push_back((*fit)->nGCS(rot));
            normals.push_back((*fit)->nGCS(rot));

            prevP = curP;
        }

        index++;

    }
}


void Manifold::makeOpenGLVerticesColorsForLines(
    vector<HalfEdgeIt>& halfEdges,
    Vec3&               color,
    vector<Vec3>&       vertices,
    vector<Vec3>&       colors,
    const Mat3x3&       rot,
    const Vec3&         CoM
) {
    makeOpenGLVerticesColorsForLines(
        halfEdges,
        color,
        vertices,
        colors,
        1.0,
        rot,
        CoM
    );
}

void Manifold::makeOpenGLVerticesColorsForLines(
    vector<HalfEdgeIt>& halfEdges,
    Vec3&               color,
    vector<Vec3>&       vertices,
    vector<Vec3>&       colors,
    const double&       scaling,
    const Mat3x3&       rot,
    const Vec3&         CoM
) {
    for (auto& heit : halfEdges) {

        Vec3 p1 = (*((*heit)->mSrc))->pGCS(scaling, rot, CoM);
        Vec3 p2 = (*((*heit)->mDst))->pGCS(scaling, rot, CoM);
        vertices.push_back(p1);
        vertices.push_back(p2);
        colors.push_back(color);
        colors.push_back(color);
    }
}


void Manifold::makeOpenGLVerticesColorsForLines(
    vector<HalfEdgeIt>& halfEdges,
    Vec3&               color,
    vector<Vec3>&       vertices,
    vector<Vec3>&       colors
) {
    for (auto& heit : halfEdges) {

        Vec3 p1 = (*((*heit)->mSrc))->pLCS();
        Vec3 p2 = (*((*heit)->mDst))->pLCS();
        vertices.push_back(p1);
        vertices.push_back(p2);
        colors.push_back(color);
        colors.push_back(color);
    }
}


void Manifold::makeOpenGLVerticesColorsForPoints(
    vector<Vec3>& points,
    Vec3&         color,
    vector<Vec3>& vertices,
    vector<Vec3>& colors
) {
    for (auto& p : points) {
        vertices.push_back(p);
        colors.push_back(color);
    }
}


void Manifold::makeOpenGLVerticesColorsForTriangleWireFrame(
    Vec3&         color,
    vector<Vec3>& vertices,
    vector<Vec3>& colors
) {

    for (auto& fp : mFaces) {

        vector<HalfEdgeIt> halfEdgesList(
                 fp->mIncidentHalfEdges.begin(), fp->mIncidentHalfEdges.end());

        makeOpenGLVerticesColorsForLines(halfEdgesList,color,vertices,colors);
    }
}


void Manifold::makeOpenGLVerticesColorsForTriangleWireFrame(
    Vec3&         color,
    vector<Vec3>& vertices,
    vector<Vec3>& colors,
    const double& scaling,
    const Mat3x3& rot,
    const Vec3&   CoM
) {

    for (auto& fp : mFaces) {

        vector<HalfEdgeIt> halfEdgesList(
                 fp->mIncidentHalfEdges.begin(), fp->mIncidentHalfEdges.end());

        makeOpenGLVerticesColorsForLines(
            halfEdgesList,color,vertices,colors, scaling, rot, CoM);

    }
}


#endif

}// namespace Makena


