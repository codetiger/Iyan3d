attribute vec3 vertPosition;
uniform mat4 mvp;

void main(void) {
    gl_Position = (mvp) * vec4(vec3(vertPosition),1.0);
}

