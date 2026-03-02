// Copyright (c) 2024 Niantic, Inc.
// Original Author: Nicholas Butko (nbutko@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "splat-skybox.h",
  };
  deps = {
    "//c8/geometry:splat",
    "//c8/pixels:pixel-transforms",
    "//c8/pixels/render:renderer",
    "//c8/stats:scope-timer",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x7f165df1);

#include "c8/model/splat-skybox.h"
#include "c8/pixels/pixel-transforms.h"
#include "c8/pixels/render/object8.h"
#include "c8/stats/scope-timer.h"

namespace c8 {

SplatAttributes bakeSplatSkybox(const SplatAttributes &splat, float radius, int faceResolution) {
  ScopeTimer t("bake-splat-skybox");
  SplatAttributes skyPoints{.header = splat.header};
  SplatAttributes restPoints{.header = splat.header};
  float sqrRadius = radius * radius;

  for (int i = 0; i < splat.positions.size(); ++i) {
    auto p = splat.positions[i];
    auto sqd = p.x() * p.x() + p.y() * p.y() + p.z() * p.z();
    auto &destSplat = sqd >= sqrRadius ? skyPoints : restPoints;
    destSplat.positions.push_back(p);
    destSplat.scales.push_back(splat.scales[i]);
    destSplat.rotations.push_back(splat.rotations[i]);
    destSplat.colors.push_back(splat.colors[i]);
    destSplat.shRed.push_back(splat.shRed[i]);
    destSplat.shGreen.push_back(splat.shGreen[i]);
    destSplat.shBlue.push_back(splat.shBlue[i]);
  }

  auto scene = ObGen::scene(faceResolution, faceResolution);
  scene->renderSpec().clearColor = Color::TRUE_BLACK;
  auto &sceneSplat = scene->add(ObGen::splatTexture(skyPoints.header, splatTexture(skyPoints)));

  auto &camera = scene->add(ObGen::perspectiveCamera(
    {.pixelsWidth = faceResolution,
     .pixelsHeight = faceResolution,
     .centerPointX = 0.5f * (faceResolution - 1),
     .centerPointY = 0.5f * (faceResolution - 1),
     .focalLengthHorizontal = 0.5f * (faceResolution),
     .focalLengthVertical = 0.5f * (faceResolution)},
    faceResolution,
    faceResolution));

  restPoints.header.numPoints = restPoints.positions.size();
  restPoints.header.maxNumPoints = restPoints.positions.size();
  restPoints.header.hasSkybox = true;
  restPoints.skybox.data.resize(24 * faceResolution * faceResolution);

  std::array<HMatrix, 6> cameras = {
    HMatrixGen::yDegrees(90.0f),
    HMatrixGen::yDegrees(-90.0f),
    HMatrixGen::xDegrees(-90.0f),
    HMatrixGen::xDegrees(90.0f),
    HMatrixGen::i(),
    HMatrixGen::yDegrees(180.0f),
  };

  Renderer renderer;
  Vector<uint32_t> sortedIds;
  uint8_t *destBytes = restPoints.skybox.data.data();
  RGBA8888PlanePixelBuffer flipScratchBuf(faceResolution, faceResolution);
  auto flipScratch = flipScratchBuf.pixels();
  sortSplatIds(skyPoints, SplatOctreeNode{}, cameras[0], {.euclideanSort = true}, &sortedIds);
  ObGen::updateSplatTexture(&sceneSplat, sortedIds);
  for (const auto &cameraPos : cameras) {
    camera.setLocal(cameraPos);
    renderer.render(*scene);
    renderer.result(flipScratch, {faceResolution, faceResolution, 4 * faceResolution, destBytes});
    destBytes += 4 * faceResolution * faceResolution;
  }

  return restPoints;
}

RGBA8888PlanePixelBuffer mergeSkybox(SplatSkybox skybox) {
  auto faceSize = skybox.px.rows();
  if (faceSize == 0) {
    return {};
  }
  RGBA8888PlanePixelBuffer fullSkyboxBuffer(3 * faceSize, 4 * faceSize);
  auto fullSkybox = fullSkyboxBuffer.pixels();
  fill(0, 0, 0, 255, &fullSkybox);

  auto nx = crop(fullSkybox, faceSize, faceSize, 1 * faceSize, 0 * faceSize);
  auto pz = crop(fullSkybox, faceSize, faceSize, 1 * faceSize, 1 * faceSize);
  auto px = crop(fullSkybox, faceSize, faceSize, 1 * faceSize, 2 * faceSize);
  auto nz = crop(fullSkybox, faceSize, faceSize, 1 * faceSize, 3 * faceSize);
  auto py = crop(fullSkybox, faceSize, faceSize, 0 * faceSize, 1 * faceSize);
  auto ny = crop(fullSkybox, faceSize, faceSize, 2 * faceSize, 1 * faceSize);

  copyPixels(skybox.px, &px);
  copyPixels(skybox.nx, &nx);
  copyPixels(skybox.py, &py);
  copyPixels(skybox.ny, &ny);
  copyPixels(skybox.pz, &pz);
  copyPixels(skybox.nz, &nz);

  return fullSkyboxBuffer;
}

}  // namespace c8
