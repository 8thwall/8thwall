// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Alvin Portillo (alvin@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  visibility = {
    "//reality/engine/executor:__subpackages__",
    "//reality/engine/features:__subpackages__",
  };
  hdrs = {
    "feature-set-request-executor.h",
  };
  deps = {
    "//c8:c8-log",
    "//c8:c8-log-proto",
    "//c8:vector",
    "//c8/stats:scope-timer",
    "//c8/time:now",
    "//reality/engine/api:reality.capnp-cc",
    "//reality/engine/api/response:features.capnp-cc",
    "//reality/engine/tracking:tracker",
    "//third_party/smhasher:murmurhash3",
  };
}
cc_end(0x77bab3c1);

#include "c8/c8-log-proto.h"
#include "c8/stats/scope-timer.h"
#include "reality/engine/features/feature-set-request-executor.h"
#include "third_party/smhasher/MurmurHash3.h"

using namespace c8;

namespace {

HPoint3 correctC8Position(HPoint3 pt, const ResponsePose::Reader &computedPose) {
  auto initRot = computedPose.getInitialTransform().getRotation();
  auto initPos = computedPose.getInitialTransform().getPosition();
  auto initRotQ = Quaternion(initRot.getW(), initRot.getX(), initRot.getY(), initRot.getZ());
  auto initPosV = HVector3(initPos.getX(), initPos.getY(), initPos.getZ());
  auto initCam = cameraMotion(initPosV, initRotQ);

  return initCam.inv() * pt;
}

HPoint3 correctARCorePosition(HPoint3 pt, const ResponsePose::Reader &computedPose) {
  auto initRot = computedPose.getInitialTransform().getRotation();
  auto initPos = computedPose.getInitialTransform().getPosition();
  auto initRotQ = Quaternion(initRot.getW(), initRot.getX(), initRot.getY(), initRot.getZ());
  auto initPosV = HVector3(initPos.getX(), initPos.getY(), initPos.getZ());
  auto initCam = cameraMotion(initPosV, initRotQ);

  return initCam.inv() * HPoint3(pt.x(), pt.y(), -pt.z());
}

HPoint3 correctARKitPosition(HPoint3 pt, const ResponsePose::Reader &computedPose) {
  auto initRot = computedPose.getInitialTransform().getRotation();
  auto initPos = computedPose.getInitialTransform().getPosition();
  auto initRotQ = Quaternion(initRot.getW(), initRot.getX(), initRot.getY(), initRot.getZ());
  auto initPosV = HVector3(initPos.getX(), initPos.getY(), initPos.getZ());
  auto initCam = cameraMotion(initPosV, initRotQ);
  auto initCamInv = initCam.inv();
  HMatrix invX{
    {-1.0f, 0.0f, 0.0f, 0.0f},
    {0.00f, 1.0f, 0.0f, 0.0f},
    {0.00f, 0.0f, 1.0f, 0.0f},
    {0.00f, 0.0f, 0.0f, 1.0f}};

  return initCamInv * invX * pt;
}

/* Appears to no longer be used.
uint64_t getIdFromPosition(HPoint3 p, uint32_t seed) {
  uint64_t hash;
  float values[3] = {p.x(), p.y(), p.z()};
  MurmurHash3_x86_32(&values, sizeof(values), seed, &hash);
  return hash;
}
*/

void setARCoreFeaturePointSet(
  const capnp::List<ARCorePoint>::Reader &pointCloud,
  const ResponsePose::Reader &computedPose,
  ResponseFeatureSet::Builder *response,
  uint32_t seed) {
  auto pointCloudSize = pointCloud.size();
  auto featureSetBuilder = response->initPoints(pointCloudSize);

  for (int i = 0; i < pointCloudSize; ++i) {
    auto point = pointCloud[i].getPosition();
    auto correctedPoint =
      correctARCorePosition(HPoint3(point.getX(), point.getY(), point.getZ()), computedPose);

    featureSetBuilder[i].getPosition().setX(correctedPoint.x());
    featureSetBuilder[i].getPosition().setY(correctedPoint.y());
    featureSetBuilder[i].getPosition().setZ(correctedPoint.z());
    featureSetBuilder[i].setId(pointCloud[i].getId());
    featureSetBuilder[i].setConfidence(pointCloud[i].getConfidence());
  }
}

void setARKitFeaturePointSet(
  const capnp::List<ARKitPoint>::Reader &pointCloud,
  const ResponsePose::Reader &computedPose,
  ResponseFeatureSet::Builder *response) {
  auto pointCloudSize = pointCloud.size();
  auto featureSetBuilder = response->initPoints(pointCloudSize);

  for (int i = 0; i < pointCloudSize; ++i) {
    auto point = pointCloud[i].getPosition();
    auto correctedPoint =
      correctARKitPosition(HPoint3(point.getX(), point.getY(), point.getZ()), computedPose);

    featureSetBuilder[i].getPosition().setX(correctedPoint.x());
    featureSetBuilder[i].getPosition().setY(correctedPoint.y());
    featureSetBuilder[i].getPosition().setZ(correctedPoint.z());
    featureSetBuilder[i].setId(pointCloud[i].getId());
    featureSetBuilder[i].setConfidence(1.0f);
  }
}

}  // namespace

void FeatureSetRequestExecutor::execute(
  const RealityRequest::Reader &request,
  const ResponsePose::Reader &computedPose,
  Tracker *tracker,
  ResponseFeatureSet::Builder *response) const {
  ScopeTimer t("feature-set");

  if (request.getSensors().hasARCore()) {
    setARCoreFeaturePointSet(
      request.getSensors().getARCore().getPointCloud(), computedPose, response, hashSeed_);
  } else if (request.getSensors().hasARKit()) {
    setARKitFeaturePointSet(
      request.getSensors().getARKit().getPointCloud(), computedPose, response);
  }
}
