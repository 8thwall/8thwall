
// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "c8/hpoint.h"

namespace c8 {

// A struct that holds the hyperparameters for RayPointFilter. There can be multiple instances of
// RayPointFilter (478 for faces), so by using a shared struct we reduce memory overhead. Use
// createRayPointFilterConfig to create a RayPointFilterConfig.
struct RayPointFilterConfig {
  // The update rate at 0 velocity in (0, 1). Lower values are more stable in low motion.
  float minAlpha = 0.0f;
  // The velocity in ray space needed for 90% update, > 0. Lower values track motion more closely.
  float update90v = 0.0f;
  // The update rate for the velocity estimate in (0, 1). Lower values react less to motion.
  float vAlpha = 0.0f;

  // derived values, don't compute these yourself.  Use
  float minCutoff = 0.0f;
  float vScale = 0.0f;
  float vBeta = 0.0f;
};

// Method for easily creating a valid filter config.  The default parameters are tuned for face
// tracking.
RayPointFilterConfig createRayPointFilterConfig(
  float minAlpha = 0.01f, float update90v = 0.02f, float vAlpha = 0.90f);

const static RayPointFilterConfig DEFAULT_RAY_FILTER_CONFIG = createRayPointFilterConfig();

struct FilterDebugData {
  float jointVelocity;
  float updateAlpha;
  float scaledVelocity;
};

// A filter for a single point in ray space. Z velocity is not taken into account.
class RayPointFilter3 {
public:
  // Pass a reference to a shared config.
  RayPointFilter3(const HPoint3 &pt, const RayPointFilterConfig &config);
  HPoint3 filter(const HPoint3 &pt);

  // For Debug and Tuning
  FilterDebugData debugData() const { return debugData_; }
  RayPointFilterConfig config() const { return config_; }
private:
  float filteredX_ = 0.0f;  // filtered x values
  float filteredY_ = 0.0f;  // filtered y values
  float filteredZ_ = 0.0f;  // filtered z values
  float rawX_ = 0.0f;       // raw x values
  float rawY_ = 0.0f;       // raw y values
  float velocityX_ = 0.0f;  // filtered x velocities
  float velocityY_ = 0.0f;  // filtered y velocities
  // Reference to a shared config.
  const RayPointFilterConfig &config_;

  FilterDebugData debugData_;
};

// Same as RayPointFilter3, filters all 3 axes. Velocity considers all axes jointly.
class RayPointFilter3a {
public:
  // Pass a reference to a shared config.
  RayPointFilter3a(const HPoint3 &pt, const RayPointFilterConfig &config);
  HPoint3 filter(const HPoint3 &pt);

  // For Debug and Tuning
  FilterDebugData debugData() const { return debugData_; }
  RayPointFilterConfig config() const { return config_; }
private:
  float filteredX_ = 0.0f;  // filtered x values
  float filteredY_ = 0.0f;  // filtered y values
  float filteredZ_ = 0.0f;  // filtered z values
  float rawX_ = 0.0f;       // raw x values
  float rawY_ = 0.0f;       // raw y values
  float rawZ_ = 0.0f;       // raw z values
  float velocityX_ = 0.0f;  // filtered x velocities
  float velocityY_ = 0.0f;  // filtered y velocities
  float velocityZ_ = 0.0f;  // filtered y velocities
  // Reference to a shared config.
  const RayPointFilterConfig &config_;

  FilterDebugData debugData_;
};

// TODO(nathan): rewrite using templates and move into c8/filter.
class RayPointFilter2 {
public:
  RayPointFilter2(const HPoint2 &pt);
  RayPointFilter2(const HPoint2 &pt, const RayPointFilterConfig &config);
  void initialize(const HPoint2 &pt);
  HPoint2 filter(const HPoint2 &pt);
  HPoint2 rawPoint() const;

private:
  float filteredX_ = 0.0f;  // filtered x values
  float filteredY_ = 0.0f;  // filtered y values
  float rawX_ = 0.0f;       // raw x values
  float rawY_ = 0.0f;       // raw y values
  float velocityX_ = 0.0f;  // filtered x velocities
  float velocityY_ = 0.0f;  // filtered y velocities

  // Reference to a shared config.
  const RayPointFilterConfig &config_;
};

// TODO(nathan): rewrite using templates and move into c8/filter.
class RayPointFilter1 {
public:
  RayPointFilter1(float val);
  RayPointFilter1(float val, const RayPointFilterConfig &config);
  void initialize(float val);
  float filter(float val);
  float rawPoint() const;

private:
  float filtered_ = 0.0f;  // filtered value
  float raw_ = 0.0f;       // raw value
  float velocity_ = 0.0f;  // filtered velocity

  // Reference to a shared config.
  const RayPointFilterConfig &config_;
};

}  // namespace c8
