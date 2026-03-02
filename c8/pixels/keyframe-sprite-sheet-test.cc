#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    "//c8/io:image-io",
    "//c8/pixels/opengl:offscreen-gl-context",
    "//c8/pixels:draw2",
    "//c8/pixels:gl-pixels",
    "//c8/pixels:keyframe-sprite-sheet",
    "//c8:c8-log",
    "//c8/stats:scope-timer",
    ":pixel-transforms",
    "@com_google_googletest//:gtest_main",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xc6de5f86);

#include <gmock/gmock.h>

#include <iostream>
#include <string>  // std::string, std::stoi

#include "c8/c8-log.h"
#include "c8/io/image-io.h"
#include "c8/pixels/draw2.h"
#include "c8/pixels/gl-pixels.h"
#include "c8/pixels/keyframe-sprite-sheet.h"
#include "c8/pixels/opengl/offscreen-gl-context.h"
#include "c8/pixels/pixel-transforms.h"
#include "gtest/gtest.h"
#include "c8/stats/scope-timer.h"

namespace c8 {

bool WRITE_IMAGE = false;

class KeyframeSpriteSheetTest : public ::testing::Test {};

void debugWriteImage(ConstRGBA8888PlanePixels pixels, const String &path) {
  if (WRITE_IMAGE) {
    writeImage(pixels, path);
    C8Log("wrote image to  %s", path.c_str());
  }
}

bool isUniformColor(ConstFourChannelPixels src, Color color) {
  const uint8_t *srcb = src.pixels();
  if (
    (color.r() != srcb[0]) || (color.g() != srcb[1]) || (color.b() != srcb[2])
    || (color.a() != srcb[3]))
    return false;

  std::array<uint8_t, 4> prev{{srcb[0], srcb[1], srcb[2], srcb[3]}};
  for (int i = 0; i < src.rows(); ++i) {
    srcb = src.pixels() + i * src.rowBytes();
    for (int j = 0; j < src.cols(); ++j) {
      if (
        (prev[0] != srcb[0]) || (prev[1] != srcb[1]) || (prev[2] != srcb[2])
        || (prev[3] != srcb[3])) {
        return false;
      }
      srcb += 4;
    }
  }
  return true;
}

/*
| blue, mint  |
| red , green |
*/
TEST_F(KeyframeSpriteSheetTest, TextureCoordinate) {
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();
  ScopeTimer t("keyframe-sprite-sheet-test");
  Renderer renderer;

  int w = 100;
  int capacity = 4;
  Vector<Color> colors{Color::RED, Color::GREEN, Color::BLUE, Color::MINT};
  KeyframeSpriteSheet kfs{w, capacity};
  const Vector<HVector2> originalUvs = ObGen::backQuad()->geometry().uvs();
  Vector<SpriteInfo> spriteInfo(4);

  // create the kfs uber texture
  for (int i = 0; i < 4; i++) {
    RGBA8888PlanePixelBuffer buffer(w, w);
    RGBA8888PlanePixels pix = buffer.pixels();
    fill(colors[i], pix);
    auto tex = readImageToLinearTexture(pix);
    kfs.insert(tex.id(), std::to_string(i), &renderer);
    spriteInfo[i] = kfs.sprite(std::to_string(i));
  }
  debugWriteImage(renderer.result().pixels(), "/tmp/keyframesprite_textureCoordinate_uber.png");

  // one scene per sprite, in sprite sheet
  for (int i = 0; i < 4; i++) {
    auto scene = ObGen::scene(w, w);
    scene->add(ObGen::pixelCamera(w, w));
    auto &quad = scene->add(ObGen::backQuad()).setMaterial(MatGen::image());
    const SpriteInfo &info = spriteInfo[i];
    quad.geometry().setUvs(
      {{info.lowerLeftUv.x(), info.lowerLeftUv.y()},
       {info.lowerLeftUv.x(), info.upperRightUv.y()},
       {info.upperRightUv.x(), info.upperRightUv.y()},
       {info.upperRightUv.x(), info.lowerLeftUv.y()}});
    quad.material().colorTexture()->setNativeId(kfs.textureId());
    renderer.render(*scene);
    auto img = renderer.result();

    EXPECT_TRUE(isUniformColor(img.pixels(), colors[i]));
    debugWriteImage(img.pixels(), format("/tmp/keyframesprite_textureCoordinate_%i.png", i));
  }

  // test uvs
  Vector<SpriteInfo> correctUvs{
    {"", {0.0f, 0.0f}, {0.5f, 0.5f}},   // blue
    {"", {0.5f, 0.0f}, {1.0f, 0.5f}},   // mint
    {"", {0.0f, 0.5f}, {0.5f, 1.0f}},   // red
    {"", {0.5f, 0.5f}, {1.0f, 1.0f}}};  // green

  for (int i = 0; i < 4; i++) {
    EXPECT_EQ(spriteInfo[i].lowerLeftUv.x(), correctUvs[i].lowerLeftUv.x());
    EXPECT_EQ(spriteInfo[i].lowerLeftUv.y(), correctUvs[i].lowerLeftUv.y());
    EXPECT_EQ(spriteInfo[i].upperRightUv.x(), correctUvs[i].upperRightUv.x());
    EXPECT_EQ(spriteInfo[i].upperRightUv.y(), correctUvs[i].upperRightUv.y());
  }

}  // namespace c8

// update textures with the same tag name
TEST_F(KeyframeSpriteSheetTest, UpdateOldSprite) {
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();
  ScopeTimer t("keyframe-sprite-sheet-test");
  Renderer renderer;

  int w = 100;
  int capacity = 4;
  KeyframeSpriteSheet kfs{w, capacity};
  for (int i = 0; i < 4; i++) {
    RGBA8888PlanePixelBuffer buffer(w, w);
    RGBA8888PlanePixels pix = buffer.pixels();
    fill(Color::CHERRY, pix);
    auto tex = readImageToLinearTexture(pix);
    kfs.insert(tex.id(), std::to_string(i % 2), &renderer);
  }
  EXPECT_EQ(kfs.size(), 2);
}

TEST_F(KeyframeSpriteSheetTest, RemoveSprite) {
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();
  ScopeTimer t("keyframe-sprite-sheet-test");
  Renderer renderer;

  int w = 100;
  int capacity = 4;
  KeyframeSpriteSheet kfs{w, capacity};
  for (int i = 0; i < 4; i++) {
    RGBA8888PlanePixelBuffer buffer(w, w);
    RGBA8888PlanePixels pix = buffer.pixels();
    fill(Color::CHERRY, pix);
    auto tex = readImageToLinearTexture(pix);
    kfs.insert(tex.id(), std::to_string(i), &renderer);
  }
  EXPECT_EQ(kfs.size(), 4);

  // test insert after removal
  auto priorSpriteInfo = kfs.sprite(std::to_string(0));
  kfs.remove(std::to_string(0));
  EXPECT_EQ(kfs.size(), 3);

  kfs.insert(0, std::to_string(5), &renderer);
  EXPECT_EQ(kfs.size(), 4);
  // uv of new sprite should be same as old sprite that was removed
  auto spriteInfo = kfs.sprite(std::to_string(5));
  EXPECT_EQ(spriteInfo.lowerLeftUv.x(), priorSpriteInfo.lowerLeftUv.x());
  EXPECT_EQ(spriteInfo.lowerLeftUv.y(), priorSpriteInfo.lowerLeftUv.y());
  EXPECT_EQ(spriteInfo.upperRightUv.x(), priorSpriteInfo.upperRightUv.x());
  EXPECT_EQ(spriteInfo.upperRightUv.y(), priorSpriteInfo.upperRightUv.y());

  EXPECT_THROW(kfs.sprite(std::to_string(0)), std::invalid_argument);

  // big number to test no throw on remove tag that does not exist
  for (int i = 0; i <= 7; i++) {
    kfs.remove(std::to_string(i));
  }
  EXPECT_EQ(kfs.size(), 0);
}

// create a grid of 16 textures
TEST_F(KeyframeSpriteSheetTest, FillGrid) {
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();
  ScopeTimer t("keyframe-sprite-sheet-test");
  Renderer renderer;

  int w = 100;
  int capacity = 16;
  KeyframeSpriteSheet kfs{w, capacity};

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      RGBA8888PlanePixelBuffer buffer(w, w);
      RGBA8888PlanePixels pix = buffer.pixels();
      fill(
        Color::CHERRY.r(),
        Color::CHERRY.g(),
        Color::CHERRY.b(),
        60 * i + 7 * j,
        &pix);
      auto tex = readImageToLinearTexture(pix);
      kfs.insert(tex.id(), std::to_string(4 * i + j), &renderer);
    }
  }
  auto img = renderer.result();
  EXPECT_EQ(img.pixels().cols(), img.pixels().rows());
  EXPECT_GE((img.pixels().cols() * img.pixels().rows()), w * w * capacity);
  debugWriteImage(img.pixels(), "/tmp/keyframesprite_gridFill.png");
}
}  // namespace c8
