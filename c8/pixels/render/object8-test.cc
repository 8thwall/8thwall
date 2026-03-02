// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":object8",
    "//c8:c8-log",
    "//c8:hpoint",
    "//c8/geometry:egomotion",
    "//c8/geometry:intrinsics",
    "@com_google_googletest//:gtest_main",
    "@json//:json",
  };
}
cc_end(0xf9477035);

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <nlohmann/json.hpp>

#include "c8/c8-log.h"
#include "c8/hpoint.h"
#include "c8/geometry/egomotion.h"
#include "c8/geometry/intrinsics.h"
#include "c8/pixels/render/object8.h"

using testing::Eq;
using testing::FloatNear;
using testing::Pointwise;

namespace c8 {

MATCHER_P(AreWithin, eps, "") { return fabs(testing::get<0>(arg) - testing::get<1>(arg)) < eps; }

decltype(auto) equalsMatrix(const HMatrix &matrix, float threshold = 1e-6) {
  return Pointwise(AreWithin(threshold), matrix.data());
}

decltype(auto) equalsPoint(const HPoint3 &pt, float threshold = 1e-6) {
  return Pointwise(AreWithin(threshold), pt.data());
}

class Object8Test : public ::testing::Test {};

TEST_F(Object8Test, TestIsAs) {
  auto scene = ObGen::scene(640, 480);
  auto &camera = scene->add(ObGen::camera());
  auto &light = camera.add(ObGen::named(ObGen::light(), "light")).setIntensity(0.75f);
  auto &group = scene->add(ObGen::group());
  const auto &renderable =
    group.add(ObGen::named(ObGen::renderable(), "mesh")).setKind(Renderable::MESH);

  EXPECT_TRUE(scene->is<Scene>());
  EXPECT_TRUE(camera.is<Camera>());
  EXPECT_TRUE(light.is<Light>());
  EXPECT_TRUE(group.is<Group>());
  EXPECT_TRUE(renderable.is<Renderable>());

  EXPECT_FLOAT_EQ(light.intensity(), 0.75f);
  EXPECT_EQ(renderable.kind(), Renderable::MESH);
  EXPECT_THAT(camera.projection().data(), equalsMatrix(HMatrixGen::i()));

  EXPECT_FLOAT_EQ(scene->find<Light>("light").intensity(), 0.75f);
  EXPECT_EQ(scene->find<Renderable>("mesh").kind(), Renderable::MESH);
  EXPECT_FALSE(scene->has<Camera>("camera"));
  EXPECT_TRUE(scene->find<Camera>().is<Camera>());

  EXPECT_TRUE(scene->has<Scene>());
  EXPECT_TRUE(scene->has<Camera>());
  EXPECT_TRUE(scene->has<Light>());
  EXPECT_TRUE(camera.has<Camera>());
  EXPECT_TRUE(camera.has<Light>());
  EXPECT_TRUE(light.has<Light>());
  EXPECT_FALSE(light.has<Camera>());
}

TEST_F(Object8Test, TestLocalWorld) {
  auto scene = ObGen::scene(640, 480);
  // Node 1 is at 0, 0, 1, facing right.
  auto &node1 = scene->add(ObGen::positioned(
    ObGen::renderable(), cameraMotion(HMatrixGen::translateZ(1.0f), HMatrixGen::yDegrees(90))));

  // Node 2 is at 0, 0, 1, facing left.
  auto &node2 = scene->add(ObGen::positioned(
    ObGen::renderable(), cameraMotion(HMatrixGen::translateZ(1.0f), HMatrixGen::yDegrees(-90))));

  // Node 3 is at 0, 0, 1 in its parent.
  auto &node3 = node1.add(ObGen::positioned(ObGen::renderable(), HMatrixGen::translateZ(1.0f)));

  // In node1 local space, a node3 origin is at 0, 0, 1, in global space, it's at 1, 0, 1.
  HPoint3 origin(0.0f, 0.0f, 0.0f);
  EXPECT_THAT((node3.local() * origin).data(), equalsPoint(HPoint3{0.0f, 0.0f, 1.0f}));
  EXPECT_THAT((node3.world() * origin).data(), equalsPoint(HPoint3{1.0f, 0.0f, 1.0f}));

  // In node2 local space, a node3 origin is at 0, 0, 1, in global space, it's at -1, 0, 1.
  node2.add(node3.remove());
  EXPECT_THAT((node3.local() * origin).data(), equalsPoint(HPoint3{0.0f, 0.0f, 1.0f}));
  EXPECT_THAT((node3.world() * origin).data(), equalsPoint(HPoint3{-1.0f, 0.0f, 1.0f}));

  // Move node2 up 1. Now node3 local origin is at 0, 1, 1, in global space, it's at -1, 1, 1.
  node2.setLocal(
    cameraMotion(HMatrixGen::translation(0.0f, 1.0f, 1.0f), HMatrixGen::yDegrees(-90)));
  EXPECT_THAT((node3.local() * origin).data(), equalsPoint(HPoint3{0.0f, 0.0f, 1.0f}));
  EXPECT_THAT((node3.world() * origin).data(), equalsPoint(HPoint3{-1.0f, 1.0f, 1.0f}));

  // Move node3 under the scene. It's local and world origin are at 0, 0, 1.
  scene->add(node3.remove());
  EXPECT_THAT((node3.local() * origin).data(), equalsPoint(HPoint3{0.0f, 0.0f, 1.0f}));
  EXPECT_THAT((node3.world() * origin).data(), equalsPoint(HPoint3{0.0f, 0.0f, 1.0f}));
}

TEST_F(Object8Test, TestRenderableGeometry) {

  auto scene = ObGen::scene(640, 480);

  auto &quad = scene->add(ObGen::quad());

  quad.geometry().setColor(Color::PURPLE);

  // Check basic stats.
  EXPECT_EQ(Renderable::MESH, quad.kind());
  EXPECT_EQ(4, quad.geometry().vertices().size());
  EXPECT_EQ(2, quad.geometry().triangles().size());
  EXPECT_EQ(4, quad.geometry().colors().size());
}

TEST_F(Object8Test, TestSubScenes) {
  auto scene = ObGen::scene(640, 480);

  // camera and sub-scene for left side
  int lWidth = 640;
  int lHeight = 480;
  scene->add(ObGen::named(
    ObGen::perspectiveCamera(
      Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_XS), lWidth, lHeight),
    "c1"));
  auto &lSubscene = scene->add(ObGen::subScene("leftScene", {{lWidth, lHeight, "r1", "c1"}}));
  EXPECT_TRUE(lSubscene.is<Scene>());

