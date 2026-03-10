// Copyright (c) 2022 8th Wall, Inc.
// Original Author: Yuyan Song (yuyansong@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "line3.h",
  };
  deps = {
    "//bzl/inliner:rules", "//c8:hpoint", "//c8:vector",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xd6a4d7e5);

#include "c8/geometry/line3.h"

#include <cmath>

namespace c8 {

static constexpr float EPSILON = 0.0000001f;

Line3::Line3(HPoint3 start, HPoint3 end) noexcept : p1_(start), p2_(end) {}

// Length of this line.
float Line3::magnitude() const noexcept {
  auto xd = p2_.x() - p1_.x();
  auto yd = p2_.y() - p1_.y();
  auto zd = p2_.z() - p1_.z();
  return std::sqrt(xd * xd + yd * yd + zd * zd);
}

bool Line3::intersects(const Line3 &other, float* muThis, float* muOther, HPoint3 *ptThis, HPoint3 *ptOther) const noexcept {
  if (!muThis || !muOther || !ptThis || !ptOther) {
    return false;
  }

  HPoint3 p3 = other.start();
  HPoint3 p4 = other.end();
  HPoint3 p13 = {p1_.x() - p3.x(), p1_.y() - p3.y(), p1_.z() - p3.z()};
  HPoint3 p43 = {p4.x() - p3.x(), p4.y() - p3.y(), p4.z() - p3.z()};
  if (std::fabs(p43.x()) < EPSILON && std::fabs(p43.y()) < EPSILON && std::fabs(p43.z()) < EPSILON) {
    return false;
  }
  HPoint3 p21 = {p2_.x() - p1_.x(), p2_.y() - p1_.y(), p2_.z() - p1_.z()};
  if (std::fabs(p21.x()) < EPSILON && std::fabs(p21.y()) < EPSILON && std::fabs(p21.z()) < EPSILON) {
    return false;
  }

  auto d1343 = p13.x() * p43.x() + p13.y() * p43.y() + p13.z() * p43.z();
  auto d4321 = p43.x() * p21.x() + p43.y() * p21.y() + p43.z() * p21.z();
  auto d1321 = p13.x() * p21.x() + p13.y() * p21.y() + p13.z() * p21.z();
  auto d4343 = p43.x() * p43.x() + p43.y() * p43.y() + p43.z() * p43.z();
  auto d2121 = p21.x() * p21.x() + p21.y() * p21.y() + p21.z() * p21.z();

  auto denom = d2121 * d4343 - d4321 * d4321;
  if (std::fabs(denom) < EPSILON) {
    return false;
  }
  auto numer = d1343 * d4321 - d1321 * d4343;

  *muThis = numer / denom;
  *muOther = (d1343 + d4321 * (*muThis)) / d4343;

  (*ptThis) = {p1_.x() + (*muThis) * p21.x(), p1_.y() + (*muThis) * p21.y(), p1_.z() + (*muThis) * p21.z()};

  (*ptOther) = {
    p3.x() + (*muOther) * p43.x(),
    p3.y() + (*muOther) * p43.y(),
    p3.z() + (*muOther) * p43.z(),
  };

  return true;
}

}  // namespace c8
