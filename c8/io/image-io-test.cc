// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":image-io",
    ":file-io",
    "@com_google_googletest//:gtest_main",
    "//c8:string",
  };
  data = {
    ":tiny-jpg",
    ":tiny-png",
    ":tiny-tiff",
    ":tiny-gif",
    "//c8/pixels/testdata:test-png-color-types",
  };
}
cc_end(0x651786ae);

#include <cstdio>

#include "c8/io/file-io.h"
#include "c8/io/image-io.h"
#include "c8/string.h"
#include "gtest/gtest.h"

namespace c8 {

class ImageIOTest : public ::testing::Test {};

static const char *tinyJpgPath() {
  static String dir = "c8/io/tiny-image.jpg";
  return dir.c_str();
}

static const char *tinyPngPath() {
  static String dir = "c8/io/tiny-image.png";
  return dir.c_str();
}

static const char *tinyTiffPath() {
  static String dir = "c8/io/tiny-image.tiff";
  return dir.c_str();
}

static const char *tinyGifPath() {
  static String dir = "c8/io/tiny-image.gif";
  return dir.c_str();
}

static const char *smallGrayscalePngPath() {
  static String dir = "c8/pixels/testdata/grayscale_sample.png";
  return dir.c_str();
}

static const char *smallGrayscaleAlphaPngPath() {
  static String dir = "c8/pixels/testdata/grayscale_alpha_sample.png";
  return dir.c_str();
}

static const char *smallPalettePngPath() {
  static String dir = "c8/pixels/testdata/palette_sample.png";
  return dir.c_str();
}

TEST_F(ImageIOTest, DecompressJpgToY) {
  auto yBuffer = readJpgToY(tinyJpgPath());
  EXPECT_EQ(yBuffer.pixels().cols(), 5);
  EXPECT_EQ(yBuffer.pixels().rows(), 7);
  EXPECT_EQ(yBuffer.pixels().rowBytes(), 8);
}

TEST_F(ImageIOTest, DecompressJpgToRGB) {
  auto rgbBuffer = readJpgToRGB(tinyJpgPath());
  EXPECT_EQ(rgbBuffer.pixels().cols(), 5);
  EXPECT_EQ(rgbBuffer.pixels().rows(), 7);
  EXPECT_EQ(rgbBuffer.pixels().rowBytes(), 16);
}

TEST_F(ImageIOTest, DecompressJpgToBGR) {
  auto bgrBuffer = readJpgToBGR(tinyJpgPath());
  EXPECT_EQ(bgrBuffer.pixels().cols(), 5);
  EXPECT_EQ(bgrBuffer.pixels().rows(), 7);
  EXPECT_EQ(bgrBuffer.pixels().rowBytes(), 16);
}

TEST_F(ImageIOTest, DecompressJpgToRGBA) {
  auto rgbaBuffer = readJpgToRGBA(tinyJpgPath());
  EXPECT_EQ(rgbaBuffer.pixels().cols(), 5);
  EXPECT_EQ(rgbaBuffer.pixels().rows(), 7);
  EXPECT_EQ(rgbaBuffer.pixels().rowBytes(), 20);
}

TEST_F(ImageIOTest, DecompressImageToY) {
  auto yBuffer = readImageToY(tinyJpgPath());
  EXPECT_EQ(yBuffer.pixels().cols(), 5);
  EXPECT_EQ(yBuffer.pixels().rows(), 7);
  EXPECT_EQ(yBuffer.pixels().rowBytes(), 8);

  yBuffer = readImageToY(tinyPngPath());
  EXPECT_EQ(yBuffer.pixels().cols(), 5);
  EXPECT_EQ(yBuffer.pixels().rows(), 7);
  EXPECT_EQ(yBuffer.pixels().rowBytes(), 5);

  try {
    yBuffer = readImageToY(tinyTiffPath());
    FAIL() << "Runtime Error Expected for type TIFF";
  } catch (RuntimeError e) {
    // pass
  }
}

TEST_F(ImageIOTest, DecompressGifToRGBA) {
  auto rgbaBuffer = readGifToRGBA(tinyGifPath());
  EXPECT_EQ(rgbaBuffer.pixels().cols(), 5);
  EXPECT_EQ(rgbaBuffer.pixels().rows(), 7);
  EXPECT_EQ(rgbaBuffer.pixels().rowBytes(), 20);
}

TEST_F(ImageIOTest, CompressYAndUvPlanes) {
  YPlanePixelBuffer yPlane{16, 16};
  UPlanePixelBuffer uPlane{8, 8};
  VPlanePixelBuffer vPlane{8, 8};

  auto data = writePixelsToJpg(yPlane.pixels(), uPlane.pixels(), vPlane.pixels());
  EXPECT_LT(0, data.size());
}

Vector<uint8_t> generateJpg(int rows, int cols) {
  YPlanePixelBuffer yPlane{rows, cols};
  for (int i = 0; i < rows * cols; i++) {
    yPlane.pixels().pixels()[i] = 128;
  }

  UPlanePixelBuffer uPlane{rows / 2, cols / 2};
  VPlanePixelBuffer vPlane{rows / 2, cols / 2};
  return writePixelsToJpg(yPlane.pixels(), uPlane.pixels(), vPlane.pixels());
}

TEST_F(ImageIOTest, AllocateBufJpg) {
  Vector<uint8_t> data = generateJpg(32, 16);

  YPlanePixelBuffer yBuf;
  UPlanePixelBuffer uBuf;
  VPlanePixelBuffer vBuf;
  readJpgToPixelsAllocate(data.data(), data.size(), &yBuf, &uBuf, &vBuf);
  EXPECT_EQ(16, yBuf.pixels().cols());
  EXPECT_EQ(32, yBuf.pixels().rows());
  EXPECT_EQ(8, uBuf.pixels().cols());
  EXPECT_EQ(16, uBuf.pixels().rows());
  EXPECT_EQ(8, vBuf.pixels().cols());
  EXPECT_EQ(16, vBuf.pixels().rows());
}

TEST_F(ImageIOTest, WriteThenRead) {
  Vector<uint8_t> data = generateJpg(8, 4);

  YPlanePixelBuffer yBuf;
  UPlanePixelBuffer uBuf;
  VPlanePixelBuffer vBuf;
  readJpgToPixelsAllocate(data.data(), data.size(), &yBuf, &uBuf, &vBuf);
  readJpgToPixels(data.data(), data.size(), yBuf.pixels(), uBuf.pixels(), vBuf.pixels());
  // A simple flat image should be fully recoverable
  EXPECT_EQ(128, yBuf.pixels().pixels()[10]);
  EXPECT_EQ(128, yBuf.pixels().pixels()[21]);
  EXPECT_EQ(128, yBuf.pixels().pixels()[31]);
}

TEST_F(ImageIOTest, AllocateBufGif) {
  auto gifBuffer = readFile(tinyGifPath());
  RGBA8888PlanePixelBuffer rgbaBuf;
  readGifToPixelsAllocate(gifBuffer.data(), gifBuffer.size(), &rgbaBuf);
  EXPECT_EQ(5, rgbaBuf.pixels().cols());
  EXPECT_EQ(7, rgbaBuf.pixels().rows());
  EXPECT_EQ(20, rgbaBuf.pixels().rowBytes());
}

TEST_F(ImageIOTest, DecompressImageToRGB) {
  auto rgbBuffer = readImageToRGB(tinyJpgPath());
  EXPECT_EQ(rgbBuffer.pixels().cols(), 5);
  EXPECT_EQ(rgbBuffer.pixels().rows(), 7);
  EXPECT_EQ(rgbBuffer.pixels().rowBytes(), 16);

  rgbBuffer = readImageToRGB(tinyPngPath());
  EXPECT_EQ(rgbBuffer.pixels().cols(), 5);
  EXPECT_EQ(rgbBuffer.pixels().rows(), 7);
  EXPECT_EQ(rgbBuffer.pixels().rowBytes(), 15);

  try {
    rgbBuffer = readImageToRGB(tinyTiffPath());
    FAIL() << "Runtime Error Expected for type TIFF";
  } catch (RuntimeError e) {
    // pass
  }
}

TEST_F(ImageIOTest, DecompressImageToBGR) {
  auto bgrBuffer = readImageToBGR(tinyJpgPath());
  EXPECT_EQ(bgrBuffer.pixels().cols(), 5);
  EXPECT_EQ(bgrBuffer.pixels().rows(), 7);
  EXPECT_EQ(bgrBuffer.pixels().rowBytes(), 16);

  bgrBuffer = readImageToBGR(tinyPngPath());
  EXPECT_EQ(bgrBuffer.pixels().cols(), 5);
  EXPECT_EQ(bgrBuffer.pixels().rows(), 7);
  EXPECT_EQ(bgrBuffer.pixels().rowBytes(), 15);

  try {
    bgrBuffer = readImageToBGR(tinyTiffPath());
    FAIL() << "Runtime Error Expected for type TIFF";
  } catch (RuntimeError e) {
    // pass
  }
}

TEST_F(ImageIOTest, DecompressImageToRGBA) {
  const int ALPHA_CHANNEL_INDEX = 3;
  auto rgbaBuffer = readImageToRGBA(tinyJpgPath());
  EXPECT_EQ(rgbaBuffer.pixels().cols(), 5);
  EXPECT_EQ(rgbaBuffer.pixels().rows(), 7);
  EXPECT_EQ(rgbaBuffer.pixels().rowBytes(), 20);
  EXPECT_EQ(rgbaBuffer.pixels().pixels()[ALPHA_CHANNEL_INDEX], 0xFF);

  rgbaBuffer = readImageToRGBA(tinyPngPath());
  EXPECT_EQ(rgbaBuffer.pixels().cols(), 5);
  EXPECT_EQ(rgbaBuffer.pixels().rows(), 7);
  EXPECT_EQ(rgbaBuffer.pixels().rowBytes(), 20);

  rgbaBuffer = readImageToRGBA(tinyGifPath());
  EXPECT_EQ(rgbaBuffer.pixels().cols(), 5);
  EXPECT_EQ(rgbaBuffer.pixels().rows(), 7);
  EXPECT_EQ(rgbaBuffer.pixels().rowBytes(), 20);
  EXPECT_EQ(rgbaBuffer.pixels().pixels()[ALPHA_CHANNEL_INDEX], 0xFF);

  rgbaBuffer = readImageToRGBA(smallGrayscalePngPath());
  EXPECT_EQ(rgbaBuffer.pixels().cols(), 150);
  EXPECT_EQ(rgbaBuffer.pixels().rows(), 200);
  EXPECT_EQ(rgbaBuffer.pixels().rowBytes(), 600);
  EXPECT_EQ(rgbaBuffer.pixels().pixels()[ALPHA_CHANNEL_INDEX], 0xFF);

  rgbaBuffer = readImageToRGBA(smallGrayscaleAlphaPngPath());
  EXPECT_EQ(rgbaBuffer.pixels().cols(), 256);
  EXPECT_EQ(rgbaBuffer.pixels().rows(), 256);
  EXPECT_EQ(rgbaBuffer.pixels().rowBytes(), 1024);
  EXPECT_EQ(rgbaBuffer.pixels().pixels()[ALPHA_CHANNEL_INDEX], 0x00);

  rgbaBuffer = readImageToRGBA(smallPalettePngPath());
  EXPECT_EQ(rgbaBuffer.pixels().cols(), 150);
  EXPECT_EQ(rgbaBuffer.pixels().rows(), 200);
  EXPECT_EQ(rgbaBuffer.pixels().rowBytes(), 600);
  EXPECT_EQ(rgbaBuffer.pixels().pixels()[ALPHA_CHANNEL_INDEX], 0xFF);

  try {
    rgbaBuffer = readImageToRGBA(tinyTiffPath());
    FAIL() << "Runtime Error Expected for type TIFF";
  } catch (RuntimeError e) {
    // pass
  }
}

TEST_F(ImageIOTest, GifThrowOnInvalidInputPath) {
  EXPECT_THROW({ readGifToRGBA("invalid-path.gif"); }, RuntimeError);
}

TEST_F(ImageIOTest, GifThrowOnInvalidInputBuffer) {
  EXPECT_THROW({ readGifToRGBA(nullptr, 0); }, RuntimeError);
}

TEST_F(ImageIOTest, WritePngFromYPlanePixels) {
  YPlanePixelBuffer yPlane{16, 16};
  for (int i = 0; i < 16 * 16; ++i) {
    yPlane.pixels().pixels()[i] = 123;
  }

  auto pngData = writePixelsToPng(yPlane.pixels());
  EXPECT_LT(0, pngData.size());

  auto yOut = readPngToY(pngData.data(), pngData.size());
  EXPECT_EQ(yOut.pixels().cols(), 16);
  EXPECT_EQ(yOut.pixels().rows(), 16);
  for (int i = 0; i < 16 * 16; ++i) {
    EXPECT_EQ(yOut.pixels().pixels()[i], 123);
  }
}

TEST_F(ImageIOTest, WritePngFromRGB888PlanePixels) {
  RGB888PlanePixelBuffer rgb{8, 8};
  for (int i = 0; i < 8 * 8 * 3; ++i) {
    rgb.pixels().pixels()[i] = static_cast<uint8_t>(i % 256);
  }

  auto pngData = writePixelsToPng(rgb.pixels());
  EXPECT_LT(0, pngData.size());

  auto rgbOut = readPngToRGB(pngData.data(), pngData.size());
  EXPECT_EQ(rgbOut.pixels().cols(), 8);
  EXPECT_EQ(rgbOut.pixels().rows(), 8);
  for (int i = 0; i < 8 * 8 * 3; ++i) {
    EXPECT_EQ(rgbOut.pixels().pixels()[i], static_cast<uint8_t>(i % 256));
  }
}

TEST_F(ImageIOTest, WritePngFromBGR888PlanePixels) {
  BGR888PlanePixelBuffer bgr{8, 8};
  for (int i = 0; i < 8 * 8 * 3; ++i) {
    bgr.pixels().pixels()[i] = static_cast<uint8_t>(255 - (i % 256));
  }

  auto pngData = writePixelsToPng(bgr.pixels());
  EXPECT_LT(0, pngData.size());

  auto bgrOut = readPngToBGR(pngData.data(), pngData.size());
  EXPECT_EQ(bgrOut.pixels().cols(), 8);
  EXPECT_EQ(bgrOut.pixels().rows(), 8);
  for (int i = 0; i < 8 * 8 * 3; ++i) {
    EXPECT_EQ(bgrOut.pixels().pixels()[i], static_cast<uint8_t>(255 - (i % 256)));
  }
}

TEST_F(ImageIOTest, WritePngFromBGRA8888PlanePixels) {
  BGRA8888PlanePixelBuffer bgra{8, 8};
  for (int i = 0; i < 8 * 8 * 4; ++i) {
    // Fill B=10, G=20, R=30, A=255.
    switch (i % 4) {
      case 0:
        bgra.pixels().pixels()[i] = 10;
        break;
      case 1:
        bgra.pixels().pixels()[i] = 20;
        break;
      case 2:
        bgra.pixels().pixels()[i] = 30;
        break;
      case 3:
        bgra.pixels().pixels()[i] = 255;
        break;
    }
  }

  auto pngData = writePixelsToPng(bgra.pixels());
  EXPECT_LT(0, pngData.size());

  auto bgraOut = readPngToBGRA(pngData.data(), pngData.size());
  EXPECT_EQ(bgraOut.pixels().cols(), 8);
  EXPECT_EQ(bgraOut.pixels().rows(), 8);
  for (int i = 0; i < 8 * 8 * 4; ++i) {
    EXPECT_EQ(
      bgraOut.pixels().pixels()[i],
      (i % 4 == 0)     ? 10
        : (i % 4 == 1) ? 20
        : (i % 4 == 2) ? 30
                       : 255);
  }
}

TEST_F(ImageIOTest, WritePngFromRGBA8888PlanePixels) {
  RGBA8888PlanePixelBuffer rgba{8, 8};
  for (int i = 0; i < 8 * 8 * 4; ++i) {
    rgba.pixels().pixels()[i] = (i % 4 == 3) ? 255 : 100;  // A = 255, RGB = 100
  }

  auto pngData = writePixelsToPng(rgba.pixels());
  EXPECT_LT(0, pngData.size());

  auto rgbaOut = readPngToRGBA(pngData.data(), pngData.size());
  EXPECT_EQ(rgbaOut.pixels().cols(), 8);
  EXPECT_EQ(rgbaOut.pixels().rows(), 8);
  for (int i = 0; i < 8 * 8 * 4; ++i) {
    EXPECT_EQ(rgbaOut.pixels().pixels()[i], (i % 4 == 3) ? 255 : 100);
  }
}

TEST_F(ImageIOTest, WriteJpgFromBGRA8888PlanePixels) {
  BGRA8888PlanePixelBuffer bgra{8, 8};
  for (int i = 0; i < 8 * 8 * 4; ++i) {
    // Fill B=50, G=100, R=150, A=200.
    switch (i % 4) {
      case 0:
        bgra.pixels().pixels()[i] = 50;
        break;
      case 1:
        bgra.pixels().pixels()[i] = 100;
        break;
      case 2:
        bgra.pixels().pixels()[i] = 150;
        break;
      case 3:
        bgra.pixels().pixels()[i] = 200;
        break;
    }
  }

  auto jpgData = writePixelsToJpg(bgra.pixels(), 90);
  EXPECT_LT(0, jpgData.size());

  auto bgraOut = readJpgToBGRA(jpgData.data(), jpgData.size());
  EXPECT_EQ(bgraOut.pixels().cols(), 8);
  EXPECT_EQ(bgraOut.pixels().rows(), 8);

  // Alpha is not stored in JPEG, so we expect it to be added back as 0xFF
  const int ALPHA_CHANNEL_INDEX = 3;
  for (int i = 0; i < 8 * 8; ++i) {
    EXPECT_EQ(bgraOut.pixels().pixels()[i * 4 + ALPHA_CHANNEL_INDEX], 0xFF);
  }
}

TEST_F(ImageIOTest, ReadRGBPngAsBGRA) {
  RGB888PlanePixelBuffer rgb{4, 4};
  for (int i = 0; i < 4 * 4 * 3; ++i) {
    rgb.pixels().pixels()[i] = (i % 3 == 0) ? 100 : (i % 3 == 1) ? 150 : 200;
  }

  auto pngData = writePixelsToPng(rgb.pixels());
  auto bgra = readPngToBGRA(pngData.data(), pngData.size());

  for (int i = 0; i < 4 * 4; ++i) {
    EXPECT_EQ(bgra.pixels().pixels()[i * 4 + 0], 200);  // B
    EXPECT_EQ(bgra.pixels().pixels()[i * 4 + 1], 150);  // G
    EXPECT_EQ(bgra.pixels().pixels()[i * 4 + 2], 100);  // R
    EXPECT_EQ(bgra.pixels().pixels()[i * 4 + 3], 255);  // A
  }
}

TEST_F(ImageIOTest, ReadRGBAPngAsBGRA) {
  RGBA8888PlanePixelBuffer rgba{2, 2};
  for (int i = 0; i < 2 * 2 * 4; ++i) {
    rgba.pixels().pixels()[i] = (i % 4 == 0) ? 1 : (i % 4 == 1) ? 2 : (i % 4 == 2) ? 3 : 4;
  }

  auto pngData = writePixelsToPng(rgba.pixels());
  auto bgra = readPngToBGRA(pngData.data(), pngData.size());

  for (int i = 0; i < 4; ++i) {
    EXPECT_EQ(bgra.pixels().pixels()[i * 4 + 0], 3);  // B
    EXPECT_EQ(bgra.pixels().pixels()[i * 4 + 1], 2);  // G
    EXPECT_EQ(bgra.pixels().pixels()[i * 4 + 2], 1);  // R
    EXPECT_EQ(bgra.pixels().pixels()[i * 4 + 3], 4);  // A
  }
}

TEST_F(ImageIOTest, ReadGrayscalePngAsBGRA) {
  YPlanePixelBuffer gray{3, 3};
  for (int i = 0; i < 3 * 3; ++i) {
    gray.pixels().pixels()[i] = static_cast<uint8_t>(i * 10);
  }

  auto pngData = writePixelsToPng(gray.pixels());
  auto bgra = readPngToBGRA(pngData.data(), pngData.size());

  for (int i = 0; i < 9; ++i) {
    uint8_t v = static_cast<uint8_t>(i * 10);
    EXPECT_EQ(bgra.pixels().pixels()[i * 4 + 0], v);    // B
    EXPECT_EQ(bgra.pixels().pixels()[i * 4 + 1], v);    // G
    EXPECT_EQ(bgra.pixels().pixels()[i * 4 + 2], v);    // R
    EXPECT_EQ(bgra.pixels().pixels()[i * 4 + 3], 255);  // A
  }
}

TEST_F(ImageIOTest, ReadGrayscaleAlphaPngAsBGRA) {
  // Simulate grayscale+alpha by creating RGBA with equal RGB and known alpha
  RGBA8888PlanePixelBuffer grayAlpha{2, 2};
  for (int i = 0; i < 4; ++i) {
    uint8_t g = static_cast<uint8_t>(i * 50);
    grayAlpha.pixels().pixels()[i * 4 + 0] = g;
    grayAlpha.pixels().pixels()[i * 4 + 1] = g;
    grayAlpha.pixels().pixels()[i * 4 + 2] = g;
    grayAlpha.pixels().pixels()[i * 4 + 3] = 128;
  }

  auto pngData = writePixelsToPng(grayAlpha.pixels());
  auto bgra = readPngToBGRA(pngData.data(), pngData.size());

  for (int i = 0; i < 4; ++i) {
    uint8_t g = static_cast<uint8_t>(i * 50);
    EXPECT_EQ(bgra.pixels().pixels()[i * 4 + 0], g);    // B
    EXPECT_EQ(bgra.pixels().pixels()[i * 4 + 1], g);    // G
    EXPECT_EQ(bgra.pixels().pixels()[i * 4 + 2], g);    // R
    EXPECT_EQ(bgra.pixels().pixels()[i * 4 + 3], 128);  // A
  }
}

TEST_F(ImageIOTest, ReadPalettedPngAsBGRA) {
  // We'll simulate a palette image by writing an RGB image and transforming it in libpng.
  // True palette input would require manual PNG generation or use of palette writer (not present).

  RGB888PlanePixelBuffer palette{2, 2};
  for (int i = 0; i < 4; ++i) {
    palette.pixels().pixels()[i * 3 + 0] = 0;    // R
    palette.pixels().pixels()[i * 3 + 1] = 128;  // G
    palette.pixels().pixels()[i * 3 + 2] = 255;  // B
  }

  auto pngData = writePixelsToPng(palette.pixels());
  auto bgra = readPngToBGRA(pngData.data(), pngData.size());

  for (int i = 0; i < 4; ++i) {
    EXPECT_EQ(bgra.pixels().pixels()[i * 4 + 0], 255);  // B
    EXPECT_EQ(bgra.pixels().pixels()[i * 4 + 1], 128);  // G
    EXPECT_EQ(bgra.pixels().pixels()[i * 4 + 2], 0);    // R
    EXPECT_EQ(bgra.pixels().pixels()[i * 4 + 3], 255);  // A
  }
}

}  // namespace c8
