// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Point classes using homogenous coordinates.

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
class HPoint {
public:
  // Default constructor.
  constexpr HPoint() noexcept {
    std::fill(xHat.begin(), xHat.end(), .0f);
    xHat[D] = 1.0f;
  }

  // Constructor with D inputs from non-homogenous coordinates.
  template <typename... Floats, typename = std::enable_if_t<sizeof...(Floats) == D>>
  constexpr HPoint(Floats... val) noexcept : xHat{{val..., 1.0f}} {}

  // Array constructor for non-homogenous coordinates.
  constexpr HPoint(float const (&xIn)[D]) noexcept {
    std::copy(std::begin(xIn), std::end(xIn), std::begin(xHat));
    xHat[D] = 1.0f;
  }
  constexpr HPoint(const std::array<float, D> &xIn) noexcept {
    std::copy(xIn.cbegin(), xIn.cend(), xHat.begin());
    xHat[D] = 1.0f;
  }

  // Array constructor for homogenous coordinates.
  constexpr HPoint(float const (&xIn)[D + 1]) noexcept {
    std::copy(std::begin(xIn), std::end(xIn), std::begin(xHat));
  }
  constexpr HPoint(const std::array<float, D + 1> &xIn) noexcept : xHat{xIn} {}

  // Default move constructors.
  constexpr HPoint(HPoint &&) noexcept = default;
  constexpr HPoint &operator=(HPoint &&) noexcept = default;

  // Allow copy and assign.
  constexpr HPoint(const HPoint &) noexcept = default;
  constexpr HPoint &operator=(const HPoint &) noexcept = default;

  // Accessor for the normalized x-coordinate.
  constexpr float x() const noexcept { return xHat[0] / xHat[D]; }

  // Accessor for the normalized y-coordinate.
  constexpr float y() const noexcept {
    static_assert(D >= 2, "y-dimension not defined for dimension < 2");
    return xHat[1] / xHat[D];
  }

  // Accessor for the normalized z-coordinate.
  constexpr float z() const noexcept {
    static_assert(D >= 3, "z-dimension not defined for dimension < 3");
    return xHat[2] / xHat[D];
  }

  // Return the normalized coordinates as an array, dimension D.
  constexpr std::array<float, D> data() const noexcept {
    std::array<float, D> normalized;
    std::transform(xHat.cbegin(), xHat.cend() - 1, normalized.begin(), [w = xHat[D]](float val) {
      return val / w;
    });
    return normalized;
  }

  // Accessor for the unnormalized coordinates as an array, dimension D+1.
  constexpr const std::array<float, D + 1> &raw() const noexcept { return xHat; }
  std::array<float, D + 1> &raw() noexcept { return xHat; }

  // Drop the last dimension in the homogenous point. Used, for example, to convert 3D points to 2D
  // screen coordinates after a perspective projection.
  constexpr HPoint<D - 1> flatten() const noexcept {
    static_assert(D >= 2, "flatten not defined for dimension < 2");
    HPoint<D - 1> result;
    std::copy(xHat.cbegin(), xHat.cbegin() + D, result.xHat.begin());
    return result;
  }

  // Keeps the first D-1 dimensions unchanged while dropping the last dimension. This can be
  // important when using 3D homogenous coordinates to effect 2D homogenous coordinate
  // transformations.
  constexpr HPoint<D - 1> truncate() const noexcept {
    static_assert(D >= 2, "flatten not defined for dimension < 2");
    auto d = data();  // Normalize the data vector.
    d[D - 1] = 1.0f;  // Set the new homogenous coordinate to 1.
    HPoint<D - 1> result;
    std::copy(d.cbegin(), d.cbegin() + D, result.xHat.begin());
    return result;
  }

  // Return an HPoint<D+1> with an additional dimension, analygous to the inverse of flatten.
  constexpr HPoint<D + 1> extrude() const noexcept {
    HPoint<D + 1> result;
    std::copy(xHat.cbegin(), xHat.cend(), result.xHat.begin());
    result.xHat[D + 1] = 1.0f;
    return result;
  }

  float sqdist(HPoint<D> rhs) const {
    HPoint<D> result;
    std::transform(
      xHat.cbegin(),
      xHat.cend() - 1,
      rhs.xHat.cbegin(),
      result.xHat.begin(),
      [aD = xHat[D], bD = rhs.xHat[D]](float a, float b) { return a / aD - b / bD; });
    return std::inner_product(
      result.xHat.cbegin(), result.xHat.cend() - 1, result.xHat.cbegin(), .0f);
  }

