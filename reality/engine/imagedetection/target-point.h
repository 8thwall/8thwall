// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include <algorithm>
#include <cfloat>

#include "c8/geometry/egomotion.h"
#include "c8/geometry/pixel-pinhole-camera-model.h"
#include "c8/hmatrix.h"
#include "c8/hpoint.h"
#include "c8/pixels/image-roi.h"
#include "c8/quaternion.h"
#include "c8/set.h"
#include "c8/vector.h"
#include "reality/engine/features/feature-manager.h"
#include "reality/engine/features/image-descriptor.h"

namespace c8 {

// TargetPoints are points as viewed by in an image (frame). TargetPoints are always represented in
// ray-space, not pixel-space. This removes the effect of any inter-frame variability in camera
// intrinsics, and generally simplifies math.
class TargetPoint {
public:
  // Initialize a frame point with purely two-d information.
  TargetPoint(HPoint2 position, uint8_t scaleIn, float angleIn, float gravityAngleIn) noexcept
      : scale_(scaleIn),
        angle_(angleIn),
        gravityAngle_(gravityAngleIn),
        x_(position.x()),
        y_(position.y()) {}

  ~TargetPoint() {}

  // Default move constructors.
  TargetPoint(TargetPoint &&) = default;
  TargetPoint &operator=(TargetPoint &&) = default;

  // Allow copying.
  TargetPoint(const TargetPoint &) = default;
  TargetPoint &operator=(const TargetPoint &) = default;

  // 2D position of the point.
  HPoint2 position() const { return HPoint2(x_, y_); }

  // Fast accessors for 2 d position.
  constexpr float x() const { return x_; }
  constexpr float y() const { return y_; }

  // Scale of the point.
  uint8_t scale() const { return scale_; };

  // Angle of the point.
  float angle() const { return angle_; };

  // Gravity angle of the point.
  float gravityAngle() const { return gravityAngle_; };

private:
  // The scale (octave) for the keypoint.
  uint8_t scale_;

  // The oriented angle for the keypoint.
  float angle_;
  // The angle of the 2d reprojection of the up-vector in world space
  float gravityAngle_;

  float x_;
  float y_;
};

// A frame with points is the collection of points that are visible from a frame. The frame can be
// captured (in pixels) or synthesized (e.g. as a view on a map). The points in a TargetWithPoints
// are always stored in ray-space, not pixel-space. This removes the effect of any inter-frame
// variability in camera intrinsics, and generally simplifies math. The frame itself stores the
// camera model needed to recover the pixel locations of points if needed.  This is generally not
// needed, except in debugging, to overlay detected points on a frame.
class TargetWithPoints {
public:
  TargetWithPoints(c8_PixelPinholeCameraModel intrinsic)
      : pts_(), intrinsic_(intrinsic), K_(HMatrixGen::intrinsic(intrinsic)) {}

  ~TargetWithPoints() {}

  // Default move constructors.
  TargetWithPoints(TargetWithPoints &&) = default;
  TargetWithPoints &operator=(TargetWithPoints &&) = default;

  // Disallow copying.
  TargetWithPoints(const TargetWithPoints &) = delete;
  TargetWithPoints &operator=(const TargetWithPoints &) = delete;

  // Add a point with location specified in pixels, typically from a feature detector. Internally
  // this is converted into a ray-based representation.
  void addImagePixelPoint(
    HPoint2 position, uint8_t scale, float angle, float gravityAngle, FeatureSet &&features) {
    pts_.emplace_back(undistort(position), scale, angle, gravityAngle);
    store_.append(std::move(features));
  }

  // Access to the intrinsic model.
  c8_PixelPinholeCameraModel intrinsic() const { return intrinsic_; };

  // Access to the Point vector.
  const Vector<TargetPoint> &points() const { return pts_; }
  const FeatureStore &store() const { return store_; }

  // Access to the Feature type.
  void setFeatureType(DescriptorType featureType) { featureType_ = featureType; }
  DescriptorType featureType() const { return featureType_; }

  void frameBounds(HPoint2 *lowerLeft, HPoint2 *upperRight) const {
    ::c8::frameBounds(intrinsic_, lowerLeft, upperRight);
  }

  void frameBoundsExcludingEdge(HPoint2 *lowerLeft, HPoint2 *upperRight) const {
    ::c8::frameBounds(intrinsic_, excludedEdgePixels_, lowerLeft, upperRight);
  }

  // Get points in image coordinates.
  Vector<HPoint2> pixels() const {
    Vector<HPoint2> retval;
    std::transform(
      pts_.begin(),
      pts_.end(),
      std::back_inserter(retval),
      [this](const TargetPoint &p) -> HPoint2 { return distort(p.position()); });
    return retval;
  }

  void reserve(size_t size) { pts_.reserve(size); }

  HPoint2 undistort(HPoint2 pt) const { return (K_.inv() * pt.extrude()).flatten(); }

  HPoint2 distort(HPoint2 pt) const { return (K_ * pt.extrude()).flatten(); }

  void setExcludedEdgePixels(int pix) { excludedEdgePixels_ = pix; }

private:
  // Feature points within the keyframe, stored in ray space.
  Vector<TargetPoint> pts_;
  FeatureStore store_;

  // Calibration parameters
  c8_PixelPinholeCameraModel intrinsic_;
  HMatrix K_;

  int excludedEdgePixels_ = 0;

  DescriptorType featureType_ = DescriptorType::ORB;  // Default to ORB features.
};

}  // namespace c8
