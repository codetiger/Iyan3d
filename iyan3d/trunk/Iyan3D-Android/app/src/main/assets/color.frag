precision lowp float;

uniform vec3 meshColor[1];
uniform float transparencyValue[1];

void main() {
	gl_FragColor = vec4(vec3(meshColor[0]), transparencyValue[0]);
}
