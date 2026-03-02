// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":assets",
    ":renderer",
    "//c8/io:image-io",
    "//c8/geometry:egomotion",
    "//c8/geometry:mesh-types",
    "//c8/geometry:load-mesh",
    "//c8:c8-log",
    "//c8/geometry:intrinsics",
    "//c8/geometry:load-splat",
    "//c8/geometry:vectors",
    "//c8/model:splat-skybox",
    "//c8/pixels:draw2",
    "//c8/pixels:draw3-widgets",
    "//c8/pixels:pixel-transforms",
    "//c8/pixels/opengl:gl-error",
    "//c8/pixels/opengl:offscreen-gl-context",
    "@com_google_googletest//:gtest_main",
  };
  data = {
    "//c8/model/models:minion-spz",
    "//c8/pixels/render/testdata:doty-glb",
    "//c8/pixels/render/testdata:textures",
    "//bzl/examples/draco/testdata:draco-asset",
  };
}
cc_end(0x6fbcbbde);

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "c8/c8-log.h"
#include "c8/geometry/egomotion.h"
#include "c8/geometry/intrinsics.h"
#include "c8/geometry/load-mesh.h"
#include "c8/geometry/load-splat.h"
#include "c8/geometry/mesh-types.h"
#include "c8/geometry/vectors.h"
#include "c8/io/image-io.h"
#include "c8/model/splat-skybox.h"
#include "c8/pixels/draw2.h"
#include "c8/pixels/draw3-widgets.h"
#include "c8/pixels/opengl/gl-error.h"
#include "c8/pixels/opengl/offscreen-gl-context.h"
#include "c8/pixels/pixel-transforms.h"
#include "c8/pixels/render/assets.h"
#include "c8/pixels/render/renderer.h"
#include "c8/stats/scope-timer.h"

namespace c8 {

bool WRITE_IMAGE = true;

class RendererTest : public ::testing::Test {};

TEST_F(RendererTest, TestMeshGeometry) {
  // Create an offscreen context.
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  MeshGeometry mesh = {
    {{-1.0f, -1.0f, 0.0f}, {-1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, -1.0f, 0.0f}},
    {Color::PURPLE, Color::CHERRY, Color::MINT, Color::MANGO},
    {{0, 1, 2}, {2, 3, 0}}};

  auto scene = ObGen::scene(640, 480);

  auto &drawMesh = scene->add(ObGen::meshGeometry(mesh));

  auto meshPos = HMatrixGen::translation(0.f, 0.f, 4.f);
  drawMesh.setLocal(meshPos);

  scene->add(
    ObGen::perspectiveCamera(
      Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_XS), 640, 480));

  Renderer renderer;
  renderer.render(*scene);

  auto img = renderer.result();
  EXPECT_EQ(640, img.pixels().cols());
  EXPECT_EQ(480, img.pixels().rows());

  if (WRITE_IMAGE) {
    writeImage(img.pixels(), "/tmp/renderer-test-mesh.png");
  }
}

TEST_F(RendererTest, TestMeshGeometryInstanced) {
  // Create an offscreen context.
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  MeshGeometry mesh = {
    {{-1.0f, -1.0f, 0.0f}, {-1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, -1.0f, 0.0f}},
    {Color::PURPLE, Color::CHERRY, Color::MINT, Color::MANGO},
    {{0, 1, 2}, {2, 3, 0}}};

  auto scene = ObGen::scene(640, 480);

  auto &drawMesh = scene->add(ObGen::meshGeometry(mesh));

  drawMesh.geometry().setInstancePositions({
    {-2.0f, 2.0f, 3.0f},
    {2.0f, 2.0f, 3.0f},
    {-2.0f, -2.0f, 3.0f},
    {2.0f, -2.0f, 3.0f},
  });

  auto meshPos = HMatrixGen::translation(0.f, 0.f, 4.f);
  drawMesh.setLocal(meshPos);

  scene->add(
    ObGen::perspectiveCamera(
      Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_XS), 640, 480));

  Renderer renderer;
  renderer.render(*scene);

  auto img = renderer.result();
  EXPECT_EQ(640, img.pixels().cols());
  EXPECT_EQ(480, img.pixels().rows());

  if (WRITE_IMAGE) {
    writeImage(img.pixels(), "/tmp/renderer-test-mesh-instanced.png");
  }
}

TEST_F(RendererTest, TestQuadMesh) {
  // Create an offscreen context.
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  auto scene = ObGen::scene(640, 480);

  auto &quad = scene->add(ObGen::quad());

  auto quadPos = HMatrixGen::translation(-.5f, 0.25f, 4.0f);
  quad.setLocal(quadPos);
  quad.geometry().setColors({
    Color::PURPLE,
    Color::CHERRY,
    Color::MINT,
    Color::MANGO,
  });

  auto camPos = updateWorldPosition(
    updateWorldPosition(HMatrixGen::yDegrees(-10), HMatrixGen::xDegrees(10)),
    HMatrixGen::zDegrees(10));

  auto &camera = scene->add(
    ObGen::perspectiveCamera(
      Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_XS), 640, 480));
  camera.setLocal(camPos);

  Renderer renderer;
  renderer.render(*scene);

  auto img = renderer.result();
  EXPECT_EQ(640, img.pixels().cols());
  EXPECT_EQ(480, img.pixels().rows());

  if (WRITE_IMAGE) {
    writeImage(img.pixels(), "/tmp/renderer-test-quad-mesh.png");
  }
}

TEST_F(RendererTest, TestRgbaPixels) {
  // Create an offscreen context.
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  auto scene = ObGen::scene(640, 480);
  auto &quad = scene->add(ObGen::quad());
  quad.setLocal(HMatrixGen::translation(0.0f, 0.0f, 4.0f));
  GeoGen::flipTextureY(&quad.geometry());

  const String texturePath = "c8/pixels/render/testdata/8w_customer_face.png";
  auto imBuffer = readImageToRGBA(texturePath.c_str());

  // The Texture does not own the pixel data, but points to the pixel data owned by imBuffer.
  quad.material().setShader(Shaders::IMAGE).setColorTexture(TexGen::rgbaPixels(imBuffer.pixels()));
  scene->add(
    ObGen::perspectiveCamera(
      Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_XS), 640, 480));

  Renderer renderer;
  renderer.render(*scene);
  auto img = renderer.result();

  EXPECT_EQ(640, img.pixels().cols());
  EXPECT_EQ(480, img.pixels().rows());

  if (WRITE_IMAGE) {
    writeImage(img.pixels(), "/tmp/renderer-test-rgbapixels-image.png");
  }
}

TEST_F(RendererTest, TestRgbaPixelBuffer) {
  // Create an offscreen context.
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  auto scene = ObGen::scene(640, 480);
  auto &quad = scene->add(ObGen::quad());
  quad.setLocal(HMatrixGen::translation(0.0f, 0.0f, 4.0f));
  GeoGen::flipTextureY(&quad.geometry());

  {
    // Once we leave this scope, imBuffer of type PixelBuffer will be automatically deleted, which
    // is fine since we moved the contents to be owned by Object8::Texture's PixelBuffer member.
    const String texturePath = "c8/pixels/render/testdata/8w_customer_face.png";
    auto imBuffer = readImageToRGBA(texturePath.c_str());

    // The Object8::Texture will now own the pixels that were previously owned by imBuffer.
    quad.material()
      .setShader(Shaders::IMAGE)
      .setColorTexture(TexGen::rgbaPixelBuffer(std::move(imBuffer)));
    scene->add(
      ObGen::perspectiveCamera(
        Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_XS), 640, 480));
  }

  Renderer renderer;
  renderer.render(*scene);
  auto img = renderer.result();

  EXPECT_EQ(640, img.pixels().cols());
  EXPECT_EQ(480, img.pixels().rows());

  if (WRITE_IMAGE) {
    writeImage(img.pixels(), "/tmp/renderer-test-rgbapixelbuffer-image.png");
  }
}

