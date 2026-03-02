#version 320 es

in vec3 position;
in vec2 uv;
out vec2 texUv;
uniform mediump int rotation;
uniform int flipY;
uniform mat4 mvp;

void main() {
  gl_Position = mvp * vec4(position, 1.0);
  vec2 fixUv = uv;
  if (flipY > 0) {
    fixUv.y = 1.0 - fixUv.y;
  }
  if (rotation == -90 || rotation == 270) {
    texUv = vec2(1.0 - fixUv.y, fixUv.x);
  } else if (rotation == 180 || rotation == -180) {
    texUv = vec2(1.0 - fixUv.x, 1.0 - fixUv.y);
  } else if (rotation == -270 || rotation == 90) {
    texUv = vec2(fixUv.y, 1.0 - fixUv.x);
  } else {
    texUv = fixUv;
  }
}
