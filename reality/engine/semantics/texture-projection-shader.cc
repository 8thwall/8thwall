// Copyright (c) 2022 8th Wall, Inc.
// Original Author: Yuyan Song (yuyansong@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "texture-projection-shader.h",
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
cc_end(0xe7fef38d);

#include "c8/pixels/render/renderer.h"
#include "reality/engine/semantics/shaders/render-shaders.h"
#include "reality/engine/semantics/texture-projection-shader.h"

namespace c8 {

void TextureProjectionShader::initialize() {
  auto vert = embeddedTextureProjectVertCStr;
  auto frag = embeddedTextureProjectFragCStr;
#ifdef __EMSCRIPTEN__
  if (isWebGL2()) {
    vert = embeddedTextureProjectWebgl2VertCStr;
    frag = embeddedTextureProjectWebgl2FragCStr;
  }
#endif

  shader_.initialize(
    vert, frag, {{"position", GlVertexAttrib::SLOT_0}}, {{"mvp"}, {"sourceCameraMvp"}});
}

void TextureProjectionShader::bind() { glUseProgram(shader_.id()); }

}  // namespace c8
