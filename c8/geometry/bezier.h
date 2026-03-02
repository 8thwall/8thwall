// Copyright (c) 2024 Niantic, Inc.
// Original Author: Nicholas Butko (nbutko@nianticlabs.com)

#pragma once

#include <array>

namespace c8 {

struct ControlPoint {
  float time;
  float value;
};

// An animation curve is a set of two adjustable control points, with an implicit pair of control
// points at (0, 0) and (1, 1) on either side. The values for time and value should be between 0 and
// 1.
typedef std::array<ControlPoint, 2> AnimationCurve;

class BezierAnimation {
public:
  BezierAnimation() = default;
  BezierAnimation(const AnimationCurve &curve);

  // Default move and copy constructors.
  BezierAnimation(BezierAnimation &&) = default;
  BezierAnimation &operator=(BezierAnimation &&) = default;
  BezierAnimation(const BezierAnimation &) = default;
  BezierAnimation &operator=(const BezierAnimation &) = default;

  // Evaluate the curve at a given time between 0 and 1.
  float at(float time) const;

  static AnimationCurve DEFAULT_CURVE;
  static AnimationCurve EASE_IN_CURVE;
  static AnimationCurve EASE_IN_EASE_OUT_CURVE;
  static AnimationCurve EASE_OUT_CURVE;
  static AnimationCurve LINEAR_CURVE;
  static AnimationCurve STRONG_EASE_CURVE;

  static BezierAnimation defaultCurve() { return BezierAnimation(DEFAULT_CURVE); }
  static BezierAnimation easeIn() { return BezierAnimation(EASE_IN_CURVE); }
  static BezierAnimation easeInEaseOut() { return BezierAnimation(EASE_IN_EASE_OUT_CURVE); }
  static BezierAnimation easeOut() { return BezierAnimation(EASE_OUT_CURVE); }
  static BezierAnimation linear() { return BezierAnimation(LINEAR_CURVE); }
  static BezierAnimation strongEase() { return BezierAnimation(STRONG_EASE_CURVE); }

private:
  AnimationCurve curve_;

  // Constant expressions for the cubic bezier curve.
  double b_ = 0.0f;
  double c_ = 0.0f;
  double x0_ = 0.0f;
  double x1_ = 0.0f;
  double x2_ = 0.0f;
  double x3_ = 0.0f;
  double x4_ = 0.0f;
  // x5 depends on input.
  double x6_ = 0.0f;
  double x7_ = 0.0f;
  double x8_ = 0.0f;
  double x9_ = 0.0f;
  double x10_ = 0.0f;
};

template <typename T, size_t D>
class AnimationState {
public:
  AnimationState() = default;

  // Default move and copy constructors.
  AnimationState(AnimationState &&) = default;
  AnimationState &operator=(AnimationState &&) = default;
  AnimationState(const AnimationState &) = default;
  AnimationState &operator=(const AnimationState &) = default;

  void set(std::array<T, D> value) {
    current_ = value;
    wasJustSet_ = true;
    animating_ = false;
  }

  // Animate starting from the timepoint of the last call to tick.
  void animate(std::array<T, D> value, BezierAnimation curve, double durationMillis) {
    previous_ = current_;
    target_ = value;
    curve_ = curve;
    durationMillis_ = durationMillis;
    startMillis_ = tickMillis_;
    animating_ = true;
  }

  // Animate starting from the timepoint of the next call to tick.
  void animateDelayed(std::array<T, D> value, BezierAnimation curve, double durationMillis) {
    animate(value, curve, durationMillis);
    animateDelayed_ = true;
  }

  bool tick(double nowMillis) {
    if (animateDelayed_) {
      startMillis_ = nowMillis;
      animateDelayed_ = false;
    }
    bool wasJustSet = wasJustSet_;
    wasJustSet_ = false;
    tickMillis_ = nowMillis;

    if (!animating_) {
      return wasJustSet;
    }

    if (startMillis_ + durationMillis_ <= nowMillis) {
      animating_ = false;
      current_ = target_;
      return true;
    }

    double time = (nowMillis - startMillis_) / durationMillis_;
    float progress = curve_.at(time);

    for (int i = 0; i < previous_.size(); ++i) {
      current_[i] = previous_[i] + (target_[i] - previous_[i]) * progress;
    }

    return true;
  }

  std::array<T, D> get() const { return current_; }

private:
  std::array<T, D> previous_ = {};
  std::array<T, D> current_ = {};
  std::array<T, D> target_ = {};
  BezierAnimation curve_ = {};
  double tickMillis_ = 0.0;
  double startMillis_ = 0.0;
  double durationMillis_ = 0.0;
  bool animating_ = false;
  bool wasJustSet_ = false;
  bool animateDelayed_ = false;
};

}  // namespace c8
