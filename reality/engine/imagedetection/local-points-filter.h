// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Dat Chu (dat@8thwall.com)

#pragma once

#include <memory>

#include "c8/map.h"
#include "c8/set.h"
#include "c8/vector.h"
#include "reality/engine/features/frame-point.h"
#include "reality/engine/imagedetection/detection-image.h"
#include "reality/engine/tracking/ray-point-filter.h"

namespace c8 {

// A bank of filters for smoothing updates to points that you have seen during local tracking
//
// You would use a filter like this right before feeding the data into solvePnP (cylinder)
// or solveImageTargetHomography (planar). Example:
//   LocalPointsFilter localMatchFilter_;
//   localMatchFilter_.apply(sc_->localMatches_, &sc_->camRays_);
//   localMatchFilter_.updateUnseenPlanar(im, *nextPose);
//.  if (solveImageTargetHomography(
//     sc_->imTargetRays_,
// .   sc_->camRays_,
//     weights,
//     {10, 20, 500e-6f, 1e-2f, 0.0f}, // make sure to set cameraMotionWeight to 0
//     nextPose,
//     &sc_->localMatchPoseInliers_,
//     &sc_->scratch_,
//     lc)) {
//     *found = true;
//     return;
//    }
class LocalPointsFilter {
public:
  // Default constructor.
  LocalPointsFilter() = default;

  // reset the filter banks when we lost tracking
  void reset();

  // Default move constructors.
  LocalPointsFilter(LocalPointsFilter &&) = default;
  LocalPointsFilter &operator=(LocalPointsFilter &&) = default;

  // Disallow copying.
  LocalPointsFilter(const LocalPointsFilter &) = delete;
  LocalPointsFilter &operator=(const LocalPointsFilter &) = delete;

  void apply(const Vector<PointMatch> &matches, Vector<HPoint2> *camRays);
  void updateUnseenObject(
    const DetectionImage &im, const FrameWithPoints &searchFrame, const HMatrix &foundPose);
  void updateUnseenPlanar(const DetectionImage &im, const HMatrix &foundPose);

private:
  TreeMap<size_t, RayPointFilter2> ptIdToFilters_;
  TreeSet<size_t> unseenIds_;
};

}  // namespace c8
