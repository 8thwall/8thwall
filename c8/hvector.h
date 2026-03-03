// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Class representing a N-dim dimensional vector, padded with a 0.0f homogenous coordinate.

#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <iterator>
#include <numeric>
#include <ostream>
#include <sstream>
#include <type_traits>

#include "c8/string.h"
#include "c8/vector.h"

namespace c8 {

template <size_t D>
class HVector {
public:
  // Default constructor.
  HVector() noexcept { std::fill(xHat.begin(), xHat.end(), .0f); }

  // Constructor with D inputs from non-homogenous coordinates.
  template <typename... Floats, typename = std::enable_if_t<sizeof...(Floats) == D>>
  HVector(Floats... val) noexcept : xHat{{val..., .0f}} {}

  // Array constructor for non-homogenous coordinates.
  HVector(float const (&xIn)[D]) noexcept {
    std::copy(std::begin(xIn), std::end(xIn), std::begin(xHat));
    xHat[D] = .0f;
  }
  HVector(const std::array<float, D> &xIn) noexcept {
    std::copy(xIn.cbegin(), xIn.cend(), xHat.begin());
    xHat[D] = .0f;
  }

  // Default move constructors.
  HVector(HVector &&) noexcept = default;
  HVector &operator=(HVector &&) noexcept = default;

  // Allow copy and assign.
  HVector(const HVector &) noexcept = default;
  HVector &operator=(const HVector &) noexcept = default;

  // Accessor for the x-coordinate.
  float x() const noexcept { return xHat[0]; }

  // Accessor for the normalized y-coordinate.
  float y() const noexcept {
    static_assert(D >= 2, "y-dimension not defined for dimension < 2");
    return xHat[1];
  }

  // Accessor for the normalized z-coordinate.
  float z() const noexcept {
    static_assert(D >= 3, "z-dimension not defined for dimension < 3");
    return xHat[2];
  }

  // Vector addition.
  HVector<D> operator+(HVector<D> rhs) const {
    HVector<D> result;
    std::transform(
      xHat.cbegin(), xHat.cend() - 1, rhs.xHat.cbegin(), result.xHat.begin(), std::plus<float>());
    return result;
  }

  // Vector subtraction.
  HVector<D> operator-(HVector<D> rhs) const {
    HVector<D> result;
    std::transform(
      xHat.cbegin(), xHat.cend() - 1, rhs.xHat.cbegin(), result.xHat.begin(), std::minus<float>());
    return result;
  }

  // Scalar multiplication
  HVector<D> operator*(float scale) const {
    HVector<D> result;
    std::transform(xHat.cbegin(), xHat.cend() - 1, result.xHat.begin(), [scale](float val) {
      return val * scale;
    });
    return result;
  }

  // Unary -.
  HVector<D> operator-() const { return *this * -1.0f; }

  // Vector addition and assignment operator
  HVector<D> &operator+=(HVector<D> rhs) {
    std::transform(
      xHat.cbegin(), xHat.cend() - 1, rhs.xHat.cbegin(), xHat.begin(), std::plus<float>());
    return *this;
  }

  // Vector subtraction and assignment operator
  HVector<D> &operator-=(HVector<D> rhs) {
    std::transform(
      xHat.cbegin(), xHat.cend() - 1, rhs.xHat.cbegin(), xHat.begin(), std::minus<float>());
    return *this;
  }

  // Returns the vector dot product.
  float dot(HVector<D> rhs) const {
    return std::inner_product(xHat.cbegin(), xHat.cend() - 1, rhs.xHat.cbegin(), .0f);
  }

  float sqdist(HVector<D> rhs) const {
    auto diff = *this - rhs;
    return diff.dot(diff);
  }

  // Computes the angle in degrees between two vectors
  float angleD(HVector<D> rhs) const {
    float dotProduct = dot(rhs);

    float selfNorm = l2Norm();
    float rhsNorm = rhs.l2Norm();

    return std::acos(dotProduct / (selfNorm * rhsNorm)) * 180.0 / M_PI;
  }

  float dist(HVector<D> rhs) const { return std::sqrt(sqdist(rhs)); }

  // Computes the vector cross product on 3D Vectors.
  // template <size_t DD = D, typename std::enable_if<DD == 3>::value>
  HVector<3> cross(HVector<3> rhs) const {
    static_assert(D == 3, "cross product only defined for dimension == 3");
    return HVector<3>{
      xHat[1] * rhs.xHat[2] - xHat[2] * rhs.xHat[1],
      xHat[2] * rhs.xHat[0] - xHat[0] * rhs.xHat[2],
      xHat[0] * rhs.xHat[1] - xHat[1] * rhs.xHat[0],
    };
  }

  // Returns the l2-norm.
  float l2Norm() const { return std::sqrt(dot(*this)); }

  // Return a HVector normalized to unit magnitude.
  HVector<D> unit() const {
    float norm = l2Norm();
    HVector<D> unitVector;
    std::transform(xHat.cbegin(), xHat.cend() - 1, unitVector.xHat.begin(), [norm](float val) {
      return val / norm;
    });
    return unitVector;
  }

  String toString() const noexcept {
    std::stringstream out;
    out << *this;
    return out.str();
  }

  String toPrettyString() const noexcept {
    std::stringstream stream;
    auto data = xHat.data();
    stream << "(";
    for (int i = 0; i < D; ++i) {
      if (i > 0) {
        stream << ", ";
      }
      stream << ((std::abs(data[i]) < 1e-7) ? 0 : data[i]);
    }
    stream << ")";
    return stream.str();
  }

  // Return the normalized coordinates as an array, dimension D.
  std::array<float, D> data() const noexcept {
    std::array<float, D> normalized;
    std::copy(xHat.cbegin(), xHat.cend() - 1, normalized.begin());
    return normalized;
  }

  // Accessor for the unnormalized coordinates as an array, dimension D+1.
  const std::array<float, D + 1> &raw() const noexcept { return xHat; }
  std::array<float, D + 1> &raw() noexcept { return xHat; }

private:
  std::array<float, D + 1> xHat;

  static_assert(D > 0, "HVector<D> dimension must be greater than zero");
};

// Type alias for 2D homogeneous vector.
using HVector2 = HVector<2>;

// Type alias for 3D homogeneous vector.
using HVector3 = HVector<3>;

template <size_t D>
HVector<D> operator*(float scalar, const HVector<D> &vec) {
  return vec * scalar;
}

// Stream output operator.
template <size_t D>
std::ostream &operator<<(std::ostream &stream, HVector<D> pt) noexcept {
  auto data = pt.data();
  stream << "(";
  for (int i = 0; i < D; ++i) {
    if (i > 0) {
      stream << ", ";
    }
    stream << data[i];
  }
  stream << ")";
  return stream;
}

}  // namespace c8
