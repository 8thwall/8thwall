// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Alvin Portillo (alvin@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  visibility = {
    "//reality/engine/executor:__subpackages__",
    "//reality/engine/hittest:__subpackages__",
  };
  hdrs = {
    "hit-test-performer.h",
  };
  deps = {
    "//c8:c8-log",
    "//c8:c8-log-proto",
    "//c8:quaternion",
    "//c8:vector",
    "//c8/geometry:homography",
    "//c8/geometry:two-d",
    "//c8/stats:scope-timer",
    "//reality/engine/api:reality.capnp-cc",
    "//reality/engine/api/response:features.capnp-cc",
    "//reality/engine/tracking:tracker",
  };
}
cc_end(0x18f116b0);

#include <math.h>

#include <cmath>

#include "c8/geometry/homography.h"
#include "c8/geometry/two-d.h"
#include "c8/hpoint.h"
#include "c8/quaternion.h"
#include "c8/vector.h"
#include "reality/engine/hittest/hit-test-performer.h"

using namespace c8;

namespace {

float calculateDistance(ResponseCamera::Reader camera, HPoint3 point) {
  auto cameraPos = camera.getExtrinsic().getPosition();
  float xd = point.x() - cameraPos.getX();
  float yd = point.y() - cameraPos.getY();
  float zd = point.z() - cameraPos.getZ();
  return std::sqrt((xd * xd) + (yd * yd) + (zd * zd));
}

HMatrix getCameraIntrinsics(ResponseCamera::Reader camera) {
  auto i = camera.getIntrinsic().getMatrix44f();
  auto intrinsicsMat = HMatrix{
    {i[0 + 0 * 4], i[0 + 1 * 4], i[0 + 2 * 4], i[0 + 3 * 4]},
    {i[1 + 0 * 4], i[1 + 1 * 4], i[1 + 2 * 4], i[1 + 3 * 4]},
    {i[2 + 0 * 4], i[2 + 1 * 4], i[2 + 2 * 4], i[2 + 3 * 4]},
    {i[3 + 0 * 4], i[3 + 1 * 4], i[3 + 2 * 4], i[3 + 3 * 4]}};
  auto model = HMatrix{
    {-1.0f, 0.0f, 0.0f, 0.0f},  // r0
    {0.0f, 1.0f, 0.0f, 0.0f},   // r1
    {0.0f, 0.0f, 1.0f, 0.0f},   // 42
    {0.0f, 0.0f, 0.0f, 1.0f},
    {-1.0f, 0.0f, 0.0f, 0.0f},  // r0
    {0.0f, 1.0f, 0.0f, 0.0f},   // r1
    {0.0f, 0.0f, 1.0f, 0.0f},   // 42
    {0.0f, 0.0f, 0.0f, 1.0f}};

  return model * intrinsicsMat;
}

HMatrix getCameraExtrinsics(ResponseCamera::Reader camera) {
  Quaternion rot(
    camera.getExtrinsic().getRotation().getW(),
    camera.getExtrinsic().getRotation().getX(),
    camera.getExtrinsic().getRotation().getY(),
    camera.getExtrinsic().getRotation().getZ());
  HPoint3 pt(
    camera.getExtrinsic().getPosition().getX(),
    camera.getExtrinsic().getPosition().getY(),
    camera.getExtrinsic().getPosition().getZ());
  return cameraMotion(pt, rot);
}

float estimateZDepth(Vector<HPoint3> points, HPoint2 pt, HMatrix extrinsic) {
  auto projectedPts3d = extrinsic.inv() * points;
  auto pts2d = flatten<2>(projectedPts3d);

  double wt = 0.0f;
  double zEst = 0.0f;
  for (int i = 0; i < pts2d.size(); ++i) {
    double xd = pt.x() - pts2d[i].x();
    double yd = pt.y() - pts2d[i].y();
    double distance = xd * xd + yd * yd;
    double invdist = std::exp(-0.5 * (1 / (.02 * .02)) * distance);

    zEst += projectedPts3d[i].z() * invdist;
    wt += invdist;
  }

  if (wt > 0.0f) {
    zEst /= wt;
  }

  return static_cast<float>(zEst);
}

Vector<HPoint3> getHitFeaturePoints(
  const HMatrix &extrinsic,
  const HMatrix &intrinsic,
  ResponseFeatureSet::Reader featureSet,
  XrHitTestRequest::Reader request) {
  if (featureSet.getPoints().size() == 0) {
    return Vector<HPoint3>();
  }

  Vector<HPoint3> featurePoints3d;
  featurePoints3d.reserve(featureSet.getPoints().size());
  for (auto point : featureSet.getPoints()) {
    featurePoints3d.push_back(
      HPoint3(point.getPosition().getX(), point.getPosition().getY(), point.getPosition().getZ()));
  }

  HPoint2 hitPointInGraphics((request.getX() - 0.5f) * 2.0f, (request.getY() - 0.5f) * 2.0f);
  auto graphicsPt3d = hitPointInGraphics.extrude();
  auto hitPoint = intrinsic.inv() * graphicsPt3d;
  auto hitPoint2d = hitPoint.flatten();

  auto zEst = estimateZDepth(featurePoints3d, hitPoint2d, extrinsic);
  HPoint3 estPt = HPoint3(zEst * hitPoint2d.x(), zEst * hitPoint2d.y(), zEst);
  HPoint3 worldPt = extrinsic * estPt;

  Vector<HPoint3> hitResults;
  hitResults.push_back(worldPt);
  return hitResults;
}

void setHitResult(
  const ResponseCamera::Reader camera,
  const HPoint3 point,
  const XrHitTestResult::ResultType type,
  XrHitTestResult::Builder *result) {
  result->setType(type);
  result->getPlace().getPosition().setX(point.x());
  result->getPlace().getPosition().setY(point.y());
  result->getPlace().getPosition().setZ(point.z());
  result->setDistance(calculateDistance(camera, point));
}

bool containsResultType(uint32_t resultMask, XrHitTestResult::ResultType type) {
  return (resultMask & (0x1 << static_cast<uint32_t>(type))) != 0 || resultMask == 0;
}

}  // namespace

