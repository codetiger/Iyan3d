#extension GL_EXT_draw_instanced : enable

attribute vec3 vertPosition;
attribute vec2 texCoord1;

uniform mat4 model;
uniform mat4 vp;

uniform vec4 startColor;
uniform vec4 midColor;
uniform vec4 endColor;

uniform vec4 props;
uniform vec4 positions[1000];
uniform mat4 rotMatrix;

varying vec4 color;
varying vec2 vTexCoord;

void main() {
    vTexCoord = texCoord1;

    float percent = positions[gl_InstanceIDEXT].w/float(props.x);
    float phase = float(percent > 0.5);
    
    vec4 s = mix(startColor, midColor, phase);
    vec4 e = mix(midColor, endColor, phase);
    float age = mix(percent, percent - 0.5, phase) * 2.0;
    color = mix(s, e, age);
    float scale = props.y + (props.z * positions[gl_InstanceIDEXT].w);
    
    mat4 translation = mat4(1);
    translation[3][0] = positions[gl_InstanceIDEXT].x;
    translation[3][1] = positions[gl_InstanceIDEXT].y;
    translation[3][2] = positions[gl_InstanceIDEXT].z;

    mat4 scaleMat = mat4(1);
    scaleMat[0][0] = scale;
    scaleMat[1][1] = scale;
    scaleMat[2][2] = scale;
    
    float live = float(positions[gl_InstanceIDEXT].w > 0.0 && positions[gl_InstanceIDEXT].w <= float(props.x));
    translation = translation * live;

    mat4 trans = translation * rotMatrix;
    gl_Position = vp * trans * vec4(vertPosition, 1.0);
}
