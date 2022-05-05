//
//  ShaderDefault.metal
//  Voxell
//
//  Created by Shoichiro Yamanishi on 01.05.22.
//

#include <metal_stdlib>
#include "structs_and_indices.h"

using namespace metal;

// Indices defined in enum FunctionConstantIndices in StructsSharedWithShaders.swift
constant bool vertex_buffer_has_normals             [[ function_constant(0) ]];
constant bool vertex_buffer_has_tangents            [[ function_constant(1) ]];
constant bool vertex_buffer_has_bitangents          [[ function_constant(2) ]];
constant bool vertex_buffer_has_texture_coordinates [[ function_constant(3) ]];
constant bool vertex_buffer_has_colors              [[ function_constant(4) ]];

constant bool has_texture_base_color                [[ function_constant(5) ]];
constant bool has_texture_tangent_space_normal      [[ function_constant(6) ]];
constant bool has_texture_roughness                 [[ function_constant(7) ]];

struct VertexInPositionColor {

    float4 position  [[ attribute( 0 ) ]];

    float3 color     [[ attribute( 1 ) ]];
};

struct VertexInPositionNormalColor {

    float4 position  [[ attribute( 0 ) ]];

    float3 normal    [[ attribute( 1 ) ]];

    float3 color     [[ attribute( 2 ) ]];
};

struct VertexInPositionNormalUV {

    float4 position  [[ attribute( 0 ) ]];

    float3 normal    [[ attribute( 1 ) ]];

    float2 uv        [[ attribute( 2 ) ]];
};

struct VertexInPositionNormalUVTangentBitangent {

    float4 position  [[ attribute( 0 ) ]];

    float3 normal    [[ attribute( 1 ) ]];

    float2 uv        [[ attribute( 2 ) ]];

    float3 tangent   [[ attribute( 3 ) ]];

    float3 bitangent [[ attribute( 4 ) ]];
};

struct VertexOut {

    float4 position [[ position ]];
    float3 world_position;
    float3 world_normal;
    float2 uv;
    float3 world_tangent;
    float3 world_bitangent;
    float3 color;
};

vertex VertexOut vertex_position_normal_uv_tangent_bitangent(

    const    ushort                     instance_id  [[ instance_id ]],
    const    VertexInPositionNormalUVTangentBitangent
                                        vertex_in    [[ stage_in ]],
    constant UniformsPerSceneVertex&    per_scene    [[ buffer( buffer_index_scene_uniform_vertex ) ]],
    constant UniformsPerInstanceVertex* per_instance [[ buffer( buffer_index_per_instance_uniform_vertex ) ]]
) {
    const float4 position =   per_scene.projection_matrix
                            * per_scene.view_matrix
                            * per_instance[instance_id].model_matrix
                            * vertex_in.position;

    const float4 world_position =   per_instance[instance_id].model_matrix
                                  * vertex_in.position;

    const float3 world_normal = per_instance[instance_id].normal_matrix * vertex_in.normal;

    const float3 world_tangent = per_instance[instance_id].normal_matrix * vertex_in.tangent;

    const float3 world_bitangent = per_instance[instance_id].normal_matrix * vertex_in.bitangent;

    const float2 uv = vertex_in.uv;
   
    const float3 color( 0.0, 0.0, 0.0 );

    VertexOut out {

        .position        = position,
        .world_position  = world_position.xyz,
        .world_normal    = world_normal,
        .uv              = uv,
        .world_tangent   = world_tangent,
        .world_bitangent = world_bitangent,
        .color           = color
    };

    return out;
}


