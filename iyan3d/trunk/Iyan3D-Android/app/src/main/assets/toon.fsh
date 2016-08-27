//
//  toon.fsh
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

uniform  float shadowDarkness;
uniform  float shadowTextureSize;
uniform float numberOfLights;
uniform vec3 lightColor[5] , lightPos[5];
uniform float fadeEndDistance[5];
uniform float lightTypes[5], uvScale;

varying float vtransparency, visVertexColored, vreflection;
varying vec3 vertexColor;
varying float shadowDist,lightingValue;
varying vec2 vTexCoord, texCoordsBias;
varying vec3 normal, eyeVec, vertexPosCam;

float unpack(const in vec4 rgba_depth) {
    const vec3 bit_shift = vec3(1.0/65536.0, 1.0/256.0, 1.0);
    float depth = dot(rgba_depth.rgb, bit_shift);
    return depth;
}

float GetShadowValue(in vec2 offset) {
    vec2 pixelCoord = vec2(texCoordsBias.x, texCoordsBias.y);
    vec4 texelColor = texture2D(shadowMap, pixelCoord + offset);
    vec4 unpackValue = vec4(vec3(texelColor.xyz),1.0);
    float extractedDistance = unpack(unpackValue);
    return (extractedDistance < shadowDist) ? (shadowDarkness):(0.0);
}

void main()
{
    float maxSpecular = 30.0;
    
    // shadow Calculation ------
    float shadowValue = 0.0;
    if(shadowDist > 0.0) {
        float delta = 1.0/2048.0; // todo
        shadowValue = GetShadowValue(vec2(0.0, 0.0));
        shadowValue += GetShadowValue(vec2(-delta, 0.0));
        shadowValue += GetShadowValue(vec2(delta, 0.0));
        shadowValue += GetShadowValue(vec2(0.0, -delta));
        shadowValue += GetShadowValue(vec2(0.0, delta));
        shadowValue /= 5.0;
    }
    //------
    
    // Lighting calculation-----
    
    lowp vec4 diffuse_color = vec4(vec3(vertexColor),1.0);
    
    if(int(visVertexColored) == 0)
        diffuse_color = texture2D(colorMap, vTexCoord);

    vec4 specular = vec4(0.0);
    vec4 toonColor = diffuse_color;
    if(lightingValue != 0.0){
        vec3 lightDir = (lightTypes[0] == 1.0) ? lightPos[0] :  normalize(lightPos[0] - vertexPosCam);
        float distanceFromLight = distance(lightPos[0], vertexPosCam);

        vec3 normal = normalize(normal);
        vec3 eyeVec = normalize(eyeVec);
        float n_dot_l = clamp(dot(normal, lightDir), 0.0, 1.0);
        vec4 diffuse = vec4(vec3(n_dot_l), 1.0);
        diffuse_color = diffuse * vec4(diffuse_color);
        
        vec3 reflectValue = -lightDir + 2.0 * n_dot_l * normal;
        float e_dot_r =  clamp(dot(eyeVec, reflectValue), 0.0, 1.0);
        specular = vec4(vreflection * pow(e_dot_r, maxSpecular));
        float e_dot_l = dot(lightDir, eyeVec);
        if(e_dot_l < -0.8)
            specular = vec4(0.0);
        
        toonColor = vec4(diffuse_color + specular) * vec4(vec3(lightColor[0]),1.0);
        if (n_dot_l > 0.95)
            toonColor = vec4(1.0, 1.0, 1.0,1.0) * toonColor;
        else if (n_dot_l > 0.6)
            toonColor = vec4(0.8, 0.8, 0.8,1.0) * toonColor;
        else if (n_dot_l > 0.2)
            toonColor = vec4(0.6, 0.6, 0.6,1.0) * toonColor;
        else
            toonColor = vec4(0.4, 0.4, 0.4,1.0) * toonColor;
    }
    //-------------
    
    
    vec4 finalColor = toonColor;
    finalColor = finalColor + (vec4(0.0, 0.0, 0.0, 0.0) - finalColor) * (shadowValue);
    
    gl_FragColor.xyz = finalColor.xyz;
    gl_FragColor.a = diffuse_color.a * vtransparency;
}
