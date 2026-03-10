// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "device-pose.h",
  };
  deps = {
    "//c8:hpoint",
    "//c8:hvector",
    "//c8:quaternion",
    "//c8/geometry:egomotion",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xc3281fd4);

#include "c8/geometry/device-pose.h"
#include "c8/geometry/egomotion.h"

namespace c8 {

namespace {

// Drag force which reduces the velocity by the given factor.
static const float DRAG_FORCE = 1.0f;

// Threshold acceleration that device must have to affect velocity.
static const float MIN_FORCE_THRESHOLD = 0.4f;

// Scale factor to convert meters to coordinate values.
static const float COORDINATE_SCALE_FACTOR = 5.0f;

static const bool DISABLE_IMU_POSITION = true;

float clampSmallAcceleration(float acceleration) {
  return (acceleration <= MIN_FORCE_THRESHOLD && acceleration >= -MIN_FORCE_THRESHOLD)
    ? 0
    : acceleration;
}

}  // namespace

bool isZero(Quaternion q) {
  return q.w() == 0.0f && q.x() == 0.0f && q.y() == 0.0f && q.z() == 0.0f;
}

Quaternion narFromDevice(Quaternion q) { return {q.w(), q.x(), -q.y(), -q.z()}; }
HVector3 narFromDevice(HVector3 p) { return {p.x(), -p.y(), -p.z()}; }
HMatrix narFromDevice(HVector3 p, Quaternion q) {
  return cameraMotion(narFromDevice(p), narFromDevice(q));
}
// Note that this transform is symmetric.
Quaternion deviceFromNar(Quaternion q) { return narFromDevice(q); }
HVector3 deviceFromNar(HVector3 p) { return narFromDevice(p); }
HMatrix deviceFromNar(HVector3 p, Quaternion q) { return narFromDevice(p, q); }

Quaternion narFromXr(Quaternion q) { return {q.w(), -q.x(), q.y(), -q.z()}; }
HVector3 narFromXr(HVector3 p) { return HVector3(p.x(), -p.y(), p.z()); }
HPoint3 narFromXr(HPoint3 p) { return HPoint3(p.x(), -p.y(), p.z()); }
HMatrix narFromXr(HVector3 p, Quaternion q) { return cameraMotion(narFromXr(p), narFromXr(q)); }
HMatrix narFromXr(const HMatrix &m) { return narFromXr(translation(m), rotation(m)); }
// Note that this transform is symmetric.
Quaternion xrFromNar(Quaternion q) { return narFromXr(q); }
HVector3 xrFromNar(HVector3 p) { return narFromXr(p); }
HPoint3 xrFromNar(HPoint3 p) { return narFromXr(p); }
HMatrix xrFromNar(HVector3 p, Quaternion q) { return narFromXr(p, q); }
HMatrix xrFromNar(const HMatrix &m) { return narFromXr(m); };

Quaternion deviceRotationFromXrRotation(Quaternion xrRotation) {
  // We need to swap y and z at this point to match unity.
  Quaternion q1(xrRotation.w(), xrRotation.x(), xrRotation.z(), xrRotation.y());
  return Quaternion(0.7071067811865476f, -0.7071067811865476f, 0.0f, 0.0f).times(q1).inverse();
}

Quaternion xrRotationFromDeviceRotation(Quaternion deviceRotation) {
  auto q1 = Quaternion(0.7071067811865476f, 0.7071067811865476f, 0.0f, 0.0f)
              .times(deviceRotation.inverse());

  // We need to swap y and z at this point to match unity.
  return Quaternion(q1.w(), q1.x(), q1.z(), q1.y());
}

HPoint3 xrPositionFromDevicePosition(
  float dt,
  HPoint3 imuAcceleration,
  Quaternion newRotation,
  HPoint3 lastPosition,
  HPoint3 lastVelocity) {

  if (DISABLE_IMU_POSITION) {
    return HPoint3(0.0f, 0.0f, 0.0f);
  }

  auto rotatedAcceleration = rotateAcceleration(imuAcceleration, newRotation);

  // Time deltas in seconds and seconds^2.
  float dt2 = 0.5f * dt * dt;

  float ax = rotatedAcceleration.x();
  float ay = rotatedAcceleration.y();
  float az = rotatedAcceleration.z();

  // Last known velocity.
  float lvx = lastVelocity.x();
  float lvy = lastVelocity.y();
  float lvz = lastVelocity.z();

  // Position displacement.
  float dx = ((lvx * dt) + (ax * dt2)) * COORDINATE_SCALE_FACTOR;
  float dy = ((lvy * dt) + (ay * dt2)) * COORDINATE_SCALE_FACTOR;
  float dz = ((lvz * dt) + (az * dt2)) * COORDINATE_SCALE_FACTOR;

  return HPoint3(lastPosition.x() + dx, lastPosition.y() + dy, lastPosition.z() + dz);
}

Quaternion xrRotationFromARKitRotation(Quaternion arkitRotation) {
  auto inPortrait = arkitRotation.toRotationMat() * HMatrixGen::rotationD(0.0f, 0.0f, -90.0f);

  // For some reason that I still don't quite understand, the result is rotated about the x-axis by
  // 180 degrees, and we need to rotate it back.
  // TODO(nb): find out why 180 degree x rotation is required.
  return Quaternion::fromHMatrix(HMatrixGen::rotationD(180.0f, 0.0f, 0.0f) * inPortrait);
}

Quaternion arkitRotationFromXRRotation(Quaternion xrRotation) {
  return Quaternion::fromHMatrix(
    HMatrixGen::rotationD(-180.0f, 0.0f, 0.0f) * xrRotation.toRotationMat()
    * HMatrixGen::rotationD(0.0f, 0.0f, 90.0f));
}

HPoint3 xrPositionFromARKitPosition(HPoint3 arkitPosition) {
  // TODO(nb): Figure out why -x is required here.
  return HPoint3(-arkitPosition.x(), arkitPosition.y(), arkitPosition.z());
}

HPoint3 arkitPositionFromXRPosition(HPoint3 xrPosition) {
  // TODO(nb): Figure out why -x is required here.
  return HPoint3(-xrPosition.x(), xrPosition.y(), xrPosition.z());
}

Quaternion xrRotationFromARCoreRotationWhileTracking(Quaternion arcoreRotation) {
  return Quaternion(
    arcoreRotation.w(), -arcoreRotation.x(), -arcoreRotation.y(), arcoreRotation.z());
}

HPoint3 xrPositionFromARCorePositionWhileTracking(HPoint3 arkitPosition) {
  return HPoint3(arkitPosition.x(), arkitPosition.y(), -arkitPosition.z());
}

Quaternion xrRotationFromARCoreRotationLostTracking(
  Quaternion imuRotation,
  Quaternion lastImuRotation,
  Quaternion lastRotation,
  Quaternion initialRotation) {
  // ARCore has lost tracking and is no longer providing rotation data. Let's calculate the new
  // rotation by taking the difference between the current gyro & previous frame gryo data, then
  // transforming the previous rotation by that delta.
  auto currentImuRot =
    Quaternion(imuRotation.w(), -imuRotation.x(), -imuRotation.y(), -imuRotation.z());
  auto lastImuRot = Quaternion(
    lastImuRotation.w(), -lastImuRotation.x(), -lastImuRotation.y(), -lastImuRotation.z());

  auto delta = lastImuRot.delta(currentImuRot);
  delta = Quaternion(delta.w(), delta.x(), delta.y(), -delta.z());

  return initialRotation.times(lastRotation.times(delta));
}

HVector3 xrPositionFromARCorePositionLostTracking(
  Quaternion initialOrientation, Quaternion lastRotation, HVector3 lastPosition) {
  // ARCore has lost tracking. Maintain previous location until it is able to re-gain tracking.
  auto lastMatrix = cameraMotion(lastPosition, lastRotation);
  auto correctedMatrix = initialOrientation.toRotationMat() * lastMatrix;
  return translation(correctedMatrix);
}

Quaternion xrRotationFromTangoRotation(Quaternion tangoPosition) {
  return xrRotationFromDeviceRotation(tangoPosition);
}

HPoint3 xrPositionFromTangoPosition(HPoint3 tangoPosition) {
  return HPoint3(tangoPosition.x(), tangoPosition.y(), tangoPosition.z());
}

// NOTE(Riyaan): Both portraitDeviceFromLandscapeMassf functions can be made more efficient
Quaternion portraitDeviceFromLandscapeMassf(Quaternion q) {
  return Quaternion::xDegrees(90.f).times(q).times(Quaternion::zDegrees(90.f));
}

HMatrix portraitDeviceFromLandscapeMassf(HVector3 p, Quaternion q) {
  auto pose = cameraMotion(p, q);
  // clang-format off
  static const auto leftMat = HMatrix{{1.0f, 0.0f, 0.0f, 0.0f},
                                      {0.0f, 0.0f, -1.0f, 0.0f},
                                      {0.0f, 1.0f, 0.0f, 0.0f},
                                      {0.0f, 0.0f, 0.0f, 1.0f}};
  static const auto rightMat = HMatrix{{0.0f, -1.0f, 0.0f, 0.0f},
                                       {1.0f, 0.0f, 0.0f, 0.0f},
                                       {0.0f, 0.0f, 1.0f, 0.0f},
                                       {0.0f, 0.0f, 0.0f, 1.0f}};
  // clang-format on
  return leftMat * pose * rightMat;
}

Quaternion rotationDelta(Quaternion q1, Quaternion q2) {
  if (isZero(q1) || isZero(q2)) {
    return Quaternion(1.0f, 0.0f, 0.0f, 0.0f);
  }

  return egomotion(q1, q2);
}

HVector3 positionDelta(HVector3 newPosition, HVector3 lastPosition) {
  return newPosition - lastPosition;
}

HPoint3 updateWorldPosition(
  HPoint3 worldAcceleration, HPoint3 lastVelocity, HPoint3 lastPosition, float dt) {
  // Time deltas in seconds and seconds^2.
  float dt2 = 0.5f * dt * dt;

  // Position displacement.
  float dx = (lastVelocity.x() * dt) + (worldAcceleration.x() * dt2);
  float dy = (lastVelocity.y() * dt) + (worldAcceleration.y() * dt2);
  float dz = (lastVelocity.z() * dt) + (worldAcceleration.z() * dt2);

  return HPoint3(lastPosition.x() + dx, lastPosition.y() + dy, lastPosition.z() + dz);
}

HPoint3 updateVelocity(
  HPoint3 imuAcceleration, Quaternion newRotation, HPoint3 lastVelocity, float dt) {
  auto rotatedAcceleration = rotateAcceleration(imuAcceleration, newRotation);
  return updateWorldVelocity(rotatedAcceleration, lastVelocity, dt, DRAG_FORCE);
}

HPoint3 updateWorldVelocity(
  HPoint3 worldAcceleration, HPoint3 lastVelocity, float dt, float dragForce) {
  float lvx = lastVelocity.x();
  float lvy = lastVelocity.y();
  float lvz = lastVelocity.z();
  float vx = (1 - dragForce) * lvx + (worldAcceleration.x() * dt);
  float vy = (1 - dragForce) * lvy + (worldAcceleration.y() * dt);
  float vz = (1 - dragForce) * lvz + (worldAcceleration.z() * dt);
  return HPoint3(vx, vy, vz);
}

HPoint3 rotateAcceleration(HPoint3 imuAcceleration, Quaternion newRotation) {
  // Current acceleration. Accelerometer z-axis coordinates are the inverse of unity z-axis ones.
  float ax = clampSmallAcceleration(imuAcceleration.x());
  float ay = clampSmallAcceleration(imuAcceleration.y());
  float az = -(clampSmallAcceleration(imuAcceleration.z()));

  // TODO(nb): should this be last rotation?
  HMatrix rotationMatrix = newRotation.toRotationMat();

  return rotationMatrix * HPoint3(ax, ay, az);
}

// See "midpoint algorithm" on https://www.compadre.org/PICUP/resources/Numerical-Integration
// We're given p_k+1 = p_k + v_k * dt + 0.5 * a_k * dt^2 - so then:
// a_k = (p_k+1 - p_k - v_k * dt) * 2 / dt^2
HVector3 accelMidpoint(HVector3 currVel, HVector3 currPos, HVector3 nextPos, float dt) {
  return (nextPos - currPos - currVel * dt) * (2.f / std::pow(dt, 2.f));
}

// See "Basic Störmer–Verlet" on https://en.wikipedia.org/wiki/Verlet_integration
// We're given p_k+1 = 2 * p_k - p_k-1 + a_k * dt^2 - so then:
// a_k = (p_k+1 - 2 * p_k + p_k-1) / dt^2
HVector3 accelBasicVerlet(HVector3 prevPos, HVector3 currPos, HVector3 nextPos, float dt) {
  return (nextPos - 2.f * currPos + prevPos) * (1.f / std::pow(dt, 2.f));
}

// See "Non-constant time differences" on https://en.wikipedia.org/wiki/Verlet_integration
// We're given p_k+1 = p_k + (p_k - p_k-1) * (dt_k / dt_k-1) + 0.5 * a_k * dt_k * (dt_k +_dt_k-1)
// a_k = (p_k+1 - p_k - (p_k - p_k-1) * (dt_k / dt_k-1)) / (0.5 * dt_k * (dt_k +_dt_k-1))
HVector3 accelVerlet(HVector3 prevPos, HVector3 currPos, HVector3 nextPos, float prevDt, float dt) {
  return (nextPos - currPos - (currPos - prevPos) * (dt / prevDt))
    * (1.f / (0.5f * dt * (dt + prevDt)));
}
}  // namespace c8
