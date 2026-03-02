// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Dat Chu (dat@8thwall.com)
//

#pragma once

#include "c8/hpoint.h"
#include "c8/hvector.h"
#include "c8/quaternion.h"

namespace c8 {

float lerp(float a, float b, float alpha);
HPoint2 lerp(const HPoint2 &p1, const HPoint2 &p2, float alpha);

// Returns the index of the greatest element in xs that is less than val. xs must be sorted.
// Example: xs: [1, 2]. val: 1.1 -> idx: 0. val: 1.6 -> idx: 0.
int lessThanIndexSorted(const Vector<float> &xs, float val);
// Returns the index of the closest element in xs to val. xs must be sorted.
// Example: xs: [1, 2]. val: 1.1 -> idx: 0. val: 1.6 -> idx: 1.
int nearestNeighbourIndexSorted(const Vector<float> &xs, float val);

// One-dimensional linear interpolation for discrete increasing data points.
// Example: xs: [1, 2, 3], ys: [1, 2, 3], newXys: [1.5, 2.5, 3.5]. Returns [1.5, 2.5, 3.5].
// NOTE: Can likely improve runtime by find the lower bound then lerp between it and the next
// element (if exists).
// @param xs The x-coordinates of the data points.
// @param ys The y-coordinates of the data points, should be the same length as xs.
// @param newXs The x-coordinates to evaluate the interpolated values at.
Vector<float> interp(
  const Vector<float> &xs,
  const Vector<float> &ys,
  const Vector<float> &newXs);
// Runs the one-dimensional interp() function on x, y, z of ys independently, then creates a series
// of new HVectors/HPoints from the result.
template <typename TypeWithXYZ>
Vector<TypeWithXYZ> interp(
  const Vector<float> &xs,
  const Vector<TypeWithXYZ> &ys,
  const Vector<float> &newXs) {
  Vector<float> x;
  Vector<float> y;
  Vector<float> z;
  for (const auto &elem : ys) {
    x.push_back(elem.x());
    y.push_back(elem.y());
    z.push_back(elem.z());
  }
  auto xRes = interp(xs, x, newXs);
  auto yRes = interp(xs, y, newXs);
  auto zRes = interp(xs, z, newXs);
  Vector<TypeWithXYZ> res;
  for (int i = 0; i < xRes.size(); ++i) {
    res.push_back({xRes[i], yRes[i], zRes[i]});
  }
  return res;
}
// Takes in a list of timestamps and quaternions, and interpolates them to the newXs. Note that xs
// must be in ascending order.
Vector<Quaternion> interp(
  const Vector<float> &xs, const Vector<Quaternion> &ys, const Vector<float> &newXs);

}  // namespace c8
