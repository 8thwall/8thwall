#version 320 es
#extension GL_GOOGLE_include_directive : enable

#include "c8/pixels/render/shaders/geometry.glsl"

in vec4 position;
in vec2 uv;
in vec4 instancePosition;
in vec4 instanceRotation;
in vec4 instanceScale;

out vec2 texUv;

uniform mat4 mvp;

void main() {
  gl_Position = mvp * trs(instancePosition, instanceRotation, instanceScale) * position;
  texUv = uv;
}
