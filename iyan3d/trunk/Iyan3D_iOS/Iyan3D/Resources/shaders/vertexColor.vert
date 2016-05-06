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

uniform int isLighting;
uniform vec3  eyePos;
uniform mat4 mvp,Model,lvp;
uniform vec3 perVertexColor;

varying float shadowDist,lightingValue;
varying vec2 vTexCoord;
varying vec3 vertexColor;
varying vec4 texCoordsBias,normal,eyeVec,lightDir , vertexPosCam;


void main()
{
    vertexColor = (perVertexColor.x == -1.0) ? vec3(optionalData1.xyz) : perVertexColor;
    vTexCoord = vec2(0.0);
    lightingValue = float(isLighting);
    vec4 vertex_position_cameraspace = Model * vec4(vertPosition, 1.0);
    vertexPosCam = vertex_position_cameraspace;

    //Lighting Calculation-------

    if(isLighting == 1) {
        vec4 vertexLightCoord = lvp * vec4(vertPosition, 1.0);
        vec4 texCoords = vertexLightCoord / vertexLightCoord.w;
        texCoordsBias = (texCoords / 2.0) + 0.5;
        
        if(vertexLightCoord.w > 0.0) {
            shadowDist = (vertexLightCoord.z) / 5000.0;
            shadowDist += 0.00000009;
        }
        vec4 eye_position_cameraspace = vec4(vec3(eyePos),1.0);
        normal = normalize(Model * vec4(vertNormal, 0.0));
        eyeVec = normalize(eye_position_cameraspace - vertex_position_cameraspace);
    } else {
        shadowDist = 0.0;
        texCoordsBias = vec4(0.0);
        normal = vec4(0.0);
        eyeVec = vec4(0.0);
    }
    
    //-----------
    gl_Position = (mvp) * vec4(vec3(vertPosition),1.0);
}

