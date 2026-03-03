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

//#include <thread>

#include "c8/pixels/pixels.h"
#include "c8/task-queue.h"
#include "c8/thread-pool.h"
#include "reality/engine/features/gr8cpu-interface.h"
#include "reality/engine/features/image-point.h"
#include "third_party/cvlite/core/core.hpp"
#include "third_party/cvlite/imgproc/filterengine.h"

namespace c8 {

class Gr8 : public Gr8CpuInterface {
public:
  // Choose a fixed size PATCH_SIZE at compile-time to allow stack allocation of patch-sized arrays.
  static constexpr int PATCH_SIZE = 31;

  /**
   * Retain the best nPoints from the set of keypoints, taking into account both response and
   * geometry.
   * @param quantizeTo256 Quantize value of keypoints.response to uint8_t in range [0,255]. Gain
   * ~10% perf.
   */
  static void retainBest(
    std::vector<c8cv::KeyPoint> &keypoints, int nPoints, int cols, bool quantizeTo256);

  /** @brief The ORB constructor

  @param nfeatures The maximum number of features to retain.
  @param scaleFactor Pyramid decimation ratio, greater than 1. scaleFactor==2 means the classical
  pyramid, where each next level has 4x less pixels than the previous, but such a big scale factor
  will degrade feature matching scores dramatically. On the other hand, too close to 1 scale factor
  will mean that to cover certain scale range you will need more pyramid levels and so the speed
  will suffer.
  @param nlevels The number of pyramid levels. The smallest level will have linear size equal to
  input_image_linear_size/pow(scaleFactor, nlevels).
  @param edgeThreshold This is size of the border where the features are not detected. It should
  roughly match the PATCH_SIZE parameter.
  @param firstLevel It should be 0 in the current implementation.
  @param WTA_K The number of points that produce each element of the oriented BRIEF descriptor. The
  default value 2 means the BRIEF where we take a random point pair and compare their brightnesses,
  so we get 0/1 response. Other possible values are 3 and 4. For example, 3 means that we take 3
  random points (of course, those point coordinates are random, but they are generated from the
  pre-defined seed, so each element of BRIEF descriptor is computed deterministically from the pixel
  rectangle), find point of maximum brightness and output index of the winner (0, 1 or 2). Such
  output will occupy 2 bits, and therefore it will need a special variant of Hamming distance,
  denoted as NORM_HAMMING2 (2 bits per bin). When WTA_K=4, we take 4 random points to compute each
  bin (that will also occupy 2 bits with possible values 0, 1, 2 or 3).
  @param scoreType The default HARRIS_SCORE means that Harris algorithm is used to rank features
  (the score is written to KeyPoint::score and is used to retain best nfeatures features);
  FAST_SCORE is alternative value of the parameter that produces slightly less stable keypoints,
  but it is a little faster to compute.
  @param fastThreshold
   */
  static Gr8 create(
    int nfeatures = 500,
    float scaleFactor = 1.2f,
    int nlevels = 8,
    int edgeThreshold = 31,
    int firstLevel = 0,
    int wtaK = 2,
    int scoreType = Gr8CpuInterface::HARRIS_SCORE,
    int fastThreshold = 20) {

    // pre-compute the end of a row in a circular patch
    constexpr int HALF_PATCH_SIZE = PATCH_SIZE / 2;
    std::vector<int> umax(HALF_PATCH_SIZE + 2);

    int v, v0, vmax = c8cv::cvFloor(HALF_PATCH_SIZE * std::sqrt(2.f) / 2 + 1);
    int vmin = c8cv::cvCeil(HALF_PATCH_SIZE * std::sqrt(2.f) / 2);
    for (v = 0; v <= vmax; ++v)
      umax[v] = c8cv::cvRound(std::sqrt((double)HALF_PATCH_SIZE * HALF_PATCH_SIZE - v * v));

    // Make sure we are symmetric
    for (v = HALF_PATCH_SIZE, v0 = 0; v >= vmin; --v) {
      while (umax[v0] == umax[v0 + 1]) ++v0;
      umax[v] = v0;
      ++v0;
    }

    return Gr8(
      nfeatures,
      scaleFactor,
      nlevels,
      edgeThreshold,
      firstLevel,
      wtaK,
      scoreType,
      fastThreshold,
      umax);
  }

