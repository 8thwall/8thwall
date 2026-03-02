// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"box3.h"};
  deps = {
    "//c8:hmatrix",
    "//c8:hpoint",
    "//c8:hvector",
    "//c8/string:format",
    "//c8:vector",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x2fc31e7b);

#include "c8/geometry/box3.h"
#include "c8/string/format.h"

namespace c8 {

// Get a box containing all points.
Box3 Box3::from(const Vector<HPoint3> &pts) {
  if (pts.empty()) {
    return {};
  }
  Box3 box = {pts[0], pts[0]};
  for (const auto &p : pts) {
    box = box.merge({p, p});
  }
  return box;
}

// Get a box containing all points.
Box3 Box3::from(const HPoint3 &center, const HVector3 &scale) {
  HPoint3 backBottomLeft = {
    center.x() - (scale.x() / 2.f), center.y() - (scale.y() / 2.f), center.z() - (scale.z() / 2.f)};
  HPoint3 frontTopRight = {
    center.x() + (scale.x() / 2.f), center.y() + (scale.y() / 2.f), center.z() + (scale.z() / 2.f)};

  return Box3::from({backBottomLeft, frontTopRight});
}

bool Box3::contains(const HPoint3 &pt) const {
  return (min.x() <= pt.x() && max.x() >= pt.x()) && (min.y() <= pt.y() && max.y() >= pt.y())
    && (min.z() <= pt.z() && max.z() >= pt.z());
}

// Check if two boxes are the same.
bool Box3::operator==(const Box3 &b) const {
  auto sameMin = min.x() == b.min.x() && min.y() == b.min.y() && min.z() == b.min.z();
  auto sameMax = max.x() == b.max.x() && max.y() == b.max.y() && max.z() == b.max.z();
  return sameMin && sameMax;
}

// Check if two boxes are different.
bool Box3::operator!=(const Box3 &b) const { return !(*this == b); }

// Get the box that contains all points of this box and another box.
Box3 Box3::merge(const Box3 &b) const {
  return {
    {std::min(min.x(), b.min.x()), std::min(min.y(), b.min.y()), std::min(min.z(), b.min.z())},
    {std::max(max.x(), b.max.x()), std::max(max.y(), b.max.y()), std::max(max.z(), b.max.z())},
  };
}

// Get the eight corners of this box.
Vector<HPoint3> Box3::corners() const {
  std::array<float, 3> mn = {min.x(), min.y(), min.z()};
  std::array<float, 3> mx = {max.x(), max.y(), max.z()};
  return {
    {mn[0], mn[1], mn[2]},
    {mn[0], mn[1], mx[2]},
    {mn[0], mx[1], mn[2]},
    {mn[0], mx[1], mx[2]},
    {mx[0], mn[1], mn[2]},
    {mx[0], mn[1], mx[2]},
    {mx[0], mx[1], mn[2]},
    {mx[0], mx[1], mx[2]},
  };
}

// Get a box that contains the points of this box with a transform applied. This may be larger than
// the original box.
Box3 Box3::transform(const HMatrix &m) const { return Box3::from(m * corners()); }

HPoint3 Box3::center() const {
  return {0.5f * (min.x() + max.x()), 0.5f * (min.y() + max.y()), 0.5f * (min.z() + max.z())};
}

bool Box3::intersects(const Box3 &b) const {
  if (b.max.x() < min.x()) {
    return false;
  }
  if (b.min.x() > max.x()) {
    return false;
  }
  if (b.max.y() < min.y()) {
    return false;
  }
  if (b.min.y() > max.y()) {
    return false;
  }
  if (b.max.z() < min.z()) {
    return false;
  }
  if (b.min.z() > max.z()) {
    return false;
  }
  return true;
}

String Box3::toString() const {
  return format(
    "{min: (x: %f, y: %f, z: %f), max: (x: %f, y: %f, z: %f)}",
    min.x(),
    min.y(),
    min.z(),
    max.x(),
    max.y(),
    max.z());
}

HVector3 Box3::dimensions() const {
  return {max.x() - min.x(), max.y() - min.y(), max.z() - min.z()};
}

// child:  0 1 2 3 4 5 6 7
// x:      - - - - + + + +
// y:      - - + + - - + +
// z:      - + - + - + - +
std::array<Box3, 8> Box3::splitOctants() const {
  auto dims = dimensions();
  std::array<float, 3> fullDims = {dims.x(), dims.y(), dims.z()};
  std::array<float, 3> halfDims = {fullDims[0] * 0.5f, fullDims[1] * 0.5f, fullDims[2] * 0.5f};
  std::array<float, 3> mn = {min.x(), min.y(), min.z()};
  std::array<float, 3> mx = {max.x(), max.y(), max.z()};
  return {
    from({{mn[0], mn[1], mn[2]}, {mn[0] + halfDims[0], mn[1] + halfDims[1], mn[2] + halfDims[2]}}),
    from(
      {{mn[0], mn[1], mn[2] + halfDims[2]},
       {mn[0] + halfDims[0], mn[1] + halfDims[1], mn[2] + fullDims[2]}}),
    from(
      {{mn[0], mn[1] + halfDims[1], mn[2]},
       {mn[0] + halfDims[0], mn[1] + fullDims[1], mn[2] + halfDims[2]}}),
    from(
      {{mn[0], mn[1] + halfDims[1], mn[2] + halfDims[2]},
       {mn[0] + halfDims[0], mn[1] + fullDims[1], mn[2] + fullDims[2]}}),
    from(
      {{mn[0] + halfDims[0], mn[1], mn[2]},
       {mn[0] + fullDims[0], mn[1] + halfDims[1], mn[2] + halfDims[2]}}),
    from(
      {{mn[0] + halfDims[0], mn[1], mn[2] + halfDims[2]},
       {mn[0] + fullDims[0], mn[1] + halfDims[1], mn[2] + fullDims[2]}}),
    from(
      {{mn[0] + halfDims[0], mn[1] + halfDims[1], mn[2]},
       {mn[0] + fullDims[0], mn[1] + fullDims[1], mn[2] + halfDims[2]}}),
    from({{mn[0] + halfDims[0], mn[1] + halfDims[1], mn[2] + halfDims[2]}, {mx[0], mx[1], mx[2]}}),
  };
}

}  // namespace c8
