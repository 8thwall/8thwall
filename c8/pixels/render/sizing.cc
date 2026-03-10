// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Dat Chu (dat@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"sizing.h"};
  deps = {};
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x5ca41259);
#include "c8/pixels/render/sizing.h"

namespace c8 {
Rect SceneSize::operator()(int colIdx, int rowIdx, int numWidths, int numHeights) const {
  return {colIdx * CAM_WIDTH, rowIdx * CAM_HEIGHT, numWidths * CAM_WIDTH, numHeights * CAM_HEIGHT};
}

// Get a span of columns for row 0
Rect SceneSize::row0(int row0StartIdx, int numWidths) const {
  return {row0StartIdx * CAM_WIDTH, 0, numWidths * CAM_WIDTH, CAM_HEIGHT};
}

// Get a span of columns for row 1
Rect SceneSize::row1(int row1StartIdx, int numWidths) const {
  return {row1StartIdx * CAM_WIDTH, CAM_HEIGHT, numWidths * CAM_WIDTH, CAM_HEIGHT};
}
}  // namespace c8
