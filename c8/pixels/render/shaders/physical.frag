#version 320 es
#extension GL_GOOGLE_include_directive : enable

precision mediump float;

#include "c8/pixels/render/shaders/occlusion-frag.glsl"
#include "c8/pixels/render/shaders/physical-lighting-frag.glsl"

in vec4 vertColor;
in vec2 texUv;

uniform sampler2D colorSampler;
uniform float hasColorTexture;
uniform float opacity;

out vec4 fragColor;

void main() {
  vec4 color = vertColor;
  if (hasColorTexture != 0.0) {
    color = texture(colorSampler, texUv);
  }
  float visibility = occlusion();
  fragColor = physicalLight() * color * vec4(1.0, 1.0, 1.0, opacity * visibility);
}
