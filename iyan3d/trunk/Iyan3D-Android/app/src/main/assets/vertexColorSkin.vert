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


uniform vec3 perVertexColor[1];
uniform vec3 eyePos;
uniform mat4 mvp, model[1], lvp;
uniform mat4 jointTransforms[57];
uniform vec4 props[1];

varying float vtransparency, visVertexColored, vreflection;
varying float shadowDist,lightingValue;
varying vec2 vTexCoord;
varying vec3 vertexColor;
varying vec4 texCoordsBias,normal,eyeVec,vertexPosCam;


void main()
{
    vtransparency = props[0].x;
    visVertexColored = props[0].y;
    vreflection = props[0].z;
    vertexColor = perVertexColor[0];
    vTexCoord = texCoord1;

    vec4 pos = vec4(0.0);
    vec4 nor = vec4(0.0);
    int jointId = int(optionalData1.x);
    float strength = optionalData2.x ;

    pos = (jointTransforms[jointId - 1] * vec4(vertPosition,1.0)) * strength;
    nor = (jointTransforms[jointId - 1] * vec4(vertNormal,0.0)) * strength;
    
    lightingValue = props[0].w;
    vec4 vertex_position_cameraspace = model[0] * pos;
    vertexPosCam = vertex_position_cameraspace;
    
    
    if(int(props[0].w) == 1) {
        vec4 vertexLightCoord = (lvp * model[0]) * pos;
        vec4 texCoords = vertexLightCoord / vertexLightCoord.w;
        texCoordsBias = (texCoords / 2.0) + 0.5;
        
        if(vertexLightCoord.w > 0.0) {
            shadowDist = (vertexLightCoord.z) / 5000.0;
            shadowDist += 0.00000009;
        }
        vec4 eye_position_cameraspace = vec4(vec3(eyePos),1.0);
        normal = normalize(model[0] * normalize(nor));
        eyeVec = normalize(eye_position_cameraspace - vertex_position_cameraspace);
    } else {
        shadowDist = 0.0;
        texCoordsBias = vec4(0.0);
        normal = vec4(0.0);
        eyeVec = vec4(0.0);
    }

    gl_Position = (mvp) * pos;
}
