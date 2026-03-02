// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Dat Chu (dat@8thwall.com)
#pragma once

#include <utility>
#include "reality/engine/features/image-point.h"

namespace c8 {
struct KeypointResponseMore {
  inline bool operator()(const ImagePointLocation &kp1, const ImagePointLocation &kp2) const {
    return kp1.response > kp2.response;
  }
};
struct KeypointResponsePairMore {
  inline bool operator()(
    const std::pair<int, ImagePointLocation> &kp1,
    const std::pair<int, ImagePointLocation> &kp2) const {
    return kp1.first != kp2.first ? kp1.first < kp2.first
                                  : kp1.second.response > kp2.second.response;
  }
};
}  // namespace c8
