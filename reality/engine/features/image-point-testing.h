// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Reusable testing code for ImagePoints

#pragma once

#include "c8/string/format.h"
#include "gmock/gmock.h"
#include "reality/engine/features/image-point.h"

namespace c8 {

// ADL method for printing in GMock.
void PrintTo(const ImagePointLocation &loc, ::std::ostream *os);

// Matcher for an ImagePointLocation, compares (x, y) position and scale.
MATCHER_P3(
  EqualsPtAndScale,
  x,
  y,
  scale,
  format("%s pt: (%g, %g) and scale: %u", negation ? "does not equal" : "equals", x, y, scale)) {
  constexpr float eps = 0.001;
  return std::abs(arg.pt.x - x) < eps && std::abs(arg.pt.y - y) < eps && arg.scale == scale;
}

}  // namespace c8
