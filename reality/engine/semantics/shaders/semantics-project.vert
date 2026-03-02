#version 320 es

in vec4 position;
in vec2 uv;

out vec2 texUv;
out vec4 sourceClipPos;

uniform mat4 mvp;
uniform mat4 sourceCameraMvp;

void main() {
  gl_Position = mvp * position;
  sourceClipPos = sourceCameraMvp * position;
  texUv = uv;
}

