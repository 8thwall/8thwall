// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "c8/pixels/opengl/gl-program.h"
#include "reality/engine/features/gr8-defs.h"

namespace c8 {

class Gr8FeatureShader {
public:
  void initialize();

  Gr8FeatureShader() = default;

  void setFeatureGain(float gain);
  void setFeatureParams(float, float scoreThresh, bool, float nmsTolerance);

  void bindAndSetParams(const GlProgramObject *shader);

  const GlProgramObject *shaderInput() const { return &shaderInput_; }
  const GlProgramObject *shaderPyramidBlur() const { return &shaderPyramidBlur_; }
  const GlProgramObject *shaderPyramidDecimate() const { return &shaderPyramidDecimate_; }
#if C8_USE_ALTERNATE_FEATURE_SCORE
  const GlProgramObject *shaderGradient() const { return &shaderGradient_; }
  const GlProgramObject *shaderMomentX() const { return &shaderMomentX_; }
  const GlProgramObject *shaderMomentY() const { return &shaderMomentY_; }
#endif
  const GlProgramObject *shader1() const { return &shader1_; }
  const GlProgramObject *shader2() const { return &shader2_; }
  const GlProgramObject *shader3() const { return &shader3_; }

  // Disallow moving.  TODO(nb): switch to moveable types.
  Gr8FeatureShader(Gr8FeatureShader &&o) = delete;
  Gr8FeatureShader &operator=(Gr8FeatureShader &&o) = delete;
  // Disallow copying.
  Gr8FeatureShader(const Gr8FeatureShader &) = delete;
  Gr8FeatureShader &operator=(const Gr8FeatureShader &) = delete;

private:
  bool needsCleanup_;
  GlProgramObject shaderInput_;
  GlProgramObject shaderPyramidBlur_;
  GlProgramObject shaderPyramidDecimate_;
#if C8_USE_ALTERNATE_FEATURE_SCORE
  GlProgramObject shaderGradient_;
  GlProgramObject shaderMomentX_;
  GlProgramObject shaderMomentY_;
#endif
  GlProgramObject shader1_;
  GlProgramObject shader2_;
  GlProgramObject shader3_;

  // Shader constants.
  float featureGain_ = 1.0f;   // Should be 1 in production, can be higher for debugging.
  float scoreThresh_ = 4.0f;   // Minimum acceptable corner score.
  float nmsTolerance_ = 1.0f;  // 0 is NMS, 1 is NMES, 255 is no NMS.

  char const *VERTEX_SHADER_CODE;
  char const *ROTATE_SHADER_CODE;
  char const *ROTATE_ONLY_SHADER_CODE;
  char const *PYRAMID_BLUR_SHADER_CODE;
  char const *PYRAMID_DECIMATE_SHADER_CODE;
#if C8_USE_ALTERNATE_FEATURE_SCORE
  char const *GRADIENT_SHADER_CODE;
  char const *MOMENT_X_SHADER_CODE;
  char const *MOMENT_Y_SHADER_CODE;
#endif
  char const *INPUT_SHADER_CODE;
  char const *STAGE_1_SHADER_CODE;
  char const *STAGE_2_SHADER_CODE;
  char const *STAGE_3_SHADER_CODE;
};

}  // namespace c8
