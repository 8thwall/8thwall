// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":image-warp",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x3a9fcd18);

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "c8/c8-log.h"
#include "c8/geometry/egomotion.h"
#include "reality/engine/geometry/image-warp.h"

using testing::Eq;
using testing::Pointwise;

namespace c8 {

MATCHER_P(AreWithin, eps, "") { return fabs(testing::get<0>(arg) - testing::get<1>(arg)) < eps; }

decltype(auto) equalsMatrix(const HMatrix &matrix) {
  return Pointwise(AreWithin(0.0001), matrix.data());
}

class ImageWarpTest : public ::testing::Test {};

TEST_F(ImageWarpTest, TestRotationToHorizontal) {
  {
    auto m = rotationToHorizontal({1.0f, 0.0f, 0.0f, 0.0f});
    EXPECT_THAT(m.data(), equalsMatrix(HMatrixGen::rotationD(-90.0f, 0.0f, 0.0f)));
  }
  {
    auto m = rotationToHorizontal({std::sqrt(2.0f) / 2.0f, std::sqrt(2.0f) / 2.0f, 0.0f, 0.0f});
    EXPECT_THAT(m.data(), equalsMatrix(HMatrixGen::i()));
  }
}

TEST_F(ImageWarpTest, TestRotationToVertical) {
  {
    auto m = rotationToVertical({1.0f, 0.0f, 0.0f, 0.0f});
    EXPECT_THAT(m.data(), equalsMatrix(HMatrixGen::i()));
  }
  {
    auto m = rotationToVertical({std::sqrt(2.0f) / 2.0f, std::sqrt(2.0f) / 2.0f, 0.0f, 0.0f});
    EXPECT_THAT(m.data(), equalsMatrix(HMatrixGen::rotationD(90.0f, 0.0f, 0.0f)));
  }
}

TEST_F(ImageWarpTest, TestGlPinholeCamera) {
  c8_PixelPinholeCameraModel k{480, 640, 239.5f, 319.5f, 485.9624f, 485.9624f};
  auto K = HMatrixGen::intrinsic(k);
  Vector<HPoint3> pts = {
    {0.000f, 0.000f, 1.0f},
    {0.000f, 639.0f, 1.0f},
    {239.5f, 319.5f, 1.0f},
    {479.0f, 639.0f, 1.0f},
    {479.0f, 0.000f, 1.0f},
  };
  auto wpts = K.inv() * pts;
  auto glk = glPinholeCameraModel(k);
  auto GLK = twoDProjectionMat(HMatrixGen::intrinsic(glk));
  auto glpts = GLK * wpts;

  EXPECT_FLOAT_EQ(glpts[0].x(), -1.0f);
  EXPECT_FLOAT_EQ(glpts[0].y(), -1.0f);
  EXPECT_FLOAT_EQ(glpts[0].z(), 1.0f);

  EXPECT_FLOAT_EQ(glpts[1].x(), -1.0f);
  EXPECT_FLOAT_EQ(glpts[1].y(), 1.0f);
  EXPECT_FLOAT_EQ(glpts[1].z(), 1.0f);

  EXPECT_FLOAT_EQ(glpts[2].x(), 0.0f);
  EXPECT_FLOAT_EQ(glpts[2].y(), 0.0f);
  EXPECT_FLOAT_EQ(glpts[2].z(), 1.0f);

  EXPECT_FLOAT_EQ(glpts[3].x(), 1.0f);
  EXPECT_FLOAT_EQ(glpts[3].y(), 1.0f);
  EXPECT_FLOAT_EQ(glpts[3].z(), 1.0f);

  EXPECT_FLOAT_EQ(glpts[4].x(), 1.0f);
  EXPECT_FLOAT_EQ(glpts[4].y(), -1.0f);
  EXPECT_FLOAT_EQ(glpts[4].z(), 1.0f);
}

TEST_F(ImageWarpTest, TestGlImageTargetWarp) {
  c8_PixelPinholeCameraModel camK{480, 640, 239.5f, 319.5f, 485.9624f, 485.9624f};
  c8_PixelPinholeCameraModel targetK{480, 640, 239.5f, 319.5f, 485.9624f, 485.9624f};

  auto tmat = HMatrixGen::translation(0.01f, -0.02f, -0.03f);
  auto rmat = HMatrixGen::xDegrees(1.0f) * HMatrixGen::yDegrees(3.0f) * HMatrixGen::zDegrees(5.0f);
  auto camLoc = cameraMotion(tmat, rmat);

  auto warp = glImageTargetWarp(targetK, camK, camLoc);

  auto corners = imageTargetCornerPixels(targetK, camK, camLoc);

  auto toGlScale = HMatrixGen::scale(2.0f, 2.0f, 1.0f) * HMatrixGen::translation(-0.5f, -0.5f, 0.0f)
    * HMatrixGen::scale(1.0f / (camK.pixelsWidth - 1), 1.0f / (camK.pixelsHeight - 1), 1.0);

  auto glpts = truncate<2>((warp * toGlScale) * extrude<3>(corners));

  EXPECT_FLOAT_EQ(glpts[0].x(), -0.9f);
  EXPECT_FLOAT_EQ(glpts[0].y(), -0.9f);

  EXPECT_FLOAT_EQ(glpts[1].x(), -0.9f);
  EXPECT_FLOAT_EQ(glpts[1].y(), 0.9f);

  EXPECT_FLOAT_EQ(glpts[2].x(), 0.9f);
  EXPECT_FLOAT_EQ(glpts[2].y(), 0.9f);

  EXPECT_FLOAT_EQ(glpts[3].x(), 0.9f);
  EXPECT_FLOAT_EQ(glpts[3].y(), -0.9f);
}

TEST_F(ImageWarpTest, TestGlImageTargetWarpSquare) {
  c8_PixelPinholeCameraModel camK{480, 640, 239.5f, 319.5f, 485.9624f, 485.9624f};
  c8_PixelPinholeCameraModel targetK{480, 480, 239.5f, 239.5f, 485.9624f, 485.9624f};

  auto tmat = HMatrixGen::translation(0.01f, -0.02f, -0.03f);
  auto rmat = HMatrixGen::xDegrees(1.0f) * HMatrixGen::yDegrees(3.0f) * HMatrixGen::zDegrees(5.0f);
  auto camLoc = cameraMotion(tmat, rmat);

  auto warp = glImageTargetWarp(targetK, camK, camLoc);

  auto corners = imageTargetCornerPixels(targetK, camK, camLoc);

  auto toGlScale = HMatrixGen::scale(2.0f, 2.0f, 1.0f) * HMatrixGen::translation(-0.5f, -0.5f, 0.0f)
    * HMatrixGen::scale(1.0f / (camK.pixelsWidth - 1), 1.0f / (camK.pixelsHeight - 1), 1.0);

  auto glpts = truncate<2>((warp * toGlScale) * extrude<3>(corners));

  EXPECT_FLOAT_EQ(glpts[0].x(), -0.9f);
  EXPECT_FLOAT_EQ(glpts[0].y(), -0.9f * .75f);

  EXPECT_FLOAT_EQ(glpts[1].x(), -0.9f);
  EXPECT_FLOAT_EQ(glpts[1].y(), 0.9f * .75f);

  EXPECT_FLOAT_EQ(glpts[2].x(), 0.9f);
  EXPECT_FLOAT_EQ(glpts[2].y(), 0.9f * .75f);

  EXPECT_FLOAT_EQ(glpts[3].x(), 0.9f);
  EXPECT_FLOAT_EQ(glpts[3].y(), -0.9f * .75f);
}

TEST_F(ImageWarpTest, TestGlImageTargetWarpShort) {
  c8_PixelPinholeCameraModel camK{480, 640, 239.5f, 319.5f, 485.9624f, 485.9624f};
  c8_PixelPinholeCameraModel targetK{480, 560, 239.5f, 279.5f, 485.9624f, 485.9624f};

  auto tmat = HMatrixGen::translation(0.01f, -0.02f, -0.03f);
  auto rmat = HMatrixGen::xDegrees(1.0f) * HMatrixGen::yDegrees(3.0f) * HMatrixGen::zDegrees(5.0f);
  auto camLoc = cameraMotion(tmat, rmat);

  auto warp = glImageTargetWarp(targetK, camK, camLoc);

  auto corners = imageTargetCornerPixels(targetK, camK, camLoc);

  auto toGlScale = HMatrixGen::scale(2.0f, 2.0f, 1.0f) * HMatrixGen::translation(-0.5f, -0.5f, 0.0f)
    * HMatrixGen::scale(1.0f / (camK.pixelsWidth - 1), 1.0f / (camK.pixelsHeight - 1), 1.0);

  auto glpts = truncate<2>((warp * toGlScale) * extrude<3>(corners));

  EXPECT_FLOAT_EQ(glpts[0].x(), -0.9f);
  EXPECT_FLOAT_EQ(glpts[0].y(), -0.9f * .875f);

  EXPECT_FLOAT_EQ(glpts[1].x(), -0.9f);
  EXPECT_FLOAT_EQ(glpts[1].y(), 0.9f * .875f);

  EXPECT_FLOAT_EQ(glpts[2].x(), 0.9f);
  EXPECT_FLOAT_EQ(glpts[2].y(), 0.9f * .875f);

  EXPECT_FLOAT_EQ(glpts[3].x(), 0.9f);
  EXPECT_FLOAT_EQ(glpts[3].y(), -0.9f * .875f);
}

TEST_F(ImageWarpTest, TestGlImageTargetWarpTall) {
  c8_PixelPinholeCameraModel camK{480, 640, 239.5f, 319.5f, 485.9624f, 485.9624f};
  c8_PixelPinholeCameraModel targetK{420, 640, 209.5f, 319.5f, 485.9624f, 485.9624f};

  auto tmat = HMatrixGen::translation(0.01f, -0.02f, -0.03f);
  auto rmat = HMatrixGen::xDegrees(1.0f) * HMatrixGen::yDegrees(3.0f) * HMatrixGen::zDegrees(5.0f);
  auto camLoc = cameraMotion(tmat, rmat);

  auto warp = glImageTargetWarp(targetK, camK, camLoc);

  auto corners = imageTargetCornerPixels(targetK, camK, camLoc);

  auto toGlScale = HMatrixGen::scale(2.0f, 2.0f, 1.0f) * HMatrixGen::translation(-0.5f, -0.5f, 0.0f)
    * HMatrixGen::scale(1.0f / (camK.pixelsWidth - 1), 1.0f / (camK.pixelsHeight - 1), 1.0);

  auto glpts = truncate<2>((warp * toGlScale) * extrude<3>(corners));

  EXPECT_FLOAT_EQ(glpts[0].x(), -0.9f * .875f);
  EXPECT_FLOAT_EQ(glpts[0].y(), -0.9f);

  EXPECT_FLOAT_EQ(glpts[1].x(), -0.9f * .875f);
  EXPECT_FLOAT_EQ(glpts[1].y(), 0.9f);

  EXPECT_FLOAT_EQ(glpts[2].x(), 0.9f * .875f);
  EXPECT_FLOAT_EQ(glpts[2].y(), 0.9f);

  EXPECT_FLOAT_EQ(glpts[3].x(), 0.9f * .875f);
  EXPECT_FLOAT_EQ(glpts[3].y(), -0.9f);
}

TEST_F(ImageWarpTest, TestGlImageTargetWarpThin) {
  c8_PixelPinholeCameraModel camK{480, 640, 239.5f, 319.5f, 485.9624f, 485.9624f};
  c8_PixelPinholeCameraModel targetK{360, 640, 179.5f, 319.5f, 485.9624f, 485.9624f};

  auto tmat = HMatrixGen::translation(0.01f, -0.02f, -0.03f);
  auto rmat = HMatrixGen::xDegrees(1.0f) * HMatrixGen::yDegrees(3.0f) * HMatrixGen::zDegrees(5.0f);
  auto camLoc = cameraMotion(tmat, rmat);

  auto warp = glImageTargetWarp(targetK, camK, camLoc);

  auto corners = imageTargetCornerPixels(targetK, camK, camLoc);

  auto toGlScale = HMatrixGen::scale(2.0f, 2.0f, 1.0f) * HMatrixGen::translation(-0.5f, -0.5f, 0.0f)
    * HMatrixGen::scale(1.0f / (camK.pixelsWidth - 1), 1.0f / (camK.pixelsHeight - 1), 1.0);

  auto glpts = truncate<2>((warp * toGlScale) * extrude<3>(corners));

  EXPECT_FLOAT_EQ(glpts[0].x(), -0.9f * .75f);
  EXPECT_FLOAT_EQ(glpts[0].y(), -0.9f);

  EXPECT_NEAR(glpts[1].x(), -0.9f * .75f, 3e-7);
  EXPECT_FLOAT_EQ(glpts[1].y(), 0.9f);

  EXPECT_FLOAT_EQ(glpts[2].x(), 0.9f * .75f);
  EXPECT_NEAR(glpts[2].y(), 0.9f, 3e-7);

  EXPECT_NEAR(glpts[3].x(), 0.9f * .75f, 3e-7);
  EXPECT_FLOAT_EQ(glpts[3].y(), -0.9f);
}

}  // namespace c8
