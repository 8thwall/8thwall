#version 320 es
#extension GL_GOOGLE_include_directive: enable

precision mediump float;

#include "reality/engine/features/gr8-moment-packing.glsl"

in vec2 texUv;
uniform sampler2D sampler;
uniform vec2 scale;
float texValue(vec2 p) {
  return texture(sampler, scale * p + texUv).r;
}

out vec4 color;
void main() {
  float center = texValue(vec2(0.0,  0.0));
  float left = texValue(vec2(-1.0,  0.0));
  float right = texValue(vec2( 1.0,  0.0));
  float top = texValue(vec2( 0.0, -1.0));
  float bottom = texValue(vec2( 0.0,  1.0));

  float ix = 0.5 * (right - left) +
    0.25 * (texValue(vec2( 1.0,  1.0)) + texValue(vec2( 1.0,  -1.0))
    - texValue(vec2( -1.0,  -1.0)) - texValue(vec2( -1.0,  1.0)));
  float iy = 0.5 * (bottom - top) +
    0.25 * (texValue(vec2( -1.0,  1.0)) + texValue(vec2( 1.0,  1.0))
    - texValue(vec2( -1.0,  -1.0)) - texValue(vec2( 1.0,  -1.0)));
  color.r = center;
  color.gba = packMoment(vec3(ix * ix, iy * iy, ix * iy));
}
