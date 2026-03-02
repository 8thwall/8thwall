// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":pixels", "//bzl/inliner:rules", "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x935285cc);

#include "c8/pixels/pixels.h"

#include "gtest/gtest.h"

namespace c8 {

class PixelsTest : public ::testing::Test {};

ConstPixels echo(const ConstPixels &r) { return r; }

TEST_F(PixelsTest, TestDefault) {
  Pixels b1;

  EXPECT_EQ(0, b1.rows());
  EXPECT_EQ(0, b1.cols());
  EXPECT_EQ(0, b1.rowBytes());
  EXPECT_EQ(nullptr, b1.pixels());
}

TEST_F(PixelsTest, TestAccess) {
  uint8_t p = 0;
  Pixels b1(1, 2, 3, &p);

  EXPECT_EQ(1, b1.rows());
  EXPECT_EQ(2, b1.cols());
  EXPECT_EQ(3, b1.rowBytes());
  EXPECT_EQ(&p, b1.pixels());
}

TEST_F(PixelsTest, TestConstAssign) {
  uint8_t p = 0;
  Pixels b1(1, 2, 3, &p);
  ConstPixels cb1 = b1;

  EXPECT_EQ(1, cb1.rows());
  EXPECT_EQ(2, cb1.cols());
  EXPECT_EQ(3, cb1.rowBytes());
  EXPECT_EQ(&p, cb1.pixels());
}

TEST_F(PixelsTest, TestConstCopy) {
  uint8_t p = 0;
  Pixels b1(1, 2, 3, &p);
  ConstPixels cb1(b1);

  EXPECT_EQ(1, cb1.rows());
  EXPECT_EQ(2, cb1.cols());
  EXPECT_EQ(3, cb1.rowBytes());
  EXPECT_EQ(&p, cb1.pixels());
}

TEST_F(PixelsTest, TestYPlanePixelsImplicitConversion) {
  YPlanePixels b0;
  EXPECT_EQ(0, b0.rows());
  EXPECT_EQ(0, b0.cols());
  EXPECT_EQ(0, b0.rowBytes());
  EXPECT_EQ(nullptr, b0.pixels());

  uint8_t p = 0;
  YPlanePixels b1(1, 2, 3, &p);

  {
    OneChannelPixels &r = b1;
    EXPECT_EQ(1, r.rows());
    EXPECT_EQ(2, r.cols());
    EXPECT_EQ(3, r.rowBytes());
    EXPECT_EQ(&p, r.pixels());
  }

  {
    ConstYPlanePixels cb1 = b1;
    ConstOneChannelPixels &r = cb1;
    EXPECT_EQ(1, r.rows());
    EXPECT_EQ(2, r.cols());
    EXPECT_EQ(3, r.rowBytes());
    EXPECT_EQ(&p, r.pixels());
  }

  {
    Pixels &r = b1;
    EXPECT_EQ(1, r.rows());
    EXPECT_EQ(2, r.cols());
    EXPECT_EQ(3, r.rowBytes());
    EXPECT_EQ(&p, r.pixels());
  }

  {
    ConstYPlanePixels cb1 = b1;
    ConstPixels &r = cb1;
    EXPECT_EQ(1, r.rows());
    EXPECT_EQ(2, r.cols());
    EXPECT_EQ(3, r.rowBytes());
    EXPECT_EQ(&p, r.pixels());
  }

  {
    ConstPixels r = echo(b1);
    EXPECT_EQ(1, r.rows());
    EXPECT_EQ(2, r.cols());
    EXPECT_EQ(3, r.rowBytes());
    EXPECT_EQ(&p, r.pixels());
  }
}

TEST_F(PixelsTest, TestUVPlanePixelsImplicitConversion) {
  UVPlanePixels b0;
  EXPECT_EQ(0, b0.rows());
  EXPECT_EQ(0, b0.cols());
  EXPECT_EQ(0, b0.rowBytes());
  EXPECT_EQ(nullptr, b0.pixels());

  uint8_t p = 0;
  UVPlanePixels b1(1, 2, 3, &p);

  {
    TwoChannelPixels &r = b1;
    EXPECT_EQ(1, r.rows());
    EXPECT_EQ(2, r.cols());
    EXPECT_EQ(3, r.rowBytes());
    EXPECT_EQ(&p, r.pixels());
  }

  {
    ConstUVPlanePixels cb1 = b1;
    ConstTwoChannelPixels &r = cb1;
    EXPECT_EQ(1, r.rows());
    EXPECT_EQ(2, r.cols());
    EXPECT_EQ(3, r.rowBytes());
    EXPECT_EQ(&p, r.pixels());
  }

  {
    Pixels &r = b1;
    EXPECT_EQ(1, r.rows());
    EXPECT_EQ(2, r.cols());
    EXPECT_EQ(3, r.rowBytes());
    EXPECT_EQ(&p, r.pixels());
  }

  {
    ConstUVPlanePixels cb1 = b1;
    ConstPixels &r = cb1;
    EXPECT_EQ(1, r.rows());
    EXPECT_EQ(2, r.cols());
    EXPECT_EQ(3, r.rowBytes());
    EXPECT_EQ(&p, r.pixels());
  }

  {
    ConstPixels r = echo(b1);
    EXPECT_EQ(1, r.rows());
    EXPECT_EQ(2, r.cols());
    EXPECT_EQ(3, r.rowBytes());
    EXPECT_EQ(&p, r.pixels());
  }
}

TEST_F(PixelsTest, TestBGR888PlanePixelsImplicitConversion) {
  BGR888PlanePixels b0;
  EXPECT_EQ(0, b0.rows());
  EXPECT_EQ(0, b0.cols());
  EXPECT_EQ(0, b0.rowBytes());
  EXPECT_EQ(nullptr, b0.pixels());

  uint8_t p = 0;
  BGR888PlanePixels b1(1, 2, 3, &p);

  {
    ThreeChannelPixels &r = b1;
    EXPECT_EQ(1, r.rows());
    EXPECT_EQ(2, r.cols());
    EXPECT_EQ(3, r.rowBytes());
    EXPECT_EQ(&p, r.pixels());
  }

  {
    ConstBGR888PlanePixels cb1 = b1;
    ConstThreeChannelPixels &r = cb1;
    EXPECT_EQ(1, r.rows());
    EXPECT_EQ(2, r.cols());
    EXPECT_EQ(3, r.rowBytes());
    EXPECT_EQ(&p, r.pixels());
  }

  {
    Pixels &r = b1;
    EXPECT_EQ(1, r.rows());
    EXPECT_EQ(2, r.cols());
    EXPECT_EQ(3, r.rowBytes());
    EXPECT_EQ(&p, r.pixels());
  }

  {
    ConstBGR888PlanePixels cb1 = b1;
    ConstPixels &r = cb1;
    EXPECT_EQ(1, r.rows());
    EXPECT_EQ(2, r.cols());
    EXPECT_EQ(3, r.rowBytes());
    EXPECT_EQ(&p, r.pixels());
  }

  {
    ConstPixels r = echo(b1);
    EXPECT_EQ(1, r.rows());
    EXPECT_EQ(2, r.cols());
    EXPECT_EQ(3, r.rowBytes());
    EXPECT_EQ(&p, r.pixels());
  }
}

TEST_F(PixelsTest, TestRGBA8888PlanePixelsImplicitConversion) {
  RGBA8888PlanePixels b0;
  EXPECT_EQ(0, b0.rows());
  EXPECT_EQ(0, b0.cols());
  EXPECT_EQ(0, b0.rowBytes());
  EXPECT_EQ(nullptr, b0.pixels());

  uint8_t p = 0;
  RGBA8888PlanePixels b1(1, 2, 3, &p);

  {
    FourChannelPixels &r = b1;
    EXPECT_EQ(1, r.rows());
    EXPECT_EQ(2, r.cols());
    EXPECT_EQ(3, r.rowBytes());
    EXPECT_EQ(&p, r.pixels());
  }

  {
    ConstRGBA8888PlanePixels cb1 = b1;
    ConstFourChannelPixels &r = cb1;
    EXPECT_EQ(1, r.rows());
    EXPECT_EQ(2, r.cols());
    EXPECT_EQ(3, r.rowBytes());
    EXPECT_EQ(&p, r.pixels());
  }

  {
    Pixels &r = b1;
    EXPECT_EQ(1, r.rows());
    EXPECT_EQ(2, r.cols());
    EXPECT_EQ(3, r.rowBytes());
    EXPECT_EQ(&p, r.pixels());
  }

  {
    ConstRGBA8888PlanePixels cb1 = b1;
    ConstPixels &r = cb1;
    EXPECT_EQ(1, r.rows());
    EXPECT_EQ(2, r.cols());
    EXPECT_EQ(3, r.rowBytes());
    EXPECT_EQ(&p, r.pixels());
  }

  {
    ConstPixels r = echo(b1);
    EXPECT_EQ(1, r.rows());
    EXPECT_EQ(2, r.cols());
    EXPECT_EQ(3, r.rowBytes());
    EXPECT_EQ(&p, r.pixels());
  }
}

class FloatPixelsTest : public ::testing::Test {};

TEST_F(FloatPixelsTest, TestFloatDefault) {
  FloatPixels b1;

  EXPECT_EQ(0, b1.rows());
  EXPECT_EQ(0, b1.cols());
  EXPECT_EQ(0, b1.rowElements());
  EXPECT_EQ(nullptr, b1.pixels());
}

TEST_F(FloatPixelsTest, TestFloatAccess) {
  float p = 0;
  FloatPixels b1(1, 2, 3, &p);

  EXPECT_EQ(1, b1.rows());
  EXPECT_EQ(2, b1.cols());
  EXPECT_EQ(3, b1.rowElements());
  EXPECT_EQ(&p, b1.pixels());
}

TEST_F(FloatPixelsTest, TestConstFloatAssign) {
  float p = 0;
  FloatPixels b1(1, 2, 3, &p);
  ConstFloatPixels cb1 = b1;

  EXPECT_EQ(1, cb1.rows());
  EXPECT_EQ(2, cb1.cols());
  EXPECT_EQ(3, cb1.rowElements());
  EXPECT_EQ(&p, cb1.pixels());
}

TEST_F(FloatPixelsTest, TestConstFloatCopy) {
  float p = 0;
  FloatPixels b1(1, 2, 3, &p);
  ConstFloatPixels cb1(b1);

  EXPECT_EQ(1, cb1.rows());
  EXPECT_EQ(2, cb1.cols());
  EXPECT_EQ(3, cb1.rowElements());
  EXPECT_EQ(&p, cb1.pixels());
}

}  // namespace c8
