// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "two-d.h",
  };
  deps = {
    "//c8:hpoint",
    "//c8:vector",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x11098da1);

#include <cmath>
#include <functional>
#include <queue>

#include "c8/geometry/two-d.h"

#define PI_F 3.14159265f

using namespace c8;

namespace {

constexpr int sign(float a) noexcept { return a == 0 ? 0 : (a < 0 ? -1 : 1); }

}  // namespace

Line2::Line2(HPoint2 start, HPoint2 end) noexcept : p1_(start), p2_(end) {}

// Cross product of this line with another line.
float Line2::cross(Line2 other) const noexcept {
  auto xd = p2_.x() - p1_.x();
  auto yd = p2_.y() - p1_.y();
  auto oxd = other.p2_.x() - other.p1_.x();
  auto oyd = other.p2_.y() - other.p1_.y();
  return xd * oyd - yd * oxd;
}

// Angle of this line with another line.
float Line2::theta(Line2 other) const noexcept {
  auto xd = p2_.x() - p1_.x();
  auto yd = p2_.y() - p1_.y();
  auto oxd = other.p2_.x() - other.p1_.x();
  auto oyd = other.p2_.y() - other.p1_.y();

  auto m = magnitude();
  auto om = other.magnitude();

  auto a = std::asin(cross(other) / (m * om));

  Line2 l(HPoint2(xd, yd), HPoint2(oxd, oyd));
  if (l.magnitude() > std::sqrt(m * m + om * om)) {
    a = a >= 0 ? PI_F - a : PI_F + a;
  }

  if (a < 0) {
    a = a + 2.0f * PI_F;
  }

  return a;
}

// Length of this line.
float Line2::magnitude() const noexcept {
  auto xd = p2_.x() - p1_.x();
  auto yd = p2_.y() - p1_.y();
  return std::sqrt(xd * xd + yd * yd);
}

// Determine whether the two lines intersect.
bool Line2::intersects(Line2 other) const noexcept {
  auto ab = *this;
  auto cd = other;
  auto ca = Line2(cd.p1_, ab.p1_);
  auto cb = Line2(cd.p1_, ab.p2_);
  auto ac = Line2(ab.p1_, cd.p1_);
  auto ad = Line2(ab.p1_, cd.p2_);

  return (sign(cd.cross(ca)) != sign(cd.cross(cb))) && (sign(ab.cross(ac) != sign(ab.cross(ad))));
}

// Determine whether the specified point is on this line.
bool Line2::intersects(HPoint2 p) const noexcept {
  auto x = p.x();
  auto y = p.y();
  auto x1 = p1_.x();
  auto y1 = p1_.y();
  auto x2 = p2_.x();
  auto y2 = p2_.y();

  if ((x < x1 && x < x2) || (x > x1 && x > x2)) {
    return false;
  }

  if ((y < y1 && y < y2) || (y > y1 && y > y2)) {
    return false;
  }

  return (y - y1) == (y1 - y2) / (x1 - x2) * (x - x1);
}

// Find the distance from the specified point to this line.
float Line2::distanceTo(HPoint2 other) const noexcept {
  auto a = p2_.x() - p1_.x();
  auto b = p2_.y() - p1_.y();
  auto len = magnitude();
  a = a / len;
  b = b / len;

  auto c = other.x() - p1_.x();
  auto d = other.y() - p1_.y();
  auto proj = c * a + d * b;
  proj = proj < 0 ? 0 : proj;
  proj = proj > len ? len : proj;
  c = c - a * proj;
  d = d - b * proj;

  Line2 f(HPoint2(0.0f, 0.0f), HPoint2(c, d));
  return f.magnitude();
}

float Line2::linearProjection(HPoint2 other) const noexcept {
  float vx = p2_.x() - p1_.x();
  float vy = p2_.y() - p1_.y();
  float norm = std::sqrt(vx * vx + vy * vy);
  if (norm < 0.00005f) {
    return 0.0f;
  }
  vx /= norm;
  vy /= norm;
  float a = (other.x() - p1_.x()) * vx + (other.y() - p1_.y()) * vy;
  a /= norm;
  return a;
}