  static Gr8* createPtr(
    int nfeatures = 500,
    float scaleFactor = 1.2f,
    int nlevels = 8,
    int edgeThreshold = 31,
    int firstLevel = 0,
    int wtaK = 2,
    int scoreType = Gr8CpuInterface::HARRIS_SCORE,
    int fastThreshold = 20) {

    // pre-compute the end of a row in a circular patch
    constexpr int HALF_PATCH_SIZE = PATCH_SIZE / 2;
    std::vector<int> umax(HALF_PATCH_SIZE + 2);

    int v, v0, vmax = c8cv::cvFloor(HALF_PATCH_SIZE * std::sqrt(2.f) / 2 + 1);
    int vmin = c8cv::cvCeil(HALF_PATCH_SIZE * std::sqrt(2.f) / 2);
    for (v = 0; v <= vmax; ++v)
      umax[v] = c8cv::cvRound(std::sqrt((double)HALF_PATCH_SIZE * HALF_PATCH_SIZE - v * v));

    // Make sure we are symmetric
    for (v = HALF_PATCH_SIZE, v0 = 0; v >= vmin; --v) {
      while (umax[v0] == umax[v0 + 1]) ++v0;
      umax[v] = v0;
      ++v0;
    }

    return new Gr8(
      nfeatures,
      scaleFactor,
      nlevels,
      edgeThreshold,
      firstLevel,
      wtaK,
      scoreType,
      fastThreshold,
      umax);
  }

  explicit Gr8(
    int nfeatures,
    float scaleFactor,
    int nlevels,
    int edgeThreshold,
    int firstLevel,
    int wtaK,
    int scoreType,
    int fastThreshold,
    std::vector<int> umax)
      : nfeatures_(nfeatures),
        scaleFactor_(scaleFactor),
        nlevels_(nlevels),
        edgeThreshold_(edgeThreshold),
        firstLevel_(firstLevel),
        wtaK_(wtaK),
        scoreType_(scoreType),
        fastThreshold_(fastThreshold),
        umax_(umax),
        threadPool_(new ThreadPool(std::max(2u, std::thread::hardware_concurrency()) - 1u)) {

    // Filtering is done in parallel theads, each thread requires separate filter instance.
    filterNumSegments_ = 8 /* max num segments */;
    filterOverlapPx_ = 7;
    for (int i = 0; i < filterNumSegments_ * nlevels_; i++) {
      filters_.push_back(c8cv::createGaussianFilter(
        0 /* matType */,
        c8cv::Size(7, 7) /* kernelSize */,
        2 /* sigmaX */,
        2 /* sigmaY */,
        c8cv::BORDER_REFLECT_101 & ~c8cv::BORDER_ISOLATED));
    }
  }

  void setMaxFeatures(int maxFeatures) { nfeatures_ = maxFeatures; }
  int getMaxFeatures() const { return nfeatures_; }

  void setScaleFactor(double scaleFactor) { scaleFactor_ = scaleFactor; }
  double getScaleFactor() const { return scaleFactor_; }

  void setNLevels(int nlevels) { nlevels_ = nlevels; }
  int getNLevels() const { return nlevels_; }

  void setEdgeThreshold(int edgeThreshold) { edgeThreshold_ = edgeThreshold; }
  int getEdgeThreshold() const { return edgeThreshold_; }

  void setFirstLevel(int firstLevel) { firstLevel_ = firstLevel; }
  int getFirstLevel() const { return firstLevel_; }

  void setWTA_K(int wtaK) { wtaK_ = wtaK; }
  int getWTA_K() const { return wtaK_; }

  void setScoreType(int scoreType) { scoreType_ = scoreType; }
  int getScoreType() const { return scoreType_; }

  static constexpr int getPatchSize() { return PATCH_SIZE; }

  void setFastThreshold(int fastThreshold) { fastThreshold_ = fastThreshold; }
  int getFastThreshold() const { return fastThreshold_; }

  // returns the descriptor size in bytes
  static constexpr int descriptorSize() { return Gr8CpuInterface::kBytes; }
  // returns the descriptor type
  int descriptorType() const;
  // returns the default norm type
  int defaultNorm() const;

  // Compute the Gr8 features and descriptors in an image.
  ImagePoints detectAndCompute(YPlanePixels in) override;

  // Compute the Gr8 features and descriptors on an image
  void detectAndCompute(
    c8cv::InputArray image,
    std::vector<c8cv::KeyPoint> &keypoints,
    c8cv::OutputArray descriptors);

  Gr8(Gr8 &&) = default;

  // Use factory method to create Gr8 features.
  Gr8() = delete;
  Gr8 &operator=(Gr8 &&) = delete;
  Gr8(const Gr8 &) = delete;
  Gr8 &operator=(const Gr8 &) = delete;

private:
  int nfeatures_;
  double scaleFactor_;
  int nlevels_;
  int edgeThreshold_;
  int firstLevel_;
  int wtaK_;
  int scoreType_;
  int fastThreshold_;
  std::vector<int> umax_;

  int filterNumSegments_;
  int filterOverlapPx_;
  std::vector<c8cv::Ptr<c8cv::FilterEngine>> filters_;

  std::unique_ptr<ThreadPool> threadPool_;
  TaskQueue taskQueue_;

  c8cv::Mat imagePyramid_, tmpPyramid_;
  std::vector<c8cv::Point> pattern_;

  void initGr8Pattern();
};

}  // namespace c8
