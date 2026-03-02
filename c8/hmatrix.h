// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Matrix class specialized for 4x4 rigid body transformations and projections of
// homogenous coordinates.

#pragma once

#include <array>
#include <initializer_list>

#include "c8/exceptions.h"
#include "c8/geometry/pixel-pinhole-camera-model.h"
#include "c8/hpoint.h"
#include "c8/hvector.h"
#include "c8/string.h"
#include "c8/vector.h"

namespace c8 {

class HMatrix {
public:
  // Construct the 4x4 matrix directly, using brace syntax.
  // Usage:
  //   auto mat = HMatrix{{1.0f, 2.0f, 3.0f, 4.0f},
  //                      {1.0f, 2.0f, 3.0f, 4.0f},
  //                      {1.0f, 2.0f, 3.0f, 4.0f},
  //                      {1.0f, 2.0f, 3.0f, 4.0f}};
  HMatrix(
    const float (&row0)[4], const float (&row1)[4], const float (&row2)[4], const float (&row3)[4]);

  // Construct the 4x4 matrix directly, using brace syntax, for a known non-invertalbe matrix.
  // Usage:
  //   auto mat = HMatrix{{1.0f, 2.0f, 3.0f, 4.0f},
  //                      {1.0f, 2.0f, 3.0f, 4.0f},
  //                      {1.0f, 2.0f, 3.0f, 4.0f},
  //                      {1.0f, 2.0f, 3.0f, 4.0f},
  //                      true};
  // The resulting mat will throw an error if you try to invert it.
  HMatrix(
    const float (&row0)[4],
    const float (&row1)[4],
    const float (&row2)[4],
    const float (&row3)[4],
    bool noInvert);

  // Construct the 4x4 matrix directly along with its inverse, using brace syntax.
  constexpr HMatrix(
    const float (&row0)[4],
    const float (&row1)[4],
    const float (&row2)[4],
    const float (&row3)[4],
    const float (&inv0)[4],
    const float (&inv1)[4],
    const float (&inv2)[4],
    const float (&inv3)[4]) noexcept
      :  // clang-format off
      matrix_{{row0[0], row1[0], row2[0], row3[0],
                 row0[1], row1[1], row2[1], row3[1],
                 row0[2], row1[2], row2[2], row3[2],
                 row0[3], row1[3], row2[3], row3[3]}},
      inverseMatrix_{{inv0[0], inv1[0], inv2[0], inv3[0],
                      inv0[1], inv1[1], inv2[1], inv3[1],
                      inv0[2], inv1[2], inv2[2], inv3[2],
                      inv0[3], inv1[3], inv2[3], inv3[3]}},
      noInvert_(false) {}  // clang-format on

  // Construct the 4x4 matrix from a clone of raw column-major data.
  // Usage:
  //   auto mat = HMatrix{&matData[0], &inverseMatData[0]};
  HMatrix(const float matData[16], const float inverseMatData[16], bool noInvert = false);

  // Multiply the matrix by a 3D point in homogenous coordinates.
  HPoint3 operator*(HPoint3 pt) const noexcept;

  // Multiply the matrix by a 3D vector in homogenous coordinates.
  HVector3 operator*(HVector3 pt) const noexcept;

  // Multiply the matrix by another matrix.
  HMatrix operator*(const HMatrix &matrix) const noexcept;

  // Multiply the matrix by a scalar.
  friend HMatrix operator*(const HMatrix &matrix, float scalar);
  friend HMatrix operator*(float scalar, const HMatrix &matrix);

  // Multiply the matrix by a vector of 3D points in homogenous coordinates.
  Vector<HPoint3> operator*(const Vector<HPoint3> &points) const noexcept;

  // Multiply the matrix by a vector of 3D vectors in homogenous coordinates.
  Vector<HVector3> operator*(const Vector<HVector3> &points) const noexcept;

  // In place versions of the multiply operators above.
  void times(const Vector<HPoint3> &points, Vector<HPoint3> *result) const noexcept;
  void times(const Vector<HVector3> &points, Vector<HVector3> *result) const noexcept;

