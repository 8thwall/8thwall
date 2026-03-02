// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Scott Pollack (scott@8thwall.com)
//

#include "c8/hpoint.h"
#include "c8/string.h"
#include "c8/string/format.h"

#pragma once
namespace c8 {
class BundleDesc {
public:
  static constexpr int N_CAMERA_PARAMS = 6;
  static constexpr int N_CAMERA_WITH_SCALE_PARAMS = 7;
};

struct ObservedPoint {
  // Position in ray space of the point.
  HPoint2 position;
  int scale;
  float descriptorDist;
  float weight;

  String toString() const noexcept {
    return format(
      "(pos: %s, scale: %d, descriptorDist: %f, weight: %f)",
      position.toString().c_str(),
      scale,
      descriptorDist,
      weight);
  }
};
}  // namespace c8
