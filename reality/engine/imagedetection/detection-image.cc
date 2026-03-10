// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "detection-image.h",
  };
  deps = {
    ":detection-image-local-matcher",
    ":detection-image-matcher",
    ":target-point",
    "//c8:c8-log",
    "//c8/geometry:parameterized-geometry",
    "//reality/engine/api:reality.capnp-cc",
  };
  copts = {
    "-D_USE_MATH_DEFINES",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x16e1384f);

#include <cmath>

#include "c8/c8-log.h"
#include "reality/engine/imagedetection/detection-image.h"

namespace c8 {

const uint8_t DetectionImage::DESCRIPTOR_DISTANCE_THRESHOLD_ORB = 48;
const uint8_t DetectionImage::DESCRIPTOR_DISTANCE_THRESHOLD_GORB = 24;

DetectionImage::DetectionImage(
  String name, TargetWithPoints &&framePoints, int rotation, const PlanarImageGeometry &geom)
    : name_(name),
      framePoints_(std::move(framePoints)),
      planarGeom_(geom),
      type_(DetectionImageType::PLANAR),
      rotation_(rotation) {
  prepareMatcher();
}

DetectionImage::DetectionImage(
  String name,
  TargetWithPoints &&framePoints,
  int rotation,
  const CurvyImageGeometry &geom,
  const CurvyImageGeometry &fullGeom,
  const HMatrix &fullLabelToCroppedLabelPose)
    : name_(name),
      framePoints_(std::move(framePoints)),
      fullLabelToCroppedLabelPose_(fullLabelToCroppedLabelPose),
      curvyGeom_(geom),
      curvyFullGeom_(fullGeom),
      type_(DetectionImageType::CURVY),
      rotation_(rotation) {
  prepareMatcher();
}

void DetectionImage::prepareMatcher() {
  ScopeTimer t("detection-image-prepare-matcher");
  uint8_t descriptorDistanceThreshold = 0;
  if (framePoints_.featureType() == DescriptorType::ORB) {
    descriptorDistanceThreshold = DESCRIPTOR_DISTANCE_THRESHOLD_ORB;
  } else {
    descriptorDistanceThreshold = DESCRIPTOR_DISTANCE_THRESHOLD_GORB;
  }
  matcher_.prepare(framePoints_, descriptorDistanceThreshold);
  localMatcher_.setQueryPointsPointer(framePoints_);
}

void DetectionImage::toCurvyGeometry(CurvyGeometry::Builder b) const {
  float circumferenceTop = 2.0f * M_PI * curvyFullGeom_.radius;
  float circumferenceBottom = 2.0f * M_PI * curvyFullGeom_.radiusBottom;
  b.setCurvyCircumferenceTop(circumferenceTop);
  b.setCurvyCircumferenceBottom(circumferenceBottom);

  // Since we track based on the cropped curvy (same radius, cropped width & height), we need
  // to extend the return height
  auto &activationRegion = curvyFullGeom_.activationRegion;
  float activationWidth = activationRegion.right - activationRegion.left;
  float activationHeight = activationRegion.bottom - activationRegion.top;
  float curvyHeight = curvyFullGeom_.height / activationHeight;
  float sideLength = std::sqrt(
    std::pow(curvyFullGeom_.radius - curvyFullGeom_.radiusBottom, 2) + std::pow(curvyHeight, 2));
  b.setCurvySideLength(sideLength);
  b.setHeight(curvyHeight);

  b.setTargetCircumferenceTop(circumferenceTop * activationWidth);

  b.setTopRadius(curvyFullGeom_.radius);
  b.setBottomRadius(curvyFullGeom_.radiusBottom);

  b.setArcLengthRadians(M_PI * 2.0f * activationWidth);
  b.setArcStartRadians(M_PI * 2.0f * activationRegion.left);
}

c8::String DetectionImage::toString() const noexcept {
  return c8::format(
    "(name: %s, type: %s, rot: %d)",
    name_.c_str(),
    type_ == DetectionImageType::CURVY ? "Curvy" : "Planar",
    rotation_);
}

}  // namespace c8
