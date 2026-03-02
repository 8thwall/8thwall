// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "image-point-testing.h",
  };
  deps = {
    ":image-point",
    "//c8/string:format",
    "@com_google_googletest//:gtest_main",
  };
  testonly = 1;
}
cc_end(0x201fb865);

#include "reality/engine/features/image-point-testing.h"

#include "c8/string/format.h"
#include "gmock/gmock.h"
#include "reality/engine/features/image-point.h"

namespace c8 {

void PrintTo(const ImagePointLocation &loc, ::std::ostream *os) {
  *os << format(
    "{pt: (%g, %g), scale: %u, size: %g, angle: %g, response: %g, roi: %d}",
    loc.pt.x,
    loc.pt.y,
    loc.scale,
    loc.size,
    loc.angle,
    loc.response,
    loc.roi);
}

}  // namespace c8
