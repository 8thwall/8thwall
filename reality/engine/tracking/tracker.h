// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "c8/hmatrix.h"
#include "c8/pixels/pixel-buffer.h"
#include "reality/engine/api/reality.capnp.h"
#include "reality/engine/api/response/features.capnp.h"
#include "reality/engine/api/response/pose.capnp.h"
#include "reality/engine/features/feature-detector.h"
#include "reality/engine/features/tracker-input.h"
#include "reality/engine/imagedetection/detection-image-tracker.h"
#include "reality/engine/tracking/tracking-sensor-event.h"

namespace c8 {

class Tracker {
public:
  // Construct a Tracker with a map that it will own.
  Tracker() = default;

  virtual ~Tracker() {}

  // Default move constructors.
  Tracker(Tracker &&) = default;
  Tracker &operator=(Tracker &&) = default;

  // Disallow copying.
  Tracker(const Tracker &) = delete;
  Tracker &operator=(const Tracker &) = delete;

  // Returns the camera extrinsic and performs image target tracking.
  HMatrix track(
    const TrackingSensorFrame &frame,
    const DetectionImagePtrMap &imageTargets,
    RandomNumbers *random,
    XrTrackingState::Builder status);

  // Returns the non yaw-corrected estimated extrinsic in responsive space.  The tamExtrinsic is
  // aligned with our map. You must run extrinsic() first if you
  // want the value to be from the current frame.
  HMatrix tamExtrinsic() const { return tamExtrinsic_; }

  // Toggle image target tracking off or on.
  void setImageTrackingDisabled(bool disableImageTracking) {
    imTracker_.setTrackingDisabled(disableImageTracking);
    if (disableImageTracking) {
      locatedImages_.clear();
    }
  }

  // Get the last extracted keypoints.
  FrameWithPoints *getLastKeypoints() const {
    return pts_ == nullptr ? nullptr : &(pts_->framePoints);
  }

  int64_t currentFrame() const { return currentFrame_; }

  DetectionImageTracker &imTracker() { return imTracker_; }

  const Vector<LocatedImage> &locatedImageTargets() const { return locatedImages_; };

private:
  int64_t currentFrame_ = -1ll;

  FeatureDetector detector_;
  std::unique_ptr<TrackerInput> pts_;
  // Our non yaw-corrected extrinsic in responsive space, which is based on the first frame's
  // relation to the original ground plane. In responsive space, the camera will always start at at
  // the origin and the ground plane will be at y=-1.
  // The starting direction of Tracker’s tamExtrinsic is determined by the deviceOrientation.
  //
  // What would it mean to correct the extrinsic with the yaw? It's the following:
  // The first frame's pose is determined by the device's provided orientation. We want to alter
  // the yaw of our tamExtrinsic so that we face directly forward in z on frame 1. If we're
  // estimating the scale, then we'll also want the first metric extrinsic to face forward, so the
  // yawOffset will have a new value then as well. In other words, we correct the yaw of the
  // tamExtrinsic to that it is facing forward using yawOffset_. Let’s say on frame 1 that the
  // tamExtrinsic has a yaw of +10°. In that case, our yawOffset_ would offset that by -10° to put
  // our responsiveExtrinsic to 0°. We want it to face forward in z, so that it can be easily
  // manipulated afterwards by orientation.cc’s recenterAndScale so that it will face the direction
  // specified by the user. Can use groundPlaneRotation(rotation(tamExtrinsic_)) to get the yaw of
  // the tamExtrinsic for correction if needed.
  HMatrix tamExtrinsic_ = HMatrixGen::i();

  DetectionImageTracker imTracker_;
  Vector<LocatedImage> locatedImages_;
};

}  // namespace c8
