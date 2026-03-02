// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)
//

#pragma once

#include "c8/hpoint.h"
#include "c8/hvector.h"
#include "c8/quaternion.h"

namespace c8 {

// Find the quaternion q that rotates src to be aligned with the direction of dest. Particularly,
// rotating src will not change its length, but its direction will be aligned with dest. I.e.
// (q * src).l2Norm() == src.l2Norm(), and (q * src).unit().dot(dest.unit()) == 1.
Quaternion rotationToVector(HVector3 src, HVector3 dest);

// Find the quaternion q that rotates a vector pointing forward along the z axis to be aligned with
// the direction of dest.
Quaternion rotationToVector(HVector3 dest);

// Finds the angular velocity that rotates start to end during timeStep.
// @param start the start rotation.
// @param end the end rotation.
// @param dt the time interval to rotate during.
// @returns angular velocity, which is axis angle (in radians) over time.
HVector3 angularVelocity(Quaternion start, Quaternion end, float dt);

// Computes the rotation delta over a time period travelling at an angular velocity.
// @param omega the angular velocity, which is axis angle (in radians) over time.
// @param dt the time interval to rotate during.
// @param rotationThreshold return identity if the L2 norm of the angular velocity is below this.
// @returns the rotation delta over time.
Quaternion rotationOverTime(const HVector3 &omega, float dt, float rotationThreshold = 0.f);

// Interpolates between two vectors given a provided interpolation value. If the interpolation is
// set to 0.f, then it will return the src. If the interpolation is set to 1.f, then it will return
// the dest.
HVector3 interpolate(const HVector3 &src, const HVector3 &dest, float t);

inline HPoint3 asPoint(HVector3 v) { return {v.x(), v.y(), v.z()}; }

inline HVector3 asVector(HPoint3 v) { return {v.x(), v.y(), v.z()}; }

inline HPoint2 asPoint(HVector2 v) { return {v.x(), v.y()}; }

inline HVector2 asVector(HPoint2 v) { return {v.x(), v.y()}; }

// Utility method for converting a vector of hpoints to hvectors. For performance reasons, do not
// use this method if you're simply going to operate on the vectors afterwards. Convert once, run
// multiple times. NOT, convert then run.
inline Vector<HVector2> asVectors(const Vector<HPoint2> &points) {
  Vector<HVector2> vectors;
  vectors.reserve(points.size());
  std::transform(
    points.begin(),
    points.end(),
    std::back_inserter(vectors),
    [](const HPoint2 pt) -> HVector2 {
      return {pt.x(), pt.y()};
    });
  return vectors;
}

}  // namespace c8
