// Copyright (c) 2022 8th Wall, Inc.
// Original Author: Yuyan Song (yuyansong@nianticlabs.com)
//
// Shader to post-process semantics result specifically for sky segmentation

#pragma once

#include "c8/pixels/opengl/gl-program.h"

namespace c8 {

// Comment 1: Here we are looking at an unknown pixel above the horizon. We know it's above the
// horizon b/c only unknown pixels above the horizon have rg = 1. And we know it's unknown b/c it
// has b = 0 (once it's known it will have b = 1). For this pixel:
// - If we have seen the sky before (haveSeenSky > 0) then we're outdoors and assume return 1.0
//   for fully unknown pixels (b == 0.0).
// - Else we've never seen the sky before so we're indoors and should return 0.0, including pixels
//   that have seen/unseen ambiguities (b != 1.0).

// Comment 2:
// for edge feathering, we will do linear interpolation between raw semantics output
// and blurred output

class SemanticsSkyShader {
public:
  void initialize();

  // Default constructor.
  SemanticsSkyShader() = default;

  void bind();

  const GlProgramObject *shader() const { return &shader1_; }

  // Disallow moving.
  SemanticsSkyShader(SemanticsSkyShader &&o) = delete;
  SemanticsSkyShader &operator=(SemanticsSkyShader &&o) = delete;
  // Disallow copying.
  SemanticsSkyShader(const SemanticsSkyShader &) = delete;
  SemanticsSkyShader &operator=(const SemanticsSkyShader &) = delete;

private:
  GlProgramObject shader1_;
};

}  // namespace c8
