#ifndef BZL_HELLOBUILD_GLSL_SHARED_GLSL_
#define BZL_HELLOBUILD_GLSL_SHARED_GLSL_

vec4 strengthenRed(vec4 inColor) {
  vec4 color;
  color.r = 1.5 * inColor.r;
  color.gb = 0.8 * inColor.gb;
  color.a = inColor.a;
  return color;
}

#endif  // BZL_HELLOBUILD_GLSL_SHARED_GLSL_