TEST_F(RendererTest, TestDepthPixels) {
  // Create an offscreen context.
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  auto scene = ObGen::scene(640, 480);
  auto &quad = scene->add(ObGen::quad());
  quad.setLocal(HMatrixGen::translation(0.0f, 0.0f, 4.0f));
  GeoGen::flipTextureY(&quad.geometry());
  DepthFloatPixelBuffer p(640, 480);
  DepthFloatPixels pr = p.pixels();
  {
    quad.material().setShader(Shaders::IMAGE).setDepthTexture(TexGen::depthPixels(pr));
    scene->add(
      ObGen::perspectiveCamera(
        Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_XS), 640, 480));
  }
  Renderer renderer;
  renderer.render(*scene);
  auto img = renderer.result();
  EXPECT_EQ(640, img.pixels().cols());
  EXPECT_EQ(480, img.pixels().rows());
}

TEST_F(RendererTest, TestDepthPixelBuffer) {
  // Create an offscreen context.
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  auto scene = ObGen::scene(640, 480);
  auto &quad = scene->add(ObGen::quad());
  quad.setLocal(HMatrixGen::translation(0.0f, 0.0f, 4.0f));
  GeoGen::flipTextureY(&quad.geometry());
  DepthFloatPixelBuffer p(640, 480);
  DepthFloatPixels pr = p.pixels();
  {
    quad.material()
      .setShader(Shaders::IMAGE)
      .setDepthTexture(TexGen::depthPixelBuffer(std::move(p)));
    scene->add(
      ObGen::perspectiveCamera(
        Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_XS), 640, 480));
  }
  Renderer renderer;
  renderer.render(*scene);
  auto img = renderer.result();
  EXPECT_EQ(640, img.pixels().cols());
  EXPECT_EQ(480, img.pixels().rows());
}

TEST_F(RendererTest, TestQuadPoints) {
  // Create an offscreen context.
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  auto scene = ObGen::scene(640, 480);

  auto &quad = scene->add(ObGen::quadPoints());

  auto quadPos = HMatrixGen::translation(-.5f, 0.25f, 4.0f);
  quad.setLocal(quadPos);
  quad.geometry().setColors({
    Color::PURPLE,
    Color::CHERRY,
    Color::MINT,
    Color::MANGO,
  });

  auto camPos = updateWorldPosition(
    updateWorldPosition(HMatrixGen::yDegrees(-10), HMatrixGen::xDegrees(10)),
    HMatrixGen::zDegrees(10));

  auto &camera = scene->add(
    ObGen::perspectiveCamera(
      Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_XS), 640, 480));
  camera.setLocal(camPos);

  Renderer renderer;
  renderer.render(*scene);

  auto img = renderer.result();
  EXPECT_EQ(640, img.pixels().cols());
  EXPECT_EQ(480, img.pixels().rows());

  if (WRITE_IMAGE) {
    writeImage(img.pixels(), "/tmp/renderer-test-quad-points.png");
  }
}

TEST_F(RendererTest, TestSubScenesRender) {
  // Create an offscreen context.
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  auto scene = ObGen::scene(640, 480);
  scene->setName("rootScene");

  // camera and sub-scene for left side
  int lWidth = 320;
  int lHeight = 480;
  const Vector<RenderSpec> renderSpecs1 = {{lWidth, lHeight, "r1", "c1"}};
  auto subscene1 = ObGen::subScene("leftScene", renderSpecs1);
  subscene1->add(
    ObGen::named(
      ObGen::perspectiveCamera(
        Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_XS), lWidth, lHeight),
      "c1"));
  auto &lQuad = subscene1->add(ObGen::quad());
  lQuad.geometry().setColors({
    Color::MINT,
    Color::CHERRY,
    Color::MANGO,
    Color::PURPLE,
  });
  auto quadPos = HMatrixGen::translation(0.0f, 0.0f, 4.0f);
  lQuad.setLocal(quadPos);
  scene->add(std::move(subscene1));

  // camera and sub-scene for right side
  int rWidth = 320;
  int rHeight = 480;
  const Vector<RenderSpec> renderSpecs2 = {{rWidth, rHeight, "r2", "c2"}};
  auto subscene2 = ObGen::subScene("rightScene", renderSpecs2);
  subscene2->add(
    ObGen::named(
      ObGen::perspectiveCamera(
        Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_XS), rWidth, rHeight),
      "c2"));
  auto &rQuad = subscene2->add(ObGen::quad());
  auto quadPos2 = HMatrixGen::translation(0.0f, 0.0f, 4.0f);
  rQuad.setLocal(quadPos2);
  rQuad.geometry().setColors({
    Color::PURPLE,
    Color::CHERRY,
    Color::MINT,
    Color::MANGO,
  });
  scene->add(std::move(subscene2));

  // Composite scene
  scene->add(ObGen::named(ObGen::pixelCamera(640, 480), "pixelCamera"));
  auto &lSceneQuad = scene->add(ObGen::pixelQuad(0, 0, 320, 480));
  lSceneQuad.setMaterial(MatGen::subsceneMaterial("leftScene", "r1"));

  auto &rSceneQuad = scene->add(ObGen::pixelQuad(320, 0, 320, 480));
  rSceneQuad.setMaterial(MatGen::subsceneMaterial("rightScene", "r2"));

  Renderer renderer;
  renderer.render(*scene);

  // Composite output
  auto img = renderer.result();
  EXPECT_EQ(640, img.pixels().cols());
  EXPECT_EQ(480, img.pixels().rows());

  if (WRITE_IMAGE) {
    writeImage(img.pixels(), "/tmp/renderer-test-subscenes.png");
  }
}

TEST_F(RendererTest, TestGetSceneOrdering) {
  // Create an offscreen context.
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  auto scene = ObGen::scene(640, 480);
  int width = 640;
  int height = 480;

  // sub1 has two renderable: ren1 and ren2
  // ren1 depends on sub2 via its texture
  // sub2 has one renderable: ren3
  // the correct rendering order is thus sub2 -> sub1 -> main

  // Subscene 1 with two renderable. One depends on subscene 2
  auto subscene1 =
    ObGen::subScene("sub1", {{width, height, "r1", "c1"}, {width, height, "unused", "c1"}});
  auto &renderable1 = subscene1->add(ObGen::pixelQuad(0, 0, width / 2, height));
  renderable1.setName("renderable1");
  // NOTE(dat): the values "sub2" and "r2" have to match what is created below
  renderable1.setMaterial(MatGen::subsceneMaterial("sub2", "r2"));

  auto &renderable2 = subscene1->add(ObGen::pixelQuad(width / 2, 0, width / 2, height));
  renderable2.setName("renderable2");

  // Add a second subscene
  auto subscene2 =
    ObGen::subScene("sub2", {{width, height, "unused", "c2"}, {width, height, "r2", "c2"}});

  scene->add(std::move(subscene1));
  scene->add(std::move(subscene2));
  // The root scene has to depends on the subscenes for the subscenes to be drawn
  scene->add(ObGen::named(ObGen::pixelQuad(0, 0, width / 2, height), "sub1-renderable"))
    .setMaterial(MatGen::subsceneMaterial("sub1", "r1"));
  scene->add(ObGen::named(ObGen::pixelQuad(width / 2, 0, width / 2, height), "sub2-renderable"))
    .setMaterial(MatGen::subsceneMaterial("sub2", "r2"));

  Vector<SceneRenderSpecPair> orderedScenes;
  bool canGetOrder = getSceneRenderingOrder(*scene, &orderedScenes);
  EXPECT_TRUE(canGetOrder);
  EXPECT_EQ(3, orderedScenes.size());
  EXPECT_EQ("sub2", orderedScenes.at(0).first->name());
  EXPECT_EQ("r2", orderedScenes.at(0).second->name);
  EXPECT_EQ("sub1", orderedScenes.at(1).first->name());
  EXPECT_EQ("r1", orderedScenes.at(1).second->name);
  EXPECT_EQ("", orderedScenes.at(2).first->name());
  EXPECT_EQ("", orderedScenes.at(2).second->name);
}

