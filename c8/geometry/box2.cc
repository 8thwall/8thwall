// Copyright (c) 2022 8th Wall, LLC
// Original Author: Yuyan Song (yuyansong@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "box2.h",
  };
  deps = {
    "//c8:c8-log",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xe2879ca6);

#include <cmath>

#include "c8/c8-log.h"
#include "c8/geometry/box2.h"

namespace c8 {

bool doBoxesOverlap(const Box2 &bboxA, const Box2 &bboxB) {
  // check if one box is completely to the left of the other box
  if (bboxA.x >= (bboxB.x + bboxB.w) || bboxB.x >= (bboxA.x + bboxA.w)) {
    return false;
  }

  // check if one box is completely above the other box
  if (bboxA.y >= (bboxB.y + bboxB.h) || bboxB.y >= (bboxA.y + bboxA.h)) {
    return false;
  }

  return true;
}

float intersectionOverUnion(const Box2 &bboxA, const Box2 &bboxB) {
  if (!doBoxesOverlap(bboxA, bboxB)) {
    return 0.0f;
  }

  auto intersectionXMin = std::max(bboxA.x, bboxB.x);
  auto intersectionYMin = std::max(bboxA.y, bboxB.y);
  auto intersectionXMax = std::min(bboxA.x + bboxA.w, bboxB.x + bboxB.w);
  auto intersectionYMax = std::min(bboxA.y + bboxA.h, bboxB.y + bboxB.h);

  auto intersection = (intersectionXMax - intersectionXMin) * (intersectionYMax - intersectionYMin);

  auto bboxUnion = (bboxA.w * bboxA.h) + (bboxB.w * bboxB.h) - intersection;
  return intersection / bboxUnion;
}

}
