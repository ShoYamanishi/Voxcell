//
//  StructsSharedWithSwift.h
//  Voxell
//
//  Created by Shoichiro Yamanishi on 01.05.22.
//

#ifndef _STRUCTS_SHARED_WITH_SWIFT_H_
#define _STRUCTS_SHARED_WITH_SWIFT_H_

#include <simd/simd.h>

// MARK: **Conversion Rules**
//
// | Metal              | Swift              | Size | Alignment |
// | ------------------ | ------------------ | ----:| ---------:|
// | bool               |                    |    1 |         1 |
// | char               |                    |    1 |         1 |
// | short              |                    |    2 |         2 |
// | enum               | enum : Int32       |    4 |         4 |
// | int                | Int32              |    4 |         4 |
// | float              | Float              |    4 |         4 |
// | ------------------ | ------------------ | ----:| ---------:|
// | float2             | SIMD2<Float>       |    8 |         8 |
// | float3             | SIMD4<Float>       |   16 |        16 | <= NOT SIMD3<Float>
// | float4             | SIMD4<Float>       |   16 |        16 |
// | packed_float2      |                    |    8 |         4 |
// | packed_float3      |                    |   12 |         4 |
// | packed_float4      |                    |   16 |         4 |
// | ------------------ | ------------------ | ----:| ---------:|
// | float3x3           | float3x3           |   48 |        16 |
// | float4x4           | float4x4           |   64 |        16 |

// MARK: Indices

typedef enum {

    texture_index_base_color           = 0,
    texture_index_tangent_space_normal = 1,
    texture_index_roughness            = 2

} TextureIndex;

typedef enum {

    // 0-9 reserved for vertex buffers (stage_in)


    buffer_index_scene_uniform_vertex        = 10,
    buffer_index_per_instance_uniform_vertex = 11,
    buffer_index_scene_uniform_fragment      = 12,
    buffer_index_lights_fragment             = 13,
    buffer_index_material_fragment           = 14

} BufferIndex;

// MARK: Structures for Vertex Shaders

typedef struct _UniformsPerSceneVertex {

    matrix_float4x4 view_matrix;
    matrix_float4x4 projection_matrix;

} UniformsPerSceneVertex;

typedef struct _UniformsPerInstance {

    matrix_float4x4 model_matrix;
    matrix_float3x3 normal_matrix;

} UniformsPerInstanceVertex;

// MARK: Structures for Fragment Shaders

typedef struct _UniformsPerSceneFragment {

    float3 camera_position;
    uint   light_count;

} UniformsPerSceneFragment;

typedef enum {

    light_type_nused        = 0,
    light_type_sunlight     = 1,
    light_type_spotlight    = 2,
    light_type_pointlight   = 3,
    light_type_ambientlight = 4

} LightType;

typedef struct _Light {

    float3 position;
    float3 color;
    float3 specular_color;
    float  intensity;
    float3 attenuation;
    uint   light_type;
    float  cone_angle;
    float3 cone_direction;
    float  cone_attenuation;

} Light;

typedef struct _MaterialFragmentBytes {

    float3 base_color;
    float3 specular;
    float3 roughness;
    float  specular_exponent;
    float3 ambient_occlusion;
    float  metallic;
    float  opacity;

} MaterialFragmentBytes;

#endif /*_STRUCTS_SHARED_WITH_SWIFT_H_*/
