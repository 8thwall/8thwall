// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"
cc_library {
  hdrs = {
    "face-types.h",
  };
  deps = {
    "//c8:hpoint",
    "//c8:hmatrix",
    "//c8:hvector",
    "//c8:quaternion",
    "//c8:string",
    "//c8:vector",
    "//c8/geometry:box2",
    "//c8/geometry:pixel-pinhole-camera-model",
    "//c8/pixels:image-roi",
    "//c8/pixels:pixel-buffer",
    "//c8/pixels:pixels",
    "//reality/engine/api:face.capnp-cc",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x753ea551);

#include "c8/geometry/face-types.h"

namespace c8 {

ConstRGBA8888PlanePixels imageForViewport(ConstRGBA8888PlanePixels p, ImageViewport vp) {
  return {
    static_cast<int>(vp.h),
    static_cast<int>(vp.w),
    p.rowBytes(),
    p.pixels() + static_cast<int>(vp.y) * p.rowBytes() + static_cast<int>(vp.x) * 4,
  };
}

}  // namespace c8
