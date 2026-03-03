// Copyright (c) 2022 8th Wall, Inc.
// Original Author: Yuyan Song (yuyansong@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "semantics-sky-shader.h",
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
cc_end(0xab813777);

#include "c8/pixels/render/renderer.h"
#include "reality/engine/semantics/semantics-sky-shader.h"
#include "reality/engine/semantics/shaders/render-shaders.h"

namespace c8 {

void SemanticsSkyShader::initialize() {
  auto vert = embeddedSemanticsSkyVertCStr;
  auto frag = embeddedSemanticsSkyFragCStr;
#ifdef __EMSCRIPTEN__
  if (isWebGL2()) {
    vert = embeddedSemanticsSkyWebgl2VertCStr;
    frag = embeddedSemanticsSkyWebgl2FragCStr;
  }
#endif

  shader1_.initialize(
    vert,
    frag,
    {
      {"position", GlVertexAttrib::SLOT_0},
      {"uv", GlVertexAttrib::SLOT_2},
    },
    {
      {"mvp"},
      {"colorSampler"},
      {"alphaSampler"},
      {"haveSeenSky"},
      {"flipAlphaMask"},
      {"smoothness"},
    });
}

void SemanticsSkyShader::bind() { glUseProgram(shader1_.id()); }

}  // namespace c8
