//
//  Shaders.metal
//  Iyan3DEngine
//
//  Created by Vivek on 23/03/14.
//  Copyright (c) 2014 Vivek. All rights reserved.
//

#include <metal_stdlib>
#include <metal_geometric>
#include <simd/simd.h>

using namespace metal;

typedef struct
{
    packed_float3 position;
    packed_float3 normal;
    packed_float2 texCoord1;
    packed_float3 tangent;
    packed_float3 bitagent;
    packed_float4 optionalData1;
} vertex_t;

typedef struct
{
    packed_float3 position;
    packed_float3 normal;
    packed_float2 texCoord1;
    packed_float3 tangent;
    packed_float3 bitagent;
    packed_float4 optionalData1;
    packed_float4 optionalData2;
    packed_float4 optionalData3;
    packed_float4 optionalData4;
} vertex_heavy_t;


typedef struct
{
    packed_float3 data;
} float3Struct;

typedef struct
{
    packed_float4 data;
} float4Struct;


typedef struct {
    float isLighting;
    float pointSize [[point_size]];
    float vertexDepth, reflection;
    float isVertexColored;
    float shadowDarkness, transparency, ambientLight;
    float2 pointCoord [[ point_coord ]];
    float2 uv,texture2UV;
    float4 position [[position]] , vertexPosCam;
    float4 normal, eyeVec, lightDir, lightColor, perVertexColor;
    float3 tangent, bitangent;
} ColorInOut;



typedef struct {
    float4x4 JointTransform;
} JointData;

typedef struct {
    float4x4 data;
} MatArray;

typedef struct {
    float3 lightPosition;
} LightPosData;

// CommonSkinVertex
#define SHADER_COMMON_SKIN_mvp 1
#define SHADER_COMMON_SKIN_transparency 2
#define SHADER_COMMON_SKIN_world 3
#define SHADER_COMMON_SKIN_isLighting 4
#define SHADER_COMMON_SKIN_refraction 5
#define SHADER_COMMON_SKIN_reflection 6
#define SHADER_COMMON_SKIN_shadowDarkness 7
#define SHADER_COMMON_SKIN_lightPos 8
#define SHADER_COMMON_SKIN_eyePos 9
#define SHADER_COMMON_SKIN_lightColor 10
#define SHADER_COMMON_SKIN_lightViewProjMatrix 11
#define SHADER_COMMON_SKIN_jointData 12
#define SHADER_COMMON_SKIN_ViewMatrix 13
#define SHADER_COMMON_SKIN_VertexColor 14
#define SHADER_COMMON_ambientLight 24

#define SHADER_COMMON_isVertexColored 21

vertex ColorInOut Common_Skin_Vertex(device vertex_heavy_t* vertex_array [[ buffer(0) ]],
                                     constant float4x4& mvp [[ buffer(SHADER_COMMON_SKIN_mvp) ]],
                                     constant JointData* Joint_Data [[ buffer(SHADER_COMMON_SKIN_jointData) ]],
                                     constant MatArray* world [[ buffer(SHADER_COMMON_SKIN_world) ]],
                                     constant float& shadowDarkness [[ buffer(SHADER_COMMON_SKIN_shadowDarkness) ]],
                                     constant float& ambientLight [[ buffer(SHADER_COMMON_ambientLight) ]],
                                     constant packed_float3& eyePos [[ buffer(SHADER_COMMON_SKIN_eyePos) ]],
                                     constant float4x4& lvp [[ buffer(SHADER_COMMON_SKIN_lightViewProjMatrix) ]],
                                     unsigned int vid [[ vertex_id ]],
                                     constant float4Struct *props [[ buffer(SHADER_COMMON_isVertexColored) ]],
                                     constant float3Struct *vertColor [[buffer(SHADER_COMMON_SKIN_VertexColor)]]
                                     )
{
    ColorInOut out;
    float4 in_position = float4(float3(vertex_array[vid].position), 1.0);
    float4 in_normal = float4(float3(vertex_array[vid].normal), 0.0);
    float4 optionalData1 = vertex_array[vid].optionalData1;
    float4 optionalData2 = vertex_array[vid].optionalData2;
    float4 optionalData3 = vertex_array[vid].optionalData3;
    float4 optionalData4 = vertex_array[vid].optionalData4;
    float4 pos = float4(0.0);
    float4 nor = float4(0.0);
    
    out.isVertexColored = props[0].data[1];
    out.perVertexColor = float4(float3(vertColor[0].data),1.0);
    out.ambientLight = ambientLight;
    
    int jointId = int(optionalData1.x);
    float strength = optionalData2.x ;
    if(jointId > 0){
        pos = pos + (Joint_Data[jointId - 1].JointTransform * in_position) * strength;
        nor = nor + (Joint_Data[jointId - 1].JointTransform * in_normal) * strength;
    }else{
        pos = in_position;
        nor = in_normal;
    }
    
    jointId = int(optionalData1.y);
    strength = optionalData2.y ;
    if(jointId > 0){
        pos = pos + (Joint_Data[jointId - 1].JointTransform * in_position) * strength;
        nor = nor + (Joint_Data[jointId - 1].JointTransform * in_normal) * strength;
    }
    
    jointId = int(optionalData1.z);
    strength = optionalData2.z ;
    if(jointId > 0){
        pos = pos + (Joint_Data[jointId - 1].JointTransform * in_position) * strength;
        nor = nor + (Joint_Data[jointId - 1].JointTransform * in_normal) * strength;
    }
    
    jointId = int(optionalData1.w);
    strength = optionalData2.w;
    if(jointId > 0){
        pos = pos + (Joint_Data[jointId - 1].JointTransform * in_position) * strength;
        nor = nor + (Joint_Data[jointId - 1].JointTransform * in_normal) * strength;
    }
    
    jointId = int(optionalData3.x);
    strength = optionalData4.x;
    if(jointId > 0){
        pos = pos + (Joint_Data[jointId - 1].JointTransform * in_position) * strength;
        nor = nor + (Joint_Data[jointId - 1].JointTransform * in_normal) * strength;
    }
    
    jointId = int(optionalData3.y);
    strength = optionalData4.y;
    if(jointId > 0){
        pos = pos + (Joint_Data[jointId - 1].JointTransform * in_position) * strength;
        nor = nor + (Joint_Data[jointId - 1].JointTransform * in_normal) * strength;
    }
    
    jointId = int(optionalData3.z);
    strength = optionalData4.z;
    if(jointId > 0){
        pos = pos + (Joint_Data[jointId - 1].JointTransform * in_position) * strength;
        nor = nor + (Joint_Data[jointId - 1].JointTransform * in_normal) * strength;
    }
    
    jointId = int(optionalData3.w);
    strength = optionalData4.w;
    if(jointId > 0){
        pos = pos + (Joint_Data[jointId - 1].JointTransform * in_position) * strength;
        nor = nor + (Joint_Data[jointId - 1].JointTransform * in_normal) * strength;
    }
    nor = normalize(nor);
    
    float4 vertex_position_objectspace = pos;
    float4 vertex_position_cameraspace = world[0].data * pos;
    out.vertexPosCam = vertex_position_cameraspace;
    
    out.transparency = props[0].data[0];
    out.position = mvp * vertex_position_objectspace;
    float2 uv = vertex_array[vid].texCoord1;
    out.uv.x = uv.x;
    out.uv.y = uv.y;
    
    float4 vertexLightCoord = (lvp * world[0].data) * vertex_position_objectspace;
    float4 texCoords = vertexLightCoord / vertexLightCoord.w;
    out.texture2UV = float4((texCoords / 2.0) + 0.5).xy;
    out.texture2UV.y = (1.0 - out.texture2UV.y); // need to flip metal texture vertically
    out.vertexDepth = texCoords.z;
    
    if(int(props[0].data[3]) == 1){
        out.isLighting = 1.0;
        float4 eye_position_cameraspace = float4(float3(eyePos),1.0);
        
        out.normal = normalize(world[0].data * float4(float3(nor.xyz), 0.0));
        out.eyeVec = normalize(eye_position_cameraspace - vertex_position_cameraspace);
        out.reflection = props[0].data[2];
        out.shadowDarkness = shadowDarkness;
    }else{
        out.isLighting = 0.0;
        out.shadowDarkness = 0.0;
    }
    
    out.tangent = normalize(float3(world[0].data * float4(vertex_array[vid].tangent, 0.0)));
    out.bitangent = normalize(float3(world[0].data * float4(vertex_array[vid].bitagent, 0.0)));
    
    return out;
}

#define SHADER_PARTICLE_vp 1
#define SHADER_PARTICLE_sColor 2
#define SHADER_PARTICLE_mColor 3
#define SHADER_PARTICLE_eColor 4
#define SHADER_PARTICLE_props 5
#define SHADER_PARTICLE_positions 6
#define SHADER_PARTICLE_rotations 7
#define SHADER_PARTICLE_texture1 8
#define SHADER_PARTICLE_world 9


