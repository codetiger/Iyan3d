//
//  ShaderParamOrder.h
//  FatMan
//
//  Created by Vivek on 12/02/15.
//  Copyright (c) 2015 Vivek. All rights reserved.
//


// format : ShaderIndex_uniformName

#ifndef Iyan3D_ShaderParamOrder_h
#define Iyan3D_ShaderParamOrder_h

#define SHADER_PARTICLE_vp 1
#define SHADER_PARTICLE_sColor 2
#define SHADER_PARTICLE_mColor 3
#define SHADER_PARTICLE_eColor 4
#define SHADER_PARTICLE_props 5
#define SHADER_PARTICLE_positions 6
#define SHADER_PARTICLE_rotations 7
#define SHADER_PARTICLE_texture1 8
#define SHADER_PARTICLE_world 9


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


#define SHADER_COLOR_mvp 1
#define SHADER_COLOR_vertexColor 2
#define SHADER_COLOR_transparency 3

#define SHADER_COLOR_SKIN_mvp 1
#define SHADER_COLOR_SKIN_vertexColor 2
#define SHADER_COLOR_SKIN_jointData 3
#define SHADER_COLOR_SKIN_transparency 4

#define SHADER_DRAW_2D_IMAGE_Texture1 0

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
#define SHADER_PERVERTEXCOLOR_jointData 13


#define SHADER_DEPTH_PASS_mvp 1

#define SHADER_DEPTH_PASS_SKIN_mvp 1
#define SHADER_DEPTH_PASS_SKIN_jointdata 2


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

#endif
