// Copyright (c) 2023 8th Wall, Inc.
// Original Author: Yuyan Song (yuyansong@nianticlabs.com)
//
// Shader to post-process semantics result for Gaussian blur with linear sampling

#pragma once

#include "c8/pixels/opengl/gl-program.h"

namespace c8 {

// 2D Gaussian blur with linear sampling

class SemanticsGaussianLinearShader {
public:
  void initialize();

  void setGaussianLinearKernels(int kernelSize, float sigma);

  // Default constructor.
  SemanticsGaussianLinearShader() = default;

  void bind();

  const GlProgramObject *shader() const { return &shader1_; }

  int getKernelSize() const { return kernelSize_; }

  const float *getKernelWeights() const { return kernel_; }

  // Disallow moving.
  SemanticsGaussianLinearShader(SemanticsGaussianLinearShader &&o) = delete;
  SemanticsGaussianLinearShader &operator=(SemanticsGaussianLinearShader &&o) = delete;
  // Disallow copying.
  SemanticsGaussianLinearShader(const SemanticsGaussianLinearShader &) = delete;
  SemanticsGaussianLinearShader &operator=(const SemanticsGaussianLinearShader &) = delete;

private:
  GlProgramObject shader1_;

  int kernelSize_ = 0;

  // Max dimension for this 1D kernel is 13, which should match the MAX_KERNEL_SIZE constant
  // in semantics-gaussian-linear.frag shader file.
  // semantics-gaussian-linear.frag shader has the convolution computation loop unrolled in order to
  // be compiled as a GLSL 1.0 shader.
  // If the kernel size is changed, the unrolled loop in semantics-gaussian-linear.frag shader file
  // should be updated accordingly.
  static constexpr int maxKernelSize_ = 13;
  float kernel_[maxKernelSize_];
};

}  // namespace c8