vertex ColorInOut Particle_Vertex(device vertex_t* vertex_array [[ buffer(0) ]],
                                  constant float4x4& vp [[ buffer(SHADER_PARTICLE_vp) ]],
                                  constant float4x4& model [[ buffer(SHADER_PARTICLE_world)]],
                                  constant packed_float4& sColor [[ buffer(SHADER_PARTICLE_sColor) ]],
                                  constant packed_float4& mColor [[ buffer(SHADER_PARTICLE_mColor) ]],
                                  constant packed_float4& eColor [[ buffer(SHADER_PARTICLE_eColor) ]],
                                  constant packed_float4& props [[ buffer(SHADER_PARTICLE_props) ]],
                                  constant float* isVertexColored[[ buffer(SHADER_COMMON_isVertexColored)]],
                                  unsigned int vid [[ vertex_id ]]
                                  )
{
    
    float4 in_position = float4(float3(vertex_array[vid].position), 1.0);
    float4 in_normal = float4(float3(vertex_array[vid].normal), 0.0);
    
    
    ColorInOut out;
    
    float percent = (in_normal.x/props[0]);
    float phase = (percent > 0.5);
    
    float4 startColor = float4(sColor[0], sColor[1], sColor[2], 1.0);
    float4 midColor = float4(mColor[0], mColor[1], mColor[2], 1.0);
    
    float4 s = mix(startColor, midColor, phase);
    float4 e = mix(midColor, eColor, phase);
    float age = mix(percent, float(percent - 0.5), phase) * 2.0;
    
    out.perVertexColor = (int(isVertexColored[0]) == 0) ? float4(mix(s, e, age)) : float4(sColor);
    
    float live = float(in_normal.x > 0.0 && in_normal.x <= props[0]);
    
    float3 temp = float3(props[1] - in_position.x, props[2] - in_position.y, props[3] - in_position.z);
    float dist = sqrt(temp.x * temp.x + temp.y * temp.y + temp.z * temp.z);
    
    out.position = vp * in_position;
    
    out.pointSize = (64.0 * sColor[3]/mColor[3] * live) * (100.0/dist);
    
    return out;
}

vertex ColorInOut Particle_Vertex_RTT(device vertex_t* vertex_array [[ buffer(0) ]],
                                      constant float4x4& vp [[ buffer(SHADER_PARTICLE_vp) ]],
                                      constant float4x4& model [[ buffer(SHADER_PARTICLE_world)]],
                                      constant packed_float4& sColor [[ buffer(SHADER_PARTICLE_sColor) ]],
                                      constant packed_float4& mColor [[ buffer(SHADER_PARTICLE_mColor) ]],
                                      constant packed_float4& eColor [[ buffer(SHADER_PARTICLE_eColor) ]],
                                      constant packed_float4& props [[ buffer(SHADER_PARTICLE_props) ]],
                                      constant float4Struct *positions [[ buffer(SHADER_PARTICLE_positions) ]],
                                      constant float4Struct *rotations [[ buffer(SHADER_PARTICLE_rotations) ]],
                                      constant float* isVertexColored[[ buffer(SHADER_COMMON_isVertexColored)]],
                                      unsigned int vid [[ vertex_id ]]
                                      )
{
    float4 in_position = float4(float3(vertex_array[vid].position), 1.0);
    float4 in_normal = float4(float3(vertex_array[vid].normal), 0.0);
    
    ColorInOut out;
    
    out.perVertexColor = float4(sColor);
    
    float live = float(in_normal.x > 0.0 && in_normal.x <= props[0]);
    
    float3 temp = float3(props[1] - in_position[0], props[2] - in_position[1], props[3] - in_position[2]);
    float dist = sqrt(temp.x * temp.x + temp.y * temp.y + temp.z * temp.z);
    
    out.position = vp * in_position;
    out.pointSize = (8.0 * live) * (100.0/dist);
    
    return out;
}


fragment float4 Particle_Fragment(ColorInOut in [[stage_in]], texture2d<half>  tex2D [[texture(SHADER_PARTICLE_texture1)]])
{
    float4 color = in.perVertexColor;
    constexpr sampler quad_sampler(address::repeat,filter::linear);
    color[3] =  tex2D.sample(quad_sampler, in.pointCoord)[0];
    return color;
}

fragment float4 Particle_Fragment_RTT(ColorInOut in [[stage_in]], texture2d<half>  tex2D [[texture(SHADER_PARTICLE_texture1)]])
{
    float4 color = in.perVertexColor;
    return color;
}


#define SHADER_COMMON_mvp 1
#define SHADER_COMMON_transparency 2
#define SHADER_COMMON_world 3
#define SHADER_COMMON_isLighting 4
#define SHADER_COMMON_refraction 5
#define SHADER_COMMON_reflection 6
#define SHADER_COMMON_shadowDarkness 7
#define SHADER_COMMON_lightPos 8
#define SHADER_COMMON_eyePos 9
#define SHADER_COMMON_lightColor 10
#define SHADER_COMMON_lightViewProjMatrix 11
#define SHADER_COMMON_viewMatrix 12
#define SHADER_COMMON_lightFadeDistance 20
#define SHADER_COMMON_lightType 22
#define SHADER_COMMON_samplerType 23
#define SHADER_COMMON_hasReflectionMap 25
#define SHADER_COMMON_hasNormalMap 26
#define SHADER_COMMON_uvScale 27

vertex ColorInOut Common_Vertex(device vertex_t* vertex_array [[ buffer(0) ]],
                                constant float4x4& vp [[ buffer(SHADER_COMMON_mvp) ]],
                                constant MatArray* world [[ buffer(SHADER_COMMON_world) ]],
                                constant float4x4& lvp [[ buffer(SHADER_COMMON_lightViewProjMatrix) ]],
                                constant packed_float3& eyePos [[ buffer(SHADER_COMMON_eyePos) ]],
                                constant float& shadowDarkness [[ buffer(SHADER_COMMON_shadowDarkness) ]],
                                constant float& ambientLight [[ buffer(SHADER_COMMON_ambientLight) ]],
                                unsigned int vid [[ vertex_id ]],
                                constant float4Struct *props [[ buffer(SHADER_COMMON_isVertexColored) ]],
                                constant float3Struct *vertColor [[buffer(SHADER_COMMON_SKIN_VertexColor)]],
                                unsigned int iid [[ instance_id ]]
                                )
{
    
    float4 vertex_position_objectspace = float4(float3(vertex_array[vid].position), 1.0);
    float4 vertex_position_cameraspace = world[iid].data * float4(float3(vertex_array[vid].position), 1.0);
    
    ColorInOut out;
    out.vertexPosCam = vertex_position_cameraspace;
    out.transparency = props[iid].data[0];
    out.position = vp * world[iid].data * vertex_position_objectspace;
    out.isVertexColored = props[iid].data[1];
    float2 uv = vertex_array[vid].texCoord1;
    out.uv.x = uv.x;
    out.uv.y = uv.y;
    
    float4 vertexLightCoord = (lvp * world[iid].data) * vertex_position_objectspace;
    float4 texCoords = vertexLightCoord/vertexLightCoord.w;
    out.texture2UV = float4((texCoords / 2.0) + 0.5).xy;
    out.texture2UV.y = (1.0 - out.texture2UV.y); // need to flip metal texture vertically
    out.vertexDepth = texCoords.z;
    out.perVertexColor = float4(float3(vertColor[iid].data),1.0);
    out.ambientLight = ambientLight;
    
    if(int(props[iid].data[3]) == 1){
        out.isLighting = 1.0;
        float4 eye_position_cameraspace =  float4(float3(eyePos),1.0);
        
        out.normal = normalize(world[iid].data * float4(float3(vertex_array[vid].normal),0.0));
        out.eyeVec = normalize(eye_position_cameraspace - vertex_position_cameraspace);
        out.reflection = props[iid].data[2];
        out.shadowDarkness = shadowDarkness;
    }else{
        out.isLighting = 0.0;
        out.shadowDarkness = 0.0;
        out.lightColor = float4(1.0);
    }
    
    out.tangent = normalize(float3(world[0].data * float4(vertex_array[vid].tangent, 0.0)));
    out.bitangent = normalize(float3(world[0].data * float4(vertex_array[vid].bitagent, 0.0)));
    
    return out;
}

#define SHADER_COMMON_colorMap 0
#define SHADER_COMMON_shadowMap 1
#define SHADER_COMMON_normalMap 2
#define SHADER_COMMON_reflectionMap 3

