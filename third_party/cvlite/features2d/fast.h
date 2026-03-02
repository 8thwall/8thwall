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

#include "third_party/cvlite/features2d/feature2d.h"

namespace c8cv {

/** @overload */
CV_EXPORTS void FAST(
  c8cv::InputArray image,
  CV_OUT std::vector<c8cv::KeyPoint> &keypoints,
  int threshold,
  bool nonmaxSuppression = true);

/** @brief Detects corners using the FAST algorithm

@param image grayscale image where keypoints (corners) are detected.
@param keypoints keypoints detected on the image.
@param threshold threshold on difference between intensity of the central pixel and pixels of a
circle around this pixel.
@param nonmaxSuppression if true, non-maximum suppression is applied to detected corners
(keypoints).
@param type one of the three neighborhoods as defined in the paper:
FastFeatureDetector::TYPE_9_16, FastFeatureDetector::TYPE_7_12,
FastFeatureDetector::TYPE_5_8

Detects corners using the FAST algorithm by @cite Rosten06 .

@note In Python API, types are given as cv2.FAST_FEATURE_DETECTOR_TYPE_5_8,
cv2.FAST_FEATURE_DETECTOR_TYPE_7_12 and cv2.FAST_FEATURE_DETECTOR_TYPE_9_16. For corner
detection, use cv2.FAST.detect() method.
 */
CV_EXPORTS void FAST(
  c8cv::InputArray image,
  CV_OUT std::vector<c8cv::KeyPoint> &keypoints,
  int threshold,
  bool nonmaxSuppression,
  int type);

//! @} features2d_main

//! @addtogroup features2d_main
//! @{

/** @brief Wrapping class for feature detection using the FAST method. :
 */
class CV_EXPORTS_W FastFeatureDetector : public Feature2D {
public:
  enum {
    TYPE_5_8 = 0,
    TYPE_7_12 = 1,
    TYPE_9_16 = 2,
    THRESHOLD = 10000,
    NONMAX_SUPPRESSION = 10001,
    FAST_N = 10002,
  };

  CV_WRAP static c8cv::Ptr<FastFeatureDetector> create(
    int threshold = 10, bool nonmaxSuppression = true, int type = FastFeatureDetector::TYPE_9_16);

  CV_WRAP virtual void setThreshold(int threshold) = 0;
  CV_WRAP virtual int getThreshold() const = 0;

  CV_WRAP virtual void setNonmaxSuppression(bool f) = 0;
  CV_WRAP virtual bool getNonmaxSuppression() const = 0;

  CV_WRAP virtual void setType(int type) = 0;
  CV_WRAP virtual int getType() const = 0;
};

} /* namespace c8cv */