TEST_F(RendererTest, TestGetSceneOrdering2) {
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  auto scene = ObGen::scene(640, 480);
  int width = 640;
  int height = 480;

  // sub1 has two renderable: ren1 and ren2. Ren2 is child of ren1
  // ren1 depends on sub2 via its texture
  // ren2 depends on sub3 via its texture
  // there is no ordering to children rendering in a scene
  // the correct rendering order is thus sub2/3 -> sub2/3 -> sub1 -> main

  auto subscene1 =
    ObGen::subScene("sub1", {{width, height, "r1", "c1"}, {width, height, "unused", "c1"}});
  auto &renderable1 = subscene1->add(ObGen::pixelQuad(0, 0, width / 2, height));
  renderable1.setName("renderable1");
  renderable1.setMaterial(MatGen::subsceneMaterial("sub2", "r2"));

  auto &renderable2 = renderable1.add(ObGen::pixelQuad(width / 2, 0, width / 2, height));
  renderable2.setName("renderable2");
  renderable2.setMaterial(MatGen::subsceneMaterial("sub3", "r3"));

  // Add a second subscene
  auto subscene2 =
    ObGen::subScene("sub2", {{width, height, "unused", "c2"}, {width, height, "r2", "c2"}});
  auto subscene3 =
    ObGen::subScene("sub3", {{width, height, "unused", "c3"}, {width, height, "r3", "c3"}});

  scene->add(std::move(subscene1));
  scene->add(std::move(subscene2));
  scene->add(std::move(subscene3));
  // The root scene has to depends on the subscenes for the subscenes to be drawn
  scene->add(ObGen::named(ObGen::pixelQuad(0, 0, width / 2, height), "sub1-renderable"))
    .setMaterial(MatGen::subsceneMaterial("sub1", "r1"));

  Vector<SceneRenderSpecPair> orderedScenes;
  bool canGetOrder = getSceneRenderingOrder(*scene, &orderedScenes);
  EXPECT_TRUE(canGetOrder);
  EXPECT_EQ(4, orderedScenes.size());
  EXPECT_TRUE(
    orderedScenes.at(0).first->name() == "sub2" || orderedScenes.at(0).first->name() == "sub3");
  EXPECT_TRUE(orderedScenes.at(0).second->name == "r2" || orderedScenes.at(0).second->name == "r3");
  EXPECT_TRUE(
    orderedScenes.at(1).first->name() == "sub2" || orderedScenes.at(1).first->name() == "sub3");
  EXPECT_TRUE(orderedScenes.at(1).second->name == "r2" || orderedScenes.at(1).second->name == "r3");
  EXPECT_EQ("sub1", orderedScenes.at(2).first->name());
  EXPECT_EQ("r1", orderedScenes.at(2).second->name);
  EXPECT_EQ("", orderedScenes.at(3).first->name());
  EXPECT_EQ("", orderedScenes.at(3).second->name);
}

TEST_F(RendererTest, TestGetSceneOrderingDiamondDAG) {
  // Create an offscreen context.
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  auto scene = ObGen::scene(640, 480);
  int width = 640;
  int height = 480;

  // sub1 has two renderable: ren1 and ren2
  // ren1 depends on sub2 via its texture
  // ren2 depends on sub3 via its texture
  // sub2 has one renderable: ren3
  // ren3 depends on sub3 via its texture
  // sub3 has nothing to render
  // the correct rendering order is thus sub3 -> sub2 -> sub1 -> main

  // Subscene 1
  auto subscene1 = ObGen::subScene("sub1", {{width, height, "r1", "c1"}});
  auto &renderable1 = subscene1->add(ObGen::pixelQuad(0, 0, width / 2, height));
  renderable1.setName("renderable1");
  renderable1.setMaterial(MatGen::subsceneMaterial("sub2", "r2"));
  auto &renderable2 = subscene1->add(ObGen::pixelQuad(width / 2, 0, width / 2, height));
  renderable2.setName("renderable2");
  renderable2.setMaterial(MatGen::subsceneMaterial("sub3", "r3"));

  // Subscene 2
  auto subscene2 = ObGen::subScene("sub2", {{width, height, "r2", "c2"}});
  auto &renderable3 = subscene2->add(ObGen::pixelQuad(width / 2, 0, width / 2, height));
  renderable3.setName("renderable3");
  renderable3.setMaterial(MatGen::subsceneMaterial("sub3", "r3"));

  // Subscene 3, note that r3.unused is not rendered
  auto subscene3 =
    ObGen::subScene("sub3", {{width, height, "r3", "c3"}, {width, height, "r3.unused", "c3"}});

  scene->add(std::move(subscene1));
  scene->add(std::move(subscene2));
  scene->add(std::move(subscene3));
  // The root scene has to depends on the subscenes for the subscenes to be drawn
  scene->add(ObGen::named(ObGen::pixelQuad(0, 0, width, height), "sub1-renderable"))
    .setMaterial(MatGen::subsceneMaterial("sub1", "r1"));

  Vector<SceneRenderSpecPair> orderedScenes;
  bool canGetOrder = getSceneRenderingOrder(*scene, &orderedScenes);
  EXPECT_TRUE(canGetOrder);
  EXPECT_EQ(4, orderedScenes.size());
  EXPECT_EQ("sub3", orderedScenes.at(0).first->name());
  EXPECT_EQ("r3", orderedScenes.at(0).second->name);
  EXPECT_EQ("sub2", orderedScenes.at(1).first->name());
  EXPECT_EQ("r2", orderedScenes.at(1).second->name);
  EXPECT_EQ("sub1", orderedScenes.at(2).first->name());
  EXPECT_EQ("r1", orderedScenes.at(2).second->name);
  EXPECT_EQ("", orderedScenes.at(3).first->name());
  EXPECT_EQ("", orderedScenes.at(3).second->name);
}

TEST_F(RendererTest, TestGetSceneOrderingBadCycle) {
  // Create an offscreen context.
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  auto scene = ObGen::scene(640, 480);
  int width = 640;
  int height = 480;

  // sub1 has a renderable: ren1
  // ren1 depends on sub2 via its texture
  // sub2 has one renderable: ren2
  // ren2 depends on sub1 via its texture

  auto subscene1 = ObGen::subScene("sub1", {{width, height, "r1", "c1"}});
  auto &renderable1 = subscene1->add(ObGen::pixelQuad(0, 0, width / 2, height));
  renderable1.setName("renderable1");
  renderable1.setMaterial(MatGen::subsceneMaterial("sub2", "r2"));

  // Add a second subscene
  auto subscene2 = ObGen::subScene("sub2", {{width, height, "r2", "c2"}});
  auto &renderable2 = subscene2->add(ObGen::pixelQuad(0, 0, width / 2, height));
  renderable2.setName("renderable2");
  renderable2.setMaterial(MatGen::subsceneMaterial("sub1", "r1"));

  scene->add(std::move(subscene1));
  scene->add(std::move(subscene2));
  // The root scene has to depends on the subscenes for the subscenes to be drawn
  scene->add(ObGen::named(ObGen::pixelQuad(0, 0, width, height), "sub1-renderable"))
    .setMaterial(MatGen::subsceneMaterial("sub1", "r1"));

  Vector<SceneRenderSpecPair> orderedScenes;
  bool canGetOrder = getSceneRenderingOrder(*scene, &orderedScenes);
  EXPECT_FALSE(canGetOrder);
}

