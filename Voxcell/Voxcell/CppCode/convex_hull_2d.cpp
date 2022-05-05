#include "convex_hull_2d.hpp"


#ifdef UNIT_TESTS
#include "gtest/gtest_prod.h"
#endif


/**
 * @file convex_hull_2d.cpp
 *
 * @brief finds the convex hull of points on a 2d xy plane.
 *
 * @reference
 *
 */
namespace Makena {

using namespace std;

class CHinternal {
  public:
    CHinternal(const Vec2& p, const long index):mP(p),mIndex(index){;}
    ~CHinternal(){;}
    Vec2 mP;
    long mIndex;  
};


class CH2sort {
  public:
    bool operator() (CHinternal& i,CHinternal& j)
    { 
        if ( (i.mP.x() < j.mP.x()) ||
             ((i.mP.x() == j.mP.x())&&(i.mP.y() < j.mP.y())) ) {
            return true;
        }    
        else {
            return false;
        }
    }
};

static void updateStack(std::vector<CHinternal>& us, CHinternal& p, bool Upper)
{
    if (us.size() <= 1) {
        us.push_back(p);
    }
    else {
        bool allConvex = false;
        while (!allConvex && us.size()>1) {
            Vec2& p1 = us[us.size()-1].mP;
            Vec2& p2 = us[us.size()-2].mP;
            Vec2 p2p1 = p1 - p2;
            Vec2 p1p  = p.mP - p1;
            auto cr   = p2p1.dot(p1p.perp());
            if (Upper && cr >= EPSILON_LINEAR) {
                break;
            }
            else if (!Upper && cr <= -1.0*EPSILON_LINEAR) {
                break;
            }
            us.pop_back();
        }
        us.push_back(p);
    }
}

static std::vector<CHinternal> mergeTwoStacks(
    std::vector<CHinternal>& upperStack,
    std::vector<CHinternal>& lowerStack
) {
    std::vector<CHinternal> merged(lowerStack.begin(), lowerStack.end());
    for (long i = upperStack.size()-2; i >= 1; i--) {
        merged.push_back(upperStack[i]);
    }
    return merged;
}


std::vector<long> findConvexHull2D(std::vector<Vec2>& points)
{

    // Sort the points along x and then y
    std::vector<CHinternal> pointsInternal;
    for (long i = 0; i < points.size(); i++) {
        pointsInternal.emplace_back(points[i],i);
    }    

    CH2sort compObj;
    std::sort(pointsInternal.begin(), pointsInternal.end(), compObj);

    std::vector<CHinternal> upperStack;
    std::vector<CHinternal> lowerStack;

    for (auto& p : pointsInternal) {

        updateStack(upperStack, p, true);
        
        updateStack(lowerStack, p, false);

    }

    auto merged = mergeTwoStacks(upperStack, lowerStack);

    vector<long> results;
    for (long i = 0; i < merged.size(); i++) {
        results.push_back(merged[i].mIndex);
    }

    return results;
}


std::vector<long> findConvexHull2D(std::vector<Vec3>& points)
{

    // Sort the points along x and then y
    std::vector<CHinternal> pointsInternal;
    for (long i = 0; i < points.size(); i++) {
        pointsInternal.emplace_back(Vec2(points[i].y(),points[i].z()),i);
    }    

    CH2sort compObj;
    std::sort(pointsInternal.begin(), pointsInternal.end(), compObj);

    std::vector<CHinternal> upperStack;
    std::vector<CHinternal> lowerStack;

    for (auto& p : pointsInternal) {

        updateStack(upperStack, p, true);
        
        updateStack(lowerStack, p, false);

    }

    auto merged = mergeTwoStacks(upperStack, lowerStack);

    vector<long> results;
    for (long i = 0; i < merged.size(); i++) {
        results.push_back(merged[i].mIndex);
    }

    return results;
}


#ifdef UNIT_TESTS
void makeOpenGLVerticesColorsForLines(
    vector<Vec3>& points,
    Vec3&         color,
    vector<Vec3>& vertices,
    vector<Vec3>& colors
) {
    for (size_t i = 0; i < points.size(); i++) {
        vertices.push_back(points[i]);
        vertices.push_back(points[(i+1)%points.size()]);
        colors.push_back(color);
        colors.push_back(color);
    }
}

void makeOpenGLVerticesColorsForPoints(
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

#endif

}// namespace Makena


