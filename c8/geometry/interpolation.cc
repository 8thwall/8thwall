// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Dat Chu (dat@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"interpolation.h"};
  deps = {
    "//c8:hpoint",
    "//c8:hvector",
    "//c8:quaternion",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xe7e55559);

#include <iostream>

#include "c8/geometry/interpolation.h"
namespace c8 {

float lerp(float a, float b, float alpha) { return a * alpha + (1 - alpha) * b; }

HPoint2 lerp(const HPoint2 &p1, const HPoint2 &p2, float alpha) {
  return {p1.x() * alpha + (1 - alpha) * p2.x(), p1.y() * alpha + (1 - alpha) * p2.y()};
}

int lessThanIndexSorted(const Vector<float> &xs, float val) {
  auto it = std::upper_bound(xs.begin(), xs.end(), val);
  if (it == xs.begin()) {
    return 0;
  }
  if (it == xs.end()) {
    return xs.size() - 1;
  }
  return it - xs.begin() - 1;
}

int nearestNeighbourIndexSorted(const Vector<float> &xs, float val) {
  auto it = std::lower_bound(xs.begin(), xs.end(), val);
  if (it == xs.begin()) {
    return 0;
  }
  if (it == xs.end()) {
    return xs.size() - 1;
  }
  float a = *(it - 1);
  float b = *(it);
  if (std::abs(val - a) < std::abs(val - b)) {
    return it - xs.begin() - 1;
  }
  return it - xs.begin();
}

Vector<float> interp(
  const Vector<float> &xs, const Vector<float> &ys, const Vector<float> &newXs) {
  if (xs.empty() || xs.size() != ys.size() || newXs.empty()) {
    return {};
  }
  if (xs.size() == 1) {
    return Vector<float>(newXs.size(), ys[0]);
  }
  auto xMaxIdx = xs.size() - 1;
  Vector<float> newYs;
  newYs.reserve(newXs.size());
  for (size_t i = 0; i < newXs.size(); ++i) {
    auto idx = nearestNeighbourIndexSorted(xs, newXs[i]);
    auto dx = 0.f;
    auto dy = 0.f;
    if (xs[idx] > newXs[i]) {
      dx = idx > 0 ? (xs[idx] - xs[idx - 1]) : (xs[idx + 1] - xs[idx]);
      dy = idx > 0 ? (ys[idx] - ys[idx - 1]) : (ys[idx + 1] - ys[idx]);
    } else {
      dx = idx < xMaxIdx ? (xs[idx + 1] - xs[idx]) : (xs[idx] - xs[idx - 1]);
      dy = idx < xMaxIdx ? (ys[idx + 1] - ys[idx]) : (ys[idx] - ys[idx - 1]);
    }
    auto m = dx == 0.f ? 0.f : dy / dx;
    auto b = ys[idx] - xs[idx] * m;
    newYs.push_back(newXs[i] * m + b);
  }
  return newYs;
}

Vector<Quaternion> interp(
  const Vector<float> &xs, const Vector<Quaternion> &ys, const Vector<float> &newXs) {
  Vector<Quaternion> newYs;
  if (xs.empty() || xs.size() != ys.size() || newXs.empty()) {
    return newYs;
  }
  newYs.reserve(newXs.size());
  for (size_t i = 0; i < newXs.size(); ++i) {
    // If our new x value is beyond the range of our xs then just push back the closest y we have.
    if (newXs[i] >= xs.back()) {
      newYs.push_back(ys.back());
      continue;
    }
    if (newXs[i] <= xs[0]) {
      newYs.push_back(ys[0]);
      continue;
    }
    // Returns the index of the greatest element in xs that is less than newX, so we can interpolate
    // from idx -> idx + 1. Just need to scale x -> x + 1 into 0 -> 1.
    auto idx = lessThanIndexSorted(xs, newXs[i]);
    auto t = (newXs[i] - xs[idx]) / (xs[idx + 1] - xs[idx]);
    newYs.push_back(ys[idx].interpolate(ys[idx + 1], t));
  }
  return newYs;
}
}  // namespace c8
