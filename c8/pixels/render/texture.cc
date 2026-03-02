// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"texture.h"};
  deps = {
    "//c8/pixels:pixels",
    "//c8/pixels:pixel-buffer",
    "//c8/pixels:pixel-transforms",
    "//c8:string",
    "//c8/string:format",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x7472a066);

#include "c8/pixels/pixel-transforms.h"
#include "c8/pixels/render/texture.h"
#include "c8/string/format.h"

namespace c8 {

Texture &Texture::setNativeId(int32_t nativeId) {
  clear();
  nativeId_ = nativeId;
  return *this;
}

Texture &Texture::setNativeTarget(TextureTarget nativeTarget) {
  // Used in conjunction with setNativeId, calling `clear()` again would be an error.
  nativeTarget_ = nativeTarget;
  return *this;
}

Texture &Texture::setRgbaPixels(ConstRGBA8888PlanePixels rgbaPixels) {
  clear();
  rgbaPixels_ = rgbaPixels;
  rgbaPixelsDirty_ = true;
  return *this;
}

Texture &Texture::setR32Pixels(ConstR32PlanePixels rPixels) {
  clear();
  r32Pixels_ = rPixels;
  r32PixelsDirty_ = true;
  return *this;
}

Texture &Texture::setRgba32Pixels(ConstRGBA32PlanePixels rgbaPixels) {
  clear();
  rgba32Pixels_ = rgbaPixels;
  rgba32PixelsDirty_ = true;
  return *this;
}

Texture &Texture::setDepthPixels(ConstDepthFloatPixels depthPixels) {
  clear();
  depthPixels_ = depthPixels;
  depthPixelsDirty_ = true;
  return *this;
}

Texture &Texture::setRgbaPixelBuffer(RGBA8888PlanePixelBuffer &&rgbaPixelBuffer) {
  clear();
  rgbaPixelBuffer_ = std::move(rgbaPixelBuffer);
  rgbaPixelsDirty_ = true;
  return *this;
}

Texture &Texture::setR32PixelBuffer(R32PlanePixelBuffer &&rPixelBuffer) {
  clear();
  r32PixelBuffer_ = std::move(rPixelBuffer);
  r32PixelsDirty_ = true;
  return *this;
}

Texture &Texture::setRgba32PixelBuffer(RGBA32PlanePixelBuffer &&rgbaPixelBuffer) {
  clear();
  rgba32PixelBuffer_ = std::move(rgbaPixelBuffer);
  rgba32PixelsDirty_ = true;
  return *this;
}

Texture &Texture::setDepthPixelBuffer(DepthFloatPixelBuffer &&depthPixelBuffer) {
  clear();
  depthPixelBuffer_ = std::move(depthPixelBuffer);
  depthPixelsDirty_ = true;
  return *this;
}

// Mark all fields as clean.
Texture &Texture::setClean() {
  rgbaPixelsDirty_ = false;
  r32PixelsDirty_ = false;
  rgba32PixelsDirty_ = false;
  depthPixelsDirty_ = false;
  return *this;
}

Texture &Texture::setBinder(std::unique_ptr<TextureBinder> &&binder) {
  binder_ = std::move(binder);
  return *this;
}

Texture &Texture::setSceneTexture(const String &subsceneName, const String &renderSpecName) {
  sceneMaterialSpec_.reset(new SceneMaterialSpec{subsceneName, renderSpecName});
  return *this;
}

String Texture::toString() const noexcept {
  if (sceneMaterialSpec_) {
    return format("subscene[%s]", sceneMaterialSpec_->subsceneName.c_str());
  } else {
    return format(
      "native %d pix %d,%d r32 %d,%d pix32 %d,%d depth %d,%d buf %d,%d rbuf32 %d,%d buf32 %d,%d bufdepth %d,%d",
      nativeId_,
      rgbaPixels_.cols(),
      rgbaPixels_.rows(),
      r32Pixels_.cols(),
      r32Pixels_.rows(),
      rgba32Pixels_.cols(),
      rgba32Pixels_.rows(),
      depthPixels_.cols(),
      depthPixels_.rows(),
      r32PixelBuffer_.pixels().cols(),
      r32PixelBuffer_.pixels().rows(),
      rgbaPixelBuffer_.pixels().cols(),
      rgbaPixelBuffer_.pixels().rows(),
      rgba32PixelBuffer_.pixels().cols(),
      rgba32PixelBuffer_.pixels().rows(),
      depthPixelBuffer_.pixels().cols(),
      depthPixelBuffer_.pixels().rows());
  }
}

void Texture::clear() {
  rgbaPixelBuffer_ = {};
  r32PixelBuffer_ = {};
  rgba32PixelBuffer_ = {};
  depthPixelBuffer_ = {};
  rgbaPixels_ = ConstRGBA8888PlanePixels{};
  r32Pixels_ = ConstR32PlanePixels{};
  rgba32Pixels_ = ConstRGBA32PlanePixels{};
  depthPixels_ = ConstDepthFloatPixels{};
  nativeId_ = -1;
}

const SceneMaterialSpec *Texture::sceneMaterialSpec() const { return sceneMaterialSpec_.get(); }

namespace TexGen {

std::unique_ptr<Texture> empty() { return std::make_unique<Texture>(); }

std::unique_ptr<Texture> nativeId(int32_t id) {
  auto t = TexGen::empty();
  t->setNativeId(id);
  return t;
}

std::unique_ptr<Texture> rgbaPixels(ConstRGBA8888PlanePixels rgbaPixels) {
  auto t = TexGen::empty();
  t->setRgbaPixels(rgbaPixels);
  return t;
}

std::unique_ptr<Texture> rgbaPixelBuffer(RGBA8888PlanePixelBuffer &&rgbaPixelBuffer) {
  auto t = TexGen::empty();
  t->setRgbaPixelBuffer(std::move(rgbaPixelBuffer));
  return t;
}

std::unique_ptr<Texture> r32Pixels(ConstR32PlanePixels rPixels) {
  auto t = TexGen::empty();
  t->setR32Pixels(rPixels);
  return t;
}

std::unique_ptr<Texture> rgba32Pixels(ConstRGBA32PlanePixels rgbaPixels) {
  auto t = TexGen::empty();
  t->setRgba32Pixels(rgbaPixels);
  return t;
}

std::unique_ptr<Texture> depthPixels(ConstDepthFloatPixels depthPixels) {
  auto t = TexGen::empty();
  t->setDepthPixels(depthPixels);
  return t;
}

std::unique_ptr<Texture> r32PixelBuffer(R32PlanePixelBuffer &&rPixelBuffer) {
  auto t = TexGen::empty();
  t->setR32PixelBuffer(std::move(rPixelBuffer));
  return t;
}

std::unique_ptr<Texture> rgba32PixelBuffer(RGBA32PlanePixelBuffer &&rgbaPixelBuffer) {
  auto t = TexGen::empty();
  t->setRgba32PixelBuffer(std::move(rgbaPixelBuffer));
  return t;
}

std::unique_ptr<Texture> rgbaPixelBuffer(ConstRGBA8888PlanePixels rgbaPixels) {
  auto t = TexGen::empty();
  RGBA8888PlanePixelBuffer rgbaPixelBuffer{rgbaPixels.rows(), rgbaPixels.cols()};
  auto bufferPix = rgbaPixelBuffer.pixels();
  copyPixels(rgbaPixels, &bufferPix);
  t->setRgbaPixelBuffer(std::move(rgbaPixelBuffer));
  return t;
}

std::unique_ptr<Texture> depthPixelBuffer(DepthFloatPixelBuffer &&depthPixelBuffer) {
  auto t = TexGen::empty();
  t->setDepthPixelBuffer(std::move(depthPixelBuffer));
  return t;
}


std::unique_ptr<Texture> sceneTexture(const String &subsceneName, const String &renderSpecName) {
  auto t = TexGen::empty();
  t->setSceneTexture(subsceneName, renderSpecName);
  return t;
}


}  // namespace TexGen

}  // namespace c8
