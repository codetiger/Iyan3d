precision lowp float;

uniform sampler2D texture1;

varying vec4 color;
varying vec2 vTexCoord;

void main() {
    
    lowp vec4 diffuse_color = texture2D(texture1,vTexCoord.xy);
    gl_FragColor = vec4(color.xyz, diffuse_color.x);
}
