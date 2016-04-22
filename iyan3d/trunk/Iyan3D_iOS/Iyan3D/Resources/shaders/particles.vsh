#extension GL_EXT_draw_instanced : enable

attribute vec3 vertPosition;
attribute vec3 vertNormal;

uniform mat4 vp;
uniform mat4 model;

uniform float isVertexColored;
uniform vec4 startColor;
uniform vec4 midColor;
uniform vec4 endColor;
uniform vec4 props;

varying vec4 color;

void main() {
    
    float percent = vertNormal.x/float(props.x);
    float phase = float(percent > 0.5);
    
    vec4 s = mix(startColor, midColor, phase);
    vec4 e = mix(midColor, endColor, phase);
    float age = mix(percent, percent - 0.5, phase) * 2.0;
    
    color = startColor;
    if(int(isVertexColored) == 0)
        color = mix(s, e, age);    
    
    float live = float(vertNormal.x > 0.0 && vertNormal.x <= float(props.x));

    gl_Position = vp * vec4(vertPosition, 1.0);
    gl_PointSize = (45.0 * live) * (100.0/props.w);
}
