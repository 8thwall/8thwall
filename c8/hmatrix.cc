// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include <cmath>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>

#include "c8/hmatrix.h"

static inline void unrolled4x4Mul4x1(const float *A, const float *B, float *Outs) {
  Outs[0] = A[0] * B[0] + A[4] * B[1] + A[8] * B[2] + A[12] * B[3];
  Outs[1] = A[1] * B[0] + A[5] * B[1] + A[9] * B[2] + A[13] * B[3];
  Outs[2] = A[2] * B[0] + A[6] * B[1] + A[10] * B[2] + A[14] * B[3];
  Outs[3] = A[3] * B[0] + A[7] * B[1] + A[11] * B[2] + A[15] * B[3];
}

#if !defined(NO_SIMD) && !defined(JAVASCRIPT) && defined(__ARM_NEON__)
#define HMATRIX_USE_NEON
#endif

#ifdef HMATRIX_USE_NEON
#include <arm_neon.h>
static inline void neon4x4Mul4x4(const float *A, const float *B, float *Outs) {
  // We might be able to speed this up with
  // float32x4x4_t vld4q_f32(__transfersize(16) float32_t const * ptr);
  const float32x4_t ACol0 = vld1q_f32(A + 0);
  const float32x4_t ACol1 = vld1q_f32(A + 4);
  const float32x4_t ACol2 = vld1q_f32(A + 8);
  const float32x4_t ACol3 = vld1q_f32(A + 12);

  const float32x4_t BCol0 = vld1q_f32(B + 0);
  const float32x4_t BCol1 = vld1q_f32(B + 4);
  const float32x4_t BCol2 = vld1q_f32(B + 8);
  const float32x4_t BCol3 = vld1q_f32(B + 12);

  float32x4_t outCol0;
  float32x4_t outCol1;
  float32x4_t outCol2;
  float32x4_t outCol3;

  // We want computation to be done on multiple registers in parallel.
  outCol0 = vmulq_n_f32(ACol0, vgetq_lane_f32(BCol0, 0));
  outCol1 = vmulq_n_f32(ACol0, vgetq_lane_f32(BCol1, 0));
  outCol2 = vmulq_n_f32(ACol0, vgetq_lane_f32(BCol2, 0));
  outCol3 = vmulq_n_f32(ACol0, vgetq_lane_f32(BCol3, 0));

  outCol0 = vmlaq_n_f32(outCol0, ACol1, vgetq_lane_f32(BCol0, 1));
  outCol1 = vmlaq_n_f32(outCol1, ACol1, vgetq_lane_f32(BCol1, 1));
  outCol2 = vmlaq_n_f32(outCol2, ACol1, vgetq_lane_f32(BCol2, 1));
  outCol3 = vmlaq_n_f32(outCol3, ACol1, vgetq_lane_f32(BCol3, 1));

  outCol0 = vmlaq_n_f32(outCol0, ACol2, vgetq_lane_f32(BCol0, 2));
  outCol1 = vmlaq_n_f32(outCol1, ACol2, vgetq_lane_f32(BCol1, 2));
  outCol2 = vmlaq_n_f32(outCol2, ACol2, vgetq_lane_f32(BCol2, 2));
  outCol3 = vmlaq_n_f32(outCol3, ACol2, vgetq_lane_f32(BCol3, 2));

  outCol0 = vmlaq_n_f32(outCol0, ACol3, vgetq_lane_f32(BCol0, 3));
  outCol1 = vmlaq_n_f32(outCol1, ACol3, vgetq_lane_f32(BCol1, 3));
  outCol2 = vmlaq_n_f32(outCol2, ACol3, vgetq_lane_f32(BCol2, 3));
  outCol3 = vmlaq_n_f32(outCol3, ACol3, vgetq_lane_f32(BCol3, 3));

  vst1q_f32(Outs, outCol0);
  vst1q_f32(Outs + 4, outCol1);
  vst1q_f32(Outs + 8, outCol2);
  vst1q_f32(Outs + 12, outCol3);
}

