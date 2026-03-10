// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"
cc_library {
  hdrs = {
    "ray-point-filter.h",
  };
  deps = {
    "//c8:hpoint",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x1fab1048);

#include <cmath>

#include "reality/engine/tracking/ray-point-filter.h"

namespace c8 {
namespace {
#ifdef NDEBUG
constexpr static int debug_ = 0;
#else
constexpr static int debug_ = 1;
#endif
}

RayPointFilterConfig createRayPointFilterConfig(float minAlpha, float update90v, float vAlpha) {
  auto minCutoff = 1.0f / (1.0f / minAlpha - 1.0f);
  auto vScale = (1.0f / (1.0f / 0.9f - 1.0f) - minCutoff) / update90v;
  auto vBeta = 1.0f - vAlpha;

  return {minAlpha, update90v, vAlpha, minCutoff, vScale, vBeta};
}

RayPointFilter3::RayPointFilter3(const HPoint3 &pt, const RayPointFilterConfig &config)
    : config_(config) {
  filteredX_ = pt.x();
  filteredY_ = pt.y();
  filteredZ_ = pt.z();
  rawX_ = filteredX_;
  rawY_ = filteredY_;
  velocityX_ = 0.0f;
  velocityY_ = 0.0f;
}

HPoint3 RayPointFilter3::filter(const HPoint3 &pt) {
  auto x = pt.x();
  auto y = pt.y();

  // Get the delta in raw values and the filtered values. This allows low velocity but consistent
  // directional shifts to accumulate across frames.
  auto deltaX = x - filteredX_;
  auto deltaY = y - filteredY_;

  // Remember this frame's raw values for next time.
  rawX_ = x;
  rawY_ = y;

  // Update the velocity estimates.
  velocityX_ = config_.vAlpha * deltaX + config_.vBeta * velocityX_;
  velocityY_ = config_.vAlpha * deltaY + config_.vBeta * velocityY_;

  // Compute the update step based on velocity estimate. Use L1 norm for velocity instead of L2
  // because it's much cheaper.
  float jointVelocity = std::abs(velocityX_) + std::abs(velocityY_);
  float updateAlpha = 1.0f / (1.0f + 1.0f / (config_.minCutoff + config_.vScale * jointVelocity));
  float updateBeta = 1.0f - updateAlpha;

  // Compute the new filtered values.
  filteredX_ = updateAlpha * x + updateBeta * filteredX_;
  filteredY_ = updateAlpha * y + updateBeta * filteredY_;
  filteredZ_ = updateAlpha * pt.z() + updateBeta * filteredZ_;

  if (debug_) {
    debugData_ = {jointVelocity, updateAlpha, config_.vScale * jointVelocity};
  }


  return {filteredX_, filteredY_, filteredZ_};
}

RayPointFilter3a::RayPointFilter3a(const HPoint3 &pt, const RayPointFilterConfig &config)
    : config_(config) {
  filteredX_ = pt.x();
  filteredY_ = pt.y();
  filteredZ_ = pt.z();
  rawX_ = filteredX_;
  rawY_ = filteredY_;
  rawZ_ = filteredZ_;
  velocityX_ = 0.0f;
  velocityY_ = 0.0f;
  velocityZ_ = 0.0f;
}

HPoint3 RayPointFilter3a::filter(const HPoint3 &pt) {
  auto x = pt.x();
  auto y = pt.y();
  auto z = pt.z();

  // Get the delta in raw values and the filtered values. This allows low velocity but consistent
  // directional shifts to accumulate across frames.
  auto deltaX = x - filteredX_;
  auto deltaY = y - filteredY_;
  auto deltaZ = z - filteredZ_;

  // Remember this frame's raw values for next time.
  rawX_ = x;
  rawY_ = y;
  rawZ_ = z;

  // Update the velocity estimates.
  velocityX_ = config_.vAlpha * deltaX + config_.vBeta * velocityX_;
  velocityY_ = config_.vAlpha * deltaY + config_.vBeta * velocityY_;
  velocityZ_ = config_.vAlpha * deltaZ + config_.vBeta * velocityZ_;

  // Compute the update step based on velocity estimate. Use L1 norm for velocity instead of L2
  // because it's much cheaper.
  float jointVelocity = std::abs(velocityX_) + std::abs(velocityY_) + std::abs(velocityZ_);
  float updateAlpha = 1.0f / (1.0f + 1.0f / (config_.minCutoff + config_.vScale * jointVelocity));
  float updateBeta = 1.0f - updateAlpha;

  // Compute the new filtered values.
  filteredX_ = updateAlpha * x + updateBeta * filteredX_;
  filteredY_ = updateAlpha * y + updateBeta * filteredY_;
  filteredZ_ = updateAlpha * z + updateBeta * filteredZ_;

  if (debug_) {
    debugData_ = {jointVelocity, updateAlpha, config_.vScale * jointVelocity};
  }

  return {filteredX_, filteredY_, filteredZ_};
}

RayPointFilter2::RayPointFilter2(const HPoint2 &pt) : config_(DEFAULT_RAY_FILTER_CONFIG) {
  initialize(pt);
}

RayPointFilter2::RayPointFilter2(const HPoint2 &pt, const RayPointFilterConfig &config)
    : config_(config) {
  initialize(pt);
}

void RayPointFilter2::initialize(const HPoint2 &pt) {
  filteredX_ = pt.x();
  filteredY_ = pt.y();
  rawX_ = filteredX_;
  rawY_ = filteredY_;
  velocityX_ = 0.0f;
  velocityY_ = 0.0f;
}

HPoint2 RayPointFilter2::filter(const HPoint2 &pt) {
  auto x = pt.x();
  auto y = pt.y();

  // Get the delta in raw values and the filtered values. This allows low velocity but consistent
  // directional shifts to accumulate across frames.
  auto deltaX = x - filteredX_;
  auto deltaY = y - filteredY_;

  // Remember this frame's raw values for next time.
  rawX_ = x;
  rawY_ = y;

  // Update the velocity estimates.
  velocityX_ = config_.vAlpha * deltaX + config_.vBeta * velocityX_;
  velocityY_ = config_.vAlpha * deltaY + config_.vBeta * velocityY_;

  // Compute the update step based on velocity estimate. Use L1 norm for velocity instead of L2
  // because it's much cheaper.
  float updateAlpha = 1.0f
    / (1.0f
       + 1.0f
         / (config_.minCutoff + config_.vScale * (std::abs(velocityX_) + std::abs(velocityY_))));
  float updateBeta = 1.0f - updateAlpha;

  // Compute the new filtered values.
  filteredX_ = updateAlpha * x + updateBeta * filteredX_;
  filteredY_ = updateAlpha * y + updateBeta * filteredY_;

  return {filteredX_, filteredY_};
}

HPoint2 RayPointFilter2::rawPoint() const { return {rawX_, rawY_}; }

RayPointFilter1::RayPointFilter1(float val) : config_(DEFAULT_RAY_FILTER_CONFIG) {
  initialize(val);
}

RayPointFilter1::RayPointFilter1(float val, const RayPointFilterConfig &config) : config_(config) {
  initialize(val);
}

void RayPointFilter1::initialize(float val) {
  filtered_ = val;
  raw_ = filtered_;
  velocity_ = 0.0f;
}

float RayPointFilter1::filter(float val) {
  // Get the delta in raw values and the filtered values. This allows low velocity but consistent
  // directional shifts to accumulate across frames.
  auto delta = val - filtered_;

  // Remember this frame's raw values for next time.
  raw_ = val;

  // Update the velocity estimates.
  velocity_ = config_.vAlpha * delta + config_.vBeta * velocity_;

  // Compute the update step based on velocity estimate. Use L1 norm for velocity instead of L2
  // because it's much cheaper.
  float updateAlpha =
    1.0f / (1.0f + 1.0f / (config_.minCutoff + config_.vScale * std::abs(velocity_)));
  float updateBeta = 1.0f - updateAlpha;

  // Compute the new filtered values.
  filtered_ = updateAlpha * val + updateBeta * filtered_;

  return filtered_;
}

float RayPointFilter1::rawPoint() const { return raw_; }

}  // namespace c8