  // Default move constructors.
  HMatrix(HMatrix &&) noexcept = default;
  HMatrix &operator=(HMatrix &&) noexcept = default;

  // HMatrix is small; allow copying.
  HMatrix(const HMatrix &) = default;
  HMatrix &operator=(const HMatrix &) = default;

  // Return the matrix inverse. Cheap to call, as HMatrix internally stores its inverse. Throws
  // a RuntimeException if the matrix is not invertable.
  constexpr HMatrix inv() const {
    if (invertFailed_) {
      C8_THROW("Matrix is not invertible, LU factorization factor U is exactly singular");
    }

    if (noInvert_) {
      C8_THROW("Matrix was specified as non-invertible");
    }
    HMatrix result;
    std::copy(inverseMatrix_.cbegin(), inverseMatrix_.cend(), result.matrix_.begin());
    std::copy(matrix_.cbegin(), matrix_.cend(), result.inverseMatrix_.begin());
    return result;
  }

  // Return the matrix transpose.
  HMatrix t() const noexcept;

  // Return a translated matrix.
  HMatrix translate(float x, float y, float z) const noexcept;

  // Return a rotated matrix. Right-hand rotation around the vector (x, y, z), by an amount equal
  // to the magnitude of the vector in radians.
  HMatrix rotateR(float x, float y, float z) const noexcept;

  // Return a rotated matrix. Right-hand rotation around the vector (x, y, z), by an amount equal
  // to the magnitude of the vector in degrees.
  HMatrix rotateD(float x, float y, float z) const noexcept;

  // Return the matrix determinant.
  float determinant() const noexcept;

  // Access an element in the matrix, (row, column).
  constexpr float operator()(int row, int col) const noexcept { return matrix_[4 * col + row]; }

  String toString() const noexcept;
  String toPrettyString() const noexcept;

  const std::array<float, 16> &data() const noexcept { return matrix_; }
  const std::array<float, 16> &inverseData() const noexcept { return inverseMatrix_; }

private:
  friend class HMatrixGen;

  // The default constructor is private.
  HMatrix() noexcept = default;

  // Column major 4x4 matrix.
  std::array<float, 16> matrix_;

  // Column major 4x4 inverse matrix.
  std::array<float, 16> inverseMatrix_;

  // If true, .inv() cannot be called on this matrix.
  bool noInvert_ = false;

  bool invertFailed_ = false;
};

// Multiplication by a scalar.
HMatrix operator*(const HMatrix &matrix, float scalar);
HMatrix operator*(float scalar, const HMatrix &matrix);

// Stream output operator.
std::ostream &operator<<(std::ostream &stream, const HMatrix &m) noexcept;

// Multiply and flatten to 2-dimensions, optionally extruding 2D input. Equivalent to
// flatten<2>(matrix * extrude<3>(pts)) with the opportunity to be more efficient.
Vector<HPoint2> project2D(const HMatrix &matrix, const Vector<HPoint2> &input);

// Multiply and flatten to 2-dimensions. Equivalent to flatten<2>(matrix * pts) with the opportunity
// to be more efficient.
Vector<HPoint2> project2D(const HMatrix &matrix, const Vector<HPoint3> &input);

// Generate common matrices.
class HMatrixGen {
public:
  // Return the identity matrix.
  static constexpr HMatrix i() noexcept {
    return HMatrix{
      {1.0f, 0.0f, 0.0f, 0.0f},
      {0.0f, 1.0f, 0.0f, 0.0f},
      {0.0f, 0.0f, 1.0f, 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f},
      {1.0f, 0.0f, 0.0f, 0.0f},
      {0.0f, 1.0f, 0.0f, 0.0f},
      {0.0f, 0.0f, 1.0f, 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f},
    };
  }

