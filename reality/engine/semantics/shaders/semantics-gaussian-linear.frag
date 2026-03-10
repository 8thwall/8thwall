#version 320 es

precision mediump float;

// max kernel size is 13
#define MAX_KERNEL_SIZE 13

in vec2 texUv;

out vec4 fragColor;

uniform sampler2D colorSampler;

uniform vec2 uvStep;

uniform int kernelSize;
uniform float kWeight[MAX_KERNEL_SIZE];

vec4 computeBlur(int step, int halfKernel) {
  float offset = float(step - halfKernel);
  vec2 sampleUv = texUv + offset * uvStep;
  vec4 c = texture(colorSampler, sampleUv);
  return kWeight[step] * c;
}

void main() {
  int s = kernelSize / 2;
  vec4 zero = vec4(0.0f, 0.0f, 0.0f, 0.0f);
  int i = 0;
  fragColor = computeBlur(i, s);
  i = 1;
  fragColor += (i < kernelSize) ? computeBlur(i, s) : zero;
  i = 2;
  fragColor += (i < kernelSize) ? computeBlur(i, s) : zero;
  i = 3;
  fragColor += (i < kernelSize) ? computeBlur(i, s) : zero;
  i = 4;
  fragColor += (i < kernelSize) ? computeBlur(i, s) : zero;
  i = 5;
  fragColor += (i < kernelSize) ? computeBlur(i, s) : zero;
  i = 6;
  fragColor += (i < kernelSize) ? computeBlur(i, s) : zero;
  i = 7;
  fragColor += (i < kernelSize) ? computeBlur(i, s) : zero;
  i = 8;
  fragColor += (i < kernelSize) ? computeBlur(i, s) : zero;
  i = 9;
  fragColor += (i < kernelSize) ? computeBlur(i, s) : zero;
  i = 10;
  fragColor += (i < kernelSize) ? computeBlur(i, s) : zero;
  i = 11;
  fragColor += (i < kernelSize) ? computeBlur(i, s) : zero;
  i = 12;
  fragColor += (i < kernelSize) ? computeBlur(i, s) : zero;
}
