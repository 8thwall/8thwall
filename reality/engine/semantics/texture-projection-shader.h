// Copyright (c) 2022 8th Wall, Inc.
// Original Author: Yuyan Song (yuyansong@nianticlabs.com)
//
// Shader to project a texture from a source camera's view frustum onto a geometry seen by another
// camera's view frustum.

#pragma once

#include "c8/pixels/opengl/gl-program.h"

namespace c8 {

class TextureProjectionShader {
public:
  void initialize();

  // Default constructor.
  TextureProjectionShader() = default;

  void bind();

  const GlProgramObject *shader() const { return &shader_; }

  // Disallow moving.  TODO(nb): switch to moveable types.
  TextureProjectionShader(TextureProjectionShader &&o) = delete;
  TextureProjectionShader &operator=(TextureProjectionShader &&o) = delete;
  // Disallow copying.
  TextureProjectionShader(const TextureProjectionShader &) = delete;
  TextureProjectionShader &operator=(const TextureProjectionShader &) = delete;

private:
  GlProgramObject shader_;

  static char const *TEX_PROJ_VERTEX_SHADER_CODE;
  static char const *TEX_PROJ_FRAGMENT_SHADER_CODE;
};

}  // namespace c8
