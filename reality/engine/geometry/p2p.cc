// Copyright (c) 2024 Niantic, Inc.
// Original Author: Haomin Zhu (hzhu@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "p2p.h",
  };
  deps = {
    "//bzl/inliner:rules2",
    "//c8:hmatrix",
    "//c8:hpoint",
    "//c8:vector",
  };
}
cc_end(0x8a2ccc60);

#include "p2p.h"

constexpr float kFloatPrecision = 1e-5;

namespace c8 {

namespace {
struct SolutionScratch {
  float u1;
  float v1;
  float v2;
  float X1;
  float Y1;
  float Z1;
  float X2;
  float Y2;
  float Z2;
  float R11;
  float R12;
  float R13;
  float R21;
  float R22;
  float R23;
  float R31;
  float R32;
  float R33;
};

void recoverSolution(
  float q, const SolutionScratch &ss, const HMatrix &trackingPoseInv, HMatrix *worldToCam) {
  const float qq = q * q;
  const float onePlusQq = 1.f + qq;
  const float oneMinusQq = 1.f - qq;
  const float onePlusQqInv = 1.f / onePlusQq;
  const float negOnePlusQqInv = -onePlusQqInv;
  const float twoQ = 2.f * q;
  const float minusTwoQ = -twoQ;
  const float oneMinusQqDivOnePlusQq = oneMinusQq * onePlusQqInv;
  const float twoQDivOnePlusQq = twoQ * onePlusQqInv;
  const float minusTwoQDivOnePlusQq = minusTwoQ * onePlusQqInv;

  const float r11 = ss.R11 * oneMinusQq + twoQ * ss.R13;
  const float r21 = ss.R21 * oneMinusQq + twoQ * ss.R23;
  const float r31 = ss.R31 * oneMinusQq + twoQ * ss.R33;
  const float r12 = ss.R12 * onePlusQq;
  const float r22 = ss.R22 * onePlusQq;
  const float r32 = ss.R32 * onePlusQq;
  const float r13 = ss.R13 * oneMinusQq - twoQ * ss.R11;
  const float r23 = ss.R23 * oneMinusQq - twoQ * ss.R21;
  const float r33 = ss.R33 * oneMinusQq - twoQ * ss.R31;

  const HVector3 n = {
    ((-r21 + r31 * ss.v1) * ss.X1 + (-r22 + r32 * ss.v1) * ss.Y1 + (-r23 + r33 * ss.v1) * ss.Z1)
      * negOnePlusQqInv,
    ((+r11 - r31 * ss.u1) * ss.X1 + (+r12 - r32 * ss.u1) * ss.Y1 + (+r13 - r33 * ss.u1) * ss.Z1)
      * negOnePlusQqInv,
    ((-r21 + r31 * ss.v2) * ss.X2 + (-r22 + r32 * ss.v2) * ss.Y2 + (-r23 + r33 * ss.v2) * ss.Z2)
      * negOnePlusQqInv};

  const float v1MinusV2Inv = 1.f / (ss.v1 - ss.v2);
  const float v2MinusV1Inv = -v1MinusV2Inv;
  const HMatrix MInv = {
    {ss.u1 * v1MinusV2Inv, 1.f, ss.u1 * v2MinusV1Inv, 0.f},
    {ss.v2 * v1MinusV2Inv, 0.f, ss.v1 * v2MinusV1Inv, 0.f},
    {v1MinusV2Inv, 0.f, v2MinusV1Inv, 0.f},
    {0.f, 0.f, 0.f, 1.f}};
  const HVector3 t = MInv * n;

  // clang-format off
  const HMatrix R = {
    {oneMinusQqDivOnePlusQq, 0.f, minusTwoQDivOnePlusQq, 0.f},
    {0.f, 1.f, 0.f, 0.f},
    {twoQDivOnePlusQq, 0.f, oneMinusQqDivOnePlusQq, 0.f},
    {0.f, 0.f, 0.f, 1.f},
    {oneMinusQqDivOnePlusQq, 0.f, twoQDivOnePlusQq, 0.f},
    {0.f, 1.f, 0.f, 0.f},
    {minusTwoQDivOnePlusQq, 0.f, oneMinusQqDivOnePlusQq, 0.f},
    {0.f, 0.f, 0.f, 1.f}
  };
  // clang-format on

  *worldToCam = HMatrixGen::fromRotationAndTranslation(trackingPoseInv * R, t);
};
}  // namespace

bool p2pImpl(
  const HPoint3 &worldPt1,
  const HPoint3 &worldPt2,
  const HPoint2 &camRay1,
  const HPoint2 &camRay2,
  const HMatrix &trackingPose,
  HMatrix *worldToCam1,
  HMatrix *worldToCam2) {
  if (!worldToCam1 || !worldToCam2)
    return false;

  bool possibleToSolve = std::fabs(camRay1.x() - camRay2.x()) > kFloatPrecision
    && std::fabs(camRay1.y() - camRay2.y()) > kFloatPrecision;
  if (!possibleToSolve) {
    return false;
  }

  // trackingPoseInv is in gravity aligned coordinate system
  // map points worldPts should be in gravity aligned coordinate
  auto trackingPoseInv = trackingPose.inv();

  // *** Inputs ***
  // 2d and 3d points, 2 of each
  float X1, Y1, Z1, X2, Y2, Z2;
  float u1, v1, u2, v2;
  X1 = worldPt1.x();
  Y1 = worldPt1.y();
  Z1 = worldPt1.z();
  X2 = worldPt2.x();
  Y2 = worldPt2.y();
  Z2 = worldPt2.z();

  u1 = camRay1.x();
  v1 = camRay1.y();
  u2 = camRay2.x();
  v2 = camRay2.y();

  // rotation from tracking coordinates to camera
  float R11, R12, R13, R21, R22, R23, R31, R32, R33;
  R11 = trackingPoseInv(0, 0);
  R12 = trackingPoseInv(0, 1);
  R13 = trackingPoseInv(0, 2);
  R21 = trackingPoseInv(1, 0);
  R22 = trackingPoseInv(1, 1);
  R23 = trackingPoseInv(1, 2);
  R31 = trackingPoseInv(2, 0);
  R32 = trackingPoseInv(2, 1);
  R33 = trackingPoseInv(2, 2);

  // *** Polynomial formulation ***
  // intermediate results and variables
  float dx, dy, dz, du, dv;
  float dux, duy, duz, dvx, dvy, dvz;
  dx = X2 - X1;
  dy = Y2 - Y1;
  dz = Z2 - Z1;
  du = u2 - u1;
  dv = v2 - v1;
  dux = u2 * X2 - u1 * X1;
  duy = u2 * Y2 - u1 * Y1;
  duz = u2 * Z2 - u1 * Z1;
  dvx = v2 * X2 - v1 * X1;
  dvy = v2 * Y2 - v1 * Y1;
  dvz = v2 * Z2 - v1 * Z1;

  float B12, B11, B10, B22, B21, B20;
  B12 = dx * R21 - dvx * R31 - dy * R22 + dvy * R32 + dz * R23 - dvz * R33;
  B11 = 2.f * (-dx * R23 + dvx * R33 + dz * R21 - dvz * R31);
  B10 = -dx * R21 + dvx * R31 - dy * R22 + dvy * R32 - dz * R23 + dvz * R33;
  B22 = -dx * R11 + dux * R31 + dy * R12 - duy * R32 - dz * R13 + duz * R33;
  B21 = 2.f * (dx * R13 - dux * R33 - dz * R11 + duz * R31);
  B20 = dx * R11 - dux * R31 + dy * R12 - duy * R32 + dz * R13 - duz * R33;

  // coefficients of the quadratic function f(x) = ax^2 + bx + c
  float a, b, c;
  a = du * B12 + dv * B22;
  b = du * B11 + dv * B21;
  c = du * B10 + dv * B20;

  // delta to see if solution exists
  float delta = b * b - 4.f * a * c;

  if (delta < 0.f || std::fabs(a) < kFloatPrecision) {
    return false;
  }

  float sqrtDelta = std::sqrt(delta);

  // two roots to the quadratic function
  float invTwoA = 1.f / (2.f * a);
  float q1 = (-b + sqrtDelta) * invTwoA;
  float q2 = (-b - sqrtDelta) * invTwoA;

  SolutionScratch ss = {
    u1, v1, v2, X1, Y1, Z1, X2, Y2, Z2, R11, R12, R13, R21, R22, R23, R31, R32, R33};

  // function to recover 4 DoF pose from root
  recoverSolution(q1, ss, trackingPoseInv, worldToCam1);
  recoverSolution(q2, ss, trackingPoseInv, worldToCam2);

  return true;
}

}  // namespace c8