  float dist(HPoint<D> rhs) const { return std::sqrt(sqdist(rhs)); }

  float l1dist(HPoint<D> rhs) const {
    HPoint<D> result;
    std::transform(
      xHat.cbegin(),
      xHat.cend() - 1,
      rhs.xHat.cbegin(),
      result.xHat.begin(),
      [aD = xHat[D], bD = rhs.xHat[D]](float a, float b) { return a / aD - b / bD; });

    return std::accumulate(
      result.xHat.cbegin(), result.xHat.cend() - 1, 0.0f, [](float sum, float val) {
        return sum + std::abs(val);
      });
  }

  String toString() const noexcept {
    std::stringstream out;
    out << *this;
    return out.str();
  }

  String toPrettyString() const noexcept {
    std::stringstream stream;
    auto d = data();
    stream << "(";
    for (int i = 0; i < D; ++i) {
      if (i > 0) {
        stream << ", ";
      }
      stream << ((std::abs(d[i]) < 1e-7) ? 0 : d[i]);
    }
    stream << ")";
    return stream.str();
  }

private:
  // Declare the following instantiations as friends to support the flatten and extrude methods.
  friend class HPoint<D + 1>;
  friend class HPoint<D - 1>;

  std::array<float, D + 1> xHat;

  static_assert(D > 0, "HPoint<D> dimension must be greater than zero");
};

// Type alias for 2D homogeneous point.
using HPoint2 = HPoint<2>;

// Type alias for 3D homogeneous point.
using HPoint3 = HPoint<3>;

template <>
float HPoint<2>::sqdist(HPoint<2> rhs) const;

// Stream output operator.
template <size_t D>
std::ostream &operator<<(std::ostream &stream, HPoint<D> pt) noexcept {
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

// Flatten all points in an array.  See HPoint<D>::flatten() for description.
// Why can't the compiler deduce D in the following method?
template <size_t D>
Vector<HPoint<D>> flatten(const Vector<HPoint<D + 1>> &input) {
  Vector<HPoint<D>> result;
  result.reserve(input.size());
  std::transform(input.cbegin(), input.cend(), std::back_inserter(result), [](HPoint<D + 1> pt) {
    return pt.flatten();
  });
  return result;
}

// Flatten all points in an array.  See HPoint<D>::flatten() for description.
// Why can't the compiler deduce D in the following method?
template <size_t D>
void flatten(const Vector<HPoint<D + 1>> &input, Vector<HPoint<D>> *result) {
  result->clear();
  result->reserve(input.size());
  std::transform(input.cbegin(), input.cend(), std::back_inserter(*result), [](HPoint<D + 1> pt) {
    return pt.flatten();
  });
}

template <>
void flatten<2>(const Vector<HPoint<3>> &input, Vector<HPoint<2>> *result);

// Extrude all points in an array.  See HPoint<D>::extrude() for description.
// Why can't the compiler deduce D in the following method?
template <size_t D>
Vector<HPoint<D>> extrude(const Vector<HPoint<D - 1>> &input) {
  Vector<HPoint<D>> result;
  result.reserve(input.size());
  std::transform(input.cbegin(), input.cend(), std::back_inserter(result), [](HPoint<D - 1> pt) {
    return pt.extrude();
  });
  return result;
}

// Extrude all points in an array.  See HPoint<D>::extrude() for description.
// Why can't the compiler deduce D in the following method?
template <size_t D>
void extrude(const Vector<HPoint<D - 1>> &input, Vector<HPoint<D>> *result) {
  result->clear();
  result->reserve(input.size());
  std::transform(input.cbegin(), input.cend(), std::back_inserter(*result), [](HPoint<D - 1> pt) {
    return pt.extrude();
  });
}

template <>
void extrude<3>(const Vector<HPoint<2>> &input, Vector<HPoint<3>> *result);

// Flatten all points in an array.  See HPoint<D>::flatten() for description.
// Why can't the compiler deduce D in the following method?
template <size_t D>
Vector<HPoint<D>> truncate(const Vector<HPoint<D + 1>> &input) {
  Vector<HPoint<D>> result;
  result.reserve(input.size());
  std::transform(input.cbegin(), input.cend(), std::back_inserter(result), [](HPoint<D + 1> pt) {
    return pt.truncate();
  });
  return result;
}

}  // namespace c8