// Fragment shader function
fragment half4 Common_Fragment_L1(ColorInOut in [[stage_in]],
                                  texture2d<half>  colorMap [[texture(SHADER_COMMON_colorMap)]],
                                  depth2d<float> shadowMap [[texture(SHADER_COMMON_shadowMap)]],
                                  texture2d<half>  normalMap [[texture(SHADER_COMMON_normalMap)]],
                                  texture2d<half>  reflectionMap [[texture(SHADER_COMMON_reflectionMap)]],
                                  constant packed_float3* lightPos [[ buffer(SHADER_COMMON_lightPos) ]],
                                  constant packed_float3* lightColor [[ buffer(SHADER_COMMON_lightColor) ]],
                                  constant float* lightFadeDistance[[ buffer(SHADER_COMMON_lightFadeDistance)]],
                                  constant float* lightType [[ buffer(SHADER_COMMON_lightType) ]],
                                  constant float& hasReflectionMap [[ buffer(SHADER_COMMON_hasReflectionMap) ]],
                                  constant float& hasNormalMap [[ buffer(SHADER_COMMON_hasNormalMap) ]],
                                  constant float& uvScale [[ buffer(SHADER_COMMON_uvScale) ]],
                                  constant float& samplerType [[ buffer(SHADER_COMMON_samplerType) ]])
{
    float shadowBias = 0.005,shadowValue = 0.0,maxSpecular = 30.0;
    if(in.shadowDarkness > 0.0){
        constexpr sampler linear_sampler(min_filter::linear, mag_filter::linear);
        float depth = shadowMap.sample(linear_sampler, in.texture2UV);
        if((depth + shadowBias) < in.vertexDepth)
            shadowValue = in.shadowDarkness;
    }
    
    half4 diffuse_color = half4(in.perVertexColor);
    half texTransparency = 1.0;
    constexpr sampler quad_sampler(address::repeat, filter::linear);
    if(in.isVertexColored < 0.5) {
        if(samplerType == 0.0) {
            diffuse_color =  colorMap.sample(quad_sampler, in.uv * uvScale);
        }
        if(samplerType == 1.0) {
            constexpr sampler nearest_sampler(min_filter::nearest, mag_filter::nearest);
            diffuse_color =  colorMap.sample(nearest_sampler, in.uv * uvScale);
        }
        
        texTransparency = diffuse_color.w;
    }
    
    if(texTransparency <= 0.5)
        discard_fragment();
    
    half4 specular = half4(0.0), colorOfLight = half4(1.0);
    
    if(in.isLighting > 0.5) {
        colorOfLight = half4(0.0);
        
        float4 light_position_cameraspace = float4(float3(lightPos[0]),1.0);
        float4 lightDir = (lightType[0] == 1.0) ? light_position_cameraspace : normalize(light_position_cameraspace - in.vertexPosCam);
        half distanceFromLight = distance(light_position_cameraspace , in.vertexPosCam);
        
        float4 normal = normalize(in.normal);
        if(hasNormalMap > 0.5) {
            half4 n =  normalMap.sample(quad_sampler, in.uv);
            float3x3 tbnMatrix = float3x3(in.tangent.xyz, in.bitangent.xyz, in.normal.xyz);
            normal = float4(normalize(tbnMatrix * float3(n.xyz * uvScale)), 0.0);
        }
        
        float4 eyeVec = normalize(in.eyeVec);
        float n_dot_l = saturate(dot(normal, lightDir));
        
        float4 reflectValue = -lightDir + 2.0f * n_dot_l * normal;
        float e_dot_r = saturate(dot(eyeVec, reflectValue));
        
        if(hasReflectionMap > 0.5) {
            float4 r = reflect(-eyeVec, normal);
            float m = 2. * sqrt(pow(r.x, 2.0) + pow(r.y, 2.0) + pow(r.z + 1.0, 2.0));
            float2 vN = r.xy / m + .5;
            vN.y = -vN.y;
            specular = reflectionMap.sample(quad_sampler, vN);
        } else {
            specular = half4(in.reflection * pow(e_dot_r, maxSpecular));
            
            float e_dot_l = dot(lightDir, eyeVec);
            if(e_dot_l < -0.8)
                specular = half4(0.0);
        }
        
        float distanceRatio = 1.0;
        if(lightType[0] == 0.0)
            distanceRatio = (1.0 - saturate(distanceFromLight/lightFadeDistance[0]));
        
        float darkness = distanceRatio * n_dot_l;
        darkness = clamp(darkness, in.ambientLight, 1.0);
        colorOfLight += half4(half3(lightColor[0]) * darkness, 1.0);
    }
    
    half4 finalColor = half4(diffuse_color + specular) * colorOfLight;
    if(hasReflectionMap > 0.5)
        finalColor = half4(diffuse_color * (1.0 - in.reflection) + specular * in.reflection) * colorOfLight;
    finalColor = finalColor + (half4(0.0,0.0,0.0,0.0) - finalColor) * (shadowValue);
    return half4(half3(finalColor.xyz) , texTransparency * in.transparency);
}

fragment half4 Common_Fragment_L2(ColorInOut in [[stage_in]],
                                  texture2d<half>  colorMap [[texture(SHADER_COMMON_colorMap)]],
                                  depth2d<float> shadowMap [[texture(SHADER_COMMON_shadowMap)]],
                                  texture2d<half>  normalMap [[texture(SHADER_COMMON_normalMap)]],
                                  texture2d<half>  reflectionMap [[texture(SHADER_COMMON_reflectionMap)]],
                                  constant packed_float3* lightPos [[ buffer(SHADER_COMMON_lightPos) ]],
                                  constant packed_float3* lightColor [[ buffer(SHADER_COMMON_lightColor) ]],
                                  constant float* lightFadeDistance[[ buffer(SHADER_COMMON_lightFadeDistance)]],
                                  constant float* lightType [[ buffer(SHADER_COMMON_lightType) ]],
                                  constant float& hasReflectionMap [[ buffer(SHADER_COMMON_hasReflectionMap) ]],
                                  constant float& hasNormalMap [[ buffer(SHADER_COMMON_hasNormalMap) ]],
                                  constant float& uvScale [[ buffer(SHADER_COMMON_uvScale) ]],
                                  constant float& samplerType [[ buffer(SHADER_COMMON_samplerType) ]])
{
    float shadowBias = 0.005, shadowValue = 0.0, maxSpecular = 30.0;
    if(in.shadowDarkness > 0.0){
        constexpr sampler linear_sampler(min_filter::linear, mag_filter::linear);
        float depth = shadowMap.sample(linear_sampler,in.texture2UV);
        if((depth + shadowBias) < in.vertexDepth)
            shadowValue = in.shadowDarkness;
    }
    
    constexpr sampler quad_sampler(address::repeat, filter::linear);
    
    half4 diffuse_color = half4(in.perVertexColor);
    half texTransparency = 1.0;
    if(in.isVertexColored < 0.5) {
        if(samplerType == 0.0) {
            diffuse_color =  colorMap.sample(quad_sampler, in.uv * uvScale);
        }
        if(samplerType == 1.0) {
            constexpr sampler nearest_sampler(min_filter::nearest, mag_filter::nearest);
            diffuse_color =  colorMap.sample(nearest_sampler, in.uv * uvScale);
        }
        
        texTransparency = diffuse_color.w;
    }
    
    if(texTransparency <= 0.5)
        discard_fragment();
    
    half4 specular = half4(0.0), colorOfLight = half4(1.0);
    
    if(in.isLighting > 0.5){
        colorOfLight = half4(0.0);
        float4 normal = normalize(in.normal);
        if(hasNormalMap > 0.5) {
            half4 n =  normalMap.sample(quad_sampler, in.uv);
            float3x3 tbnMatrix = float3x3(in.tangent.xyz, in.bitangent.xyz, in.normal.xyz);
            normal = float4(normalize(tbnMatrix * float3(n.xyz * uvScale)), 0.0);
        }
        
        float4 eyeVec = normalize(in.eyeVec);
        
        for(int i = 0 ; i < 2; i++) {
            float4 light_position_cameraspace = float4(float3(lightPos[i]),1.0);
            float4 lightDir = (lightType[i] == 1.0) ? light_position_cameraspace : normalize(light_position_cameraspace - in.vertexPosCam);
            half distanceFromLight = distance(light_position_cameraspace, in.vertexPosCam);
            
            float n_dot_l = saturate(dot(normal, lightDir));
            
            float4 reflectValue = -lightDir + 2.0f * n_dot_l * normal;
            float e_dot_r = saturate(dot(eyeVec, reflectValue));
            
            if(hasReflectionMap > 0.5) {
                float4 r = reflect(-eyeVec, normal);
                float m = 2. * sqrt(pow(r.x, 2.0) + pow(r.y, 2.0) + pow(r.z + 1.0, 2.0));
                float2 vN = r.xy / m + .5;
                vN.y = -vN.y;
                specular = reflectionMap.sample(quad_sampler, vN);
            } else {
                specular = half4(in.reflection * pow(e_dot_r, maxSpecular));
                
                float e_dot_l = dot(lightDir, eyeVec);
                if(e_dot_l < -0.8)
                    specular = half4(0.0);
            }
            
            float distanceRatio = 1.0;
            if(lightType[0] == 0.0)
                distanceRatio = (1.0 - saturate(distanceFromLight / lightFadeDistance[i]));
            
            float darkness = distanceRatio * n_dot_l;
            darkness = clamp(darkness, in.ambientLight, 1.0);
            colorOfLight += half4(half3(lightColor[i]) * darkness, 1.0);
            if(i == 0)
                colorOfLight = colorOfLight + (half4(0.0, 0.0, 0.0, 0.0) - colorOfLight) * (shadowValue);
        }
    }
    
    half4 finalColor = half4(diffuse_color + specular) * colorOfLight;
    if(hasReflectionMap > 0.5)
        finalColor = half4(diffuse_color * (1.0 - in.reflection) + specular * in.reflection) * colorOfLight;
    return half4(half3(finalColor.xyz) , texTransparency * in.transparency);
}

