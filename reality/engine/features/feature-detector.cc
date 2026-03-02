// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "reality/engine/features/feature-detector.h"

#include "c8/parameter-data.h"
#include "c8/stats/scope-timer.h"
#include "c8/vector.h"

namespace c8 {

namespace {

struct Settings {
  bool useGravityOrb;  // Use gravity orb for feature detection.
};

const Settings &settings() {
  static int paramsVersion_ = -1;
  static Settings settings_;
  if (globalParams().version() == paramsVersion_) {
    return settings_;
  }
  settings_ = {
    globalParams().getOrSet("FeatureDetector.useGravityOrb", false),
  };
  paramsVersion_ = globalParams().version();
  return settings_;
}

// Threshold for FAST detector
static constexpr int FAST_THRESHOLD = 25;
static constexpr int FEATURES_PER_FRAME_CPU = 500;

}  // namespace

FeatureDetector::FeatureDetector()
    : feat_(
        Gr8Cpu::create(
          FEATURES_PER_FRAME_CPU, 1.44f, 4, 31, 0, 2, Gr8CpuInterface::FAST_SCORE, FAST_THRESHOLD)),
      glFeat_(Gr8Gl::create()) {}

void FeatureDetector::detectFeatures(YPlanePixels frame, FrameWithPoints *pts) {
  ScopeTimer t("features");

  auto feats = feat_.detectAndCompute(frame);

  {
    // copy output.
    size_t s = feats.size();
    pts->clear();
    pts->reserve(s);
    for (const auto &f : feats) {
      auto l = f.location();
      pts->addImagePixelPoint(
        HPoint2(l.pt.x, l.pt.y), l.scale, l.angle, l.gravityAngle, f.features().clone());
    }
  }
}

void FeatureDetector::detectFeatures(
  const Gr8Pyramid &pyramid,
  const DetectionConfig &config,
  FrameWithPoints *pts,
  Vector<FrameWithPoints> *rois) {
  ScopeTimer t("features");

  auto feats = glFeat_.detectAndCompute(pyramid, *pts, config);

  {
    // copy output.
    size_t s = feats.size();
    pts->clear();
    pts->reserve(s);
    for (const auto &f : feats) {
      auto l = f.location();
      pts->addImagePixelPoint(
        HPoint2(l.pt.x, l.pt.y), l.scale, l.angle, l.gravityAngle, f.features().clone());
    }
    pts->setExcludedEdgePixels(glFeat_.edgeThreshold());
  }

  // If rois is nullptr, skip detecting roi features.
  if (!rois) {
    return;
  }
  auto roiFeats = glFeat_.detectAndComputeRois(pyramid, *pts, config);

  rois->clear();
  for (const auto &imagePoints : roiFeats) {
    rois->emplace_back(pts->intrinsic());
    auto &framePoints = rois->back();
    framePoints.setRoi(pyramid.rois[rois->size() - 1].roi);
    framePoints.setXRDevicePose(pts->xrDevicePose());
    // copy output.
    size_t s = imagePoints.size();
    framePoints.clear();
    framePoints.reserve(s);
    for (const auto &f : imagePoints) {
      auto l = f.location();
      framePoints.addImagePixelPoint(
        HPoint2(l.pt.x, l.pt.y), l.scale, l.angle, l.gravityAngle, f.features().clone());
    }
    // NOTE(nb): The effective excluded pixels could be larger based on the ratio of the roi to the
    // full frame. Also, the ROI could bleed over the edge of the image, so the edge threshold may
    // not be valid anyway.
    framePoints.setExcludedEdgePixels(0);
  }
}

// definitions

}  // namespace c8
