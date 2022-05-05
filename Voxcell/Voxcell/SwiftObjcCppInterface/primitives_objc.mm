#import <Foundation/Foundation.h>
#import "primitives_objc.h"
#include "primitives.hpp"

using namespace std;
using namespace Makena;

@implementation PrimitivesObjc
{
    Vec3   _mEigenValues;
    Mat3x3 _mEigenVectors;

}
-(instancetype) init
{
    self = [super init];
    if (self) {

    }
    return self;
}

-(void) EigenVectorsIfSymmetric:(const simd_float3x3) mObj;
{
    Mat3x3 mCpp(
        (double)mObj.columns[0][0], (double)mObj.columns[0][1], (double)mObj.columns[0][2],
        (double)mObj.columns[1][0], (double)mObj.columns[1][1], (double)mObj.columns[1][2],
        (double)mObj.columns[2][0], (double)mObj.columns[2][1], (double)mObj.columns[2][2]
    );

    _mEigenVectors = mCpp.EigenVectorsIfSymmetric( _mEigenValues );
}

-(float) EigenVectorMatrixRow:(const int) row Col:(const int) col
{
    return _mEigenVectors.cell(row+1, col+1);
}

-(float) EigenValue:(const int) index
{
    return _mEigenValues[index+1];
}
@end
