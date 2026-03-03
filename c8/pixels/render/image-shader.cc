// Copyright (c) 2022 Niantic Labs, Inc.
// Original Author: Yuyan Song (yuyansong@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "image-shader.h",
  };
  deps = {
    "//c8:c8-log",
    "//c8/pixels/opengl:gl-headers",
    "//c8/pixels/opengl:gl-program",
    "//c8/pixels/render:renderer",
    "//c8/pixels/render/shaders:render-shaders",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xd6a248fb)

#include "c8/pixels/render/image-shader.h"
#include "c8/pixels/render/renderer.h"
#include "c8/pixels/render/shaders/render-shaders.h"

  namespace c8 {

  void ImageShader::initialize() {
    auto vert = embeddedImageVertCStr;
    auto frag = embeddedImageFragCStr;
#ifdef __EMSCRIPTEN__
    if (isWebGL2()) {
      vert = embeddedImageWebgl2VertCStr;
      frag = embeddedImageWebgl2FragCStr;
    }
#endif

    shader1_.initialize(
      vert,
      frag,
      {{"position", GlVertexAttrib::SLOT_0}, {"uv", GlVertexAttrib::SLOT_2}},
      {{"mvp"}, {"opacity"}});
  }

  void ImageShader::bind() { glUseProgram(shader1_.id()); }

}  // namespace c8
