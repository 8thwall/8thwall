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

#include "c8/stats/scope-timer.h"

namespace c8cv {

/************************************ Base Classes ************************************/

/** @brief Abstract base class for 2D image feature detectors and descriptor extractors
 */
class CV_EXPORTS_W Feature2D : public virtual c8cv::Algorithm {
public:
  virtual ~Feature2D();

  /** @brief Detects keypoints in an image (first variant) or image set (second variant).

  @param image Image.
  @param keypoints The detected keypoints. In the second variant of the method keypoints[i] is a set
  of keypoints detected in images[i] .
  @param mask Mask specifying where to look for keypoints (optional). It must be a 8-bit integer
  matrix with non-zero values in the region of interest.
   */
  CV_WRAP virtual void detect(
    c8cv::InputArray image,
    CV_OUT std::vector<c8cv::KeyPoint> &keypoints,
    c8cv::InputArray mask = c8cv::noArray());

  /** @overload
  @param images Image set.
  @param keypoints The detected keypoints. In the second variant of the method keypoints[i] is a set
  of keypoints detected in images[i] .
  @param masks Masks for each input image specifying where to look for keypoints (optional).
  masks[i] is a mask for images[i].
  */
  CV_WRAP virtual void detect(
    c8cv::InputArrayOfArrays images,
    CV_OUT std::vector<std::vector<c8cv::KeyPoint> > &keypoints,
    c8cv::InputArrayOfArrays masks = c8cv::noArray());

  /** @brief Computes the descriptors for a set of keypoints detected in an image (first variant) or
  image set (second variant).

  @param image Image.
  @param keypoints Input collection of keypoints. Keypoints for which a descriptor cannot be
  computed are removed. Sometimes new keypoints can be added, for example: SIFT duplicates keypoint
  with several dominant orientations (for each orientation).
  @param descriptors Computed descriptors. In the second variant of the method descriptors[i] are
  descriptors computed for a keypoints[i]. Row j is the keypoints (or keypoints[i]) is the
  descriptor for keypoint j-th keypoint.
   */
  CV_WRAP virtual void compute(
    c8cv::InputArray image,
    CV_OUT CV_IN_OUT std::vector<c8cv::KeyPoint> &keypoints,
    c8cv::OutputArray descriptors);

  /** @overload

  @param images Image set.
  @param keypoints Input collection of keypoints. Keypoints for which a descriptor cannot be
  computed are removed. Sometimes new keypoints can be added, for example: SIFT duplicates keypoint
  with several dominant orientations (for each orientation).
  @param descriptors Computed descriptors. In the second variant of the method descriptors[i] are
  descriptors computed for a keypoints[i]. Row j is the keypoints (or keypoints[i]) is the
  descriptor for keypoint j-th keypoint.
  */
  CV_WRAP virtual void compute(
    c8cv::InputArrayOfArrays images,
    CV_OUT CV_IN_OUT std::vector<std::vector<c8cv::KeyPoint> > &keypoints,
    c8cv::OutputArrayOfArrays descriptors);

  /** Detects keypoints and computes the descriptors */
  CV_WRAP virtual void detectAndCompute(
    c8cv::InputArray image,
    c8cv::InputArray mask,
    CV_OUT std::vector<c8cv::KeyPoint> &keypoints,
    c8cv::OutputArray descriptors,
    bool useProvidedKeypoints = false);

  CV_WRAP virtual int descriptorSize() const;
  CV_WRAP virtual int descriptorType() const;
  CV_WRAP virtual int defaultNorm() const;

  CV_WRAP void write(const c8cv::String &fileName) const;

  CV_WRAP void read(const c8cv::String &fileName);

  virtual void write(c8cv::FileStorage &) const override;

  virtual void read(const c8cv::FileNode &) override;

  //! Return true if detector object is empty
  CV_WRAP virtual bool empty() const override;
};

/** Feature detectors in OpenCV have wrappers with a common interface that enables you to easily
switch between different algorithms solving the same problem. All objects that implement keypoint
detectors inherit the FeatureDetector interface. */
typedef Feature2D FeatureDetector;

/** Extractors of keypoint descriptors in OpenCV have wrappers with a common interface that enables
you to easily switch between different algorithms solving the same problem. This section is devoted
to computing descriptors represented as vectors in a multidimensional space. All objects that
implement the vector descriptor extractors inherit the DescriptorExtractor interface.
 */
typedef Feature2D DescriptorExtractor;

} /* namespace c8cv */
