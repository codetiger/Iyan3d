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

uniform int isLighting;
uniform float brightness, shininess,shadowDarkness;
uniform vec3 lightPos, eyePos, lightColor;
uniform mat4 lightViewProjection;
uniform mat4 Model;
uniform mat4 mvp;

varying float shadowDist;
varying vec3 lighting;
varying vec2 vTexCoord;
varying float intensity;
varying vec4 texCoordsBias;


void main()
{
    //Shadow calculation-------
    mat4 lvp = lightViewProjection;
    vec4 vertexLightCoord = (lvp * Model) * vec4(vertPosition, 1.0);
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
        vec3 normal = normalize((Model * vec4(vertNormal, 0.0)).xyz);
        vec3 lightDir = normalize(lightPos - (Model * vec4(vertPosition, 1.0)).xyz);
        vec3 eyeVec = normalize(eyePos - (Model * vec4(vertPosition, 1.0)).xyz);
        vec3 r = normalize(-reflect(lightDir,normal));
        
        vec4 specular = vec4(vec3(shininess), 1.0) * pow(max(dot(r, eyeVec),0.0), 0.75);
        specular = clamp(specular, 0.0, 1.0);
        
        intensity = dot(normal, normalize(lightDir));
        vec4 diffuse = vec4(1.0) * dot(normal, lightDir);
        vec4 ambient = vec4(vec3(brightness), 1.0);
        lighting = (ambient.xyz + diffuse.xyz + specular.xyz);
        lighting = clamp(lighting, 0.0, 1.0) * lightColor;
    }
    else{
        shadowDist = 0.0;
        lighting = vec3(1.0);
    }
    //-----------
   
    vTexCoord = texCoord1;
    gl_Position = (mvp) * vec4(vec3(vertPosition),1.0);
}

