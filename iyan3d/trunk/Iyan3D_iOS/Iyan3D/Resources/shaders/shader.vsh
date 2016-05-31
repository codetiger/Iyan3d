#extension GL_EXT_draw_instanced : enable

attribute vec3 vertPosition;
attribute vec3 vertNormal;
attribute vec2 texCoord1;
attribute vec4 optionalData1;

uniform mat4 model[uniSize];
uniform vec4 props[uniSize];
uniform vec3 perVertexColor[uniSize];

uniform vec3 eyePos;
uniform mat4 vp;
uniform mat4 lvp;


varying float vtransparency, visVertexColored, vreflection;
varying float shadowDist,lightingValue;
varying vec2 vTexCoord;
varying vec3 vertexColor;
varying vec4 texCoordsBias,normal,eyeVec , vertexPosCam;


void main()
{
    int iId = gl_InstanceIDEXT;
    vtransparency = props[iId].x;
    visVertexColored = props[iId].y;
    vreflection = props[iId].z;
    vertexColor = perVertexColor[iId];
    vTexCoord = texCoord1;
    
    lightingValue = props[iId].w;
    vec4 vertex_position_cameraspace = model[iId] * vec4(vertPosition, 1.0);
    vertexPosCam = vertex_position_cameraspace;
    
    //Lighting Calculation-------
    if(int(props[iId].w) == 1) {
        vec4 vertexLightCoord = (lvp * model[iId]) * vec4(vertPosition, 1.0);
        vec4 texCoords = vertexLightCoord / vertexLightCoord.w;
        texCoordsBias = (texCoords / 2.0) + 0.5;
        
        if(vertexLightCoord.w > 0.0) {
            shadowDist = (vertexLightCoord.z) / 5000.0;
            shadowDist += 0.00000009;
        }
        vec4 eye_position_cameraspace = vec4(vec3(eyePos),1.0);
        normal = normalize(model[iId] * vec4(vertNormal, 0.0));
        eyeVec = normalize(eye_position_cameraspace - vertex_position_cameraspace);
    } else {
        shadowDist = 0.0;
        texCoordsBias = vec4(0.0);
        normal = vec4(0.0);
        eyeVec = vec4(0.0);
    }
    //-----------
    gl_Position = vp * model[iId] * vec4(vertPosition,1.0);
}

