// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":draw2",
    ":embedded-drawing-font",
    "//c8:color",
    "//c8/pixels:pixel-buffer",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xad62546c);

#include "c8/pixels/draw2.h"

#include "c8/pixels/pixel-buffer.h"
#include "c8/color.h"
#include "c8/pixels/embedded-drawing-font.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace c8 {

class Draw2Test : public ::testing::Test {
protected:
  Draw2Test() : canvas_(5, 5) {}

  Color pix(int x, int y) {
    uint8_t *p = canvas_.pixels().pixels() + (x << 2) + y * canvas_.pixels().rowBytes();
    return Color(p[0], p[1], p[2], p[3]);
  };

  RGBA8888PlanePixelBuffer canvas_;
};

TEST_F(Draw2Test, HorizontalExactLine) {
  // Draw a pure horizontal line centered in the image.
  fill(Color::OFF_WHITE, canvas_.pixels());
  drawLine({0.5f, 2.5f}, {4.5f, 2.5f}, 1, Color::PURPLE_GRAY, canvas_.pixels());

  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.5f), pix(0, 2));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(1, 2));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(2, 2));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(3, 2));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.5f), pix(4, 2));

  EXPECT_EQ(Color::OFF_WHITE, pix(0, 1));
  EXPECT_EQ(Color::OFF_WHITE, pix(1, 1));
  EXPECT_EQ(Color::OFF_WHITE, pix(2, 1));
  EXPECT_EQ(Color::OFF_WHITE, pix(3, 1));
  EXPECT_EQ(Color::OFF_WHITE, pix(4, 1));

  EXPECT_EQ(Color::OFF_WHITE, pix(0, 3));
  EXPECT_EQ(Color::OFF_WHITE, pix(1, 3));
  EXPECT_EQ(Color::OFF_WHITE, pix(2, 3));
  EXPECT_EQ(Color::OFF_WHITE, pix(3, 3));
  EXPECT_EQ(Color::OFF_WHITE, pix(4, 3));
}
TEST_F(Draw2Test, Horizontal5050Line) {
  // Draw a pure horizontal line that is split 50%/50% between two pixels.
  fill(Color::OFF_WHITE, canvas_.pixels());
  drawLine({0.5f, 2.0f}, {4.5f, 2.0f}, 1, Color::PURPLE_GRAY, canvas_.pixels());

  EXPECT_EQ(Color::OFF_WHITE, pix(0, 0));
  EXPECT_EQ(Color::OFF_WHITE, pix(1, 0));
  EXPECT_EQ(Color::OFF_WHITE, pix(2, 0));
  EXPECT_EQ(Color::OFF_WHITE, pix(3, 0));
  EXPECT_EQ(Color::OFF_WHITE, pix(4, 0));

  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.25f), pix(0, 1));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.5f), pix(1, 1));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.5f), pix(2, 1));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.5f), pix(3, 1));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.25f), pix(4, 1));

  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.25f), pix(0, 2));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.5f), pix(1, 2));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.5f), pix(2, 2));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.5f), pix(3, 2));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.25f), pix(4, 2));

  EXPECT_EQ(Color::OFF_WHITE, pix(0, 3));
  EXPECT_EQ(Color::OFF_WHITE, pix(1, 3));
  EXPECT_EQ(Color::OFF_WHITE, pix(2, 3));
  EXPECT_EQ(Color::OFF_WHITE, pix(3, 3));
  EXPECT_EQ(Color::OFF_WHITE, pix(4, 3));
}

TEST_F(Draw2Test, Horizontal2575Line) {
  // Draw a pure horizontal line that is split 25%/75% between two pixels.
  fill(Color::OFF_WHITE, canvas_.pixels());
  drawLine({0.5f, 2.75f}, {4.5f, 2.75f}, 1, Color::PURPLE_GRAY, canvas_.pixels());

  EXPECT_EQ(Color::OFF_WHITE, pix(0, 1));
  EXPECT_EQ(Color::OFF_WHITE, pix(1, 1));
  EXPECT_EQ(Color::OFF_WHITE, pix(2, 1));
  EXPECT_EQ(Color::OFF_WHITE, pix(3, 1));
  EXPECT_EQ(Color::OFF_WHITE, pix(4, 1));

  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.375f), pix(0, 2));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.75f), pix(1, 2));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.75f), pix(2, 2));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.75f), pix(3, 2));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.375f), pix(4, 2));

  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.125), pix(0, 3));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.25f), pix(1, 3));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.25f), pix(2, 3));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.25f), pix(3, 3));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.125), pix(4, 3));

  EXPECT_EQ(Color::OFF_WHITE, pix(0, 4));
  EXPECT_EQ(Color::OFF_WHITE, pix(1, 4));
  EXPECT_EQ(Color::OFF_WHITE, pix(2, 4));
  EXPECT_EQ(Color::OFF_WHITE, pix(3, 4));
  EXPECT_EQ(Color::OFF_WHITE, pix(4, 4));
}

