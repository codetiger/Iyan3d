//
//  Shader.fsh
//  SGEngine2
//
//  Created by Harishankar on 12/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//
precision highp float;

uniform sampler2D colorMap;
uniform sampler2D shadowMap;
uniform sampler2D normalMap;
uniform sampler2D reflectionMap;

uniform float shadowDarkness;
uniform float shadowTextureSize;
uniform float hasReflectionMap, hasNormalMap, ambientLight;
uniform vec3 lightColor[5] , lightPos[5];
uniform float fadeEndDistance[5];
uniform float lightTypes[5];

varying float vtransparency, visVertexColored, vreflection;
varying vec3 vertexColor;
varying float shadowDist, lightingValue;
varying vec2 vTexCoord, texCoordsBias, vReflectionCoord;
varying vec3 normal, eyeVec, vertexPosCam;
varying mat3 tbnMatrix;

float unpack(const in vec4 rgba_depth) {
    const vec3 bit_shift = vec3(1.0/65536.0, 1.0/256.0, 1.0);
    float depth = dot(rgba_depth.rgb, bit_shift);
    return depth;
}

float GetShadowValue(in vec2 coords) {
    vec4 texelColor = texture2D(shadowMap, coords);
    vec4 unpackValue = vec4(vec3(texelColor.xyz), 1.0);
    float extractedDistance = unpack(unpackValue);
    return (extractedDistance < shadowDist) ? (shadowDarkness):(0.0);
}

void getColorOfLight(in int index, inout vec4 specular , inout vec4 colorOfLight) {
    
    float maxSpecular = 30.0;
    vec3 lightDir = (lightTypes[index] == 1.0) ? lightPos[index] :  normalize(lightPos[index] - vertexPosCam);
    float distanceFromLight = distance(lightPos[index] , vertexPosCam);
    vec3 eyeVec = normalize(eyeVec);
    
    vec3 normal = normalize(normal);
    if(hasNormalMap > 0.5) {
        vec3 n = texture2D(normalMap, vTexCoord).xyz * 2.0 - 1.0;
        normal = normalize(tbnMatrix * n);
    }
    
    float n_dot_l = clamp(dot(normal, lightDir), 0.0, 1.0);
    
    vec3 reflectValue = -lightDir + 2.0 * n_dot_l * normal;
    float e_dot_r =  clamp(dot(eyeVec, reflectValue), 0.0, 1.0);
    
    if(hasReflectionMap > 0.5) {
        specular = texture2D(reflectionMap, vReflectionCoord);
    } else {
        specular = vec4(vreflection * pow(e_dot_r, maxSpecular));
        
        float e_dot_l = dot(lightDir, eyeVec);
        if(e_dot_l < -0.8)
            specular = vec4(0.0);
    }
    
    float distanceRatio = 1.0;
    if(lightTypes[index] == 0.0)
        distanceRatio = (1.0 - clamp((distanceFromLight/fadeEndDistance[index]) , 0.0,1.0));
    
    float darkness = distanceRatio * n_dot_l;
    darkness = clamp(darkness, ambientLight, 1.0);
    colorOfLight += vec4(vec3(lightColor[index]) * darkness, 1.0);
}

void main()
{
    lowp vec4 diffuse_color = vec4(vertexColor,1.0);
    
    if(visVertexColored < 0.5)
        diffuse_color = texture2D(colorMap, vTexCoord);
    
    if(diffuse_color.a <= 0.5)
        discard;
    
    float shadowValue = 0.0;
    if(shadowDist > 0.0) {
        shadowValue = GetShadowValue(texCoordsBias);
    }
    
    vec4 specular = vec4(0.0), colorOfLight = vec4(1.0);
    
    if(lightingValue > 0.5) {
        colorOfLight = vec4(0.0);
        getColorOfLight(0, specular, colorOfLight);
        getColorOfLight(1, specular, colorOfLight);
        getColorOfLight(2, specular, colorOfLight);
        getColorOfLight(3, specular, colorOfLight);
    }
    
    vec4 finalColor = vec4(diffuse_color + specular) * colorOfLight;
    if(hasReflectionMap > 0.5)
        finalColor = vec4(diffuse_color * (1.0 - vreflection) + specular * vreflection) * colorOfLight;
    finalColor = finalColor + (vec4(0.0, 0.0, 0.0, 0.0) - finalColor) * (shadowValue);
    
    gl_FragColor.xyz = finalColor.xyz;
    gl_FragColor.a = diffuse_color.a * vtransparency;
}