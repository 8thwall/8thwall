#version 320 es

in vec3 position;
in vec2 uv;
out vec2 texUv;

void main() {
  gl_Position = vec4(position, 1.0);
  texUv = uv;
}
