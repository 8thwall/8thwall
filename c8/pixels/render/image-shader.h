// Copyright (c) 2022 Niantic Labs, Inc.
// Original Author: Yuyan Song (yuyansong@nianticlabs.com)
//
// Shader to render a 2D texture

#pragma once

#include "c8/pixels/opengl/gl-program.h"

namespace c8 {

class ImageShader {
public:
  void initialize();

  // Default constructor.
  ImageShader() = default;

  void bind();

  const GlProgramObject *shader() const { return &shader1_; }

  // Disallow moving.
  ImageShader(ImageShader &&o) = delete;
  ImageShader &operator=(ImageShader &&o) = delete;

  // Disallow copying.
  ImageShader(const ImageShader &) = delete;
  ImageShader &operator=(const ImageShader &) = delete;

private:
  GlProgramObject shader1_;
};

}  // namespace c8
