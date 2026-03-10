#version 320 es

precision mediump float;

uniform sampler2D sampler;
uniform vec2 scale;
in vec2 texUv;

vec3 texValueRGB(vec2 p) {
  return texture(sampler, scale * p + texUv).rgb;
}

vec3 blur3x3() {
  // Sigma=0.75.
  const float p00 = 0.268958;
  const float p11 = 0.124827;
  const float p22 = 0.057934;
  return
    p22 * texValueRGB(vec2(-1.0, -1.0)) + p11 * texValueRGB(vec2(0.0, -1.0)) + p22 * texValueRGB(vec2(1.0, -1.0)) +
    p11 * texValueRGB(vec2(-1.0,  0.0)) + p00 * texValueRGB(vec2(0.0,  0.0)) + p11 * texValueRGB(vec2(1.0,  0.0)) +
    p22 * texValueRGB(vec2(-1.0,  1.0)) + p11 * texValueRGB(vec2(0.0,  1.0)) + p22 * texValueRGB(vec2(1.0,  1.0));
}

out vec4 fragColor;
void main() {
  fragColor.rgb = blur3x3();
  fragColor.a = 1.0;
}
