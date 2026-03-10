// Copyright (c) 2022 8th Wall, Inc.
// Original Author: Yuyan Song (yuyansong@nianticlabs.com)
//
// Shader to project semantics inference results into semantics sky cubemap.

#pragma once

#include "c8/pixels/opengl/gl-program.h"

namespace c8 {

class SemanticsProjectionShader {
public:
  void initialize();

  // Default constructor.
  SemanticsProjectionShader() = default;

  void bind();

  const GlProgramObject *shader() const { return &shader1_; }

  // Disallow moving.
  SemanticsProjectionShader(SemanticsProjectionShader &&o) = delete;
  SemanticsProjectionShader &operator=(SemanticsProjectionShader &&o) = delete;
  // Disallow copying.
  SemanticsProjectionShader(const SemanticsProjectionShader &) = delete;
  SemanticsProjectionShader &operator=(const SemanticsProjectionShader &) = delete;

private:
  GlProgramObject shader1_;
};

}  // namespace c8
