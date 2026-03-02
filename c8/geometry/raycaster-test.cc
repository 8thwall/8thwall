// Copyright (c) 2022 8th Wall, LLC.
// Original Author: Nathan Waters (nathanwaters@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":raycaster",
    "//c8/geometry:egomotion",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x6f2d3234);

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "c8/geometry/egomotion.h"
#include "c8/geometry/raycaster.h"

namespace c8 {

class RaycasterTests : public ::testing::Test {
protected:
  void SetUp() override {
    mesh_ = {
      {
        {-1.0f, -1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {1.0f, -1.0f, 0.0f},
        // Another triangle behind the first triangle
        {-1.0f, -1.0f, 2.0f},
        {2.0f, 1.0f, 2.0f},
        {1.0f, -1.0f, 2.0f},
      },
      {},
      {{0, 1, 2}, {3, 4, 5}}};

    // Move the mesh in front of the camera.
    meshTransformInWorld_ = HMatrixGen::translateZ(3.f);
  }

  // The ground truth of the 100 points in world space.
  MeshGeometry mesh_;
  float zDist_ = 3.0f;
  HMatrix meshTransformInWorld_ = HMatrixGen::i();
};

using testing::Pointwise;

MATCHER_P(AreWithin, eps, "") { return fabs(testing::get<0>(arg) - testing::get<1>(arg)) < eps; }

decltype(auto) equalsHVector(const HVector3 &vec) {
  return Pointwise(AreWithin(0.0001), vec.data());
}

TEST_F(RaycasterTests, TestRayPlaneIntersection) {
  // Hit test at the center, we expect to find the point.
  HPoint3 intersectionInWorld;
  HVector3 rayOrigin = {0.f, 0.f, 1.f};
  HVector3 rayVector0 = {1.f, 1.f, -1.f};
  HVector3 planeNormal0 = {0.0f, 0.0f, 1.0f};
  float planeD0 = 0.0f;
  auto hasIntersection =
    rayPlaneIntersection(rayOrigin, rayVector0, planeNormal0, planeD0, &intersectionInWorld);
  EXPECT_TRUE(hasIntersection);
  EXPECT_THAT(intersectionInWorld.data(), equalsHVector({1.f, 1.f, 0.0f}));

  HVector3 rayVector1 = {0.f, 0.f, -1.f};
  HVector3 planeNormal1 = {0.0f, 1.0f, 0.0f};
  float planeD1 = 1.0f;
  hasIntersection =
    rayPlaneIntersection(rayOrigin, rayVector1, planeNormal1, planeD1, &intersectionInWorld);
  EXPECT_FALSE(hasIntersection);
}

}  // namespace c8
