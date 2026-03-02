// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "c8/hpoint.h"
#include "c8/vector.h"

namespace c8 {

class Line2 {
public:
  // Construct a line with the specified start and end.
  Line2(HPoint2 start, HPoint2 end) noexcept;

  // Accessors.
  inline HPoint2 start() const noexcept { return p1_; }
  inline HPoint2 end() const noexcept { return p2_; }

  // Cross product of this line with another line.
  float cross(Line2 other) const noexcept;

  // Angle of this line with another line.
  float theta(Line2 other) const noexcept;

  // Length of this line.
  float magnitude() const noexcept;

  // Determine whether the two lines intersect.
  bool intersects(Line2 other) const noexcept;

  // Determine whether the specified point is on this line.
  bool intersects(HPoint2 other) const noexcept;

  // Find the distance from the specified point to this line.
  float distanceTo(HPoint2 other) const noexcept;

  // Computes "a", the projection of a point outside the line to this line,
  // such that the point "q" closest to point p in the line is given by
  // q = (1 - a) * p1 + a * p2
  // if p==p1, then a=0 and if p==p2, then a=1
  float linearProjection(HPoint2 other) const noexcept;

  // Require explicit initialization.
  Line2() = delete;

  // Default move constructors.
  Line2(Line2 &&) = default;
  Line2 &operator=(Line2 &&) = default;

  // Line is small, so allow copying.
  Line2(const Line2 &) = default;
  Line2 &operator=(Line2 &) = default;

private:
  HPoint2 p1_;
  HPoint2 p2_;
};

class Angle2 {
public:
  inline Angle2(Line2 l1, Line2 l2) noexcept : a_(l1), b_(l2), theta_(l1.theta(l2)) {}

  // Accessors.
  inline Line2 l1() const noexcept { return a_; }
  inline Line2 l2() const noexcept { return b_; }

  // Comparison operators.
  inline bool operator==(const Angle2 &b) const noexcept { return compare(b) == 0.0f; }
  inline bool operator!=(const Angle2 &b) const noexcept { return compare(b) != 0.0f; }
  inline bool operator<(const Angle2 &b) const noexcept { return compare(b) < 0.0f; }
  inline bool operator<=(const Angle2 &b) const noexcept { return compare(b) <= 0.0f; }
  inline bool operator>(const Angle2 &b) const noexcept { return compare(b) > 0.0f; }
  inline bool operator>=(const Angle2 &b) const noexcept { return compare(b) >= 0.0f; }

  // Require explicit initialization.
  Angle2() = delete;

  // Default move constructors.
  Angle2(Angle2 &&) = default;
  Angle2 &operator=(Angle2 &&) = default;

  // Angle is small, so allow copying.
  Angle2(const Angle2 &) = default;
  Angle2 &operator=(Angle2 &) = default;

private:
  inline float compare(const Angle2 &o) const noexcept { return theta_ - o.theta_; }
  Line2 a_;
  Line2 b_;
  float theta_;
};

class Poly2 {
public:
  Poly2(Vector<HPoint2> orderedPoints) noexcept;
  // Poly2 factory methods.
  static Poly2 circle(HPoint2 center, float radius, int numPoints) noexcept;
  static Poly2 convexHull(const Vector<HPoint2> &pts) noexcept;

  // Accessors.
  inline const Vector<Line2> &poly() const { return lines_; }

  bool intersects(const Poly2 &other) const noexcept;
  bool containsPointAssumingConvex(HPoint2 pt) const noexcept;
  float areaAssumingConvex() const noexcept;

  // Require explicit initialization.
  Poly2() = delete;

  // Default move constructors.
  Poly2(Poly2 &&) = default;
  Poly2 &operator=(Poly2 &&) = default;

  // Poly is large, so disallow copying.
  Poly2(const Poly2 &) = delete;
  Poly2 &operator=(Poly2 &) = delete;

private:
  Vector<Line2> lines_;
};

}  // namespace c8