vertex VertexOut vertex_position_normal_uv(

    const    ushort                     instance_id  [[ instance_id ]],
    const    VertexInPositionNormalUV   vertex_in    [[ stage_in ]],
    constant UniformsPerSceneVertex&    per_scene    [[ buffer( buffer_index_scene_uniform_vertex ) ]],
    constant UniformsPerInstanceVertex* per_instance [[ buffer( buffer_index_per_instance_uniform_vertex ) ]]
) {
    const float4 position =   per_scene.projection_matrix
                            * per_scene.view_matrix
                            * per_instance[instance_id].model_matrix
                            * vertex_in.position;

    const float4 world_position =   per_instance[instance_id].model_matrix
                                  * vertex_in.position;

    const float3 world_normal = per_instance[instance_id].normal_matrix * vertex_in.normal;

    const float3 world_tangent( 0.0, 0.0, 0.0 );

    const float3 world_bitangent( 0.0, 0.0, 0.0 );

    const float2 uv = vertex_in.uv;
   
    const float3 color( 0.0, 0.0, 0.0 );

    VertexOut out {

        .position        = position,
        .world_position  = world_position.xyz,
        .world_normal    = world_normal,
        .uv              = uv,
        .world_tangent   = world_tangent,
        .world_bitangent = world_bitangent,
        .color           = color
    };

    return out;
}

vertex VertexOut vertex_position_normal_color(

    const    ushort                      instance_id  [[ instance_id ]],
    const    VertexInPositionNormalColor vertex_in    [[ stage_in ]],
    constant UniformsPerSceneVertex&     per_scene    [[ buffer( buffer_index_scene_uniform_vertex ) ]],
    constant UniformsPerInstanceVertex*  per_instance [[ buffer( buffer_index_per_instance_uniform_vertex ) ]]
) {
    const float4 position =   per_scene.projection_matrix
                            * per_scene.view_matrix
                            * per_instance[instance_id].model_matrix
                            * vertex_in.position;

    const float4 world_position =   per_instance[instance_id].model_matrix
                                  * vertex_in.position;

    const float3 world_normal = per_instance[instance_id].normal_matrix * vertex_in.normal;

    const float3 world_tangent( 0.0, 0.0, 0.0 );

    const float3 world_bitangent( 0.0, 0.0, 0.0 );

    const float2 uv( 0.0, 0.0 );
   
    const float3 color = vertex_in.color;

    VertexOut out {

        .position        = position,
        .world_position  = world_position.xyz,
        .world_normal    = world_normal,
        .uv              = uv,
        .world_tangent   = world_tangent,
        .world_bitangent = world_bitangent,
        .color           = color
    };

    return out;
}

vertex VertexOut vertex_position_color(

    const    ushort                      instance_id  [[ instance_id ]],
    const    VertexInPositionNormalColor vertex_in    [[ stage_in ]],
    constant UniformsPerSceneVertex&     per_scene    [[ buffer( buffer_index_scene_uniform_vertex ) ]],
    constant UniformsPerInstanceVertex*  per_instance [[ buffer( buffer_index_per_instance_uniform_vertex ) ]]
) {
    const float4 position =   per_scene.projection_matrix
                            * per_scene.view_matrix
                            * per_instance[instance_id].model_matrix
                            * vertex_in.position;

    const float4 world_position =   per_instance[instance_id].model_matrix
                                  * vertex_in.position;

    const float3 world_normal( 0.0, 0.0, 0.0 );

    const float3 world_tangent( 0.0, 0.0, 0.0 );

    const float3 world_bitangent( 0.0, 0.0, 0.0 );

    const float2 uv( 0.0, 0.0 );
   
    const float3 color = vertex_in.color;

    VertexOut out {

        .position        = position,
        .world_position  = world_position.xyz,
        .world_normal    = world_normal,
        .uv              = uv,
        .world_tangent   = world_tangent,
        .world_bitangent = world_bitangent,
        .color           = color
    };

    return out;
}

