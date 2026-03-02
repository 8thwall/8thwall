#version 320 es

in vec4 position;
in vec2 uv;
out vec2 texUv;

uniform mat4 mvp;

void main() {
  gl_Position = mvp * position;
  texUv = uv;
}