  // Return translation matrix.
  static constexpr HMatrix translation(float x, float y, float z) noexcept {
    return HMatrix{
      {1.0f, 0.0f, 0.0f, x},
      {0.0f, 1.0f, 0.0f, y},
      {0.0f, 0.0f, 1.0f, z},
      {0.0f, 0.0f, 0.0f, 1.0f},
      {1.0f, 0.0f, 0.0f, -x},
      {0.0f, 1.0f, 0.0f, -y},
      {0.0f, 0.0f, 1.0f, -z},
      {0.0f, 0.0f, 0.0f, 1.0f},
    };
  }

  static constexpr HMatrix translateX(float v) noexcept {
    return HMatrix{
      {1.0f, 0.0f, 0.0f, v},
      {0.0f, 1.0f, 0.0f, 0.0f},
      {0.0f, 0.0f, 1.0f, 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f},
      {1.0f, 0.0f, 0.0f, -v},
      {0.0f, 1.0f, 0.0f, 0.0f},
      {0.0f, 0.0f, 1.0f, 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f},
    };
  }

  static constexpr HMatrix translateY(float v) noexcept {
    return HMatrix{
      {1.0f, 0.0f, 0.0f, 0.0f},
      {0.0f, 1.0f, 0.0f, v},
      {0.0f, 0.0f, 1.0f, 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f},
      {1.0f, 0.0f, 0.0f, 0.0f},
      {0.0f, 1.0f, 0.0f, -v},
      {0.0f, 0.0f, 1.0f, 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f},
    };
  }

  static constexpr HMatrix translateZ(float v) noexcept {
    return HMatrix{
      {1.0f, 0.0f, 0.0f, 0.0f},
      {0.0f, 1.0f, 0.0f, 0.0f},
      {0.0f, 0.0f, 1.0f, v},
      {0.0f, 0.0f, 0.0f, 1.0f},
      {1.0f, 0.0f, 0.0f, 0.0f},
      {0.0f, 1.0f, 0.0f, 0.0f},
      {0.0f, 0.0f, 1.0f, -v},
      {0.0f, 0.0f, 0.0f, 1.0f},
    };
  }

  // Scale the x dimenion by a given factor.
  static constexpr HMatrix scaleX(float fact) noexcept {
    float ifac = 1.0f / fact;
    return HMatrix{
      {fact, 0.0f, 0.0f, 0.0f},
      {0.0f, 1.0f, 0.0f, 0.0f},
      {0.0f, 0.0f, 1.0f, 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f},
      {ifac, 0.0f, 0.0f, 0.0f},
      {0.0f, 1.0f, 0.0f, 0.0f},
      {0.0f, 0.0f, 1.0f, 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f},
    };
  }

  // Scale the y dimenion by a given factor.
  static constexpr HMatrix scaleY(float fact) noexcept {
    float ifac = 1.0f / fact;
    return HMatrix{
      {1.0f, 0.0f, 0.0f, 0.0f},
      {0.0f, fact, 0.0f, 0.0f},
      {0.0f, 0.0f, 1.0f, 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f},
      {1.0f, 0.0f, 0.0f, 0.0f},
      {0.0f, ifac, 0.0f, 0.0f},
      {0.0f, 0.0f, 1.0f, 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f},
    };
  }

  // Scale the z dimenion by a given factor.
  static constexpr HMatrix scaleZ(float fact) noexcept {
    float ifac = 1.0f / fact;
    return HMatrix{
      {1.0f, 0.0f, 0.0f, 0.0f},
      {0.0f, 1.0f, 0.0f, 0.0f},
      {0.0f, 0.0f, fact, 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f},
      {1.0f, 0.0f, 0.0f, 0.0f},
      {0.0f, 1.0f, 0.0f, 0.0f},
      {0.0f, 0.0f, ifac, 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f},
    };
  }

  // Scale the z dimenion by a given factor.
  static constexpr HMatrix scale(float xscl, float yscl, float zscl) noexcept {
    float ixsc = 1.0f / xscl;
    float iysc = 1.0f / yscl;
    float izsc = 1.0f / zscl;
    return HMatrix{
      {xscl, 0.0f, 0.0f, 0.0f},
      {0.0f, yscl, 0.0f, 0.0f},
      {0.0f, 0.0f, zscl, 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f},
      {ixsc, 0.0f, 0.0f, 0.0f},
      {0.0f, iysc, 0.0f, 0.0f},
      {0.0f, 0.0f, izsc, 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f},
    };
  }

