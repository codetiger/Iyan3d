#extension GL_EXT_draw_instanced : enable

attribute vec3 vertPosition;
attribute vec3 vertNormal;

uniform mat4 vp;
uniform mat4 model;

uniform vec4 startColor;
uniform vec4 midColor;
uniform vec4 endColor;
uniform vec4 props;

varying vec4 color;

void main() {
    
    
    float percent = vertNormal.x/float(props.x);
    float phase = float(percent > 0.5);
    
    vec4 sColor = vec4(startColor.x, startColor.y, startColor.z, 1.0);
    vec4 mColor = vec4(midColor.x, midColor.y, midColor.z, 1.0);
    
    vec4 s = mix(sColor, mColor, phase);
    vec4 e = mix(mColor, endColor, phase);
    float age = mix(percent, percent - 0.5, phase) * 2.0;
    
    color = startColor;
    
    float live = float(vertNormal.x > 0.0 && vertNormal.x <= float(props.x));
    
    vec3 temp = vec3(props.y - vertPosition.x, props.z - vertPosition.y, props.w - vertPosition.z);
    float dist = sqrt(temp.x * temp.x + temp.y * temp.y + temp.z * temp.z);
    
    gl_Position = vp * vec4(vertPosition, 1.0);
    gl_PointSize = (8.0 * live) * (100.0/dist);
}
