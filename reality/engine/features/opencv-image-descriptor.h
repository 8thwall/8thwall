// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// ImageDescriptor implementation using OpenCV.

#pragma once

#include "c8/cvlite/convert.h"
#include "reality/engine/features/image-descriptor.h"
#include "third_party/cvlite/core/core.hpp"

namespace c8 {

// Return a read-only wrapper to the ImageDescriptor as a c8cv::Mat.
template <size_t N>
MatBackedInputArray asInputArray(const ImageDescriptor<N> &descriptor);

// Create an ImageDescriptor from an OpenCV type.
template <size_t N>
ImageDescriptor<N> toImageDescriptor(const c8cv::Mat &mat);

}  // namespace c8
