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
attribute vec4 optionalData1;
attribute vec4 optionalData2;
attribute vec4 optionalData3;
attribute vec4 optionalData4;

uniform int isLighting;
uniform vec3 eyePos;
uniform mat4 mvp,Model,lightViewProjection;
uniform mat4 jointTransforms[57];


varying float shadowDist,lightingValue;
varying vec2 vTexCoord;
varying vec3 vertexColor;
varying vec4 texCoordsBias,normal,eyeVec , vertexPosCam;


void main()
{
    vertexColor = vec3(0.0);
    vTexCoord = texCoord1;
    
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
    
    lightingValue = float(isLighting);
    vec4 vertex_position_cameraspace = Model * pos;
    vertexPosCam = vertex_position_cameraspace;
    //Shadow calculation-------
    vec4 vertexLightCoord = (lightViewProjection * Model) * pos;
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
        vec4 eye_position_cameraspace = vec4(vec3(eyePos),1.0);
        normal = normalize(Model * normalize(nor));
        eyeVec = normalize(eye_position_cameraspace - vertex_position_cameraspace);
    }else{
        shadowDist = 0.0;
    }
    //-----------
    vTexCoord = texCoord1;
    gl_Position = (mvp) * pos;
}

