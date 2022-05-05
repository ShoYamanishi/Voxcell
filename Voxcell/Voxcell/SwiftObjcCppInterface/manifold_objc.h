#import <Foundation/Foundation.h>

#import <simd/simd.h>

//        let m : ManifoldObjc = ManifoldObjc()
//        var a : [SIMD3<Float>] = [SIMD3<Float>(0.1, 0.2, 0.3), SIMD3<Float>(0.4, 0.5, 0.6)]
//        let p = UnsafeMutablePointer< SIMD3<Float> >( &a )
//        m.findConvexHull( p, numPoints: 2 )

@interface ManifoldObjc: NSObject
-(instancetype) init;
-(void) findConvexHull:          (const simd_float3 * const) points numPoints: (const int) numPoints;
-(void) findOrientedBoundingBox: (const simd_float3 * const) points numPoints: (const int) numPoints;
 
-(const simd_float3*) vertices;
-(long) numVertices;

-(const simd_float3*) faceNormals;
-(long) numFaceNormals;

-(const simd_float3*) faceTangents;
-(long) numFaceTangents;

-(const simd_float3*) faceBitangents;
-(long) numFaceBitangents;

-(const long*) facesIndexAroundVerticeFor: (const long) index;
-(long) numFacesIndexAroundVerticeFor: (const long) index;

-(const long*) verticesIndexAroundFaceFor: (const long) index;
-(const simd_float2*) textureCoordinatesAroundFaceFor: (const long) index;
-(long) numVerticesIndexAroundFaceFor: (const long) index;

@end




