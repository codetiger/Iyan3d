#extension GL_EXT_draw_instanced : enable

attribute vec3 vertPosition;
attribute vec2 texCoord1;

uniform mat4 vp;

uniform vec4 startColor;
uniform vec4 midColor;
uniform vec4 endColor;
uniform vec4 props;
uniform vec4 positions[500];
uniform vec4 rotations[500];

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
    translation[3][0] = positions[gl_InstanceIDEXT].x * scale;
    translation[3][1] = positions[gl_InstanceIDEXT].y * scale;
    translation[3][2] = positions[gl_InstanceIDEXT].z * scale;

    mat4 rotationMat = mat4(1);
    float cr = cos(rotations[gl_InstanceIDEXT].x);
    float sr = sin(rotations[gl_InstanceIDEXT].x);
    float cp = cos(rotations[gl_InstanceIDEXT].y);
    float sp = sin(rotations[gl_InstanceIDEXT].y);
    float cy = cos(rotations[gl_InstanceIDEXT].z);
    float sy = sin(rotations[gl_InstanceIDEXT].z);
    
    rotationMat[0][0] = (cp * cy);
    rotationMat[0][1] = (cp * sy);
    rotationMat[0][2] = (-sp);
    
    float srsp = sr * sp;
    float crsp = cr * sp;
    
    rotationMat[1][0] = (srsp * cy - cr * sy);
    rotationMat[1][1] = (srsp * sy + cr * cy);
    rotationMat[1][2] = (sr * cp);
    
    rotationMat[2][0] = (crsp * cy + sr * sy);
    rotationMat[2][1] = (crsp * sy - sr * cy);
    rotationMat[2][2] = (cr * cp);

    mat4 scaleMat = mat4(1);
    scaleMat[0][0] = scale;
    scaleMat[1][1] = scale;
    scaleMat[2][2] = scale;
    
    float live = float(positions[gl_InstanceIDEXT].w > 0.0 && positions[gl_InstanceIDEXT].w <= float(props.x));
    translation = translation * live;

    mat4 trans = translation * rotationMat * scaleMat;
    gl_Position = vp * trans * vec4(vertPosition, 1.0);
}
