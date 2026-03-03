// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)
//
//
// Gr8 features were originally forked from OpenCV's orb features.
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install, copy or use the software.
//
//
//                           License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
// Copyright (C) 2009, Willow Garage Inc., all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice, this list of
//   conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice, this list of
//   conditions and the following disclaimer in the documentation and/or other materials provided
//   with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and any express or
// implied warranties, including, but not limited to, the implied warranties of merchantability and
// fitness for a particular purpose are disclaimed. In no event shall the Intel Corporation or
// contributors be liable for any direct, indirect, incidental, special, exemplary, or consequential
// damages (including, but not limited to, procurement of substitute goods or services; loss of use,
// data, or profits; or business interruption) however caused and on any theory of liability,
// whether in contract, strict liability, or tort (including negligence or otherwise) arising in any
// way out of the use of this software, even if advised of the possibility of such damage.

#pragma once
#include <cmath>

#include "c8/pixels/gr8-pyramid.h"
#include "c8/pixels/pixel-buffer.h"
#include "c8/task-queue.h"
#include "c8/thread-pool.h"
#include "c8/vector.h"
#include "reality/engine/features/frame-point.h"
#include "reality/engine/features/image-point.h"
#include "reality/engine/features/keypoint-queue.h"

namespace c8 {

namespace {

/* begin fastAtan2 impl from cvlite/core/mathfuncs_core.cpp */
constexpr float atan2_p1 = 0.9997878412794807f * static_cast<float>(180 / M_PI);
constexpr float atan2_p3 = -0.3258083974640975f * static_cast<float>(180 / M_PI);
constexpr float atan2_p5 = 0.1555786518463281f * static_cast<float>(180 / M_PI);
constexpr float atan2_p7 = -0.04432655554792128f * static_cast<float>(180 / M_PI);

constexpr float fastAtan2(float y, float x) {
  float ax = std::abs(x);
  float ay = std::abs(y);
  float a;
  if (ax >= ay) {
    float c = ay / (ax + static_cast<float>(DBL_EPSILON));
    float c2 = c * c;
    a = (((atan2_p7 * c2 + atan2_p5) * c2 + atan2_p3) * c2 + atan2_p1) * c;
  } else {
    float c = ax / (ay + static_cast<float>(DBL_EPSILON));
    float c2 = c * c;
    a = 90.f - (((atan2_p7 * c2 + atan2_p5) * c2 + atan2_p3) * c2 + atan2_p1) * c;
  }
  if (x < 0) {
    a = 180.f - a;
  }
  if (y < 0) {
    a = 360.f - a;
  }
  return a;
}
/* end fastAtan2 impl from cvlite/core/mathfuncs_core.cpp */

constexpr float RAD_TO_DEG = 180.f / M_PI;
// Adapted from https://developer.download.nvidia.com/cg/acos.html
constexpr float fastAcos(float x) {
  float negate = float(x < 0);
  x = std::abs(x);
  float ret = -0.0187293f;
  ret = ret * x;
  ret = ret + 0.0742610f;
  ret = ret * x;
  ret = ret - 0.2121144f;
  ret = ret * x;
  ret = ret + 1.5707288f;
  ret = ret * std::sqrt(1.0f - x);
  ret = ret - 2.0f * negate * ret;
  return (negate * 3.14159265358979f + ret) * RAD_TO_DEG;
}

}  // namespace

class Gr8Gl {
public:
  // Descriptor size.
  static constexpr int kBytes = 32;

  // Choose a fixed size PATCH_SIZE at compile-time to allow stack allocation of patch-sized arrays.
  static constexpr int PATCH_SIZE = 31;
  // TODO(nb): revert to computing angle on smaller patch when tracking is stabilized.
  // static constexpr int ANGLES_PATCH_SIZE = 7;
  static constexpr int ANGLES_PATCH_SIZE = 31;

  /**
   * Retain the best nPoints from the set of keypoints, taking into account both response and
   * geometry.
   * @param quantizeTo256 Quantize value of keypoints.response to uint8_t in range [0,255]. Gain
   * ~10% perf.
   */
  static void retainBest(
    Vector<ImagePointLocation> &keypoints,
    int nPoints,
    int cols,
    int rows,
    int edgeThreshold,
    bool quantizeTo256);

