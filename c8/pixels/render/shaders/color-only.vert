#version 320 es
#extension GL_GOOGLE_include_directive : enable

#include "c8/pixels/render/shaders/geometry.glsl"

in vec4 position;
in vec4 color;
in vec4 instancePosition;
in vec4 instanceRotation;
in vec4 instanceScale;

out vec4 vertColor;

uniform mat4 mvp;

void main() {
  float colorScale = 1.0 / 255.0;
  gl_Position = mvp * trs(instancePosition, instanceRotation, instanceScale) * position;
  vertColor = color * colorScale;
}
