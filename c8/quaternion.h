// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "c8/hmatrix.h"
#include "c8/string.h"

namespace c8 {

class Quaternion {
public:
  // Default constructor.
  constexpr Quaternion() noexcept : x_(0.0f), y_(0.0f), z_(0.0f), w_(1.0f) {}
  constexpr Quaternion(float w, float x, float y, float z) noexcept : x_(x), y_(y), z_(z), w_(w) {}

  constexpr float w() const noexcept { return w_; }
  constexpr float x() const noexcept { return x_; }
  constexpr float y() const noexcept { return y_; }
  constexpr float z() const noexcept { return z_; }

  // Extracts the upper left 3x3 rotation component of the HMatrix and represents it as a
  // quaternion. Only works if the matrix consists solely of rotation and translation.
  static Quaternion fromHMatrix(const HMatrix &hmat) noexcept;
  // Use if your matrix has rotation, translation and scale. Will not work if the matrix has shear,
  // as in a matrix consisting of S * R where S is not uniform,
  static Quaternion fromTrsMatrix(const HMatrix &hmat) noexcept;

  // Constructs a quaternion rotation with the specified rotation.
  static Quaternion xDegrees(float val) noexcept;
  static Quaternion yDegrees(float val) noexcept;
  static Quaternion zDegrees(float val) noexcept;
  static Quaternion xRadians(float val) noexcept;
  static Quaternion yRadians(float val) noexcept;
  static Quaternion zRadians(float val) noexcept;

  // Constructs a quaternion rotation by first applying a rotation around the x-axis, followed by a
  // rotation around the y-axis, followed by a rotation around the z-axis.
  //
  // This is known as an XYZ euler angle transformation, and it is generally difficult to understand
  // or interpret. Use of this method is discouraged. Prefer `fromPitchYawRollDegrees` below.
  static Quaternion fromEulerAngleDegrees(float xDeg, float yDeg, float zDeg) noexcept;

  // Construct a quaternion from a pitch / yaw / roll represetnation, also known as YXZ euler
  // angles. This gives a gravity-aligned representation that is generally easy to interpret, and
  // should generally be preferred to using XYZ Eurler angles above.
  static Quaternion fromPitchYawRollRadians(float x, float y, float z) noexcept;
  static Quaternion fromPitchYawRollDegrees(float x, float y, float z) noexcept;

  // Constructs a Quaternion from an Axis-Angle representation.
  static Quaternion fromAxisAngle(const HVector3 &axisAngle) noexcept;

  // Converts a 4x1 double quaternion column vector to a rotation-only HMatrix.
  HMatrix toRotationMat() const noexcept;

  // Converts a 4x1 quaternion vector to a 3x1 double euler angle column vector.
  //
  // This is known as an XYZ euler angle transformation, and it is generally difficult to understand
  // or interpret. Use of this method is discouraged. Prefer `toPitchYawRollDegrees` below.
  void toEulerAngleDegrees(float *xDeg, float *yDeg, float *zDeg) const noexcept;
  HVector3 toEulerAngleDegrees() const noexcept;

  // Convert this quaternion to a pitch / yaw / roll represetnation, also known as YXZ euler angles.
  // This gives a gravity-aligned representation that is generally easy to interpret, and should
  // generally be preferred to using XYZ Eurler angles above.
  //
  // Pitch (x-axis rotation) has a range of -90 to 90 degrees. Yaw (y-axis rotation) has a range of
  // -180 to 180 degrees. Roll (z-axis rotation) has a range of -180 to 180 degrees.
  void toPitchYawRollRadians(float *x, float *y, float *z) const noexcept;
  void toPitchYawRollDegrees(float *x, float *y, float *z) const noexcept;
  HVector3 toPitchYawRollRadians() const noexcept;
  HVector3 toPitchYawRollDegrees() const noexcept;

  // More efficient implementations of toPitchYawRollRadians etc. when only one element is required.
  // If multiple elements are required, calling toPitchYawRollRadians etc. is more efficient.
  float pitchRadians() const noexcept;
  float pitchDegrees() const noexcept;
  float yawRadians() const noexcept;
  float yawDegrees() const noexcept;
  float rollRadians() const noexcept;
  float rollDegrees() const noexcept;

  // Converts a Quaternion to its Axis-Angle representation.
  HVector3 toAxisAngle() const noexcept;

  // Computes the quaternion required to rotate from this quaterion to the other.
  Quaternion delta(Quaternion other) const noexcept;

  // Default move and copy constructors. Quaternion is a small type, and copy is expected to be
  // more efficient than reference.
  Quaternion(Quaternion &&) = default;
  Quaternion &operator=(Quaternion &&) = default;
  Quaternion(const Quaternion &) = default;
  Quaternion &operator=(const Quaternion &) = default;

  // Returns the rotational conjugate of this quaternion. The conjugate of a quaternion represents
  // the same rotation in the opposite direction about the rotational axis.
  Quaternion conjugate() const;
  float squaredNorm() const;
  float norm() const;
  Quaternion normalize() const;
  Quaternion inverse() const;
  Quaternion plus(Quaternion rhs) const;
  Quaternion times(Quaternion rhs) const;

  // Interpolates between two quaternions given a provided interpolation value.  If the
  // interpolation is set to 0.0f, then it will return the left-hand side quaternion.  If the
  // interpolation is set to 1.0f, then it will return the right-hand side quaternion.
  Quaternion interpolate(Quaternion rhs, float t) const;
  // non-linear distance between two unit quaternion (0 = same quaternion, 1 = complete opposite)
  float dist(Quaternion rhs) const;

  // Linear distance between two unit quaternion (0 = same quaternion, 3.14 = complete opposite).
  // For smaller rotations, radians() produces a larger delta than dist().  For a 10 degree
  // difference, radians() has a 23x larger delta than dist().  For a 180 degree difference,
  // radians() has a 3.14 larger delta than dist().
  float radians(Quaternion rhs) const;

  String toString() const noexcept;
  String toPrettyString() const noexcept;
  String toPitchYawRollString() const noexcept;

  // If you inverse each component of a quaternion, it represents the same rotation, just the axis
  // and angle have both been reversed.
  Quaternion negate() const;

private:
  float x_ = 0.0f;
  float y_ = 0.0f;
  float z_ = 0.0f;
  float w_ = 1.0f;
};

// Stream output operator.
inline std::ostream &operator<<(std::ostream &stream, Quaternion q) noexcept {
  stream << "(w: " << q.w() << ", x: " << q.x() << ", y: " << q.y() << ", z: " << q.z() << ")";
  return stream;
}

}  // namespace c8
