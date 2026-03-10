// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Dat Chu (dat@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "line.h"
  };
  deps = {
    "//c8:hpoint",
    "//c8:hvector",
  };
  copts = {
    "-D_USE_MATH_DEFINES",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x9167e686);
#include <cmath>

#include "c8/geometry/line.h"

namespace c8 {

HVector2 line(HPoint2 from, HPoint2 to) {
  HVector2 line = {to.x() - from.x(), to.y() - from.y()};
  return line.unit();
}

HVector3 line(HPoint3 from, HPoint3 to) {
  HVector3 line = {to.x() - from.x(), to.y() - from.y(), to.z() - from.z()};
  return line.unit();
}

bool pointOnLineBetween(HPoint3 bottomPt, HPoint3 midPt, HPoint3 topPt) {
  HVector3 midToBottom = line(midPt, bottomPt);
  HVector3 topToBottom = line(topPt, bottomPt);
  return (topToBottom - midToBottom).l2Norm() < 1e-6;
}

bool linesPerpendicular(HVector3 line1, HVector3 line2) {
  return std::abs(line1.dot(line2)) < 1e-6;
}

float angleBetweenABAndAC(HPoint2 A, HPoint2 B, HPoint2 C) {
  HVector2 AB = line(A, B);
  HVector2 AC = line(A, C);
  float angle = std::atan2(AC.y(), AC.x()) - atan2(AB.y(), AB.x());
  if (angle < 0) {
    angle += 2 * M_PI;
  }
  return angle;
}

HPoint2 rotateCW(const HPoint2 &pt, int height) {
  return {height - 1 - pt.y(), pt.x()};
}

HPoint2 rotateCCW(const HPoint2 &pt, int width) {
  return {pt.y(), width - 1 - pt.x()};
}

}