fragment half4 Common_Fragment_L3(ColorInOut in [[stage_in]],
                                  texture2d<half>  colorMap [[texture(SHADER_COMMON_colorMap)]],
                                  depth2d<float> shadowMap [[texture(SHADER_COMMON_shadowMap)]],
                                  texture2d<half>  normalMap [[texture(SHADER_COMMON_normalMap)]],
                                  texture2d<half>  reflectionMap [[texture(SHADER_COMMON_reflectionMap)]],
                                  constant packed_float3* lightPos [[ buffer(SHADER_COMMON_lightPos) ]],
                                  constant packed_float3* lightColor [[ buffer(SHADER_COMMON_lightColor) ]],
                                  constant float* lightFadeDistance[[ buffer(SHADER_COMMON_lightFadeDistance)]],
                                  constant float* lightType [[ buffer(SHADER_COMMON_lightType) ]],
                                  constant float& hasReflectionMap [[ buffer(SHADER_COMMON_hasReflectionMap) ]],
                                  constant float& hasNormalMap [[ buffer(SHADER_COMMON_hasNormalMap) ]],
                                  constant float& uvScale [[ buffer(SHADER_COMMON_uvScale) ]],
                                  constant float& samplerType [[ buffer(SHADER_COMMON_samplerType) ]])
{    float shadowBias = 0.005, shadowValue = 0.0, maxSpecular = 30.0;
    if(in.shadowDarkness > 0.0){
        constexpr sampler linear_sampler(min_filter::linear, mag_filter::linear);
        float depth = shadowMap.sample(linear_sampler,in.texture2UV);
        if((depth + shadowBias) < in.vertexDepth)
            shadowValue = in.shadowDarkness;
    }
    
    constexpr sampler quad_sampler(address::repeat, filter::linear);
    
    half4 diffuse_color = half4(in.perVertexColor);
    half texTransparency = 1.0;
    if(in.isVertexColored < 0.5) {
        if(samplerType == 0.0) {
            diffuse_color =  colorMap.sample(quad_sampler, in.uv * uvScale);
        }
        if(samplerType == 1.0) {
            constexpr sampler nearest_sampler(min_filter::nearest, mag_filter::nearest);
            diffuse_color =  colorMap.sample(nearest_sampler, in.uv * uvScale);
        }
        
        texTransparency = diffuse_color.w;
    }
    
    if(texTransparency <= 0.5)
        discard_fragment();
    
    half4 specular = half4(0.0), colorOfLight = half4(1.0);
    
    if(in.isLighting > 0.5){
        colorOfLight = half4(0.0);
        float4 normal = normalize(in.normal);
        if(hasNormalMap > 0.5) {
            half4 n =  normalMap.sample(quad_sampler, in.uv);
            float3x3 tbnMatrix = float3x3(in.tangent.xyz, in.bitangent.xyz, in.normal.xyz);
            normal = float4(normalize(tbnMatrix * float3(n.xyz * uvScale)), 0.0);
        }
        
        float4 eyeVec = normalize(in.eyeVec);
        
        for(int i = 0 ; i < 3; i++) {
            float4 light_position_cameraspace = float4(float3(lightPos[i]),1.0);
            float4 lightDir = (lightType[i] == 1.0) ? light_position_cameraspace : normalize(light_position_cameraspace - in.vertexPosCam);
            half distanceFromLight = distance(light_position_cameraspace, in.vertexPosCam);
            
            float n_dot_l = saturate(dot(normal, lightDir));
            
            float4 reflectValue = -lightDir + 2.0f * n_dot_l * normal;
            float e_dot_r = saturate(dot(eyeVec, reflectValue));
            
            if(hasReflectionMap > 0.5) {
                float4 r = reflect(-eyeVec, normal);
                float m = 2. * sqrt(pow(r.x, 2.0) + pow(r.y, 2.0) + pow(r.z + 1.0, 2.0));
                float2 vN = r.xy / m + .5;
                vN.y = -vN.y;
                specular = reflectionMap.sample(quad_sampler, vN);
            } else {
                specular = half4(in.reflection * pow(e_dot_r, maxSpecular));
                
                float e_dot_l = dot(lightDir, eyeVec);
                if(e_dot_l < -0.8)
                    specular = half4(0.0);
            }
            
            float distanceRatio = 1.0;
            if(lightType[0] == 0.0)
                distanceRatio = (1.0 - saturate(distanceFromLight / lightFadeDistance[i]));
            
            float darkness = distanceRatio * n_dot_l;
            darkness = clamp(darkness, in.ambientLight, 1.0);
            colorOfLight += half4(half3(lightColor[i]) * darkness, 1.0);
            if(i == 0)
                colorOfLight = colorOfLight + (half4(0.0, 0.0, 0.0, 0.0) - colorOfLight) * (shadowValue);
        }
    }
    
    half4 finalColor = half4(diffuse_color + specular) * colorOfLight;
    if(hasReflectionMap > 0.5)
        finalColor = half4(diffuse_color * (1.0 - in.reflection) + specular * in.reflection) * colorOfLight;
    return half4(half3(finalColor.xyz) , texTransparency * in.transparency);
}

fragment half4 Common_Fragment_L4(ColorInOut in [[stage_in]],
                                  texture2d<half>  colorMap [[texture(SHADER_COMMON_colorMap)]],
                                  depth2d<float> shadowMap [[texture(SHADER_COMMON_shadowMap)]],
                                  texture2d<half>  normalMap [[texture(SHADER_COMMON_normalMap)]],
                                  texture2d<half>  reflectionMap [[texture(SHADER_COMMON_reflectionMap)]],
                                  constant packed_float3* lightPos [[ buffer(SHADER_COMMON_lightPos) ]],
                                  constant packed_float3* lightColor [[ buffer(SHADER_COMMON_lightColor) ]],
                                  constant float* lightFadeDistance[[ buffer(SHADER_COMMON_lightFadeDistance)]],
                                  constant float* lightType [[ buffer(SHADER_COMMON_lightType) ]],
                                  constant float& hasReflectionMap [[ buffer(SHADER_COMMON_hasReflectionMap) ]],
                                  constant float& hasNormalMap [[ buffer(SHADER_COMMON_hasNormalMap) ]],
                                  constant float& uvScale [[ buffer(SHADER_COMMON_uvScale) ]],
                                  constant float& samplerType [[ buffer(SHADER_COMMON_samplerType) ]])
{
    float shadowBias = 0.005, shadowValue = 0.0, maxSpecular = 30.0;
    if(in.shadowDarkness > 0.0){
        constexpr sampler linear_sampler(min_filter::linear, mag_filter::linear);
        float depth = shadowMap.sample(linear_sampler, in.texture2UV);
        if((depth + shadowBias) < in.vertexDepth)
            shadowValue = in.shadowDarkness;
    }
    
    constexpr sampler quad_sampler(address::repeat, filter::linear);
    
    half4 diffuse_color = half4(in.perVertexColor);
    half texTransparency = 1.0;
    if(in.isVertexColored < 0.5) {
        if(samplerType == 0.0) {
            diffuse_color =  colorMap.sample(quad_sampler, in.uv * uvScale);
        }
        if(samplerType == 1.0) {
            constexpr sampler nearest_sampler(min_filter::nearest, mag_filter::nearest);
            diffuse_color =  colorMap.sample(nearest_sampler, in.uv * uvScale);
        }
        
        texTransparency = diffuse_color.w;
    }
    
    if(texTransparency <= 0.5)
        discard_fragment();
    
    half4 specular = half4(0.0), colorOfLight = half4(1.0);
    
    if(in.isLighting > 0.5){
        colorOfLight = half4(0.0);
        float4 normal = normalize(in.normal);
        if(hasNormalMap > 0.5) {
            half4 n =  normalMap.sample(quad_sampler, in.uv);
            float3x3 tbnMatrix = float3x3(in.tangent.xyz, in.bitangent.xyz, in.normal.xyz);
            normal = float4(normalize(tbnMatrix * float3(n.xyz * uvScale)), 0.0);
        }
        
        float4 eyeVec = normalize(in.eyeVec);
        
        for(int i = 0 ; i < 4; i++) {
            float4 light_position_cameraspace = float4(float3(lightPos[i]),1.0);
            float4 lightDir = (lightType[i] == 1.0) ? light_position_cameraspace : normalize(light_position_cameraspace - in.vertexPosCam);
            half distanceFromLight = distance(light_position_cameraspace, in.vertexPosCam);
            
            float n_dot_l = saturate(dot(normal, lightDir));
            
            float4 reflectValue = -lightDir + 2.0f * n_dot_l * normal;
            float e_dot_r = saturate(dot(eyeVec, reflectValue));
            
            if(hasReflectionMap > 0.5) {
                float4 r = reflect(-eyeVec, normal);
                float m = 2. * sqrt(pow(r.x, 2.0) + pow(r.y, 2.0) + pow(r.z + 1.0, 2.0));
                float2 vN = r.xy / m + .5;
                vN.y = -vN.y;
                specular = reflectionMap.sample(quad_sampler, vN);
            } else {
                specular = half4(in.reflection * pow(e_dot_r, maxSpecular));
                
                float e_dot_l = dot(lightDir, eyeVec);
                if(e_dot_l < -0.8)
                    specular = half4(0.0);
            }
            
            float distanceRatio = 1.0;
            if(lightType[0] == 0.0)
                distanceRatio = (1.0 - saturate(distanceFromLight / lightFadeDistance[i]));
            
            float darkness = distanceRatio * n_dot_l;
            darkness = clamp(darkness, in.ambientLight, 1.0);
            colorOfLight += half4(half3(lightColor[i]) * darkness, 1.0);
            if(i == 0)
                colorOfLight = colorOfLight + (half4(0.0, 0.0, 0.0, 0.0) - colorOfLight) * (shadowValue);
        }
    }
    
    half4 finalColor = half4(diffuse_color + specular) * colorOfLight;
    if(hasReflectionMap > 0.5)
        finalColor = half4(diffuse_color * (1.0 - in.reflection) + specular * in.reflection) * colorOfLight;
    return half4(half3(finalColor.xyz) , texTransparency * in.transparency);
}


