#ifndef REALITY_ENGINE_FEATURES_GR8_MOMENT_
#define REALITY_ENGINE_FEATURES_GR8_MOMENT_

#include "reality/engine/features/gr8-moment-packing.glsl"

in vec2 texUv;
uniform sampler2D sampler;
uniform vec2 scale;
vec4 texRGBA(vec2 p) {
  vec4 tex = texture(sampler, scale * p + texUv);
  tex.gba = unpackMoment(tex.gba);
  return tex;
}

#endif  // REALITY_ENGINE_FEATURES_GR8_MOMENT_
