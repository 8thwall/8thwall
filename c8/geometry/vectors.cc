// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"vectors.h"};
  deps = {
    "//c8:hpoint",
    "//c8:hvector",
    "//c8:quaternion",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x0ef4a334);

#include "c8/geometry/vectors.h"

namespace c8 {

Quaternion rotationToVector(HVector3 src, HVector3 dest) {
  // If vectors are degenerate, don't rotate.
  if (src.l2Norm() < 1e-10f || dest.l2Norm() < 1e-10f) {
    return {1.0f, 0.0f, 0.0f, 0.0f};
  }
  auto srcUnit = src.unit();
  auto destUnit = dest.unit();
  auto dot = srcUnit.dot(destUnit);
  // If vectors are 180 degrees, the solution below is degenerate. Pick an arbitrary second vector
  // to rotate through. We rotate through the x-axis unless the src is x-axis aligned, and then we
  // rotate through the y-axis.
  if (std::abs(dot + 1.0f) < 1e-5f) {
    auto xAxis = HVector3{1.0f, 0.0f, 0.0f};
    auto yAxis = HVector3{0.0f, 1.0f, 0.0f};
    auto intermediate = std::abs(srcUnit.dot(xAxis)) > (1.0f - 1e-5f) ? yAxis : xAxis;
    return rotationToVector(src, intermediate).times(rotationToVector(intermediate, dest));
  }
  auto cross = srcUnit.cross(destUnit);
  return Quaternion{
    1.0f + dot,
    cross.x(),
    cross.y(),
    cross.z(),
  }.normalize();
}

Quaternion rotationToVector(HVector3 dest) { return rotationToVector({0.f, 0.f, 1.f}, dest); }

HVector3 angularVelocity(Quaternion start, Quaternion end, float dt) {
  // Two rotations are similar but with different signs, so negate one. The negation of a quaternion
  // represents the same rotation.
  if ((start.w() < 0) != (end.w() < 0)) {
    end = end.negate();
  }
  // Find the rotation we need to make, convert to axis angle, and then divide by the time step to
  // get rotation over time.
  return start.delta(end).toAxisAngle() * (1.f / dt);
}

Quaternion rotationOverTime(const HVector3 &omega, float dt, float rotationThreshold) {
  auto magnitude = omega.l2Norm();
  if (magnitude > rotationThreshold) {
    auto halfTheta = dt * magnitude / 2.f;
    auto sinHalfTheta = std::sin(halfTheta);
    return Quaternion(
      std::cos(halfTheta),
      omega.x() / magnitude * sinHalfTheta,
      omega.y() / magnitude * sinHalfTheta,
      omega.z() / magnitude * sinHalfTheta);
  }
  return {};
}

HVector3 interpolate(const HVector3 &src, const HVector3 &dest, float t) {
  return src + t * (dest - src);
}
}  // namespace c8
