#include <metal_stdlib>
#include "DepthPeelerShadersTypes.h"

using namespace metal;

constant bool hasTexture [[ function_constant(0) ]];

bool getRayIdx( uint2 tid, constant UniformRayIntersector& uniforms, thread uint& rayIdx ) {

    const uint tid_u = tid.x;
    const uint tid_v = tid.y;

    switch ( uniforms.castDirection ) {

      case CD_X_POSITIVE:
      case CD_X_NEGATIVE:
        if ( tid_u < uniforms.gridDimension.y && tid_v < uniforms.gridDimension.z ) {
            rayIdx = tid_v * uniforms.gridDimension.y + tid_u;
            return true;
        }
        break;

      case CD_Y_POSITIVE:
      case CD_Y_NEGATIVE:
        if ( tid_u < uniforms.gridDimension.z && tid_v < uniforms.gridDimension.x ) {
            rayIdx = tid_v * uniforms.gridDimension.z + tid_u;
            return true;
        }
        break;

      case CD_Z_POSITIVE:
      case CD_Z_NEGATIVE:
        if ( tid_u < uniforms.gridDimension.x && tid_v < uniforms.gridDimension.y ) {
            rayIdx = tid_v * uniforms.gridDimension.x + tid_u;
            return true;
        }
        break;

      default:
        break;
    }
    return false;
}


kernel void initializeRays(

             uint2                  tid      [[ thread_position_in_grid ]],
    constant UniformRayIntersector& uniforms [[ buffer(0) ]],
    device   Ray*                   rays     [[ buffer(1) ]]
) {
    const uint tid_u = tid.x;
    const uint tid_v = tid.y;

    uint rayIdx;
    if ( !getRayIdx( tid, uniforms, rayIdx ) ) {
        return;
    }

    device Ray& ray = rays[ rayIdx ];
    ray.mask        = 1;
    ray.maxDistance = INFINITY;

    const float pitch_x  = ( uniforms.extentMax.x - uniforms.extentMin.x ) / (float)uniforms.gridDimension.x;
    const float pitch_y  = ( uniforms.extentMax.y - uniforms.extentMin.y ) / (float)uniforms.gridDimension.y;
    const float pitch_z  = ( uniforms.extentMax.z - uniforms.extentMin.z ) / (float)uniforms.gridDimension.z;

    switch ( uniforms.castDirection ) {

      case CD_X_POSITIVE:

        ray.origin.x    = uniforms.extentMin.x - uniforms.epsilon;
        ray.origin.y    = uniforms.extentMin.y + pitch_y * ( (float)tid_u + 0.5 );
        ray.origin.z    = uniforms.extentMin.z + pitch_z * ( (float)tid_v + 0.5 );
        ray.direction   = packed_float3( 1.0, 0.0, 0.0 );
        break;

      case CD_X_NEGATIVE:

        ray.origin.x    = uniforms.extentMax.x + uniforms.epsilon;
        ray.origin.y    = uniforms.extentMin.y + pitch_y * ( (float)tid_u + 0.5 );
        ray.origin.z    = uniforms.extentMin.z + pitch_z * ( (float)tid_v + 0.5 );
        ray.direction   = packed_float3( -1.0, 0.0, 0.0 );
        break;

      case CD_Y_POSITIVE:

        ray.origin.y    = uniforms.extentMin.y - uniforms.epsilon;
        ray.origin.z    = uniforms.extentMin.z + pitch_z * ( (float)tid_u + 0.5 );
        ray.origin.x    = uniforms.extentMin.x + pitch_x * ( (float)tid_v + 0.5 );
        ray.direction   = packed_float3( 0.0, 1.0, 0.0 );
        break;
       
      case CD_Y_NEGATIVE:

        ray.origin.y    = uniforms.extentMax.y + uniforms.epsilon;
        ray.origin.z    = uniforms.extentMin.z + pitch_z * ( (float)tid_u + 0.5 );
        ray.origin.x    = uniforms.extentMin.x + pitch_x * ( (float)tid_v + 0.5 );
        ray.direction   = packed_float3( 0.0, -1.0, 0.0 );
        break;

      case CD_Z_POSITIVE:

        ray.origin.z    = uniforms.extentMin.z - uniforms.epsilon;
        ray.origin.x    = uniforms.extentMin.x + pitch_x * ( (float)tid_u + 0.5 );
        ray.origin.y    = uniforms.extentMin.y + pitch_y * ( (float)tid_v + 0.5 );
        ray.direction   = packed_float3( 0.0, 0.0, 1.0 );
        break;

      case CD_Z_NEGATIVE:

        ray.origin.z    = uniforms.extentMax.z + uniforms.epsilon;
        ray.origin.x    = uniforms.extentMin.x + pitch_x * ( (float)tid_u + 0.5 );
        ray.origin.y    = uniforms.extentMin.y + pitch_y * ( (float)tid_v + 0.5 );
        ray.direction   = packed_float3( 0.0, 0.0, -1.0 );
        break;

      default:
        break;
    }
}

