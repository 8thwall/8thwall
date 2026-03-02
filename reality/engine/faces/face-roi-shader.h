// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "c8/pixels/opengl/gl-program.h"

namespace c8 {

class FaceRoiShader {
public:
  void initialize();

  FaceRoiShader() = default;

  void bind(const GlProgramObject *shader);

  const GlProgramObject *shader1() const { return &shader1_; }

  // Disallow moving.  TODO(nb): switch to moveable types.
  FaceRoiShader(FaceRoiShader &&o) = delete;
  FaceRoiShader &operator=(FaceRoiShader &&o) = delete;
  // Disallow copying.
  FaceRoiShader(const FaceRoiShader &) = delete;
  FaceRoiShader &operator=(const FaceRoiShader &) = delete;

private:
  GlProgramObject shader1_;

  static char const *VERTEX_SHADER_CODE;
  static char const *ROTATE_SHADER_CODE;
  static char const *STAGE_1_SHADER_CODE;
};

}  // namespace c8
