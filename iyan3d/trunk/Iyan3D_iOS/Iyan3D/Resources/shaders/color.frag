precision lowp float;
uniform vec3 vertexColor;
uniform float transparency;
void main() {
	gl_FragColor = vec4(vec3(vertexColor),transparency);
}
