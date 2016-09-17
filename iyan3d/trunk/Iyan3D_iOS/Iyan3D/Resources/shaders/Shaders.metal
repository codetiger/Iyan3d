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
    packed_float4 vertColor;
} vertex_t;

typedef struct
{
    packed_float3 position;
    packed_float3 normal;
    packed_float2 texCoord1;
    packed_float3 tangent;
    packed_float3 bitagent;
    packed_float4 vertColor;
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
    float hasLighting;
    float pointSize [[point_size]];
    float vertexDepth, reflection;
    float hasMeshColor;
    float shadowDarkness, transparency;
    float2 pointCoord [[ point_coord ]];
    float2 uv,texture2UV;
    float4 position [[position]] , vertexPosition_ws;
    float4 eyeVec, lightDir, lightColor;
    float3 meshColor;
    float3 T;
    float3 B;
    float3 N;
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

float4x4 Identity()
{
    simd::float4 X = { 1, 0, 0, 0 };
    simd::float4 Y = { 0, 1, 0, 0 };
    simd::float4 Z = { 0, 0, 1, 0 };
    simd::float4 W = { 0, 0, 0, 1 };
    simd::float4x4 mat = { X, Y, Z, W };
    return mat;
}

// CommonSkinVertex
#define SHADER_COMMON_SKIN_mvp 12
#define SHADER_COMMON_SKIN_jointData 13
#define SHADER_COMMON_SKIN_model 14
#define SHADER_COMMON_SKIN_shadowDarkness 15
#define SHADER_COMMON_SKIN_eyePos 16
#define SHADER_COMMON_SKIN_lightViewProjMatrix 17
#define SHADER_COMMON_SKIN_hasMeshColor 18
#define SHADER_COMMON_SKIN_reflectionValue 19
#define SHADER_COMMON_SKIN_transparency 20
#define SHADER_COMMON_SKIN_hasLighting 21
#define SHADER_COMMON_SKIN_uvScale 22
#define SHADER_COMMON_SKIN_meshColor 23

vertex ColorInOut Skin_Vertex(device vertex_heavy_t* vertex_array [[ buffer(0) ]],
                                     constant float4x4& mvp [[ buffer(SHADER_COMMON_SKIN_mvp) ]],
                                     constant JointData* Joint_Data [[ buffer(SHADER_COMMON_SKIN_jointData) ]],
                                     constant MatArray* model [[ buffer(SHADER_COMMON_SKIN_model) ]],
                                     constant float& shadowDarkness [[ buffer(SHADER_COMMON_SKIN_shadowDarkness) ]],
                                     constant packed_float3& eyePos [[ buffer(SHADER_COMMON_SKIN_eyePos) ]],
                                     constant float4x4& lvp [[ buffer(SHADER_COMMON_SKIN_lightViewProjMatrix) ]],
                                     unsigned int vid [[ vertex_id ]],
                                     constant float *hasMeshColor [[ buffer(SHADER_COMMON_SKIN_hasMeshColor) ]],
                                     constant float *reflectionValue [[ buffer(SHADER_COMMON_SKIN_reflectionValue) ]],
                                     constant float *transparencyValue [[ buffer(SHADER_COMMON_SKIN_transparency) ]],
                                     constant float *hasLighting [[ buffer(SHADER_COMMON_SKIN_hasLighting) ]],
                                     constant float *uvScaleValue [[ buffer(SHADER_COMMON_SKIN_uvScale) ]],
                                     constant float3Struct *meshColor [[buffer(SHADER_COMMON_SKIN_meshColor)]]
                                     )
{
    ColorInOut out;
    float4 in_position = float4(float3(vertex_array[vid].position), 1.0);
    float4 in_normal = float4(float3(vertex_array[vid].normal), 0.0);
    float4 in_vertColor = float4(vertex_array[vid].vertColor);
    float4 optionalData1 = vertex_array[vid].optionalData1;
    float4 optionalData2 = vertex_array[vid].optionalData2;
    float4 optionalData3 = vertex_array[vid].optionalData3;
    float4 optionalData4 = vertex_array[vid].optionalData4;
    float3 tangent = vertex_array[vid].tangent;
    float3 bitangent = vertex_array[vid].bitagent;
    
    out.hasMeshColor = hasMeshColor[0];
    out.meshColor = (meshColor[0].data[0] == -1.0) ? in_vertColor.xyz : float3(meshColor[0].data);
    out.reflection = reflectionValue[0];
    out.transparency = transparencyValue[0];
    out.hasLighting = hasLighting[0];

    float4x4 finalMatrix = Identity();
    
    int jointId = int(optionalData1.x);
    if(jointId > 0)
        finalMatrix = Joint_Data[jointId - 1].JointTransform * optionalData2.x;
    
    jointId = int(optionalData1.y);
    if(jointId > 0)
        finalMatrix = finalMatrix + (Joint_Data[jointId - 1].JointTransform * optionalData2.y);
    
    jointId = int(optionalData1.z);
    if(jointId > 0)
        finalMatrix = finalMatrix + (Joint_Data[jointId - 1].JointTransform * optionalData2.z);
    
    jointId = int(optionalData1.w);
    if(jointId > 0)
        finalMatrix = finalMatrix + (Joint_Data[jointId - 1].JointTransform * optionalData2.w);
    
    jointId = int(optionalData3.x);
    if(jointId > 0)
        finalMatrix = finalMatrix + (Joint_Data[jointId - 1].JointTransform * optionalData4.x);
    
    jointId = int(optionalData3.y);
    if(jointId > 0)
        finalMatrix = finalMatrix + (Joint_Data[jointId - 1].JointTransform * optionalData4.y);
    
    jointId = int(optionalData3.z);
    if(jointId > 0)
        finalMatrix = finalMatrix + (Joint_Data[jointId - 1].JointTransform * optionalData4.z);
    
    jointId = int(optionalData3.w);
    if(jointId > 0)
        finalMatrix = finalMatrix + (Joint_Data[jointId - 1].JointTransform * optionalData4.w);
    
    out.position = mvp * finalMatrix * in_position;

    finalMatrix = model[0].data * finalMatrix;
    float4 pos = finalMatrix * in_position;
    float4 nor = finalMatrix * in_normal;

    nor = normalize(nor);
    
    out.vertexPosition_ws = pos;
    
    float2 uv = vertex_array[vid].texCoord1;
    out.uv.x = uv.x * uvScaleValue[0];
    out.uv.y = uv.y * uvScaleValue[0];

    out.T = normalize(float3(finalMatrix * float4(tangent, 0.0)));
    out.B = normalize(float3(finalMatrix * float4(bitangent, 0.0)));
    out.N = normalize(float3(nor));
    
    float4 vertexLightCoord = lvp * pos;
    float4 texCoords = vertexLightCoord / vertexLightCoord.w;
    out.texture2UV = float4((texCoords / 2.0) + 0.5).xy;
    out.texture2UV.y = (1.0 - out.texture2UV.y); // need to flip metal texture vertically
    out.vertexDepth = texCoords.z;
    
    float4 eye_position_cameraspace = float4(float3(eyePos),1.0);
    out.eyeVec = normalize(eye_position_cameraspace - pos);
    
    if(int(hasLighting[0]) == 1){
        out.shadowDarkness = shadowDarkness;
    }else{
        out.shadowDarkness = 0.0;
    }
    
    return out;
}

vertex ColorInOut Text_Skin_Vertex(device vertex_heavy_t* vertex_array [[ buffer(0) ]],
                              constant float4x4& mvp [[ buffer(SHADER_COMMON_SKIN_mvp) ]],
                              constant JointData* Joint_Data [[ buffer(SHADER_COMMON_SKIN_jointData) ]],
                              constant MatArray* model [[ buffer(SHADER_COMMON_SKIN_model) ]],
                              constant float& shadowDarkness [[ buffer(SHADER_COMMON_SKIN_shadowDarkness) ]],
                              constant packed_float3& eyePos [[ buffer(SHADER_COMMON_SKIN_eyePos) ]],
                              constant float4x4& lvp [[ buffer(SHADER_COMMON_SKIN_lightViewProjMatrix) ]],
                              unsigned int vid [[ vertex_id ]],
                              constant float *hasMeshColor [[ buffer(SHADER_COMMON_SKIN_hasMeshColor) ]],
                              constant float *reflectionValue [[ buffer(SHADER_COMMON_SKIN_reflectionValue) ]],
                              constant float *transparencyValue [[ buffer(SHADER_COMMON_SKIN_transparency) ]],
                              constant float *hasLighting [[ buffer(SHADER_COMMON_SKIN_hasLighting) ]],
                              constant float *uvScaleValue [[ buffer(SHADER_COMMON_SKIN_uvScale) ]],
                              constant float3Struct *meshColor [[buffer(SHADER_COMMON_SKIN_meshColor)]]
                              )
{
    ColorInOut out;
    float4 in_position = float4(float3(vertex_array[vid].position), 1.0);
    float4 in_normal = float4(float3(vertex_array[vid].normal), 0.0);
    float4 in_vertColor = float4(vertex_array[vid].vertColor);
    float4 optionalData1 = vertex_array[vid].optionalData1;
    float4 optionalData2 = vertex_array[vid].optionalData2;
    float3 tangent = vertex_array[vid].tangent;
    float3 bitangent = vertex_array[vid].bitagent;
    
    out.hasMeshColor = hasMeshColor[0];
    out.meshColor = (meshColor[0].data[0] == -1.0) ? in_vertColor.xyz : float3(meshColor[0].data);
    out.reflection = reflectionValue[0];
    out.transparency = transparencyValue[0];
    out.hasLighting = hasLighting[0];
    
    float4x4 finalMatrix = Identity();

    int jointId = int(optionalData1.x);
    float strength = optionalData2.x ;
    if(jointId > 0)
        finalMatrix = Joint_Data[jointId - 1].JointTransform * strength;

    float4 pos = finalMatrix * in_position;
    float4 nor = finalMatrix * in_normal;

    nor = normalize(nor);
    
    float4 vertexPosition_ws = model[0].data * pos;
    out.vertexPosition_ws = vertexPosition_ws;
    
    out.position = mvp * pos;
    float2 uv = vertex_array[vid].texCoord1;
    out.uv = uv * uvScaleValue[0];
    
    out.T = normalize(float3(finalMatrix * float4(tangent, 0.0)));
    out.B = normalize(float3(finalMatrix * float4(bitangent, 0.0)));
    out.N = normalize(float3(nor));
    
    float4 vertexLightCoord = lvp * vertexPosition_ws;
    float4 texCoords = vertexLightCoord / vertexLightCoord.w;
    out.texture2UV = float4((texCoords / 2.0) + 0.5).xy;
    out.texture2UV.y = (1.0 - out.texture2UV.y); // need to flip metal texture vertically
    out.vertexDepth = texCoords.z;
    
    float4 eye_position_cameraspace = float4(float3(eyePos),1.0);
    out.eyeVec = normalize(eye_position_cameraspace - vertexPosition_ws);
    
    if(int(hasLighting[0]) == 1){
        out.shadowDarkness = shadowDarkness;
    }else{
        out.shadowDarkness = 0.0;
    }
    
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
#define SHADER_PARTICLE_hasMeshColor 10

vertex ColorInOut Particle_Vertex(device vertex_t* vertex_array [[ buffer(0) ]],
                                  constant float4x4& vp [[ buffer(SHADER_PARTICLE_vp) ]],
                                  constant float4x4& model [[ buffer(SHADER_PARTICLE_world)]],
                                  constant packed_float4& sColor [[ buffer(SHADER_PARTICLE_sColor) ]],
                                  constant packed_float4& mColor [[ buffer(SHADER_PARTICLE_mColor) ]],
                                  constant packed_float4& eColor [[ buffer(SHADER_PARTICLE_eColor) ]],
                                  constant packed_float4& props [[ buffer(SHADER_PARTICLE_props) ]],
                                  constant float* hasMeshColor[[ buffer(SHADER_PARTICLE_hasMeshColor)]],
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
    
    out.meshColor = (int(hasMeshColor[0]) == 0) ? float3(mix(s, e, age).xyz) : float3(float4(sColor).xyz);
    
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
                                      constant float* hasMeshColor[[ buffer(SHADER_PARTICLE_hasMeshColor)]],
                                      unsigned int vid [[ vertex_id ]]
                                      )
{
    float4 in_position = float4(float3(vertex_array[vid].position), 1.0);
    float4 in_normal = float4(float3(vertex_array[vid].normal), 0.0);
    
    ColorInOut out;
    
    out.meshColor = float4(sColor).xyz;
    
    float live = float(in_normal.x > 0.0 && in_normal.x <= props[0]);
    
    float3 temp = float3(props[1] - in_position[0], props[2] - in_position[1], props[3] - in_position[2]);
    float dist = sqrt(temp.x * temp.x + temp.y * temp.y + temp.z * temp.z);
    
    out.position = vp * in_position;
    out.pointSize = (8.0 * live) * (100.0/dist);
    
    return out;
}

fragment float4 Particle_Fragment(ColorInOut in [[stage_in]], texture2d<half>  tex2D [[texture(SHADER_PARTICLE_texture1)]])
{
    float4 color = float4(in.meshColor, 1.0);
    constexpr sampler quad_sampler(address::repeat,filter::linear);
    color[3] =  tex2D.sample(quad_sampler, in.pointCoord)[0];
    return color;
}

fragment float4 Particle_Fragment_RTT(ColorInOut in [[stage_in]], texture2d<half>  tex2D [[texture(SHADER_PARTICLE_texture1)]])
{
    float4 color = float4(in.meshColor, 1.0);
    return color;
}

#define SHADER_MESH_mvp 1
#define SHADER_MESH_model 2
#define SHADER_MESH_lightViewProjMatrix 3
#define SHADER_MESH_eyePos 4
#define SHADER_MESH_shadowDarkness 5
#define SHADER_MESH_hasMeshColor 6
#define SHADER_MESH_meshColor 7
#define SHADER_MESH_reflectionValue 8
#define SHADER_MESH_transparency 9
#define SHADER_MESH_hasLighting 10
#define SHADER_MESH_uvScale 11

vertex ColorInOut Mesh_Vertex(device vertex_t* vertex_array [[ buffer(0) ]],
                                constant float4x4& vp [[ buffer(SHADER_MESH_mvp) ]],
                                constant MatArray* model [[ buffer(SHADER_MESH_model) ]],
                                constant float4x4& lvp [[ buffer(SHADER_MESH_lightViewProjMatrix) ]],
                                constant packed_float3& eyePos [[ buffer(SHADER_MESH_eyePos) ]],
                                constant float& shadowDarkness [[ buffer(SHADER_MESH_shadowDarkness) ]],
                                constant float *hasMeshColor [[ buffer(SHADER_MESH_hasMeshColor) ]],
                                constant float *reflectionValue [[ buffer(SHADER_MESH_reflectionValue) ]],
                                constant float *transparency [[ buffer(SHADER_MESH_transparency) ]],
                                constant float *hasLighting [[ buffer(SHADER_MESH_hasLighting) ]],
                                constant float *uvScaleValue [[ buffer(SHADER_MESH_uvScale) ]],
                                constant float3Struct *meshColor [[buffer(SHADER_MESH_meshColor)]],
                                unsigned int vid [[ vertex_id ]],
                                unsigned int iId [[ instance_id ]]
                                )
{
    
    float4 vertexPosition_ws = model[iId].data * float4(float3(vertex_array[vid].position), 1.0);
    float4 vertColor = float4(vertex_array[vid].vertColor);
    float3 tangent = float3(vertex_array[vid].tangent);
    float3 bitangent = float3(vertex_array[vid].bitagent);
    float3 normal = float3(vertex_array[vid].normal);
    
    ColorInOut out;
    out.hasMeshColor = hasMeshColor[iId];
    out.meshColor = (meshColor[iId].data[0] == -1.0) ? vertColor.xyz : float3(meshColor[iId].data);
    out.reflection = reflectionValue[iId];
    
    out.T = normalize(float3(model[iId].data * float4(tangent, 0.0)));
    out.B = normalize(float3(model[iId].data * float4(bitangent, 0.0)));
    out.N = normalize(float3(model[iId].data * float4(normal,  0.0)));

    out.vertexPosition_ws = vertexPosition_ws;
    out.transparency = transparency[iId];
    out.hasLighting = hasLighting[iId];
    out.position = vp * vertexPosition_ws;
    
    float2 uv = vertex_array[vid].texCoord1;
    out.uv.x = uv.x * uvScaleValue[iId];
    out.uv.y = uv.y * uvScaleValue[iId];
    
    float4 vertexLightCoord = lvp * vertexPosition_ws;
    float4 texCoords = vertexLightCoord/vertexLightCoord.w;
    out.texture2UV = float4((texCoords / 2.0) + 0.5).xy;
    out.texture2UV.y = (1.0 - out.texture2UV.y); // need to flip metal texture vertically
    out.vertexDepth = texCoords.z;
    
    float4 eye_position_ws = vertexPosition_ws - float4(float3(eyePos), 1.0);
    out.eyeVec = eye_position_ws;

    if(int(hasLighting[iId]) == 1)
        out.shadowDarkness = shadowDarkness;
    else
        out.shadowDarkness = 0.0;
    
    return out;
}

#define SHADER_COMMON_colorMap 0
#define SHADER_COMMON_normalMap 1
#define SHADER_COMMON_shadowMap 2
#define SHADER_COMMON_reflectionMap 3
#define SHADER_COMMON_lightPos 4
#define SHADER_COMMON_lightColor 5
#define SHADER_COMMON_lightFadeDistance 6
#define SHADER_COMMON_lightType 7
#define SHADER_COMMON_lightCount 8
#define SHADER_COMMON_hasReflectionMap 9
#define SHADER_COMMON_hasNormalMap 10
#define SHADER_COMMON_ambientLight 11
#define SHADER_COMMON_samplerType 12

// Fragment shader function

fragment half4 Common_Fragment(ColorInOut in [[stage_in]],
                                  texture2d<half>  colorMap [[texture(SHADER_COMMON_colorMap)]],
                                  depth2d<float> shadowMap [[texture(SHADER_COMMON_shadowMap)]],
                                  texture2d<half>  normalMap [[texture(SHADER_COMMON_normalMap)]],
                                  texture2d<half>  reflectionMap [[texture(SHADER_COMMON_reflectionMap)]],
                                  constant packed_float3* lightPos [[ buffer(SHADER_COMMON_lightPos) ]],
                                  constant packed_float3* lightColor [[ buffer(SHADER_COMMON_lightColor) ]],
                                  constant float* lightFadeDistance[[ buffer(SHADER_COMMON_lightFadeDistance)]],
                                  constant float* lightType [[ buffer(SHADER_COMMON_lightType) ]],
                                  constant float& lightCount [[ buffer(SHADER_COMMON_lightCount) ]],
                                  constant float& hasReflectionMap [[ buffer(SHADER_COMMON_hasReflectionMap) ]],
                                  constant float& hasNormalMap [[ buffer(SHADER_COMMON_hasNormalMap) ]],
                                  constant float& ambientLight [[ buffer(SHADER_COMMON_ambientLight) ]],
                                  constant float& samplerType [[ buffer(SHADER_COMMON_samplerType) ]])
{
    float shadowBias = 0.005, shadowValue = 0.0;
    if(in.shadowDarkness > 0.0) {
        constexpr sampler linear_sampler(min_filter::linear, mag_filter::linear);
        float depth = shadowMap.sample(linear_sampler,in.texture2UV);
        if((depth + shadowBias) < in.vertexDepth)
            shadowValue = in.shadowDarkness;
    }
    
    constexpr sampler quad_sampler(address::repeat, filter::linear);
    
    half4 diffuse_color = half4(half3(in.meshColor), 1.0);
    half texTransparency = 1.0;
    if(in.hasMeshColor < 0.5) {
        if(samplerType == 0.0)
            diffuse_color = colorMap.sample(quad_sampler, in.uv);
        
        if(samplerType == 1.0) {
            constexpr sampler nearest_sampler(min_filter::nearest, mag_filter::nearest);
            diffuse_color = colorMap.sample(nearest_sampler, in.uv);
        }
        texTransparency = diffuse_color.w;
    }
    
    if(texTransparency <= 0.01)
        discard_fragment();
    
    float3x3 TBNMatrix = float3x3(in.T, in.B, in.N);
    
    float4 normal = normalize(float4(TBNMatrix[2], 0.0));
    if(hasNormalMap > 0.5) {
        half4 n =  normalMap.sample(quad_sampler, in.uv);
        normal = float4(normalize(TBNMatrix * float3(n.xyz)), 0.0);
    }

    half4 specular = half4(0.0), colorOfLight = half4(1.0);
    
    if(in.hasLighting > 0.5) {
        
        if(hasReflectionMap > 0.5) {
            float4 r = reflect(in.eyeVec, normal);
            float m = 2.0 * sqrt(r.x * r.x + r.y * r.y + (r.z + 1.0) * (r.z + 1.0));
            float2 vN = (r.xy / m) + 0.5;
            vN.y = -vN.y;
            specular = reflectionMap.sample(quad_sampler, vN);
        } else {
            float4 light_position_ws = float4(lightPos[0], 1.0);
            float4 lightDir = (lightType[0] == 1.0) ? light_position_ws : normalize(light_position_ws - in.vertexPosition_ws);
            float n_dot_l = saturate(dot(normal, lightDir));
            
            float4 reflectValue = -lightDir + 2.0f * n_dot_l * normal;
            float e_dot_r = saturate(dot(in.eyeVec, reflectValue));
            
            float maxSpecular = 30.0;
            specular = half4(in.reflection * pow(e_dot_r, maxSpecular));
        }
        
        colorOfLight = half4(0.0);
        for (int i = 0; i < lightCount; i++) {
            
            float4 light_position_ws = float4(lightPos[i], 1.0);
            float4 lightDir = (lightType[i] == 1.0) ? light_position_ws : normalize(light_position_ws - in.vertexPosition_ws);
            float distanceFromLight = distance(light_position_ws, in.vertexPosition_ws);
            
            float distanceRatio = (1.0 - saturate(distanceFromLight / lightFadeDistance[i]));
            distanceRatio = mix(1.0, distanceRatio, lightType[i]);
            
            float n_dot_l = saturate(dot(normal, lightDir));
            
            float darkness = distanceRatio * n_dot_l;
            darkness = clamp(darkness, ambientLight, 1.0);
            
            colorOfLight += half4(half3(lightColor[i]) * darkness, 1.0);
        }
    }

    half4 finalColor = half4(diffuse_color + specular) * colorOfLight;
    
    if(hasReflectionMap > 0.5)
        finalColor = half4(diffuse_color * (1.0 - in.reflection) + specular * in.reflection) * colorOfLight; //mix(diffuse_color, specular, in.reflection) * colorOfLight;
    
    finalColor -= (finalColor * shadowValue);

    return half4(half3(finalColor.xyz), texTransparency * in.transparency);
}

#define SHADER_COLOR_mvp 1
#define SHADER_COLOR_vertexColor 2
#define SHADER_COLOR_transparency 3

vertex ColorInOut Color_Vertex(device vertex_t* vertex_array [[ buffer(0) ]],
                               constant float4x4& mvp [[buffer(SHADER_COLOR_mvp)]],
                               constant float3Struct *meshColor [[buffer(SHADER_COLOR_vertexColor)]],
                               constant float* transparency [[ buffer(SHADER_COLOR_transparency) ]],
                               unsigned int vid [[ vertex_id ]]
                               )
{
    ColorInOut out;
    out.position = (mvp) * float4(float3(vertex_array[vid].position), 1.0);
    out.meshColor = float3(meshColor[0].data);
    out.transparency = transparency[0];
    return out;
}

fragment half4 Color_Fragment(ColorInOut in [[stage_in]])
{
    half4 color = half4(half3(in.meshColor), in.transparency);
    return color;
}


#define SHADER_COLOR_SKIN_mvp 1
#define SHADER_COLOR_SKIN_vertexColor 2
#define SHADER_COLOR_SKIN_jointData 3
#define SHADER_COLOR_SKIN_transparency 4

vertex ColorInOut Color_Skin_Vertex(device vertex_heavy_t* vertex_array [[ buffer(0) ]],
                                    constant float4x4& mvp [[ buffer(SHADER_COLOR_SKIN_mvp) ]],
                                    constant float3Struct *meshColor [[buffer(SHADER_COLOR_SKIN_vertexColor)]],
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
    out.meshColor = float3(meshColor[0].data);
    out.transparency = transparency[0];
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


#define SHADER_DRAW_2D_IMAGE_texture1 0
vertex ColorInOut Draw2DImage_Vertex(device vertex_t* vertex_array [[ buffer(0) ]],
                                     unsigned int vid [[ vertex_id ]]
                                     )
{
    float2 uv = vertex_array[vid].texCoord1;
    ColorInOut out;
    out.position = float4(float3(vertex_array[vid].position), 1.0);
    out.uv.x = uv.x;
    out.uv.y = uv.y;
    out.meshColor = float3(1.0);
    out.transparency = 1.0;
    return out;
}

fragment half4 Draw2DImage_Fragment(ColorInOut in [[stage_in]],texture2d<half>  tex2D [[texture(0)]])
{
    constexpr sampler quad_sampler(address::repeat,filter::linear);
    half4 texColor = tex2D.sample(quad_sampler,in.uv);
    return texColor;
}

fragment half4 Draw2DImage_Fragment_Depth(ColorInOut in [[stage_in]],depth2d<float> shadowMap [[texture(0)]])
{
    constexpr sampler linear_sampler(min_filter::linear, mag_filter::linear);
    float d = shadowMap.sample(linear_sampler, in.uv.xy);
    half4 texColor = half4(d, d, d,1.0);
    return texColor;
}

#define SHADER_DEPTH_PASS_mvp 1
vertex ColorInOut Depth_Pass_vert(device vertex_t* vertex_array [[ buffer(0) ]],
                                  constant float4x4& mvp [[buffer(SHADER_DEPTH_PASS_mvp)]],
                                  unsigned int vid [[ vertex_id ]]
                                  )
{
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
    return out;
}

vertex ColorInOut Depth_Pass_Text_vert(device vertex_heavy_t* vertex_array [[ buffer(0) ]],
                                       constant float4x4& mvp [[buffer(SHADER_DEPTH_PASS_SKIN_mvp)]],
                                       constant JointData* Joint_Data [[ buffer(SHADER_DEPTH_PASS_SKIN_jointdata)]],
                                       unsigned int vid [[ vertex_id ]]
                                       )
{
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

