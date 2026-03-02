// Copyright (c) 2022 8th Wall, Inc.
// Original Author: Yuyan Song (yuyansong@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "semantics-projection-shader.h",
  };
  deps = {
    "//c8:c8-log",
    "//c8/pixels/opengl:gl-headers",
    "//c8/pixels/opengl:gl-program",
    "//c8/pixels/render:renderer",
    "//reality/engine/semantics/shaders:render-shaders",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xdafd2b25);

#include "c8/pixels/render/renderer.h"
#include "reality/engine/semantics/semantics-projection-shader.h"
#include "reality/engine/semantics/shaders/render-shaders.h"

namespace c8 {

void SemanticsProjectionShader::initialize() {
  auto vert = embeddedSemanticsProjectVertCStr;
  auto frag = embeddedSemanticsProjectFragCStr;
#ifdef __EMSCRIPTEN__
  if (isWebGL2()) {
    vert = embeddedSemanticsProjectWebgl2VertCStr;
    frag = embeddedSemanticsProjectWebgl2FragCStr;
  }
#endif

  shader1_.initialize(
    vert,
    frag,
    {{"position", GlVertexAttrib::SLOT_0}, {"uv", GlVertexAttrib::SLOT_2}},
    {
      {"mvp"},
      {"sourceCameraMvp"},
      {"colorSampler"},
      {"semSampler"},
    });
}

void SemanticsProjectionShader::bind() { glUseProgram(shader1_.id()); }

}  // namespace c8