void HitTestPerformer::performHitTest(
  ResponseCamera::Reader camera,
  ResponseFeatureSet::Reader featurePointSet,
  ResponseSurfaces::Reader surfaces,
  const XrHitTestRequest::Reader request,
  XrHitTestResponse::Builder *response) const {
  auto extrinsic = getCameraExtrinsics(camera);
  auto intrinsic = getCameraIntrinsics(camera);

  uint32_t hitTestResultsMask = 0;
  for (auto resultType : request.getIncludedTypes()) {
    hitTestResultsMask |= (0x1 << static_cast<uint32_t>(resultType));
  }

  auto featurePoints =
    containsResultType(hitTestResultsMask, XrHitTestResult::ResultType::FEATURE_POINT)
    ? getHitFeaturePoints(extrinsic, intrinsic, featurePointSet, request)
    : Vector<HPoint3>();
  auto surfacePoints = Vector<HPoint3>();
  auto estimatedSurfacePoints = Vector<HPoint3>();
  getSurfacePoints(
    extrinsic,
    intrinsic,
    surfaces,
    request,
    hitTestResultsMask,
    &surfacePoints,
    &estimatedSurfacePoints);

  auto totalHits = featurePoints.size() + surfacePoints.size() + estimatedSurfacePoints.size();
  if (totalHits == 0) {
    return;
  }

  auto hits = response->initHits(totalHits);
  auto index = 0;
  for (auto point : featurePoints) {
    auto hit = hits[index];
    setHitResult(camera, point, XrHitTestResult::ResultType::FEATURE_POINT, &hit);
    ++index;
  }

  for (auto point : surfacePoints) {
    auto hit = hits[index];
    setHitResult(camera, point, XrHitTestResult::ResultType::DETECTED_SURFACE, &hit);
    ++index;
  }

  for (auto point : estimatedSurfacePoints) {
    auto hit = hits[index];
    setHitResult(camera, point, XrHitTestResult::ResultType::ESTIMATED_SURFACE, &hit);
    ++index;
  }
}

