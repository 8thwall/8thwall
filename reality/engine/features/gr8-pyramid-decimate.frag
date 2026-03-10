#version 320 es

precision mediump float;

in vec2 texUv;
uniform sampler2D sampler;
uniform vec4 roi;
uniform vec2 scale;
float texValue(vec2 p) {
  return texture(sampler, scale * p).r;
}

out vec4 fragColor;
void main() {
  // Do bilinear sampling since for non-power-of-two decimation.

  // Get the texUv coordinate in pixel units.
  vec2 pix = roi.zw * texUv + roi.xy;

  // Get the top-left pixel to sample.
  vec2 ltPix = floor(pix);

  // Get the frac values.
  vec2 fraction = pix - ltPix;

  float l = ltPix.x;
  float t = ltPix.y;
  float r = l + 1.0;
  float b = t + 1.0;

  float v00 = texValue(vec2(l, t));
  float v10 = texValue(vec2(r, t));
  float v01 = texValue(vec2(l, b));
  float v11 = texValue(vec2(r, b));

  // First linear interpolate x, then interpolate y.
  float tx = mix(v00, v10, fraction.x);
  float bx = mix(v01, v11, fraction.x);
  float val = mix(tx, bx, fraction.y);

  fragColor = vec4(val, 0.0, 0.0, 1.0);
}
