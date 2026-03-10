// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Paris Morgan (paris@8thwall.com)
//
// Common filters.

#pragma once

#include <cmath>
#include <deque>

#include "c8/hpoint.h"
#include "c8/hvector.h"
#include "c8/vector.h"

namespace c8 {

// Recursive filter low pass filter
template <typename T>
class RecursiveFilterLowPass {
public:
  RecursiveFilterLowPass(T alphaFactor) : alphaFactor_(alphaFactor), initialized_(false) {}
  ~RecursiveFilterLowPass() = default;

  // Filters the input data.
  //
  //  y(i) = x(i) * alpha + (1 - alpha) * y(i - 1)
  //
  //  where,
  //  y(i) = current output value
  //  x(i) = current input value
  //  y(i - 1) = previous output value
  //  alpha = the alpha factor
  //
  // @param input, the new value which want to be filtered.
  // @return the filtered data.
  T filter(T input) {
    currentInput_ = input;
    if (!initialized_) {
      prevOutput_ = input;
      initialized_ = true;
      return input;
    }

    const T result = input * alphaFactor_ + (1.0f - alphaFactor_) * prevOutput_;

    prevOutput_ = result;

    return result;
  }

  void setAlphaFactor(T alphaFactor) {
    alphaFactor_ = alphaFactor;
  }

  T rawInput() const { return currentInput_; }

private:
  // The filter alpha factor.
  // What percentage of the input and previous output data will used.
  T alphaFactor_;

  // The previous result value of the filter.
  T prevOutput_;

  // current raw input
  T currentInput_;

  // In the beginning initialize the filter with the first input value.
  bool initialized_ = false;
};

// Creates a trailing filter, i.e. a Kalman Filter with a spherical gaussian noise model.
template <typename TypeWithXYZ>
class TrailingLowPass {
public:
  TrailingLowPass(float alpha = .5f) : alpha_(alpha) {}

  // Add one element and return all filter values.
  const Vector<TypeWithXYZ> &push(TypeWithXYZ val);
  // Add multiple elements and return all filter values.
  const Vector<TypeWithXYZ> &push(const Vector<TypeWithXYZ> &vals);

private:
  float alpha_;
  Vector<TypeWithXYZ> values_;
};

extern template class TrailingLowPass<HPoint3>;
extern template class TrailingLowPass<HVector3>;

// Compute a Rolling Mean within a time window
class RollingMean {
public:
  RollingMean(float timeWindowInSecs) : timeWindowInSecs_{timeWindowInSecs}, currentSum_(0){};

  // Push on a new measurement and return the current rolling mean
  float push(float timeInSec, float measurement);
  float currentMean() const;

private:
  struct TimedMeasurement {
    float timeInSec = 0.f;
    float measurement = 0.f;
  };
  std::deque<TimedMeasurement> measurements_;
  float timeWindowInSecs_;
  double currentSum_;
};

// Helper for gaussianFilter(). Public for unit tests.
Vector<float> gaussKernel(float sigma, int window);
// One dimension gaussian filter. Note this could be made into a class that stores past values and
// just recomputes elements within samples.
// @param sigma the standard deviation - determines the shape of the gaussian
// @param window the window size of the filter - scipy uses int(4 * sigma + .5), others 3 * sigma
Vector<float> gaussianFilter(const Vector<float> &kernel, const Vector<float> &values);
Vector<float> gaussianFilter(const Vector<float> &values, float sigma, int window);
template <typename TypeWithXYZ>
Vector<TypeWithXYZ> gaussianFilter(const Vector<TypeWithXYZ> &values, float sigma, int window) {
  Vector<float> x;
  Vector<float> y;
  Vector<float> z;
  x.reserve(values.size());
  y.reserve(values.size());
  z.reserve(values.size());
  for (const auto &elem : values) {
    x.push_back(elem.x());
    y.push_back(elem.y());
    z.push_back(elem.z());
  }
  auto kernel = gaussKernel(sigma, window);
  auto xRes = gaussianFilter(x, sigma, window);
  auto yRes = gaussianFilter(y, sigma, window);
  auto zRes = gaussianFilter(z, sigma, window);
  Vector<TypeWithXYZ> res;
  for (int i = 0; i < xRes.size(); ++i) {
    res.push_back({xRes[i], yRes[i], zRes[i]});
  }
  return res;
}

}  // namespace c8