TEST_F(Draw2Test, VerticalExactLine) {
  // Draw a pure vertical line centered in the image.
  fill(Color::OFF_WHITE, canvas_.pixels());
  drawLine({2.5f, 0.5f}, {2.5f, 4.5f}, 1, Color::PURPLE_GRAY, canvas_.pixels());

  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.5f), pix(2, 0));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(2, 1));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(2, 2));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(2, 3));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.5f), pix(2, 4));

  EXPECT_EQ(Color::OFF_WHITE, pix(1, 0));
  EXPECT_EQ(Color::OFF_WHITE, pix(1, 1));
  EXPECT_EQ(Color::OFF_WHITE, pix(1, 2));
  EXPECT_EQ(Color::OFF_WHITE, pix(1, 3));
  EXPECT_EQ(Color::OFF_WHITE, pix(1, 4));

  EXPECT_EQ(Color::OFF_WHITE, pix(3, 0));
  EXPECT_EQ(Color::OFF_WHITE, pix(3, 1));
  EXPECT_EQ(Color::OFF_WHITE, pix(3, 2));
  EXPECT_EQ(Color::OFF_WHITE, pix(3, 3));
  EXPECT_EQ(Color::OFF_WHITE, pix(3, 4));
}

TEST_F(Draw2Test, Vertical5050Line) {
  // Draw a pure vertical line that is split 50%/50% between two pixels.
  fill(Color::OFF_WHITE, canvas_.pixels());
  drawLine({2.0f, 0.5f}, {2.0f, 4.5f}, 1, Color::PURPLE_GRAY, canvas_.pixels());

  EXPECT_EQ(Color::OFF_WHITE, pix(0, 0));
  EXPECT_EQ(Color::OFF_WHITE, pix(0, 1));
  EXPECT_EQ(Color::OFF_WHITE, pix(0, 2));
  EXPECT_EQ(Color::OFF_WHITE, pix(0, 3));
  EXPECT_EQ(Color::OFF_WHITE, pix(0, 4));

  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.25f), pix(1, 0));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.5f), pix(1, 1));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.5f), pix(1, 2));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.5f), pix(1, 3));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.25f), pix(1, 4));

  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.25f), pix(2, 0));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.5f), pix(2, 1));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.5f), pix(2, 2));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.5f), pix(2, 3));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.25f), pix(2, 4));

  EXPECT_EQ(Color::OFF_WHITE, pix(3, 0));
  EXPECT_EQ(Color::OFF_WHITE, pix(3, 1));
  EXPECT_EQ(Color::OFF_WHITE, pix(3, 2));
  EXPECT_EQ(Color::OFF_WHITE, pix(3, 3));
  EXPECT_EQ(Color::OFF_WHITE, pix(3, 4));
}

TEST_F(Draw2Test, Vertical2575Line) {
  // Draw a pure vertical line that is split 25%/75% between two pixels.
  fill(Color::OFF_WHITE, canvas_.pixels());
  drawLine({2.75f, 0.5f}, {2.75f, 4.5f}, 1, Color::PURPLE_GRAY, canvas_.pixels());

  EXPECT_EQ(Color::OFF_WHITE, pix(1, 0));
  EXPECT_EQ(Color::OFF_WHITE, pix(1, 1));
  EXPECT_EQ(Color::OFF_WHITE, pix(1, 2));
  EXPECT_EQ(Color::OFF_WHITE, pix(1, 3));
  EXPECT_EQ(Color::OFF_WHITE, pix(1, 4));

  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.375f), pix(2, 0));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.75f), pix(2, 1));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.75f), pix(2, 2));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.75f), pix(2, 3));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.375f), pix(2, 4));

  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.125f), pix(3, 0));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.25f), pix(3, 1));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.25f), pix(3, 2));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.25f), pix(3, 3));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.125f), pix(3, 4));

  EXPECT_EQ(Color::OFF_WHITE, pix(4, 0));
  EXPECT_EQ(Color::OFF_WHITE, pix(4, 1));
  EXPECT_EQ(Color::OFF_WHITE, pix(4, 2));
  EXPECT_EQ(Color::OFF_WHITE, pix(4, 3));
  EXPECT_EQ(Color::OFF_WHITE, pix(4, 4));
}

