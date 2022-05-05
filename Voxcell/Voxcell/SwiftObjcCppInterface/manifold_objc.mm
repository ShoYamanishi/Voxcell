#import <Foundation/Foundation.h>
#import "manifold_objc.h"
#include "manifold.hpp"
#include "orienting_bounding_box.hpp"
#include <vector>
#include <map>
#include <algorithm>

using namespace std;
using namespace Makena;

@implementation ManifoldObjc
{
    vector< simd_float3  > _mVertices;
    vector< vector<long> > _mFacesIndexAroundVerticesCCW;
    vector< simd_float3  > _mFaceNormals;
    vector< simd_float3  > _mFaceTangents;
    vector< simd_float3  > _mFaceBitangents;
    vector< vector<long> > _mVerticesIndexAroundFacesCCW;
    vector< vector<simd_float2> >
                           _mTextureCoordinatesAroundFacesCCW;
}
 -(instancetype) init
 {
    self = [super init];
    if (self) {

    }
    return self;
 }

-(void) findConvexHull:(const simd_float3 *const) points numPoints:(const int) numPoints
 {
    [ self findConvexHullOrOrientedBoundingBox: points  numPoints: numPoints  forOrientedBoundingBox: false ];
 }

-(void) findOrientedBoundingBox:(const simd_float3 *const) points numPoints:(const int) numPoints
 {
    [ self findConvexHullOrOrientedBoundingBox: points  numPoints: numPoints  forOrientedBoundingBox: true ];
 }

 -(void) findConvexHullOrOrientedBoundingBox:(const simd_float3 *const) points
                                   numPoints:(const int) numPoints
                      forOrientedBoundingBox:(const bool) orientedBoundingBox
 {
    _mVertices.clear();
    _mFacesIndexAroundVerticesCCW.clear();
    _mFaceNormals.clear();
    _mVerticesIndexAroundFacesCCW.clear();

    Manifold convex_hull;
    enum predicate pred;
    vector<Vec3> vec;

    for ( int i = 0; i < numPoints; i++ ) {

        vec.emplace_back( (double)(points[i].x), (double)(points[i].y), (double)(points[i].z) );
        //NSLog( @"%d : (%f, %f, %f)", i, points[i].x, points[i].y, points[i].z );
    }

    convex_hull.findConvexHull(vec, pred);
    NSLog(@"pred: %d", pred);

    if (pred != NONE ) {
        return;
    }

    Mat3x3   axes;
    Vec3     center;
    Vec3     extent;
    double   volume;

    if ( orientedBoundingBox ) {

        Manifold obb;
        findOBB3D( convex_hull, obb, axes, center, extent, volume );
        [self generateVerticesAndFacesListForSwiftFor: obb ];

    } else {

        [self generateVerticesAndFacesListForSwiftFor: convex_hull ];
    }
}


