// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  visibility = {
    "//visibility:public",
  };
  deps = {
    ":string",
    ":vector",
  };
  hdrs = {
    "hpoint.h",
  };
}
cc_end(0x1fcf0569);

#include "c8/hpoint.h"

namespace c8 {

// Specialized sqdist function for HPoint<2> for better performance.
template <>
float HPoint<2>::sqdist(HPoint<2> rhs) const {
  auto dx = x() - rhs.x();
  auto dy = y() - rhs.y();
  return dx * dx + dy * dy;
}

// Specialized flatten function for HPoint<3> for better performance.
template <>
void flatten<2>(const Vector<HPoint<3>> &input, Vector<HPoint<2>> *result) {
  result->resize(input.size());
  const float *inputPtr = reinterpret_cast<const float *>(input.data());
  const float *inputEnd = inputPtr + input.size() * 4;
  float *resultPtr = reinterpret_cast<float *>(result->data());
  while (inputPtr != inputEnd) {
    *resultPtr++ = *inputPtr++;
    *resultPtr++ = *inputPtr++;
    *resultPtr++ = *inputPtr++;
    inputPtr++;
  }
}

// Specialized extrude function for HPoint<3> for better performance.
template <>
void extrude<3>(const Vector<HPoint<2>> &input, Vector<HPoint<3>> *result) {
  result->resize(input.size());
  const float *inputPtr = reinterpret_cast<const float *>(input.data());
  const float *inputEnd = inputPtr + input.size() * 3;
  float *resultPtr = reinterpret_cast<float *>(result->data());
  while (inputPtr != inputEnd) {
    *resultPtr++ = *inputPtr++;
    *resultPtr++ = *inputPtr++;
    *resultPtr++ = *inputPtr++;
    *resultPtr++ = 1.0f;
  }
}

}  // namespace c8
