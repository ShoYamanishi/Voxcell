#include "primitives.hpp"
#include "orienting_bounding_box.hpp"
#include "convex_hull_2d.hpp"

#ifdef UNIT_TESTS
#include "gtest/gtest_prod.h"
#endif


/**
 * @file orienting_bounding_box.cpp
 *
 * @brief finds the optimum orienting bounding box
 *
 * @reference
 *
 */
namespace Makena {

using namespace std;


/** @brief finds the origented bounding box for the given convex hull
 *         in 2-space spanned by Y and Z axes.
 *
 *  @param CH         (in):  points along the convex hull ccw.
 *
 *  @param axis1      (out): the primary axis for the OBB.
 *
 *  @param axis2      (out): the secondary axis for the OBB
 *
 *  @param lowerLeft  (out): the lower left point of the OBB
 *
 *  @param upperLeft  (out): the upper left point of the OBB
 *
 *  @param upperRight (out): the upper right point of the OBB
 *
 *  @param lowerRight (out): the lower right point of the OBB
 *
 *  @param extent1    (out): the length of the box along axis 1
 *
 *  @param extent2    (out): the length of the box along axis 2
 *
 *  @param area        (out): area of the box.
 */
void findOBB2D(
    std::vector<Vec3>& CH,
    Vec3&              axis1,
    Vec3&              axis2,
    Vec3&              lowerLeft,
    Vec3&              upperLeft,
    Vec3&              upperRight,
    Vec3&              lowerRight,
    double&            extent1,
    double&            extent2,
    double&            area
) {
    for (size_t i = 0 ; i < CH.size(); i++) {
        size_t j = (i < (CH.size()-1))?i+1:0;
        Vec3 ax0(1.0, 0.0, 0.0);
        Vec3 ax1 = CH[i] - CH[j];
        ax1.setX(0.0);
        if (ax1.squaredNorm2() < EPSILON_SQUARED) {
            continue;
        }
        ax1.normalize();
        Vec3 ax2(0.0, -1.0 * ax1.z(), ax1.y());
        Mat3x3 Minv(ax0, ax1, ax2);
        Mat3x3 Mrot = Minv.transpose();
        std::vector<Vec3> rotatedCH;
        Vec3 pMin;
        Vec3 pMax;
        for (size_t k = 0; k < CH.size(); k++) {
            auto rotP = Mrot * CH[k];
            if (k ==0) {
                pMin = rotP;
                pMax = rotP;
            }
            else {
                if (pMin.y() > rotP.y()) {
                    pMin.setY(rotP.y());
                }
                if (pMin.z() > rotP.z()) {
                    pMin.setZ(rotP.z());
                }
                if (pMax.y() < rotP.y()) {
                    pMax.setY(rotP.y());
                }
                if (pMax.z() < rotP.z()) {
                    pMax.setZ(rotP.z());
                }
            }
        }

        double curArea = (pMax.y() - pMin.y())*(pMax.z() - pMin.z());

        if (i == 0 || area > curArea) {
            area = curArea;
            axis1 = ax1;
            axis2 = ax2;
            Vec3 lowerLeftR (0.0, pMax.y(), pMin.z());
            Vec3 upperLeftR (0.0, pMax.y(), pMax.z());
            Vec3 upperRightR(0.0, pMin.y(), pMax.z());
            Vec3 lowerRightR(0.0, pMin.y(), pMin.z());
            lowerLeft =  Minv * lowerLeftR;
            upperLeft =  Minv * upperLeftR;
            upperRight = Minv * upperRightR;
            lowerRight = Minv * lowerRightR;
            extent1 = pMax.y() - pMin.y();
            extent2 = pMax.z() - pMin.z();
        }
    }
}


static Mat3x3 findRotationMatrixFromNormal(Vec3& n)
{
    Vec3 axisX(1.0, 0.0, 0.0);
    Vec3 axisY(0.0, 1.0, 0.0);
    Vec3 axisZ(0.0, 0.0, 1.0);

    auto cr_X = axisX.cross(n);
    auto cr_Y = axisY.cross(n);
    auto cr_Z = axisZ.cross(n);
    auto sq_X = cr_X.squaredNorm2();
    auto sq_Y = cr_Y.squaredNorm2();
    auto sq_Z = cr_Z.squaredNorm2();

    Mat3x3 M;

    if (sq_X > sq_Y) {
        if (sq_X > sq_Z) {
            cr_X.normalize();
            auto crRest = n.cross(cr_X);
            crRest.normalize();
            Mat3x3 cM(n, cr_X, crRest);
            M = cM;
        }
        else{
            cr_Z.normalize();
            auto crRest = n.cross(cr_Z);
            crRest.normalize();
            Mat3x3 cM(n, cr_Z, crRest);
            M = cM;
        }
    }
    else if (sq_Y > sq_Z) {
            cr_Y.normalize();
        auto crRest = n.cross(cr_Y);
        crRest.normalize();
        Mat3x3 cM(n, cr_Y, crRest);
        M = cM;
    }
    else {
        cr_Z.normalize();
        auto crRest = n.cross(cr_Z);
        crRest.normalize();
        Mat3x3 cM(n, cr_Z, crRest);
        M = cM;
    }
    return M.transpose();
}


static void findMinMaxAlongX(vector<Vec3>& points, double& xMin, double& xMax)
{
    for (size_t i = 0; i < points.size(); i++) {
        auto& p = points[i];
        if (i == 0) {
            xMin = p.x();
            xMax = p.x();
        }
        else {
            xMin = std::min(xMin, p.x());
            xMax = std::max(xMax, p.x());
        }
    }
}


void findOBB3D(
    Manifold& convexHull,
    Manifold& obb,
    Mat3x3&   axes,
    Vec3&     center,
    Vec3&     extents,
    double&   volume
) {
    // Gather points

    Vec3 frontLowerLeft;
    Vec3 frontUpperLeft;
    Vec3 frontUpperRight;
    Vec3 frontLowerRight;

    Vec3 backLowerLeft;
    Vec3 backUpperLeft;
    Vec3 backUpperRight;
    Vec3 backLowerRight;

    vector<Vec3> points = convexHull.getPointsLCS();
    if (points.empty()) {
        return;
    }

    vector<Vec3> faceNormals = convexHull.getFaceNormalsOriginal();
    for (size_t i = 0; i < faceNormals.size(); i++) {

        auto& n = faceNormals[i];
        Mat3x3 Mrot = findRotationMatrixFromNormal(n);

        vector<Vec3> rotatedPoints;
        for (auto& p : points) {
            rotatedPoints.push_back(Mrot * p);
        }

        double xMin, xMax;
        findMinMaxAlongX(rotatedPoints, xMin, xMax);

        Vec3    axisY;
        Vec3    axisZ;
        Vec3    backLower;
        Vec3    backUpper;
        Vec3    frontUpper;
        Vec3    frontLower;
        double  extent1;
        double  extent2;
        double  area;

        vector<long> convexHullYZind = findConvexHull2D(rotatedPoints);
        vector<Vec3> convexHullYZ;
        for (auto j :convexHullYZind) {
            convexHullYZ.push_back(rotatedPoints[j]);
        }

        findOBB2D(
            convexHullYZ,
            axisY,
            axisZ,
            backLower,
            backUpper,
            frontUpper,
            frontLower,
            extent1,
            extent2,
            area
        );

        double curVolume = area * (xMax - xMin);

        if (i == 0 || volume > curVolume) {

            Mat3x3 Minv = Mrot.transpose();

            frontLowerLeft  = frontLower;
            frontLowerRight = frontLower;
            frontUpperLeft  = frontUpper;
            frontUpperRight = frontUpper;

            frontLowerLeft.setX(xMin);
            frontLowerRight.setX(xMax);
            frontUpperLeft.setX(xMin);
            frontUpperRight.setX(xMax);

            frontLowerLeft  = Minv * frontLowerLeft;
            frontLowerRight = Minv * frontLowerRight;
            frontUpperLeft  = Minv * frontUpperLeft;
            frontUpperRight = Minv * frontUpperRight;
 
            backLowerLeft   = backLower;
            backLowerRight  = backLower;
            backUpperLeft   = backUpper;
            backUpperRight  = backUpper;

            backLowerLeft.setX(xMin);
            backLowerRight.setX(xMax);
            backUpperLeft.setX(xMin);
            backUpperRight.setX(xMax);

            backLowerLeft  = Minv * backLowerLeft;
            backLowerRight = Minv * backLowerRight;
            backUpperLeft  = Minv * backUpperLeft;
            backUpperRight = Minv * backUpperRight;

            Mat3x3 curAxes(n, Minv* axisY, Minv* axisZ);
            axes = curAxes;
            extents.setX(xMax - xMin);
            extents.setY(extent1);
            extents.setZ(extent2);
            volume = curVolume;

        }
    }

    obb.constructCuboid(
        frontLowerLeft,
        frontUpperLeft,
        frontUpperRight,
        frontLowerRight,
        backLowerLeft,
        backUpperLeft,
        backUpperRight,
        backLowerRight
    );

    center = 
        frontLowerLeft  +
        frontUpperLeft  +
        frontUpperRight +
        frontLowerRight +
        backLowerLeft   +
        backUpperLeft   +
        backUpperRight  +
        backLowerRight;

    center.scale(1.0/8.0);

}



#ifdef UNIT_TESTS

void makeOpenGLVerticesColorsForAxes(
    Vec3&         axis1,
    Vec3&         axis2,
    vector<Vec3>& vertices,
    vector<Vec3>& colors
) {
    Vec3 ori(0.0, 0.0, 0.0);

    Vec3 green(0.5, 1.0, 0.5);
    Vec3 blue (0.5, 0.5, 1.0);

    vertices.push_back(ori);
    vertices.push_back(axis1);
    vertices.push_back(ori);
    vertices.push_back(axis2);
    colors.push_back(green);
    colors.push_back(green);
    colors.push_back(blue);
    colors.push_back(blue);
}


void makeOpenGLVerticesColorsForAxes(
    Mat3x3&       axes,
    vector<Vec3>& vertices,
    vector<Vec3>& colors
) {
    Vec3 ori(0.0, 0.0, 0.0);

    Vec3 red  (1.0, 0.3, 0.3);
    Vec3 green(0.3, 1.0, 0.3);
    Vec3 blue (0.3, 0.3, 1.0);

    auto ax1(axes.col(1));
    auto ax2(axes.col(2));
    auto ax3(axes.col(3));
    ax1.scale(10.0);
    ax2.scale(10.0);
    ax3.scale(10.0);

    vertices.push_back(ori);
    vertices.push_back(ax1);
    vertices.push_back(ori);
    vertices.push_back(ax2);
    vertices.push_back(ori);
    vertices.push_back(ax3);

    colors.push_back(red);
    colors.push_back(red);
    colors.push_back(green);
    colors.push_back(green);
    colors.push_back(blue);
    colors.push_back(blue);
}

#endif

}// namespace Makena