static inline void neon4x4Mul4xN(const float *H, const float *Pts, float *Outs, size_t numPoints) {
  // Use hand-roll NEON intrinsics
  // The explanations at https://github.com/thenifty/neon-guide is useful
  // NOTE(dat): How do we quantify cache hit for loading?
  // TODO(dat): Consider padding numPoints to be divisible by 4, then operate in block of 4x4
  const float32x4_t hCol0 = vld1q_f32(H + 0);
  const float32x4_t hCol1 = vld1q_f32(H + 4);
  const float32x4_t hCol2 = vld1q_f32(H + 8);
  const float32x4_t hCol3 = vld1q_f32(H + 12);

  float32x4_t outCol;
  float32x4_t pCol;
  // Works one input vector at a time
  for (size_t k = 0; k < numPoints; ++k) {
    // Compute one output vector
    pCol = vld1q_f32(Pts);
    outCol = vmulq_n_f32(hCol0, vgetq_lane_f32(pCol, 0));
    outCol = vmlaq_n_f32(outCol, hCol1, vgetq_lane_f32(pCol, 1));
    outCol = vmlaq_n_f32(outCol, hCol2, vgetq_lane_f32(pCol, 2));
    outCol = vmlaq_n_f32(outCol, hCol3, vgetq_lane_f32(pCol, 3));
    // Store output vector
    vst1q_f32(Outs, outCol);

    Pts += 4;
    Outs += 4;
  }
}
#else
static inline void unrolled4x4Mul4x4(const float *A, const float *B, float *Outs) {
  Outs[0] = A[0] * B[0] + A[4] * B[1] + A[8] * B[2] + A[12] * B[3];
  Outs[1] = A[1] * B[0] + A[5] * B[1] + A[9] * B[2] + A[13] * B[3];
  Outs[2] = A[2] * B[0] + A[6] * B[1] + A[10] * B[2] + A[14] * B[3];
  Outs[3] = A[3] * B[0] + A[7] * B[1] + A[11] * B[2] + A[15] * B[3];

  Outs[4] = A[0] * B[4] + A[4] * B[5] + A[8] * B[6] + A[12] * B[7];
  Outs[5] = A[1] * B[4] + A[5] * B[5] + A[9] * B[6] + A[13] * B[7];
  Outs[6] = A[2] * B[4] + A[6] * B[5] + A[10] * B[6] + A[14] * B[7];
  Outs[7] = A[3] * B[4] + A[7] * B[5] + A[11] * B[6] + A[15] * B[7];

  Outs[8] = A[0] * B[8] + A[4] * B[9] + A[8] * B[10] + A[12] * B[11];
  Outs[9] = A[1] * B[8] + A[5] * B[9] + A[9] * B[10] + A[13] * B[11];
  Outs[10] = A[2] * B[8] + A[6] * B[9] + A[10] * B[10] + A[14] * B[11];
  Outs[11] = A[3] * B[8] + A[7] * B[9] + A[11] * B[10] + A[15] * B[11];

  Outs[12] = A[0] * B[12] + A[4] * B[13] + A[8] * B[14] + A[12] * B[15];
  Outs[13] = A[1] * B[12] + A[5] * B[13] + A[9] * B[14] + A[13] * B[15];
  Outs[14] = A[2] * B[12] + A[6] * B[13] + A[10] * B[14] + A[14] * B[15];
  Outs[15] = A[3] * B[12] + A[7] * B[13] + A[11] * B[14] + A[15] * B[15];
}

// Perf comparison on M1 laptop 8/28/2022
// -----------------------------------------------------------------------------
// Benchmark                                      Time           CPU Iterations
// -----------------------------------------------------------------------------
// (this version) group of 4 then group of 1:
// HMatrixBenchmarkTest/MatMulVecHPoint3      63600 ns      63598 ns      31948
// (old version) group of 1
// HMatrixBenchmarkTest/MatMulVecHPoint3      83883 ns      83873 ns      81761
// (simd) compile with :hmatrix instead of :hmatrix:nosimd
// HMatrixBenchmarkTest/MatMulVecHPoint3     121087 ns     120747 ns     117349
static inline void unrolled4x4Mul4xN(const float *A, const float *B, float *Outs, size_t numPoints) {
  size_t i = 0;
  // do it in group of 4
  for (; i + 4 < numPoints; i+=4) {
    unrolled4x4Mul4x4(A, B, Outs);
    B += 16;
    Outs += 16;
  }
  // finish off with group of 1
  for (; i < numPoints; i++) {
    unrolled4x4Mul4x1(A, B, Outs);
    B += 4;
    Outs += 4;
  }
}
#endif

