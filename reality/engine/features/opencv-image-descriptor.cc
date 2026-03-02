// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules.h"

cc_library {
  visibility = {
    "//visibility:private",
  };
  hdrs = {"opencv-image-descriptor.h"};
  deps = {
    "//bzl/inliner:rules",
    "//c8/cvlite:convert",
    "//c8:exceptions",
    "//reality/engine/features:image-descriptor",
    "//third_party/cvlite/core:core",
  };
}

#include "reality/engine/features/opencv-image-descriptor.h"

#include <string>
#include "c8/exceptions.h"
#include "third_party/cvlite/core/core.hpp"

namespace c8 {

template <size_t N>
MatBackedInputArray asInputArray(const ImageDescriptor<N> &descriptor) {
  // Uses a const_cast to create a c8cv::Mat header, but maintains logical constness by returning a
  // read-only view of the c8cv::Mat through conversion to c8cv::InputArray;
  return MatBackedInputArray(c8cv::Mat(
    1, descriptor.size(), CV_8U, static_cast<void *>(const_cast<uint8_t *>(descriptor.data()))));
}

template <size_t N>
ImageDescriptor<N> toImageDescriptor(const c8cv::Mat &input) {
  if (!(input.cols == 1 && input.rows == N) && !(input.cols == N && input.rows == 1)) {
    // C8_THROW_INVALID_ARGUMENT("descriptor must be a Nx1 or 1xN vector");
    C8_THROW_INVALID_ARGUMENT("descriptor must be a Nx1 or 1xN vector, with N=" + std::to_string(N));
  }

  if (input.type() != CV_8UC1) {
    C8_THROW_INVALID_ARGUMENT("descriptor type must be 8-bit and single channel");
  }

  ImageDescriptor<N> descriptor;
  std::copy(input.data, input.data + N, descriptor.mutableData());
  return descriptor;
}

// Explicit instantiations
template MatBackedInputArray asInputArray(const ImageDescriptor16 &descriptor);
template MatBackedInputArray asInputArray(const ImageDescriptor32 &descriptor);
template MatBackedInputArray asInputArray(const ImageDescriptor64 &descriptor);

template ImageDescriptor16 toImageDescriptor<16>(const c8cv::Mat &input);
template ImageDescriptor32 toImageDescriptor<32>(const c8cv::Mat &input);
template ImageDescriptor64 toImageDescriptor<64>(const c8cv::Mat &input);

}  // namespace c8
