//
//  GPUSkinShader.fsh
//  SGEngine2
//
//  Created by Vivek on 12/02/15.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//
precision highp float;

uniform sampler2D texture1,depthTexture;
uniform  float shadowTextureSize;
uniform  float transparency,shadowDarkness;

varying  vec2 vTexCoord;
varying  float shadowDist;
varying  vec3 lighting;
varying highp vec4 texCoordsBias;

float unpack(const in vec4 rgba_depth) {
    const vec3 bit_shift = vec3(1.0/65536.0, 1.0/256.0, 1.0);
    float depth = dot(rgba_depth.rgb, bit_shift);
    return depth;
}

float GetShadowValue(in vec2 offset) {
    vec2 pixelCoord = vec2(texCoordsBias.x, texCoordsBias.y);
    vec4 texelColor = texture2D(depthTexture, pixelCoord + offset);
    vec4 unpackValue = vec4(vec3(texelColor.xyz),1.0);
    float extractedDistance = unpack(unpackValue);
    return (extractedDistance < shadowDist) ? (shadowDarkness):(0.0); // todo
}

void main()
{
    lowp float shadowValue = 0.0;
    if(shadowDist > 0.0) {
        float delta = 1.0/2048.0; // todo
        shadowValue = GetShadowValue(vec2(0.0, 0.0));
        shadowValue += GetShadowValue(vec2(-delta, 0.0));
        shadowValue += GetShadowValue(vec2(delta, 0.0));
        shadowValue += GetShadowValue(vec2(0.0, -delta));
        shadowValue += GetShadowValue(vec2(0.0, delta));
        shadowValue /= 5.0;
    }
    lowp vec4 base = texture2D(texture1,vTexCoord.xy);
    gl_FragColor.xyz = mix(base.xyz * lighting, vec3(0.0, 0.0, 0.0), shadowValue);
    gl_FragColor.a = base.a * transparency;
}
