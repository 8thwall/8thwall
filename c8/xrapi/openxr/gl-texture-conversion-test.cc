#include "c8/xrapi/openxr/gl-texture-conversion.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "c8/c8-log.h"
#include "c8/pixels/opengl/offscreen-gl-context.h"
#include "c8/vector.h"

namespace c8 {

bool WRITE_IMAGE = false;

class GlTextureConversionTest : public ::testing::Test {};

TEST_F(GlTextureConversionTest, TestRedLuminanceConversion) {
  // Create an offscreen context.
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  GlTextureConversion textureConversion;

  // Create the source texture.
  const int width = 128;
  const int height = 64;

  Vector<uint8_t> pixelsData(width * height * 4);

  // Fill the pixel data with red
  const int numPixels = pixelsData.size() / 4;
  for (int i = 0; i < numPixels; ++i) {
    pixelsData[i * 4 + 0] = 255;  // Red component
    pixelsData[i * 4 + 1] = 0;    // Green component
    pixelsData[i * 4 + 2] = 0;    // Blue component
    pixelsData[i * 4 + 3] = 255;  // Alpha component
  }

  GlTexture2D srcTex = makeNearestRGBA8888Texture2D(width, height);
  srcTex.bind();
  srcTex.updateImage(pixelsData.data());
  srcTex.unbind();

  // Create destination texture
  GlTexture2D destTex;
  destTex.initialize(GL_TEXTURE_2D, GL_R8, width, height, GL_RED, GL_UNSIGNED_BYTE, nullptr);

  // Allocate the destination framebuffer.
  GlFramebufferObject destFb;
  destFb.initialize(std::move(destTex), GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0);

  // Render the source texture to the destination framebuffer.
  GlTexture srcTexHandle = srcTex.tex();
  textureConversion.convertRGBToSingleChannelLum(srcTexHandle, destFb);

  // Get output
  Vector<uint8_t> destData(width * height);  // Single channel luminance

  destFb.bind();
  glReadPixels(0, 0, width, height, GL_RED, GL_UNSIGNED_BYTE, destData.data());
  destFb.unbind();

  // Check the output
  const int red2Y = 76;  // 0.299 * 255
  for (int i = 0; i < destData.size(); ++i) {
    EXPECT_EQ(red2Y, destData[i]);
  }

  EXPECT_EQ(GL_NO_ERROR, glGetError());
}

TEST_F(GlTextureConversionTest, TestFlipYLuminanceConversion) {
  // Create an offscreen context.
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  GlTextureConversion textureConversion;

  // Create the source texture.
  const int width = 128;
  const int height = 64;

  Vector<uint8_t> pixelsData(width * height * 4);

  // Fill the pixel data with red
  const int numPixels = pixelsData.size() / 4;

  // Top half is red, bottom half is green
  for (int i = 0; i < numPixels / 2; ++i) {
    pixelsData[i * 4 + 0] = 255;
    pixelsData[i * 4 + 1] = 0;
    pixelsData[i * 4 + 2] = 0;
    pixelsData[i * 4 + 3] = 255;
  }

  for (int i = numPixels / 2; i < numPixels; ++i) {
    pixelsData[i * 4 + 0] = 0;
    pixelsData[i * 4 + 1] = 255;
    pixelsData[i * 4 + 2] = 0;
    pixelsData[i * 4 + 3] = 255;
  }

  GlTexture2D srcTex = makeNearestRGBA8888Texture2D(width, height);
  srcTex.bind();
  srcTex.updateImage(pixelsData.data());
  srcTex.unbind();

  // Create destination texture
  GlTexture2D destTex;
  destTex.initialize(GL_TEXTURE_2D, GL_R8, width, height, GL_RED, GL_UNSIGNED_BYTE, nullptr);

  // Allocate the destination framebuffer.
  GlFramebufferObject destFb;
  destFb.initialize(std::move(destTex), GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0);

  // Render the source texture to the destination framebuffer.
  GlTexture srcTexHandle = srcTex.tex();
  textureConversion.convertRGBToSingleChannelLum(srcTexHandle, destFb, true);

  // Get output
  Vector<uint8_t> destData(width * height);  // Single channel luminance

  destFb.bind();
  glReadPixels(0, 0, width, height, GL_RED, GL_UNSIGNED_BYTE, destData.data());
  destFb.unbind();

  // Check the flipped output
  const int green2Y = 150;  // 0.587 * 255
  for (int i = 0; i < destData.size() / 2; ++i) {
    EXPECT_EQ(green2Y, destData[i]);
  }

  const int red2Y = 76;  // 0.299 * 255
  for (int i = destData.size() / 2; i < destData.size(); ++i) {
    EXPECT_EQ(red2Y, destData[i]);
  }

  EXPECT_EQ(GL_NO_ERROR, glGetError());
}

}  // namespace c8
