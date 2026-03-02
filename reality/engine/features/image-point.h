// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "c8/string.h"
#include "c8/vector.h"
#include "reality/engine/features/detection-config.h"
#include "reality/engine/features/feature-manager.h"
#include "reality/engine/features/image-descriptor.h"

namespace c8 {

constexpr size_t MAX_POINTS_PER_IMAGE = 2500;

namespace {
constexpr float PORTION_8_BY_180 = 8.0f / 180.0f;
}

// Note: Input must be between 0 and 180 inclusive.
constexpr GravityPortion angleWithUpToPortion(float degs) {
  // Clamp the input between 0 and just below 180 to avoid index 8
  degs = degs < 0.0f ? 0.0f : (degs >= 180.0f ? 179.99f : degs);
  return static_cast<GravityPortion>(1 << static_cast<uint8_t>(degs * PORTION_8_BY_180));
}

struct ImagePointXY {
public:
  ImagePointXY() = default;
  ImagePointXY(float x_, float y_) : x(x_), y(y_) {}
  float x = 0.0f;
  float y = 0.0f;

  bool operator==(ImagePointXY b) const { return x == b.x && y == b.y; }
  ImagePointXY &operator*=(float scale) {
    x *= scale;
    y *= scale;
    return *this;
  }
};

struct ImagePointLocation {
public:
  ImagePointXY pt;
  float size = 0.0f;
  uint8_t scale = 0;
  float angle = 0.0f;
  float gravityAngle = 0.0f;
  float response = 0.0f;
  int8_t roi = -1;
  GravityPortion gravityPortion = GravityPortion::NONE;

  ImagePointLocation() = default;
  ImagePointLocation(
    float x,
    float y,
    float size_,
    uint8_t scale_,
    float angle_,
    float gravityAngle_,
    float response_,
    int8_t roi_,
    GravityPortion gravityPortion_ = GravityPortion::NONE)
      : pt(x, y),
        size(size_),
        scale(scale_),
        angle(angle_),
        gravityAngle(gravityAngle_),
        response(response_),
        roi(roi_),
        gravityPortion(gravityPortion_) {}

  bool operator==(ImagePointLocation b) const {
    return pt == b.pt && size == b.size && scale == b.scale && angle == b.angle
      && gravityAngle == b.gravityAngle && response == b.response;
  }

  String toString() const noexcept;
};

class ImagePoint {
public:
  ImagePoint() = default;
  ImagePoint(ImagePointLocation location) : location_(location) {}

  // Default move constructors.
  ImagePoint(ImagePoint &&) = default;
  ImagePoint &operator=(ImagePoint &&) = default;

  // Disallow copying.
  ImagePoint(const ImagePoint &) = delete;
  ImagePoint &operator=(const ImagePoint &) = delete;

  ImagePointLocation location() const { return location_; }
  void setLocation(ImagePointLocation location) { location_ = location; }

  FeatureSet &mutableFeatures() { return features_; }
  const FeatureSet &features() const { return features_; }

private:
  ImagePointLocation location_;
  FeatureSet features_;
};

class ImagePoints {
public:
  ImagePoints(size_t numPoints = MAX_POINTS_PER_IMAGE) { points_.reserve(numPoints); }
  // Default move constructors.
  ImagePoints(ImagePoints &&) = default;
  ImagePoints &operator=(ImagePoints &&) = default;

  // Disallow copying.
  ImagePoints(const ImagePoints &) = delete;
  ImagePoints &operator=(const ImagePoints &) = delete;

  size_t size() const { return points_.size(); }

  const ImagePoint *begin() const { return points_.data(); }
  const ImagePoint *end() const { return points_.data() + points_.size(); }

  ImagePoint *begin() { return points_.data(); }
  ImagePoint *end() { return points_.data() + points_.size(); }

  ImagePoint &at(size_t idx) { return points_[idx]; }
  const ImagePoint &at(size_t idx) const { return points_[idx]; }

  ImagePoint &operator[](size_t idx) { return at(idx); }
  const ImagePoint &operator[](size_t idx) const { return at(idx); }

  ImagePoint &back() { return at(size() - 1); }
  const ImagePoint &back() const { return at(size() - 1); }

  void push_back(ImagePointLocation l) {
    if (size() == MAX_POINTS_PER_IMAGE) {
      return;
    }
    points_.emplace_back(l);
  }

private:
  Vector<ImagePoint> points_;
};

}  // namespace c8
