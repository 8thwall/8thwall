// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Paris Morgan (paris@8thwall.com)
//
// Functions to create a cubic spline and then evaluate it at different values of x.

#pragma once

#include "c8/hpoint.h"
#include "c8/hvector.h"
#include "c8/string/format.h"
namespace c8 {

// Holds a cubic of the form a + bx + cx^2 + dx^3 = y.
struct Cubic {
  // y = a + bx + cx^2 + dx^3
  float pos(float x) const {
    auto zeroedX = x - x_;
    return a_ + b_ * zeroedX + c_ * zeroedX * zeroedX + d_ * std::pow(zeroedX, 3.f);
  }

  // y' = b + 2cx + 3dx^2
  float vel(float x) const {
    auto zeroedX = x - x_;
    // return b + .5f * c * x + (1.f / 3) * d * x * x;
    return b_ + 2.f * c_ * zeroedX + 3.f * d_ * zeroedX * zeroedX;
  }

  // y" = 2c + 6dx
  float accel(float x) const {
    auto zeroedX = x - x_;
    return 2.f * c_ + 6.f * d_ * zeroedX;
  }

  float pos() const { return pos(x_); }
  float vel() const { return vel(x_); }
  float accel() const { return accel(x_); }

  String toString() const {
    return format("%.2fx^3 + %.2fx^2 + %.2fx + %.2f = y | x = %f", d_, c_, b_, a_, x_);
  }

  const float a_ = 0.f;
  const float b_ = 0.f;
  const float c_ = 0.f;
  const float d_ = 0.f;
  const float x_ = 0.f;
};

struct Spline3 {
  Vector<Cubic> x;
  Vector<Cubic> y;
  Vector<Cubic> z;
};

struct SplineEvaluation3 {
  Vector<float> xs;
  Vector<HPoint3> pos;
  Vector<HPoint3> vel;
  Vector<HPoint3> accel;
};

// Finds the spline bin which x belongs to.
// Example:
//  xs = [0, 1, 2, 3]
//  val: -1 -> 0, val: 0 -> 0, val: .9 -> 0, val: 1 -> 1 val: 4 -> 3
int splineBin(const Vector<float> &xs, float val);

// Computes a natural cubic spline. Based off of:
// https://en.wikipedia.org/w/index.php?title=Spline_%28mathematics%29&oldid=288288033#Algorithm_for_computing_natural_cubic_splines
Vector<Cubic> spline(const Vector<float> &xs, const Vector<float> &ys);
template <typename TypeWithXYZ>
Spline3 spline3(const Vector<float> &xs, const Vector<TypeWithXYZ> &ys) {
  Vector<float> x;
  Vector<float> y;
  Vector<float> z;
  x.reserve(ys.size());
  y.reserve(ys.size());
  z.reserve(ys.size());
  for (const auto &elem : ys) {
    x.push_back(elem.x());
    y.push_back(elem.y());
    z.push_back(elem.z());
  }
  auto xSpline = spline(xs, x);
  auto ySpline = spline(xs, y);
  auto zSpline = spline(xs, z);
  return {xSpline, ySpline, zSpline};
}

// Evaluates a spline for pos, vel, and accel at the values of x which the spline was created from.
template <typename TypeWithXYZ>
SplineEvaluation3 evalSpline3(const Spline3 &spline) {
  if (spline.x.empty()) {
    return {};
  }
  auto n = spline.x.size();
  SplineEvaluation3 res;
  res.xs.reserve(n);
  res.pos.reserve(n);
  res.vel.reserve(n);
  res.accel.reserve(n);
  for (int i = 0; i < n; ++i) {
    res.xs.push_back(spline.x[i].x_);
    res.pos.push_back({spline.x[i].pos(), spline.y[i].pos(), spline.z[i].pos()});
    res.vel.push_back({spline.x[i].vel(), spline.y[i].vel(), spline.z[i].vel()});
    res.accel.push_back({spline.x[i].accel(), spline.y[i].accel(), spline.z[i].accel()});
  }

  return res;
}

// Evaluates a spline for pos, vel, and accel at the input newXs.
template <typename TypeWithXYZ>
SplineEvaluation3 evalSpline3(const Spline3 &spline, const Vector<float> &newXs) {
  if (spline.x.empty() || newXs.empty()) {
    return {};
  }

  // Get xs from the original spline.
  Vector<float> xs;
  xs.reserve(spline.x.size());
  for (auto cubic : spline.x) {
    xs.push_back(cubic.x_);
  }

  auto n = newXs.size();
  SplineEvaluation3 res;
  res.pos.reserve(n);
  res.vel.reserve(n);
  res.accel.reserve(n);
  for (int i = 0; i < n; ++i) {
    // Find the cubic that x falls within and evaluate the cubic at x for pos, vel, and accel.
    auto x = newXs[i];
    auto idx = splineBin(xs, x);
    res.pos.push_back({spline.x[idx].pos(x), spline.y[idx].pos(x), spline.z[idx].pos(x)});
    res.vel.push_back({spline.x[idx].vel(x), spline.y[idx].vel(x), spline.z[idx].vel(x)});
    res.accel.push_back({spline.x[idx].accel(x), spline.y[idx].accel(x), spline.z[idx].accel(x)});
  }
  return res;
}
}  // namespace c8
