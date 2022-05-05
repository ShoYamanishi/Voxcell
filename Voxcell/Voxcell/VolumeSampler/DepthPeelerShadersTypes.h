#ifndef _DEPTH_PEELER_SHADERS_TYPES_H_
#define _DEPTH_PEELER_SHADERS_TYPES_H_

// This is the Metal-shader counterpart of DepthPeelerSwiftTypes.swift.

// struct Ray is required by MPSRayIntersector
// if rayDataType = MPSRayDataType.originMaskDirectionMaxDistance
// It is not defined anywhere by Metal.
struct Ray {

  packed_float3 origin;      // 12 bytes
  uint          mask;        //  4 bytes
  packed_float3 direction;   // 12 bytes
  float         maxDistance; //  4 bytes
  float3        color;       // 16 bytes

}; // 48 bytes

// struct Intersection is required by MPSRayIntersector
// if intersectionDataType =  MPSIntersectionDataType.distancePrimitiveIndexCoordinates
// It is not defined anywhere by Metal.
struct Intersection {

    float  distance;
    int    primitiveIndex;
    float2 coordinates;
};

constant static const uint32_t CD_INVALID    = 0;
constant static const uint32_t CD_X_POSITIVE = 1;
constant static const uint32_t CD_X_NEGATIVE = 2;
constant static const uint32_t CD_Y_POSITIVE = 3;
constant static const uint32_t CD_Y_NEGATIVE = 4;
constant static const uint32_t CD_Z_POSITIVE = 5;
constant static const uint32_t CD_Z_NEGATIVE = 6;

constant static const uint32_t DPRT_NONE            = 0;
constant static const uint32_t DPRT_ENTERING_VOLUME = 1;
constant static const uint32_t DPRT_LEAVING_VOLUME  = 2;

typedef struct {

    vector_float3 extentMin;      // The lowerst corner of the grid
    vector_float3 extentMax;      // The highest corner of the grid
    vector_uint3  gridDimension;  // Number of the cells along each axis
    uint32_t      castDirection;
    float         epsilon;        // perturbation added when the ray position is updated
                                  // for the next peeling to avoid detecting the same intersection
                                  // due to numerical errors.

    vector_float3 defaultColor;   // color values in RGB. Used if a texture is not available.

} UniformRayIntersector;


struct DepthPeelingResult {

    uint32_t               resultType;
    int32_t                triangleIndex;
    float                  depthCoord;
    float3                 color;

};

#endif /*_DEPTH_PEELER_SHADERS_TYPES_H_*/
