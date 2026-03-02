// Copyright (c) 2019 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)
#pragma once

#include "c8/geometry/parameterized-geometry.h"
#include "reality/engine/api/reality.capnp.h"
#include "reality/engine/imagedetection/detection-image-local-matcher.h"
#include "reality/engine/imagedetection/detection-image-matcher.h"
#include "reality/engine/imagedetection/target-point.h"

namespace c8 {
enum class DetectionImageType {
  UNSPECIFIED = 0,
  PLANAR = 1,
  CURVY = 2,
};

class DetectionImage {
public:
  static const uint8_t DESCRIPTOR_DISTANCE_THRESHOLD_ORB;
  static const uint8_t DESCRIPTOR_DISTANCE_THRESHOLD_GORB;

  DetectionImage(
    String name, TargetWithPoints &&framePoints, int rotation, const PlanarImageGeometry &geom);
  DetectionImage(
    String name,
    TargetWithPoints &&framePoints,
    int rotation,
    const CurvyImageGeometry &geom,
    const CurvyImageGeometry &fullGeom,
    const HMatrix &fullLabelToCroppedLabelPose);

  DetectionImage() = default;
  DetectionImage(DetectionImage &&) = default;
  DetectionImage &operator=(DetectionImage &&) = default;

  DetectionImage(const DetectionImage &) = delete;
  DetectionImage &operator=(const DetectionImage &) = delete;

  TargetWithPoints &framePoints() { return framePoints_; }
  const TargetWithPoints &framePoints() const { return framePoints_; }

  DetectionImageMatcher &globalMatcher() {
    // This is a hacky way to correct for this object being moved.
    // TODO(nb): come up with a more robust solution here.
    matcher_.ensureQueryPointsPointer(framePoints_);
    return matcher_;
  }

  DetectionImageLocalMatcher &localMatcher() {
    // This is a hacky way to correct for this object being moved.
    // TODO(nb): come up with a more robust solution here.
    localMatcher_.ensureQueryPointsPointer(framePoints_);
    return localMatcher_;
  }

  int rotation() const { return rotation_; }
  const String &getName() const { return name_; }
  const DetectionImageType getType() const { return type_; }
  const CurvyImageGeometry getGeometry() const { return curvyGeom_; }
  const CurvyImageGeometry getFullGeometry() const { return curvyFullGeom_; }
  const PlanarImageGeometry getPlanarGeometry() const { return planarGeom_; }
  const HMatrix &getFullLabelToCroppedLabelPose() const { return fullLabelToCroppedLabelPose_; }
  void toCurvyGeometry(CurvyGeometry::Builder b) const;

  c8::String toString() const noexcept;

private:
  void prepareMatcher();
  String name_;
  DetectionImageLocalMatcher localMatcher_{30, 20, .1f};
  DetectionImageMatcher matcher_;
  TargetWithPoints framePoints_{{}};
  // the pose of the target (cropped) from the curvy object (full original)
  HMatrix fullLabelToCroppedLabelPose_ = HMatrixGen::i();
  PlanarImageGeometry planarGeom_;
  CurvyImageGeometry curvyGeom_;
  CurvyImageGeometry curvyFullGeom_;
  DetectionImageType type_;
  int rotation_ = 0;
};

}  // namespace c8
