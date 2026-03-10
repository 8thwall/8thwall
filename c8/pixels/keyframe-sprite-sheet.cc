#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "keyframe-sprite-sheet.h",
  };
  deps = {
    "//c8/pixels/render:object8",
    "//c8/pixels/render:renderer",
    "//c8:hvector",
    "//c8:map",
    "//c8:set",
    "//c8:string",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xba4de755);

#include <cmath>

#include "c8/pixels/keyframe-sprite-sheet.h"
#include "c8/pixels/render/renderer.h"

namespace c8 {

KeyframeSpriteSheet::KeyframeSpriteSheet(int spritePixels, int capacity)
    : width_(spritePixels * std::ceil(std::sqrt(capacity))), spriteWidth_(spritePixels) {
  scene_ = std::unique_ptr<Scene>(new Scene());
  scene_->add(ObGen::pixelCamera(width_, width_));
  auto &quad = scene_->add(ObGen::backQuad()).setMaterial(MatGen::image());
  GeoGen::flipTextureY(&quad.geometry());
}

Viewport KeyframeSpriteSheet::idxToViewport(int idx) const {
  int spritesPerRow = width_ / spriteWidth_;
  int y = spriteWidth_ * floor(idx / spritesPerRow);
  int x = spriteWidth_ * (idx % spritesPerRow);
  return {x, y, spriteWidth_, spriteWidth_};
}

void KeyframeSpriteSheet::insert(int32_t sourceTextureId, const String &tag, Renderer *renderer) {
  auto hasTag = tagToLayoutIndex_.count(tag);
  int capacity = std::pow((width_ / spriteWidth_), 2);
  if (tagToLayoutIndex_.size() - deletedIndexes_.size() == capacity && !hasTag) {
    C8_THROW_INVALID_ARGUMENT(
      "[KeyframeSpriteSheet] Can not insert into SpriteSheet, already at max capacity");
  }

  int idx;
  if (!hasTag) {
    if (!deletedIndexes_.size()) {
      idx = tagToLayoutIndex_.size();
    } else {
      auto it = deletedIndexes_.begin();
      idx = *it;
      deletedIndexes_.erase(it);
    }
    tagToLayoutIndex_[tag] = idx;
  } else {
    idx = tagToLayoutIndex_.at(tag);
  }

  scene_->find<Renderable>("back-quad").material().colorTexture()->setNativeId(sourceTextureId);
  scene_->setRenderSpecs({{width_, width_, "", "", idxToViewport(idx), BufferClear::NONE}});
  renderer->render(*scene_);
}

void KeyframeSpriteSheet::remove(const String &tag) {
  if (tagToLayoutIndex_.count(tag)) {
    deletedIndexes_.insert(tagToLayoutIndex_[tag]);
    tagToLayoutIndex_.erase(tag);
  }
}

int KeyframeSpriteSheet::textureId() const { return scene_->renderer()->nativeTextureId(""); }

int KeyframeSpriteSheet::size() const { return tagToLayoutIndex_.size(); }

bool KeyframeSpriteSheet::has(const String &tag) const { return tagToLayoutIndex_.count(tag) != 0; }

SpriteInfo KeyframeSpriteSheet::sprite(const String &tag) const {
  if (!tagToLayoutIndex_.count(tag)) {
    C8_THROW_INVALID_ARGUMENT(
      "[KeyframeSpriteSheet] Tag \"%s\" not in spriteSheet", tag.c_str());
  }
  Viewport vp = idxToViewport(tagToLayoutIndex_.at(tag));
  float xpos = (float)vp.x / width_;
  float ypos = (float)vp.y / width_;
  float delta = (float)spriteWidth_ / width_;
  return {tag, {xpos, ypos}, {xpos + delta, ypos + delta}};
}

Vector<SpriteInfo> KeyframeSpriteSheet::sprites() const {
  Vector<SpriteInfo> sprites(tagToLayoutIndex_.size());
  int i = 0;
  for (auto const &pair : tagToLayoutIndex_) {
    sprites[i++] = sprite(pair.first);
  }
  return sprites;
}

}  // namespace c8
