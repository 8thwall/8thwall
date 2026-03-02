#version 320 es
#extension GL_GOOGLE_include_directive : enable

#include "c8/pixels/render/shaders/geometry.glsl"

in vec4 position;
in vec4 color;
in vec4 instancePosition;
in vec4 instanceRotation;
in vec4 instanceScale;

// Renderer uniforms.
uniform float raysToPix;
uniform mat4 mv;
uniform mat4 mvp;

// Material uniforms.
uniform float pointSize;

out vec4 vertColor;

void main() {
  float colorScale = 1.0 / 255.0;
  vec4 mvPosition = mv * position;
  float mvz = mvPosition.z / mvPosition.w;
  if (raysToPix == 1.0) {
    mvz = 1.0;
  }
  gl_Position = mvp * trs(instancePosition, instanceRotation, instanceScale) * position;
  gl_PointSize = (pointSize * raysToPix) / mvz;
  vertColor = color * colorScale;
}
