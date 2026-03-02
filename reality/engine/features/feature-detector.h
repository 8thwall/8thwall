// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include <limits>

#include "c8/hpoint.h"
#include "c8/map.h"
#include "c8/pixels/gr8-pyramid.h"
#include "c8/pixels/pixels.h"
#include "c8/vector.h"
#include "reality/engine/features/frame-point.h"
#include "reality/engine/features/gr8cpu.h"
#include "reality/engine/features/gr8gl.h"
#include "reality/engine/features/image-descriptor.h"

namespace c8 {

class FeatureDetector {
public:
  // Default constructor.
  FeatureDetector();

  // Default move constructors.
  FeatureDetector(FeatureDetector &&) = default;
  FeatureDetector &operator=(FeatureDetector &&) = default;

  // CPU features.
  void detectFeatures(YPlanePixels frame, FrameWithPoints *pts);

  // GL features.
  // Detect the default number of ORB features `FEATURES_PER_FRAME_GL` for this frame.
  void detectFeatures(
    const Gr8Pyramid &pyramid, FrameWithPoints *pts, Vector<FrameWithPoints> *rois = nullptr) {
    detectFeatures(pyramid, DETECTION_CONFIG_ALL_ORB, pts, rois);
  };

  void detectFeatures(
    const Gr8Pyramid &pyramid,
    const DetectionConfig &config,
    FrameWithPoints *pts,
    Vector<FrameWithPoints> *rois = nullptr);

  // Detect `nFeatures` ORB features for this frame.
  void detectFeatures(
    const Gr8Pyramid &pyramid,
    int nFeatures,
    FrameWithPoints *pts,
    Vector<FrameWithPoints> *rois = nullptr) {
    DetectionConfig config = DETECTION_CONFIG_ALL_ORB;
    config.nKeypoints = nFeatures;
    detectFeatures(pyramid, config, pts, rois);
  };

  // Disallow copying.
  FeatureDetector(const FeatureDetector &) = delete;
  FeatureDetector &operator=(const FeatureDetector &) = delete;

private:
  // These are constants from the point of view of this class, although they may
  // maintain internal state.
  Gr8Cpu feat_;
  Gr8Gl glFeat_;
};

}  // namespace c8