static inline void mat4x4Mul4x1(const float *A, const float *B, float *Outs) {
  unrolled4x4Mul4x1(A, B, Outs);
}

static inline void mat4x4Mul4x4(const float *A, const float *B, float *Outs) {
#ifdef HMATRIX_USE_NEON
  neon4x4Mul4x4(A, B, Outs);
#else
  unrolled4x4Mul4x4(A, B, Outs);
#endif
}
static inline void mat4x4Mul4xN(const float *A, const float *B, float *Outs, size_t numPoints) {
#ifdef HMATRIX_USE_NEON
  neon4x4Mul4xN(A, B, Outs, numPoints);
#else
  unrolled4x4Mul4xN(A, B, Outs, numPoints);
#endif
}

namespace {

constexpr float det2(float m00, float m01, float m10, float m11) { return m00 * m11 - m01 * m10; }

constexpr float det3(
  float m00,
  float m01,
  float m02,
  float m10,
  float m11,
  float m12,
  float m20,
  float m21,
  float m22) {
  return m00 * det2(m11, m12, m21, m22)  // Col 0
    - m01 * det2(m10, m12, m20, m22)     // Col 1
    + m02 * det2(m10, m11, m20, m21);    // Col 2
}

constexpr bool invert4x4(const float *m, float *inv) {
  // clang-format off
    inv[0] =   m[5]  * m[10] * m[15] - m[5]  * m[11] * m[14]
             - m[9]  * m[6]  * m[15] + m[9]  * m[7]  * m[14]
             + m[13] * m[6]  * m[11] - m[13] * m[7]  * m[10];

    inv[4] = - m[4]  * m[10] * m[15] + m[4]  * m[11] * m[14]
             + m[8]  * m[6]  * m[15] - m[8]  * m[7]  * m[14]
             - m[12] * m[6]  * m[11] + m[12] * m[7]  * m[10];

    inv[8] = m[4]  * m[9] * m[15] - m[4]  * m[11] * m[13]
             - m[8]  * m[5] * m[15] + m[8]  * m[7] * m[13]
             + m[12] * m[5] * m[11] - m[12] * m[7] * m[9];

    inv[12] = - m[4]  * m[9] * m[14] + m[4]  * m[10] * m[13]
              + m[8]  * m[5] * m[14] - m[8]  * m[6] * m[13]
              - m[12] * m[5] * m[10] + m[12] * m[6] * m[9];
  // clang-format on

  float det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
  if (det == 0) {
    std::fill(inv, inv + 16, 0.0f);
    return false;
  }

  float idet = 1.0f / det;
  inv[0] *= idet;
  inv[4] *= idet;
  inv[8] *= idet;
  inv[12] *= idet;

  // clang-format off
    inv[1] = (-m[1]  * m[10] * m[15] + m[1]  * m[11] * m[14] +
               m[9]  * m[2] * m[15] -  m[9]  * m[3] * m[14] -
               m[13] * m[2] * m[11] + m[13] * m[3] * m[10]) * idet;

    inv[5] = (m[0]  * m[10] * m[15] - m[0]  * m[11] * m[14] -
              m[8]  * m[2] * m[15] + m[8]  * m[3] * m[14] +
              m[12] * m[2] * m[11] - m[12] * m[3] * m[10]) * idet;

    inv[9] = (-m[0]  * m[9] * m[15] + m[0]  * m[11] * m[13] +
               m[8]  * m[1] * m[15] - m[8]  * m[3] * m[13] -
               m[12] * m[1] * m[11] + m[12] * m[3] * m[9]) * idet;

    inv[13] = (m[0]  * m[9] * m[14] - m[0]  * m[10] * m[13] -
               m[8]  * m[1] * m[14] + m[8]  * m[2] * m[13] +
               m[12] * m[1] * m[10] - m[12] * m[2] * m[9]) * idet;

    inv[2] = (m[1]  * m[6] * m[15] - m[1]  * m[7] * m[14] -
              m[5]  * m[2] * m[15] + m[5]  * m[3] * m[14] +
              m[13] * m[2] * m[7] - m[13] * m[3] * m[6]) * idet;

    inv[6] = (-m[0]  * m[6] * m[15] + m[0]  * m[7] * m[14] +
               m[4]  * m[2] * m[15] - m[4]  * m[3] * m[14] -
               m[12] * m[2] * m[7] + m[12] * m[3] * m[6]) * idet;

    inv[10] = (m[0]  * m[5] * m[15] - m[0]  * m[7] * m[13] -
               m[4]  * m[1] * m[15] + m[4]  * m[3] * m[13] +
               m[12] * m[1] * m[7] - m[12] * m[3] * m[5]) * idet;

    inv[14] = (-m[0]  * m[5] * m[14] + m[0]  * m[6] * m[13] +
                m[4]  * m[1] * m[14] - m[4]  * m[2] * m[13] -
                m[12] * m[1] * m[6] + m[12] * m[2] * m[5]) * idet;

    inv[3] = (-m[1] * m[6] * m[11] + m[1] * m[7] * m[10] +
               m[5] * m[2] * m[11] - m[5] * m[3] * m[10] -
               m[9] * m[2] * m[7] + m[9] * m[3] * m[6]) * idet;

    inv[7] = (m[0] * m[6] * m[11] - m[0] * m[7] * m[10] -
              m[4] * m[2] * m[11] + m[4] * m[3] * m[10] +
              m[8] * m[2] * m[7] - m[8] * m[3] * m[6]) * idet;

    inv[11] = (-m[0] * m[5] * m[11] + m[0] * m[7] * m[9] +
                m[4] * m[1] * m[11] - m[4] * m[3] * m[9] -
                m[8] * m[1] * m[7] + m[8] * m[3] * m[5]) * idet;

    inv[15] = (m[0] * m[5] * m[10] - m[0] * m[6] * m[9] -
               m[4] * m[1] * m[10] + m[4] * m[2] * m[9] +
               m[8] * m[1] * m[6] - m[8] * m[2] * m[5]) * idet;
  // clang-format on

  return true;
}

float pretty(float f) { return (std::abs(f) < 1e-7) ? 0.f : f; }

}  // namespace

