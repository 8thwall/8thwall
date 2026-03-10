#version 320 es

in vec4 position;
out vec4 sourceClipPos;

uniform mat4 mvp;
uniform mat4 sourceCameraMvp;

void main() {
  gl_Position = mvp * position;
  sourceClipPos = sourceCameraMvp * position;
}

