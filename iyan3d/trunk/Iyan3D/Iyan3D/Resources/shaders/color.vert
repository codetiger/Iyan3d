attribute vec3 vertPosition;

uniform mat4 mvp;
void main() {
    gl_Position = (mvp) * vec4(vertPosition, 1.0);
}