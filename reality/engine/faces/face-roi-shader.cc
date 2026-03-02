// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "face-roi-shader.h",
    "face-roi-shader-data.h",
  };
  deps = {
    "//c8:c8-log",
    "//c8/pixels/opengl:gl-headers",
    "//c8/pixels/opengl:gl-program",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x5120d749);

#include "c8/c8-log.h"
#include "reality/engine/faces/face-roi-shader-data.h"
#include "reality/engine/faces/face-roi-shader.h"

namespace c8 {

void FaceRoiShader::initialize() {
  shader1_.initialize(
    ROTATE_SHADER_CODE,
    STAGE_1_SHADER_CODE,
    {{"position", GlVertexAttrib::SLOT_0}, {"uv", GlVertexAttrib::SLOT_2}},
    {{"rotation"}, {"flipY"}, {"mvp"}, {"clear"}});
}

void FaceRoiShader::bind(const GlProgramObject *shader) { glUseProgram(shader->id()); }

}  // namespace c8