TEST_F(Draw2Test, FullHorizontalExactLine) {
  // Draw a pure full-width horizontal line centered in the image.
  fill(Color::OFF_WHITE, canvas_.pixels());
  drawLine({0.0f, 2.5f}, {5.0f, 2.5f}, 1, Color::PURPLE_GRAY, canvas_.pixels());

  EXPECT_EQ(Color::PURPLE_GRAY, pix(0, 2));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(1, 2));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(2, 2));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(3, 2));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(4, 2));
}

TEST_F(Draw2Test, FullVerticalExactLine) {
  // Draw a pure full-width horizontal line centered in the image.
  fill(Color::OFF_WHITE, canvas_.pixels());
  drawLine({2.5f, 0.0f}, {2.5f, 5.0f}, 1, Color::PURPLE_GRAY, canvas_.pixels());

  EXPECT_EQ(Color::PURPLE_GRAY, pix(2, 0));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(2, 1));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(2, 2));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(2, 3));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(2, 4));
}

TEST_F(Draw2Test, FullUpsideDownVerticalExactLine) {
  // Draw a pure full-width horizontal line centered in the image.
  fill(Color::OFF_WHITE, canvas_.pixels());
  drawLine({2.5f, 5.0f}, {2.5f, 0.0f}, 1, Color::PURPLE_GRAY, canvas_.pixels());

  EXPECT_EQ(Color::PURPLE_GRAY, pix(2, 0));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(2, 1));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(2, 2));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(2, 3));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(2, 4));
}

TEST_F(Draw2Test, DiagonalLine) {
  // Draw a perfect diagonal line.
  fill(Color::OFF_WHITE, canvas_.pixels());
  drawLine({0.5f, 0.5f}, {4.5f, 4.5f}, 1, Color::PURPLE_GRAY, canvas_.pixels());

  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.5f), pix(0, 0));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(1, 1));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(2, 2));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(3, 3));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.5f), pix(4, 4));
}

TEST_F(Draw2Test, FullDiagonalLine) {
  // Draw a corner to corner diagonal line.
  fill(Color::OFF_WHITE, canvas_.pixels());
  drawLine({0.0f, 0.0f}, {5.0f, 5.0f}, 1, Color::PURPLE_GRAY, canvas_.pixels());

  EXPECT_EQ(Color::PURPLE_GRAY, pix(0, 0));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(1, 1));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(2, 2));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(3, 3));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(4, 4));
}

TEST_F(Draw2Test, FullBackwardsDiagonalLine) {
  // Draw a corner to corner diagonal line.
  fill(Color::OFF_WHITE, canvas_.pixels());
  drawLine({5.0f, 5.0f}, {0.0f, 0.0f}, 1, Color::PURPLE_GRAY, canvas_.pixels());

  EXPECT_EQ(Color::PURPLE_GRAY, pix(4, 4));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(3, 3));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(2, 2));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(1, 1));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(0, 0));
}

TEST_F(Draw2Test, FullUpperDiagonalLine) {
  // Draw a corner to corner diagonal line.
  fill(Color::OFF_WHITE, canvas_.pixels());
  drawLine({0.0f, 5.0f}, {5.0f, 0.0f}, 1, Color::PURPLE_GRAY, canvas_.pixels());

  EXPECT_EQ(Color::PURPLE_GRAY, pix(0, 4));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(1, 3));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(2, 2));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(3, 1));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(4, 0));
}

TEST_F(Draw2Test, FullBackwardsUpperDiagonalLine) {
  // Draw a corner to corner diagonal line.
  fill(Color::OFF_WHITE, canvas_.pixels());
  drawLine({5.0f, 0.0f}, {0.0f, 5.0f}, 1, Color::PURPLE_GRAY, canvas_.pixels());

  EXPECT_EQ(Color::PURPLE_GRAY, pix(0, 4));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(1, 3));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(2, 2));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(3, 1));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(4, 0));
}

TEST_F(Draw2Test, Diagonal8020Line) {
  // Draw a diagonal line that is split 80%/20% between two pixels.
  fill(Color::OFF_WHITE, canvas_.pixels());
  drawLine({0.0f, -0.2f}, {5.0f, 4.8f}, 1, Color::PURPLE_GRAY, canvas_.pixels());

  //EXPECT_EQ(Color::OFF_WHITE, pix(2, 0));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.20f), pix(2, 1));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.80f), pix(2, 2));
  //EXPECT_EQ(Color::OFF_WHITE, pix(2, 3));
}

TEST_F(Draw2Test, SteepDiagonal8020Line) {
  // Draw a steep diagonal line that is split 80%/20% between two pixels.
  fill(Color::OFF_WHITE, canvas_.pixels());
  drawLine({1.45f, 0.0f}, {3.95f, 5.0f}, 1, Color::PURPLE_GRAY, canvas_.pixels());

  EXPECT_EQ(Color::OFF_WHITE, pix(1, 2));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.80f), pix(2, 2));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.20f), pix(3, 2));
  EXPECT_EQ(Color::OFF_WHITE, pix(4, 2));
}