-(void) generateVerticesAndFacesListForSwiftFor:(Manifold&) m
{
    auto vits = m.vertices();
    auto fits = m.faces();

    long index = 0;
    for (auto vit = vits.first; vit != vits.second; vit++ ) {
        (*vit)->userUtil = index++;
        const auto& p = (*vit)->pLCS();
        simd_float3 pf;
        pf.x = p.x();
        pf.y = p.y();
        pf.z = p.z();
        _mVertices.push_back(pf);
    }

    index = 0;
    for (auto fit = fits.first; fit != fits.second; fit++ ) {
        (*fit)->userUtil = index++;
        const auto& n = (*fit)->nLCS();
        simd_float3 nf;
        nf.x = n.x();
        nf.y = n.y();
        nf.z = n.z();
        _mFaceNormals.push_back( nf );

        // take the 1st halfedge as the tangent direction.
        const auto he0it = (*fit)->halfEdges().begin();
        const auto src0it = (*(*he0it))->src();
        const auto dst0it = (*(*he0it))->dst();
        auto t = (*dst0it)->pLCS() - (*src0it)->pLCS();
        t.normalize();
        simd_float3 tf; // tangent
        tf.x = (float)t.x();
        tf.y = (float)t.y();
        tf.z = (float)t.z();
        _mFaceTangents.push_back( tf );

        const auto bt = n.cross(t);
        simd_float3 btf; // bitangent
        btf.x = (float)bt.x();
        btf.y = (float)bt.y();
        btf.z = (float)bt.z();
        _mFaceBitangents.push_back( btf );

        // Pass 1: find the center point of the face
        Vec3 centerPoint( 0.0, 0.0, 0.0 );
        int  numOfPoints = 0;
        for ( auto heit = (*fit)->halfEdges().begin(); heit != (*fit)->halfEdges().end(); heit++ ) {

            const auto dstit = (*(*heit))->dst();
            centerPoint += ((*dstit)->pLCS());
            numOfPoints++;
        }
        centerPoint.scale( 1.0 / (double)numOfPoints );

        // Pass 2: find the max xy-extents from center to the points in the face-local coordinates.
        float maxOrthoDist = 0.0;
        for ( auto heit = (*fit)->halfEdges().begin(); heit != (*fit)->halfEdges().end(); heit++ ) {

            const auto dstit = (*(*heit))->dst();

            Vec3 centerToDstLCS = ((*dstit)->pLCS()) - centerPoint;
            Vec2 centerToDstFCS( t.dot(centerToDstLCS), bt.dot(centerToDstLCS) );
            maxOrthoDist = std::max(maxOrthoDist, abs((float)centerToDstFCS.x()));
            maxOrthoDist = std::max(maxOrthoDist, abs((float)centerToDstFCS.y()));
        }

        // Pass 3: store the vertex indices and the normalized texture UV coordinates.
        vector<long> verticesIndexAroundFaceCCW;
        vector<simd_float2> textureCoordinatesAroundFacesCCW;

        for ( auto heit = (*fit)->halfEdges().begin(); heit != (*fit)->halfEdges().end(); heit++ ) {

            const auto dstit = (*(*heit))->dst();

            verticesIndexAroundFaceCCW.push_back( (*dstit)->userUtil );

            Vec3 centerToDstLCS = ((*dstit)->pLCS()) - centerPoint;
            Vec2 centerToDstFCS( t.dot(centerToDstLCS), bt.dot(centerToDstLCS) );
            simd_float2 dstFCS;
            dstFCS.x = 0.5 + 0.5 * centerToDstFCS.x() / maxOrthoDist;
            dstFCS.y = 0.5 + 0.5 * centerToDstFCS.y() / maxOrthoDist;
            dstFCS.x = std::min( 1.0f, std::max(0.0f, dstFCS.x) );
            dstFCS.y = std::min( 1.0f, std::max(0.0f, dstFCS.y) );

            textureCoordinatesAroundFacesCCW.push_back( dstFCS );
        }

        _mVerticesIndexAroundFacesCCW.emplace_back( verticesIndexAroundFaceCCW );

        _mTextureCoordinatesAroundFacesCCW.emplace_back( textureCoordinatesAroundFacesCCW );
    }

    for (auto vit = vits.first; vit != vits.second; vit++ ) {

        vector<long> facesIndexAroundVertexCCW;
        for ( auto heit = (*vit)->halfEdges().begin(); heit != (*vit)->halfEdges().end(); heit++ ) {
            const auto srcit = (*(*heit))->src();
            if ( (*vit)->userUtil == (*srcit)->userUtil ) {
                const auto fit = (*(*heit))->face();
                facesIndexAroundVertexCCW.push_back( (*fit)->userUtil );
            }
        }
        _mFacesIndexAroundVerticesCCW.emplace_back(facesIndexAroundVertexCCW);
    }
/*
    NSLog(@"Vertex points");
    for ( int i = 0; i < _mVertices.size(); i++ ) {
        auto v = _mVertices[i];
        NSLog(@"v[%d] (%f, %f, %f)", i, v.x, v.y, v.z);
    }

    NSLog(@"Faces around vertices");
    for ( int i = 0; i < _mFacesIndexAroundVerticesCCW.size(); i++ ) {
        auto& vec = _mFacesIndexAroundVerticesCCW[i];
        std::cerr << i << ": [";
        for ( auto j : vec ) {
            std::cerr << j << ", ";
        }
        std::cerr << "]\n";
    }

    NSLog(@"Face normals");
    for ( int i = 0; i < _mFaceNormals.size(); i++ ) {
        auto n = _mFaceNormals[i];
        NSLog(@"n[%d] (%f, %f, %f)", i, n.x, n.y, n.z);
    }

    NSLog(@"Vertices around faces");
    for ( int i = 0; i < _mVerticesIndexAroundFacesCCW.size(); i++ ) {
        auto& vec = _mVerticesIndexAroundFacesCCW[i];
        std::cerr << i << ": [";
        for ( auto j : vec ) {
            std::cerr << j << ", ";
        }
        std::cerr << "]\n";
    }

    NSLog(@"Texture Coordinates around faces");
    for ( int i = 0; i < _mTextureCoordinatesAroundFacesCCW.size(); i++ ) {

        auto& vec = _mTextureCoordinatesAroundFacesCCW[i];
        std::cerr << i << ": [";
        for ( auto j : vec ) {
            std::cerr << "(" << j.x << ", " << j.y << "), ";
        }
        std::cerr << "]\n";
    }
*/
 }




-(const simd_float3*) vertices {
    return &_mVertices[0];
}

-(long) numVertices {
    return _mVertices.size();
}

-(const simd_float3*) faceNormals {
    return &_mFaceNormals[0];
}

-(const simd_float3*) faceTangents {
    return &_mFaceTangents[0];
}

-(const simd_float3*) faceBitangents {
    return &_mFaceBitangents[0];
}


-(long) numFaceNormals {
    return _mFaceNormals.size();
}

-(long) numFaceTangents {
    return _mFaceTangents.size();
}

-(long) numFaceBitangents {
    return _mFaceBitangents.size();
}


-(const long*) facesIndexAroundVerticeFor: (const long) index {
    return &_mFacesIndexAroundVerticesCCW[index][0];
}

-(long) numFacesIndexAroundVerticeFor: (const long) index {
    return _mFacesIndexAroundVerticesCCW[index].size();
}

-(const long*) verticesIndexAroundFaceFor: (const long) index {
    return &_mVerticesIndexAroundFacesCCW[index][0];
}

-(const simd_float2*) textureCoordinatesAroundFaceFor: (const long) index {
    return &_mTextureCoordinatesAroundFacesCCW[index][0];
}

-(long) numVerticesIndexAroundFaceFor: (const long) index {
    return _mVerticesIndexAroundFacesCCW[index].size();
}

@end



