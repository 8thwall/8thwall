// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":pixel-buffer",
    ":pixel-buffers",
    ":pixels",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x959990ce);

#include "c8/pixels/pixel-buffer.h"
#include "c8/pixels/pixel-buffers.h"
#include "gtest/gtest.h"

namespace c8 {

class PixelBufferTest : public ::testing::Test {};

TEST_F(PixelBufferTest, TestYPlaneConstructors) {
  {
    YPlanePixelBuffer p(3, 5);
    YPlanePixels pr = p.pixels();
    EXPECT_EQ(3, pr.rows());
    EXPECT_EQ(5, pr.cols());
    EXPECT_EQ(5, pr.rowBytes());
    EXPECT_NE(nullptr, pr.pixels());
  }

  {
    YPlanePixelBuffer p(3, 5, 7);
    YPlanePixels pr = p.pixels();
    EXPECT_EQ(3, pr.rows());
    EXPECT_EQ(5, pr.cols());
    EXPECT_EQ(7, pr.rowBytes());
    EXPECT_NE(nullptr, pr.pixels());
  }
}

TEST_F(PixelBufferTest, TestUVPlaneConstructors) {
  {
    UVPlanePixelBuffer p(3, 5);
    UVPlanePixels pr = p.pixels();
    EXPECT_EQ(3, pr.rows());
    EXPECT_EQ(5, pr.cols());
    EXPECT_EQ(10, pr.rowBytes());
    EXPECT_NE(nullptr, pr.pixels());
  }

  {
    UVPlanePixelBuffer p(3, 5, 15);
    UVPlanePixels pr = p.pixels();
    EXPECT_EQ(3, pr.rows());
    EXPECT_EQ(5, pr.cols());
    EXPECT_EQ(15, pr.rowBytes());
    EXPECT_NE(nullptr, pr.pixels());
  }
}

TEST_F(PixelBufferTest, TestRGBA8888PlaneConstructors) {
  {
    RGBA8888PlanePixelBuffer p(3, 5);
    RGBA8888PlanePixels pr = p.pixels();
    EXPECT_EQ(3, pr.rows());
    EXPECT_EQ(5, pr.cols());
    EXPECT_EQ(20, pr.rowBytes());
    EXPECT_NE(nullptr, pr.pixels());
  }

  {
    RGBA8888PlanePixelBuffer p(3, 5, 25);
    RGBA8888PlanePixels pr = p.pixels();
    EXPECT_EQ(3, pr.rows());
    EXPECT_EQ(5, pr.cols());
    EXPECT_EQ(25, pr.rowBytes());
    EXPECT_NE(nullptr, pr.pixels());
  }
}

TEST_F(PixelBufferTest, TestCopyPixelBufferSubImage) {
  RGBA8888PlanePixelBuffer p(3, 3);
  auto pixPtr = p.pixels().pixels();
  for (int i = 0; i < 9; i++) {
    pixPtr[4 * i] = i;
    pixPtr[4 * i + 1] = i;
    pixPtr[4 * i + 2] = i;
    pixPtr[4 * i + 3] = i;
  }

  RGBA8888PlanePixelBuffer dst(1, 1);
  copyPixelBufferSubImage(p.pixels(), 1, 1, 0, 0, 1, 1, dst);
  auto dstPtr = dst.pixels().pixels();
  EXPECT_EQ(4, dstPtr[0]);
  EXPECT_EQ(4, dstPtr[1]);
  EXPECT_EQ(4, dstPtr[2]);
  EXPECT_EQ(4, dstPtr[3]);
}

TEST_F(PixelBufferTest, TestR32) {
  {
    R32PlanePixelBuffer p(3, 5);
    ConstR32PlanePixels pix = p.pixels();
    EXPECT_EQ(3, pix.rows());
    EXPECT_EQ(5, pix.cols());
    EXPECT_EQ(5, pix.rowElements());
    EXPECT_NE(nullptr, pix.pixels());
  }
}

TEST_F(PixelBufferTest, TestRGBA32) {
  {
    RGBA32PlanePixelBuffer p(3, 5);
    ConstRGBA32PlanePixels pix = p.pixels();
    EXPECT_EQ(3, pix.rows());
    EXPECT_EQ(5, pix.cols());
    EXPECT_EQ(20, pix.rowElements());
    EXPECT_NE(nullptr, pix.pixels());
  }
}

TEST_F(PixelBufferTest, TestDepthFloatConstructor) {
  {
    DepthFloatPixelBuffer p(3, 5);
    DepthFloatPixels pr = p.pixels();
    EXPECT_EQ(3, pr.rows());
    EXPECT_EQ(5, pr.cols());
    EXPECT_EQ(5, pr.rowElements());
    EXPECT_NE(nullptr, pr.pixels());
  }

  {
    DepthFloatPixelBuffer p(3, 5, 7);
    DepthFloatPixels pr = p.pixels();
    EXPECT_EQ(3, pr.rows());
    EXPECT_EQ(5, pr.cols());
    EXPECT_EQ(7, pr.rowElements());
    EXPECT_NE(nullptr, pr.pixels());
  }
}

TEST_F(PixelBufferTest, TestCopyDepthFloatPixelBuffer) {
  {
    DepthFloatPixelBuffer p(3, 3);
    auto pixPtr = p.pixels().pixels();
    for (int i = 0; i < 9; i++) {
      pixPtr[i] = static_cast<float>(i) * 0.32f;
    }
    DepthFloatPixelBuffer dst(1, 1);
    copyDepthFloatPixelBuffer(dst, p.pixels());
    auto dstPtr = dst.pixels().pixels();
    EXPECT_EQ(0.32f * 1.f, dstPtr[1]);
    EXPECT_EQ(0.32f * 2.f, dstPtr[2]);
    EXPECT_EQ(0.32f * 3.f, dstPtr[3]);
    EXPECT_EQ(0.32f * 8.f, dstPtr[8]);
  }
}

}  // namespace c8
