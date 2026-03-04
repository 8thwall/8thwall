#pragma once

#include "c8/c8-log.h"
#include "c8/pixels/opengl/egl.h"
#include "c8/pixels/opengl/gl-error.h"
#include "c8/pixels/opengl/gl.h"
#include "c8/pixels/opengl/glext.h"

#define DEBUG_MOTION_ESTIMATION false

namespace {

// Working with the Quest 3, the GL_QCOM_motion_estimation should be available, but for some reason
// the headers don't seem to have the extension. This is a workaround to get the extension values.

// Example of Meta doing something similar:
// https://github.com/meta-quest/Meta-OpenXR-SDK/blob/a2d59b10014bc3cfa5f201dd28eff765ddf6dfce/
// Samples/XrSamples/XrCompositor_NativeActivity/Src/XrCompositor_NativeActivity.c#L79
#ifndef GL_QCOM_motion_estimation
#define GL_QCOM_motion_estimation 1
#define GL_MOTION_ESTIMATION_SEARCH_BLOCK_X_QCOM 0x8C90
#define GL_MOTION_ESTIMATION_SEARCH_BLOCK_Y_QCOM 0x8C91

typedef void(GL_APIENTRY *PFNGLTEXESTIMATEMOTIONQCOMPROC)(GLuint ref, GLuint target, GLuint output);

typedef void(GL_APIENTRY *PFNGLTEXESTIMATEMOTIONREGIONSQCOMPROC)(
  GLuint ref, GLuint target, GLuint output, GLuint mask);

#endif

}  // namespace

namespace c8 {

/**
 * Get the output dimensions based on the input dimensions and the internal search block size.
 * Returns a pair of integers representing the width and height of the output texture.
 * WARNING: This forces cpu / gpu synchronization, so it should be used sparingly.
 */
std::pair<int, int> texEstimateMotionOutputDimensions(int inputWidth, int inputHeight) {
  int searchBlockSizeX, searchBlockSizeY;

  glGetIntegerv(GL_MOTION_ESTIMATION_SEARCH_BLOCK_X_QCOM, &searchBlockSizeX);
  glGetIntegerv(GL_MOTION_ESTIMATION_SEARCH_BLOCK_Y_QCOM, &searchBlockSizeY);

  if (searchBlockSizeX == 0 || searchBlockSizeY == 0) {
    C8Log("[gl-motion-estimation] Incompatible search block size of 0");
    return {0, 0};
  }

  int outputWidth = inputWidth / searchBlockSizeX;
  int outputHeight = inputHeight / searchBlockSizeY;

  return {outputWidth, outputHeight};
}

void texEstimateMotion(GLuint refRgba, GLuint targetRgba, GLuint output) {
  // Implementation of the function
  static PFNGLTEXESTIMATEMOTIONQCOMPROC glTexEstimateMotionQCOM =
    reinterpret_cast<PFNGLTEXESTIMATEMOTIONQCOMPROC>(eglGetProcAddress("glTexEstimateMotionQCOM"));

  if (!glTexEstimateMotionQCOM) {
    // Handle the error if the function pointer is not retrieved
    C8Log("[gl-motion-estimation] Failed to get glTexEstimateMotionQCOM function pointer");
    return;
  }

#if DEBUG_MOTION_ESTIMATION
  int inputWidth, inputHeight;

#ifdef GL_ES_VERSION_3_1
  // Get the input dimensions (Both input textures should have the same dimensions)
  glBindTexture(GL_TEXTURE_2D, refRgba);
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &inputWidth);
  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &inputHeight);
#else
  // Use hardcoded values for testing
  inputWidth = 3360;
  inputHeight = 1760;
#endif

  auto outDims = texEstimateMotionOutputDimensions(inputWidth, inputHeight);

  // On Quest 3, the search block size seems to be 8x8. Resulting in an output texture that is
  // 420x220 for an input texture of 3360x1760.
  C8Log("[gl-motion-estimation] Output Dimensions: %d x %d", outDims.first, outDims.second);
#endif

  // Use glTexEstimateMotionQCOM
  glTexEstimateMotionQCOM(refRgba, targetRgba, output);
}

}  // namespace c8