  // camera and sub-scene for right side
  int rWidth = 640;
  int rHeight = 480;
  scene->add(ObGen::named(
    ObGen::perspectiveCamera(
      Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_XS), rWidth, rHeight),
    "c2"));
  auto &rSubscene = scene->add(ObGen::subScene("rightScene", {{rWidth, rHeight, "r2", "c2"}}));
  EXPECT_TRUE(rSubscene.is<Scene>());
}

TEST_F(Object8Test, TestPerspectiveCamera) {
  auto scene = ObGen::scene(640, 480);

  auto &quad = scene->add(ObGen::quad());

  quad.setLocal(HMatrixGen::translation(-.5f, 0.25f, 4.0f));
  quad.geometry().setColors({
    Color::PURPLE,
    Color::CHERRY,
    Color::MINT,
    Color::MANGO,
  });

  auto &camera = scene->add(ObGen::perspectiveCamera(
    Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_XS), 640, 480));

  auto mvp = camera.projection() * camera.world().inv() * quad.world();
  auto pts = mvp * quad.geometry().vertices();

  EXPECT_EQ(4, pts.size());

  // All points on the quad should be in clip space.
  for (int i = 0; i < pts.size(); ++i) {
    EXPECT_GE(pts[i].x(), -1);
    EXPECT_GE(pts[i].y(), -1);
    EXPECT_GE(pts[i].z(), -1);
    EXPECT_LE(pts[i].x(), 1);
    EXPECT_LE(pts[i].y(), 1);
    EXPECT_LE(pts[i].z(), 1);
  }
}

