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
uniform float brightness, shininess,shadowDarkness;
uniform vec3 lightPos, eyePos, lightColor;
uniform mat4 lightViewProjection;
uniform mat4 Model;

varying float shadowDist,intensity;
varying vec2 vTexCoord;
varying vec3 lighting;
varying vec4 texCoordsBias;

uniform mat4 jointTransforms[57];
uniform mat4 mvp;

void main()
{
    vTexCoord = texCoord1;
    
    vec4 pos = vec4(0.0);
    vec4 nor = vec4(vertNormal,1.0);
    int jointId = int(optionalData1.x);
    float strength = optionalData2.x ;
    pos = pos + (jointTransforms[jointId - 1] * vec4(vertPosition,1.0)) * strength;
    nor = nor + (jointTransforms[jointId - 1] * vec4(vertNormal,0.0)) * strength;
    
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
    strength = optionalData3.z;
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
    
    //Shadow Calculation---------
    vec4 vertexLightCoord = (lightViewProjection * Model) * pos;
    if(vertexLightCoord.w > 0.0) {
        shadowDist = (vertexLightCoord.z) /5000.0;
        shadowDist += 0.000000009;
    }else
        shadowDist = 0.0;
    vec4 texCoords = vertexLightCoord / vertexLightCoord.w;
    texCoordsBias = (texCoords / 2.0) + 0.5;
    //-----------
    
    if(isLighting == 1){
        
        vec3 normal = normalize((Model * vec4(nor.xyz,0.0)).xyz);
        vec3 lightDir = normalize(lightPos - (Model * pos).xyz);
        vec3 eyeVec = normalize(eyePos - (Model * pos).xyz);
        vec3 r = normalize(-reflect(lightDir,normal));
        
        vec4 specular = vec4(vec3(shininess), 1.0) * pow(max(dot(r, eyeVec),0.0), 0.75);
        specular = clamp(specular, 0.0, 1.0);
        
        intensity = dot(normal,lightDir);
        vec4 diffuse = vec4(1.0) * dot(normal, lightDir);
        vec4 ambient = vec4(vec3(brightness), 1.0);
        lighting = (ambient.xyz + diffuse.xyz + specular.xyz);
        lighting = clamp(lighting, 0.0, 1.0) * lightColor;
    }else{
        shadowDist = 0.0;
        lighting = vec3(1.0);
    }
    
    gl_Position = (mvp) * pos;
}

