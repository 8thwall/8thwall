#version 320 es
#extension GL_GOOGLE_include_directive : enable


in vec4 position;
in vec2 uv;

out vec2 texUv;

void main() {
  gl_Position = position;
  texUv = uv;
}