void HitTestPerformer::getSurfacePoints(
  const HMatrix &extrinsic,
  const HMatrix &intrinsic,
  ResponseSurfaces::Reader surfaceResponse,
  XrHitTestRequest::Reader request,
  uint32_t hitTestResultsMask,
  Vector<HPoint3> *detectedSurfaceResults,
  Vector<HPoint3> *estimatedSurfaceResults) const {
  Vector<HPoint3> hitPoints;
  if (
    !surfaceResponse.hasSet() || surfaceResponse.getSet().getSurfaces().size() == 0
    || (!containsResultType(hitTestResultsMask, XrHitTestResult::ResultType::DETECTED_SURFACE) && containsResultType(hitTestResultsMask, XrHitTestResult::ResultType::ESTIMATED_SURFACE))) {
    return;
  }

  Vector<HPoint3> surfacePts;
  surfacePts.reserve(surfaceResponse.getSet().getVertices().size());
  for (auto vertex : surfaceResponse.getSet().getVertices()) {
    surfacePts.push_back(HPoint3(vertex.getX(), vertex.getY(), vertex.getZ()));
  }

  auto surfaces = surfaceResponse.getSet().getSurfaces();
  auto faces = surfaceResponse.getSet().getFaces();
  HPoint2 hitPointInGraphics((request.getX() - 0.5f) * 2.0f, (request.getY() - 0.5f) * 2.0f);
  auto graphicsPt3d = hitPointInGraphics.extrude();
  auto hitPoint3d = intrinsic.inv() * graphicsPt3d;

  for (auto surface : surfaces) {
    HPoint3 camIntersectPoint;
    bool hitDetectedSurface = false;
    for (int i = surface.getFacesBeginIndex(); i < surface.getFacesEndIndex(); ++i) {
      auto face = faces[i];
      Vector<HPoint3> worldTriangle{
        surfacePts[face.getV0()], surfacePts[face.getV1()], surfacePts[face.getV2()]};

      // Rotate triangle such that the points are all in front of the camera.
      Vector<HPoint3> triangleInFrontOfCam;
      HMatrix r = HMatrixGen::i();
      float d;
      rotatePointsToFrontOfCamera(worldTriangle, extrinsic, &triangleInFrontOfCam, &d, &r);

      // Rotate hit test point by said rotation too.
      auto rHitPoint = r * hitPoint3d.flatten().extrude();

      //  Perform hit test on new triangle.
      auto triangle2d = flatten<2>(triangleInFrontOfCam);
      Poly2 poly(triangle2d);
      auto rHitPoint2d = rHitPoint.flatten();
      camIntersectPoint = r.inv() * HPoint3(rHitPoint2d.x() * d, rHitPoint2d.y() * d, d);
      if (!poly.containsPointAssumingConvex(rHitPoint2d)) {
        // Not found in the current triangle. Let's try the next.
        continue;
      }

      // Find intersect ray using distance
      if (camIntersectPoint.z() < 0.0f) {
        // The hit result is behind the camera.
        continue;
      }

      auto worldIntersectPoint = extrinsic * camIntersectPoint;
      if (containsResultType(hitTestResultsMask, XrHitTestResult::ResultType::DETECTED_SURFACE)) {
        detectedSurfaceResults->push_back(worldIntersectPoint);
      } else {
        estimatedSurfaceResults->push_back(worldIntersectPoint);
      }
      hitDetectedSurface = true;
      break;
    }

    if (!hitDetectedSurface && camIntersectPoint.z() >= 0.0f) {
      auto worldIntersectPoint = extrinsic * camIntersectPoint;
      estimatedSurfaceResults->push_back(worldIntersectPoint);
    }
  }
}

void HitTestPerformer::rotatePointsToFrontOfCamera(
  const Vector<HPoint3> &pts,
  const HMatrix &extrinsic,
  Vector<HPoint3> *rotPts,
  float *d,
  HMatrix *r) {
  auto camTriangle = extrinsic.inv() * pts;
  HVector3 normal;
  planeFromThreePoints({camTriangle[0], camTriangle[1], camTriangle[2]}, &normal, d);

  // First find the angle to rotate about y so that the thing is along the z dimension (x = 0).
  float radY = 0;
  if (normal.x() != 0) {
    radY = atan2(normal.x(), normal.z());
  }
  auto newNormal = HMatrixGen::yRadians(-radY) * normal;

  // Then find the angle to rotate about x so that the thing is along the z dimension (y = 0).
  float radX = 0;
  if (newNormal.y() != 0) {
    radX = atan2(newNormal.y(), newNormal.z());
  }
  newNormal = HMatrixGen::xRadians(radX) * newNormal;
  *r = HMatrixGen::xRadians(radX) * HMatrixGen::yRadians(-radY);
  *rotPts = *r * camTriangle;
  *d = (*rotPts)[0].z();
}
