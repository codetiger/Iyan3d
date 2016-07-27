precision lowp float;

uniform vec3 perVertexColor[1];
uniform float transparency;

void main() {
	gl_FragColor = vec4(vec3(perVertexColor[0]), transparency);
}
