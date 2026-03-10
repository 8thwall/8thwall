#version 320 es

precision mediump float;

uniform sampler2D texSampler;
uniform vec2 scale;
uniform vec4 roi;
in vec2 texUv;

float texValue(vec2 p) {
  vec2 pix = roi.zw * texUv + roi.xy + p;
  return texture(texSampler, scale * pix).r;
}

out vec4 fragColor;
void main() {
  // Blur with 3x3 Gaussian kernel, sigma = 0.72px.
  float blur =
    0.054659 * texValue(vec2(-1.0, -1.0)) +
    0.124474 * texValue(vec2( 0.0, -1.0)) +
    0.054659 * texValue(vec2( 1.0, -1.0)) +
    0.124474 * texValue(vec2(-1.0,  0.0)) +
    0.283464 * texValue(vec2( 0.0,  0.0)) +
    0.124474 * texValue(vec2( 1.0,  0.0)) +
    0.054659 * texValue(vec2(-1.0,  1.0)) +
    0.124474 * texValue(vec2( 0.0,  1.0)) +
    0.054659 * texValue(vec2( 1.0,  1.0));
  fragColor =  vec4(blur, 0.0, 0.0, 1.0);
}
