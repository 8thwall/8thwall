// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Conversions from c8 types to OpenCV types.

#pragma once

#include <type_traits>

#include "c8/vector.h"
#include "c8/hpoint.h"
#include "third_party/cvlite/core/affine.hpp"
#include "third_party/cvlite/core/core.hpp"

namespace c8 {

class HMatrix;

class MatBackedInputArray {
private:
  using Type = std::remove_const_t<std::remove_reference_t<c8cv::InputArray>>;

public:
  MatBackedInputArray() = delete;

  // Construct by taking ownership of a c8cv::Mat header.
  MatBackedInputArray(c8cv::Mat mat);

  // Implicit conversion to c8cv::InputArray.
  operator Type() const;

  // Default copy and assign.
  MatBackedInputArray(const MatBackedInputArray &mat) = default;
  MatBackedInputArray &operator=(const MatBackedInputArray &mat) = default;

private:
  c8cv::Mat matrix;
};

// Convert the HMatrix to a c8cv::Matx44f, changing the internal storage from column-major to
// row-major.
c8cv::Matx44f toMatx(const HMatrix &matrix);

// Convert the upper-left submatrix of the HMatrix to a c8cv::Matx, changing the internal storage from
// column-major to row-major.
c8cv::Matx44d toMatx44d(const HMatrix &matrix);
c8cv::Matx34f toMatx34f(const HMatrix &matrix);
c8cv::Matx34d toMatx34d(const HMatrix &matrix);
c8cv::Matx33f toMatx33f(const HMatrix &matrix);
c8cv::Matx33d toMatx33d(const HMatrix &matrix);
c8cv::Matx22f toMatx22f(const HMatrix &matrix);
c8cv::Matx22d toMatx22d(const HMatrix &matrix);

// Convert the upper right submatrix of the HMatrix to a c8cv::Matx; this can be used to extract the
// translational component of the matrix.
c8cv::Matx31f toMatx31f(const HMatrix &matrix);
c8cv::Matx31d toMatx31d(const HMatrix &matrix);

// Convert the c8cv::Mat to an HMatrix, changing the internal storage from row-major to
// column-major.
HMatrix toHMatrix(const c8cv::Matx44f &matrix);
HMatrix toHMatrix(const c8cv::Matx44d &matrix);
HMatrix toHMatrix(const c8cv::Matx33f &matrix);
HMatrix toHMatrix(const c8cv::Matx33d &matrix);
HMatrix toHMatrix(const c8cv::Matx22f &matrix);
HMatrix toHMatrix(const c8cv::Matx22d &matrix);

// Convert the HMatrix to an c8cv::Affine3f, changing the internal storage from column-major to
// row-major.
c8cv::Affine3f toAffine3(const HMatrix &matrix);

// Return a read-only wrapper to the HPoint vector as a c8cv::Mat.
MatBackedInputArray asInputArray(const Vector<HPoint2>& points);
MatBackedInputArray asInputArray(const Vector<HPoint3>& points);

// Return a read-only wrapper to the HPoint vector as a c8cv::Mat, where each Mat element is a D-channel homogeneous vector.
MatBackedInputArray as3ChanInputArray(const Vector<HPoint2> &points);
MatBackedInputArray as4ChanInputArray(const Vector<HPoint3> &points);

}  // namespace c8
