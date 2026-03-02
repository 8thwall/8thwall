// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Paris Morgan (paris@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "filters.h",
  };
  visibility = {
    "//visibility:public",
  };
  deps = {
    "//c8:vector",
    "//c8:hpoint",
    "//c8:hvector",
  };
  copts = {
    "-D_USE_MATH_DEFINES",
  };
}
cc_end(0x1e23096a);

#include <cmath>

#include "c8/filters.h"

namespace c8 {

template <typename TypeWithXYZ>
const Vector<TypeWithXYZ> &TrailingLowPass<TypeWithXYZ>::push(TypeWithXYZ val) {
  if (values_.empty()) {
    values_.push_back(val);
  } else {
    auto oldVal = values_.back();
    values_.push_back(
      {(1.f - alpha_) * oldVal.x() + alpha_ * val.x(),
       (1.f - alpha_) * oldVal.y() + alpha_ * val.y(),
       (1.f - alpha_) * oldVal.z() + alpha_ * val.z()});
  }
  return values_;
}

template <typename TypeWithXYZ>
const Vector<TypeWithXYZ> &TrailingLowPass<TypeWithXYZ>::push(const Vector<TypeWithXYZ> &vals) {
  for (const auto &val : vals) {
    push(val);
  }
  return values_;
}

template class TrailingLowPass<HPoint3>;
template class TrailingLowPass<HVector3>;

float RollingMean::push(float timeInSecs, float measurement) {
  measurements_.push_back({timeInSecs, measurement});
  currentSum_ += measurements_.back().measurement;

  // pop until the first element is within `timeWindowInSecs_` from the last element
  while (measurements_.size() >= 2
         && ((measurements_.back().timeInSec - measurements_[0].timeInSec) >= timeWindowInSecs_)) {
    currentSum_ -= measurements_.front().measurement;
    measurements_.pop_front();
  }
  return currentSum_ / measurements_.size();
}

float RollingMean::currentMean() const {
  return currentSum_ / measurements_.size();
}

// Note there are fancier ways to do this, i.e.
// https://dev.theomader.com/gaussian-kernel-calculator/
Vector<float> gaussKernel(float sigma, int window) {
  int width = window / 2;
  auto norm = 1.f / (sigma * std::sqrt(2.f * M_PI));
  auto coefficient = -2.f * sigma * sigma;
  auto sum = 0.f;
  Vector<float> kernel;
  for (int i = 0; i < width * 2 + 1; ++i) {
    auto x = i - width;
    auto val = norm * std::exp(x * x / coefficient);
    kernel.push_back(val);
    sum += val;
  }

  // Divide by total to make sure the sum of all the values is equal to 1.
  for (int i = 0; i < kernel.size(); ++i) {
    kernel[i] /= sum;
  }
  return kernel;
};

Vector<float> gaussianFilter(const Vector<float> &kernel, const Vector<float> &values) {
  Vector<float> out;
  out.reserve(values.size());
  int width = (kernel.size() - 1) / 2;
  for (int i = 0; i < values.size(); ++i) {
    auto sample = 0.f;
    auto kernelSum = 0.f;
    for (int j = i - width; j <= i + width; ++j) {
      if (j >= 0 && j < values.size()) {
        int kernelIdx = width + (j - i);
        sample += kernel[kernelIdx] * values[j];
        kernelSum += kernel[kernelIdx];
      }
    }
    // Can either zero pad the border, resize kernel, or hallucinate out-of-bounds values. Here we
    // zero pad the border and normalize based on how much of the kernel we used.
    // Note if this becomes slow we can optimize by avoiding the add + divide ops in regions of the
    // loop where kerrnelSum is expected to be 1.
    out.push_back(sample / kernelSum);
  }
  return out;
}

Vector<float> gaussianFilter(const Vector<float> &values, float sigma, int window) {
  return gaussianFilter(gaussKernel(sigma, window), values);
}

}  // namespace c8
