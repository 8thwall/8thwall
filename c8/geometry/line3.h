// Copyright (c) 2022 8th Wall, Inc.
// Original Author: Yuyan Song (yuyansong@nianticlabs.com)
//
// 3D line geometry

#pragma once

#include "c8/hpoint.h"

namespace c8 {

class Line3 {
public:
  // Construct a line with the specified start and end.
  Line3(HPoint3 start, HPoint3 end) noexcept;

  // Accessors.
  inline HPoint3 start() const noexcept { return p1_; }
  inline HPoint3 end() const noexcept { return p2_; }

  // Length of this line.
  float magnitude() const noexcept;

  // Determine whether the two lines intersect in 3D space.
  // Calculate the points on this line and other line so that the distance between the two points
  // are minimum. If the point pair exists, return true.
  // @param other Other line
  // @param muThis The interpolation coefficient between p1_ and p2_ on this line.
  // ptThis = p1_ + muThis * (p2_ - p1_)
  // @param muOther The interpolation coefficient between other.p1_ and other.p2_ on the other line.
  // ptOther = other.p1_ + muOther * (other.p2_ - other.p1_)
  // @param ptThis The point on this line that is closest to the other line.
  // ptThis = p1_ + muThis * (p2_ - p1_)
  // @param ptOther The point on the other line that is closest to this line.
  // ptOther = other.p1_ + muOther * (other.p2_ - other.p1_)
  bool intersects(
    const Line3 &other,
    float *muThis,
    float *muOther,
    HPoint3 *ptThis,
    HPoint3 *ptOther) const noexcept;

  // Require explicit initialization.
  Line3() = delete;

  // Default move constructors.
  Line3(Line3 &&) = default;
  Line3 &operator=(Line3 &&) = default;

  // Line3 is small, so allow copying.
  Line3(const Line3 &) = default;
  Line3 &operator=(Line3 &) = default;

private:
  HPoint3 p1_;
  HPoint3 p2_;
};

}  // namespace c8
