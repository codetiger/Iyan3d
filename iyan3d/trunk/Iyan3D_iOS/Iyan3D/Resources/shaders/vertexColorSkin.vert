//
//  Shader.vsh
//  SGEngine2
//
//  Created by Harishankar on 12/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//
#extension GL_EXT_draw_instanced : enable

attribute vec3 vertPosition;
attribute vec3 vertNormal;
attribute vec2 texCoord1;
attribute vec4 optionalData1;
attribute vec4 optionalData2;
attribute vec4 optionalData3;
attribute vec4 optionalData4;

uniform int isLighting;
uniform float isVertexColored;
uniform vec3 perVertexColor;
uniform vec3 eyePos;
uniform mat4 mvp,Model,lightViewProjection;
uniform mat4 jointTransforms[161];

varying float shadowDist,lightingValue;
varying vec2 vTexCoord;
varying vec3 vertexColor;
varying vec4 texCoordsBias,normal,eyeVec,vertexPosCam;


void main()
{
    
    vertexColor = (int(isVertexColored) == 0) ? vec3(0.0) : perVertexColor;
    vTexCoord = (int(isVertexColored) == 0) ? texCoord1 : vec2(0.0);

//    vertexColor = vec3(optionalData4.xyz);
//    vTexCoord = vec2(0.0);
    vec4 pos = vec4(0.0);
    vec4 nor = vec4(0.0);
    int jointId = int(optionalData1.x);
    float strength = optionalData2.x ;

    pos = (jointTransforms[jointId - 1] * vec4(vertPosition,1.0)) * strength;
    nor = (jointTransforms[jointId - 1] * vec4(vertNormal,0.0)) * strength;
    
    lightingValue = float(isLighting);
    vec4 vertex_position_cameraspace = Model * pos;
    vertexPosCam = vertex_position_cameraspace;
    //Shadow calculation-------
    mat4 lvp = lightViewProjection;
    vec4 vertexLightCoord = (lvp * Model) * pos;
    if(vertexLightCoord.w > 0.0) {
        shadowDist = (vertexLightCoord.z) / 5000.0;
        shadowDist += 0.000000009;
    }else
        shadowDist = 0.0;
    vec4 texCoords = vertexLightCoord / vertexLightCoord.w;
    texCoordsBias = (texCoords / 2.0) + 0.5;
    //--------------
    
    //Lighting Calculation-------
    if(isLighting == 1){
        vec4 eye_position_cameraspace =  vec4(vec3(eyePos),1.0);
        normal = normalize(Model * normalize(nor));
        eyeVec = normalize(eye_position_cameraspace - vertex_position_cameraspace);
    }else{
        shadowDist = 0.0;
    }
    gl_Position = (mvp) * pos;
}
