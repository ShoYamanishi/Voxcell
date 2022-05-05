#ifndef _MAKENA_CONVEX_HULL_2D_HPP_
#define _MAKENA_CONVEX_HULL_2D_HPP_

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

#ifdef UNIT_TESTS
#include "gtest/gtest_prod.h"
#endif


/**
 * @file convex_hull_2d.hpp
 *
 * @brief Finds the convex hull of two convex polygons.
 *
 * @reference
 *
 */
namespace Makena {

using namespace std;


/** @brief finds the convex hull of the given points on yz-plane.
 *
 *  @param points   (in):  the points
 *
 *  @return indices into points along the convex hull in counter-clockwise ordering.
 */
vector<long> findConvexHull2D(vector<Vec3>& points);


/** @brief finds the convex hull of the given points on 2d space
 *
 *  @param points   (in):  the points
 *
 *  @return indices into points along the convex hull in counter-clockwise ordering.
 */
vector<long> findConvexHull2D(vector<Vec2>& points);


#ifdef UNIT_TESTS


/** @brief generates arrays for vertices, and colors
 *         for the lines of the given circular half edges.
 *         Those are for OpenGL's GL_LINES
 *
 *  @param points (in): circular list of points around the convex hull
 *
 *  @param color  (in):  color of this circumference
 *
 *  @param vertices (out): vertices of the line segments
 *
 *  @param colors   (out): colors of the line segments
 */
void makeOpenGLVerticesColorsForLines(
    vector<Vec3>& points,
    Vec3&         color,
    vector<Vec3>& vertices,
    vector<Vec3>& colors
);


/** @brief generates arrays for vertices, and colors
 *         for the given points.
 *         Those are for OpenGL's GL_POINTS
 *
 *  @param points (in): points to be presented
 *
 *  @param color  (in):  color of the points
 *
 *  @param vertices (out): vertices of the points
 *
 *  @param colors   (out): colors of the points
 */
void makeOpenGLVerticesColorsForPoints(
    vector<Vec3>& points,
    Vec3&         color,
    vector<Vec3>& vertices,
    vector<Vec3>& colors
);

#endif

}// namespace Makena

#endif/*_MAKENA_CONVEX_HULL_2D_HPP_*/