fragment float4 fragment_default(

    VertexOut                          in               [[ stage_in ]],

    constant UniformsPerSceneFragment& per_scene        [[ buffer( buffer_index_scene_uniform_fragment ) ]],
    constant Light*                    lights           [[ buffer( buffer_index_lights_fragment ) ]],
    constant MaterialFragmentBytes&    material         [[ buffer( buffer_index_material_fragment ) ]],

    sampler           texture_sampler     [[ sampler(0) ]],

    texture2d<float>  texture_base_color           [[ texture          ( texture_index_base_color ),
                                                      function_constant( has_texture_base_color ) ]],

    texture2d<float>  texture_tangent_space_normal [[ texture          ( texture_index_tangent_space_normal ),
                                                      function_constant( has_texture_tangent_space_normal ) ]],

    texture2d<float>  texture_roughness            [[ texture( texture_index_roughness ),
                                                      function_constant( has_texture_roughness ) ]]
) {
    float3 base_color;

    if ( has_texture_base_color ) {

        base_color = texture_base_color.sample( texture_sampler, in.uv ).rgb;

    } else if ( vertex_buffer_has_colors ) {

        base_color = in.color;
    } else {

        base_color = material.base_color;
    }

    if ( !vertex_buffer_has_normals )  {
        return float4( base_color, 1.0 - material.opacity );
    }

    float3 sampled_normal;

    if ( has_texture_tangent_space_normal ) {

        sampled_normal = texture_tangent_space_normal.sample( texture_sampler, in.uv ).rgb;
        sampled_normal = sampled_normal * 2.0 - 1.0;
    } else {
        sampled_normal = float3( 0.0, 0.0, 1.0 );
    }
    
    sampled_normal = normalize( sampled_normal );

    float3 diffuse_color  = 0;
    float3 ambient_color  = 0;
    float3 specular_color = 0;

    float  material_shininess      = material.specular_exponent;
    float3 material_specular_color = material.specular;

    float3 normal_direction = in.world_normal;

    if ( vertex_buffer_has_tangents && vertex_buffer_has_bitangents ) {
  
        normal_direction = float3x3( in.world_tangent, in.world_bitangent, in.world_normal ) * sampled_normal;
    }
    normal_direction = normalize(normal_direction);

    for ( uint i = 0; i < per_scene.light_count; i++ ) {

        Light light = lights[i];

        if ( light.light_type == light_type_sunlight ) {

            float3 light_direction = normalize( -1.0 * light.position );

            float diffuse_intensity = saturate( -1.0 * dot(light_direction, normal_direction) );

            diffuse_color += light.color * base_color * diffuse_intensity;

            if ( diffuse_intensity > 0.0 ) {

                float3 reflection = reflect( light_direction, normal_direction );

                float3 camera_direction = normalize(in.world_position - per_scene.camera_position);

                float specular_intensity  = pow( saturate(-1.0 * dot(reflection, camera_direction)), material_shininess );

                specular_color += light.specular_color * material_specular_color * specular_intensity;
            }
        } else if ( light.light_type == light_type_ambientlight ) {

            ambient_color += light.color * light.intensity;

        } else if ( light.light_type == light_type_pointlight ) {

            float dist = distance( light.position, in.world_position );

            float3 light_direction = normalize( in.world_position - light.position );

            float attenuation = 1.0 / (   light.attenuation.x
                                        + light.attenuation.y * dist
                                        + light.attenuation.z * dist * dist );
      
            float diffuse_intensity = saturate( -1.0 * dot(light_direction, normal_direction) );
      
            float3 color = light.color * base_color * diffuse_intensity;
            color *= attenuation;

            diffuse_color += color;

        } else if ( light.light_type == light_type_spotlight ) {

            float dist = distance( light.position, in.world_position );

            float3 light_direction = normalize( in.world_position - light.position );
            float3 cone_direction  = normalize( light.cone_direction );
            float spot_result = dot( light_direction, cone_direction );

            if ( spot_result > cos(light.cone_angle) ) {

                float attenuation = 1.0 / (   light.attenuation.x
                                            + light.attenuation.y * dist
                                            + light.attenuation.z * dist * dist );
                                  
                attenuation *= pow( spot_result, light.cone_attenuation );
                float diffuse_intensity = saturate( dot(-1.0 * light_direction, normal_direction) );
        
                float3 color = light.color * base_color * diffuse_intensity;

                color *= attenuation;

                diffuse_color += color;
            }
        }
    }

    float3 color = saturate( diffuse_color + ambient_color + specular_color );
    return float4( color, 1.0 - material.opacity );
}
