//
//  GPUSkinShader.vsh
//  SGEngine2
//
//  Created by Vivek on 12/02/15.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

attribute vec3 vertPosition;
attribute vec3 vertNormal;
attribute vec2 texCoord1;
attribute vec3 vertTangent;
attribute vec3 vertBitangent;
attribute vec4 optionalData1;
attribute vec4 optionalData2;
attribute vec4 optionalData3;
attribute vec4 optionalData4;

uniform vec3 eyePos;
uniform vec3 perVertexColor[1];
uniform mat4 mvp, model[1] ,lvp;
uniform mat4 jointTransforms[57];
uniform vec4 props[1];
uniform float ambientLight, uvScale;

varying float vtransparency, visVertexColored, vreflection;
varying float shadowDist, lightingValue;
varying vec2 vTexCoord, texCoordsBias, vReflectionCoord;
varying vec3 vertexColor;
varying vec3 normal, eyeVec, vertexPosCam;
varying mat3 tbnMatrix;

void main()
{
    vtransparency = props[0].x;
    visVertexColored = props[0].y;
    vreflection = props[0].z;

    vertexColor = perVertexColor[0];
    vTexCoord = texCoord1 * uvScale;

    vec4 pos = vec4(0.0);
    vec4 nor = vec4(0.0);
    int jointId = int(optionalData1.x);
    float strength = optionalData2.x ;
    if(jointId > 0){
        pos = pos + (jointTransforms[jointId - 1] * vec4(vertPosition,1.0)) * strength;
        nor = nor + (jointTransforms[jointId - 1] * vec4(vertNormal,0.0)) * strength;
    }else{
        pos = vec4(vertPosition,1.0);
        nor = vec4(vertNormal,0.0);
    }
    
    jointId = int(optionalData1.y);
    strength = optionalData2.y;
    if(jointId > 0){
        pos = pos + (jointTransforms[jointId - 1] * vec4(vertPosition,1.0)) * strength;
        nor = nor + (jointTransforms[jointId - 1] * vec4(vertNormal,0.0)) * strength;
    }
    
    jointId = int( optionalData1.z);
    strength = optionalData2.z;
    if(jointId > 0){
        pos = pos + (jointTransforms[jointId - 1] * vec4(vertPosition,1.0)) * strength;
        nor = nor + (jointTransforms[jointId - 1] * vec4(vertNormal,0.0)) * strength;
    }
    
    jointId = int( optionalData1.w);
    strength = optionalData2.w;
    if(jointId > 0){
        pos = pos + (jointTransforms[jointId - 1] * vec4(vertPosition,1.0)) * strength;
        nor = nor + (jointTransforms[jointId - 1] * vec4(vertNormal,0.0)) * strength;
    }
    
    jointId = int( optionalData3.x);
    strength = optionalData4.x;
    if(jointId > 0){
        pos = pos + (jointTransforms[jointId - 1] * vec4(vertPosition,1.0)) * strength;
        nor = nor + (jointTransforms[jointId - 1] * vec4(vertNormal,0.0)) * strength;
    }
    
    jointId = int( optionalData3.y);
    strength = optionalData4.y;
    if(jointId > 0){
        pos = pos + (jointTransforms[jointId - 1] * vec4(vertPosition,1.0)) * strength;
        nor = nor + (jointTransforms[jointId - 1] * vec4(vertNormal,0.0)) * strength;
    }
    
    jointId = int( optionalData3.z);
    strength = optionalData4.z;
    if(jointId > 0){
        pos = pos + (jointTransforms[jointId - 1] * vec4(vertPosition,1.0)) * strength;
        nor = nor + (jointTransforms[jointId - 1] * vec4(vertNormal,0.0)) * strength;
    }
    
    jointId = int( optionalData3.w);
    strength = optionalData4.w;
    if(jointId > 0){
        pos = pos + (jointTransforms[jointId - 1] * vec4(vertPosition,1.0)) * strength;
        nor = nor + (jointTransforms[jointId - 1] * vec4(vertNormal,0.0)) * strength;
    }
    
    lightingValue = props[0].w;
    vertexPosCam = (model[0] * pos).xyz;
    
    if(int(props[0].w) == 1) {
        vec4 vertexLightCoord = (lvp * model[0]) * pos;
        vec4 texCoords = vertexLightCoord / vertexLightCoord.w;
        texCoordsBias = ((texCoords / 2.0) + 0.5).xy;
        
        if(vertexLightCoord.w > 0.0) {
            shadowDist = (vertexLightCoord.z) / 5000.0;
            shadowDist += 0.00000009;
        }
        normal = normalize(model[0] * normalize(nor)).xyz;
        eyeVec = normalize(eyePos - vertexPosCam);
    } else {
        shadowDist = 0.0;
        texCoordsBias = vec2(0.0);
        normal = vec3(0.0);
        eyeVec = vec3(0.0);
    }

    vec3 T = normalize(vec3(model[0] * vec4(vertTangent, 0.0)));
    vec3 B = normalize(vec3(model[0] * vec4(vertBitangent, 0.0)));
    vec3 N = normalize(vec3(model[0] * vec4(vertNormal,  0.0)));
    tbnMatrix = mat3(T, B, N);

    vec3 r = reflect( -eyeVec, normal );
    float m = 2. * sqrt(pow( r.x, 2. ) + pow( r.y, 2. ) + pow( r.z + 1., 2.0));
    vReflectionCoord = r.xy / m + .5;
    vReflectionCoord.y = -vReflectionCoord.y;

    gl_Position = mvp * pos;
}

