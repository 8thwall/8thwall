// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules.h"

cc_library {
  hdrs = {
    "convert.h",
  };
  deps = {
    "//bzl/inliner:rules", "//c8:hmatrix", "//c8:hpoint", "//third_party/cvlite/core:core",
  };
  visibility = {
    "//visibility:public",
  };
}

#include "c8/hmatrix.h"
#include "c8/cvlite/convert.h"

namespace c8 {

namespace {
constexpr float fc(double a) { return static_cast<float>(a); }
}

MatBackedInputArray::MatBackedInputArray(c8cv::Mat mat) : matrix(mat) {}

MatBackedInputArray::operator MatBackedInputArray::Type() const {
  return MatBackedInputArray::Type(matrix);
}

c8cv::Matx44f toMatx(const HMatrix &matrix) {
  // Return as a Matx44f, converting from column-major to row-major order.
  return c8cv::Matx44f(matrix.data().data()).t();
}

c8cv::Matx44d toMatx44d(const HMatrix &m) {
  return c8cv::Matx44d(
    m(0, 0), m(0, 1), m(0, 2), m(0, 3),   // Row 0
    m(1, 0), m(1, 1), m(1, 2), m(1, 3),   // Row 1
    m(2, 0), m(2, 1), m(2, 2), m(2, 3),   // Row 2
    m(3, 0), m(3, 1), m(3, 2), m(3, 3));  // Row 3
}

c8cv::Matx34f toMatx34f(const HMatrix &m) {
  return c8cv::Matx34f(
    m(0, 0), m(0, 1), m(0, 2), m(0, 3),   // Row 0
    m(1, 0), m(1, 1), m(1, 2), m(1, 3),   // Row 1
    m(2, 0), m(2, 1), m(2, 2), m(2, 3));  // Row 2
}

c8cv::Matx34d toMatx34d(const HMatrix &m) {
  return c8cv::Matx34d(
    m(0, 0), m(0, 1), m(0, 2), m(0, 3),   // Row 0
    m(1, 0), m(1, 1), m(1, 2), m(1, 3),   // Row 1
    m(2, 0), m(2, 1), m(2, 2), m(2, 3));  // Row 2
}

c8cv::Matx33f toMatx33f(const HMatrix &m) {
  return c8cv::Matx33f(
    m(0, 0), m(0, 1), m(0, 2),   // Row 0
    m(1, 0), m(1, 1), m(1, 2),   // Row 1
    m(2, 0), m(2, 1), m(2, 2));  // Row 2
}

c8cv::Matx33d toMatx33d(const HMatrix &m) {
  return c8cv::Matx33d(
    m(0, 0), m(0, 1), m(0, 2),   // Row 0
    m(1, 0), m(1, 1), m(1, 2),   // Row 1
    m(2, 0), m(2, 1), m(2, 2));  // Row 2
}

c8cv::Matx22f toMatx22f(const HMatrix &m) {
  return c8cv::Matx22f(
    m(0, 0), m(0, 1),   // Row 0
    m(1, 0), m(1, 1));  // Row 1
}

c8cv::Matx22d toMatx22d(const HMatrix &m) {
  return c8cv::Matx22d(
    m(0, 0), m(0, 1),   // Row 0
    m(1, 0), m(1, 1));  // Row 1
}

c8cv::Matx31f toMatx31f(const HMatrix &m) {
  return c8cv::Matx31f(
    m(0, 3),   // Row 0
    m(1, 3),   // Row 1
    m(2, 3));  // Row 2
}

c8cv::Matx31d toMatx31d(const HMatrix &m) {
  return c8cv::Matx31d(
    m(0, 3),   // Row 0
    m(1, 3),   // Row 1
    m(2, 3));  // Row 2
}

HMatrix toHMatrix(const c8cv::Matx44f &m) {
  return HMatrix{{m(0, 0), m(0, 1), m(0, 2), m(0, 3)},
                 {m(1, 0), m(1, 1), m(1, 2), m(1, 3)},
                 {m(2, 0), m(2, 1), m(2, 2), m(2, 3)},
                 {m(3, 0), m(3, 1), m(3, 2), m(3, 3)}};
}

HMatrix toHMatrix(const c8cv::Matx44d &m) {
  return HMatrix{{fc(m(0, 0)), fc(m(0, 1)), fc(m(0, 2)), fc(m(0, 3))},
                 {fc(m(1, 0)), fc(m(1, 1)), fc(m(1, 2)), fc(m(1, 3))},
                 {fc(m(2, 0)), fc(m(2, 1)), fc(m(2, 2)), fc(m(2, 3))},
                 {fc(m(3, 0)), fc(m(3, 1)), fc(m(3, 2)), fc(m(3, 3))}};
}

HMatrix toHMatrix(const c8cv::Matx33f &m) {
  return HMatrix{{m(0, 0), m(0, 1), m(0, 2), 0.0f},
                 {m(1, 0), m(1, 1), m(1, 2), 0.0f},
                 {m(2, 0), m(2, 1), m(2, 2), 0.0f},
                 {0.0000f, 0.0000f, 0.0000f, 1.0f}};
}

HMatrix toHMatrix(const c8cv::Matx33d &m) {
  return HMatrix{{fc(m(0, 0)), fc(m(0, 1)), fc(m(0, 2)), 0.0f},
                 {fc(m(1, 0)), fc(m(1, 1)), fc(m(1, 2)), 0.0f},
                 {fc(m(2, 0)), fc(m(2, 1)), fc(m(2, 2)), 0.0f},
                 {0.00000000f, 0.00000000f, 0.00000000f, 1.0f}};
}

HMatrix toHMatrix(const c8cv::Matx22f &m) {
  return HMatrix{{m(0, 0), m(0, 1), 0.0f, 0.0f},
                 {m(1, 0), m(1, 1), 0.0f, 0.0f},
                 {0.0000f, 0.0000f, 1.0f, 0.0f},
                 {0.0000f, 0.0000f, 0.0f, 1.0f}};
}

HMatrix toHMatrix(const c8cv::Matx22d &m) {
  return HMatrix{{fc(m(0, 0)), fc(m(0, 1)), 0.0f, 0.0f},
                 {fc(m(1, 0)), fc(m(1, 1)), 0.0f, 0.0f},
                 {0.00000000f, 0.00000000f, 1.0f, 0.0f},
                 {0.00000000f, 0.00000000f, 0.0f, 1.0f}};
}


c8cv::Affine3f toAffine3(const HMatrix &matrix) {
  // Return as an Affine3f, converting from column-major to row-major order.
  return c8cv::Affine3f(toMatx(matrix));
}

MatBackedInputArray asInputArray(const Vector<HPoint2> &points) {
  // Uses a const_cast to create a c8cv::Mat header, but maintains logical constness by returning a
  // read-only view of the c8cv::Mat through conversion to c8cv::InputArray;
  return MatBackedInputArray(c8cv::Mat(
    3,
    points.size(),
    CV_32F,
    static_cast<void *>(const_cast<float *>(points.data()->raw().data()))));
}

MatBackedInputArray asInputArray(const Vector<HPoint3> &points) {
  // Uses a const_cast to create a c8cv::Mat header, but maintains logical constness by returning a
  // read-only view of the c8cv::Mat through conversion to c8cv::InputArray;
  return MatBackedInputArray(c8cv::Mat(
    4,
    points.size(),
    CV_32F,
    static_cast<void *>(const_cast<float *>(points.data()->raw().data()))));
}

MatBackedInputArray as3ChanInputArray(const Vector<HPoint2> &points) {
  // Uses a const_cast to create a c8cv::Mat header, but maintains logical constness by returning a
  // read-only view of the c8cv::Mat through conversion to c8cv::InputArray;
  c8cv::Mat cvPoints = c8cv::Mat(
    3,
    points.size(),
    CV_32F,
    static_cast<void *>(const_cast<float *>(points.data()->raw().data())));
  return MatBackedInputArray(cvPoints.reshape(3));
}


MatBackedInputArray as4ChanInputArray(const Vector<HPoint3> &points) {
  // Uses a const_cast to create a c8cv::Mat header, but maintains logical constness by returning a
  // read-only view of the c8cv::Mat through conversion to c8cv::InputArray;
  c8cv::Mat cvPoints = c8cv::Mat(
    4,
    points.size(),
    CV_32F,
    static_cast<void *>(const_cast<float *>(points.data()->raw().data())));
  return MatBackedInputArray(cvPoints.reshape(4));
}

}  // namespace c8
