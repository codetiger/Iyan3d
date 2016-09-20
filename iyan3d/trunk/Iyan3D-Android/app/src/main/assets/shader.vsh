//#extension GL_EXT_draw_instanced : enable

attribute vec3 vertPosition;
attribute vec3 vertNormal;
attribute vec2 texCoord1;
attribute vec3 vertTangent;
attribute vec3 vertBitangent;
attribute vec4 optionalData1;

uniform mat4 model[uniSize];
uniform vec4 props[uniSize];
uniform vec3 perVertexColor[uniSize];
uniform float ambientLight, uvScale;

uniform vec3 eyePos;
uniform mat4 vp;
uniform mat4 lvp;

varying float vtransparency, visVertexColored, vreflection;
varying float shadowDist, lightingValue;
varying vec2 vTexCoord, texCoordsBias, vReflectionCoord;
varying vec3 vertexColor;
varying vec3 normal, eyeVec, vertexPosCam;
varying mat3 tbnMatrix;

void main()
{
    int iId = gl_InstanceIDEXT;
    vtransparency = props[iId].x;
    visVertexColored = props[iId].y;
    vreflection = props[iId].z;
    vertexColor = perVertexColor[iId];
    vTexCoord = texCoord1 * uvScale;
    
    lightingValue = props[iId].w;
    vertexPosCam = (model[iId] * vec4(vertPosition, 1.0)).xyz;
    
    vec3 T = normalize(vec3(model[iId] * vec4(vertTangent, 0.0)));
    vec3 B = normalize(vec3(model[iId] * vec4(vertBitangent, 0.0)));
    vec3 N = normalize(vec3(model[iId] * vec4(vertNormal,  0.0)));
    tbnMatrix = mat3(T, B, N);

    //Lighting Calculation-------
    if(int(props[iId].w) == 1) {
        vec4 vertexLightCoord = (lvp * model[iId]) * vec4(vertPosition, 1.0);
        vec4 texCoords = vertexLightCoord / vertexLightCoord.w;
        texCoordsBias = ((texCoords / 2.0) + 0.5).xy;
        
        if(vertexLightCoord.w > 0.0) {
            shadowDist = (vertexLightCoord.z) / 5000.0;
            shadowDist += 0.00000009;
        }
        normal = normalize(model[iId] * vec4(vertNormal, 0.0)).xyz;
        eyeVec = normalize(eyePos - vertexPosCam);
    } else {
        shadowDist = 0.0;
        texCoordsBias = vec2(0.0);
        normal = vec3(0.0);
        eyeVec = vec3(0.0);
    }

    vec3 r = reflect( -eyeVec, normal );
    float m = 2. * sqrt(pow( r.x, 2. ) + pow( r.y, 2. ) + pow( r.z + 1., 2.0));
    vReflectionCoord = r.xy / m + .5;
    vReflectionCoord.y = -vReflectionCoord.y;

    gl_Position = vp * model[iId] * vec4(vertPosition, 1.0);
}