TEST_F(RendererTest, TestGetSubscenes) {
  // Create an offscreen context.
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  Vector<Scene *> subscenes;
  auto scene = ObGen::scene(640, 480);

  getSubscenes(*scene.get(), &subscenes);
  EXPECT_EQ(0, subscenes.size());
  subscenes.clear();

  int width = 640;
  int height = 480;

  // Add one subscene
  scene->add(ObGen::subScene("subscene", {{width, height, "r1", "c1"}}));
  getSubscenes(*scene.get(), &subscenes);
  EXPECT_EQ(1, subscenes.size());
  subscenes.clear();

  // Add a second subscene
  scene->add(ObGen::subScene("subscene", {{width, height, "r2", "c2"}}));
  getSubscenes(*scene.get(), &subscenes);
  EXPECT_EQ(2, subscenes.size());
  subscenes.clear();

  // Add a third subscene that is within a group
  auto group = ObGen::group();
  group->add(ObGen::subScene("subscene", {{width, height, "r3", "c3"}}));
  scene->add(std::move(group));

  getSubscenes(*scene.get(), &subscenes);
  EXPECT_EQ(3, subscenes.size());
  subscenes.clear();
}

TEST_F(RendererTest, TextPixelCamera) {
  // Create an offscreen context.
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  auto scene = ObGen::scene(640, 480);
  scene->setName("rootScene");

  auto &lQuad = scene->add(ObGen::pixelQuad(0, 0, 320, 480));
  lQuad.geometry().setColor(Color::PURPLE);

  auto &rQuad = scene->add(ObGen::pixelQuad(320, 0, 320, 480));
  rQuad.geometry().setColor(Color::MANGO);

  scene->add(ObGen::pixelCamera(640, 480));

  Renderer renderer;
  renderer.render(*scene);

  auto img = renderer.result();
  EXPECT_EQ(640, img.pixels().cols());
  EXPECT_EQ(480, img.pixels().rows());

  if (WRITE_IMAGE) {
    writeImage(img.pixels(), "/tmp/renderer-test-pixel-camera.png");
  }
}

TEST_F(RendererTest, TestCube) {
  // Create an offscreen context.
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  char cubeTexturePath[] = "c8/pixels/render/testdata/cubemaps_skybox.png";
  auto cubeTextureUnflipped = readImageToRGBA(cubeTexturePath);
  ScopeTimer t("test-cube");
  RGBA8888PlanePixelBuffer cubeBuffer(
    cubeTextureUnflipped.pixels().rows(), cubeTextureUnflipped.pixels().cols());
  auto cubeTexture = cubeBuffer.pixels();
  // OpenGL expects the 0.0 coordinate on the y-axis to be on the bottom side of the image, but
  // images have 0.0 at the top of the y-axis
  flipVertical(cubeTextureUnflipped.pixels(), &cubeTexture);

  auto scene = ObGen::scene(640, 480);

  auto &cube = scene->add(ObGen::cubeMesh());
  cube.material().setShader(Shaders::IMAGE).setColorTexture(TexGen::rgbaPixels(cubeTexture));
  cube.setLocal(HMatrixGen::translation(-1.0f, 0.0f, 4.0f));

  auto &quad = scene->add(ObGen::quad());
  quad.material().setShader(Shaders::IMAGE).setColorTexture(TexGen::rgbaPixels(cubeTexture));
  quad.setLocal(HMatrixGen::translation(1.0f, 0.0f, 4.0f));

  scene->add(
    ObGen::perspectiveCamera(
      Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_XS), 640, 480));

  Renderer renderer;

  for (int i = 0; i < 4; i++) {
    renderer.render(*scene);

    auto img = renderer.result();
    EXPECT_EQ(640, img.pixels().cols());
    EXPECT_EQ(480, img.pixels().rows());
    String output = format("/tmp/renderer-test-cube_%d.png", i);

    if (WRITE_IMAGE) {
      writeImage(img.pixels(), output);
    }

    auto delta = updateWorldPosition(cube.local(), HMatrixGen::yDegrees(90));
    cube.setLocal(delta);
  }
}

TEST_F(RendererTest, TestPixelLines) {
  // Create an offscreen context.
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();
  ScopeTimer t("test-pixel-lines");

  auto scene = ObGen::scene(640, 480);

  // lines that are one pixel thick.
  Vector<std::pair<HPoint2, HPoint2>> onePixelLines = {
    {{1.0f, 1.0f}, {638.0f, 1.0f}},      // left->right
    {{620.0f, 100.0f}, {3.0f, 100.0f}},  // right->left
  };

  // lines that are three pixels thick.
  Vector<std::pair<HPoint2, HPoint2>> threePixelLines = {
    {{240.0f, 3.0f}, {240.0f, 33.0f}},     // top->bottom
    {{440.0f, 478.0f}, {440.0f, 378.0f}},  // bottom->top
  };

  scene->add(ObGen::pixelLines(onePixelLines, Color::MANGO, 640, 480, 1.0f));
  scene->add(ObGen::pixelLines(threePixelLines, Color::CHERRY, 640, 480, 3.0f));

  // perspective camera should be ignored by pixel lines.
  scene->add(
    ObGen::perspectiveCamera(
      Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_XS), 640, 480));

  Renderer renderer;

  renderer.render(*scene);

  auto img = renderer.result();

  // Check each pixel in the image.
  for (int r = 0; r < 480; ++r) {
    for (int c = 0; c < 640; ++c) {
      const auto *p = &img.pixels().pixels()[img.pixels().rowBytes() * r + c * 4];
      // one pixel lines (MANGO)
      if (r == 1 && (c >= 1 && c <= 638)) {
        EXPECT_EQ(p[0], Color::MANGO.r()) << "r: " << r << "; c: " << c;
        EXPECT_EQ(p[1], Color::MANGO.g()) << "r: " << r << "; c: " << c;
        EXPECT_EQ(p[2], Color::MANGO.b()) << "r: " << r << "; c: " << c;
        EXPECT_EQ(p[3], Color::MANGO.a()) << "r: " << r << "; c: " << c;
        continue;
      }
      if (r == 100 && (c >= 3 && c <= 620)) {
        EXPECT_EQ(p[0], Color::MANGO.r()) << "r: " << r << "; c: " << c;
        EXPECT_EQ(p[1], Color::MANGO.g()) << "r: " << r << "; c: " << c;
        EXPECT_EQ(p[2], Color::MANGO.b()) << "r: " << r << "; c: " << c;
        EXPECT_EQ(p[3], Color::MANGO.a()) << "r: " << r << "; c: " << c;
        continue;
      }
      // Three pixel lines (CHERRY)
      if ((r >= 3 && r <= 33) && (c >= 239 && c <= 241)) {
        EXPECT_EQ(p[0], Color::CHERRY.r()) << "r: " << r << "; c: " << c;
        EXPECT_EQ(p[1], Color::CHERRY.g()) << "r: " << r << "; c: " << c;
        EXPECT_EQ(p[2], Color::CHERRY.b()) << "r: " << r << "; c: " << c;
        EXPECT_EQ(p[3], Color::CHERRY.a()) << "r: " << r << "; c: " << c;
        continue;
      }
      if ((r >= 378 && r <= 478) && (c >= 439 && c <= 441)) {
        EXPECT_EQ(p[0], Color::CHERRY.r()) << "r: " << r << "; c: " << c;
        EXPECT_EQ(p[1], Color::CHERRY.g()) << "r: " << r << "; c: " << c;
        EXPECT_EQ(p[2], Color::CHERRY.b()) << "r: " << r << "; c: " << c;
        EXPECT_EQ(p[3], Color::CHERRY.a()) << "r: " << r << "; c: " << c;
        continue;
      }

      // Background (BLACK)
      EXPECT_EQ(p[0], Color::BLACK.r()) << "r: " << r << "; c: " << c;
      EXPECT_EQ(p[1], Color::BLACK.g()) << "r: " << r << "; c: " << c;
      EXPECT_EQ(p[2], Color::BLACK.b()) << "r: " << r << "; c: " << c;
      EXPECT_EQ(p[3], Color::BLACK.a()) << "r: " << r << "; c: " << c;
    }
  }

  if (WRITE_IMAGE) {
    writeImage(img.pixels(), "/tmp/renderer-test-pixel-lines.png");
  }
}

