// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Lynn Dang (lynn@8thwall.com)

#include "bzl/inliner/rules2.h"
cc_library {
  hdrs = {
    "gpu-pixels-resizer.h",
  };
  deps = {
    "//c8/pixels:pixels",
    "//c8/pixels:pixel-buffer",
    "//c8/pixels/render:renderer",
    "//c8/stats:scope-timer",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xb487af0b);

#include <array>

#include "c8/pixels/gpu-pixels-resizer.h"
#include "c8/pixels/pixel-buffer.h"
#include "c8/pixels/pixels.h"
#include "c8/pixels/render/renderer.h"
#include "c8/stats/scope-timer.h"

namespace c8 {
GpuPixelsResizer::GpuPixelsResizer() { renderer_ = std::make_unique<Renderer>(); }

RGBA8888PlanePixelBuffer GpuPixelsResizer::resizeOnGpu(
  ConstRGBA8888PlanePixels pix, int rows, int cols) {
  ScopeTimer t("gpu-pixel-resizer");
  {
    ScopeTimer t1("gpu-resize-input");
    // Resize input image.
    auto inputResizerScene = ObGen::scene(cols, rows);
    inputResizerScene->add(ObGen::pixelCamera(cols, rows));
    auto &inputQuad =
      inputResizerScene->add(ObGen::named(ObGen::pixelQuad(0, 0, cols, rows), "resize-texture"));
    inputQuad.material().setShader(Shaders::IMAGE).setColorTexture(TexGen::rgbaPixels(pix));
    GeoGen::flipTextureY(&inputQuad.geometry());

    renderer_->render(*inputResizerScene);
    // This is the input image but scaled.
    auto resizedInput = renderer_->result();
    return resizedInput;
  }
}

void GpuPixelsResizer::draw(GlTexture tex, int desiredWidth, int desiredHeight) {
  if (!shouldDraw_) {
    return;
  }

  int inputWidth = tex.width();
  int inputHeight = tex.height();
  float aspectRatio = float(inputHeight) / inputWidth;
  float desiredAspectRatio = float(desiredHeight) / desiredWidth;

  // Adjusted dimensions to fit the quad within the expected input image format.
  int adjustedHeight;
  int adjustedWidth;
  int adjustedX = 0;
  int adjustedY = 0;

  if (  // Same orientation.
    ((aspectRatio > 1) && (desiredAspectRatio >= 1))
    || ((aspectRatio < 1) && (desiredAspectRatio < 1))) {
    if (aspectRatio > desiredAspectRatio) {
      adjustedHeight = desiredHeight;
      adjustedWidth = adjustedHeight / aspectRatio;
      adjustedX = (desiredWidth - adjustedWidth) / 2;
    } else {
      adjustedWidth = desiredWidth;
      adjustedHeight = adjustedWidth * aspectRatio;
      adjustedY = (desiredHeight - adjustedHeight) / 2;
    }
  } else {  // Different orientation
    if (1 / aspectRatio > desiredAspectRatio) {
      adjustedHeight = desiredHeight;
      adjustedWidth = adjustedHeight * aspectRatio;
      adjustedX = (desiredWidth - adjustedWidth) / 2;
    } else {
      adjustedWidth = desiredWidth;
      adjustedHeight = adjustedWidth / aspectRatio;
      adjustedY = (desiredHeight - adjustedHeight) / 2;
    }
  }

  // If the scene isn't initialized, or if the input dimensions or desired dimensions are different.
  auto r = scene_ == nullptr ? RenderSpec{} : scene_->renderSpecs().front();
  if (
    (r.resolutionWidth != desiredWidth) || (r.resolutionHeight != desiredHeight)
    || (curInputHeight_ != inputHeight) || (curInputWidth_ != inputWidth)) {
    // Update current input dimensions.
    curInputHeight_ = inputHeight;
    curInputWidth_ = inputWidth;

    // Create scene with the desired width and height to transform the input image.
    scene_ = ObGen::scene(desiredWidth, desiredHeight);
    String name = scene_->renderSpecs().front().name;
    scene_->renderSpec(name).clearColor = Color::TRUE_BLACK;
    scene_->add(ObGen::pixelCamera(desiredWidth, desiredHeight));
    auto &inputQuad = scene_->add(ObGen::named(
      ObGen::pixelQuad(adjustedX, adjustedY, adjustedWidth, adjustedHeight), "resize-texture"));
    inputQuad.material().setShader(Shaders::IMAGE).setColorTexture(TexGen::nativeId(tex.id()));

    // Rotate the scene counter clockwise if orientation is different.
    if (
      ((aspectRatio > 1) && (desiredAspectRatio < 1))
      || ((aspectRatio < 1) && (desiredAspectRatio > 1))) {
      GeoGen::rotateCCW(&inputQuad.geometry());
    }
    GeoGen::flipTextureY(&inputQuad.geometry());
  }

  // Configure scene to draw on the new texture.
  scene_->find<Renderable>("resize-texture").material().colorTexture()->setNativeId(tex.id());

  // Create renderer if it doesn't exist.
  if (renderer_ == nullptr) {
    renderer_ = std::make_unique<Renderer>();
  }

  // Render the texture to the scene.
  renderer_->render(*scene_);

  // Mark that we do not need to draw.
  shouldDraw_ = false;

  // Mark that we are now ready to read.
  shouldRead_ = true;
}

void GpuPixelsResizer::read() {
  if (!shouldRead_) {
    return;
  }
  // Allocate buffers as necessary.
  if (!imageBuffer_) {
    auto [scratch, buf] = renderer_->allocateForResult();
    // This actually still requires 1 memcpy. Would be nice to be able to allocate directly into
    // member variable.
    imageBuffer_ = std::make_unique<RGBA8888PlanePixelBuffer>(std::move(buf));
    scratch_ = std::make_unique<RGBA8888PlanePixelBuffer>(std::move(scratch));
  }

  renderer_->result(scratch_->pixels(), imageBuffer_->pixels());

  // Mark that we don't need to read.
  shouldRead_ = false;
  // But we do have an image ready to be claimed.
  hasImage_ = true;
}

RGBA8888PlanePixels GpuPixelsResizer::claimImage() {
  hasImage_ = false;
  return imageBuffer_->pixels();
}
}  // namespace c8
