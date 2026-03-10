// Copyright (c) 2023 Niantic Labs
// Original Author: Yuyan Song (yuyansong@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":ear-types",
    "//c8:string",
    "@com_google_googletest//:gtest_main",
  };
  linkstatic = 1;
}
cc_end(0x60685123);

#include <cmath>
#include <cstdio>
#include <fstream>
#include <iostream>

#include "c8/stats/scope-timer.h"
#include "c8/string.h"
#include "gtest/gtest.h"
#include "reality/engine/ears/ear-types.h"

namespace c8 {

class EarTypesTest : public ::testing::Test {};

HPoint3 random0To1Point() {
  float x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
  float y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
  float z = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
  return {x, y, z};
}

TEST_F(EarTypesTest, TestMirrorInvisibleEarVertices) {
  Ear3d ear3d;
  HPoint3 leftRefPt = random0To1Point();
  HPoint3 rightRefPt = random0To1Point();
  ear3d.leftVertices.clear();
  ear3d.leftVisibilities.clear();
  ear3d.rightVertices.clear();
  ear3d.rightVisibilities.clear();
  for (size_t i = 0; i < 3; ++i) {
    ear3d.leftVertices.push_back(random0To1Point());
    ear3d.rightVertices.push_back(random0To1Point());
    ear3d.leftVisibilities.push_back(0.6f);
    ear3d.rightVisibilities.push_back(0.2f);
  }
  // make left ear vertices visible and right ear vertices invisible
  constexpr float delta = 0.00001f;
  mirrorInvisibleEarVertices(leftRefPt, rightRefPt, &ear3d);
  for (size_t i = 0; i < ear3d.leftVertices.size(); ++i) {
    EXPECT_NEAR(
      ear3d.leftVertices[i].x() - leftRefPt.x(), rightRefPt.x() - ear3d.rightVertices[i].x(), delta);
    EXPECT_NEAR(
      ear3d.leftVertices[i].y() - leftRefPt.y(), ear3d.rightVertices[i].y() - rightRefPt.y(), delta);
    EXPECT_NEAR(
      ear3d.leftVertices[i].z() - leftRefPt.z(), ear3d.rightVertices[i].z() - rightRefPt.z(), delta);
  }
}

}  // namespace c8
