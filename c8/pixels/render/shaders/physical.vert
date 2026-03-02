#version 320 es
#extension GL_GOOGLE_include_directive : enable

#include "c8/pixels/render/shaders/geometry.glsl"
#include "c8/pixels/render/shaders/occlusion-vert.glsl"
#include "c8/pixels/render/shaders/physical-lighting-vert.glsl"

in vec4 position;
in vec4 color;
in vec4 normal;
in vec2 uv;
in vec4 instancePosition;
in vec4 instanceRotation;
in vec4 instanceScale;

out vec2 texUv;

uniform mat4 mv;
uniform mat4 mvp;
uniform mat4 normalMatrix;

out vec4 vertColor;

void main() {
  vec4 instancePos = trs(instancePosition, instanceRotation, instanceScale) * position;
  physicalLighting(instancePos, normal, mv, normalMatrix);
  gl_Position = mvp * instancePos;
  occlusion(mvp, mv, instancePos);
  float colorScale = 1.0 / 255.0;
  vertColor = color * colorScale;
  texUv = uv;
}
