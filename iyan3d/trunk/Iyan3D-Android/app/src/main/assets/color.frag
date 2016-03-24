precision lowp float;
uniform vec3 perVertexColor;
uniform float transparency;
void main() {
	gl_FragColor = vec4(vec3(perVertexColor),transparency);
}
