precision highp float;

uniform sampler2D colorMap;
uniform sampler2D shadowMap;
uniform sampler2D normalMap;
uniform sampler2D reflectionMap;

uniform float shadowDarkness, hasReflectionMap, hasNormalMap, ambientLight, lightCount;
uniform vec3 lightColor[5], lightPos[5];
uniform float fadeEndDistance[5], lightTypes[5];

varying float vHasLighting, vReflectionValue, vHasMeshColor, vTransparencyValue, vShadowDist;
varying vec2 vTexCoord, vTexCoordBias;
varying vec3 vMeshColor;
varying vec3 vEyeVec, vVertexPosition;
varying mat3 vTBNMatrix;

float unpack(const in vec4 rgba_depth)
{
    const vec3 bit_shift = vec3(1.0/65536.0, 1.0/256.0, 1.0);
    float depth = dot(rgba_depth.rgb, bit_shift);
    return depth;
}

float GetShadowValue()
{
    vec4 texelColor = texture2D(shadowMap, vTexCoordBias);
    vec4 unpackValue = vec4(texelColor.xyz, 1.0);
    float extractedDistance = unpack(unpackValue);
    float shadow = clamp(extractedDistance - vShadowDist, 0.0, shadowDarkness);
    return shadow;
}

vec4 getColorOfLight(in int index, in vec3 normal)
{
    vec3 lightDir = (lightTypes[index] == 1.0) ? lightPos[index] :  normalize(lightPos[index] - vVertexPosition);
    float distanceFromLight = distance(lightPos[index] , vVertexPosition);

    float distanceRatio = (1.0 - clamp((distanceFromLight / fadeEndDistance[index]), 0.0, 1.0));
    distanceRatio = mix(1.0, distanceRatio, lightTypes[index]);

    float n_dot_l = clamp(dot(normal, lightDir), 0.0, 1.0);

    float darkness = distanceRatio * n_dot_l;
    darkness = clamp(darkness, ambientLight, 1.0);

    vec4 colorOfLight = vec4(lightColor[index] * darkness, 1.0);
    return colorOfLight;
}

float getSpecularOfLight(in int index, in vec3 normal)
{
    vec3 lightDir = (lightTypes[index] == 1.0) ? lightPos[index] :  normalize(lightPos[index] - vVertexPosition);
    float n_dot_l = clamp(dot(normal, lightDir), 0.0, 1.0);

    vec3 reflectValue = -lightDir + 2.0 * n_dot_l * normal;
    float e_dot_r =  clamp(dot(vEyeVec, reflectValue), 0.0, 1.0);
    
    float maxSpecular = 30.0;
    float s = vReflectionValue * pow(e_dot_r, maxSpecular);
    
    float e_dot_l = dot(lightDir, vEyeVec);
    if(e_dot_l < -0.8)
        s = 0.0;
    
    return s;
}

void main()
{
    vec4 diffuse_color = vec4(vMeshColor, 1.0);
    
    if(vHasMeshColor < 0.5)
        diffuse_color = texture2D(colorMap, vTexCoord);
    
    if(diffuse_color.a <= 0.5)
        discard;
    
    float shadowValue = 0.0;
    
    if(vShadowDist > 0.1)
        shadowValue = GetShadowValue();

    vec3 normal = normalize(vTBNMatrix[2]);
    if(hasNormalMap > 0.5) {
        vec3 n = texture2D(normalMap, vTexCoord).xyz * 2.0 - 1.0;
        normal = normalize(vTBNMatrix * n);
    }

    vec4 specular = vec4(0.0);
    vec4 colorOfLight = vec4(1.0);

    if(vHasLighting > 0.5) {
        if(vReflectionValue > 0.0) {
            if(hasReflectionMap > 0.5) {
                vec3 r = reflect(vEyeVec, normal);
                float m = 2.0 * sqrt(r.x * r.x + r.y * r.y + (r.z + 1.0) * (r.z + 1.0));
                vec2 vN = (r.xy / m) + 0.5;
                
                specular = texture2D(reflectionMap, vN);
            } else
                specular = vec4(getSpecularOfLight(0, normal));
        }
        
        colorOfLight = vec4(0.0);
        for (int i = 0; i < int(lightCount); i++) {
            colorOfLight += getColorOfLight(i, normal);
        }
    }

    vec4 finalColor = vec4(diffuse_color + specular) * colorOfLight;

    if(hasReflectionMap > 0.5)
        finalColor = vec4(diffuse_color * (1.0 - vReflectionValue) + specular * vReflectionValue) * colorOfLight;
    
    finalColor -= (finalColor * shadowValue);
    
    gl_FragColor.xyz = finalColor.xyz;
    gl_FragColor.a = diffuse_color.a * vTransparencyValue;
}