fragment half4 Common_Fragment_L5(ColorInOut in [[stage_in]],
                                  texture2d<half>  colorMap [[texture(SHADER_COMMON_colorMap)]],
                                  depth2d<float> shadowMap [[texture(SHADER_COMMON_shadowMap)]],
                                  texture2d<half>  normalMap [[texture(SHADER_COMMON_normalMap)]],
                                  texture2d<half>  reflectionMap [[texture(SHADER_COMMON_reflectionMap)]],
                                  constant packed_float3* lightPos [[ buffer(SHADER_COMMON_lightPos) ]],
                                  constant packed_float3* lightColor [[ buffer(SHADER_COMMON_lightColor) ]],
                                  constant float* lightFadeDistance[[ buffer(SHADER_COMMON_lightFadeDistance)]],
                                  constant float* lightType [[ buffer(SHADER_COMMON_lightType) ]],
                                  constant float& hasReflectionMap [[ buffer(SHADER_COMMON_hasReflectionMap) ]],
                                  constant float& hasNormalMap [[ buffer(SHADER_COMMON_hasNormalMap) ]],
                                  constant float& uvScale [[ buffer(SHADER_COMMON_uvScale) ]],
                                  constant float& samplerType [[ buffer(SHADER_COMMON_samplerType) ]])
{    float shadowBias = 0.005, shadowValue = 0.0, maxSpecular = 30.0;
    if(in.shadowDarkness > 0.0){
        constexpr sampler linear_sampler(min_filter::linear, mag_filter::linear);
        float depth = shadowMap.sample(linear_sampler,in.texture2UV);
        if((depth + shadowBias) < in.vertexDepth)
            shadowValue = in.shadowDarkness;
    }
    
    constexpr sampler quad_sampler(address::repeat, filter::linear);
    
    half4 diffuse_color = half4(in.perVertexColor);
    half texTransparency = 1.0;
    if(in.isVertexColored < 0.5) {
        if(samplerType == 0.0) {
            diffuse_color =  colorMap.sample(quad_sampler, in.uv * uvScale);
        }
        if(samplerType == 1.0) {
            constexpr sampler nearest_sampler(min_filter::nearest, mag_filter::nearest);
            diffuse_color =  colorMap.sample(nearest_sampler, in.uv * uvScale);
        }
        
        texTransparency = diffuse_color.w;
    }
    
    if(texTransparency <= 0.5)
        discard_fragment();
    
    half4 specular = half4(0.0), colorOfLight = half4(1.0);
    
    if(in.isLighting > 0.5){
        colorOfLight = half4(0.0);
        float4 normal = normalize(in.normal);
        if(hasNormalMap > 0.5) {
            half4 n =  normalMap.sample(quad_sampler, in.uv);
            float3x3 tbnMatrix = float3x3(in.tangent.xyz, in.bitangent.xyz, in.normal.xyz);
            normal = float4(normalize(tbnMatrix * float3(n.xyz * uvScale)), 0.0);
        }
        
        float4 eyeVec = normalize(in.eyeVec);
        
        for(int i = 0 ; i < 5; i++) {
            float4 light_position_cameraspace = float4(float3(lightPos[i]),1.0);
            float4 lightDir = (lightType[i] == 1.0) ? light_position_cameraspace : normalize(light_position_cameraspace - in.vertexPosCam);
            half distanceFromLight = distance(light_position_cameraspace, in.vertexPosCam);
            
            float n_dot_l = saturate(dot(normal, lightDir));
            
            float4 reflectValue = -lightDir + 2.0f * n_dot_l * normal;
            float e_dot_r = saturate(dot(eyeVec, reflectValue));
            
            if(hasReflectionMap > 0.5) {
                float4 r = reflect(-eyeVec, normal);
                float m = 2. * sqrt(pow(r.x, 2.0) + pow(r.y, 2.0) + pow(r.z + 1.0, 2.0));
                float2 vN = r.xy / m + .5;
                vN.y = -vN.y;
                specular = reflectionMap.sample(quad_sampler, vN);
            } else {
                specular = half4(in.reflection * pow(e_dot_r, maxSpecular));
                
                float e_dot_l = dot(lightDir, eyeVec);
                if(e_dot_l < -0.8)
                    specular = half4(0.0);
            }
            
            float distanceRatio = 1.0;
            if(lightType[0] == 0.0)
                distanceRatio = (1.0 - saturate(distanceFromLight / lightFadeDistance[i]));
            
            float darkness = distanceRatio * n_dot_l;
            darkness = clamp(darkness, in.ambientLight, 1.0);
            colorOfLight += half4(half3(lightColor[i]) * darkness, 1.0);
            if(i == 0)
                colorOfLight = colorOfLight + (half4(0.0, 0.0, 0.0, 0.0) - colorOfLight) * (shadowValue);
        }
    }
    
    half4 finalColor = half4(diffuse_color + specular) * colorOfLight;
    if(hasReflectionMap > 0.5)
        finalColor = half4(diffuse_color * (1.0 - in.reflection) + specular * in.reflection) * colorOfLight;
    return half4(half3(finalColor.xyz), texTransparency * in.transparency);
}



#define SHADER_TOON_SKIN_mvp 1
#define SHADER_TOON_SKIN_transparency 2
#define SHADER_TOON_SKIN_world 3
#define SHADER_TOON_SKIN_isLighting 4
#define SHADER_TOON_SKIN_refraction 5
#define SHADER_TOON_SKIN_reflection 6
#define SHADER_TOON_SKIN_shadowDarkness 7
#define SHADER_TOON_SKIN_lightPos 8
#define SHADER_TOON_SKIN_eyePos 9
#define SHADER_TOON_SKIN_lightColor 10
#define SHADER_TOON_SKIN_lightViewProjMatrix 11
#define SHADER_TOON_SKIN_jointData 12

#define SHADER_TOON_mvp 1
#define SHADER_TOON_transparency 2
#define SHADER_TOON_world 3
#define SHADER_TOON_isLighting 4
#define SHADER_TOON_refraction 5
#define SHADER_TOON_reflection 6
#define SHADER_TOON_shadowDarkness 7
#define SHADER_TOON_lightPos 8
#define SHADER_TOON_eyePos 9
#define SHADER_TOON_lightColor 10
#define SHADER_TOON_lightViewProjMatrix 11

