// Copyright (c) 2023 8th Wall, Inc.
// Original Author: Yuyan Song (yuyansong@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "semantics-gaussian-linear-shader.h",
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
cc_end(0x586c4e61);

#include "c8/pixels/render/renderer.h"
#include "reality/engine/semantics/semantics-gaussian-linear-shader.h"
#include "reality/engine/semantics/shaders/render-shaders.h"

namespace c8 {

void SemanticsGaussianLinearShader::initialize() {
  auto vert = embeddedSemanticsSkyVertCStr;
  auto frag = embeddedSemanticsGaussianLinearFragCStr;
#ifdef __EMSCRIPTEN__
  if (isWebGL2()) {
    vert = embeddedSemanticsSkyWebgl2VertCStr;
    frag = embeddedSemanticsGaussianLinearWebgl2FragCStr;
  }
#endif

  shader1_.initialize(
    vert,
    frag,
    {
      {"position", GlVertexAttrib::SLOT_0},
      {"uv", GlVertexAttrib::SLOT_2},
    },
    {
      {"mvp"},
      {"colorSampler"},
      {"uvStep"},
      {"kernelSize"},
      {"kWeight"},
    });
}

void SemanticsGaussianLinearShader::bind() { glUseProgram(shader1_.id()); }

void SemanticsGaussianLinearShader::setGaussianLinearKernels(int kernelSize, float sigma) {
  if (kernelSize > maxKernelSize_) {
    C8_THROW("[Gaussian] kernel size is larger than the max kernel dimension.");
  }

  for (int i = 0; i < maxKernelSize_; ++i) {
    kernel_[i] = 0;
  }

  kernelSize_ = kernelSize;
  const int mean = kernelSize / 2;
  float sum = 0;
  for (int x = 0; x < kernelSize; ++x) {
    kernel_[x] = std::exp(-0.5 * std::pow((x - mean) / sigma, 2.0));
    sum += kernel_[x];
  }

  for (int x = 0; x < kernelSize; ++x) {
    kernel_[x] /= sum;
  }
}

}  // namespace c8
