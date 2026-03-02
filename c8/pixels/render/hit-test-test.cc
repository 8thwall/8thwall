// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":hit-test",
    "//c8/geometry:intrinsics",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xbe5a8d50);

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "c8/geometry/intrinsics.h"
#include "c8/pixels/render/hit-test.h"

namespace c8 {

class HitTestTest : public ::testing::Test {};

TEST_F(HitTestTest, TestMesh) {
  // Create a scene with a cube in the center and a backquad.
  auto scene = ObGen::scene(640, 480);

  scene->add(ObGen::perspectiveCamera(
    Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_XS), 640, 480));

  scene->add(ObGen::named(
    ObGen::positioned(ObGen::cubeMesh(), HMatrixGen::translation(0.0f, 0.0f, 3.0f)), "cube"));
  scene->add(ObGen::named(ObGen::backQuad(), "backquad"));

  // Hit test near the center, we expect to hit two faces of the cube and then the backquad.
  auto hits = hitTestPixel(*scene, {322.0f, 242.0f});
  EXPECT_EQ(3, hits.size());
  EXPECT_EQ("cube", hits[0].target->name());
  EXPECT_EQ("cube", hits[1].target->name());
  EXPECT_EQ("backquad", hits[2].target->name());

  // Hit test near the top, we expect to miss the cube but hit the backquad.
  hits = hitTestPixel(*scene, {0.0f, 242.0f});
  EXPECT_EQ(1, hits.size());
  EXPECT_EQ("backquad", hits[0].target->name());

  // Disable the backquad and try again. We expect the backquad to be missing from hits this time.
  scene->find<Renderable>("backquad").setEnabled(false);
  hits = hitTestPixel(*scene, {322.0f, 242.0f});
  EXPECT_EQ(2, hits.size());
  EXPECT_EQ("cube", hits[0].target->name());
  EXPECT_EQ("cube", hits[1].target->name());

  hits = hitTestPixel(*scene, {0.0f, 242.0f});
  EXPECT_EQ(0, hits.size());
}

TEST_F(HitTestTest, TestSubscene) {
  // Add wide scene with a subscene on the left. The subscene has a backquad and a central cube.
  auto scene = ObGen::scene(1280, 480);

  auto &subscene = scene->add(ObGen::subScene("subscene", {{640, 480}}));
  subscene.add(ObGen::perspectiveCamera(
    Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_XS), 640, 480));

  subscene.add(ObGen::named(
    ObGen::positioned(ObGen::cubeMesh(), HMatrixGen::translation(0.0f, 0.0f, 3.0f)), "cube"));
  subscene.add(ObGen::named(ObGen::backQuad(), "backquad"));

  scene->add(ObGen::pixelCamera(1280, 480));

  scene->add(ObGen::named(ObGen::pixelQuad(0, 0, 640, 480), "subscene-render"))
    .setMaterial(MatGen::subsceneMaterial("subscene"));

  // Test the center of the subscene (left of center in the full scene). We expect to hit two faces
  // of the cube, and the backquad and the quad that is rendering the scene.
  auto hits = hitTestPixel(*scene, {322.0f, 242.0f});
  EXPECT_EQ(4, hits.size());
  EXPECT_EQ("cube", hits[0].target->name());
  EXPECT_EQ("cube", hits[1].target->name());
  EXPECT_EQ("backquad", hits[2].target->name());
  EXPECT_EQ("subscene-render", hits[3].target->name());

  // Test the top of the subscene. We expect to hit the backquad and the quad that is rendering the
  // scene.
  hits = hitTestPixel(*scene, {0.0f, 242.0f});
  EXPECT_EQ(2, hits.size());
  EXPECT_EQ("backquad", hits[0].target->name());
  EXPECT_EQ("subscene-render", hits[1].target->name());
}

TEST_F(HitTestTest, TestPointCloud) {
  // Create a scene with a point cloud with a single point in the middle of the field of view.
  auto scene = ObGen::scene(640, 480);

  scene->add(ObGen::perspectiveCamera(
    Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_XS), 640, 480));

  scene->add(ObGen::named(ObGen::pointCloud({{0.0f, 0.0f, 3.0f}}, 0.05f), "point"));

  // Hit test near the center, we expect to find the point.
  auto hits = hitTestPixel(*scene, {322.0f, 242.0f});
  EXPECT_EQ(1, hits.size());
  EXPECT_EQ("point", hits[0].target->name());

  // Hit test near the top, we expect no point.
  hits = hitTestPixel(*scene, {0.0f, 242.0f});
  EXPECT_EQ(0, hits.size());
}

}  // namespace c8
