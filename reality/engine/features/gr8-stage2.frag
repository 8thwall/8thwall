#version 320 es
#extension GL_GOOGLE_include_directive: enable

precision mediump float;

#include "reality/engine/features/gr8-defs.h"
#include "reality/engine/features/gr8-moment-packing.glsl"

in vec2 texUv;
uniform sampler2D sampler;
uniform vec2 scale;

float texValue(vec2 p) { return texture(sampler, texUv + p * scale).x; }

vec4 texRGBA(vec2 p) { return texture(sampler, texUv + p * scale); }

vec4 min9Quad(vec4 da, vec4 db, vec4 dc) {
  vec2 innerMin2 = min(db.xy, db.zw);
  float innerMin = min(innerMin2.x, innerMin2.y);
  vec4 outerMin4 = min(vec4(da.y, dc.y, da.z, dc.z), vec4(da.ww, dc.xx));
  vec2 outerMin = min(min(outerMin4.xy, outerMin4.zw), innerMin);
  return min(vec4(da.xz, dc.yw), vec4(outerMin, outerMin));
}

float max9Full(vec4 a, vec4 b, vec4 c, vec4 d) {
  vec4 m =
    max(min9Quad(a, b, c), max(min9Quad(b, c, d), max(min9Quad(c, d, a), min9Quad(d, a, b))));
  vec2 m2 = max(m.xy, m.zw);
  return max(m2.x, m2.y);
}

float scoreGr8(vec4 d0, vec4 d4, vec4 d8, vec4 d12) {
  return max(max9Full(d0, d4, d8, d12), max9Full(-d0, -d4, -d8, -d12));
}

float fastValue(float c, vec4 v0, vec4 v4, vec4 v8, vec4 v12) {
  return scoreGr8(v0 - c, v4 - c, v8 - c, v12 - c);
}

out vec4 color;
void main() {
  float v02 = texValue(vec2(-1.0, -3.0));
  float v03 = texValue(vec2(0.0, -3.0));
  float v04 = texValue(vec2(1.0, -3.0));
  float v11 = texValue(vec2(-2.0, -2.0));
  float v15 = texValue(vec2(2.0, -2.0));
  float v20 = texValue(vec2(-3.0, -1.0));
  float v26 = texValue(vec2(3.0, -1.0));
  float v30 = texValue(vec2(-3.0, 0.0));
  float v31 = texValue(vec2(-2.0, 0.0));
  float v32 = texValue(vec2(-1.0, 0.0));
  vec4 center = texRGBA(vec2(0.0, 0.0));
  float v34 = texValue(vec2(1.0, 0.0));
  float v35 = texValue(vec2(2.0, 0.0));
  float v36 = texValue(vec2(3.0, 0.0));
  float v40 = texValue(vec2(-3.0, 1.0));
  float v46 = texValue(vec2(3.0, 1.0));
  float v51 = texValue(vec2(-2.0, 2.0));
  float v55 = texValue(vec2(2.0, 2.0));
  float v62 = texValue(vec2(-1.0, 3.0));
  float v63 = texValue(vec2(0.0, 3.0));
  float v64 = texValue(vec2(1.0, 3.0));

  color.r = center.r;
  color.g = dot(vec4(0.070159, 0.131075, 0.190713, 0.216106), vec4(v30, v31, v32, center.r))
    + dot(vec3(0.190713, 0.131075, 0.070159), vec3(v34, v35, v36));

#if C8_USE_ALTERNATE_FEATURE_SCORE
  vec3 unpacked = unpackMoment(center.gba);
  float ix2 = unpacked.x;
  float iy2 = unpacked.y;
  float ixy = unpacked.z;

  float trace = ix2 + iy2;
  float det = ix2 * iy2 - ixy * ixy;
  // float score = 256.0  (det - 0.04 * trace * trace); // Harris Score.
  float sq = sqrt(trace * trace - 4.0 * det);
  float eig0 = 0.5 * (trace + sq);
  float eig1 = 0.5 * (trace - sq);

  float score = sqrt(35.2 * min(eig0, eig1));
#else
  float score = fastValue(
    center.r,
    vec4(v30, v20, v11, v02),
    vec4(v03, v04, v15, v26),
    vec4(v36, v46, v55, v64),
    vec4(v63, v62, v51, v40));
#endif
  // Use two channels to output a 16-bit score.
  float clmp = clamp(score, 0.0, 0.99999);
  float scaleUp = 255.0 * clmp;
  float lowBits = scaleUp - floor(scaleUp);
  const float UNIT = 1.0 / 255.0;
  color.ba = vec2(clmp - UNIT * lowBits, lowBits);
}
