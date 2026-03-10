/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
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
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#pragma once

#include "third_party/cvlite/core/core.hpp"

namespace c8cv {

//! @addtogroup features2d
//! @{

// //! writes vector of keypoints to the file storage
// CV_EXPORTS void write(FileStorage& fs, const String& name, const std::vector<c8cv::KeyPoint>&
// keypoints);
// //! reads vector of keypoints from the specified file storage node
// CV_EXPORTS void read(const FileNode& node, CV_OUT std::vector<c8cv::KeyPoint>& keypoints);

/** @brief A class filters a vector of keypoints.

 Because now it is difficult to provide a convenient interface for all usage scenarios of the
 keypoints filter class, it has only several needed by now static methods.
 */
class CV_EXPORTS KeyPointsFilter {
public:
  KeyPointsFilter() = default;

  /*
   * Remove keypoints within borderPixels of an image edge.
   */
  static void runByImageBorder(
    std::vector<c8cv::KeyPoint> &keypoints, c8cv::Size imageSize, int borderSize);
  /*
   * Remove keypoints of sizes out of range.
   */
  static void runByKeypointSize(
    std::vector<c8cv::KeyPoint> &keypoints, float minSize, float maxSize = FLT_MAX);
  /*
   * Remove keypoints from some image by mask for pixels of this image.
   */
  static void runByPixelsMask(std::vector<c8cv::KeyPoint> &keypoints, const c8cv::Mat &mask);
  /*
   * Remove duplicated keypoints.
   */
  static void removeDuplicated(std::vector<c8cv::KeyPoint> &keypoints);

  /*
   * Retain the specified number of the best keypoints (according to the response)
   */
  static void retainBest(std::vector<c8cv::KeyPoint> &keypoints, int npoints);
};

} /* namespace c8cv */
