// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "tracker.h",
  };
  deps = {
    ":tracking-sensor-event",
    "//c8:hmatrix",
    "//c8:parameter-data",
    "//c8/geometry:device-pose",
    "//c8/io:capnp-messages",
    "//c8/geometry:homography",
    "//c8/pixels:base64",
    "//c8/pixels:pixel-buffer",
    "//c8/pixels:pixel-transforms",
    "//reality/engine/features:tracker-input",
    "//c8/pixels:pixels",
    "//c8/stats:scope-timer",
    "//reality/engine/api:reality.capnp-cc",
    "//reality/engine/api/request:precomputed.capnp-cc",
    "//reality/engine/api/response:features.capnp-cc",
    "//reality/engine/api/response:pose.capnp-cc",
    "//reality/engine/features:feature-detector",
    "//reality/engine/imagedetection:detection-image-tracker",
  };
}
cc_end(0xfe019cb5);

#include "c8/c8-log.h"
#include "c8/geometry/device-pose.h"
#include "c8/geometry/homography.h"
#include "c8/io/capnp-messages.h"
#include "c8/parameter-data.h"
#include "c8/pixels/base64.h"
#include "c8/pixels/pixel-transforms.h"
#include "c8/pixels/pixels.h"
#include "c8/stats/scope-timer.h"
#include "reality/engine/api/request/precomputed.capnp.h"
#include "reality/engine/tracking/tracker.h"

namespace c8 {

namespace {

#ifdef NDEBUG
constexpr static int debug_ = 0;
#else
constexpr static int debug_ = 1;
#endif

struct Settings {
  int trackingDescriptorType;
};

const Settings &settings() {
  static int paramsVersion_ = -1;
  static Settings settings_;
  if (globalParams().version() == paramsVersion_) {
    return settings_;
  }
  settings_ = {
    globalParams().getOrSet<int>("Tracker.trackingDescriptorType", DescriptorType::GORB),
  };
  paramsVersion_ = globalParams().version();
  return settings_;
}

}  // namespace

HMatrix Tracker::track(
  const TrackingSensorFrame &frame,
  const DetectionImagePtrMap &imageTargets,
  RandomNumbers *random,
  XrTrackingState::Builder status) {
  // Create a new FrameWithPoints from the current intrinsic matrix.
  pts_.reset(new TrackerInput(
    frame.intrinsic, xrRotationFromDeviceRotation(frame.devicePose), frame.timeNanos));
  auto trackerInput = pts_.get();

  // Extract Gr8 features.
  if (frame.pyramid.levels.size() == 0) {
    detector_.detectFeatures(frame.srcY, &(pts_->framePoints));
  } else {
    DetectionConfig detectionConfig;

    // Use the default descriptor type, unless using a VPS map.
    auto trackingDescriptorType = static_cast<DescriptorType>(settings().trackingDescriptorType);

    switch (trackingDescriptorType) {
      case DescriptorType::ORB:
        // If the map has ORB descriptor type, use ORB for localization and tracking.
        detectionConfig.allOrb = true;
        break;
      case DescriptorType::GORB:
        // If the map has GORB descriptor type, use GORB for localization and tracking.
        detectionConfig.allGorb = true;
        break;
      default:
        C8_THROW("[tracker] Invalid descriptor type for tracking: %d", trackingDescriptorType);
        break;
    }

    // Image Targets Descriptor types
    if (imageTargets.size() > 0) {
      detectionConfig.allOrb = true;
      detectionConfig.allGorb = true;
    }

    // ORB features for lowest 45degrees of rays to track the ground plane stably
    detectionConfig.gravityPortions[DescriptorType::ORB] =
      GravityPortion(GravityPortion::SEVENTH | GravityPortion::EIGHTH);

    detector_.detectFeatures(frame.pyramid, detectionConfig, &pts_->framePoints, &pts_->roiPoints);

    if (debug_) {
      C8Log(
        "[tracker] Detected %zu keypoints: %zu ORB, %zu GORB",
        pts_->framePoints.store().numKeypoints(),
        pts_->framePoints.store().numDescriptors<OrbFeature>(),
        pts_->framePoints.store().numDescriptors<GorbFeature>());
    }
  }

  // Locate image targets.
  if (!imageTargets.empty()) {
    locatedImages_ = imTracker_.locate(
      trackerInput->framePoints, trackerInput->roiPoints, tamExtrinsic_, imageTargets, random);
  }

  ++currentFrame_;
  return tamExtrinsic_;
}

}  // namespace c8
