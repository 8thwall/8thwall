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

#include "reality/engine/features/image-point.h"

namespace c8 {

class Gr8GlSlow {
public:
  enum { kBytes = 32, HARRIS_SCORE = 0, FAST_SCORE = 1 };

  // Choose a fixed size PATCH_SIZE at compile-time to allow stack allocation of patch-sized arrays.
  static constexpr int PATCH_SIZE = 31;

  // Retain the best nPoints from the set of keypoints, taking into account both response and
  // geometry.
  static void retainBest(std::vector<ImagePointLocation> &keypoints, int nPoints, int cols);
  Gr8GlSlow(Gr8GlSlow &&) = default;

  // Use factory method to create Gr8 features.
  Gr8GlSlow() = delete;
  Gr8GlSlow &operator=(Gr8GlSlow &&) = delete;
  Gr8GlSlow(const Gr8GlSlow &) = delete;
};

}  // namespace c8
