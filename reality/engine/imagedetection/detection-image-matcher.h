// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include <algorithm>
#include <cfloat>

#include "c8/c8-log.h"
#include "c8/stats/scope-timer.h"
#include "c8/vector.h"
#include "reality/engine/features/frame-point.h"
#include "reality/engine/features/image-descriptor.h"
#include "reality/engine/imagedetection/target-point.h"
#include "third_party/cvlite/flann/lsh-index.h"

namespace c8 {

class DetectionImageMatcher {
public:
  typedef ::c8flann::HammingPopcount HammingDistance;

  DetectionImageMatcher() = default;

  DetectionImageMatcher(DetectionImageMatcher &&) = default;
  DetectionImageMatcher &operator=(DetectionImageMatcher &&) = default;

  DetectionImageMatcher(const DetectionImageMatcher &) = delete;
  DetectionImageMatcher &operator=(const DetectionImageMatcher &) = delete;

  void prepare(const TargetWithPoints &targetPoints, int distanceThreshold);

  void match(const FrameWithPoints &imagePoints, Vector<PointMatch> *matches);

  void match(
    const FrameWithPoints &imagePoints, int distanceThreshold, Vector<PointMatch> *matches);

  // Make sure the query points pointer is at the current location. These must be the points that
  // were originally set. This is a hacky way to recover from TargetWithPoints being moved.
  // TODO(nb): come up with a more robust solution here.
  void ensureQueryPointsPointer(const TargetWithPoints &pts);

private:
  std::unique_ptr<c8flann::LshIndex<HammingDistance>> matcher_;
  const TargetWithPoints *trainPts_ = nullptr;
  c8flann::Matrix<uint8_t> dataset_;
  int distanceThreshold_;
  DescriptorType featureType_ = DescriptorType::ORB;  // Default to ORB features.
};

}  // namespace c8
