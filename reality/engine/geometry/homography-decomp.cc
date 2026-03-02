///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2019 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)
//
// Homography Decomp was originally forked from OpenCV.
//
// This is a homography decomposition implementation contributed to OpenCV
// by Samson Yilma. It implements the homography decomposition algorithm
// descriped in the research report:
// Malis, E and Vargas, M, "Deeper understanding of the homography decomposition
// for vision-based control", Research Report 6303, INRIA (2007)
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2014, Samson Yilma¸ (samson_yilma@yahoo.com), all rights reserved.
//
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "homography-decomp.h",
  };
  deps = {
    "//c8:hmatrix",
    "//c8:hpoint",
    "//c8:hvector",
    "//c8:quaternion",
    "@eigen3",
  };
}
cc_end(0x1db05145);

#include "reality/engine/geometry/homography-decomp.h"

#include <Eigen/Core>
#include <Eigen/SVD>

namespace c8 {

namespace {

// struct to hold solutions of homography decomposition
typedef struct _CameraMotion {
  HMatrix R = HMatrixGen::i();  //!< rotation matrix
  HVector3 t;                   //!< translation vector
  HVector3 n;                   //!< normal of the plane the camera is looking at
} CameraMotion;

constexpr int signf(const float x) { return x >= 0 ? 1 : -1; }

bool isZero3x3(const HMatrix &h) {
  float val = 0.0f;
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      val = std::max(val, std::abs(h(i, j)));
    }
  }
  return val < 1e-3;
}

HMatrix removeScale(const HMatrix &H) {
  // Using eigen for svd
  Eigen::Matrix3f m;
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      m(i, j) = H(i, j);
    }
  }
  Eigen::JacobiSVD<Eigen::Matrix3f> svd(m, Eigen::EigenvaluesOnly);
  return (1.0 / svd.singularValues()[1]) * H;
}

float oppositeOfMinor(const HMatrix &M, const int row, const int col) {
  int x1 = col == 0 ? 1 : 0;
  int x2 = col == 2 ? 1 : 2;
  int y1 = row == 0 ? 1 : 0;
  int y2 = row == 2 ? 1 : 2;

  return (M(y1, x2) * M(y2, x1) - M(y1, x1) * M(y2, x2));
}

// computes R = H( I - (2/v)*te_star*ne_t )
HMatrix findRmatFrom_tstar_n(
  const HMatrix &H, const HVector3 &t, const HVector3 &n, const float v) {
  float s = -2.0f / v;
  return H
    * HMatrix{{s * t.x() * n.x() + 1.0f, s * t.x() * n.y(), s * t.x() * n.z(), 0.0f},
              {s * t.y() * n.x(), s * t.y() * n.y() + 1.0f, s * t.y() * n.z(), 0.0f},
              {s * t.z() * n.x(), s * t.z() * n.y(), s * t.z() * n.z() + 1.0f, 0.0f},
              {0.00000000000000000f, 0.0000000000000000f, 0.0000000000000000f, 1.0f},
              true};
}

Vector<CameraMotion> decompose(const HMatrix &_H) {
  auto H = removeScale(_H);

  // S = H'H - I
  auto HH = H.t() * H;
  HMatrix S{{HH(0, 0) - 1.0f, HH(0, 1), HH(0, 2), 0.0f},
            {HH(1, 0), HH(1, 1) - 1.0f, HH(1, 2), 0.0f},
            {HH(2, 0), HH(2, 1), HH(2, 2) - 1.0f, 0.0f},
            {0.00000000f, 0.0000000f, 0.0000000f, 1.0f},
            true};

  // check if H is rotation matrix
  if (isZero3x3(S)) {
    return {{H, HVector3{0.0f, 0.0f, 0.0f}, HVector3{0.0f, 0.0f, 0.0f}}};
  }

  //! Compute nvectors
  float M00 = oppositeOfMinor(S, 0, 0);
  float M11 = oppositeOfMinor(S, 1, 1);
  float M22 = oppositeOfMinor(S, 2, 2);

  float rtM00 = std::sqrt(M00);
  float rtM11 = std::sqrt(M11);
  float rtM22 = std::sqrt(M22);

  float nS00 = std::abs(S(0, 0));
  float nS11 = std::abs(S(1, 1));
  float nS22 = std::abs(S(2, 2));

  // find max( |Sii| ), i=0, 1, 2
  int indx = 0;
  if (nS00 < nS11) {
    indx = 1;
    if (nS11 < nS22) {
      indx = 2;
    }
  } else if (nS00 < nS22) {
    indx = 2;
  }

  int s;
  HVector3 na;
  HVector3 nb;
  switch (indx) {
    case 0:
      s = signf(oppositeOfMinor(S, 1, 2));
      na = HVector3{S(0, 0), S(0, 1) + rtM22, S(0, 2) + s * rtM11}.unit();
      nb = HVector3{S(0, 0), S(0, 1) - rtM22, S(0, 2) - s * rtM11}.unit();
      break;
    case 1:
      s = signf(oppositeOfMinor(S, 0, 2));
      na = HVector3{S(0, 1) + rtM22, S(1, 1), S(1, 2) - s * rtM00}.unit();
      nb = HVector3{S(0, 1) - rtM22, S(1, 1), S(1, 2) + s * rtM00}.unit();
      break;
    case 2:
      s = signf(oppositeOfMinor(S, 0, 1));
      na = HVector3{S(0, 2) + s * rtM11, S(1, 2) + rtM00, S(2, 2)}.unit();
      nb = HVector3{S(0, 2) - s * rtM11, S(1, 2) - rtM00, S(2, 2)}.unit();
      break;
    default:
      break;
  }

  float traceS = S(0, 0) + S(1, 1) + S(2, 2);
  float v = 2.0 * std::sqrt(1 + traceS - M00 - M11 - M22);

  float r = std::sqrt(2 + traceS + v);
  float n_t = std::sqrt(2 + traceS - v);

  float half_nt = 0.5 * n_t;
  float esii_t_r = signf(S(indx, indx)) * r;

  auto ta_star = half_nt * (esii_t_r * nb - n_t * na);
  auto tb_star = half_nt * (esii_t_r * na - n_t * nb);

  // Ra, ta, na
  auto Ra = findRmatFrom_tstar_n(H, ta_star, na, v);
  auto ta = Ra * ta_star;

  // Rb, tb, nb
  auto Rb = findRmatFrom_tstar_n(H, tb_star, nb, v);
  auto tb = Rb * tb_star;

  return {
    {Ra, ta, na},
    {Ra, -ta, -na},
    {Rb, tb, nb},
    {Rb, -tb, -nb},
  };
}

}  // namespace

Vector<HomographyDecomp> decomposeHomographyMat(const HMatrix &h) {
  Vector<HomographyDecomp> cameraPoses;
  cameraPoses.reserve(4);
  for (const auto &m : decompose(h)) {
    cameraPoses.push_back({Quaternion::fromHMatrix(m.R), HPoint3{m.t.x(), m.t.y(), m.t.z()}, m.n});
  }

  return cameraPoses;
}

}  // namespace c8
