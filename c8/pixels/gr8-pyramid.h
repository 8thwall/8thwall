#pragma once

#include <array>
#include "c8/hmatrix.h"
#include "c8/pixels/image-roi.h"
#include "c8/pixels/pixel-buffer.h"
#include "c8/pixels/pixels.h"
#include "c8/vector.h"
namespace c8 {

struct LevelLayout {
  int r = 0;
  int c = 0;
  int w = 0;
  int h = 0;
  bool rotated = false;
};

struct ROI {
  ImageRoi roi;
  LevelLayout layout;
};

struct Gr8Pyramid {
  ConstRGBA8888PlanePixels data;
  Vector<LevelLayout> levels;
  Vector<ROI> rois;

  ConstRGBA8888PlanePixels level(int i) const {
    return ConstRGBA8888PlanePixels(
      levels[i].h,
      levels[i].w,
      data.rowBytes(),
      data.pixels() + levels[i].r * data.rowBytes() + 4 * levels[i].c);
  }

  ConstRGBA8888PlanePixels roi(int i) const {
    return ConstRGBA8888PlanePixels(
      rois[i].layout.h,
      rois[i].layout.w,
      data.rowBytes(),
      data.pixels() + rois[i].layout.r * data.rowBytes() + 4 * rois[i].layout.c);
  }

  // Compute the parameters needed to map pixels in each scale to the base level.
  // The mapping can be computed by:
  //     pt.x = pt.x * layerScale[pt.scale][0] + layerScale[pt.scale][1];
  //     pt.pt.y = pt.pt.y * layerScale[pt.scale][2] + layerScale[pt.scale][3];
  // The 4th element of the mapping is the average scaling factor of both dimensions.
  Vector<std::array<float, 5>> mapLevelsToBase() const;
};
}  // namespace c8
