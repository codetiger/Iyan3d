precision lowp float;

uniform sampler2D texture1;

varying vec4 color;

void main() {
    
    lowp vec4 diffuse_color = texture2D(texture1, gl_PointCoord);
    gl_FragColor = vec4(color.xyz, diffuse_color.x);
}