fragment half4 Common_Toon_Fragment(ColorInOut in [[stage_in]],texture2d<half>  colorMap [[texture(SHADER_COMMON_colorMap)]],
                                    depth2d<float> shadowMap [[texture(SHADER_COMMON_shadowMap)]],
                                    texture2d<half>  normalMap [[texture(SHADER_COMMON_normalMap)]],
                                    texture2d<half>  reflectionMap [[texture(SHADER_COMMON_reflectionMap)]],
                                    constant packed_float3* lightPos [[ buffer(SHADER_COMMON_lightPos) ]],
                                    constant packed_float3* lightColor [[ buffer(SHADER_COMMON_lightColor) ]],
                                    constant float& hasReflectionMap [[ buffer(SHADER_COMMON_hasReflectionMap) ]],
                                    constant float& hasNormalMap [[ buffer(SHADER_COMMON_hasNormalMap) ]],
                                    constant float& uvScale [[ buffer(SHADER_COMMON_uvScale) ]],
                                    constant float* lightType [[ buffer(SHADER_COMMON_lightType) ]])
{
    float shadowBias = 0.005,shadowValue = 0.0,maxSpecular = 30.0;
    if(in.shadowDarkness > 0.0){
        constexpr sampler linear_sampler(min_filter::linear, mag_filter::linear);
        float depth = shadowMap.sample(linear_sampler,in.texture2UV);
        if((depth + shadowBias) < in.vertexDepth)
            shadowValue = in.shadowDarkness;
    }
    
    constexpr sampler quad_sampler(address::repeat, filter::linear);
    
    half4 diffuse_color = half4(in.perVertexColor);
    half texTransparency = 1.0;
    if(in.isVertexColored < 0.5) {
        diffuse_color =  colorMap.sample(quad_sampler, in.uv * uvScale);
        texTransparency = diffuse_color.w;
    }
    
    half4 specular;
    half4 finalColor;
    if(in.isLighting > 0.5){
        
        float4 light_position_cameraspace = float4(float3(lightPos[0]),1.0);
        float4 lightDir = (lightType[0] == 1.0) ? light_position_cameraspace : normalize(light_position_cameraspace - in.vertexPosCam);
        
        float4 normal = normalize(in.normal);
        float4 eyeVec = normalize(in.eyeVec);
        
        float n_dot_l = saturate(dot(normal,lightDir));
        float4 diffuse = float4(float3(n_dot_l),1.0);
        diffuse_color = half4(diffuse) * half4(diffuse_color);
        
        float4 reflectValue = -lightDir + 2.0f * n_dot_l * normal;
        float e_dot_r =  saturate(dot(eyeVec,reflectValue));
        
        if(hasReflectionMap > 0.5) {
            float4 r = reflect(-eyeVec, normal);
            float m = 2. * sqrt(pow(r.x, 2.0) + pow(r.y, 2.0) + pow(r.z + 1.0, 2.0));
            float2 vN = r.xy / m + .5;
            vN.y = -vN.y;
            specular = reflectionMap.sample(quad_sampler, vN);
        } else {
            specular = half4(in.reflection * pow(e_dot_r, maxSpecular));
            
            float e_dot_l = dot(lightDir, eyeVec);
            if(e_dot_l < -0.8)
                specular = half4(0.0);
        }
        
        half4 toonColor = half4(diffuse_color + specular);
        if(n_dot_l > 0.95)
            toonColor = half4(1.0,1.0,1.0,1.0) * toonColor;
        else if(n_dot_l > 0.6)
            toonColor = half4(0.8,0.8,0.8,1.0) * toonColor;
        else if(n_dot_l > 0.2)
            toonColor = half4(0.6,0.6,0.6,1.0) * toonColor;
        else
            toonColor = half4(0.4,0.4,0.4,1.0) * toonColor;
        finalColor = toonColor * half4(half3(lightColor[0]),1.0);
    } else {
        specular = half4(0.0);
        finalColor = half4(diffuse_color);
    }

    finalColor = finalColor + (half4(0.0,0.0,0.0,0.0) - finalColor) * (shadowValue);
    return half4(half3(finalColor.xyz) , texTransparency * in.transparency);
}

#define SHADER_COLOR_mvp 1
#define SHADER_COLOR_vertexColor 2
#define SHADER_COLOR_transparency 3

vertex ColorInOut Color_Vertex(device vertex_t* vertex_array [[ buffer(0) ]],
                               constant float4x4& mvp [[buffer(SHADER_COLOR_mvp)]],
                               constant float3Struct *vertColor [[buffer(SHADER_COLOR_vertexColor)]],
                               constant float* transparency [[ buffer(SHADER_COLOR_transparency) ]],
                               unsigned int vid [[ vertex_id ]]
                               )
{
    ColorInOut out;
    out.position = (mvp) * float4(float3(vertex_array[vid].position), 1.0);
    out.perVertexColor = float4(float3(vertColor[0].data),transparency[0]);
    return out;
}

fragment half4 Color_Fragment(ColorInOut in [[stage_in]])
{
    half4 color = half4(in.perVertexColor);
    return color;
}


#define SHADER_COLOR_SKIN_mvp 1
#define SHADER_COLOR_SKIN_vertexColor 2
#define SHADER_COLOR_SKIN_jointData 3
#define SHADER_COLOR_SKIN_transparency 4

vertex ColorInOut Color_Skin_Vertex(device vertex_heavy_t* vertex_array [[ buffer(0) ]],
                                    constant float4x4& mvp [[ buffer(SHADER_COLOR_SKIN_mvp) ]],
                                    constant float3Struct *vertColor [[buffer(SHADER_COLOR_SKIN_vertexColor)]],
                                    constant JointData* Joint_Data [[ buffer(SHADER_COLOR_SKIN_jointData) ]],
                                    constant float* transparency [[ buffer(SHADER_COLOR_SKIN_transparency) ]],
                                    unsigned int vid [[ vertex_id ]]
                                    )
{
    ColorInOut out;
    float4 in_position = float4(float3(vertex_array[vid].position), 1.0);
    float4 optionalData1 = vertex_array[vid].optionalData1;
    float4 optionalData2 = vertex_array[vid].optionalData2;
    float4 optionalData3 = vertex_array[vid].optionalData3;
    float4 optionalData4 = vertex_array[vid].optionalData4;
    float4 pos = float4(0.0);
    
    int jointId = int(optionalData1.x);
    float strength = optionalData2.x ;
    if(jointId > 0)
        pos = pos + (Joint_Data[jointId - 1].JointTransform * in_position) * strength;
    else
        pos = in_position;
    
    jointId = int(optionalData1.y);
    strength = optionalData2.y ;
    if(jointId > 0)
        pos = pos + (Joint_Data[jointId - 1].JointTransform * in_position) * strength;
    
    jointId = int(optionalData1.z);
    strength = optionalData2.z ;
    if(jointId > 0)
        pos = pos + (Joint_Data[jointId - 1].JointTransform * in_position) * strength;
    
    jointId = int(optionalData1.w);
    strength = optionalData2.w;
    if(jointId > 0)
        pos = pos + (Joint_Data[jointId - 1].JointTransform * in_position) * strength;
    
    jointId = int(optionalData3.x);
    strength = optionalData4.x;
    if(jointId > 0)
        pos = pos + (Joint_Data[jointId - 1].JointTransform * in_position) * strength;
    
    jointId = int(optionalData3.y);
    strength = optionalData4.y;
    if(jointId > 0)
        pos = pos + (Joint_Data[jointId - 1].JointTransform * in_position) * strength;
    
    jointId = int(optionalData3.z);
    strength = optionalData4.z;
    if(jointId > 0)
        pos = pos + (Joint_Data[jointId - 1].JointTransform * in_position) * strength;
    
    jointId = int(optionalData3.w);
    strength = optionalData4.w;
    if(jointId > 0)
        pos = pos + (Joint_Data[jointId - 1].JointTransform * in_position) * strength;
    
    out.position = mvp * pos;
    out.perVertexColor = float4(float3(vertColor[0].data),transparency[0]);
    return out;
}


#define SHADER_PERVERTEXCOLOR_mvp 1
#define SHADER_PERVERTEXCOLOR_transparency 2
#define SHADER_PERVERTEXCOLOR_isLighting 3
#define SHADER_PERVERTEXCOLOR_refraction 4
#define SHADER_PERVERTEXCOLOR_reflection 5
#define SHADER_PERVERTEXCOLOR_shadowDarkness 6
#define SHADER_PERVERTEXCOLOR_lightPos 7
#define SHADER_PERVERTEXCOLOR_eyePos 8
#define SHADER_PERVERTEXCOLOR_world 9
#define SHADER_PERVERTEXCOLOR_lightViewProjMatrix 10
#define SHADER_PERVERTEXCOLOR_lightColor 11
#define SHADER_PERVERTEXCOLOR_viewMatrix 12
#define SHADER_PERVERTEXCOLOR_texture1 1
#define SHADER_PERVERTEXCOLOR_jointData 13

vertex ColorInOut Per_Vertex_Color(device vertex_t* vertex_array [[ buffer(0) ]],
                                   constant float4x4& vp [[buffer(SHADER_COLOR_mvp)]],
                                   constant MatArray* world [[ buffer(SHADER_PERVERTEXCOLOR_world) ]],
                                   constant packed_float3& eyePos [[ buffer(SHADER_PERVERTEXCOLOR_eyePos) ]],
                                   constant float4x4& lvp [[ buffer(SHADER_PERVERTEXCOLOR_lightViewProjMatrix) ]],
                                   constant float& shadowDarkness [[ buffer(SHADER_PERVERTEXCOLOR_shadowDarkness) ]],
                                   constant float& ambientLight [[ buffer(SHADER_COMMON_ambientLight) ]],
                                   constant float4Struct *props [[ buffer(SHADER_COMMON_isVertexColored) ]],
                                   constant float3Struct *vertColor [[buffer(SHADER_COMMON_SKIN_VertexColor)]],
                                   unsigned int vid [[ vertex_id ]],
                                   unsigned int iid [[ instance_id ]]
                                   ) {
    float4 vertex_position_objectspace = float4(float3(vertex_array[vid].position), 1.0);
    float4 vertex_position_cameraspace = world[iid].data * float4(float3(vertex_array[vid].position), 1.0);
    
    ColorInOut out;
    out.vertexPosCam = vertex_position_cameraspace;
    out.transparency = props[iid].data[0];
    out.position = vp * world[iid].data * vertex_position_objectspace;
    out.perVertexColor =  (vertColor[iid].data[0] == -1.0) ? vertex_array[vid].optionalData1 : float4(float3(vertColor[iid].data),1.0);;
    out.isVertexColored = props[iid].data[1];
    out.ambientLight = ambientLight;
    
    out.texture2UV = float2(0.0);
    out.vertexDepth = 0.0;
    
    float4 vertexLightCoord = (lvp * world[iid].data) * vertex_position_objectspace;
    float4 texCoords = vertexLightCoord/vertexLightCoord.w;
    out.texture2UV = float4((texCoords / 2.0) + 0.5).xy;
    out.texture2UV.y = (1.0 - out.texture2UV.y); // need to flip metal texture vertically
    out.vertexDepth = texCoords.z;
    
    if(int(props[iid].data[3]) == 1){
        out.isLighting = 1.0;
        float4 eye_position_cameraspace = float4(float3(eyePos),1.0);
        out.normal = normalize(world[iid].data * float4(float3(vertex_array[vid].normal),0.0));
        out.eyeVec = normalize(eye_position_cameraspace - vertex_position_cameraspace);
        out.reflection = props[iid].data[2];
        out.shadowDarkness = shadowDarkness;
    }else{
        out.isLighting = 0.0;
        out.shadowDarkness = 0.0;
    }
    
    out.tangent = normalize(float3(world[0].data * float4(vertex_array[vid].tangent, 0.0)));
    out.bitangent = normalize(float3(world[0].data * float4(vertex_array[vid].bitagent, 0.0)));
    
    return out;
}

