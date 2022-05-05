#ifndef _MAKENA_ORIENTING_BOUNDING_BOX_HPP_
#define _MAKENA_ORIENTING_BOUNDING_BOX_HPP_

#include <memory>
#include <array>
#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <exception>
#include <stdexcept>
#include <cmath>

#include "primitives.hpp"
#include "manifold.hpp"


#ifdef UNIT_TESTS
#include "gtest/gtest_prod.h"
#endif


/**
 * @file orienting_bounding_box.cpp
 *
 * @brief Finds the optimum orienting bounding box of convex polytope.
 *
 * @reference
 *
 */
namespace Makena {

using namespace std;


/** @brief finds the optimum oriented bounding box for the given convex hull
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
 *  @param area       (out): the area of the box
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
);


/** @brief finds the oriented bounding box for the given convex hull
 *
 *  @param convexHull (in):  convex hull
 *
 *  @param obb        (out): optimum oriented bounding box
 *
 *  @param axes       (out): the axes of the box.
 *
 *  @param center     (out): center of the box.
 *
 *  @param extent    (out): the lengths of the box
 *
 *  @param volume     (out): volume of the box
 */
void findOBB3D(
    Manifold& convexHull,
    Manifold& obb,
    Mat3x3&   axes,
    Vec3&     center,
    Vec3&     extent,
    double&   volume
);


#ifdef UNIT_TESTS

void makeOpenGLVerticesColorsForAxes(
    Vec3&         axis1,
    Vec3&         axis2,
    vector<Vec3>& vertices,
    vector<Vec3>& colors
);

void makeOpenGLVerticesColorsForAxes(
    Mat3x3&       axes,
    vector<Vec3>& vertices,
    vector<Vec3>& colors
);

#endif

}// namespace Makena


#endif/*_MAKENA_ORIENTING_BOUNDING_BOX_HPP_*/
