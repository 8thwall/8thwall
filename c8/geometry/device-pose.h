// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)
//
// Coordinate system converions and motion helper functions.

#pragma once

#include "c8/hpoint.h"
#include "c8/hvector.h"
#include "c8/quaternion.h"

namespace c8 {

bool isZero(Quaternion q);

// The device coordinate system is opengl right-handed: +x: right, +y: up, +z: towards.
// The NAR coordinate system is opencv right-handed: +x: right, +y: down, +z: away.
// https://<REMOVED_BEFORE_OPEN_SOURCING>.atlassian.net/wiki/spaces/AR/pages/1044873887/Coordinate+Systems+in+NAR+ARKit+ARCore+Unity
Quaternion narFromDevice(Quaternion q);
HVector3 narFromDevice(HVector3 p);
HMatrix narFromDevice(HVector3 p, Quaternion q);
Quaternion deviceFromNar(Quaternion q);
HVector3 deviceFromNar(HVector3 p);
HMatrix deviceFromNar(HVector3 p, Quaternion q);

// The Xr coordinate system is unity left-handed: +x: right, +y: up, +z: away.
// The NAR coordinate system is opencv right-handed: +x: right, +y: down, +z: away.
// https://<REMOVED_BEFORE_OPEN_SOURCING>.atlassian.net/wiki/spaces/AR/pages/1044873887/Coordinate+Systems+in+NAR+ARKit+ARCore+Unity
Quaternion narFromXr(Quaternion q);
HVector3 narFromXr(HVector3 p);
HPoint3 narFromXr(HPoint3 p);
HMatrix narFromXr(HVector3 p, Quaternion q);
HMatrix narFromXr(const HMatrix &m);

Quaternion xrFromNar(Quaternion q);
HVector3 xrFromNar(HVector3 p);
HPoint3 xrFromNar(HPoint3 p);
HMatrix xrFromNar(HVector3 p, Quaternion q);
HMatrix xrFromNar(const HMatrix &m);

Quaternion deviceRotationFromXrRotation(Quaternion xrRotation);
Quaternion xrRotationFromDeviceRotation(Quaternion deviceRotation);
HPoint3 xrPositionFromDevicePosition(
  float dt,
  HPoint3 imuAcceleration,
  Quaternion newRotation,
  HPoint3 lastPosition,
  HPoint3 lastVelocity);

Quaternion xrRotationFromARKitRotation(Quaternion arkitRotation);
Quaternion arkitRotationFromXRRotation(Quaternion arkitRotation);
HPoint3 xrPositionFromARKitPosition(HPoint3 arkitPosition);
HPoint3 arkitPositionFromXRPosition(HPoint3 xrPosition);

Quaternion xrRotationFromARCoreRotationWhileTracking(Quaternion arkitRotation);
HPoint3 xrPositionFromARCorePositionWhileTracking(HPoint3 arkitPosition);

Quaternion xrRotationFromARCoreRotationLostTracking(
  Quaternion imuRotation,
  Quaternion lastImuRotation,
  Quaternion lastRotation,
  Quaternion initialRotation);
HVector3 xrPositionFromARCorePositionLostTracking(
  Quaternion initialOrientation, Quaternion lastRotation, HVector3 lastPosition);

Quaternion xrRotationFromTangoRotation(Quaternion tangoRotation);
HPoint3 xrPositionFromTangoPosition(HPoint3 tangoPosition);

// Device and MASSF both use OpenGL, but have different conventions
// See https://<REMOVED_BEFORE_OPEN_SOURCING>.atlassian.net/wiki/spaces/AR/pages/1983774723/
Quaternion portraitDeviceFromLandscapeMassf(Quaternion q);
HMatrix portraitDeviceFromLandscapeMassf(HVector3 p, Quaternion q);

Quaternion rotationDelta(Quaternion q1, Quaternion q2);
HVector3 positionDelta(HVector3 newPosition, HVector3 lastPosition);
HPoint3 updateVelocity(
  HPoint3 imuAcceleration, Quaternion newRotation, HPoint3 lastVelocity, float dt);

// Update the velocity by the acceleration: new_v = velocity + dt * acceleration
HPoint3 updateWorldVelocity(HPoint3 acceleration, HPoint3 velocity, float dt, float dragForce);

// Update the position by the velocity and acceleration:
//   new_p = position + velocity * dt + 0.5 * dt * dt * acceleration
HPoint3 updateWorldPosition(
  HPoint3 worldAcceleration, HPoint3 lastVelocity, HPoint3 lastPosition, float dt);

HPoint3 rotateAcceleration(HPoint3 imuAcceleration, Quaternion newRotation);

// Calculate acceleration using the Midpoint algorithm.
// See "midpoint algorithm" on https://www.compadre.org/PICUP/resources/Numerical-Integration
// @param currVel the velocity at time k
// @param currPos the position at time k
// @param nextPos the position at time k+1
// @param dt the delta from k -> k+1
// @returns the acceleration at time k
HVector3 accelMidpoint(HVector3 currVel, HVector3 currPos, HVector3 nextPos, float dt);

// Calculate acceleration using the basic Stormet-Verlet method.
// See "Basic Störmer–Verlet" on https://en.wikipedia.org/wiki/Verlet_integration
// @param prevPos the position at time k-1
// @param currPos the position at time k
// @param nextPos the position at time k+1
// @param dt the delta from k-1 -> k, as well as k -> k+1
// @returns the acceleration at time k
HVector3 accelBasicVerlet(HVector3 prevPos, HVector3 currPos, HVector3 nextPos, float dt);

// Calculate acceleration using the Stormet-Verlet method which accounts for non-constant dts.
// See "Non-constant time differences" on https://en.wikipedia.org/wiki/Verlet_integration
// @param prevPos the position at time k-1
// @param currPos the position at time k
// @param nextPos the position at time k+1
// @param prevDt the delta from k-1 -> k
// @param dt the delta from k -> k+1
// @returns the acceleration at time k
HVector3 accelVerlet(HVector3 prevPos, HVector3 currPos, HVector3 nextPos, float prevDt, float dt);

}  // namespace c8