namespace c8 {
HMatrix::HMatrix(
  const float (&row0)[4], const float (&row1)[4], const float (&row2)[4], const float (&row3)[4])
    // clang-format off
    : HMatrix(row0, row1, row2, row3, false) {}

HMatrix::HMatrix(
  const float (&row0)[4], const float (&row1)[4], const float (&row2)[4], const float (&row3)[4], bool noInvert)
    // clang-format off
    : matrix_{{row0[0], row1[0], row2[0], row3[0],
               row0[1], row1[1], row2[1], row3[1],
               row0[2], row1[2], row2[2], row3[2],
               row0[3], row1[3], row2[3], row3[3]}},  // clang-format on
      noInvert_(noInvert) {
  if (noInvert_) {
    return;
  }

  // TODO(nb): Fix clapack based implementation.
  if (!invert4x4(&matrix_[0], &inverseMatrix_[0])) {
    noInvert_ = true;
    invertFailed_ = true;
  }
}

HMatrix::HMatrix(const float matData[16], const float inverseMatData[16], bool noInvert) {
  std::copy(&matData[0], &matData[0] + 16, matrix_.begin());
  if (inverseMatData == nullptr) {
    std::fill(inverseMatrix_.begin(), inverseMatrix_.end(), 0.0f);
    noInvert_ = true;
  } else {
    std::copy(&inverseMatData[0], &inverseMatData[0] + 16, inverseMatrix_.begin());
    noInvert_ = noInvert;
  }
}

HMatrix HMatrix::t() const noexcept {
  const HMatrix &m = *this;
  if (noInvert_) {
    HMatrix tr{{m(0, 0), m(1, 0), m(2, 0), m(3, 0)},
               {m(0, 1), m(1, 1), m(2, 1), m(3, 1)},
               {m(0, 2), m(1, 2), m(2, 2), m(3, 2)},
               {m(0, 3), m(1, 3), m(2, 3), m(3, 3)},
               noInvert_};
    tr.invertFailed_ = invertFailed_;
    return tr;
  }
  HMatrix i = this->inv();
  return HMatrix{{m(0, 0), m(1, 0), m(2, 0), m(3, 0)},
                 {m(0, 1), m(1, 1), m(2, 1), m(3, 1)},
                 {m(0, 2), m(1, 2), m(2, 2), m(3, 2)},
                 {m(0, 3), m(1, 3), m(2, 3), m(3, 3)},
                 {i(0, 0), i(1, 0), i(2, 0), i(3, 0)},
                 {i(0, 1), i(1, 1), i(2, 1), i(3, 1)},
                 {i(0, 2), i(1, 2), i(2, 2), i(3, 2)},
                 {i(0, 3), i(1, 3), i(2, 3), i(3, 3)}};
}

HMatrix HMatrix::translate(float x, float y, float z) const noexcept {
  return HMatrixGen::translation(x, y, z) * (*this);
}

HMatrix HMatrix::rotateR(float x, float y, float z) const noexcept {
  return HMatrixGen::rotationR(x, y, z) * (*this);
}

HMatrix HMatrix::rotateD(float x, float y, float z) const noexcept {
  return HMatrixGen::rotationD(x, y, z) * (*this);
}

Vector<HPoint2> project2D(const HMatrix &matrix, const Vector<HPoint2> &input) {
  return flatten<2>(matrix * extrude<3>(input));
}

Vector<HPoint2> project2D(const HMatrix &matrix, const Vector<HPoint3> &input) {
  return flatten<2>(matrix * input);
}

HMatrix operator*(float scalar, const HMatrix &matrix) { return operator*(matrix, scalar); }

HMatrix operator*(const HMatrix &mat, float scalar) {
  HMatrix result;
  for (int i = 0; i < 16; i+=4) {
    result.matrix_[i] = mat.matrix_[i] * scalar;
    result.matrix_[i + 1] = mat.matrix_[i + 1] * scalar;
    result.matrix_[i + 2] = mat.matrix_[i + 2] * scalar;
    result.matrix_[i + 3] = mat.matrix_[i + 3] * scalar;
  }

  if (scalar == 0.0f) {
    C8_THROW_INVALID_ARGUMENT("Matrix is singular");
  }

  auto invScalar = 1.f / scalar;
  for (int i = 0; i < 16; i+=4) {
    result.inverseMatrix_[i] = mat.inverseMatrix_[i] * invScalar;
    result.inverseMatrix_[i + 1] = mat.inverseMatrix_[i + 1] * invScalar;
    result.inverseMatrix_[i + 2] = mat.inverseMatrix_[i + 2] * invScalar;
    result.inverseMatrix_[i + 3] = mat.inverseMatrix_[i + 3] * invScalar;
  }

  return result;
}

Vector<HPoint3> HMatrix::operator*(const Vector<HPoint3> &points) const noexcept {
  Vector<HPoint3> result;
  times(points, &result);
  return result;
}

void HMatrix::times(const Vector<HPoint3> &points, Vector<HPoint3> *result) const noexcept {
  auto numPoints = points.size();
  result->resize(numPoints);

  if (numPoints == 0) {
    // Nothing left to do but return an empty array if there are no points.
    return;
  }

  mat4x4Mul4xN(
    matrix_.data(), points.data()->raw().data(), result->data()->raw().data(), numPoints);
}

Vector<HVector3> HMatrix::operator*(const Vector<HVector3> &points) const noexcept {
  Vector<HVector3> result;
  times(points, &result);
  return result;
}

void HMatrix::times(const Vector<HVector3> &points, Vector<HVector3> *result) const noexcept {
  auto numPoints = points.size();
  result->resize(numPoints);

  mat4x4Mul4xN(
    matrix_.data(), points.data()->raw().data(), result->data()->raw().data(), numPoints);
}

HPoint3 HMatrix::operator*(HPoint3 pt) const noexcept {
  HPoint3 result;
  mat4x4Mul4x1(matrix_.data(), pt.raw().data(), result.raw().data());
  return result;
}

HVector3 HMatrix::operator*(HVector3 pt) const noexcept {
  HVector3 result;
  mat4x4Mul4x1(matrix_.data(), pt.raw().data(), result.raw().data());
  return result;
}

HMatrix HMatrix::operator*(const HMatrix &rhs) const noexcept {
  HMatrix result = HMatrixGen::i();

  mat4x4Mul4x4(matrix_.data(), rhs.matrix_.data(), result.matrix_.data());

  if (noInvert_ || rhs.noInvert_) {
    result.noInvert_ = true;
    result.invertFailed_ = invertFailed_ || rhs.invertFailed_;
    return result;
  }

  mat4x4Mul4x4(rhs.inverseMatrix_.data(), inverseMatrix_.data(), result.inverseMatrix_.data());
  return result;
}

std::ostream &operator<<(std::ostream &stream, const HMatrix &mat) noexcept {
  stream << "{{ " << mat(0, 0) << ", " << mat(0, 1) << ", " << mat(0, 2) << ", " << mat(0, 3)
         << " }" << std::endl;
  stream << " { " << mat(1, 0) << ", " << mat(1, 1) << ", " << mat(1, 2) << ", " << mat(1, 3)
         << " }" << std::endl;
  stream << " { " << mat(2, 0) << ", " << mat(2, 1) << ", " << mat(2, 2) << ", " << mat(2, 3)
         << " }" << std::endl;
  stream << " { " << mat(3, 0) << ", " << mat(3, 1) << ", " << mat(3, 2) << ", " << mat(3, 3)
         << " }}" << std::endl;

  return stream;
}

HMatrix HMatrixGen::xRadians(float rads) noexcept {
  float cose = std::cos(rads);
  float sine = std::sin(rads);
  float nsin = -sine;
  return HMatrix{{1.0f, 0.0f, 0.0f, 0.0f},
                 {0.0f, cose, nsin, 0.0f},
                 {0.0f, sine, cose, 0.0f},
                 {0.0f, 0.0f, 0.0f, 1.0f},
                 {1.0f, 0.0f, 0.0f, 0.0f},
                 {0.0f, cose, sine, 0.0f},
                 {0.0f, nsin, cose, 0.0f},
                 {0.0f, 0.0f, 0.0f, 1.0f}};
}

HMatrix HMatrixGen::yRadians(float rads) noexcept {
  float cose = std::cos(rads);
  float sine = std::sin(rads);
  float nsin = -sine;
  return HMatrix{{cose, 0.0f, sine, 0.0f},
                 {0.0f, 1.0f, 0.0f, 0.0f},
                 {nsin, 0.0f, cose, 0.0f},
                 {0.0f, 0.0f, 0.0f, 1.0f},
                 {cose, 0.0f, nsin, 0.0f},
                 {0.0f, 1.0f, 0.0f, 0.0f},
                 {sine, 0.0f, cose, 0.0f},
                 {0.0f, 0.0f, 0.0f, 1.0f}};
}

HMatrix HMatrixGen::zRadians(float rads) noexcept {
  float cose = std::cos(rads);
  float sine = std::sin(rads);
  float nsin = -sine;
  return HMatrix{{cose, nsin, 0.0f, 0.0f},
                 {sine, cose, 0.0f, 0.0f},
                 {0.0f, 0.0f, 1.0f, 0.0f},
                 {0.0f, 0.0f, 0.0f, 1.0f},
                 {cose, sine, 0.0f, 0.0f},
                 {nsin, cose, 0.0f, 0.0f},
                 {0.0f, 0.0f, 1.0f, 0.0f},
                 {0.0f, 0.0f, 0.0f, 1.0f}};
}

namespace {
float toRadians(float degrees) noexcept {
  static constexpr float pi = M_PI;
  return pi * degrees / 180.0f;
}
}  // namespace

HMatrix HMatrixGen::xDegrees(float degrees) noexcept {
  return HMatrixGen::xRadians(toRadians(degrees));
}

HMatrix HMatrixGen::yDegrees(float degrees) noexcept {
  return HMatrixGen::yRadians(toRadians(degrees));
}

HMatrix HMatrixGen::zDegrees(float degrees) noexcept {
  return HMatrixGen::zRadians(toRadians(degrees));
}

HMatrix HMatrixGen::z90() noexcept {
  return HMatrix{{0.0f, -1.0f, 0.0f, 0.0f},
                 {1.0f, 0.0f, 0.0f, 0.0f},
                 {0.0f, 0.0f, 1.0f, 0.0f},
                 {0.0f, 0.0f, 0.0f, 1.0f},
                 {0.0f, 1.0f, 0.0f, 0.0f},
                 {-1.0f, 0.0f, 0.0f, 0.0f},
                 {0.0f, 0.0f, 1.0f, 0.0f},
                 {0.0f, 0.0f, 0.0f, 1.0f}};
}

HMatrix HMatrixGen::z180() noexcept {
  return HMatrix{{-1.0f, 0.0f, 0.0f, 0.0f},
                 {0.0f, -1.0f, 0.0f, 0.0f},
                 {0.0f, 0.0f, 1.0f, 0.0f},
                 {0.0f, 0.0f, 0.0f, 1.0f},
                 {-1.0f, 0.0f, 0.0f, 0.0f},
                 {0.0f, -1.0f, 0.0f, 0.0f},
                 {0.0f, 0.0f, 1.0f, 0.0f},
                 {0.0f, 0.0f, 0.0f, 1.0f}};
}

HMatrix HMatrixGen::z270() noexcept {
  return HMatrix{{0.0f, 1.0f, 0.0f, 0.0f},
                 {-1.0f, 0.0f, 0.0f, 0.0f},
                 {0.0f, 0.0f, 1.0f, 0.0f},
                 {0.0f, 0.0f, 0.0f, 1.0f},
                 {0.0f, -1.0f, 0.0f, 0.0f},
                 {1.0f, 0.0f, 0.0f, 0.0f},
                 {0.0f, 0.0f, 1.0f, 0.0f},
                 {0.0f, 0.0f, 0.0f, 1.0f}};
}

HMatrix HMatrixGen::rotationR(float x, float y, float z) noexcept {
  float theta = std::sqrt(x * x + y * y + z * z);
  float cose = std::cos(theta);
  float sine = std::sin(theta);
  float cmi1 = 1.0f - cose;
  float invTheta = theta ? 1.0f / theta : 0.0f;

  float rx = invTheta * x;
  float ry = invTheta * y;
  float rz = invTheta * z;

  float ident[3][3]{
    {1.0f, 0.0f, 0.0f},  // Symmetric
    {0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 1.0f},
  };

  float rrt[3][3]{
    {rx * rx, rx * ry, rx * rz},  // Symmetric
    {rx * ry, ry * ry, ry * rz},
    {rx * rz, ry * rz, rz * rz},
  };

  float r_x[3][3]{
    {0.0f, rz, -ry},  // Transposed to col-major.
    {-rz, 0.0f, rx},
    {ry, -rx, 0.0f},
  };

  // Initialize result to identity matrix.
  HMatrix result = HMatrixGen::i();

  int row = 0;
  for (int x = 0; x < 3; ++x) {
    for (int y = 0; y < 3; ++y) {
      // Compute rotation elements.
      result.matrix_[row + y] = cose * ident[x][y] + cmi1 * rrt[x][y] + sine * r_x[x][y];

      // Rotation inverse is a transpose.
      result.inverseMatrix_[4 * y + x] = result.matrix_[row + y];
    }
    row += 4;
  }

  return result;
}

HMatrix HMatrixGen::rotationD(float x, float y, float z) noexcept {
  return HMatrixGen::rotationR(toRadians(x), toRadians(y), toRadians(z));
}

HMatrix HMatrixGen::cross(float x, float y, float z) noexcept {
  // non-invertable.
  return HMatrix{
    {0.0f, -z, y, 0.0f}, {z, 0.0f, -x, 0.0f}, {-y, x, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}, true};
}

float HMatrix::determinant() const noexcept {
  const auto &m = *this;
  auto m0 = m(0, 0);
  auto m1 = m(0, 1);
  auto m2 = m(0, 2);
  auto m3 = m(0, 3);
  return m0 * det3(m(1, 1), m(1, 2), m(1, 3), m(2, 1), m(2, 2), m(2, 3), m(3, 1), m(3, 2), m(3, 3))
    - m1 * det3(m(1, 0), m(1, 2), m(1, 3), m(2, 0), m(2, 2), m(2, 3), m(3, 0), m(3, 2), m(3, 3))
    + m2 * det3(m(1, 0), m(1, 1), m(1, 3), m(2, 0), m(2, 1), m(2, 3), m(3, 0), m(3, 1), m(3, 3))
    - m3 * det3(m(1, 0), m(1, 1), m(1, 2), m(2, 0), m(2, 1), m(2, 2), m(3, 0), m(3, 1), m(3, 2));
}

// Returns the HMatrix that rotates unit vector a so that it aligns to unit vector b
// Code based on https://math.stackexchange.com/a/476311/705989
HMatrix HMatrixGen::unitRotationAlignment(const HVector3 &a, const HVector3 &b) noexcept {
  const auto crossProduct = a.cross(b);
  const float length = crossProduct.l2Norm();
  const float dotProduct = a.dot(b);

  // The remaining part of this algorithm doesn't work if the vectors are pointing in the exact
  // same or opposite direction.  The following lines handle those cases
  if (dotProduct > 0.999f) {
    return HMatrixGen::i();
  }
  if (dotProduct < -0.999f) {
    return HMatrixGen::scale(-1.0f, -1.0f, -1.0f);
  }

  const float m = (1 - dotProduct) / std::pow(length, 2);
  const float x = crossProduct.x();
  const float y = crossProduct.y();
  const float z = crossProduct.z();

  const float mx = m * x;
  const float mxz = mx * z;
  const float my = m * y;
  const float myz = my * z;
  const float mxx = mx * x;
  const float myy = my * y;
  const float mzz = m * z * z;

  return {
    // clang-format off
    {1.0f - myy - mzz,             mxz - z,             mxz + y, 0.0f},
    {         mxz + z,    1.0f - mxx - mzz,             myz - x, 0.0f},
    {         mxz - y,             myz + x,    1.0f - mxx - myy, 0.0f},
    {            0.0f,                0.0f,                0.0f, 1.0f},
    // clang-format on
  };
}

HMatrix HMatrixGen::fromRotationAndTranslation(const HMatrix &R, const HVector3 &t) noexcept {
  return HMatrix{
    {R(0, 0), R(0, 1), R(0, 2), t.x()},
    {R(1, 0), R(1, 1), R(1, 2), t.y()},
    {R(2, 0), R(2, 1), R(2, 2), t.z()},
    {0.0f, 0.0f, 0.0f, 1.0f},
    {R(0, 0), R(1, 0), R(2, 0), - R(0, 0) * t.x() - R(1, 0) * t.y() - R(2, 0) * t.z()},
    {R(0, 1), R(1, 1), R(2, 1), - R(0, 1) * t.x() - R(1, 1) * t.y() - R(2, 1) * t.z()},
    {R(0, 2), R(1, 2), R(2, 2), - R(0, 2) * t.x() - R(1, 2) * t.y() - R(2, 2) * t.z()},
    {0.0f, 0.0f, 0.0f, 1.0f}
  };
}

String HMatrix::toString() const noexcept {
  std::stringstream out;
  out << *this;
  return out.str();
}

String HMatrix::toPrettyString() const noexcept {
  const auto &m = *this;
  std::stringstream out;
  out << "{{ " << pretty(m(0, 0)) << ", " << pretty(m(0, 1)) << ", " << pretty(m(0, 2)) << ", "
      << pretty(m(0, 3)) << " }" << std::endl;
  out << " { " << pretty(m(1, 0)) << ", " << pretty(m(1, 1)) << ", " << pretty(m(1, 2)) << ", "
      << pretty(m(1, 3)) << " }" << std::endl;
  out << " { " << pretty(m(2, 0)) << ", " << pretty(m(2, 1)) << ", " << pretty(m(2, 2)) << ", "
      << pretty(m(2, 3)) << " }" << std::endl;
  out << " { " << pretty(m(3, 0)) << ", " << pretty(m(3, 1)) << ", " << pretty(m(3, 2)) << ", "
      << pretty(m(3, 3)) << " }}" << std::endl;

  return out.str();
}

}  // namespace c8