vertex ColorInOut Per_Vertex_Color_Skin(device vertex_heavy_t* vertex_array [[ buffer(0) ]],
                                        constant float4x4& mvp [[buffer(SHADER_COLOR_mvp)]],
                                        constant MatArray* world [[ buffer(SHADER_PERVERTEXCOLOR_world) ]],
                                        constant packed_float3& eyePos [[ buffer(SHADER_PERVERTEXCOLOR_eyePos) ]],
                                        constant float4x4& lvp [[ buffer(SHADER_PERVERTEXCOLOR_lightViewProjMatrix) ]],
                                        constant float& shadowDarkness [[ buffer(SHADER_PERVERTEXCOLOR_shadowDarkness) ]],
                                        constant float& ambientLight [[ buffer(SHADER_COMMON_ambientLight) ]],
                                        unsigned int vid [[ vertex_id ]],
                                        constant JointData* Joint_Data [[ buffer(SHADER_PERVERTEXCOLOR_jointData) ]],
                                        constant float4Struct *props [[ buffer(SHADER_COMMON_isVertexColored) ]],
                                        constant float3Struct *vertColor [[buffer(SHADER_COMMON_SKIN_VertexColor)]]
                                        ) {
    ColorInOut out;
    
    float4 in_position = float4(float3(vertex_array[vid].position), 1.0);
    float4 in_normal = float4(float3(vertex_array[vid].normal), 0.0);
    float4 optionalData1 = vertex_array[vid].optionalData1;
    float4 optionalData2 = vertex_array[vid].optionalData2;
    out.transparency = props[0].data[0];
    out.ambientLight = ambientLight;
    
    out.isVertexColored = props[0].data[1];
    out.perVertexColor = float4(float3(vertColor[0].data),1.0);
    float2 uv = vertex_array[vid].texCoord1;
    out.uv.x = uv.x;
    out.uv.y = uv.y;
    
    out.texture2UV = float2(0.0);
    out.vertexDepth = 0.0;
    
    
    float4 pos = float4(0.0);
    float4 nor = float4(0.0);
    
    int jointId = int(optionalData1.x);
    float strength = optionalData2.x ;
    if(jointId > 0){
        pos = pos + (Joint_Data[jointId - 1].JointTransform * in_position) * strength;
        nor = nor + (Joint_Data[jointId - 1].JointTransform * in_normal) * strength;
    }else{
        pos = in_position;
        nor = in_normal;
    }
    
    float4 vertex_position_objectspace = pos;
    float4 vertex_position_cameraspace = world[0].data * pos;
    out.vertexPosCam = vertex_position_cameraspace;
    out.position = mvp * vertex_position_objectspace;
    
    float4 vertexLightCoord = (lvp * world[0].data) * vertex_position_objectspace;
    float4 texCoords = vertexLightCoord/vertexLightCoord.w;
    out.texture2UV = float4((texCoords / 2.0) + 0.5).xy;
    out.texture2UV.y = (1.0 - out.texture2UV.y); // need to flip metal texture vertically
    out.vertexDepth = texCoords.z;
    
    if(int(props[0].data[3]) == 1) {
        out.isLighting = 1.0;
        float4 eye_position_cameraspace = float4(float3(eyePos),1.0);
        
        out.normal = normalize(world[0].data * nor);
        out.eyeVec = normalize(eye_position_cameraspace - vertex_position_cameraspace);
        out.reflection = props[0].data[2];
        out.shadowDarkness = shadowDarkness;
    } else {
        out.isLighting = 0.0;
        out.shadowDarkness = 0.0;
    }
    
    out.tangent = normalize(float3(world[0].data * float4(vertex_array[vid].tangent, 0.0)));
    out.bitangent = normalize(float3(world[0].data * float4(vertex_array[vid].bitagent, 0.0)));
    
    return out;
}

fragment half4 Per_Vertex_Color_Shadow_Fragment(ColorInOut in [[stage_in]],depth2d<float> shadowMap [[texture(SHADER_PERVERTEXCOLOR_texture1)]],
                                                constant packed_float3* lightPos [[ buffer(SHADER_PERVERTEXCOLOR_lightPos) ]],
                                                constant packed_float3* lightColor [[ buffer(SHADER_PERVERTEXCOLOR_lightColor) ]],
                                                constant float* lightFadeDistance[[ buffer(SHADER_COMMON_lightFadeDistance)]],
                                                constant float& numberOfLights[[ buffer(SHADER_COMMON_isVertexColored)]]) {
    // Shadow Calculation----------
    float shadowBias = 0.005,shadowValue = 0.0,maxSpecular = 30.0;
    if(in.shadowDarkness > 0.0){
        constexpr sampler linear_sampler(min_filter::linear, mag_filter::linear);
        float depth = shadowMap.sample(linear_sampler,in.texture2UV);
        if((depth + shadowBias) < in.vertexDepth)
            shadowValue = in.shadowDarkness;
    }
    //------------------
    
    
    // Lighting Calculation----------
    float4 diffuse_color = in.perVertexColor,specular,colorOfLight = float4(1.0);
    if(in.isLighting > 0.5){
        colorOfLight = float4(0.0);
        
        int lightsCount = int(numberOfLights);
        for(int i = 0; i < lightsCount; i++) {
            
            float4 light_position_cameraspace = float4(float3(lightPos[i]),1.0);
            float4 lightDir = normalize(light_position_cameraspace - in.vertexPosCam);
            float distanceFromLight = distance(light_position_cameraspace , in.vertexPosCam);
            
            float4 normal = normalize(in.normal);
            float4 eyeVec = normalize(in.eyeVec);
            float n_dot_l = saturate(dot(normal,lightDir));
            float4 diffuse = float4(float3(n_dot_l),1.0);
            
            float4 reflectValue = -lightDir + 2.0f * n_dot_l * normal;
            float e_dot_r =  saturate(dot(eyeVec,reflectValue));
            specular = float4(in.reflection * pow(e_dot_r,maxSpecular));
            
            float e_dot_l = dot(lightDir,eyeVec);
            if(e_dot_l < -0.8)
                specular = float4(0.0);
            colorOfLight += float4(float3(lightColor[i]),1.0) * (1.0 - saturate(distanceFromLight/lightFadeDistance[i])) * diffuse;
        }
        
    }else{
        diffuse_color = in.perVertexColor;
        specular = float4(0.0);
    }
    //-------------
    
    half4 finalColor = half4(diffuse_color + specular) * half4(colorOfLight);
    finalColor = finalColor + (half4(0.0,0.0,0.0,0.0) - finalColor) * (shadowValue);
    return half4(half3(finalColor.xyz) , in.transparency);
}

fragment half4 Per_Vertex_Color_Toon_Fragment(ColorInOut in [[stage_in]],depth2d<float> shadowMap [[texture(SHADER_PERVERTEXCOLOR_texture1)]],
                                              constant packed_float3* lightPos [[ buffer(SHADER_PERVERTEXCOLOR_lightPos) ]],
                                              constant packed_float3* lightColor [[ buffer(SHADER_PERVERTEXCOLOR_lightColor) ]]) {
    // Shadow Calculation----------
    float shadowBias = 0.005,shadowValue = 0.0,maxSpecular = 30.0;
    if(in.shadowDarkness > 0.0){
        constexpr sampler linear_sampler(min_filter::linear, mag_filter::linear);
        float depth = shadowMap.sample(linear_sampler,in.texture2UV);
        if((depth + shadowBias) < in.vertexDepth)
            shadowValue = in.shadowDarkness;
    }
    //------------------
    
    // Lighting Calculation----------
    float4 diffuse_color,specular;
    half4 finalColor;
    if(in.isLighting > 0.5){
        
        float4 light_position_cameraspace = float4(float3(lightPos[0]),1.0);
        float4 lightDir = normalize(light_position_cameraspace - in.vertexPosCam);
        
        float4 normal = normalize(in.normal);
        float4 eyeVec = normalize(in.eyeVec);
        
        float n_dot_l = saturate(dot(normal,lightDir));
        float4 diffuse = float4(float3(n_dot_l),1.0);
        diffuse_color = diffuse * in.perVertexColor;
        
        float4 reflectValue = -lightDir + 2.0f * n_dot_l * normal;
        float e_dot_r =  saturate(dot(eyeVec,reflectValue));
        specular = float4(in.reflection * pow(e_dot_r,maxSpecular));
        float e_dot_l = dot(lightDir,eyeVec);
        if(e_dot_l < -0.8)
            specular = float4(0.0);
        
        //Toon Calculation----------
        half4 toonColor = half4(diffuse_color + specular);
        if(n_dot_l > 0.95)
            toonColor = half4(1.0,1.0,1.0,1.0) * toonColor;
        else if(n_dot_l > 0.6)
            toonColor = half4(0.8,0.8,0.8,1.0) * toonColor;
        else if(n_dot_l > 0.2)
            toonColor = half4(0.6,0.6,0.6,1.0) * toonColor;
        else
            toonColor = half4(0.4,0.4,0.4,1.0) * toonColor;
        //----------------
        finalColor = toonColor * half4(half3(lightColor[0]),1.0);
    }else{
        diffuse_color = in.perVertexColor;
        specular = float4(0.0);
        finalColor = half4(diffuse_color);
    }
    //-------------
    finalColor = finalColor + (half4(0.0,0.0,0.0,0.0) - finalColor) * (shadowValue);
    return half4(half3(finalColor.xyz) , in.transparency);
}

