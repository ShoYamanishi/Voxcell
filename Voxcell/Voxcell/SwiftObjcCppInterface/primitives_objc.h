#ifndef primitives_objc_h
#define primitives_objc_h

#import <Foundation/Foundation.h>
#import <simd/simd.h>

@interface PrimitivesObjc: NSObject
-(instancetype) init;
-(void) EigenVectorsIfSymmetric:(const simd_float3x3) m;
-(float) EigenVectorMatrixRow:(const int) row Col:(const int) col;
-(float) EigenValue:(const int) index;
@end




#endif /* primitives_objc_h */
