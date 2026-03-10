
#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "gr8-pyramid.h",
  };
  deps = {
    "//c8:hmatrix",
    "//c8:vector",
    "//c8/pixels:image-roi",
    "//c8/pixels:pixel-buffer",
    "//c8/pixels:pixels",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xfa293514);

#include "c8/pixels/gr8-pyramid.h"

namespace c8 {

Vector<std::array<float, 5>> Gr8Pyramid::mapLevelsToBase() const {
  Vector<std::array<float, 5>> layerScale(levels.size());
  auto b = levels[0];
  for (int level = 0; level < levels.size(); level++) {
    auto l = levels[level];
    // If level is rotated, compare h to w instead of h to h
    float lw = (l.rotated != b.rotated) ? l.h : l.w;
    float lh = (l.rotated != b.rotated) ? l.w : l.h;

    // To scale pixel x, y to base image, we need to compute:
    // baseX = (x + 0.5) * b.w / l.w - 0.5
    //       = x * (b.w / l.w) + (0.5 * ((b.w / l.w) - 1))
    //       = x * scaleFactor + pixelCenterOffset
    // Similarly for y.

    // Compute scale factor.
    layerScale[level][0] = b.w / lw;
    layerScale[level][2] = b.h / lh;
    // Compute pixel center offsets.
    layerScale[level][1] = 0.5f * (layerScale[level][0] - 1.0f);
    layerScale[level][3] = 0.5f * (layerScale[level][2] - 1.0f);
    // Compute the mean of the scaling factors as a rough estimate of the overall scaling factor.
    layerScale[level][4] = 0.5f * (layerScale[level][0] + layerScale[level][2]);
  }
  return layerScale;
}
}  // namespace c8
