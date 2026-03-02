// Copyright (c) 2019 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"location.h"};
  deps = {
    ":detection-image",
    "//c8:hpoint",
    "//c8:vector",
    "//c8:color",
    "//c8/geometry:homography",
    "//c8/geometry:parameterized-geometry",
    "//c8/pixels:pixels",
    "//reality/engine/features:frame-point",
    "//reality/engine/features:image-point",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x47c051f0);

#include "c8/geometry/homography.h"
#include "reality/engine/imagedetection/location.h"

namespace c8 {

void getPointsAndRays(
  const Vector<PointMatch> &matches,
  const DetectionImage &target,
  const FrameWithPoints &inputPoints,
  Vector<HPoint3> *worldPts,
  Vector<HPoint2> *camRays) {
  worldPts->clear();
  camRays->clear();
  worldPts->reserve(matches.size());
  camRays->reserve(matches.size());
  const auto &target2 = target.framePoints().points();
  for (const auto &match : matches) {
    const auto &tp = target2[match.dictionaryIdx];
    worldPts->push_back({tp.x(), tp.y(), 1.0f});
    auto cp = inputPoints.points()[match.wordsIdx].position();
    camRays->push_back({cp.x(), cp.y()});
  }
}

// like getPointsAndRays but only does points and map it onto a geometry
void getPointsOnGeometry(
  const Vector<PointMatch> &matches,
  const Vector<HPoint2> &targetPointsInPixel,
  const CurvyImageGeometry &geom,
  Vector<HPoint3> *worldPts) {
  worldPts->clear();
  worldPts->reserve(matches.size());

  Vector<HPoint2> ptsOnImg;
  std::transform(
    matches.begin(),
    matches.end(),
    std::back_inserter(ptsOnImg),
    [targetPointsInPixel](PointMatch match) -> HPoint2 {
      const HPoint2 pt = targetPointsInPixel[match.dictionaryIdx];
      return pt;
    });

  mapToGeometryPoints(geom, ptsOnImg, worldPts);
}

void getRainbowPointsOnGeometry(
  const Vector<PointMatch> &matches,
  const Vector<HPoint2> &targetPointsInPixel,
  const CurvyImageGeometry &geom,
  const RainbowMetadata &rainbowMetadata,
  Vector<HPoint3> *worldPts) {
  worldPts->clear();
  worldPts->reserve(matches.size());

  Vector<HPoint2> ptsOnImg;
  std::transform(
    matches.begin(),
    matches.end(),
    std::back_inserter(ptsOnImg),
    [targetPointsInPixel](PointMatch match) -> HPoint2 {
      const HPoint2 pt = targetPointsInPixel[match.dictionaryIdx];
      return pt;
    });

  mapRainbowToGeometryPoints(geom, rainbowMetadata, ptsOnImg, worldPts);
}

// Like getPointsAndRays but only get the rays
void getMatchedCamRays(
  const Vector<PointMatch> &matches, const FrameWithPoints &inputPoints, Vector<HPoint2> *camRays) {
  camRays->clear();
  camRays->reserve(matches.size());
  const auto &inputPts = inputPoints.points();
  for (const auto &match : matches) {
    auto cp = inputPts[match.wordsIdx].position();
    camRays->push_back({cp.x(), cp.y()});
  }
}

FrameWithPoints projectToTargetSpace(
  const DetectionImage &im, const FrameWithPoints &points, const HMatrix &pose) {
  FrameWithPoints raysInTargetSpace(points.intrinsic());
  raysInTargetSpace.reserve(points.points().size());
  const auto &r = points.points();
  const auto &store = points.store();
  auto H = homographyForPlane(
             pose,
             // This is the plane parallel to the xy plane at z = 1
             getScaledPlaneNormal({0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 1.0f}))
             .inv();

  size_t i = 0;
  for (auto &pt : points.worldPoints()) {
    auto ray = (H * HPoint3{pt.x(), pt.y(), 1.0f}).flatten();
    raysInTargetSpace.addWorldMapPoint(
      WorldMapPointId::nullId(),
      {ray.x(), ray.y(), 1.0f},
      r[i].scale(),
      r[i].angle(),
      r[i].gravityAngle(),
      store.getClone(i));
    i++;
  }
  return raysInTargetSpace;
}

FrameWithPoints projectSearchRayToTargetRay(
  const DetectionImage &im,
  const FrameWithPoints &points,
  const HMatrix &pose,
  const CurvyImageGeometry &geom) {
  FrameWithPoints raysInTargetSpace(im.framePoints().intrinsic());
  raysInTargetSpace.reserve(points.points().size());
  const auto &r = points.points();
  const auto &store = points.store();
  auto ptsInPixel = cameraPointsSearchWorldToTargetPixel(geom, points.hpoints(), pose);

  size_t i = 0;
  for (auto &ptInPixel : ptsInPixel) {
    raysInTargetSpace.addImagePixelPoint(
      ptInPixel, r[i].scale(), r[i].angle(), r[i].gravityAngle(), store.getClone(i));
    i++;
  }
  // C8Log(
  //   "[location] %d points in search pixel mapped to %d points in target ray",
  //   ptsInPixel.size(),
  //   raysInTargetSpace.points().size());

  return raysInTargetSpace;
}

void getMatchedRays(
  const Vector<PointMatch> &matches,
  const DetectionImage &target,
  const FrameWithPoints &inputPoints,
  Vector<HPoint2> *imTargetRays,
  Vector<HPoint2> *camRays,
  Vector<float> *weights) {
  imTargetRays->clear();
  camRays->clear();
  weights->clear();
  imTargetRays->reserve(matches.size());
  camRays->reserve(matches.size());
  weights->reserve(matches.size());
  const auto &target2 = target.framePoints().points();
  for (const auto &match : matches) {
    imTargetRays->push_back(target2[match.dictionaryIdx].position());
    auto cp = inputPoints.points()[match.wordsIdx].position();
    camRays->push_back({cp.x(), cp.y()});
    weights->push_back((256.0f - match.descriptorDist) / 256.0f);
  }
}

}  // namespace c8
