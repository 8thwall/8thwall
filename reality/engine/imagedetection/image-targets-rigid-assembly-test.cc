// Copyright (c) 2019 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":image-targets-rigid-assembly",
    "//c8:hmatrix",
    "//c8/geometry:intrinsics",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x78354f7b);

#include "reality/engine/imagedetection/image-targets-rigid-assembly.h"

#include "c8/geometry/intrinsics.h"
#include "c8/hmatrix.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using testing::Eq;
using testing::FloatNear;
using testing::Pointwise;

namespace c8 {

MATCHER_P(AreWithin, eps, "") { return fabs(testing::get<0>(arg) - testing::get<1>(arg)) < eps; }

decltype(auto) equalsMatrix(const HMatrix &matrix, float threshold = 1e-6) {
  return Pointwise(AreWithin(threshold), matrix.data());
}

class ImageTagetsRigidAssemblyTest : public ::testing::Test {};

// Get a consistent test intrinsic matrix.
static c8_PixelPinholeCameraModel testK() {
  return Intrinsics::getCameraIntrinsics(DeviceInfos::GOOGLE_PIXEL3);
}

TrackedImage TI(int index, const HMatrix &pose) {
  return TrackedImage{ TrackedImage::Status::TRACKED, index, "", pose, HMatrixGen::i(), 1.0f, 0, 0, testK(), testK() };
}

TEST_F(ImageTagetsRigidAssemblyTest, TestInit) {
  ImageTargetsRigidAssembly ra;
  EXPECT_EQ(ra.valid(), false);
  EXPECT_EQ(ra.valid(10), false);
  EXPECT_THAT(ra.worldPose().data(), equalsMatrix(HMatrixGen::i()));

  Vector<TrackedImage> targets;
  ra.observe(targets);
  EXPECT_EQ(ra.valid(), false);
  EXPECT_EQ(ra.valid(10), false);
  EXPECT_THAT(ra.worldPose().data(), equalsMatrix(HMatrixGen::i()));
}

TEST_F(ImageTagetsRigidAssemblyTest, TestObserve) {
  ImageTargetsRigidAssembly ra;
  Vector<TrackedImage> targets;
  ra.observe(targets);
  targets.emplace_back(TI(5, HMatrixGen::translation(1.0f, 5.0f, 0.0f)));

  ra.observe(targets);
  EXPECT_EQ(ra.valid(), true);
  EXPECT_EQ(ra.valid(3), false);
  EXPECT_EQ(ra.valid(5), true);
  EXPECT_THAT(ra.worldPose().data(), equalsMatrix(targets.at(0).pose));

  // Move
  targets.at(0).pose = HMatrixGen::translation(1.0f, 5.0f, 0.5f);
  ra.observe(targets);
  EXPECT_THAT(ra.worldPose().data(), equalsMatrix(targets.at(0).pose));

  // Observe second
  targets.emplace(targets.begin(), TI(3, HMatrixGen::translation(2.0f, 5.0f, 0.5f)));
  ra.observe(targets);
  EXPECT_THAT(ra.worldPose().data(), equalsMatrix(targets.at(1).pose));
  EXPECT_THAT(ra.transform(5).data(), equalsMatrix(HMatrixGen::i()));
  EXPECT_THAT(ra.transform(3).data(), equalsMatrix(HMatrixGen::translation(-1.0f, 0.0f, 0.0f)));

  // First is no longer observable.
  // Move the second
  targets.at(1).status = TrackedImage::Status::NOT_FOUND;
  targets.at(0).pose = targets.at(0).pose.translate(0.0f, 0.0f, -0.5f);
  ra.observe(targets);
  EXPECT_EQ(ra.valid(), true);
  EXPECT_EQ(ra.valid(3), true);
  EXPECT_EQ(ra.valid(5), false);
  EXPECT_THAT(ra.worldPose().data(), equalsMatrix(targets.at(1).pose.translate(0.0f, 0.0f, -0.5f)));
  EXPECT_THAT(ra.transform(5).data(), equalsMatrix(HMatrixGen::i()));
  EXPECT_THAT(ra.transform(3).data(), equalsMatrix(HMatrixGen::translation(-1.0f, 0.0f, 0.0f)));

  // Loose the second
  targets.at(0).status = TrackedImage::Status::NOT_FOUND;
  ra.observe(targets);
  EXPECT_EQ(ra.valid(), false); // none observable
  EXPECT_EQ(ra.valid(3), false);
  EXPECT_EQ(ra.valid(5), false);

  // Observe a third
  targets.emplace(targets.begin(), TI(1, HMatrixGen::translation(3.0f, 5.0f, 0.25f)));
  ra.observe(targets);
  EXPECT_EQ(ra.valid(), true);
  EXPECT_EQ(ra.valid(1), true);
  EXPECT_THAT(ra.worldPose().data(), equalsMatrix(targets.at(0).pose));

  // Regain the first two, now relative to index=1 (at the end)
  std::reverse(targets.begin(), targets.end());
  targets.at(2).pose = targets.at(0).pose.translate(0.0f, 0.0f, -0.25f);
  targets.at(1).status = TrackedImage::Status::TRACKED;
  targets.at(0).status = TrackedImage::Status::TRACKED;
  ra.observe(targets);
  EXPECT_EQ(ra.valid(), true);
  EXPECT_EQ(ra.valid(1), true);
  EXPECT_EQ(ra.valid(3), true);
  EXPECT_EQ(ra.valid(5), true);
  EXPECT_THAT(ra.worldPose().data(), equalsMatrix(targets.at(2).pose));
}

TEST_F(ImageTagetsRigidAssemblyTest, TestScale) {
  ImageTargetsRigidAssembly ra;
  Vector<TrackedImage> targets;
  ra.observe(targets);
  targets.emplace_back(TI(3, HMatrixGen::translation(1.0f, 5.0f, 0.0f)));
  targets.emplace_back(TI(5, HMatrixGen::translation(2.0f, 5.0f, 0.0f)));

  ra.observe(targets);
  EXPECT_EQ(ra.valid(), true);
  EXPECT_EQ(ra.valid(3), true);
  EXPECT_EQ(ra.valid(5), true);
  EXPECT_THAT(ra.worldPose().data(), equalsMatrix(targets.at(0).pose));
  EXPECT_THAT(ra.transform(3).data(), equalsMatrix(HMatrixGen::i()));
  EXPECT_THAT(ra.transform(5).data(), equalsMatrix(HMatrixGen::translation(-1.0f, 0.0f, 0.0f)));

  targets.at(0).pose = targets.at(0).pose.translate(0.0f, 0.0f, 1.0f);
  targets.at(1).pose = targets.at(1).pose.translate(0.0f, 0.0f, 0.25f);
  ra.observe(targets);
  EXPECT_THAT(ra.worldPose().data(), equalsMatrix(targets.at(0).pose));
  EXPECT_THAT(ra.transform(3).data(), equalsMatrix(HMatrixGen::i()));

  // TODO(scott) get relative scale working so that the following remains true
  // EXPECT_THAT(ra.transform(5).data(), equalsMatrix(HMatrixGen::translation(-1.0f, 0.0f, 0.0f)));
}

}  // namespace c8
