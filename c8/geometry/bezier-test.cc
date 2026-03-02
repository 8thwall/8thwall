// Copyright (c) 2024 Niantic, Inc.
// Original Author: Nicholas Butko (nbutko@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":bezier",
    "@com_google_googletest//:gtest_main",
  };
  linkstatic = 1;
}
cc_end(0x40002892);

#include <gtest/gtest.h>

#include "c8/c8-log.h"
#include "c8/geometry/bezier.h"

namespace c8 {

namespace {

std::array<float, 2> parametricTimeValue(AnimationCurve curve, float t) {
  // See https://en.wikipedia.org/wiki/B%C3%A9zier_curve
  float u = (1.0f - t);
  float tt = t * t;
  float uu = u * u;
  float uuu = uu * u;
  float ttt = tt * t;
  auto bt = curve[0].time;
  auto ct = curve[1].time;
  auto bv = curve[0].value;
  auto cv = curve[1].value;
  return {
    uuu * 0.0f + 3 * uu * t * bt + 3 * u * tt * ct + ttt * 1.0f,
    uuu * 0.0f + 3 * uu * t * bv + 3 * u * tt * cv + ttt * 1.0f,
  };
}

}  // namespace

class BezierTest : public ::testing::Test {};

TEST_F(BezierTest, TestDefault) {
  auto animation = BezierAnimation::defaultCurve();

  for (int i = 0; i < 101; i++) {
    auto [time, value] = parametricTimeValue(BezierAnimation::DEFAULT_CURVE, i / 100.0f);
    EXPECT_NEAR(animation.at(time), value, 3e-7f);
  }
}

TEST_F(BezierTest, TestEaseIn) {
  auto animation = BezierAnimation::easeIn();

  for (int i = 0; i < 101; i++) {
    auto [time, value] = parametricTimeValue(BezierAnimation::EASE_IN_CURVE, i / 100.0f);
    EXPECT_NEAR(animation.at(time), value, 3e-7f);
  }
}

TEST_F(BezierTest, TestEaseInEaseOut) {
  auto animation = BezierAnimation::easeInEaseOut();

  for (int i = 0; i < 101; i++) {
    auto [time, value] = parametricTimeValue(BezierAnimation::EASE_IN_EASE_OUT_CURVE, i / 100.0f);
    EXPECT_NEAR(animation.at(time), value, 3e-7f);
  }
}

TEST_F(BezierTest, TestEaseOut) {
  auto animation = BezierAnimation::easeOut();

  for (int i = 0; i < 101; i++) {
    auto [time, value] = parametricTimeValue(BezierAnimation::EASE_OUT_CURVE, i / 100.0f);
    EXPECT_NEAR(animation.at(time), value, 3e-7f);
  }
}

TEST_F(BezierTest, TestStrongEase) {
  auto animation = BezierAnimation::strongEase();

  for (int i = 0; i < 101; i++) {
    auto [time, value] = parametricTimeValue(BezierAnimation::STRONG_EASE_CURVE, i / 100.0f);
    EXPECT_NEAR(animation.at(time), value, 3e-7f);
  }
}

TEST_F(BezierTest, TestLinear) {
  auto animation = BezierAnimation::linear();

  for (int i = 0; i < 101; i++) {
    auto [time, value] = parametricTimeValue(BezierAnimation::LINEAR_CURVE, i / 100.0f);
    EXPECT_NEAR(animation.at(time), value, 3e-7f);
    // Additional check for linear -- time should equal value.
    EXPECT_NEAR(time, value, 3e-7f);
  }
}

TEST_F(BezierTest, TestGrid) {
  // Test a grid of animation curves.
  for (int i = 0; i < 10; i++) {            // 0.0 to 0.9
    for (int j = i + 1; j < 11; ++j) {      // i + .1 to 1.0
      for (int k = 0; k < 10; ++k) {        // 0.0 to 0.9
        for (int l = k + 1; l < 11; ++l) {  // k + .1 to 1.0
          auto curve = AnimationCurve{{{i / 10.0f, k / 10.0f}, {j / 10.0f, l / 10.0f}}};
          auto animation = BezierAnimation{curve};
          for (int t = 0; t < 101; t++) {
            auto [time, value] = parametricTimeValue(curve, t / 100.0f);
            EXPECT_NEAR(animation.at(time), value, 3e-5f);
          }
        }
      }
    }
  }
}

TEST_F(BezierTest, TestAnimation) {
  AnimationState<float, 2> state;

  // Set value (no animation).
  state.set({1.0f, 1.0f});
  EXPECT_TRUE(state.tick(1));   // We just set the value, so there is an updated value.
  EXPECT_FALSE(state.tick(2));  // No change, so no update.
  EXPECT_EQ(1.0f, state.get()[0]);
  EXPECT_EQ(1.0f, state.get()[1]);

  // Animation aligned to clock.
  state.animate({2.0f, 3.0f}, BezierAnimation::linear(), 2.0);
  EXPECT_TRUE(state.tick(3));  // We are animating.
  EXPECT_EQ(1.5f, state.get()[0]);
  EXPECT_EQ(2.0f, state.get()[1]);
  EXPECT_TRUE(state.tick(4));  // We are animating.
  EXPECT_EQ(2.0f, state.get()[0]);
  EXPECT_EQ(3.0f, state.get()[1]);
  EXPECT_FALSE(state.tick(5));  // Animation finished.
  EXPECT_EQ(2.0f, state.get()[0]);
  EXPECT_EQ(3.0f, state.get()[1]);

  // Animation not aligned to clock.
  state.animate({3.0f, 4.0f}, BezierAnimation::linear(), 1.0);
  EXPECT_TRUE(state.tick(5.4));  // We are animating.
  EXPECT_EQ(2.4f, state.get()[0]);
  EXPECT_EQ(3.4f, state.get()[1]);
  EXPECT_TRUE(state.tick(5.8));  // We are animating.
  EXPECT_EQ(2.8f, state.get()[0]);
  EXPECT_EQ(3.8f, state.get()[1]);
  EXPECT_TRUE(state.tick(6.2));  // Animation finished halfway through the period.
  EXPECT_EQ(3.0f, state.get()[0]);
  EXPECT_EQ(4.0f, state.get()[1]);
  EXPECT_FALSE(state.tick(7));  // Animation finished.
  EXPECT_EQ(3.0f, state.get()[0]);
  EXPECT_EQ(4.0f, state.get()[1]);

  // Set during animation.
  state.animate({8.0f, 9.0f}, BezierAnimation::linear(), 5.0);
  EXPECT_TRUE(state.tick(8));  // We are animating.
  EXPECT_EQ(4.0f, state.get()[0]);
  EXPECT_EQ(5.0f, state.get()[1]);
  EXPECT_TRUE(state.tick(9));  // We are animating.
  EXPECT_EQ(5.0f, state.get()[0]);
  EXPECT_EQ(6.0f, state.get()[1]);
  state.set({2.0f, 3.0f});
  EXPECT_TRUE(state.tick(10));  // We just set, so animation stopped but there is an update.
  EXPECT_EQ(2.0f, state.get()[0]);
  EXPECT_EQ(3.0f, state.get()[1]);
  EXPECT_FALSE(state.tick(11));  // No more animation, no more updates.
  EXPECT_EQ(2.0f, state.get()[0]);
  EXPECT_EQ(3.0f, state.get()[1]);
}

}  // namespace c8