TEST_F(Draw2Test, NotSteepDiagonal8020Line) {
  // Draw a not-steep diagonal line that is split 80%/20% between two pixels.
  fill(Color::OFF_WHITE, canvas_.pixels());
  drawLine({-2.5f, -0.2f}, {7.5f, 4.8f}, 1, Color::PURPLE_GRAY, canvas_.pixels());

  EXPECT_EQ(Color::OFF_WHITE, pix(2, 0));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.20f), pix(2, 1));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.80f), pix(2, 2));
  EXPECT_EQ(Color::OFF_WHITE, pix(2, 3));
}

TEST_F(Draw2Test, CleanFilledRectangle) {
  // Draw a pixel-exact rectangle centered in the image.
  fill(Color::OFF_WHITE, canvas_.pixels());
  drawRectangle({1.0f, 2.0f}, {4.0f, 4.0f}, Color::PURPLE_GRAY, canvas_.pixels());

  EXPECT_EQ(Color::OFF_WHITE, pix(0, 0));
  EXPECT_EQ(Color::OFF_WHITE, pix(1, 0));
  EXPECT_EQ(Color::OFF_WHITE, pix(2, 0));
  EXPECT_EQ(Color::OFF_WHITE, pix(3, 0));
  EXPECT_EQ(Color::OFF_WHITE, pix(4, 0));

  EXPECT_EQ(Color::OFF_WHITE, pix(0, 1));
  EXPECT_EQ(Color::OFF_WHITE, pix(1, 1));
  EXPECT_EQ(Color::OFF_WHITE, pix(2, 1));
  EXPECT_EQ(Color::OFF_WHITE, pix(3, 1));
  EXPECT_EQ(Color::OFF_WHITE, pix(4, 1));

  EXPECT_EQ(Color::OFF_WHITE, pix(0, 2));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(1, 2));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(2, 2));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(3, 2));
  EXPECT_EQ(Color::OFF_WHITE, pix(4, 2));

  EXPECT_EQ(Color::OFF_WHITE, pix(0, 3));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(1, 3));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(2, 3));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(3, 3));
  EXPECT_EQ(Color::OFF_WHITE, pix(4, 3));

  EXPECT_EQ(Color::OFF_WHITE, pix(0, 4));
  EXPECT_EQ(Color::OFF_WHITE, pix(1, 4));
  EXPECT_EQ(Color::OFF_WHITE, pix(2, 4));
  EXPECT_EQ(Color::OFF_WHITE, pix(3, 4));
  EXPECT_EQ(Color::OFF_WHITE, pix(4, 4));
}

TEST_F(Draw2Test, OffsetFilledRectangle) {
  // Draw an offset rectangle in the image.
  fill(Color::OFF_WHITE, canvas_.pixels());
  drawRectangle({1.25f, 2.5f}, {4.25f, 4.5f}, Color::PURPLE_GRAY, canvas_.pixels());

  EXPECT_EQ(Color::OFF_WHITE, pix(0, 0));
  EXPECT_EQ(Color::OFF_WHITE, pix(1, 0));
  EXPECT_EQ(Color::OFF_WHITE, pix(2, 0));
  EXPECT_EQ(Color::OFF_WHITE, pix(3, 0));
  EXPECT_EQ(Color::OFF_WHITE, pix(4, 0));

  EXPECT_EQ(Color::OFF_WHITE, pix(0, 1));
  EXPECT_EQ(Color::OFF_WHITE, pix(1, 1));
  EXPECT_EQ(Color::OFF_WHITE, pix(2, 1));
  EXPECT_EQ(Color::OFF_WHITE, pix(3, 1));
  EXPECT_EQ(Color::OFF_WHITE, pix(4, 1));

  EXPECT_EQ(Color::OFF_WHITE, pix(0, 2));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.375f), pix(1, 2));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.5f), pix(2, 2));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.5f), pix(3, 2));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.125f), pix(4, 2));

  EXPECT_EQ(Color::OFF_WHITE, pix(0, 3));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.75f), pix(1, 3));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(2, 3));
  EXPECT_EQ(Color::PURPLE_GRAY, pix(3, 3));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.25f), pix(4, 3));

  EXPECT_EQ(Color::OFF_WHITE, pix(0, 4));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.375f), pix(1, 4));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.5f), pix(2, 4));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.5f), pix(3, 4));
  EXPECT_EQ(mixSRGB(Color::PURPLE_GRAY, Color::OFF_WHITE, 0.125f), pix(4, 4));
}

}  // namespace c8
