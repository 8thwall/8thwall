// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Common transformation of GPU textures.

#pragma once

#include <functional>

#include "c8/geometry/parameterized-geometry.h"
#include "c8/hmatrix.h"
#include "c8/pixels/opengl/gl-framebuffer.h"
#include "c8/pixels/opengl/gl-pixel-buffer.h"
#include "c8/pixels/opengl/gl-texture.h"
#include "c8/pixels/opengl/gl-version.h"
#include "c8/pixels/pixel-buffer.h"
#include "c8/pixels/pixels.h"

namespace c8 {

// Read a framebuffer into a existing RGBA8888PlanePixels destination.
void readFramebufferRGBA8888Pixels(const GlFramebufferObject &fb, RGBA8888PlanePixels dest);

// Allocate a new image buffer and source its contents from the framebuffer.
RGBA8888PlanePixelBuffer readFramebufferRGBA8888PixelBuffer(const GlFramebufferObject &fb);

#if C8_OPENGL_VERSION_3
// Read framebuffer into a GlPixelBuffer. It is the caller's responsibility to ensure the dest
// buffer is correctly initialized and large enough to store the result.
void readFramebufferRGBAToPixelBuffer(
  const GlFramebufferObject &fb, GlRGBA8888PlanePixelBuffer *dest);
void readFramebufferYUVAToPixelBuffer(
  const GlFramebufferObject &fb, GlYUVA8888PlanePixelBuffer *dest);
#endif

// Creates a method for copying 2D GPU textures. If the src and dest.tex() textures have different
// sizes, the src will be stretched to fit the destination. Does not support
// GL_EXTERNAL_TEXTURE_OES.
std::function<void(GlTexture src, GlFramebufferObject *dest)> compileCopyTexture2D();

// Creates a method for warping curvy 2D GPU textures. Does not support
// GL_EXTERNAL_TEXTURE_OES.
std::function<void(
  CurvyImageGeometry geom,
  const HMatrix &intrinsics,
  const HMatrix &globalPose,
  int rotation,
  float roiAspectRatio,
  HPoint2 searchDims,
  GlTexture src,
  GlFramebufferObject *dest)>
compileWarpCurvyTexture2D();

// Creates a method for warping 2D GPU textures. Does not support GL_EXTERNAL_TEXTURE_OES.
std::function<void(const HMatrix &mvp, GlTexture src, GlFramebufferObject *dest)>
compileWarpTexture2D();

// Creates a method for warping 2D GPU textures. Does not support GL_EXTERNAL_TEXTURE_OES.
std::function<void(const HMatrix &mvp, int rotation, GlTexture src, GlFramebufferObject *dest)>
compileWarpRotateTexture2D();

// Creates a method for displaying image channels. Does not support GL_EXTERNAL_TEXTURE_OES.
std::function<void(int channel, GlTexture src, GlFramebufferObject *dest)> compileChannelHeatmap();

// Creates a method for rotating 2D GPU textures by 90 degrees. If the src and dest.tex() textures
// have different sizes (adjusting for rotation), the src will be stretched to fit the destination.
// Does not support GL_EXTERNAL_TEXTURE_OES.
std::function<void(GlTexture src, GlFramebufferObject *dest)> compileRotate90Texture2D();

// Creates a method for rotating 2D GPU textures by 270 degrees. If the src and dest.tex() textures
// have different sizes (adjusting for rotation), the src will be stretched to fit the destination.
// Does not support GL_EXTERNAL_TEXTURE_OES.
std::function<void(GlTexture src, GlFramebufferObject *dest)> compileRotate270Texture2D();

// Creates a method for copying a GL_EXTERNAL_TEXTURE_OES into a framebuffer with sampling
// transform. If the src and dest.tex() textures have different sizes, the src will be stretched to
// fit the destination.
//
// mtx - is a 4x4 matrix in *column-major* order, to be applied to the texture coordinates during
//       sampling.
std::function<void(const float mtx[16], GlTexture src, GlFramebufferObject *dest)>
compileCopyExternalOesTexture2D();

// Creates a method for converting a RGB(A) texture to YUV(A).
std::function<void(GlTexture src, GlFramebufferObject *dest)> compileConvertTextureRGBToYUV();

// Creates a method for converting a RGB(A) texture to YUV(A), with a Y-flip.
std::function<void(GlTexture src, GlFramebufferObject *dest)> compileConvertTextureRGBToYUVFlipY();

}  // namespace c8
