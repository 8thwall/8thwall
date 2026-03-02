// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "c8/geometry/face-types.h"
#include "c8/hmatrix.h"
#include "reality/engine/tracking/ray-point-filter.h"

namespace c8 {

// A bank of filters for smoothing updates to the points on a face in rayspace.
class FacePointsFilter {
public:
  // Default constructor.
  FacePointsFilter() = default;

  // Default move constructors.
  FacePointsFilter(FacePointsFilter &&) = delete;
  FacePointsFilter &operator=(FacePointsFilter &&) = delete;

  // Disallow copying.
  FacePointsFilter(const FacePointsFilter &) = delete;
  FacePointsFilter &operator=(const FacePointsFilter &) = delete;

  // Update the ray space detect points in place to filtered values.
  void apply(DetectedPoints *raySpacePoints);

private:
  Vector<RayPointFilter3> filters_;
  const RayPointFilterConfig config_ = createRayPointFilterConfig();
};

// Stores the valid IPD estimates in order to output a single IPD estimate.
struct IPDState {
  bool needsMoreData = true;
  Vector<float> validIPDEstimates;
  float finalIPDEstimate = 0.0f;
};

// Stores data about a user's neutral state for emitting face gesture events.
struct FaceGestureState {
  bool determinedRestState = false;
  float restLeftMiddleEyebrowY = 0.0f;
  float restRightMiddleEyebrowY = 0.0f;
};

// FaceDetectorLocal provides an abstraction layer above the deep net model for analyzing a face
// in detail from a high res region of interest.
class TrackedFaceState {
public:
  // Default constructor.
  TrackedFaceState() = default;

  // Default move constructors.
  TrackedFaceState(TrackedFaceState &&) = delete;
  TrackedFaceState &operator=(TrackedFaceState &&) = delete;

  // Disallow copying.
  TrackedFaceState(const TrackedFaceState &) = delete;
  TrackedFaceState &operator=(const TrackedFaceState &) = delete;

  Face3d locateFace(const DetectedPoints &localFaceDetection);

  // Mark that a frame could have been tracked, so that framesSinceTracked can be updated, which
  // helps identify lost faces.
  void markFrame();

  Face3d::TrackingStatus status() const;

private:
  // TODO(nb): Track last mesh points for stability filter.
  int framesTracked_ = 0;
  int framesSinceLocated_ = 0;
  HMatrix localPose_ = HMatrixGen::i();
  FacePointsFilter filter_;
  TreeMap<int, IPDState> faceToIPDState_;
  TreeMap<int, FaceGestureState> faceToGestureState_;
};

}  // namespace c8
