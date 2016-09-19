
#extension GL_EXT_draw_instanced : enable

attribute vec3 vertPosition;
attribute vec3 vertNormal;
attribute vec2 texCoord1;
attribute vec3 vertTangent;
attribute vec3 vertBitangent;
attribute vec4 vertColor;
attribute vec4 optionalData1;
attribute vec4 optionalData2;
attribute vec4 optionalData3;
attribute vec4 optionalData4;

uniform vec3 eyePos;
uniform vec3 meshColor[1];
uniform mat4 mvp, lvp, model[1], jointTransforms[jointsSize];
uniform float hasLighting[1], reflectionValue[1], hasMeshColor[1], uvScaleValue[1], transparencyValue[1];

varying float vHasLighting, vReflectionValue, vHasMeshColor, vTransparencyValue, vShadowDist;
varying vec2 vTexCoord, vTexCoordBias;
varying vec3 vMeshColor;
varying vec3 vEyeVec, vVertexPosition;
varying mat3 vTBNMatrix;

void main()
{
    vHasMeshColor = hasMeshColor[0];
    vMeshColor = (meshColor[0].x == -1.0) ? vertColor.xyz : meshColor[0];
    vTexCoord = texCoord1 * uvScaleValue[0];
    vReflectionValue = reflectionValue[0];
    vTransparencyValue = transparencyValue[0];
    vHasLighting = hasLighting[0];

    mat4 finalMatrix = mat4(1.0);

    int jointId = int(optionalData1.x);
    if(jointId > 0)
        finalMatrix = jointTransforms[jointId - 1] * optionalData2.x;

    jointId = int(optionalData1.y);
    if(jointId > 0)
        finalMatrix = finalMatrix + (jointTransforms[jointId - 1] * optionalData2.y);
    
    jointId = int( optionalData1.z);
    if(jointId > 0)
        finalMatrix = finalMatrix + (jointTransforms[jointId - 1] * optionalData2.z);
    
    jointId = int( optionalData1.w);
    if(jointId > 0)
        finalMatrix = finalMatrix + (jointTransforms[jointId - 1] * optionalData2.w);
    
    jointId = int( optionalData3.x);
    if(jointId > 0)
        finalMatrix = finalMatrix + (jointTransforms[jointId - 1] * optionalData4.x);
    
    jointId = int( optionalData3.y);
    if(jointId > 0)
        finalMatrix = finalMatrix + (jointTransforms[jointId - 1] * optionalData4.y);
    
    jointId = int( optionalData3.z);
    if(jointId > 0)
        finalMatrix = finalMatrix + (jointTransforms[jointId - 1] * optionalData4.z);
    
    jointId = int( optionalData3.w);
    if(jointId > 0)
        finalMatrix = finalMatrix + (jointTransforms[jointId - 1] * optionalData4.w);
    
    gl_Position = mvp * finalMatrix * vec4(vertPosition, 1.0);
    
    finalMatrix = model[0] * finalMatrix;
    vec4 pos = finalMatrix * vec4(vertPosition, 1.0);
    vec4 nor = finalMatrix * vec4(vertNormal, 0.0);

    vec3 T = normalize(vec3(finalMatrix * vec4(vertTangent, 0.0)));
    vec3 B = normalize(vec3(finalMatrix * vec4(vertBitangent, 0.0)));
    vec3 N = normalize(vec3(nor));
    vTBNMatrix = mat3(T, B, N);
    
    vVertexPosition = pos.xyz;
    vEyeVec = normalize(vVertexPosition - eyePos);
    
    if(bool(hasLighting[0])) {
        vec4 vertexLightCoord = lvp * pos;
        vec4 texCoords = vertexLightCoord / vertexLightCoord.w;
        vTexCoordBias = ((texCoords / 2.0) + 0.5).xy;
        
        if(vertexLightCoord.w > 0.0) {
            vShadowDist = (vertexLightCoord.z) / 5000.0;
            vShadowDist += 0.00000009;
        }
    } else {
        vShadowDist = 0.0;
        vTexCoordBias = vec2(0.0);
    }
}

