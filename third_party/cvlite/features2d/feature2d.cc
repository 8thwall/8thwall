/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                          License Agreement
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

#include "feature2d.h"

#include "third_party/cvlite/core/private.hpp"

namespace c8cv {

using std::vector;

Feature2D::~Feature2D() {}

/*
 * Detect keypoints in an image.
 * image        The image.
 * keypoints    The detected keypoints.
 * mask         Mask specifying where to look for keypoints (optional). Must be a char
 *              matrix with non-zero values in the region of interest.
 */
void Feature2D::detect(
  c8cv::InputArray image, std::vector<c8cv::KeyPoint> &keypoints, c8cv::InputArray mask) {
  CV_INSTRUMENT_REGION()

  if (image.empty()) {
    keypoints.clear();
    return;
  }
  detectAndCompute(image, mask, keypoints, c8cv::noArray(), false);
}

void Feature2D::detect(
  c8cv::InputArrayOfArrays _images,
  std::vector<std::vector<c8cv::KeyPoint> > &keypoints,
  c8cv::InputArrayOfArrays _masks) {
  CV_INSTRUMENT_REGION()

  vector<c8cv::Mat> images, masks;

  _images.getMatVector(images);
  size_t i, nimages = images.size();

  if (!_masks.empty()) {
    _masks.getMatVector(masks);
    C8CV_Assert(masks.size() == nimages);
  }

  keypoints.resize(nimages);

  for (i = 0; i < nimages; i++) {
    detect(images[i], keypoints[i], masks.empty() ? c8cv::Mat() : masks[i]);
  }
}

/*
 * Compute the descriptors for a set of keypoints in an image.
 * image        The image.
 * keypoints    The input keypoints. Keypoints for which a descriptor cannot be computed are
 * removed. descriptors  Copmputed descriptors. Row i is the descriptor for keypoint i.
 */
void Feature2D::compute(
  c8cv::InputArray image, std::vector<c8cv::KeyPoint> &keypoints, c8cv::OutputArray descriptors) {
  CV_INSTRUMENT_REGION()

  if (image.empty()) {
    descriptors.release();
    return;
  }
  detectAndCompute(image, c8cv::noArray(), keypoints, descriptors, true);
}

void Feature2D::compute(
  c8cv::InputArrayOfArrays _images,
  std::vector<std::vector<c8cv::KeyPoint> > &keypoints,
  c8cv::OutputArrayOfArrays _descriptors) {
  CV_INSTRUMENT_REGION()

  if (!_descriptors.needed())
    return;

  vector<c8cv::Mat> images;

  _images.getMatVector(images);
  size_t i, nimages = images.size();

  C8CV_Assert(keypoints.size() == nimages);
  C8CV_Assert(_descriptors.kind() == c8cv::_InputArray::STD_VECTOR_MAT);

  std::vector<c8cv::Mat> &descriptors = *(std::vector<c8cv::Mat> *)_descriptors.getObj();
  descriptors.resize(nimages);

  for (i = 0; i < nimages; i++) {
    compute(images[i], keypoints[i], descriptors[i]);
  }
}

/* Detects keypoints and computes the descriptors */
void Feature2D::detectAndCompute(
  c8cv::InputArray,
  c8cv::InputArray,
  std::vector<c8cv::KeyPoint> &,
  c8cv::OutputArray,
  bool) {
  CV_INSTRUMENT_REGION()

  C8CV_Error(c8cv::Error::StsNotImplemented, "");
}

void Feature2D::write(const c8cv::String &fileName) const {
  c8cv::FileStorage fs(fileName, c8cv::FileStorage::WRITE);
  write(fs);
}

void Feature2D::read(const c8cv::String &fileName) {
  c8cv::FileStorage fs(fileName, c8cv::FileStorage::READ);
  read(fs.root());
}

void Feature2D::write(c8cv::FileStorage &) const {}

void Feature2D::read(const c8cv::FileNode &) {}

int Feature2D::descriptorSize() const { return 0; }

int Feature2D::descriptorType() const { return CV_32F; }

int Feature2D::defaultNorm() const {
  int tp = descriptorType();
  return tp == CV_8U ? c8cv::NORM_HAMMING : c8cv::NORM_L2;
}

// Return true if detector object is empty
bool Feature2D::empty() const { return true; }

}  // namespace c8cv
