// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "float-vector.h",
  };
  visibility = {
    "//visibility:public",
  };
  deps = {
    "//c8:exceptions",
    "//c8:vector",
    "//c8/string:format",
  };
}
cc_end(0xcaa173db);

#include <algorithm>
#include <cmath>
#include <functional>
#include <numeric>

#include "c8/exceptions.h"
#include "c8/float-vector.h"
#include "c8/string/format.h"

namespace c8 {

FloatVector &FloatVector::operator+=(float value) {
  std::transform(
    vector_.begin(),
    vector_.end(),
    vector_.begin(),
    std::bind(std::plus<float>(), std::placeholders::_1, value));
  return *this;
}

FloatVector &FloatVector::operator-=(float value) {
  std::transform(
    vector_.begin(),
    vector_.end(),
    vector_.begin(),
    std::bind(std::minus<float>(), std::placeholders::_1, value));
  return *this;
}

FloatVector &FloatVector::operator*=(float value) {
  std::transform(
    vector_.begin(),
    vector_.end(),
    vector_.begin(),
    std::bind(std::multiplies<float>(), std::placeholders::_1, value));
  return *this;
}

FloatVector &FloatVector::operator+=(const FloatVector &vec) {
  std::transform(vector_.begin(), vector_.end(), vec.cbegin(), vector_.begin(), std::plus<float>());
  return *this;
}

FloatVector &FloatVector::operator-=(const FloatVector &vec) {
  std::transform(
    vector_.begin(), vector_.end(), vec.cbegin(), vector_.begin(), std::minus<float>());
  return *this;
}

FloatVector &FloatVector::operator*=(const FloatVector &vec) {
  std::transform(
    vector_.begin(), vector_.end(), vec.cbegin(), vector_.begin(), std::multiplies<float>());
  return *this;
}

FloatVector FloatVector::clone() const {
  FloatVector result;
  result.vector_ = vector_;
  return result;
}

FloatVector &FloatVector::copyFrom(const FloatVector &vec) {
  vector_ = vec.vector_;
  return *this;
}

void FloatVector::fill(float value) { std::fill(vector_.begin(), vector_.end(), value); }

FloatVector &FloatVector::l1Normalize() {
  float scale = 1.0f / l1Norm(*this);
  return *this *= scale;
}

FloatVector &FloatVector::l2Normalize() {
  float scale = 1.0f / l2Norm(*this);
  return *this *= scale;
}

FloatVector &FloatVector::signSqrt() {
  for (auto &bin : vector_) {
    bin = bin < 0 ? -std::sqrt(-bin) : std::sqrt(bin);
  }
  return *this;
}

FloatVector &FloatVector::sqrt() {
  for (auto &bin : vector_) {
    bin = std::sqrt(bin);
  }
  return *this;
}

FloatVector &FloatVector::invert() {
  for (auto &bin : vector_) {
    bin = 1.0f / bin;
  }
  return *this;
}

float l1Distance(const FloatVector &a, const FloatVector &b) {
  return std::inner_product(
    a.cbegin(), a.cend(), b.cbegin(), 0.0f, std::plus<>(), [](float aa, float bb) {
      return std::fabs(aa - bb);
    });
}

float l2Distance(const FloatVector &a, const FloatVector &b) {
  return std::sqrt(l2SquaredDistance(a, b));
}

float l2SquaredDistance(const FloatVector &a, const FloatVector &b) {
  return std::inner_product(
    a.cbegin(), a.cend(), b.cbegin(), 0.0f, std::plus<>(), [](float aa, float bb) {
      float diff = aa - bb;
      return diff * diff;
    });
}

float l1Norm(const FloatVector &vec) {
  return std::accumulate(
    vec.cbegin(), vec.cend(), 0.0f, [](float acc, float val) { return acc + std::fabs(val); });
}

float l2Norm(const FloatVector &vec) { return std::sqrt(l2SquaredNorm(vec)); }

float l2SquaredNorm(const FloatVector &vec) {
  return std::accumulate(
    vec.cbegin(), vec.cend(), 0.0f, [](float acc, float val) { return acc + (val * val); });
}

float mean(const FloatVector &vec) {
  float result = std::accumulate(vec.begin(), vec.end(), 0.0f);
  return result / vec.size();
}

// Compute the inner product of two FloatVectors.
float innerProduct(const FloatVector &a, const FloatVector &b) {
  return std::inner_product(a.begin(), a.end(), b.begin(), 0.0f);
}

FloatVector operator+(const FloatVector &vec, float value) {
  FloatVector result = vec.clone();
  result += value;
  return result;
}

FloatVector operator-(const FloatVector &vec, float value) {
  FloatVector result = vec.clone();
  result -= value;
  return result;
}

FloatVector operator*(const FloatVector &vec, float value) {
  FloatVector result = vec.clone();
  result *= value;
  return result;
}

FloatVector operator+(const FloatVector &a, const FloatVector &b) {
  FloatVector result = a.clone();
  result += b;
  return result;
}

FloatVector operator-(const FloatVector &a, const FloatVector &b) {
  FloatVector result = a.clone();
  result -= b;
  return result;
}

FloatVector operator*(const FloatVector &a, const FloatVector &b) {
  FloatVector result = a.clone();
  result *= b;
  return result;
}

}  // namespace c8