Poly2::Poly2(Vector<HPoint2> orderedPoints) noexcept {
  auto s = orderedPoints.size();
  for (int i = 0; i < s; ++i) {
    lines_.push_back(Line2(orderedPoints[i], orderedPoints[(i + 1) % s]));
  }
}

// Poly2 factory methods.
Poly2 Poly2::circle(HPoint2 center, float radius, int numPoints) noexcept {
  Vector<HPoint2> pts;
  for (int i = 0; i < numPoints; ++i) {
    float theta = 2.0f * PI_F / numPoints;
    pts.push_back(
      HPoint2(radius * std::cos(theta) + center.x(), radius * std::sin(theta) + center.y()));
  }
  return Poly2(pts);
}

Poly2 Poly2::convexHull(const Vector<HPoint2> &pts) noexcept {
  if (pts.size() < 2) {
    return Poly2(pts);
  }

  // Find the lower left point of the input set.
  // A point is the min if it has the lowest y position, breaking ties by the lowest x.
  auto minpt = pts[0];
  int index = 0;
  for (int i = 1; i < pts.size(); ++i) {
    auto pt = pts[i];
    if (pt.y() < minpt.y() || (pts[i].y() == minpt.y() && pts[i].x() < minpt.x())) {
      minpt = pts[i];
      index = i;
    }
  }

  // Find the point with the minimum angle to the lower left point.
  Line2 b(minpt, HPoint2(minpt.x() + 1.0f, minpt.y()));
  auto mintheta = PI_F + 1.0f;

  int index2 = index == 0 ? 1 : 0;
  for (int i = 0; i < pts.size(); ++i) {
    if (i == index) {
      continue;
    }
    Line2 l(minpt, pts[i]);
    auto theta = b.theta(l);
    if (theta < mintheta) {
      mintheta = theta;
      index2 = i;
    }
  }

  Line2 base(minpt, pts[index2]);

  // Create a priority queue sorted by min angle to the base.
  std::priority_queue<Angle2, Vector<Angle2>, std::greater<Angle2>> angles;
  for (int i = 0; i < pts.size(); ++i) {
    if (i == index || i == index2) {
      continue;
    }
    Line2 l(minpt, pts[i]);
    angles.push(Angle2(base, l));
  }

  // Iterate counter clockwise through points in relation to the min point, and determine if a point
  // is on the hull.
  Vector<Line2> stack;
  stack.push_back(base);
  while (angles.size() > 0) {
    auto first = stack.back();
    auto l = angles.top().l2();
    Line2 second(first.end(), l.end());
    if (first.cross(second) > 0) {
      stack.push_back(second);
      angles.pop();
    } else {
      stack.pop_back();
      if (stack.empty()) {
        stack.push_back(l);
        angles.pop();
      }
    }
  }

  Vector<HPoint2> hullPts;
  hullPts.push_back(stack.back().end());

  auto sz = stack.size();
  for (int i = 0; i < sz; ++i) {
    hullPts.push_back(stack.back().start());
    stack.pop_back();
  }

  return Poly2(hullPts);
}

bool Poly2::intersects(const Poly2 &other) const noexcept {
  for (auto l : lines_) {
    for (auto ol : other.lines_) {
      if (l.intersects(ol)) {
        return true;
      }
    }
  }
  return false;
}

bool Poly2::containsPointAssumingConvex(HPoint2 pt) const noexcept {
  if (lines_.size() < 2) {
    return false;
  }

  int s = 0;
  bool first = true;

  for (auto l1 : lines_) {
    Line2 l2(l1.start(), pt);
    auto thisS = sign(l1.cross(l2));
    if (first) {
      s = thisS;
      first = false;
    }

    if (s != thisS) {
      return false;
    }
  }
  return true;
}

float Poly2::areaAssumingConvex() const noexcept {
  float area = 0.0f;
  for (int i = 1; i < lines_.size() - 1; ++i) {
    Line2 l1(lines_[0].start(), lines_[i].start());
    Line2 l2(lines_[0].start(), lines_[i + 1].start());
    area += std::abs(l1.cross(l2) / 2.0f);
  }
  return area;
}