fragment half4 Per_Vertex_Color_Skin_Fragment(ColorInOut in [[stage_in]],
                                              constant packed_float3* lightPos [[ buffer(SHADER_PERVERTEXCOLOR_lightPos) ]],
                                              constant packed_float3* lightColor [[ buffer(SHADER_PERVERTEXCOLOR_lightColor) ]],
                                              constant float* lightFadeDistance[[ buffer(SHADER_COMMON_lightFadeDistance)]],
                                              constant float& numberOfLights[[ buffer(SHADER_COMMON_isVertexColored)]]) {
    //------------------
    float maxSpecular = 30.0;
    
    // Lighting Calculation----------
    float4 diffuse_color = in.perVertexColor,specular , colorOfLight = float4(1.0);
    
    if(in.isLighting > 0.5){
        colorOfLight = float4(0.0);
        
        int lightsCount = int(numberOfLights);
        for(int i = 0; i < lightsCount; i++) {
            
            float4 light_position_cameraspace = float4(float3(lightPos[i]),1.0);
            float4 lightDir = normalize(light_position_cameraspace - in.vertexPosCam);
            float distanceFromLight = distance(light_position_cameraspace , in.vertexPosCam);
            
            float4 normal = normalize(in.normal);
            float4 eyeVec = normalize(in.eyeVec);
            
            float n_dot_l = saturate(dot(normal,lightDir));
            float4 diffuse = float4(float3(n_dot_l),1.0);
            
            float4 reflectValue = -lightDir + 2.0f * n_dot_l * normal;
            float e_dot_r =  saturate(dot(eyeVec,reflectValue));
            specular = float4(in.reflection * pow(e_dot_r,maxSpecular));
            float e_dot_l = dot(lightDir,eyeVec);
            if(e_dot_l < -0.8)
                specular = float4(0.0);
            colorOfLight += float4(float3(lightColor[i]),1.0) * (1.0 - saturate(distanceFromLight/lightFadeDistance[i])) * diffuse;
        }
    }else{
        diffuse_color = in.perVertexColor;
        specular = float4(0.0);
    }
    //-------------
    
    half4 finalColor = half4(diffuse_color + specular) * half4(colorOfLight);
    finalColor = finalColor;
    return half4(half3(finalColor.xyz) , in.transparency);
}

fragment half4 Per_Vertex_Color_Fragment(ColorInOut in [[stage_in]],
                                         constant packed_float3* lightPos [[ buffer(SHADER_PERVERTEXCOLOR_lightPos) ]],
                                         constant packed_float3* lightColor [[ buffer(SHADER_PERVERTEXCOLOR_lightColor) ]]) {
    half4 finalColor = half4(in.perVertexColor);
    return half4(half3(finalColor.xyz) , in.transparency);
}

#define SHADER_DRAW_2D_IMAGE_texture1 0
vertex ColorInOut Draw2DImage_Vertex(device vertex_t* vertex_array [[ buffer(0) ]],
                                     unsigned int vid [[ vertex_id ]]
                                     ) {
    float2 uv = vertex_array[vid].texCoord1;
    ColorInOut out;
    out.position = float4(float3(vertex_array[vid].position), 1.0);
    out.uv.x = uv.x;
    out.uv.y = uv.y;
    out.perVertexColor = float4(1.0);
    return out;
}

fragment half4 Draw2DImage_Fragment(ColorInOut in [[stage_in]],texture2d<half>  tex2D [[texture(0)]]) {
    constexpr sampler quad_sampler(address::repeat,filter::linear);
    half4 texColor = tex2D.sample(quad_sampler,in.uv);
    return texColor;
}

fragment half4 Draw2DImage_Fragment_Depth(ColorInOut in [[stage_in]],depth2d<float> shadowMap [[texture(0)]]) {
    constexpr sampler linear_sampler(min_filter::linear, mag_filter::linear);
    float d = shadowMap.sample(linear_sampler, in.uv.xy);
    half4 texColor = half4(d, d, d,1.0);
    return texColor;
}

#define SHADER_DEPTH_PASS_mvp 1
vertex ColorInOut Depth_Pass_vert(device vertex_t* vertex_array [[ buffer(0) ]],
                                  constant float4x4& mvp [[buffer(SHADER_DEPTH_PASS_mvp)]],
                                  unsigned int vid [[ vertex_id ]]
                                  ) {
    ColorInOut out;
    out.position = (mvp) * float4(float3(vertex_array[vid].position), 1.0);
    return out;
}

#define SHADER_DEPTH_PASS_SKIN_mvp 1
#define SHADER_DEPTH_PASS_SKIN_jointdata 2
vertex ColorInOut Depth_Pass_Skin_vert(device vertex_heavy_t* vertex_array [[ buffer(0) ]],
                                       constant float4x4& mvp [[buffer(SHADER_DEPTH_PASS_SKIN_mvp)]],
                                       constant JointData* Joint_Data [[ buffer(SHADER_DEPTH_PASS_SKIN_jointdata)]],
                                       unsigned int vid [[ vertex_id ]]
                                       ) {
    ColorInOut out;
    float4 in_position = float4(float3(vertex_array[vid].position), 1.0);
    float4 optionalData1 = vertex_array[vid].optionalData1;
    float4 optionalData2 = vertex_array[vid].optionalData2;
    float4 optionalData3 = vertex_array[vid].optionalData3;
    float4 optionalData4 = vertex_array[vid].optionalData4;
    float4 pos = float4(0.0);
    
    int jointId = int(optionalData1.x);
    float strength = optionalData2.x ;
    if(jointId > 0)
        pos = pos + (Joint_Data[jointId - 1].JointTransform * in_position) * strength;
    else
        pos = in_position;
    
    jointId = int(optionalData1.y);
    strength = optionalData2.y ;
    if(jointId > 0)
        pos = pos + (Joint_Data[jointId - 1].JointTransform * in_position) * strength;
    
    jointId = int(optionalData1.z);
    strength = optionalData2.z ;
    if(jointId > 0)
        pos = pos + (Joint_Data[jointId - 1].JointTransform * in_position) * strength;
    
    jointId = int(optionalData1.w);
    strength = optionalData2.w;
    if(jointId > 0)
        pos = pos + (Joint_Data[jointId - 1].JointTransform * in_position) * strength;
    
    jointId = int(optionalData3.x);
    strength = optionalData4.x;
    if(jointId > 0)
        pos = pos + (Joint_Data[jointId - 1].JointTransform * in_position) * strength;
    
    jointId = int(optionalData3.y);
    strength = optionalData4.y;
    if(jointId > 0)
        pos = pos + (Joint_Data[jointId - 1].JointTransform * in_position) * strength;
    
    jointId = int(optionalData3.z);
    strength = optionalData4.z;
    if(jointId > 0)
        pos = pos + (Joint_Data[jointId - 1].JointTransform * in_position) * strength;
    
    jointId = int(optionalData3.w);
    strength = optionalData4.w;
    if(jointId > 0)
        pos = pos + (Joint_Data[jointId - 1].JointTransform * in_position) * strength;
    
    
    out.position = mvp * pos;
    return out;
}

vertex ColorInOut Depth_Pass_Text_vert(device vertex_heavy_t* vertex_array [[ buffer(0) ]],
                                       constant float4x4& mvp [[buffer(SHADER_DEPTH_PASS_SKIN_mvp)]],
                                       constant JointData* Joint_Data [[ buffer(SHADER_DEPTH_PASS_SKIN_jointdata)]],
                                       unsigned int vid [[ vertex_id ]]
                                       ) {
    ColorInOut out;
    float4 in_position = float4(float3(vertex_array[vid].position), 1.0);
    float4 optionalData1 = vertex_array[vid].optionalData1;
    float4 optionalData2 = vertex_array[vid].optionalData2;
    float4 pos = float4(0.0);
    
    int jointId = int(optionalData1.x);
    float strength = optionalData2.x ;
    
    pos = (Joint_Data[jointId - 1].JointTransform * in_position) * strength;
    
    out.position = mvp * pos;
    return out;
}

