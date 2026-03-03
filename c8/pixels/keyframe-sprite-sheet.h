#pragma once
#include "c8/hvector.h"
#include "c8/map.h"
#include "c8/pixels/render/object8.h"
#include "c8/pixels/render/renderer.h"
#include "c8/set.h"
#include "c8/string.h"

namespace c8 {

struct SpriteInfo {
  String tag;  // A user-supplied tag
  HVector2 lowerLeftUv;
  HVector2 upperRightUv;

  void setQuadUvs(Renderable &quad) const {
    quad.geometry().setUvs(
      {{lowerLeftUv.x(), lowerLeftUv.y()},     // bl(1)
       {lowerLeftUv.x(), upperRightUv.y()},    // tl(2)
       {upperRightUv.x(), upperRightUv.y()},   // tr(3)
       {upperRightUv.x(), lowerLeftUv.y()}});  // br(4)
  }

  void setQuadUvsFlippedY(Renderable &quad) const {
    quad.geometry().setUvs(
      {{lowerLeftUv.x(), upperRightUv.y()},    // tl(2)
       {lowerLeftUv.x(), lowerLeftUv.y()},     // bl(1)
       {upperRightUv.x(), lowerLeftUv.y()},    // br(4)
       {upperRightUv.x(), upperRightUv.y()}}); // tr(3)
  }

  String toString() const {
    return format(
      "tag: %s; lowerLeftUv: %s; upperRightUv: %s",
      tag.c_str(),
      lowerLeftUv.toString().c_str(),
      upperRightUv.toString().c_str());
  }
};

// Store a sheet of low-res keyframe textures. The sprite ordering is
// |(0,h)...,(w,h)|
// |..., ..., ... |
// |(0,0),...,(w,0)|
class KeyframeSpriteSheet {
public:
  // Size of sprites, in pixels, e.g. 128 -> 128x128. The allocated capacity is the ceiling
  // of the next square number to capacity
  KeyframeSpriteSheet(int spritePixels, int capacity);

  // Add a keyframe sprite. Will throw if at capacity
  void insert(int32_t sourceTextureId, const String &tag, Renderer *renderer);

  // Remove a keyframe sprite, adds tag to a set to be used to overwrite old sprites
  void remove(const String &tag);

  // Get the textureId for this SpriteSheet.
  int textureId() const;

  // Return the SpriteSheet size
  int size() const;

  // Return true if tag in SpriteSheet
  bool has(const String &tag) const;

  // Get sprites in the current sheet.
  SpriteInfo sprite(const String &tag) const;

  // Get Vector of sprites in the current sheet.
  Vector<SpriteInfo> sprites() const;

private:
  int width_;
  int spriteWidth_;
  std::unique_ptr<Scene> scene_;
  TreeMap<String, int> tagToLayoutIndex_;
  TreeSet<int> deletedIndexes_;

  // Return the Viewport given the index of the sprite
  Viewport idxToViewport(int i) const;
};

}  // namespace c8