TEST_F(RendererTest, TestScene) {
  // Create an offscreen context.
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  auto scene = ObGen::scene(640, 480);

  // Create an axis visualization.
  scene->add(ObGen::named(orientedPoint({0.0f, -1.0f, 0.0f}, {}, 0.5f), "origin"));

  // Add floor visualization.
  scene->add(
    ObGen::named(
      ObGen::positioned(groundLineGrid(11, 1.0f), HMatrixGen::translateY(-1.0f)), "ground"));

  // Add cube.
  auto &cube = scene->add(ObGen::cubeMesh());
  cube.geometry().setColor(Color::WHITE);
  cube.setLocal(HMatrixGen::translation(-2.f, 0.0f, 2.f));

  // Update camera position.
  auto camPos = HMatrixGen::translation(.2f, 0.f, -3.f) * HMatrixGen::rotationD(10.f, 0.f, 0.f);
  auto &camera = scene->add(
    ObGen::perspectiveCamera(
      Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_XS), 640, 480));
  camera.setLocal(camPos);

  scene->add(ObGen::ambientLight(Color::WHITE, .4f));

  // One face of the cube should be matcha.
  auto lightPos = HMatrixGen::translation(.5f, 0.f, 1.f) * HMatrixGen::scale(.5f);
  scene->add(ObGen::positioned(ObGen::pointLight(Color::MATCHA, 1.f, true), lightPos));

  // The other should be purple.
  scene->add(
    ObGen::positioned(
      ObGen::directionalLight(Color::PURPLE, .4f, true),
      rotationToVector({.3f, 0.f, 1.f}).toRotationMat()));

  Renderer renderer;
  renderer.render(*scene);

  auto [scratch, img] = renderer.allocateForResult();
  renderer.result(scratch.pixels(), img.pixels());
  EXPECT_EQ(640, img.pixels().cols());
  EXPECT_EQ(480, img.pixels().rows());
  // We don't care about scratch

  // Calling the second time does not require allocation.
  renderer.result(scratch.pixels(), img.pixels());
  if (WRITE_IMAGE) {
    writeImage(img.pixels(), "/tmp/renderer-test-scene.png");
  }
}

TEST_F(RendererTest, TestPixelsPointsPerspective) {
  // Create an offscreen context.
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  int radius = 4;
  int diameter = 2 * radius + 1;
  int width = diameter * 9;
  int height = diameter * 5;

  auto scene = ObGen::scene(width, height);
  scene->add(
    ObGen::perspectiveCamera(
      Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_XS), width, height));
  auto &ptCloud = scene->add(ObGen::pixelPoints());

  Vector<HPoint2> points;
  for (int i = radius; i < width; i += diameter) {
    for (int j = radius; j < height; j += diameter) {
      points.push_back({static_cast<float>(i), static_cast<float>(j)});
    }
  }

  ObGen::updatePixelPoints(&ptCloud, points, Color::PURPLE, width, height, diameter);

  Renderer renderer;
  renderer.render(*scene);

  auto imgbuf = renderer.result();
  auto img = imgbuf.pixels();
  EXPECT_EQ(width, img.cols());
  EXPECT_EQ(height, img.rows());

  // Corners should be black.
  for (int i = 0; i < width; i += diameter) {
    for (int j = 0; j < height; j += diameter) {
      const auto *pix = img.pixels() + img.rowBytes() * j + i * 4;
      EXPECT_EQ(pix[0], Color::BLACK.r());
      EXPECT_EQ(pix[1], Color::BLACK.g());
      EXPECT_EQ(pix[2], Color::BLACK.b());
      EXPECT_EQ(pix[3], Color::BLACK.a());
    }
  }

  // Centers should be purple.
  for (int i = radius; i < width; i += diameter) {
    for (int j = radius; j < height; j += diameter) {
      const auto *pix = img.pixels() + img.rowBytes() * j + i * 4;
      EXPECT_EQ(pix[0], Color::PURPLE.r());
      EXPECT_EQ(pix[1], Color::PURPLE.g());
      EXPECT_EQ(pix[2], Color::PURPLE.b());
      EXPECT_EQ(pix[3], Color::PURPLE.a());
    }
  }

  if (WRITE_IMAGE) {
    writeImage(img, "/tmp/renderer-test-pixels-points-perspective.png");
  }
}

TEST_F(RendererTest, TestPixelsPointsPixelCamera) {
  // Create an offscreen context.
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  int radius = 40;
  int diameter = 2 * radius + 1;
  int width = diameter * 9;
  int height = diameter * 5;

  auto scene = ObGen::scene(width, height);
  scene->add(ObGen::pixelCamera(width, height));
  auto &ptCloud = scene->add(ObGen::pixelPoints());

  Vector<HPoint2> points;
  for (int i = radius; i < width; i += diameter) {
    for (int j = radius; j < height; j += diameter) {
      points.push_back({static_cast<float>(i), static_cast<float>(j)});
    }
  }

  ObGen::updatePixelPoints(&ptCloud, points, Color::PURPLE, width, height, diameter);

  Renderer renderer;
  renderer.render(*scene);

  auto imgbuf = renderer.result();
  auto img = imgbuf.pixels();
  EXPECT_EQ(width, img.cols());
  EXPECT_EQ(height, img.rows());

  // Corners should be black.
  for (int i = 0; i < width; i += diameter) {
    for (int j = 0; j < height; j += diameter) {
      const auto *pix = img.pixels() + img.rowBytes() * j + i * 4;
      EXPECT_EQ(pix[0], Color::BLACK.r());
      EXPECT_EQ(pix[1], Color::BLACK.g());
      EXPECT_EQ(pix[2], Color::BLACK.b());
      EXPECT_EQ(pix[3], Color::BLACK.a());
    }
  }

  // Centers should be purple.
  for (int i = radius; i < width; i += diameter) {
    for (int j = radius; j < height; j += diameter) {
      const auto *pix = img.pixels() + img.rowBytes() * j + i * 4;
      EXPECT_EQ(pix[0], Color::PURPLE.r());
      EXPECT_EQ(pix[1], Color::PURPLE.g());
      EXPECT_EQ(pix[2], Color::PURPLE.b());
      EXPECT_EQ(pix[3], Color::PURPLE.a());
    }
  }

  if (WRITE_IMAGE) {
    writeImage(img, "/tmp/renderer-test-pixels-points-pixel-camera.png");
  }
}

TEST_F(RendererTest, TestAssetsGlbDiffuseJpg) {
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  auto scene = ObGen::scene(1000, 1000);

  auto &drawMesh = scene->add(glbFileToMesh("c8/pixels/render/testdata/doty.glb"));
  auto meshPos = HMatrixGen::translation(0.05f, -0.175f, 0.6f);
  drawMesh.setLocal(meshPos);

  scene->add(
    ObGen::perspectiveCamera(
      Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_XS), 1000, 1000));

  Renderer renderer;
  renderer.render(*scene);

  auto img = renderer.result();

  EXPECT_EQ(1000, img.pixels().cols());
  EXPECT_EQ(1000, img.pixels().rows());

  if (WRITE_IMAGE) {
    writeImage(img.pixels(), "/tmp/renderer-test-assets-glb-diffuse-jpg.png");
  }
}

