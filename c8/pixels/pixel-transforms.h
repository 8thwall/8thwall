// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)
//
// Operations that produce images from images.

#pragma once

#include <array>

#include "c8/hmatrix.h"
#include "c8/pixels/pixels.h"
#include "c8/task-queue.h"
#include "c8/thread-pool.h"

namespace c8 {

namespace ColorMat {
// YCbCr color matrix conversion functions. See https://en.wikipedia.org/wiki/YCbCr
// Wikipedia variously refers to variants as "analog" and "digital" or "full" and "swing" to refer
// whether or not these use the full range of 0-255 or a compressed range of 16-235 for y.
HMatrix rgbToYCbCrAnalog(float kr, float kg, float kb);
HMatrix rgbToYCbCrDigital(float kr, float kg, float kb);
HMatrix rgbToYCbCrBt601Digital();
HMatrix rgbToYCbCrBt709Digital();
HMatrix rgbToYCbCrBt2020NcDigital();
HMatrix rgbToYCbCrJpg();
}  // namespace ColorMat

// Copy pixels from the source image to the dest image.
void copyPixels(const ConstPixels &src, Pixels *dest);
void copyFloatPixels(const ConstFloatPixels &src, FloatPixels *dest);

void downsize(const ConstOneChannelPixels &src, OneChannelPixels *dest);
void downsize(const ConstTwoChannelPixels &src, TwoChannelPixels *dest);
void downsize(const ConstFourChannelPixels &src, FourChannelPixels *dest);

// Merge pixels from two one-channel buffers into one two-channel buffer.
void mergePixels(
  const ConstOneChannelPixels &src1, const ConstOneChannelPixels &src2, TwoChannelPixels *dest);

void mergePixels(
  const ConstUSkipPlanePixels &src1, const ConstVSkipPlanePixels &src2, TwoChannelPixels *dest);

// Split 2-channel UV into two 1-channel plane pixels. UPlanePixels and VPlanePixels have half the
// height of the original src.
void splitPixels(ConstUVPlanePixels src, UPlanePixels dest1, VPlanePixels dest2);

// Split 4-channel image into 1, 2, 3, or 4 channels.
template <int N>
void splitPixels(const ConstFourChannelPixels &src, OneChannelPixels *c[N]);

// Split 4-channel image into 1, 2, 3, or 4 channels.
// Use the above version, unless you don't know the output channel count at build time.
void splitPixels(
  const ConstFourChannelPixels &src,
  OneChannelPixels *c1,
  OneChannelPixels *c2,
  OneChannelPixels *c3,
  OneChannelPixels *c4);

// Rotate the source image 90 degrees clockwise, into the second image. The second image must have
// width and height reversed from the first image.
void rotate90Clockwise(const ConstOneChannelPixels &src, OneChannelPixels *dest);
void rotate180Clockwise(const ConstOneChannelPixels &src, OneChannelPixels *dest);
void rotate270Clockwise(const ConstOneChannelPixels &src, OneChannelPixels *dest);

void rotate90Clockwise(const ConstTwoChannelPixels &src, TwoChannelPixels *dest);
void rotate90Clockwise(
  const ConstUSkipPlanePixels &srcU, const ConstVSkipPlanePixels &srcV, TwoChannelPixels *dest);
void rotate90Clockwise(
  const ConstUPlanePixels &srcU, const ConstVPlanePixels &srcV, TwoChannelPixels *dest);

void rotate180Clockwise(const ConstTwoChannelPixels &src, TwoChannelPixels *dest);
void rotate180Clockwise(
  const ConstUSkipPlanePixels &srcU, const ConstVSkipPlanePixels &srcV, TwoChannelPixels *dest);
void rotate180Clockwise(
  const ConstUPlanePixels &srcU, const ConstVPlanePixels &srcV, TwoChannelPixels *dest);

void rotate270Clockwise(const ConstTwoChannelPixels &src, TwoChannelPixels *dest);
void rotate270Clockwise(
  const ConstUSkipPlanePixels &srcU, const ConstVSkipPlanePixels &srcV, TwoChannelPixels *dest);
void rotate270Clockwise(
  const ConstUPlanePixels &srcU, const ConstVPlanePixels &srcV, TwoChannelPixels *dest);

void rotate90Clockwise(const ConstFourChannelPixels &src, FourChannelPixels *dest);
void rotateFloat90Clockwise(const ConstOneChannelFloatPixels &src, OneChannelFloatPixels *dest);
void rotate270Clockwise(const ConstFourChannelPixels &src, FourChannelPixels *dest);

// Rotate the source image 90 degrees clockwise, into the dest image. The dest image must have
// a width that is less than or equal to the sources's height, and a height that is less than or
// equal to the source's width.
void downsizeAndRotate90Clockwise(const ConstOneChannelPixels &src, OneChannelPixels *dest);
void downsizeAndRotate90Clockwise(const ConstTwoChannelPixels &src, TwoChannelPixels *dest);
void downsizeAndRotate90Clockwise(
  const ConstUSkipPlanePixels &srcU, const ConstVSkipPlanePixels &srcV, TwoChannelPixels *dest);
void downsizeAndRotate90Clockwise(
  const ConstUPlanePixels &srcU, const ConstVPlanePixels &srcV, TwoChannelPixels *dest);

// Compute the histogram of a one channel image.
std::array<int32_t, 256> computeHistogram(ConstOneChannelPixels &src);

std::array<float, 1> meanPixelValue(ConstOneChannelPixels src);
std::array<float, 2> meanPixelValue(ConstTwoChannelPixels src);
std::array<float, 3> meanPixelValue(ConstThreeChannelPixels src);
std::array<float, 4> meanPixelValue(ConstFourChannelPixels src);

static constexpr uint8_t EXPOSURE_SCORE_LOW = 25u;
static constexpr uint8_t EXPOSURE_SCORE_HIGH = 230u;
// Estimate the exposure score of a one channel image.
float estimateExposureScore(
  ConstOneChannelPixels &src, TaskQueue *taskQueue, ThreadPool *threadPool, int numChannels);
float estimateExposureScoreNEON(ConstOneChannelPixels &src);

void flipVertical(const ConstPixels &src, Pixels *dest);

// Convert YUV to RGBA8888.
void yuvToRgb(
  const ConstYPlanePixels &srcY, const ConstUVPlanePixels &srcUV, RGBA8888PlanePixels *dest);
void yuvToRgb(
  const ConstYPlanePixels &srcY,
  const ConstUPlanePixels &srcU,
  const ConstVPlanePixels &srcV,
  RGBA8888PlanePixels *dest);
void bt709ToRgbHighPrecision(
  const ConstYPlanePixels &srcY,
  const ConstUPlanePixels &srcU,
  const ConstVPlanePixels &srcV,
  RGBA8888PlanePixels *dest);

void yToRgb(const ConstYPlanePixels &srcY, RGBA8888PlanePixels *dest);

// Convert YUV to BGR888.
void yuvToBgr(
  const ConstYPlanePixels &srcY, const ConstUVPlanePixels &srcUV, BGR888PlanePixels *dest);

// Convert RGBA8888 to YUVA8888.
void rgbToYuv(const ConstRGBA8888PlanePixels &src, YUVA8888PlanePixels *dest);

// Multiply a 3x4 matrix by [r, g, b, 1] to produce [r', g', b'], and keep a from the original.
void applyColorMatrixKeepAlpha(
  const HMatrix &mat, const ConstFourChannelPixels &src, FourChannelPixels *dest);

// Convert YUVA8888 to Y and UV.
void yuvToPlanarYuv(
  const ConstYUVA8888PlanePixels &src, YPlanePixels *yDest, UVPlanePixels *uvDest);

// Convert RGB888 to grayscale Y.
void rgbToGray(const ConstRGB888PlanePixels &src, YPlanePixels *dest);

// Convert RGBA8888 to grayscale Y.
void rgbToGray(const ConstRGBA8888PlanePixels &src, YPlanePixels *dest);

// Convert BGR888 to grayscale Y.
void bgrToGray(const ConstBGR888PlanePixels &src, YPlanePixels *dest);

// Convert BGR888 to RGBA888.
void bgrToRgba(const ConstBGR888PlanePixels &src, RGBA8888PlanePixels *dest);

// Convert RGB888 to BGR888.
void rgbToBgr(const ConstRGB888PlanePixels &src, BGR888PlanePixels *dest);

// Convert RGBA8888 to BGR888.
void rgbaToBgr(const ConstRGBA8888PlanePixels &src, BGR888PlanePixels *dest);

// Convert RGBA8888 to BGRA8888.
void rgbaToBgra(const ConstRGBA8888PlanePixels &src, BGRA8888PlanePixels *dest);

// Convert float pixels to a colormapped (turbo, viridis, etc) RGBA8888.
void floatToRgba(
  const ConstFloatPixels &src,
  RGBA8888PlanePixels *dest,
  const uint8_t colormap[256][3],
  float min,
  float max);

// Convert float pixels to RGBA8888 grayscale.
void floatToRgbaGray(const ConstFloatPixels &src, RGBA8888PlanePixels *dest, float min, float max);

// Convert float pixels to a colormapped (turbo, viridis, etc) RGBA8888 and remove letterbox
// paddings
void floatToRgbaRemoveLetterbox(
  const ConstFloatPixels &src,
  RGBA8888PlanePixels *dest,
  const uint8_t colormap[256][3],
  float min,
  float max);

// Convert float pixels to RGBA8888 grayscale and remove letterbox paddings
void floatToRgbaGrayRemoveLetterbox(
  const ConstFloatPixels &src, RGBA8888PlanePixels *dest, float min, float max);

// Rescale single channel float pixels from [srcMin, srcMax] to [0.0, 1.0]
void floatToOneChannelFloatRescale0To1(
  const ConstOneChannelFloatPixels &src, OneChannelFloatPixels *dest, float srcMin, float srcMax);

void fill(uint8_t a, OneChannelPixels *dest);
void fill(uint8_t a, uint8_t b, TwoChannelPixels *dest);
void fill(uint8_t a, uint8_t b, uint8_t c, ThreeChannelPixels *dest);
void fill(uint8_t a, uint8_t b, uint8_t c, uint8_t d, FourChannelPixels *dest);

YPlanePixels crop(YPlanePixels src, int h, int w, int y, int x);
RGBA8888PlanePixels crop(RGBA8888PlanePixels src, int h, int w, int y, int x);

bool isAllZeroes(ConstFourChannelPixels src, int channel);

// compute the letterbox dimensions after gpu-pixels-resizer operation
// with the same aspect ratio as input
//
// @param inputWidth input width before gpu resizer
// @param inputHeight input height before gpu resizer
// @param outputWidth resizer output width
// @param outputHeight resizer output height
// @param letterboxWidth result width without letterbox padding
// @param letterboxHeight result height without letterbox padding
void letterboxDimensionSameAspectRatio(
  int inputWidth,
  int inputHeight,
  int outputWidth,
  int outputHeight,
  int *letterboxWidth,
  int *letterboxHeight);

// Compute image dimension if try to crop the image to desired aspect ratio
//
// @param inputWidth input width of original image
// @param inputHeight input height of original image
// @param aspectRatio desired aspect ratio
// @param cropWidth result width after cropping
// @param cropHeight result height after cropping
void cropDimensionByAspectRatio(
  int inputWidth, int inputHeight, float aspectRatio, int *cropWidth, int *cropHeight);

// convert RGBA8888 pixels to RGB F32, normalized to [-1, 1]
void toLetterboxRGBFloatN1to1(
  ConstRGBA8888PlanePixels src, const int dstCols, const int dstRows, float *dst);
// convert RGBA8888 pixels to RGB F32, normalized to [0, 1]
void toLetterboxRGBFloat0To1(
  ConstRGBA8888PlanePixels src, const int dstCols, const int dstRows, float *dst);
// convert RGBA8888 pixels to RGB F32, flip x, and normalized to [0, 1]
void toLetterboxRGBFloat0To1FlipX(
  ConstRGBA8888PlanePixels src, const int dstCols, const int dstRows, float *dst);

// Stretch depthBuffer into visible range.
// Stretch the values such that [min, max] maps to [0, 255].
// @param src input float depth
// @param dst output depth value stretched
// @param upperBound val > upperBound is set to 0.
void stretchDepthValues(
  ConstDepthFloatPixels src, RGBA8888PlanePixels dst, float upperBound = 1000.f);

// Decode a float that has been encoded into rgba uint8_t channels. Extend it to [near, far] from
// [0, 1]. See depth.frag for encoding routine that takes a float in [0, 1] and encode in four
// channels of uint8_t.
void decodeToDepth(
  ConstRGBA8888PlanePixels encodedFloat, DepthFloatPixels decodedFloat, float near, float far);

}  // namespace c8
