// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "gr8-feature-shader.h",
  };
  deps = {
    ":embedded-shaders",
    ":gr8-defs",
    "//c8:c8-log",
    "//c8/pixels/opengl:gl-headers",
    "//c8/pixels/opengl:gl-program",
    "//c8/pixels/opengl:gl-version",
    "//c8/pixels/render:renderer",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x04dfea6b);

#include <array>

#include "c8/c8-log.h"
#include "c8/pixels/opengl/gl-version.h"
#include "c8/pixels/render/renderer.h"
#include "reality/engine/features/embedded-shaders.h"
#include "reality/engine/features/gr8-feature-shader.h"

namespace c8 {

namespace {

static constexpr std::array<float, 6> createSubpixelParams() {
  // Discrete Gaussian params for sigma=0.2. This is really small, since our
  // current features are sparse.
  // TODO(mc): Investigate increasing this when we have non-sparse scores.
  constexpr double p0 = 0.975316;
  constexpr double p1 = 0.006133;
  constexpr double p2 = 0.000039;

  // See http://go.8thwall.com/gr8sp for details on these constants.
  constexpr double k0 = p0 * p1;
  constexpr double k1 = p0 * p2;
  constexpr double k2 = p1 * p2;
  constexpr double k3 = k1 + 2.0 * k2;
  constexpr double k4 = k0 + 2.0 * k1;
  constexpr double k5 = k0 + 4.0 * (k1 + k2);
  constexpr double k6 = 2.0 * p1 + 4.0 * p2;

  std::array<float, 6> q = {
    static_cast<float>(0.5 * k3 / k5),
    static_cast<float>(0.5 * k4 / k5),
    static_cast<float>(p1 / k6),
    static_cast<float>(p2 / k6),
    static_cast<float>(k2 / k5),
    static_cast<float>((k4 + 2.0 * k3 - 4.0 * k2) / k5),
  };

  return q;
}

static constexpr std::array<float, 6> subpixelParams = createSubpixelParams();
}  // namespace

void Gr8FeatureShader::initialize() {
  VERTEX_SHADER_CODE = embeddedGr8NoopVertCStr;
  ROTATE_SHADER_CODE = embeddedGr8RotateVertCStr;
  ROTATE_ONLY_SHADER_CODE = embeddedGr8RotateOnlyVertCStr;
  PYRAMID_BLUR_SHADER_CODE = embeddedGr8PyramidBlurFragCStr;
  PYRAMID_DECIMATE_SHADER_CODE = embeddedGr8PyramidDecimateFragCStr;
#if C8_USE_ALTERNATE_FEATURE_SCORE
  GRADIENT_SHADER_CODE = embeddedGr8GradientFragCStr;
  MOMENT_X_SHADER_CODE = embeddedGr8MomentXFragCStr;
  MOMENT_Y_SHADER_CODE = embeddedGr8MomentYFragCStr;
#endif
  INPUT_SHADER_CODE = embeddedGr8InputFragCStr;
  STAGE_1_SHADER_CODE = embeddedGr8Stage1FragCStr;
  STAGE_2_SHADER_CODE = embeddedGr8Stage2FragCStr;
  STAGE_3_SHADER_CODE = embeddedGr8Stage3FragCStr;

#ifdef __EMSCRIPTEN__
  if (isWebGL2()) {
    // This is a WebGL2 context. Use WebGL2 shaders.
    VERTEX_SHADER_CODE = embeddedGr8NoopWebgl2VertCStr;
    ROTATE_SHADER_CODE = embeddedGr8RotateWebgl2VertCStr;
    ROTATE_ONLY_SHADER_CODE = embeddedGr8RotateOnlyWebgl2VertCStr;
    PYRAMID_BLUR_SHADER_CODE = embeddedGr8PyramidBlurWebgl2FragCStr;
    PYRAMID_DECIMATE_SHADER_CODE = embeddedGr8PyramidDecimateWebgl2FragCStr;
#if C8_USE_ALTERNATE_FEATURE_SCORE
    GRADIENT_SHADER_CODE = embeddedGr8GradientWebgl2FragCStr;
    MOMENT_X_SHADER_CODE = embeddedGr8MomentXWebgl2FragCStr;
    MOMENT_Y_SHADER_CODE = embeddedGr8MomentYWebgl2FragCStr;
#endif
    INPUT_SHADER_CODE = embeddedGr8InputWebgl2FragCStr;
    STAGE_1_SHADER_CODE = embeddedGr8Stage1Webgl2FragCStr;
    STAGE_2_SHADER_CODE = embeddedGr8Stage2Webgl2FragCStr;
    STAGE_3_SHADER_CODE = embeddedGr8Stage3Webgl2FragCStr;
  }
#endif

  needsCleanup_ = true;
  shaderInput_.initialize(
    VERTEX_SHADER_CODE,
    INPUT_SHADER_CODE,
    {{"position", GlVertexAttrib::SLOT_0}, {"uv", GlVertexAttrib::SLOT_2}},
    {{"scale"}});
  shader1_.initialize(
    ROTATE_SHADER_CODE,
    STAGE_1_SHADER_CODE,
    {{"position", GlVertexAttrib::SLOT_0}, {"uv", GlVertexAttrib::SLOT_2}},
    {{"rotation"},
     {"flipY"},
     {"mvp"},
     {"clear"},
     {"scale"},
     {"pixScale"},
     {"targetDims"},
     {"roiDims"},
     {"searchDims"},
     {"isCurvy"},
     {"intrinsics"},
     {"globalPoseInverse"},
     {"scales"},
     {"shift"},
     {"radius"},
     {"radiusBottom"},
     {"curvyPostRotation"}});
  shaderPyramidBlur_.initialize(
    VERTEX_SHADER_CODE,
    PYRAMID_BLUR_SHADER_CODE,
    {{"position", GlVertexAttrib::SLOT_0}, {"uv", GlVertexAttrib::SLOT_2}},
    {{"roi"}, {"scale"}});
#if C8_USE_ALTERNATE_FEATURE_SCORE
  shaderGradient_.initialize(
    VERTEX_SHADER_CODE,
    GRADIENT_SHADER_CODE,
    {{"position", GlVertexAttrib::SLOT_0}, {"uv", GlVertexAttrib::SLOT_2}},
    {{"scale"}});
  shaderMomentX_.initialize(
    VERTEX_SHADER_CODE,
    MOMENT_X_SHADER_CODE,
    {{"position", GlVertexAttrib::SLOT_0}, {"uv", GlVertexAttrib::SLOT_2}},
    {{"scale"}});
  shaderMomentY_.initialize(
    VERTEX_SHADER_CODE,
    MOMENT_Y_SHADER_CODE,
    {{"position", GlVertexAttrib::SLOT_0}, {"uv", GlVertexAttrib::SLOT_2}},
    {{"scale"}});
#endif
  shaderPyramidDecimate_.initialize(
    ROTATE_ONLY_SHADER_CODE,
    PYRAMID_DECIMATE_SHADER_CODE,
    {{"position", GlVertexAttrib::SLOT_0}, {"uv", GlVertexAttrib::SLOT_2}},
    {{"rotation"}, {"roi"}, {"scale"}});
  shader2_.initialize(
    VERTEX_SHADER_CODE,
    STAGE_2_SHADER_CODE,
    {{"position", GlVertexAttrib::SLOT_0}, {"uv", GlVertexAttrib::SLOT_2}},
    {{"scale"}});
  shader3_.initialize(
    VERTEX_SHADER_CODE,
    STAGE_3_SHADER_CODE,
    {{"position", GlVertexAttrib::SLOT_0}, {"uv", GlVertexAttrib::SLOT_2}},
    {{"scale"}, {"nmsTolerance"}, {"skipSubpixel"}, {"subpixelQ"}, {"scoreThresh"}, {"gain"}});

  // Set the subpixel q params once during initialization.
  glUseProgram(shader3_.id());
  glUniform1fv(shader3_.location("subpixelQ"), subpixelParams.size(), subpixelParams.data());
}

void Gr8FeatureShader::setFeatureGain(float gain) { featureGain_ = gain; }
void Gr8FeatureShader::setFeatureParams(float, float scoreThresh, bool, float nmsTolerance) {
  scoreThresh_ = scoreThresh;
  nmsTolerance_ = nmsTolerance;
}

void Gr8FeatureShader::bindAndSetParams(const GlProgramObject *shader) {
  glUseProgram(shader->id());

  if (shader->location("nmsTolerance") >= 0) {
    glUniform1f(shader->location("nmsTolerance"), nmsTolerance_);
    glUniform1f(shader->location("scoreThresh"), scoreThresh_);
    glUniform1f(shader->location("gain"), featureGain_);
  }
}

}  // namespace c8
