#version 320 es

precision mediump float;

in vec2 texUv;

uniform sampler2D colorSampler;
uniform sampler2D alphaSampler;

uniform int haveSeenSky;
uniform int flipAlphaMask;

// See comment 2 in semantics-sky-shader.h.
uniform float smoothness;

out vec4 fragColor;

void main() {
  fragColor = texture(colorSampler, texUv);
  float blurEdge = clamp(fragColor.r - 0.5f, 0.0f, 1.0f) * 2.0f;
  vec4 rawSemColor = texture(alphaSampler, texUv);
  float isMarked = rawSemColor.b;
  float sky = rawSemColor.r * (1.0f - smoothness) + blurEdge * smoothness;
  fragColor = vec4(sky, sky, isMarked, 1.0f);
  // See Comment 1 in semantics-sky-shader.h.
  if (sky > 0.0f) {
    if (haveSeenSky > 0 && isMarked == 0.0f) {
      fragColor = vec4(1.0, 1.0, isMarked, 1.0f);
    }
    if (haveSeenSky == 0 && isMarked < 1.0f) {
      fragColor = vec4(0.0, 0.0, isMarked, 1.0f);
    }
  }
  if (flipAlphaMask > 0) {
    fragColor = vec4(1.0f - fragColor.r, 1.0f - fragColor.g, isMarked, 1.0f);
  }
}
