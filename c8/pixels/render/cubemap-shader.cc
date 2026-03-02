// Copyright (c) 2022 Niantic Labs, Inc.
// Original Author: Yuyan Song (yuyansong@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "cubemap-shader.h",
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
cc_end(0x896c78f8)

#include "c8/pixels/render/cubemap-shader.h"
#include "c8/pixels/render/renderer.h"
#include "c8/pixels/render/shaders/render-shaders.h"

  namespace c8 {

  void CubemapShader::initialize() {
    auto vert = embeddedCubemapVertCStr;
    auto frag = embeddedCubemapFragCStr;
#ifdef __EMSCRIPTEN__
    if (isWebGL2()) {
      vert = embeddedCubemapWebgl2VertCStr;
      frag = embeddedCubemapWebgl2FragCStr;
    }
#endif

    shader1_.initialize(
      vert, frag, {{"position", GlVertexAttrib::SLOT_0}}, {{"mvp"}, {"opacity"}, {"colorSampler"}});
  }

  void CubemapShader::bind() { glUseProgram(shader1_.id()); }

}  // namespace c8