TEST_F(RendererTest, TestAssetsGlbDiffuseJpgInstanced) {
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  auto scene = ObGen::scene(1000, 1000);

  auto &drawMesh = scene->add(glbFileToMesh("c8/pixels/render/testdata/doty.glb"));
  auto meshPos = HMatrixGen::translation(0.5f, -0.5f, 0.6f);
  drawMesh.setLocal(meshPos);

  drawMesh.geometry().setInstancePositions({
    {-1.0f, 1.0f, 2.0f},
    {0.0f, 1.0f, 2.0f},
    {-1.0f, 0.0f, 2.0f},
    {0.0f, 0.0f, 2.0f},
  });

  drawMesh.geometry().setInstanceRotations({
    Quaternion::fromPitchYawRollDegrees(0.0f, 30.0f, 0.0f),
    Quaternion::fromPitchYawRollDegrees(-30.0f, 0.0f, 0.0f),
    Quaternion::fromPitchYawRollDegrees(0.0f, 0.0f, 30.0f),
    Quaternion::fromPitchYawRollDegrees(0.0f, 0.0f, 0.0f),
  });

  drawMesh.geometry().setInstanceScales({
    {0.6f, 0.6f, 0.6f},
    {0.8f, 0.8f, 0.8f},
    {1.0f, 1.0f, 1.0f},
    {1.2f, 1.2f, 1.2f},
  });

  scene->add(
    ObGen::perspectiveCamera(
      Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_XS), 1000, 1000));

  Renderer renderer;
  renderer.render(*scene);

  auto img = renderer.result();

  EXPECT_EQ(1000, img.pixels().cols());
  EXPECT_EQ(1000, img.pixels().rows());

  if (WRITE_IMAGE) {
    writeImage(img.pixels(), "/tmp/renderer-test-assets-glb-diffuse-jpg-instanced.png");
  }
}

TEST_F(RendererTest, TestSplat) {
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  auto scene = ObGen::scene(151, 251);
  scene->renderSpec().clearColor = Color::TRUE_BLACK;

  scene->add(
    ObGen::splatAttributes(
      {
        // Positions
        {-0.5f, 0.5f, 0.0f},
        {-1.5f, -2.9f, -1.5f},
        {0.0f, 0.0f, 0.0f},
        {1.5f, 2.9f, -1.5f},
        {1.5f, -2.9f, -1.0f},
        {-1.5f, 2.9f, -1.5f},
      },
      {
        // Rotations
        Quaternion::fromPitchYawRollDegrees(0.0f, 90.0f, 90.0f),
        Quaternion::fromPitchYawRollDegrees(0.0f, 0.0f, 0.0f),
        Quaternion::fromPitchYawRollDegrees(90.0f, 0.0f, 90.0f),
        Quaternion::fromPitchYawRollDegrees(0.0f, 0.0f, 0.0f),
        Quaternion::fromPitchYawRollDegrees(0.0f, 0.0f, 0.0f),
        Quaternion::fromPitchYawRollDegrees(0.0f, 0.0f, 0.0f),
      },
      {
        // Scales
        {1.0f, 1.0f, 3.0f},
        {1.0f, 1.0f, 1.0f},
        {1.0f, 2.0f, 3.0f},
        {1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f},
      },
      {
        // Colors
        Color::WHITE,
        Color::BLUE,
        Color::WHITE,
        Color::GREEN,
        Color::RED,
        Color::YELLOW,
      }));

  scene->add(
    ObGen::positioned(
      ObGen::perspectiveCamera(
        Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_XS), 151, 251),
      HMatrixGen::translation(0.0f, 0.0f, -10.0f)));

  Renderer renderer;
  renderer.render(*scene);

  auto img = renderer.result();

  EXPECT_EQ(151, img.pixels().cols());
  EXPECT_EQ(251, img.pixels().rows());

  // Check the pixel color at the center of the splat.
  const int col = 151 / 2;
  const int row = 251 / 2;
  const auto pixels = img.pixels();
  const uint8_t *data = pixels.pixels() + row * pixels.rowBytes() + (col << 2);

  const double normalizeFactor = 255.0;
  const double epsilon = 0.02;

  EXPECT_LE((Color::WHITE.r() - data[0]) / normalizeFactor, epsilon);
  EXPECT_LE((Color::WHITE.g() - data[1]) / normalizeFactor, epsilon);
  EXPECT_LE((Color::WHITE.b() - data[2]) / normalizeFactor, epsilon);

  if (WRITE_IMAGE) {
    writeImage(img.pixels(), "/tmp/renderer-test-splat.png");
  }
}

TEST_F(RendererTest, TestSplatSpz) {
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  constexpr int outRows = 1080;
  constexpr int outCols = 1920;

  auto fullSplat = splatAttributes(loadSplatSpzFilePacked("c8/model/models/minion.spz"), true);

  EXPECT_EQ(660325, fullSplat.header.maxNumPoints);
  EXPECT_EQ(660325, fullSplat.positions.size());

  auto scene = ObGen::scene(outCols, outRows);
  scene->renderSpec().clearColor = Color::TRUE_BLACK;

  auto &camera = scene->add(
    ObGen::perspectiveCamera(
      Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_XS), outCols, outRows));
  camera.setLocal(HMatrixGen::translation(1.1122911f, -0.73037136f, -6.7184558f));

  auto &splat = scene->add(ObGen::splatTexture(fullSplat.header, splatTexture(fullSplat)));
  Vector<uint32_t> sortedIds;
  sortSplatIds(fullSplat, SplatOctreeNode{}, camera.world(), {}, &sortedIds);
  EXPECT_EQ(660325, sortedIds.size());
  ObGen::updateSplatTexture(&splat, sortedIds);

  Renderer renderer;
  renderer.render(*scene);
  checkGLError("[renderer-test] TestSplatSpz: after render");

  auto img = renderer.result();

  EXPECT_EQ(outCols, img.pixels().cols());
  EXPECT_EQ(outRows, img.pixels().rows());

  if (WRITE_IMAGE) {
    writeImage(img.pixels(), "/tmp/renderer-test-splat-spz.png");
  }
}

TEST_F(RendererTest, TestSplatBakeSkybox) {
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  constexpr int outRows = 1080;
  constexpr int outCols = 1920;

  auto fullSplat = bakeSplatSkybox(
    splatAttributes(loadSplatSpzFilePacked("c8/model/models/minion.spz"), true), 2.0f, 512);

  EXPECT_EQ(61972, fullSplat.header.maxNumPoints);
  EXPECT_EQ(61972, fullSplat.positions.size());

  auto scene = ObGen::scene(outCols, outRows);
  scene->renderSpec().clearColor = Color::TRUE_BLACK;

  auto &camera = scene->add(
    ObGen::perspectiveCamera(
      Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_XS), outCols, outRows));
  camera.setLocal(HMatrixGen::translation(1.1122911f, -0.73037136f, -6.7184558f));

  auto &splat = scene->add(ObGen::splatTexture(fullSplat.header, splatTexture(fullSplat)));

  // TODO: Move this into ObGen::splatTexture.
  splat.add(ObGen::positioned(ObGen::cubeMesh(), trsMat({}, Quaternion::xDegrees(180), 700.0f)))
    .setMaterial(MatGen::image())
    .material()
    .setRenderSide(RenderSide::BACK)
    .colorTexture()
    ->setRgbaPixelBuffer(mergeSkybox(fullSplat.skybox.skybox()));

  Vector<uint32_t> sortedIds;
  sortSplatIds(fullSplat, SplatOctreeNode{}, camera.world(), {}, &sortedIds);
  EXPECT_EQ(61972, sortedIds.size());
  ObGen::updateSplatTexture(&splat, sortedIds);

  Renderer renderer;
  renderer.render(*scene);
  checkGLError("[renderer-test] TestSplatBakeSkybox: after render");

  auto img = renderer.result();

  EXPECT_EQ(outCols, img.pixels().cols());
  EXPECT_EQ(outRows, img.pixels().rows());

  if (WRITE_IMAGE) {
    writeImage(img.pixels(), "/tmp/renderer-test-splat-bake-skybox.png");
  }
}

