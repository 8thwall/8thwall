// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "quaternion.h",
  };
  deps = {":hmatrix", ":string", "//c8:exceptions"};
}
cc_end(0x098c05bd);

#include <cmath>
#include <sstream>

#include "c8/quaternion.h"

#define PI_F 3.14159265f
#define RAD_TO_DEG 180.0f / PI_F
#define DEG_TO_RAD PI_F / 180.0f

namespace c8 {

namespace {

constexpr float clip(float x, float mn, float mx) { return (x < mn) ? mn : ((x > mx) ? mx : x); }

float dotQ(Quaternion lhs, Quaternion rhs) {
  return lhs.w() * rhs.w() + lhs.x() * rhs.x() + lhs.y() * rhs.y() + lhs.z() * rhs.z();
}

float pretty(float f) { return (std::abs(f) < 1e-6) ? 0.f : f; }

}  // namespace

// Extracts the upper left 3x3 rotation component of the HMatrix and respresents it as a
// quaternion.
Quaternion Quaternion::fromHMatrix(const HMatrix &R) noexcept {
#ifdef DEBUG

  auto xScale2 = R(0, 0) * R(0, 0) + R(1, 0) * R(1, 0) + R(2, 0) * R(2, 0);
  auto yScale2 = R(0, 1) * R(0, 1) + R(1, 1) * R(1, 1) + R(2, 1) * R(2, 1);
  auto zScale2 = R(0, 2) * R(0, 2) + R(1, 2) * R(1, 2) + R(2, 2) * R(2, 2);

  if (
    std::fabsf(xScale2 - 1.0f) > 1e-6 || std::fabsf(yScale2 - 1.0f) > 1e-6
    || std::fabsf(zScale2 - 1.0f) > 1e-6) {
    C8_THROW_INVALID_ARGUMENT("fromHMatrix only supports rotation + transform, use fromTrsMatrix");
  }

#endif

  double q[4] = {0.0, 0.0, 0.0, 0.0};
  double trace = R(0, 0) + R(1, 1) + R(2, 2);
  if (trace >= 0.0) {
    auto t = std::sqrt(trace + 1.0);
    q[0] = 0.5 * t;
    t = 0.5 / t;
    q[1] = (R(2, 1) - R(1, 2)) * t;
    q[2] = (R(0, 2) - R(2, 0)) * t;
    q[3] = (R(1, 0) - R(0, 1)) * t;
  } else {
    int i = R(0, 0) > R(1, 1) ? (R(0, 0) > R(2, 2) ? 0 : 2) : (R(1, 1) > R(2, 2) ? 1 : 2);
    int j = (i + 1) % 3;
    int k = (j + 1) % 3;
    auto t = std::sqrt(1.0 + R(i, i) - R(j, j) - R(k, k));
    q[i + 1] = 0.5 * t;
    t = 0.5 / t;
    q[0] = (R(k, j) - R(j, k)) * t;
    q[j + 1] = (R(j, i) + R(i, j)) * t;
    q[k + 1] = (R(k, i) + R(i, k)) * t;
  }

  return {
    static_cast<float>(q[0]),
    static_cast<float>(q[1]),
    static_cast<float>(q[2]),
    static_cast<float>(q[3])};
}

Quaternion Quaternion::fromTrsMatrix(const HMatrix &R) noexcept {
  /*
    | s1 s2 s3 _ |
    | s4 s5 s6 _ |
    | s7 s8 s9 _ |
    | _  _  _  _ |
  */

  const auto s1 = R(0, 0);
  const auto s2 = R(0, 1);
  const auto s3 = R(0, 2);
  const auto s4 = R(1, 0);
  const auto s5 = R(1, 1);
  const auto s6 = R(1, 2);
  const auto s7 = R(2, 0);
  const auto s8 = R(2, 1);
  const auto s9 = R(2, 2);

  // clang-format off
  const auto det3x3 = (s1 * s5 * s9) 
              - (s1 * s6 * s8) 
              - (s2 * s4 * s9) 
              + (s2 * s6 * s7) 
              + (s3 * s4 * s8)
              - (s3 * s5 * s7);
  // clang-format on

  const auto xScale2 = s1 * s1 + s4 * s4 + s7 * s7;
  const auto yScale2 = s2 * s2 + s5 * s5 + s8 * s8;
  const auto zScale2 = s3 * s3 + s6 * s6 + s9 * s9;

  const auto detSign = det3x3 < 0.0f ? -1.0f : 1.0f;

  const auto invXScale = detSign / std::sqrt(xScale2);
  const auto invYScale = 1.0f / std::sqrt(yScale2);
  const auto invZScale = 1.0f / std::sqrt(zScale2);

  const HMatrix scaleFreeMatrix(
    {{s1 * invXScale, s2 * invYScale, s3 * invZScale, 0.0f},
     {s4 * invXScale, s5 * invYScale, s6 * invZScale, 0.0f},
     {s7 * invXScale, s8 * invYScale, s9 * invZScale, 0.0f},
     {0.0f, 0.0f, 0.0f, 1.0f}});

  return fromHMatrix(scaleFreeMatrix);
}

// Constructs a quaternion rotation by first applying a rotation around the
// x-axis, followed by a rotation around the y-axis, followed by a rotation
// around the z-axis.
Quaternion Quaternion::fromEulerAngleDegrees(float xDeg, float yDeg, float zDeg) noexcept {
  return zDegrees(zDeg).times(yDegrees(yDeg)).times(xDegrees(xDeg));
}

// Constructs a quaternion rotation with the specified rotation.
Quaternion Quaternion::xDegrees(float val) noexcept { return xRadians(val * DEG_TO_RAD); }
Quaternion Quaternion::yDegrees(float val) noexcept { return yRadians(val * DEG_TO_RAD); }
Quaternion Quaternion::zDegrees(float val) noexcept { return zRadians(val * DEG_TO_RAD); }

Quaternion Quaternion::xRadians(float v) noexcept {
  float hv = v * 0.5f;
  return {std::cos(hv), std::sin(hv), 0.0f, 0.0f};
}

Quaternion Quaternion::yRadians(float v) noexcept {
  float hv = v * 0.5f;
  return {std::cos(hv), 0.0f, std::sin(hv), 0.0f};
}

Quaternion Quaternion::zRadians(float v) noexcept {
  float hv = v * 0.5f;
  return {std::cos(hv), 0.0f, 0.0f, std::sin(hv)};
}

// Constructs a Quaternion from an Axis-Angle representation.
Quaternion Quaternion::fromAxisAngle(const HVector3 &axisAngle) noexcept {
  const float &a0 = axisAngle.x();
  const float &a1 = axisAngle.y();
  const float &a2 = axisAngle.z();
  const float thetaSquared = a0 * a0 + a1 * a1 + a2 * a2;
  // For points not at the origin, the full conversion is numerically stable.
  if (thetaSquared > 0.0f) {
    const float theta = sqrt(thetaSquared);
    const float halfTheta = theta * 0.5f;
    const float k = sin(halfTheta) / theta;
    return {std::cos(halfTheta), a0 * k, a1 * k, a2 * k};
  }
  // If thetaSquared is 0, then we will get NaNs when dividing by theta.  By approximating with a
  // Taylor series, and truncating at one term, the value will be computed correctly.
  const float k(0.5);
  return {1.0f, a0 * k, a1 * k, a2 * k};
}

// Converts a 4x1 double quaternion column vector to a rotation-only HMatrix.
HMatrix Quaternion::toRotationMat() const noexcept {
  double wx = w_ * x_;
  double wy = w_ * y_;
  double wz = w_ * z_;
  double xx = x_ * x_;
  double xy = x_ * y_;
  double xz = x_ * z_;
  double yy = y_ * y_;
  double yz = y_ * z_;
  double zz = z_ * z_;

  float mm00 = 1.0 - 2.0 * (yy + zz);
  float mm01 = 2.0000000 * (xy - wz);
  float mm02 = 2.0000000 * (xz + wy);

  float mm10 = 2.0000000 * (xy + wz);
  float mm11 = 1.0 - 2.0 * (xx + zz);
  float mm12 = 2.0000000 * (yz - wx);

  float mm20 = 2.0000000 * (xz - wy);
  float mm21 = 2.0000000 * (yz + wx);
  float mm22 = 1.0 - 2.0 * (xx + yy);

  return HMatrix{
    {mm00, mm01, mm02, 0.0f},   // Row 0
    {mm10, mm11, mm12, 0.0f},   // Row 1
    {mm20, mm21, mm22, 0.0f},   // Row 2
    {0.0f, 0.0f, 0.0f, 1.0f},   // Row 3
    {mm00, mm10, mm20, 0.0f},   // IRow 0
    {mm01, mm11, mm21, 0.0f},   // IRow 1
    {mm02, mm12, mm22, 0.0f},   // IRow 2
    {0.0f, 0.0f, 0.0f, 1.0f}};  // IRow 3
}

// Converts a 4x1 quaternion vector to a 3x1 double euler angle column vector.
void Quaternion::toEulerAngleDegrees(float *xDeg, float *yDeg, float *zDeg) const noexcept {
  float wx = w_ * x_;
  float wy = w_ * y_;
  float wz = w_ * z_;
  float xx = x_ * x_;
  float xy = x_ * y_;
  float xz = x_ * z_;
  float yy = y_ * y_;
  float yz = y_ * z_;
  float zz = z_ * z_;

  *xDeg = std::atan2(2.0f * (wx + yz), 1.0f - 2.0f * (xx + yy)) * RAD_TO_DEG;
  *yDeg = std::asin(clip(2.0f * (wy - xz), -1.0f, 1.0f)) * RAD_TO_DEG;
  *zDeg = std::atan2(2.0f * (wz + xy), 1.0f - 2.0f * (yy + zz)) * RAD_TO_DEG;
}

HVector3 Quaternion::toEulerAngleDegrees() const noexcept {
  float x;
  float y;
  float z;
  toEulerAngleDegrees(&x, &y, &z);
  return {x, y, z};
}

Quaternion Quaternion::fromPitchYawRollRadians(float x, float y, float z) noexcept {
  return (yRadians(y).times(xRadians(x))).times(zRadians(z));
}

void Quaternion::toPitchYawRollRadians(float *x, float *y, float *z) const noexcept {
  // Convert quaternion to y-x-z euler angles.
  auto wx = w_ * x_;
  auto wy = w_ * y_;
  auto wz = w_ * z_;
  auto xx = x_ * x_;
  auto xy = x_ * y_;
  auto xz = x_ * z_;
  auto yy = y_ * y_;
  auto yz = y_ * z_;
  auto zz = z_ * z_;

  auto m00 = 1.0f - 2.0f * (yy + zz);
  auto m02 = 2.0f * (xz + wy);
  auto m10 = 2.0f * (xy + wz);
  auto m11 = 1.0f - 2.0f * (xx + zz);
  auto m12 = 2.0f * (yz - wx);
  auto m20 = 2.0f * (xz - wy);
  auto m22 = 1.0f - 2.0f * (xx + yy);

  *x = std::asin(std::max(-1.0f, std::min(-m12, 1.0f)));
  // If pitch is 1 or -1, the camera is facing straight up or straight down. In this plane, yaw
  // and roll are interchangeable and ambiguous. In this case, we resolve ties in favor of
  // yaw-only motion (no roll).
  auto fullPitch = std::abs(m12) < (1.0f - 1e-7f);
  *y = fullPitch ? std::atan2(m02, m22) : std::atan2(-m20, m00);
  *z = fullPitch ? std::atan2(m10, m11) : 0.0f;
}

float Quaternion::pitchRadians() const noexcept {
  auto wx = w_ * x_;
  auto yz = y_ * z_;

  auto m12 = 2.0f * (yz - wx);

  return std::asin(std::max(-1.0f, std::min(-m12, 1.0f)));
}
float Quaternion::yawRadians() const noexcept {
  auto wx = w_ * x_;
  auto wy = w_ * y_;
  auto xx = x_ * x_;
  auto xz = x_ * z_;
  auto yy = y_ * y_;
  auto yz = y_ * z_;
  auto zz = z_ * z_;

  auto m00 = 1.0f - 2.0f * (yy + zz);
  auto m02 = 2.0f * (xz + wy);
  auto m12 = 2.0f * (yz - wx);
  auto m20 = 2.0f * (xz - wy);
  auto m22 = 1.0f - 2.0f * (xx + yy);

  // If pitch is 1 or -1, the camera is facing straight up or straight down. In this plane, yaw
  // and roll are interchangeable and ambiguous. In this case, we resolve ties in favor of
  // yaw-only motion (no roll).
  auto fullPitch = std::abs(m12) < (1.0f - 1e-7f);
  return fullPitch ? std::atan2(m02, m22) : std::atan2(-m20, m00);
}

float Quaternion::rollRadians() const noexcept {
  auto wx = w_ * x_;
  auto wz = w_ * z_;
  auto xx = x_ * x_;
  auto xy = x_ * y_;
  auto yz = y_ * z_;
  auto zz = z_ * z_;

  auto m10 = 2.0f * (xy + wz);
  auto m11 = 1.0f - 2.0f * (xx + zz);
  auto m12 = 2.0f * (yz - wx);

  // If pitch is 1 or -1, the camera is facing straight up or straight down. In this plane, yaw
  // and roll are interchangeable and ambiguous. In this case, we resolve ties in favor of
  // yaw-only motion (no roll).
  auto fullPitch = std::abs(m12) < (1.0f - 1e-7f);
  return fullPitch ? std::atan2(m10, m11) : 0.0f;
}

Quaternion Quaternion::fromPitchYawRollDegrees(float x, float y, float z) noexcept {
  return fromPitchYawRollRadians(x * DEG_TO_RAD, y * DEG_TO_RAD, z * DEG_TO_RAD);
}

void Quaternion::toPitchYawRollDegrees(float *x, float *y, float *z) const noexcept {
  toPitchYawRollRadians(x, y, z);
  *x *= RAD_TO_DEG;
  *y *= RAD_TO_DEG;
  *z *= RAD_TO_DEG;
}

HVector3 Quaternion::toPitchYawRollRadians() const noexcept {
  float x, y, z;
  toPitchYawRollRadians(&x, &y, &z);
  return {x, y, z};
}

HVector3 Quaternion::toPitchYawRollDegrees() const noexcept {
  float x, y, z;
  toPitchYawRollDegrees(&x, &y, &z);
  return {x, y, z};
}

float Quaternion::pitchDegrees() const noexcept { return RAD_TO_DEG * pitchRadians(); };
float Quaternion::yawDegrees() const noexcept { return RAD_TO_DEG * yawRadians(); }
float Quaternion::rollDegrees() const noexcept { return RAD_TO_DEG * rollRadians(); }

// Converts a Quaternion to its Axis-Angle representation.
HVector3 Quaternion::toAxisAngle() const noexcept {
  const float sinSquared = x_ * x_ + y_ * y_ + z_ * z_;
  // For quaternions representing non-zero rotation, the conversion is numerically stable.
  if (sinSquared > 0.0f) {
    const float sinTheta = sqrt(sinSquared);
    const float k = 2.0f * atan2(sinTheta, w_) / sinTheta;
    return {x_ * k, y_ * k, z_ * k};
  }
  // If sinSquared is 0, then we will get NaNs when dividing by sinTheta.  By approximating with a
  // Taylor series, and truncating at one term, the value will be computed correctly.
  return {x_ * 2.0f, y_ * 2.0f, z_ * 2.0f};
}

// Computes the quaternion required to rotate from this quaterion to the other.
Quaternion Quaternion::delta(Quaternion other) const noexcept {
  return other.times(this->inverse());
}

Quaternion Quaternion::conjugate() const { return Quaternion(w_, -x_, -y_, -z_); }

float Quaternion::squaredNorm() const { return w_ * w_ + x_ * x_ + y_ * y_ + z_ * z_; }

float Quaternion::norm() const { return (float)std::sqrt(squaredNorm()); }

Quaternion Quaternion::normalize() const {
  float n = norm();
  float in = n > 0.0f ? 1.0f / norm() : 1.0f;
  return Quaternion(w_ * in, x_ * in, y_ * in, z_ * in);
}

Quaternion Quaternion::inverse() const { return conjugate().normalize(); }

Quaternion Quaternion::plus(Quaternion rhs) const {
  return Quaternion(w_ + rhs.w_, x_ + rhs.x_, y_ + rhs.y_, z_ + rhs.z_);
}

Quaternion Quaternion::times(Quaternion rhs) const {
  return Quaternion(
           w_ * rhs.w_ - x_ * rhs.x_ - y_ * rhs.y_ - z_ * rhs.z_,
           w_ * rhs.x_ + x_ * rhs.w_ + y_ * rhs.z_ - z_ * rhs.y_,
           w_ * rhs.y_ - x_ * rhs.z_ + y_ * rhs.w_ + z_ * rhs.x_,
           w_ * rhs.z_ + x_ * rhs.y_ - y_ * rhs.x_ + z_ * rhs.w_)
    .normalize();
}

Quaternion Quaternion::interpolate(Quaternion rhs, float t) const {
  auto a = normalize();
  auto b = rhs.normalize();

  float dot = dotQ(a, b);

  if (dot < 0.0f) {
    b = b.negate();
    dot *= -1.0f;
  }

  if (dot > .995) {
    return Quaternion(
             a.w() + t * (b.w() - a.w()),
             a.x() + t * (b.x() - a.x()),
             a.y() + t * (b.y() - a.y()),
             a.z() + t * (b.z() - a.z()))
      .normalize();
  }

  double sqrSinHalfTheta = 1.0f - dot * dot;
  double sinHalfTheta = std::sqrt(sqrSinHalfTheta);
  double r0 = std::atan2(sinHalfTheta, dot);
  double sr0 = std::sin(r0);
  double r = t * r0;
  double sr = std::sin(r);
  double ta = std::cos(r) - dot * sr / sr0;
  double tb = sr / sr0;
  return Quaternion(
           ta * a.w() + tb * b.w(),
           ta * a.x() + tb * b.x(),
           ta * a.y() + tb * b.y(),
           ta * a.z() + tb * b.z())
    .normalize();
}

String Quaternion::toString() const noexcept {
  std::stringstream out;
  out << *this;
  return out.str();
}

String Quaternion::toPrettyString() const noexcept {
  return format("(w: %f, x: %f, y: %f, z: %f)", pretty(w_), pretty(x_), pretty(y_), pretty(z_));
}

String Quaternion::toPitchYawRollString() const noexcept {
  float x = 0;
  float y = 0;
  float z = 0;
  toPitchYawRollDegrees(&x, &y, &z);
  return format("(x: %f, y: %f, z: %f)", pretty(x), pretty(y), pretty(z));
}

float Quaternion::dist(Quaternion rhs) const { return 1 - std::pow(dotQ(*this, rhs), 2); }

float Quaternion::radians(Quaternion rhs) const {
  // We need to clip because floating point errors can cause the dot product to be slightly out of
  // range of -1 to 1, which would cause acos to return a NaN.
  return std::acos(2.0f * std::pow(clip(dotQ(*this, rhs), -1.0f, 1.0f), 2) - 1.0);
}

Quaternion Quaternion::negate() const { return {-w_, -x_, -y_, -z_}; }

}  // namespace c8