  /** @brief The ORB constructor

  @param scaleFactor Pyramid decimation ratio, greater than 1. scaleFactor==2 means the classical
  pyramid, where each next level has 4x less pixels than the previous, but such a big scale factor
  will degrade feature matching scores dramatically. On the other hand, too close to 1 scale factor
  will mean that to cover certain scale range you will need more pyramid levels and so the speed
  will suffer.
  @param nlevels The number of pyramid levels. The smallest level will have linear size equal to
  input_image_linear_size/pow(scaleFactor, nlevels).
  @param edgeThreshold This is size of the border where the features are not detected. It should
  roughly match the PATCH_SIZE parameter.
  @param WTA_K The number of points that produce each element of the oriented BRIEF descriptor. The
  default value 2 means the BRIEF where we take a random point pair and compare their brightnesses,
  so we get 0/1 response. Other possible values are 3 and 4. For example, 3 means that we take 3
  random points (of course, those point coordinates are random, but they are generated from the
  pre-defined seed, so each element of BRIEF descriptor is computed deterministically from the pixel
  rectangle), find point of maximum brightness and output index of the winner (0, 1 or 2). Such
  output will occupy 2 bits, and therefore it will need a special variant of Hamming distance,
  denoted as NORM_HAMMING2 (2 bits per bin). When WTA_K=4, we take 4 random points to compute each
  bin (that will also occupy 2 bits with possible values 0, 1, 2 or 3).
   */
  static Gr8Gl create(
    float scaleFactor = 1.44f,
    int edgeThreshold = 21,  // 18 for FEATURE_LUT and 3 for gaussian blur.
    int wtaK = 2) {

    // Precompute the corners of a 90 degree circle arc. This is used to generate a circular ROI for
    // computing the center of mass of a patch, and using it to anchor desriptor angles across
    // frames.
    constexpr int HALF_PATCH_SIZE = ANGLES_PATCH_SIZE / 2;  // 15
    Vector<int> circularMask(HALF_PATCH_SIZE + 1);          // 16

    // Compute the 45 degrees point which is used to ensure symmetry.
    int vmax = std::floor(HALF_PATCH_SIZE * std::sqrt(2.f) / 2 + 1);  // 11
    int vmin = std::ceil(HALF_PATCH_SIZE * std::sqrt(2.f) / 2);       // 11

    // Compute circle points around the first 45 degree arc.  Solve for b^2 = c^2 - a^2.
    for (int v = 0; v <= vmax; ++v) {
      circularMask[v] = std::round(std::sqrt((double)HALF_PATCH_SIZE * HALF_PATCH_SIZE - v * v));
    }

    // Compute the second half of the circle starting from the top and going to the 45 degree point
    // in the way that makes it symmetric with the first 45 degree arc.
    int v0 = 0;
    for (int v = HALF_PATCH_SIZE; v >= vmin; --v) {
      while (circularMask[v0] == circularMask[v0 + 1]) {
        ++v0;
      }
      circularMask[v] = v0;
      ++v0;
    }

    // When ANGLES_PATCH_SIZE = 31, produces:
    // circularMask[16] = [ 15, 15, 15, 15, 14, 14, 14, 13, 13, 12, 11, 10, 9, 8, 6, 3]

    // When ANGLES_PATCH_SIZE = 7, produces:
    // circularMask[16] = [ 3 3 2 1 ]

    return Gr8Gl(scaleFactor, edgeThreshold, wtaK, circularMask);
  }

  explicit Gr8Gl(float scaleFactor, int edgeThreshold, int wtaK, const Vector<int> &circularMask)
      : scaleFactor_(scaleFactor),
        edgeThreshold_(edgeThreshold),
        wtaK_(wtaK),
        circularMask_(circularMask),
        threadPool_(new ThreadPool(std::max(2u, std::thread::hardware_concurrency()) - 1u)) {}

  ImagePoints detectAndCompute(
    const Gr8Pyramid &pyramid,
    const FrameWithPoints &frame,
    const DetectionConfig &config = {},
    float gravityAngleOffset = 0.0f);
  ImagePoints detectAndCompute(const Gr8Pyramid &pyramid, int numFeatures = -1);

  Vector<ImagePoints> detectAndComputeRois(
    const Gr8Pyramid &pyramid,
    const FrameWithPoints &frame,
    const DetectionConfig &config = {},
    float gravityAngleOffset = 0.0f);
  Vector<ImagePoints> detectAndComputeRois(const Gr8Pyramid &pyramid);

  static constexpr int descriptorSize() { return kBytes; }
  int edgeThreshold() const { return edgeThreshold_; }

  Vector<size_t> computeOrbKeypoints(
    const Gr8Pyramid &pyramid,
    const DetectionConfig &config,
    Vector<ImagePointLocation> *pts) const;

  Vector<size_t> computeGorbKeypoints(
    const Gr8Pyramid &pyramid,
    const DetectionConfig &config,
    const FrameWithPoints &frame,
    const Vector<HPoint3> &raysInCam,
    const Vector<HPoint3> &raysInWorld,
    Vector<ImagePointLocation> *pts,
    float gravityAngleOffset) const;

  static ImagePoints toPointsWithEmptyDescriptors(const Vector<ImagePointLocation> &pts);

  void computeGr8Descriptors(
    const Gr8Pyramid &pyramid,
    const Vector<size_t> &kptIndices,
    bool useGravityAngle,
    ImagePoints *pointsWithDescriptors);

  Gr8Gl(Gr8Gl &&) = default;
  Gr8Gl &operator=(Gr8Gl &&) = default;

  // Use factory method to create Gr8 features.
  Gr8Gl() = delete;
  Gr8Gl(const Gr8Gl &) = delete;
  Gr8Gl &operator=(const Gr8Gl &) = delete;

private:
  double scaleFactor_;
  int edgeThreshold_;
  int wtaK_;
  Vector<int> circularMask_;

  std::unique_ptr<ThreadPool> threadPool_;
  TaskQueue taskQueue_;

  Vector<Vector<ImagePointLocation>> keypointLevels_;

  // Subcomponent methods of detectAndCompute.
  Vector<ImagePointLocation> computeKeyPoints(const Gr8Pyramid &pyramid, int nFeatures);
  Vector<ImagePointLocation> computeKeyPointsRoi(const Gr8Pyramid &pyramid, int roi);
  static void computeRayVectors(
    const Gr8Pyramid &pyramid,
    const FrameWithPoints &frame,
    const Vector<ImagePointLocation> &ptImageLocations,
    Vector<HPoint3> *raysInCam,
    Vector<HPoint3> *raysInWorld);
  static void computeGravityPortions(
    const Vector<HPoint3> &raysInWorld, Vector<ImagePointLocation> *pts);
  static void scaleToBaseImage(const Gr8Pyramid &pyramid, ImagePoints *pointsWithDescriptors);
};

}  // namespace c8
