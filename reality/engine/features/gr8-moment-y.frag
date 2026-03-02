#version 320 es
#extension GL_GOOGLE_include_directive: enable

precision mediump float;

#include "reality/engine/features/gr8-moment.glsl"

out vec4 color;
void main() {
  vec4 center = texRGBA(vec2(0.0, 0.0));

  vec3 v0 = texRGBA(vec2(0.0, -2.0)).gba;
  vec3 v1 = texRGBA(vec2(0.0, -1.0)).gba;
  vec3 v3 = texRGBA(vec2(0.0,  1.0)).gba;
  vec3 v4 = texRGBA(vec2(0.0,  2.0)).gba;

  color.r = center.r;
  color.gba = packMoment(
    0.06136 * v0 +
    0.24477 * v1 +
    0.38774 * center.gba +
    0.24477 * v3 +
    0.06136 * v4
  );
}