TEST_F(RendererTest, TestSplatTextureStacked) {
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  constexpr int outRows = 1080;
  constexpr int outCols = 1920;

  auto fullSplat = splatAttributes(loadSplatSpzFilePacked("c8/model/models/minion.spz"), true);

  EXPECT_EQ(660325, fullSplat.header.maxNumPoints);
  EXPECT_EQ(660325, fullSplat.positions.size());

  auto scene = ObGen::scene(outCols, outRows);
  scene->renderSpec().clearColor = Color::TRUE_BLACK;

  auto &camera = scene->add(
    ObGen::perspectiveCamera(
      Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_XS), outCols, outRows));
  camera.setLocal(HMatrixGen::translation(1.1122911f, -0.73037136f, -6.7184558f));

  auto &splat =
    scene->add(ObGen::splatTextureStacked(fullSplat.header, splatTexture(fullSplat), 128));
  Vector<uint32_t> sortedIds;
  sortSplatIds(fullSplat, SplatOctreeNode{}, camera.world(), {}, &sortedIds);
  EXPECT_EQ(660325, sortedIds.size());
  ObGen::updateSplatTextureStacked(&splat, sortedIds.data(), sortedIds.size());

  Renderer renderer;
  renderer.render(*scene);
  checkGLError("[renderer-test] TestSplatSpz: after render");

  auto img = renderer.result();

  EXPECT_EQ(outCols, img.pixels().cols());
  EXPECT_EQ(outRows, img.pixels().rows());

  if (WRITE_IMAGE) {
    writeImage(img.pixels(), "/tmp/renderer-test-splat-texture-stacked.png");
  }
}

TEST_F(RendererTest, TestSplatMultiTexSh) {
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  auto fullSplat = splatAttributes(loadSplatSpzFilePacked("c8/model/models/minion.spz"), true);

  EXPECT_EQ(660325, fullSplat.header.maxNumPoints);
  EXPECT_EQ(660325, fullSplat.positions.size());

  SplatMultiTexture multitex{
    .header = fullSplat.header,
    .sortedIds = sortedIds(fullSplat),
    .positionColor = positionColor(fullSplat),
    .rotationScale = rotationScale(fullSplat),
    .shRG = sh(fullSplat, 0),
    .shB = sh(fullSplat, 2),
    .skybox = fullSplat.skybox,
  };

  SplatMultiTextureView splatView = {
    .header = multitex.header,
    .sortedIds = multitex.sortedIds.pixels(),
    .positionColor = multitex.positionColor.pixels(),
    .rotationScale = multitex.rotationScale.pixels(),
    .shRG = multitex.shRG.pixels(),
    .shB = multitex.shB.pixels(),
    .skybox = multitex.skybox.skybox(),
  };

  auto expected =
    RGBA8888PlanePixelBuffer(splatView.positionColor.rows(), splatView.positionColor.cols());
  std::fill(
    expected.pixels().pixels(),
    expected.pixels().pixels() + expected.pixels().rows() * expected.pixels().rowBytes(),
    0);

  for (int idx = 0; idx < expected.pixels().rows() * expected.pixels().rowBytes(); idx += 4) {
    auto color = splatView.positionColor.pixels()[idx + 3];
    std::memcpy(expected.pixels().pixels() + idx, &color, 4);
  }

  auto scene = ObGen::scene(splatView.positionColor.cols(), splatView.positionColor.rows());
  scene->renderSpec().clearColor = Color::CLEAR;

  auto &shCamera = scene->add(ObGen::named(ObGen::camera(), "shCam"));
  shCamera.setLocal(HMatrixGen::translation(1.1122911f, -0.73037136f, -6.7184558f));

  auto &shQuad = scene->add(ObGen::quad());
  shQuad.setMaterial(MatGen::splatMultiTexSh())
    .material()
    .setTexture(
      Shaders::RENDERER_SPLAT_POSITIONCOLOR_TEX, TexGen::rgba32Pixels(splatView.positionColor))
    .setTexture(Shaders::RENDERER_SPLAT_SHRG_TEX, TexGen::rgba32Pixels(splatView.shRG))
    .setTexture(Shaders::RENDERER_SPLAT_SHB_TEX, TexGen::rgba32Pixels(splatView.shB))
    .setTransparent(false);

  Renderer renderer;
  renderer.render(*scene);
  checkGLError("[renderer-test] TestSplatSpz: after render");

  RGBA8888PlanePixelBuffer img(splatView.positionColor.rows(), splatView.positionColor.cols());
  renderer.result(img.pixels());

  EXPECT_EQ(splatView.positionColor.cols(), img.pixels().cols());
  EXPECT_EQ(splatView.positionColor.rows(), img.pixels().rows());

  if (WRITE_IMAGE) {
    writeImage(img.pixels(), "/tmp/renderer-test-splat-multitex-sh.png");
    writeImage(expected.pixels(), "/tmp/renderer-test-splat-multitex-sh-expected.png");
  }
}

TEST_F(RendererTest, TestSplatMultiTex) {
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  constexpr int outRows = 1080;
  constexpr int outCols = 1920;

  auto fullSplat = splatAttributes(loadSplatSpzFilePacked("c8/model/models/minion.spz"), true);

  EXPECT_EQ(660325, fullSplat.header.maxNumPoints);
  EXPECT_EQ(660325, fullSplat.positions.size());

  SplatMultiTexture multitex{
    .header = fullSplat.header,
    .sortedIds = sortedIds(fullSplat),
    .positionColor = positionColor(fullSplat),
    .rotationScale = rotationScale(fullSplat),
    .shRG = sh(fullSplat, 0),
    .shB = sh(fullSplat, 2),
    .skybox = fullSplat.skybox,
  };

  SplatMultiTextureView splatView = {
    .header = multitex.header,
    .sortedIds = multitex.sortedIds.pixels(),
    .positionColor = multitex.positionColor.pixels(),
    .rotationScale = multitex.rotationScale.pixels(),
    .shRG = multitex.shRG.pixels(),
    .shB = multitex.shB.pixels(),
    .skybox = multitex.skybox.skybox(),
  };

  auto scene = ObGen::scene(outCols, outRows);
  scene->renderSpec().clearColor = Color::TRUE_BLACK;

  auto &camera = scene->add(
    ObGen::perspectiveCamera(
      Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_XS), outCols, outRows));
  camera.setLocal(HMatrixGen::translation(1.1122911f, -0.73037136f, -6.7184558f));

  auto &splat = scene->add(ObGen::splatMultiTex(splatView));

  Vector<uint32_t> sortedIds;
  sortSplatIds(fullSplat, SplatOctreeNode{}, camera.world(), {}, &sortedIds);
  EXPECT_EQ(660325, sortedIds.size());
  ObGen::updateSplatMultiTex(&splat, sortedIds.data(), sortedIds.size());
  ObGen::updateSplatMultiTexCamera(&splat, camera);

  Renderer renderer;
  renderer.render(*scene);
  checkGLError("[renderer-test] TestSplatSpz: after render");

  auto img = renderer.result();

  EXPECT_EQ(outCols, img.pixels().cols());
  EXPECT_EQ(outRows, img.pixels().rows());

  if (WRITE_IMAGE) {
    writeImage(img.pixels(), "/tmp/renderer-test-splat-multitex.png");
  }
}

