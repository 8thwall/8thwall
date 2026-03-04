#pragma once

#include "c8/pixels/opengl/gl-framebuffer.h"
#include "c8/pixels/opengl/gl-program.h"
#include "c8/pixels/opengl/gl-quad.h"
#include "c8/pixels/opengl/gl-texture.h"
#include "c8/pixels/opengl/gl-vertex-array.h"

namespace c8 {

class GlTextureConversion {
public:
  void convertRGBToSingleChannelLum(GlTexture &src, GlFramebufferObject &dest, bool flipY = false);

private:
  struct ConversionParams {
    bool initialized = false;

    GlVertexArray vertexArray;
    GlProgramObject program;
    int flipYUniformLocation;
    bool flipY;
  };

  void fullViewportRenderPass(GlTexture &src, GlFramebufferObject &dest, ConversionParams &params);

  ConversionParams rgbToSingleChannelLumParams_;
};

};  // namespace c8
