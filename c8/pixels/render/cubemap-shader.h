// Copyright (c) 2022 Niantic Labs, Inc.
// Original Author: Yuyan Song (yuyansong@nianticlabs.com)
//
// Shader to render a Cubemap texture

#pragma once

#include "c8/pixels/opengl/gl-program.h"

namespace c8 {

class CubemapShader {
public:
  void initialize();

  // Default constructor.
  CubemapShader() = default;

  void bind();

  const GlProgramObject *shader() const { return &shader1_; }

  // Disallow moving.
  CubemapShader(CubemapShader &&o) = delete;
  CubemapShader &operator=(CubemapShader &&o) = delete;

  // Disallow copying.
  CubemapShader(const CubemapShader &) = delete;
  CubemapShader &operator=(const CubemapShader &) = delete;

private:
  GlProgramObject shader1_;
};

}  // namespace c8
