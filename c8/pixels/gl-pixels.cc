// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nathan Waters (nathan@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "gl-pixels.h",
  };
  deps = {
    "//c8/pixels:pixels",
    "//c8/pixels/opengl:gl-texture",
    "//c8/stats:scope-timer",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xd88f1ee1);

#include "c8/pixels/opengl/gl-texture.h"
#include "c8/pixels/pixels.h"
#include "c8/stats/scope-timer.h"

namespace c8 {

GlTexture2D readImageToLinearTexture(ConstRGBA8888PlanePixels p) {
  ScopeTimer t("read-image-to-linear-texture");
  GlTexture2D srcTexture = makeLinearRGBA8888Texture2D(p.cols(), p.rows());
  srcTexture.tex().bind();
  srcTexture.tex().setPixels(p.pixels());
  srcTexture.tex().unbind();
  return srcTexture;
}

GlTexture2D readImageToNearestTexture(ConstRGBA8888PlanePixels p) {
  ScopeTimer t("read-image-to-nearest-texture");
  GlTexture2D srcTexture = makeNearestRGBA8888Texture2D(p.cols(), p.rows());
  srcTexture.tex().bind();
  srcTexture.tex().setPixels(p.pixels());
  srcTexture.tex().unbind();
  return srcTexture;
}

}  // namespace c8