  static constexpr HMatrix scale(float scal) noexcept {
    float iscl = 1.0f / scal;
    return HMatrix{
      {scal, 0.0f, 0.0f, 0.0f},
      {0.0f, scal, 0.0f, 0.0f},
      {0.0f, 0.0f, scal, 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f},
      {iscl, 0.0f, 0.0f, 0.0f},
      {0.0f, iscl, 0.0f, 0.0f},
      {0.0f, 0.0f, iscl, 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f},
    };
  }

  // Return an intrinsic matrix for a pinhole camera model.
  static constexpr HMatrix intrinsic(c8_PixelPinholeCameraModel m) noexcept {
    auto a = m.focalLengthHorizontal;
    auto b = -1.0f * m.focalLengthVertical;
    auto x = m.centerPointX;
    auto y = m.centerPointY;
    auto ai = 1.0f / a;
    auto bi = 1.0f / b;
    auto xi = -x * ai;
    auto yi = -y * bi;
    return HMatrix{
      {a, 0.0000000f, x, 0.0f},
      {0.0000000f, b, y, 0.0f},
      {0.0f, 0.0f, 1.0f, 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f},
      {ai, 0.00000f, xi, 0.0f},
      {0.00000f, bi, yi, 0.0f},
      {0.0f, 0.0f, 1.0f, 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f},
    };
  }

  // Return a rotation matrix. Right-hand rotation around the vector (x, y, z), by an amount equal
  // to the magnitude of the vector in radians.
  static HMatrix rotationR(float x, float y, float z) noexcept;

  // Return a rotation matrix. Right-hand rotation around the vector (x, y, z), by an amount equal
  // to the magnitude of the vector in degrees.
  static HMatrix rotationD(float x, float y, float z) noexcept;

  // Return rotation matrix around x-axis in radians.
  static HMatrix xRadians(float rads) noexcept;

  // Return rotation matrix around y-axis in radians.
  static HMatrix yRadians(float rads) noexcept;

  // Return rotation matrix around z-axis in radians.
  static HMatrix zRadians(float rads) noexcept;

  // Return rotation matrix around x-axis in degrees.
  static HMatrix xDegrees(float degrees) noexcept;

  // Return rotation matrix around y-axis in degrees.
  static HMatrix yDegrees(float degrees) noexcept;

  // Return rotation matrix around z-axis in degrees.
  static HMatrix zDegrees(float degrees) noexcept;

  // Rotate 90 degrees in-plane. More numerically precise than zDegrees(90).
  static HMatrix z90() noexcept;

  // Rotate 180 degrees in-plane. More numerically precise than zDegrees(180).
  static HMatrix z180() noexcept;

  // Rotate 270 degrees in-plane. More numerically precise than zDegrees(270).
  static HMatrix z270() noexcept;

  // Return the matrix C which can be used compute a vector cross product through matrix
  // multiplication.  Given a vector v1 and v2, C(v1.x, v1.y, v1.z) * v2 == v1.cross(v2). Matrices
  // of this form appear commonly in projective geometry, e.g. in the construction of the essential
  // matrix.
  static HMatrix cross(float x, float y, float z) noexcept;

  // Returns the HMatrix that rotates unit vector a so that it aligns to unit vector b
  static HMatrix unitRotationAlignment(const HVector3 &a, const HVector3 &b) noexcept;

  // Returns a HMatrix built from given rotation or rotation part of a given matrix, and a
  // translation
  // @param R: pure rotation matrix or a transform matrix, when it's a transform matrix with
  // translation, only its rotation part will be used
  static HMatrix fromRotationAndTranslation(const HMatrix &R, const HVector3 &t) noexcept;
};

}  // namespace c8
