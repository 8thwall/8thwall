// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)
//
// Gr8 features were originally forked from OpenCV's orb features.
//
// Software License Agreement (BSD License)
//
//  Copyright (c) 2009, Willow Garage, Inc.
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//   * Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above
//     copyright notice, this list of conditions and the following
//     disclaimer in the documentation and/or other materials provided
//     with the distribution.
//   * Neither the name of the Willow Garage nor the names of its
//     contributors may be used to endorse or promote products derived
//     from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
//  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
//  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
//  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
//  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
//  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
//  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
//  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.

#include "bzl/inliner/rules2.h"

cc_library {
  visibility = {
    "//reality/engine/executor:__subpackages__",
    "//reality/engine/features:__subpackages__",
  };
  hdrs = {
    "gr8gl-slow.h",
  };
  deps = {
    ":keypoint-queue",
    "//c8:c8-log",
    "//third_party/cvlite/core:core",
    "//reality/engine/features:image-point",
  };
  testonly = 1;
}
cc_end(0x80b1c860);

#include <array>
#include <cmath>
#include <map>
#include <queue>
#include <vector>

#include "c8/c8-log.h"
#include "reality/engine/features/gr8gl-slow.h"
#include "reality/engine/features/keypoint-queue.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

namespace c8 {

namespace {}  // namespace

constexpr int bin(int x, int y, int wbp, int hbp, int numBinCols) {
  return numBinCols * (y / hbp) + (x / wbp);
}

void Gr8GlSlow::retainBest(std::vector<ImagePointLocation> &keypoints, int nPoints, int cols) {
  // If we already have enough points, then we're done.
  if (keypoints.size() <= nPoints) {
    return;
  }

  // If we have no budget for points, clear all of them.
  if (nPoints == 0) {
    keypoints.clear();
    return;
  }
  std::array<int, 12000> binCounts;
  std::fill(binCounts.begin(), binCounts.end(), 0);

  std::vector<std::pair<int, ImagePointLocation>> collisionToScore;

  collisionToScore.reserve(keypoints.size());
  std::stable_sort(keypoints.begin(), keypoints.end(), KeypointResponseMore());
  for (const auto &keypoint : keypoints) {
    const int bini = bin(keypoint.pt.x, keypoint.pt.y, 60, 64, 8);
    collisionToScore.push_back(std::make_pair(binCounts[bini]++, keypoint));
  }

  // We are only interested in nPoints from this list
  std::nth_element(
    collisionToScore.begin(),
    collisionToScore.begin() + nPoints,
    collisionToScore.end(),
    KeypointResponsePairMore());

  keypoints.resize(nPoints);
  for (size_t i = 0; i < nPoints; i++) {
    keypoints[i] = collisionToScore[i].second;
  }
}

}  // namespace c8
