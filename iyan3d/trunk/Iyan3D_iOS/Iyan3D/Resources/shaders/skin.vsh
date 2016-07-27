
#extension GL_EXT_draw_instanced : enable

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
uniform mat4 vp, lvp, model[1], jointTransforms[161];
uniform float hasLighting[1], reflectionValue[1], hasMeshColor[1], uvScaleValue[1], transparencyValue[1];
uniform float ambientLight;

varying float vHasLighting, vReflectionValue, vHasMeshColor, vTransparencyValue, vShadowDist;
varying vec2 vTexCoord, vTexCoordBias, vReflectionCoord;
varying vec3 vMeshColor;
varying vec3 vEyeVec, vVertexPosition;
varying mat3 vTBNMatrix;

void main()
{
    vMeshColor = (meshColor[0].x == -1.0) ? optionalData1.xyz : meshColor[0];
    vTexCoord = texCoord1 * uvScaleValue[0];
    
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

    vMeshColor = hasMeshColor[0];
    vTexCoord = texCoord1 * uvScaleValue[0];
    
    vec3 T = normalize(vec3(model[0] * vec4(vertTangent, 0.0)));
    vec3 B = normalize(vec3(model[0] * vec4(vertBitangent, 0.0)));
    vec3 N = normalize(vec3(model[0] * vec4(nor,  0.0)));
    vTBNMatrix = mat3(T, B, N);
    
    vVertexPosition = (model[0] * vec4(pos, 1.0)).xyz;
    vEyeVec = normalize(eyePos - vVertexPosition);
    
    if(bool(hasLighting[0])) {
        vec4 vertexLightCoord = lvp * vVertexPosition;
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
    
    if(reflectionValue[0] > 0.0) {
        vec3 r = reflect( -eyeVec, N );
        float m = 2. * sqrt(pow( r.x, 2. ) + pow( r.y, 2. ) + pow( r.z + 1., 2.0));
        vReflectionCoord = r.xy / m + .5;
        vReflectionCoord.y = -vReflectionCoord.y;
    }
    
    gl_Position = vp * vVertexPosition;
}