TEST_F(Object8Test, TestPixelCameraAndPixelQuad) {
  auto scene = ObGen::scene(640, 480);

  auto &quad = scene->add(ObGen::pixelQuad(0, 0, 640, 480));
  quad.geometry().setColors({
    Color::PURPLE,
    Color::CHERRY,
    Color::MINT,
    Color::MANGO,
  });

  auto &camera = scene->add(ObGen::pixelCamera(640, 480));

  auto mvp = camera.projection() * camera.world().inv() * quad.world();
  auto ptsClip = mvp * quad.geometry().vertices();

  EXPECT_EQ(4, ptsClip.size());

  EXPECT_THAT(ptsClip[0].data(), equalsPoint(HPoint3{-1.0f, -1.0f, 1.0f}));
  EXPECT_THAT(ptsClip[1].data(), equalsPoint(HPoint3{-1.0f, 1.0f, 1.0f}));
  EXPECT_THAT(ptsClip[2].data(), equalsPoint(HPoint3{1.0f, 1.0f, 1.0f}));
  EXPECT_THAT(ptsClip[3].data(), equalsPoint(HPoint3{1.0f, -1.0f, 1.0f}));

  auto vertsLocal = camera.projection().inv() * camera.world() * quad.world().inv() * ptsClip;

  EXPECT_THAT(vertsLocal[0].data(), equalsPoint(HPoint3{0.0f, 0.0f, 1.0f}));
  EXPECT_THAT(vertsLocal[1].data(), equalsPoint(HPoint3{0.0f, 480.0f, 1.0f}));
  EXPECT_THAT(vertsLocal[2].data(), equalsPoint(HPoint3{640.0f, 480.0f, 1.0f}));
  EXPECT_THAT(vertsLocal[3].data(), equalsPoint(HPoint3{640.0f, 0.0f, 1.0f}));
}

TEST_F(Object8Test, TestMetadata) {
  auto scene = ObGen::scene(640, 480);

  auto &quad = scene->add(ObGen::pixelQuad(0, 0, 640, 480));

  nlohmann::json metadataJSON = {
    {"pi", 3.141},
    {"happy", true},
  };

  quad.setMetadata(metadataJSON.dump());
  auto metadata = quad.metadata();
  nlohmann::json deserialized = nlohmann::json::parse(metadata);
  EXPECT_THAT(deserialized["pi"], 3.141);
  EXPECT_THAT(deserialized["happy"], true);

  Vector<String> elementMetadata = {"the", "cat", "in", "the", "hat"};
  quad.setElementMetadata(elementMetadata);
  auto eData = quad.elementMetadata();
  EXPECT_THAT(eData[0], "the");
  EXPECT_THAT(eData[4], "hat");
}

TEST_F(Object8Test, TestFindNameOfKindImpl) {
  auto scene = ObGen::scene(640, 480);
  auto &camera = scene->add(ObGen::camera());
  scene->add(ObGen::group());
  scene->add(ObGen::renderable());
  // find("name") and has("name") should not go into subscenes
  auto &subscene = scene->add(ObGen::subScene("sub1", {{320, 480}}));
  subscene.add(ObGen::light());

  // Make sure that, even if they have the same name, find("name") will return the right instance
  EXPECT_TRUE(scene->find<Scene>("").is<Scene>());
  EXPECT_TRUE(scene->find<Renderable>("").is<Renderable>());
  EXPECT_TRUE(scene->find<Group>("").is<Group>());
  EXPECT_TRUE(scene->find<Camera>("").is<Camera>());
  EXPECT_TRUE(subscene.find<Light>("").is<Light>());

  // Make sure that, even if they have the same name, has("name") will return the right instance
  EXPECT_TRUE(scene->has<Scene>(""));
  EXPECT_TRUE(scene->has<Renderable>(""));
  EXPECT_TRUE(scene->has<Group>(""));
  EXPECT_TRUE(scene->has<Camera>(""));
  // find("name") shouldn't recursively search into subscenes.
  EXPECT_FALSE(scene->has<Light>(""));
  EXPECT_FALSE(subscene.has<Group>(""));
  EXPECT_FALSE(subscene.has<Renderable>(""));
  EXPECT_TRUE(subscene.has<Scene>("sub1"));

  camera.setName("perspective");
  EXPECT_FALSE(scene->has<Camera>(""));
  EXPECT_TRUE(scene->has<Camera>("perspective"));
  EXPECT_FALSE(scene->has<Scene>("perspective"));

  scene->setName("world-scene");
  EXPECT_FALSE(scene->has<Scene>(""));
  EXPECT_TRUE(scene->has<Scene>("world-scene"));
  EXPECT_FALSE(scene->has<Camera>("world-scene"));
}

}  // namespace c8
