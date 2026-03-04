#include "c8/xrapi/openxr/gl-texture-conversion.h"

#include "c8/c8-log.h"
#include "c8/pixels/opengl/gl-error.h"
#include "c8/pixels/opengl/gl-extensions.h"
#include "c8/pixels/opengl/gl-version.h"
#include "c8/pixels/opengl/gl.h"

namespace c8 {

// Based on 'c8/pixels/opengl/texture-transforms.cc'
// In the future, texture-transforms could have a general method to accept shaders.

namespace {

static constexpr char const NOP_UV_VERTEX_CODE[] = C8_GLSL_VERSION_LINE
  R"(
in vec3 position;
in vec2 uv;
out vec2 texUv;
void main() {
  gl_Position = vec4(position, 1.0);
  texUv = uv;
}
)";

static constexpr char const RGB_TO_SINGLE_CHANNEL_LUM_FRAGMENT_CODE_EXT_YUV[] = C8_GLSL_VERSION_LINE
  R"(
#extension GL_EXT_YUV_target : enable
precision mediump float;
in vec2 texUv;
out float fragmentColor;

uniform sampler2D sampler;
uniform bool flipY;

void main() {
  vec2 uv = flipY ? vec2(texUv.x, 1.0 - texUv.y) : texUv;
  vec3 color = texture(sampler, uv).rgb;
  fragmentColor = rgb_2_yuv(color, itu_601_full_range).r;
}
)";

static constexpr char const RGB_TO_SINGLE_CHANNEL_LUM_FRAGMENT_CODE[] = C8_GLSL_VERSION_LINE
  R"(
precision mediump float;
in vec2 texUv;
out float fragmentColor;

uniform sampler2D sampler;
uniform bool flipY;

void main() {
  vec2 uv = flipY ? vec2(texUv.x, 1.0 - texUv.y) : texUv;
  vec4 color = texture(sampler, uv);
  float y = .299 * color.r + .587 * color.g + .114 * color.b;
  fragmentColor = y;
}
)";

}  // namespace

void GlTextureConversion::convertRGBToSingleChannelLum(
  GlTexture &src, GlFramebufferObject &dest, bool flipY) {
  if (!rgbToSingleChannelLumParams_.initialized) {
    GlProgramObject program;

    // Recommended by QCOM_motion_estimation extension.
    bool hasYUVEXT = hasGlExtension("GL_EXT_YUV_target");
    program.initialize(
      NOP_UV_VERTEX_CODE,
      (hasYUVEXT) ? RGB_TO_SINGLE_CHANNEL_LUM_FRAGMENT_CODE_EXT_YUV
                  : RGB_TO_SINGLE_CHANNEL_LUM_FRAGMENT_CODE,
      {{"position", GlVertexAttrib::SLOT_0}, {"uv", GlVertexAttrib::SLOT_2}},
      {{"flipY"}});

    int flipYUniformLocation = program.location("flipY");
    GlVertexArray rect = makeVertexArrayRect();

    rgbToSingleChannelLumParams_ = {
      true, std::move(rect), std::move(program), flipYUniformLocation, flipY};
  }

  fullViewportRenderPass(src, dest, rgbToSingleChannelLumParams_);
}

void GlTextureConversion::fullViewportRenderPass(
  GlTexture &src, GlFramebufferObject &dest, ConversionParams &params) {
  // Bind the destination framebuffer.
  dest.bind();
  // Copy using the render pipeline.
  glUseProgram(params.program.id());
  glUniform1i(params.flipYUniformLocation, params.flipY);
  params.vertexArray.bind();

  glActiveTexture(GL_TEXTURE0);
  src.bind();
  glUniform1i(params.program.location("sampler"), 0);

  glFrontFace(GL_CCW);
  glViewport(0, 0, dest.tex().width(), dest.tex().height());
  params.vertexArray.drawElements();
  src.unbind();
  params.vertexArray.unbind();
  dest.unbind();
}

}  // namespace c8