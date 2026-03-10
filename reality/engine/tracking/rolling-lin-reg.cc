// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Scott Pollack (scott@8thwall.com)

#include "bzl/inliner/rules.h"

cc_library {
  hdrs = {"rolling-lin-reg.h"};
  deps = {
    "//bzl/inliner:rules",
  };
}

#include <iostream>

#include "reality/engine/tracking/rolling-lin-reg.h"

namespace c8 {

float RollingLinReg::slope(float x, float y) {
  points_.push(std::pair<float, float>(x, y));

  sX_ += x;
  sY_ += y;
  sXX_ += x * x;
  sXY_ += x * y;
  sYY_ += y * y;

  while (x - points_.front().first >= window_) {
    auto p = points_.front();
    sX_ -= p.first;
    sY_ -= p.second;
    sXX_ -= p.first * p.first;
    sXY_ -= p.first * p.second;
    sYY_ -= p.second * p.second;
    points_.pop();
  }

  beta_ = (points_.size() * sXY_ - sX_ * sY_) / (points_.size() * sXX_ - sX_ * sX_);
  alpha_ = (sY_ - beta_ * sX_) / points_.size();
  return beta_;
}

float RollingLinReg::slope() const {
  return beta_;
}

float RollingLinReg::offset() const {
  return alpha_;
}

}  // namespace c8
