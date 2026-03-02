#version 320 es
precision mediump float;

out vec4 fragColor;

uniform float near;
uniform float far;

float linearizeDepth(float depth) {
  float z = depth * 2.0 - 1.0;  // back to NDC
  return (2.0 * near * far) / (far + near - z * (far - near));
}

// Encode a float in the range [0,1] into a vec4 of int
// See packed.cc for a C++ implementation with unit test
vec4 encodeFloat01(in float value) {
  vec4 encode = fract(value * vec4(1.0, 256.0, 256.0 * 256.0, 256.0 * 256.0 * 256.0));
  // NOTE(dat): this doesn't multiply by 256 because this is implicit on float conversion.
  // TODO(dat): Confirm that this is multiplied by 256 implicitly and not 255. Adjust packed.cc BASE
  return vec4(encode.xyz - encode.yzw / 256.0, encode.w);
}

void main() {
  float depth = linearizeDepth(gl_FragCoord.z);
  float depthIn01 = depth / (far - near);
  fragColor = encodeFloat01(depthIn01);
  // values in fragColor is mapped to uint8_t on reading
  // 0 => 0
  // 0.5 => 128
  // 1 => 255
  // 2 => 255
}
