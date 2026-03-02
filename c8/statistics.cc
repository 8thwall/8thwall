// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Paris Morgan (paris@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "statistics.h",
  };
  visibility = {
    "//visibility:public",
  };
  deps = {
    ":hpoint",
    ":hvector",
    ":c8-log",
  };
}
cc_end(0x2ac2060a);

#include <algorithm>
#include <cmath>

#include "c8/statistics.h"

namespace c8 {

float mean(const Vector<float> &v) {
  if (v.empty()) {
    return 0.f;
  }
  float sum = 0.f;
  for (int i = 0; i < v.size(); ++i) {
    sum += v[i];
  }
  return sum / v.size();
}

float stdDev(const Vector<float> &v, float mean) {
  if (v.empty()) {
    return 0.f;
  }
  float sum = 0.f;
  for (int i = 0; i < v.size(); i++) {
    sum += std::pow(v[i] - mean, 2.f);
  }
  return std::sqrt(sum / v.size());
}

Vector<float> crossCorrelateFull(const Vector<float> &x, const Vector<float> &y) {
  if (x.empty() || y.empty()) {
    return {};
  }
  Vector<float> z;
  auto N = std::max(x.size(), y.size());
  for (size_t k = 0; k < x.size() + y.size() - 1; ++k) {
    auto sum = 0.f;
    for (size_t l = 0; l < x.size(); ++l) {
      auto yIdx = l - k + N - 1;
      auto yVal = yIdx >= 0 && yIdx < y.size() ? y[yIdx] : 0.f;
      auto xVal = x[l];
      sum += xVal * yVal;
    }
    z.push_back(sum);
  }

  return z;
}
}  // namespace c8