TEST_F(RendererTest, TestInterleavedRgbaPixelBuffer) {
  // Create an offscreen context.
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  const int width = 640;
  const int height = 480;

  auto scene = ObGen::scene(width, height);
  auto &quad = scene->add(ObGen::quad(true));
  quad.setLocal(HMatrixGen::translation(0.0f, 0.0f, 3.0f));

  {
    // Once we leave this scope, imBuffer of type PixelBuffer will be automatically deleted, which
    // is fine since we moved the contents to be owned by Object8::Texture's PixelBuffer member.
    const String texturePath = "c8/pixels/render/testdata/8w_customer_face.png";
    auto imBuffer = readImageToRGBA(texturePath.c_str());

    // The Object8::Texture will now own the pixels that were previously owned by imBuffer.
    quad.material()
      .setShader(Shaders::IMAGE)
      .setColorTexture(TexGen::rgbaPixelBuffer(std::move(imBuffer)));
    scene->add(
      ObGen::perspectiveCamera(
        Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_XS), width, height));
  }

  Renderer renderer;
  renderer.render(*scene);
  auto img = renderer.result();

  EXPECT_EQ(640, img.pixels().cols());
  EXPECT_EQ(480, img.pixels().rows());

  // Check the pixel color at the center of the image.
  const int col = width / 2;
  const int row = height / 2;
  const auto pixels = img.pixels();
  const uint8_t *data = pixels.pixels() + row * pixels.rowBytes() + (col << 2);

  const double normalizeFactor = 255.0;
  const double epsilon = 0.02;
  const Color expectedColor = Color(0xFE, 0xDE, 0x57);  // Yellow Smiley Face

  EXPECT_LE((expectedColor.r() - data[0]) / normalizeFactor, epsilon);
  EXPECT_LE((expectedColor.g() - data[1]) / normalizeFactor, epsilon);
  EXPECT_LE((expectedColor.b() - data[2]) / normalizeFactor, epsilon);

  if (WRITE_IMAGE) {
    writeImage(img.pixels(), "/tmp/renderer-test-interleaved-rgbapixelbuffer-image.png");
  }
}

TEST_F(RendererTest, TestInterleavedInstanceRgbaPixelBuffer) {
  // Create an offscreen context.
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  auto scene = ObGen::scene(640, 480);
  auto &quad = scene->add(ObGen::quad(true));
  quad.setLocal(HMatrixGen::translation(0.0f, 0.0f, 10.0f));

  // Assign interleaved instance data to the quad.
  // Note: Position and Scale are vec4s
  const int posDims = 4;
  const int rotationDims = 4;
  const int scaleDims = 4;
  const int stride = (posDims + rotationDims + scaleDims) * sizeof(float);

  const Quaternion q = Quaternion::fromPitchYawRollDegrees(0.0f, 0.0f, 45.0f);
  InterleavedBufferData ibd = {
    .data = GeoGen::dataToBytes<float>({
      -2.0f, 2.0f,  0.0f, 1.0f, q.x(), q.y(), q.z(), q.w(), 1.0f, 1.0f, 1.0f, 1.0f,
      -2.0f, -2.0f, 0.0f, 1.0f, q.x(), q.y(), q.z(), q.w(), 1.5f, 1.5f, 1.0f, 1.0f,
      2.0f,  2.0f,  0.0f, 1.0f, q.x(), q.y(), q.z(), q.w(), 0.5f, 0.5f, 1.0f, 1.0f,
      2.0f,  -2.0f, 0.0f, 1.0f, q.x(), q.y(), q.z(), q.w(), 1.0f, 1.0f, 1.0f, 1.0f,
    }),
    .attributes =
      {
        {Shaders::INSTANCE_POSITION, posDims, GpuDataType::FLOAT, false, stride, 0},
        {Shaders::INSTANCE_ROTATION,
         rotationDims,
         GpuDataType::FLOAT,
         false,
         stride,
         posDims * sizeof(float)},
        {Shaders::INSTANCE_SCALE,
         scaleDims,
         GpuDataType::FLOAT,
         false,
         stride,
         (posDims + rotationDims) * sizeof(float)},
      },
  };
  quad.geometry().setInterleavedInstanceData(ibd);

  // Used to verify that the interleaved instance data is properly copied.
  ibd.data.clear();
  ibd.attributes.clear();

  {
    // Once we leave this scope, imBuffer of type PixelBuffer will be automatically deleted, which
    // is fine since we moved the contents to be owned by Object8::Texture's PixelBuffer member.
    const String texturePath = "c8/pixels/render/testdata/8w_customer_face.png";
    auto imBuffer = readImageToRGBA(texturePath.c_str());

    // The Object8::Texture will now own the pixels that were previously owned by imBuffer.
    quad.material()
      .setShader(Shaders::IMAGE)
      .setColorTexture(TexGen::rgbaPixelBuffer(std::move(imBuffer)));
    scene->add(
      ObGen::perspectiveCamera(
        Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_XS), 640, 480));
  }

  Renderer renderer;
  renderer.render(*scene);
  auto img = renderer.result();

  EXPECT_EQ(640, img.pixels().cols());
  EXPECT_EQ(480, img.pixels().rows());

  // Check the pixel color at the each smiley face.
  const Vector<int> col = {240, 420, 255, 440};
  const Vector<int> row = {160, 156, 364, 364};
  const Color expectedColor = Color(0xFE, 0xDE, 0x57);  // Yellow Smiley Face

  for (int i = 0; i < 4; i++) {
    int iCol = col[i];
    int iRow = row[i];

    const auto pixels = img.pixels();
    const uint8_t *data = pixels.pixels() + iRow * pixels.rowBytes() + (iCol << 2);

    const double normalizeFactor = 255.0;
    const double epsilon = 0.02;

    EXPECT_LE((expectedColor.r() - data[0]) / normalizeFactor, epsilon);
    EXPECT_LE((expectedColor.g() - data[1]) / normalizeFactor, epsilon);
    EXPECT_LE((expectedColor.b() - data[2]) / normalizeFactor, epsilon);
  }

  if (WRITE_IMAGE) {
    writeImage(img.pixels(), "/tmp/renderer-test-interleaved-instance-rgbapixelbuffer-image.png");
  }
}

TEST_F(RendererTest, TestGravityAlignedVectors) {
  // Create an offscreen context.
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  // Create a 11x11x11 3D cube of up vectors. They are spaced at 1 unit intervals, with a height of
  // 0.3 units, centered at the origin.
  Vector<std::pair<HPoint3, HPoint3>> bars;
  for (float i = -5.0f; i <= 5.0f; i += 1.0f) {
    for (float j = -5.0f; j <= 5.0f; j += 1.0f) {
      for (float k = -5.0f; k <= 5.0f; k += 1.0f) {
        bars.push_back({HPoint3(i, j, k), HPoint3(i, j + 0.3f, k)});
      }
    }
  }

  // Create a high res image so that the lines of the thin bars can be seen clearly, with minimal
  // aliasing.
  auto scene = ObGen::scene(4000, 4000);
  scene->add(ObGen::barCloud(bars, Color::CHERRY, 0.01f));
  auto &camera = scene->add(
    ObGen::perspectiveCamera(
      Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_XS), 4000, 4000));

  // To see the distortion of the up-angle, it's important to have a high degree of pitch.
  // Offsetting slightly in x allows the bars behind each other to be seen better.
  camera.setLocal(cameraMotion(
    HPoint3{0.2f, 16.0f, -9.0f}, Quaternion::fromPitchYawRollDegrees(60.0f, 5.0f, 15.0f)));

  Renderer renderer;
  renderer.render(*scene);
  auto img = renderer.result();

  EXPECT_EQ(4000, img.pixels().cols());
  EXPECT_EQ(4000, img.pixels().rows());

  if (WRITE_IMAGE) {
    writeImage(img.pixels(), "/tmp/renderer-test-gravity-aligned-vectors.png");
  }
}

}  // namespace c8