template<typename T>
inline T interpolateVertexAttribute( device T *attributes, Intersection intersection ) {

    const float e0 = intersection.coordinates.x;
    const float e1 = intersection.coordinates.y;
    const float e2 = 1.0 - ( e0  + e1 );
    
    unsigned int triangleIndex = intersection.primitiveIndex;
    
    T T0 = attributes[ triangleIndex * 3 + 0 ];
    T T1 = attributes[ triangleIndex * 3 + 1 ];
    T T2 = attributes[ triangleIndex * 3 + 2 ];
    
    return e0 * T0 + e1 * T1 + e2 * T2;
}

kernel void checkIntersectionAndUpdateRays(

             uint2                  tid                [[ thread_position_in_grid ]],
    constant UniformRayIntersector& uniforms           [[ buffer(0) ]],
    device   Ray*                   rays               [[ buffer(1) ]],
    device   Intersection*          intersections      [[ buffer(2) ]],
    device   DepthPeelingResult*    results            [[ buffer(3) ]],
    device   vector_float3*         normals            [[ buffer(4) ]],
    device   vector_float2*         textureCoordinates [[ buffer(5), function_constant( hasTexture ) ]],

    texture2d< float, access::sample > colorTexture    [[ texture(0), function_constant( hasTexture ) ]]

) {
    constexpr sampler textureSampler( min_filter::nearest, mag_filter::nearest, mip_filter::none );

    uint rayIdx;
    if ( !getRayIdx( tid, uniforms, rayIdx ) ) {
        return;
    }
    
    device Ray&                ray          = rays[ rayIdx ];
    device Intersection&       intersection = intersections[ rayIdx ];
    device DepthPeelingResult& result       = results[ rayIdx ];

    if ( intersection.distance >= 0.0f ) {

        result.triangleIndex = intersection.primitiveIndex;

        vector_float3 point  = ray.origin + ray.direction * intersection.distance;
            
        vector_float3 normal = interpolateVertexAttribute( normals, intersection );

        ray.origin = ray.origin + ray.direction * ( intersection.distance + uniforms.epsilon );

        switch ( uniforms.castDirection ) {

          case CD_X_POSITIVE:

            if ( normal.x < 0.0 ) {
                result.resultType = DPRT_ENTERING_VOLUME;
            }
            else if ( normal.x > 0.0 ) {
                result.resultType = DPRT_LEAVING_VOLUME;
            }
            result.depthCoord = point.x;
            break;

          case CD_X_NEGATIVE:

            if ( normal.x > 0.0 ) {
                result.resultType = DPRT_ENTERING_VOLUME;
            }
            else if ( normal.x < 0.0 ) {
                result.resultType = DPRT_LEAVING_VOLUME;
            }
            result.depthCoord = point.x;
            break;

          case CD_Y_POSITIVE:

            if ( normal.y < 0.0 ) {
                result.resultType = DPRT_ENTERING_VOLUME;
            }
            else if ( normal.y > 0.0 ) {
                result.resultType = DPRT_LEAVING_VOLUME;
            }
            result.depthCoord = point.y;
            break;

          case CD_Y_NEGATIVE:

            if ( normal.y > 0.0 ) {
                result.resultType = DPRT_ENTERING_VOLUME;
            }
            else if ( normal.y < 0.0 ) {
                result.resultType = DPRT_LEAVING_VOLUME;
            }
            result.depthCoord = point.y;
            break;

          case CD_Z_POSITIVE:

            if ( normal.z < 0.0 ) {
                result.resultType = DPRT_ENTERING_VOLUME;
            }
            else if ( normal.z > 0.0 ) {
                result.resultType = DPRT_LEAVING_VOLUME;
            }
            result.depthCoord = point.z;
            break;

          case CD_Z_NEGATIVE:

            if ( normal.z > 0.0 ) {
                result.resultType = DPRT_ENTERING_VOLUME;
            }
            else if ( normal.z < 0.0 ) {
                result.resultType = DPRT_LEAVING_VOLUME;
            }
            result.depthCoord = point.z;
            break;

          default:
            break;
        }

        if ( hasTexture ) {

            vector_float2 uv = interpolateVertexAttribute( textureCoordinates, intersection );
            result.color = colorTexture.sample( textureSampler, uv ).xyz;
        }
        else {
            result.color = uniforms.defaultColor;
        }
    }
    else {
        result.resultType = DPRT_NONE;
    }
}

