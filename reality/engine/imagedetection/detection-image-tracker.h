// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "c8/hmatrix.h"
#include "c8/hpoint.h"
#include "c8/random-numbers.h"
#include "c8/vector.h"
#include "reality/engine/features/frame-point.h"
#include "reality/engine/imagedetection/detection-image.h"
#include "reality/engine/imagedetection/image-tracking-state.h"
#include "reality/engine/imagedetection/tracked-image.h"

using DetectionImageMap = TreeMap<String, DetectionImage>;
using DetectionImagePtrMap = TreeMap<String, DetectionImage *>;

namespace c8 {

static float constexpr SHADER_ROI_RADIUS_INCREASE = 0.04f;

class DetectionImageTracker {
public:
  DetectionImageTracker() = default;
  DetectionImageTracker &operator=(DetectionImageTracker &&) = default;
  DetectionImageTracker(DetectionImageTracker &&) = default;

  DetectionImageTracker &operator=(const DetectionImageTracker &) = delete;
  DetectionImageTracker(const DetectionImageTracker &) = delete;

  Vector<LocatedImage> locate(
    const FrameWithPoints &feats,
    const Vector<FrameWithPoints> &roiFeats,
    const HMatrix &tamExtrinsic,
    DetectionImageMap &imageTargets,
    RandomNumbers *random);

  int lastSeen(const String &name) {
    auto it = images_.find(name);
    return it != images_.end() ? it->second.lastSeen() : 10000;
  }

  // Debug API for visualization only.
  const Vector<uint8_t> &inliers(const String &name) const { return images_.at(name).inliers(); }
  const Vector<uint8_t> &globalInliers(const String &name) const {
    return images_.at(name).globalInliers();
  }
  const Vector<PointMatch> &globalMatches(const String &name) const {
    return images_.at(name).globalMatches();
  }
  const Vector<PointMatch> &localMatches(const String &name) const {
    return images_.at(name).localMatches();
  }
  const FrameWithPoints &featsRayInTargetView(const String &name) const {
    return images_.at(name).featsRayInTargetView();
  }
  const FrameWithPoints &featsRayInTargetViewPostFit(const String &name) const {
    return images_.at(name).featsRayInTargetViewPostFit();
  }
  float angleFromLastPose(const String &name) const { return images_.at(name).angleFromLastPose(); }

  void init() {
    images_.clear();
    roundRobinIterator_ = images_.begin();
    front_ = "";
  }

  void setTrackingDisabled(bool disabled) {
    disabled_ = disabled;
    if (disabled_) {
      init();
    }
  }

  const bool valid() const { return !disabled_; }

  const String &front() { return front_; }

  Vector<LocatedImage> locate(
    const FrameWithPoints &feats,
    const Vector<FrameWithPoints> &roiFeats,
    const HMatrix &tamExtrinsic,
    const DetectionImagePtrMap &imageTargets,
    RandomNumbers *random);

  // For test:
  TrackedImage track(
    const FrameWithPoints &feats,
    const Vector<FrameWithPoints> &roiFeats,
    DetectionImageMap &imageTargets,
    RandomNumbers *random);

  static LocatedImage locate(
    const TrackedImage &t, const DetectionImageMap &imageTargets, const HMatrix &camPos);

  static LocatedImage locate(
    const TrackedImage &t,
    const DetectionImageMap &imageTargets,
    const HMatrix &camPos,
    float unused);

private:
  TreeMap<String, ImageTrackingState> images_;
  String front_;
  bool disabled_ = false;
  // Make sure to reset this when imagesMap_ changes.
  typename TreeMap<String, ImageTrackingState>::iterator roundRobinIterator_;

  Vector<TrackedImage> track(
    const FrameWithPoints &feats,
    const Vector<FrameWithPoints> &roiFeats,
    const DetectionImagePtrMap &imageTargets,
    RandomNumbers *random,
    const HMatrix &tamExtrinsic);

  Vector<TrackedImage> track(
    const FrameWithPoints &feats,
    const Vector<FrameWithPoints> &roiFeats,
    DetectionImageMap &imageTargets,
    RandomNumbers *random,
    const HMatrix &tamExtrinsic);

  static LocatedImage locate(
    const TrackedImage &t, const DetectionImagePtrMap &imageTargets, const HMatrix &camPos);

  static LocatedImage locate(
    const HMatrix &pose,
    const DetectionImage &t,
    float scale,
    const ImageRoi &roi,
    const TrackedImage &targetSpace);
};

}  // namespace c8
