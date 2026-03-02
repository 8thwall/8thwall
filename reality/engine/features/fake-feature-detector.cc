// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "fake-feature-detector.h",
  };
  deps = {
    ":frame-point",
    ":image-descriptor",
    "//c8:hpoint",
    "//c8:hmatrix",
    "//c8:c8-log",
    "//c8:vector",
    "//c8/geometry:egomotion",
    "//c8/geometry:pixel-pinhole-camera-model",
    "//c8/stats:scope-timer",
  };
}
cc_end(0xbbfc940d);

#include <cstdlib>

#include "c8/c8-log.h"
#include "c8/geometry/egomotion.h"
#include "c8/geometry/pixel-pinhole-camera-model.h"
#include "c8/hmatrix.h"
#include "c8/stats/scope-timer.h"
#include "c8/vector.h"
#include "reality/engine/features/fake-feature-detector.h"
#include "reality/engine/features/image-descriptor.h"

namespace c8 {

namespace {}  // namespace

FakeFeatureDetector::FakeFeatureDetector(const FakeFeatureDetectorNoise &noise)
    : rng_(), uniform8u_(0, 255), noise_(noise) {}

void FakeFeatureDetector::detectFeatures(
  const HMatrix &camPos, const Vector<HPoint3> &worldPts, FrameWithPoints *pts) {
  ScopeTimer t("fake-detect-features");

  // Generate our descriptors based on point id.
  Vector<OrbFeature> descriptors;
  for (size_t i = 0; i < worldPts.size(); ++i) {
    descriptors.emplace_back(generateDescriptorForPoint(camPos, worldPts[i], i));
  }

  // TODO(nb): Pick bits to flip with a second rng. Otherwise the bit flips will be deterministic.

  // Project three-d and two-d to check for chirality and image bounds.
  auto camPts = camPos.inv() * worldPts;
  c8_PixelPinholeCameraModel intrinsic = pts->intrinsic();
  HMatrix K = HMatrixGen::intrinsic(intrinsic);
  auto imPts = flatten<2>(K * camPts);

  // Perform visibility (chirality and image bounds) tests, and only accept points that are visible.
  pts->clear();
  for (int i = 0; i < worldPts.size(); ++i) {
    if (camPts[i].z() <= 0.0f) {
      continue;
    }
    if (imPts[i].x() < 0 || imPts[i].x() >= intrinsic.pixelsWidth) {
      continue;
    }
    if (imPts[i].y() < 0 || imPts[i].y() >= intrinsic.pixelsHeight) {
      continue;
    }

    // TODO(nb): Pick octave based on distance.
    int octave = 0;
    float angle = 0.0f;
    float gravityAngle = 0.0f;
    pts->addImagePixelPoint(imPts[i], octave, angle, gravityAngle, {descriptors[i].clone()});
  }
}

void FakeFeatureDetector::detectFeatures(
  const Vector<HPoint2> &pixPts, const Vector<uint8_t> &foundMask, FrameWithPoints *pts) {
  ScopeTimer t("fake-detect-features");

  // Generate our descriptors based on point id.
  Vector<OrbFeature> descriptors;
  for (size_t i = 0; i < pixPts.size(); ++i) {
    descriptors.emplace_back(generateDescriptor(i));
  }

  // TODO(nb): Pick bits to flip with a second rng. Otherwise the bit flips will be deterministic.

  // Perform visibility (chirality and image bounds) tests, and only accept points that are visible.
  pts->clear();
  for (int i = 0; i < pixPts.size(); ++i) {
    if (foundMask[i] == false) {
      continue;
    }

    // TODO(nb): Pick octave based on distance.
    int octave = 0;
    float angle = 0.0f;
    float gravityAngle = 0.0f;
    pts->addImagePixelPoint(pixPts[i], octave, angle, gravityAngle, {descriptors[i].clone()});
  }
}

OrbFeature FakeFeatureDetector::generateDescriptor(uint32_t id) {
  rng_.seed(id);
  std::array<uint8_t, 32> descriptor;
  for (int i = 0; i < descriptor.size(); ++i) {
    descriptor[i] = uniform8u_(rng_);
  }
  return OrbFeature(descriptor);
}

OrbFeature FakeFeatureDetector::generateDescriptorForPoint(
  const HMatrix &cam, const HPoint3 &point, uint32_t pointIndex) {

  uint32_t id = pointIndex;

  if (noise_.viewpointQuantization) {
    // Get the camera position.
    HPoint3 camPos = cam * HPoint3{};

    // Points viewed at sufficiently different orientations in the world look different. To account
    // for that, create a different descriptor based on the quantized orientation of the viewpoint
    // vector.
    HVector3 orientation(camPos.x() - point.x(), camPos.y() - point.y(), camPos.z() - point.z());

    // Project the vector onto unit planes for X, Y, and Z.
    int maxIndex = std::fabs(orientation.x()) > std::fabs(orientation.y()) ? 0 : 1;
    maxIndex = (maxIndex == 0) ? (std::fabs(orientation.x()) > std::fabs(orientation.z()) ? 0 : 2)
                               : (std::fabs(orientation.y()) > std::fabs(orientation.z()) ? 1 : 2);

    const float maxNorm = std::fabs(orientation.data()[maxIndex]);

    const HVector3 unitCubeVector =
      HVector3(orientation.x() / maxNorm, orientation.y() / maxNorm, orientation.z() / maxNorm);

    const uint32_t cubeFaceIndex =
      (maxIndex << 1) + static_cast<uint32_t>(unitCubeVector.data()[maxIndex] > 0);

    const uint32_t xIndex = (maxIndex + 1) % 3;
    const uint32_t yIndex = (maxIndex + 2) % 3;

    // Quantize each cube face into squares.
    constexpr int BUCKETS_PER_FACE = 4;
    const uint32_t xHash =
      static_cast<int>(BUCKETS_PER_FACE * 0.5f * (unitCubeVector.data()[xIndex] + 1.0f) + 0.5f);
    const uint32_t yHash =
      static_cast<int>(BUCKETS_PER_FACE * 0.5f * (unitCubeVector.data()[yIndex] + 1.0f) + 0.5f);

    // Also quantize based on log-depth.
    const float logDepth = std::log(orientation.l2Norm());

    // Quantization factor with scales at (1.2x). Later consider octaves here.
    const uint32_t depthHash = std::max(static_cast<int>(logDepth / std::log(1.2)), 32);

    const int hashShift = static_cast<int>(std::ceil(std::log2(BUCKETS_PER_FACE)));
    id = cubeFaceIndex;
    id = (id << hashShift) | yHash;
    id = (id << hashShift) | xHash;
    id = (id << 6) | depthHash;
    id = (id << 16) | pointIndex;
  }

  return generateDescriptor(id);
}

}  // namespace c8
