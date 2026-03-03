// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include <random>

#include "c8/hmatrix.h"
#include "c8/hpoint.h"
#include "c8/vector.h"
#include "reality/engine/features/frame-point.h"

namespace c8 {

// Sources of simulated noise that can be used to have fake feature detectors mimic characteristics
// of real data.
struct FakeFeatureDetectorNoise {
  // Return a noise-free model.
  static FakeFeatureDetectorNoise none() {
    FakeFeatureDetectorNoise noise;
    noise.viewpointQuantization = false;
    return noise;
  }

  // Return a model with an approximation of realistic noise.
  static FakeFeatureDetectorNoise realistic() {
    FakeFeatureDetectorNoise noise;
    noise.viewpointQuantization = true;
    return noise;
  }

  // Generate different features based on quantized view orientation and depth.
  bool viewpointQuantization = false;

  // TODO(nb): implement me.
  // float discriptorCorruptionLevel = 0.0f;
  // bool discretizeFeaturePixels = false;
  // float threeDPointPositionNoise = 0.0f;
  // float imagePointPositionNoise = 0.0f;
};

class FakeFeatureDetector {
public:
  // Construct a FakeFeatureDetector with a noise model.
  //
  // Example construction:
  //   FakeFeatureDetector d(FakeFeatureDetectorNoise::realistic());
  FakeFeatureDetector(const FakeFeatureDetectorNoise &noise);

  // Default move constructors.
  FakeFeatureDetector(FakeFeatureDetector &&) = default;
  FakeFeatureDetector &operator=(FakeFeatureDetector &&) = default;

  // Compute feature points by projecting the known points from the world using camPos, and
  // filtering by visibility. Invisible points are not added, but are used to keep the generated
  // descriptors coherent.
  void detectFeatures(const HMatrix &camPos, const Vector<HPoint3> &worldPts, FrameWithPoints *pts);

  // Construct feature points at the asserted image locations; points with a found-mask of false are
  // not added, but are used to keep the generated descriptors coherent.
  void detectFeatures(
    const Vector<HPoint2> &pixelPts, const Vector<uint8_t> &foundMask, FrameWithPoints *pts);

  // Disallow copying.
  FakeFeatureDetector(const FakeFeatureDetector &) = delete;
  FakeFeatureDetector &operator=(const FakeFeatureDetector &) = delete;

private:
  // Converts a uint32_t to an OrbFeature.
  OrbFeature generateDescriptor(uint32_t id);
  OrbFeature generateDescriptorForPoint(
    const HMatrix &cam, const HPoint3 &point, uint32_t pointIndex);

  // These are constants from the point of view of this class, although they may maintain internal
  // state.
  std::mt19937 rng_;
  std::uniform_int_distribution<uint8_t> uniform8u_;
  FakeFeatureDetectorNoise noise_;
};

}  // namespace c8
