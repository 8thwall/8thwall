// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Scott Pollack (scott@8thwall.com)

#pragma once

#include <queue>

namespace c8 {

// Computes a rolling linear regression keeping track of previous data points.
class RollingLinReg {
public:
  // window is the width of the rolling boxcar filter in the X direction.
  RollingLinReg(float window) : window_{window}, sX_{0}, sY_{0}, sXX_{0}, sXY_{0}, sYY_{0} {};

  // Push new data (x,y) into rolling boxcar filter and return new linear slope.
  // Assumes x values are monotonically increasing.
  float slope(float x, float y);
  float slope() const;

  // Return the offset for the estimated slope
  float offset() const;

  void reset() {
    points_ = {};
    sX_ = sY_ = sXX_ = sXY_ = sYY_ = alpha_ = beta_ = 0.0f;
  }

private:
  std::queue<std::pair<float, float>> points_;
  float window_;
  float sX_;
  float sY_;
  float sXX_;
  float sXY_;
  float sYY_;
  float alpha_;
  float beta_;
};

}  // namespace c8
