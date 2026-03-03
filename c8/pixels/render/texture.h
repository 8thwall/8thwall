// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include <memory>

#include "c8/pixels/pixel-buffer.h"
#include "c8/pixels/pixels.h"
#include "c8/string.h"

namespace c8 {

enum class TextureTarget {
  TEXTURE_2D,
  TEXTURE_CUBE_MAP,
};

// Abstract base class for binding this texture. The implementation of this is specific to a
// gpu framework (e.g. vulkan vs. metal vs. opengl) and this will be set by the renderer.
class TextureBinder {
public:
  virtual void bind(int slot) = 0;
  virtual ~TextureBinder() = default;
};

// A SceneMaterialSpec is most often used to render sub-scenes onto quads, so as to render multiple
// subscenes given a single Scene hierarchy.  It will get the off-screen framebuffer's content
// for a given sub-scene / renderSpec and use that as a texture.
struct SceneMaterialSpec {
  // name of the subscene that contains the renderSpec
  String subsceneName;
  // the name of the render spec
  String renderSpecName;
};

// A Texture has image data that is used in rendering. A texture's data can come from one of several
// sources:
// * Native ID: This is a direct pointer to an existing texture in the rendering engine, from an
//   external source like a camera feed.
// * RGBA Pixels: Set the data for this texture to the pixels of a provided image.
class Texture {
public:
  // Set the texture source to a known id of a texture native to the rendering engine. This usually
  // comes from some external source, such as a camera feed.
  Texture &setNativeId(int32_t nativeId);
  int32_t nativeId() const { return nativeId_; };
  bool hasNativeId() const { return nativeId_ >= 0; }

  // Set the texture native target so that the binder can bind the correct texture target type.
  Texture &setNativeTarget(TextureTarget nativeTarget);
  TextureTarget nativeTarget() const { return nativeTarget_; }

  // Set the texture source to a provided image that is managed externally. ConstRGBA8888PlanePixels
  // points to the pixels managed by a PixelBuffer.  If you want the Texture to manage the lifecycle
  // of an image, then use setRgbaPixelBuffer.
  Texture &setRgbaPixels(ConstRGBA8888PlanePixels rgbaPixels);
  Texture &setR32Pixels(ConstR32PlanePixels rPixels);
  Texture &setRgba32Pixels(ConstRGBA32PlanePixels rgbaPixels);
  Texture &setDepthPixels(ConstDepthFloatPixels depthPixels);

  ConstRGBA8888PlanePixels rgbaPixels() const {
    if (rgbaPixels_.pixels() != nullptr) {
      return rgbaPixels_;
    }

    return rgbaPixelBuffer_.pixels();
  }

  ConstR32PlanePixels r32Pixels() const {
    if (r32Pixels_.pixels() != nullptr) {
      return r32Pixels_;
    }
    return r32PixelBuffer_.pixels();
  }

  ConstRGBA32PlanePixels rgba32Pixels() const {
    if (rgba32Pixels_.pixels() != nullptr) {
      return rgba32Pixels_;
    }
    return rgba32PixelBuffer_.pixels();
  }

  ConstDepthFloatPixels depthPixels() const {
    if (depthPixels_.pixels() != nullptr) {
      return depthPixels_;
    }
    return depthPixelBuffer_.pixels();
  }

  R32PlanePixels mutableR32Pixels() {
    r32PixelsDirty_ = true;
    return r32PixelBuffer_.pixels();
  }

  // Moves the ownership of an image into the Texture.
  Texture &setRgbaPixelBuffer(RGBA8888PlanePixelBuffer &&rgbaPixelBuffer);
  Texture &setR32PixelBuffer(R32PlanePixelBuffer &&rPixelBuffer);
  Texture &setRgba32PixelBuffer(RGBA32PlanePixelBuffer &&rgbaPixelBuffer);
  Texture &setDepthPixelBuffer(DepthFloatPixelBuffer &&depthPixelBuffer);

  bool hasRgbaPixels() const { return rgbaPixels().pixels() != nullptr; }
  bool hasR32Pixels() const { return r32Pixels().pixels() != nullptr; }
  bool hasRgba32Pixels() const { return rgba32Pixels().pixels() != nullptr; }
  bool hasDepthPixels() const { return depthPixels().pixels() != nullptr; }

  bool rgbaPixelsDirty() const { return rgbaPixelsDirty_; }
  bool r32PixelsDirty() const { return r32PixelsDirty_; }
  bool rgba32PixelsDirty() const { return rgba32PixelsDirty_; }
  bool depthPixelsDirty() const { return depthPixelsDirty_; }

  // Mark all fields as clean.
  Texture &setClean();

  TextureBinder *binder() { return binder_.get(); }
  Texture &setBinder(std::unique_ptr<TextureBinder> &&binder);

  Texture &setSceneTexture(const String &subsceneName, const String &renderSpecName);
  const SceneMaterialSpec *sceneMaterialSpec() const;

  String toString() const noexcept;

private:
  void clear();

  std::unique_ptr<SceneMaterialSpec> sceneMaterialSpec_;
  int32_t nativeId_ = -1;
  TextureTarget nativeTarget_ = TextureTarget::TEXTURE_2D;
  // Points to pixels managed by an outside PixelBuffer.
  ConstRGBA8888PlanePixels rgbaPixels_;
  ConstR32PlanePixels r32Pixels_;
  ConstRGBA32PlanePixels rgba32Pixels_;
  ConstDepthFloatPixels depthPixels_;
  // Stores the pixels directly on this Texture.
  RGBA8888PlanePixelBuffer rgbaPixelBuffer_;
  R32PlanePixelBuffer r32PixelBuffer_;
  RGBA32PlanePixelBuffer rgba32PixelBuffer_;
  DepthFloatPixelBuffer depthPixelBuffer_;
  bool rgbaPixelsDirty_ = false;
  bool rgba32PixelsDirty_ = false;
  bool r32PixelsDirty_ = false;
  bool depthPixelsDirty_ = false;

  std::unique_ptr<TextureBinder> binder_;
};

// Factory methods for creating Texture instances.
namespace TexGen {

// Basic factory methods for creating a texture with default data.
std::unique_ptr<Texture> empty();
std::unique_ptr<Texture> nativeId(int32_t id);
std::unique_ptr<Texture> rgbaPixels(ConstRGBA8888PlanePixels rgbaPixels);
std::unique_ptr<Texture> r32Pixels(ConstR32PlanePixels rPixels);
std::unique_ptr<Texture> rgba32Pixels(ConstRGBA32PlanePixels rgbaPixels);
std::unique_ptr<Texture> depthPixels(ConstDepthFloatPixels depthPixels);
std::unique_ptr<Texture> rgbaPixelBuffer(RGBA8888PlanePixelBuffer &&rgbaPixelBuffer);
std::unique_ptr<Texture> r32PixelBuffer(R32PlanePixelBuffer &&rgbaPixelBuffer);
std::unique_ptr<Texture> rgba32PixelBuffer(RGBA32PlanePixelBuffer &&rgbaPixelBuffer);
std::unique_ptr<Texture> rgbaPixelBuffer(ConstRGBA8888PlanePixels rgbaPixels);
std::unique_ptr<Texture> depthPixelBuffer(DepthFloatPixelBuffer &&depthPixelBuffer);
std::unique_ptr<Texture> sceneTexture(const String &subsceneName, const String &renderSpecName);

}  // namespace TexGen

}  // namespace c8
