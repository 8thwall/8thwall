// Copyright (c) 2019 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)
//
// ap3p was originally forked from OpenCV.
//
// Software License Agreement (BSD License)
//
//  Copyright (c) 2009, Willow Garage, Inc.
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without modification, are permitted
//  provided that the following conditions are met:
//
//   * Redistributions of source code must retain the above copyright notice, this list of
//     conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright notice, this list of
//     conditions and the following disclaimer in the documentation and/or other materials provided
//     with the distribution.
//   * Neither the name of the Willow Garage nor the names of its contributors may be used to
//     endorse or promote products derived from this software without specific prior written
//     permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
//  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
//  FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
//  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
//  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "ap3p.h",
  };
  deps = {
    "//c8:hmatrix",
    "//c8:hpoint",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xf9a9d511);

#include "reality/engine/geometry/ap3p.h"

#include <cmath>
#include <complex>
#if defined(_MSC_VER) && (_MSC_VER <= 1700)
static inline double cbrt(double x) { return (double)c8cv::cubeRoot((float)x); };
#endif

namespace c8 {

using Arr5 = std::array<double, 5>;
using Arr4 = std::array<double, 4>;

using namespace std;

namespace {

constexpr complex<double> cdiv(complex<double> l, complex<double>r) {
  double a = l.real();
  double b = l.imag();
  double c = r.real();
  double d = r.imag();
  if (b == 0 && d == 0) {
    return {a / c, 0.0};
  }

  double ac = a * c;
  double bc = b * c;
  double ad = a * d;
  double bd = b * d;
  double cc = c * c;
  double dd = d * d;
  double ic2d2 = 1.0 / (cc + dd);
  return {(ac + bd) * ic2d2, (bc - ad) * ic2d2};
}

void solveQuartic(const Arr5 &factors, Arr4 &realRoots) {
  const double &a4 = factors[0];
  const double &a3 = factors[1];
  const double &a2 = factors[2];
  const double &a1 = factors[3];
  const double &a0 = factors[4];

  double a4_2 = a4 * a4;
  double a3_2 = a3 * a3;
  double a4_3 = a4_2 * a4;
  double a2a4 = a2 * a4;

  double p4 = (8 * a2a4 - 3 * a3_2) / (8 * a4_2);
  double q4 = (a3_2 * a3 - 4 * a2a4 * a3 + 8 * a1 * a4_2) / (8 * a4_3);
  double r4 = (256 * a0 * a4_3 - 3 * (a3_2 * a3_2) - 64 * a1 * a3 * a4_2 + 16 * a2a4 * a3_2)
    / (256 * (a4_3 * a4));

  double p3 = ((p4 * p4) / 12 + r4) / 3;                               // /=-3
  double q3 = (72 * r4 * p4 - 2 * p4 * p4 * p4 - 27 * q4 * q4) / 432;  // /=2

  double t;  // *=2
  complex<double> w{};
  if (q3 >= 0) {
    w = -sqrt(static_cast<complex<double> >(q3 * q3 - p3 * p3 * p3)) - q3;
  } else {
    w = sqrt(static_cast<complex<double> >(q3 * q3 - p3 * p3 * p3)) - q3;
  }

  if (w.imag() == 0.0) {
    w.real(cbrt(w.real()));
    t = 2.0 * (w.real() + p3 / w.real());
  } else {
    w = pow(w, 1.0 / 3);
    t = 4.0 * w.real();
  }

  complex<double> sqrt_2m = sqrt(static_cast<complex<double> >(-2 * p4 / 3 + t));
  double B_4A = -a3 / (4 * a4);
  double complex1 = 4 * p4 / 3 + t;
  /*
#if defined(__clang__) && defined(__arm__) && (__clang_major__ == 3 || __clang_minor__ == 4) \
  && !defined(__ANDROID__)
  // details: https://github.com/opencv/opencv/issues/11135
  // details: https://github.com/opencv/opencv/issues/11056
  complex<double> complex2 = 2 * q4;
  complex2 = complex<double>(complex2.real() / sqrt_2m.real(), 0);
#else
  complex<double> complex2 = 2 * q4 / sqrt_2m;
#endif
  */
  // NOTE(nb): The complex<double> / operator led to code around here crashing in web assembly as of
  // 2/2019, possibly related to the linked bugs which are referred to in the original code above.
  complex<double> complex2 = cdiv(2 * q4, sqrt_2m);
  double sqrt_2m_rh = sqrt_2m.real() / 2;
  double sqrt1 = sqrt(-(complex1 + complex2)).real() / 2;
  realRoots[0] = B_4A + sqrt_2m_rh + sqrt1;
  realRoots[1] = B_4A + sqrt_2m_rh - sqrt1;
  double sqrt2 = sqrt(-(complex1 - complex2)).real() / 2;
  realRoots[2] = B_4A - sqrt_2m_rh + sqrt2;
  realRoots[3] = B_4A - sqrt_2m_rh - sqrt2;
}

void polishQuarticRoots(const Arr5 &coeffs, Arr4 &roots) {
  const int iterations = 2;
  for (int i = 0; i < iterations; ++i) {
    for (int j = 0; j < 4; ++j) {
      double error =
        (((coeffs[0] * roots[j] + coeffs[1]) * roots[j] + coeffs[2]) * roots[j] + coeffs[3])
          * roots[j]
        + coeffs[4];
      double derivative =
        ((4 * coeffs[0] * roots[j] + 3 * coeffs[1]) * roots[j] + 2 * coeffs[2]) * roots[j]
        + coeffs[3];
      roots[j] -= error / derivative;
    }
  }
}

inline void vect_cross(const Arr3 &a, const Arr3 &b, Arr3 &result) {
  result[0] = a[1] * b[2] - a[2] * b[1];
  result[1] = -(a[0] * b[2] - a[2] * b[0]);
  result[2] = a[0] * b[1] - a[1] * b[0];
}

inline double vect_dot(const Arr3 &a, const Arr3 &b) {
  return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

inline double vect_norm(const Arr3 &a) { return sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]); }

inline void vect_scale(const double s, const Arr3 &a, Arr3 &result) {
  result[0] = a[0] * s;
  result[1] = a[1] * s;
  result[2] = a[2] * s;
}

inline void vect_sub(const Arr3 &a, const Arr3 &b, Arr3 &result) {
  result[0] = a[0] - b[0];
  result[1] = a[1] - b[1];
  result[2] = a[2] - b[2];
}

inline void vect_divide(const Arr3 &a, const double d, Arr3 &result) {
  result[0] = a[0] / d;
  result[1] = a[1] / d;
  result[2] = a[2] / d;
}

inline void mat_mult(const Mat33 &a, const Mat33 &b, Mat33 &result) {
  result[0][0] = a[0][0] * b[0][0] + a[0][1] * b[1][0] + a[0][2] * b[2][0];
  result[0][1] = a[0][0] * b[0][1] + a[0][1] * b[1][1] + a[0][2] * b[2][1];
  result[0][2] = a[0][0] * b[0][2] + a[0][1] * b[1][2] + a[0][2] * b[2][2];

  result[1][0] = a[1][0] * b[0][0] + a[1][1] * b[1][0] + a[1][2] * b[2][0];
  result[1][1] = a[1][0] * b[0][1] + a[1][1] * b[1][1] + a[1][2] * b[2][1];
  result[1][2] = a[1][0] * b[0][2] + a[1][1] * b[1][2] + a[1][2] * b[2][2];

  result[2][0] = a[2][0] * b[0][0] + a[2][1] * b[1][0] + a[2][2] * b[2][0];
  result[2][1] = a[2][0] * b[0][1] + a[2][1] * b[1][1] + a[2][2] * b[2][1];
  result[2][2] = a[2][0] * b[0][2] + a[2][1] * b[1][2] + a[2][2] * b[2][2];
}
}  // namespace

// This algorithm is from "Tong Ke, Stergios Roumeliotis, An Efficient Algebraic Solution to the
// Perspective-Three-Point Problem" (Accepted by CVPR 2017) See https://arxiv.org/pdf/1701.08237.pdf
// featureVectors: The 3 bearing measurements (normalized) stored as column vectors
// worldPoints: The positions of the 3 feature points stored as column vectors
// solutionsR: 4 possible solutions of rotation matrix of the world w.r.t the camera frame
// solutionsT: 4 possible solutions of translation of the world origin w.r.t the camera frame
int ap3p::computePoses(
  const Mat33 &featureVectors, const Mat33 &worldPoints, Mat433 &solutionsR, Mat43 &solutionsT) {
  // world point vectors
  Arr3 w1 = {worldPoints[0][0], worldPoints[1][0], worldPoints[2][0]};
  Arr3 w2 = {worldPoints[0][1], worldPoints[1][1], worldPoints[2][1]};
  Arr3 w3 = {worldPoints[0][2], worldPoints[1][2], worldPoints[2][2]};
  // k1
  Arr3 u0{};
  vect_sub(w1, w2, u0);

  double nu0 = vect_norm(u0);
  Arr3 k1{};
  vect_divide(u0, nu0, k1);
  // bi
  Arr3 b1 = {featureVectors[0][0], featureVectors[1][0], featureVectors[2][0]};
  Arr3 b2 = {featureVectors[0][1], featureVectors[1][1], featureVectors[2][1]};
  Arr3 b3 = {featureVectors[0][2], featureVectors[1][2], featureVectors[2][2]};
  // k3,tz
  Arr3 k3{};
  vect_cross(b1, b2, k3);
  double nk3 = vect_norm(k3);
  vect_divide(k3, nk3, k3);

  Arr3 tz{};
  vect_cross(b1, k3, tz);
  // ui,vi
  Arr3 v1{};
  vect_cross(b1, b3, v1);
  Arr3 v2{};
  vect_cross(b2, b3, v2);

  Arr3 u1{};
  vect_sub(w1, w3, u1);
  // coefficients related terms
  double u1k1 = vect_dot(u1, k1);
  double k3b3 = vect_dot(k3, b3);
  // f1i
  double f11 = k3b3;
  double f13 = vect_dot(k3, v1);
  double f15 = -u1k1 * f11;
  // delta
  Arr3 nl{};
  vect_cross(u1, k1, nl);
  double delta = vect_norm(nl);
  vect_divide(nl, delta, nl);
  f11 *= delta;
  f13 *= delta;
  // f2i
  double u2k1 = u1k1 - nu0;
  double f21 = vect_dot(tz, v2);
  double f22 = nk3 * k3b3;
  double f23 = vect_dot(k3, v2);
  double f24 = u2k1 * f22;
  double f25 = -u2k1 * f21;
  f21 *= delta;
  f22 *= delta;
  f23 *= delta;
  double g1 = f13 * f22;
  double g2 = f13 * f25 - f15 * f23;
  double g3 = f11 * f23 - f13 * f21;
  double g4 = -f13 * f24;
  double g5 = f11 * f22;
  double g6 = f11 * f25 - f15 * f21;
  double g7 = -f15 * f24;
  Arr5 coeffs = {g5 * g5 + g1 * g1 + g3 * g3,
                 2 * (g5 * g6 + g1 * g2 + g3 * g4),
                 g6 * g6 + 2 * g5 * g7 + g2 * g2 + g4 * g4 - g1 * g1 - g3 * g3,
                 2 * (g6 * g7 - g1 * g2 - g3 * g4),
                 g7 * g7 - g2 * g2 - g4 * g4};
  Arr4 s{};
  solveQuartic(coeffs, s);
  polishQuarticRoots(coeffs, s);

  Arr3 temp{};
  vect_cross(k1, nl, temp);

  Mat33 Ck1nl = {
    Arr3{k1[0], nl[0], temp[0]}, Arr3{k1[1], nl[1], temp[1]}, Arr3{k1[2], nl[2], temp[2]}};

  Mat33 Cb1k3tzT = {
    Arr3{b1[0], b1[1], b1[2]}, Arr3{k3[0], k3[1], k3[2]}, Arr3{tz[0], tz[1], tz[2]}};

  Arr3 b3p{};
  vect_scale((delta / k3b3), b3, b3p);

  int nb_solutions = 0;
  for (int i = 0; i < 4; ++i) {
    double ctheta1p = s[i];
    if (abs(ctheta1p) > 1) {
      continue;
    }
    double stheta1p = sqrt(1 - ctheta1p * ctheta1p);
    stheta1p = (k3b3 > 0) ? stheta1p : -stheta1p;
    double ctheta3 = g1 * ctheta1p + g2;
    double stheta3 = g3 * ctheta1p + g4;
    double ntheta3 = stheta1p / ((g5 * ctheta1p + g6) * ctheta1p + g7);
    ctheta3 *= ntheta3;
    stheta3 *= ntheta3;

    Mat33 C13 = {Arr3{ctheta3, 0, -stheta3},
                 Arr3{stheta1p * stheta3, ctheta1p, stheta1p * ctheta3},
                 Arr3{ctheta1p * stheta3, -stheta1p, ctheta1p * ctheta3}};

    Mat33 temp_matrix{};
    Mat33 R{};
    mat_mult(Ck1nl, C13, temp_matrix);
    mat_mult(temp_matrix, Cb1k3tzT, R);

    // R' * p3
    Arr3 rp3 = {w3[0] * R[0][0] + w3[1] * R[1][0] + w3[2] * R[2][0],
                w3[0] * R[0][1] + w3[1] * R[1][1] + w3[2] * R[2][1],
                w3[0] * R[0][2] + w3[1] * R[1][2] + w3[2] * R[2][2]};

    Arr3 pxstheta1p{};
    vect_scale(stheta1p, b3p, pxstheta1p);

    vect_sub(pxstheta1p, rp3, solutionsT[nb_solutions]);

    solutionsR[nb_solutions][0][0] = R[0][0];
    solutionsR[nb_solutions][1][0] = R[0][1];
    solutionsR[nb_solutions][2][0] = R[0][2];
    solutionsR[nb_solutions][0][1] = R[1][0];
    solutionsR[nb_solutions][1][1] = R[1][1];
    solutionsR[nb_solutions][2][1] = R[1][2];
    solutionsR[nb_solutions][0][2] = R[2][0];
    solutionsR[nb_solutions][1][2] = R[2][1];
    solutionsR[nb_solutions][2][2] = R[2][2];

    nb_solutions++;
  }

  return nb_solutions;
}

namespace {
HMatrix toHMatrix(const Mat33 &r, const Arr3 &t) {
  // clang-format off
  return HMatrixGen::translation(t[0], t[1], t[2]) * HMatrix{
    {static_cast<float>(r[0][0]), static_cast<float>(r[0][1]), static_cast<float>(r[0][2]), 0.0f},
    {static_cast<float>(r[1][0]), static_cast<float>(r[1][1]), static_cast<float>(r[1][2]), 0.0f},
    {static_cast<float>(r[2][0]), static_cast<float>(r[2][1]), static_cast<float>(r[2][2]), 0.0f},
    {0.000000000000000000000000f, 0.000000000000000000000000f, 0.000000000000000000000000f, 1.0f},
    {static_cast<float>(r[0][0]), static_cast<float>(r[1][0]), static_cast<float>(r[2][0]), 0.0f},
    {static_cast<float>(r[0][1]), static_cast<float>(r[1][1]), static_cast<float>(r[2][1]), 0.0f},
    {static_cast<float>(r[0][2]), static_cast<float>(r[1][2]), static_cast<float>(r[2][2]), 0.0f},
    {0.000000000000000000000000f, 0.000000000000000000000000f, 0.000000000000000000000000f, 1.0f}};
  // clang-format on
}
}  // namespace

std::array<HMatrix, 4> ap3p::solve(
  const std::array<HPoint3, 3> &mapPts, const std::array<HPoint2, 3> &imPts) {
  Mat433 r{};
  Mat43 t{};

  // clang-format off
  int n = ap3p().solve(r, t,
    imPts[0].x(), imPts[0].y(), mapPts[0].x(), mapPts[0].y(), mapPts[0].z(),
    imPts[1].x(), imPts[1].y(), mapPts[1].x(), mapPts[1].y(), mapPts[1].z(),
    imPts[2].x(), imPts[2].y(), mapPts[2].x(), mapPts[2].y(), mapPts[2].z());
  // clang-format on

  return {
    n > 0 ? toHMatrix(r[0], t[0]).inv() : HMatrixGen::i(),
    n > 1 ? toHMatrix(r[1], t[1]).inv() : HMatrixGen::i(),
    n > 2 ? toHMatrix(r[2], t[2]).inv() : HMatrixGen::i(),
    n > 3 ? toHMatrix(r[3], t[3]).inv() : HMatrixGen::i(),
  };
}

HMatrix ap3p::solve(const std::array<HPoint3, 4> &mapPts, const std::array<HPoint2, 4> &imPts) {
  Mat33 r{};
  Arr3 t{};

  // clang-format off
  if (ap3p().solve(r, t,
    imPts[0].x(), imPts[0].y(), mapPts[0].x(), mapPts[0].y(), mapPts[0].z(),
    imPts[1].x(), imPts[1].y(), mapPts[1].x(), mapPts[1].y(), mapPts[1].z(),
    imPts[2].x(), imPts[2].y(), mapPts[2].x(), mapPts[2].y(), mapPts[2].z(),
    imPts[3].x(), imPts[3].y(), mapPts[3].x(), mapPts[3].y(), mapPts[3].z())) {
    return toHMatrix(r, t).inv();
  }
  // clang-format on

  return HMatrixGen::i();
}

// clang-format off
bool ap3p::solve(Mat33 &R, Arr3 &t,
  double mu0, double mv0, double X0, double Y0, double Z0,
  double mu1, double mv1, double X1, double Y1, double Z1,
  double mu2, double mv2, double X2, double Y2, double Z2,
  double mu3, double mv3, double X3, double Y3, double Z3) {
  // clang-format on
  Mat433 Rs{};
  Mat43 ts{};

  int n = solve(Rs, ts, mu0, mv0, X0, Y0, Z0, mu1, mv1, X1, Y1, Z1, mu2, mv2, X2, Y2, Z2);
  if (n == 0) {
    return false;
  }

  int ns = 0;
  double min_reproj = 0;
  for (int i = 0; i < n; i++) {
    double X3p = Rs[i][0][0] * X3 + Rs[i][0][1] * Y3 + Rs[i][0][2] * Z3 + ts[i][0];
    double Y3p = Rs[i][1][0] * X3 + Rs[i][1][1] * Y3 + Rs[i][1][2] * Z3 + ts[i][1];
    double Z3p = Rs[i][2][0] * X3 + Rs[i][2][1] * Y3 + Rs[i][2][2] * Z3 + ts[i][2];
    double mu3p = cx + fx * X3p / Z3p;
    double mv3p = cy + fy * Y3p / Z3p;
    double reproj = (mu3p - mu3) * (mu3p - mu3) + (mv3p - mv3) * (mv3p - mv3);
    if (i == 0 || min_reproj > reproj) {
      ns = i;
      min_reproj = reproj;
    }
  }

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      R[i][j] = Rs[ns][i][j];
    }
    t[i] = ts[ns][i];
  }

  return true;
}

// clang-format off
int ap3p::solve(Mat433 &R, Mat43 &t, double mu0, double mv0, double X0, double Y0,
  double Z0, double mu1, double mv1, double X1, double Y1, double Z1, double mu2, double mv2,
  double X2, double Y2, double Z2) {
  // clang-format on
  double mk0 = 0;
  double mk1 = 0;
  double mk2 = 0;
  double norm = 0;

  mu0 = inv_fx * mu0 - cx_fx;
  mv0 = inv_fy * mv0 - cy_fy;
  norm = sqrt(mu0 * mu0 + mv0 * mv0 + 1);
  mk0 = 1. / norm;
  mu0 *= mk0;
  mv0 *= mk0;

  mu1 = inv_fx * mu1 - cx_fx;
  mv1 = inv_fy * mv1 - cy_fy;
  norm = sqrt(mu1 * mu1 + mv1 * mv1 + 1);
  mk1 = 1. / norm;
  mu1 *= mk1;
  mv1 *= mk1;

  mu2 = inv_fx * mu2 - cx_fx;
  mv2 = inv_fy * mv2 - cy_fy;
  norm = sqrt(mu2 * mu2 + mv2 * mv2 + 1);
  mk2 = 1. / norm;
  mu2 *= mk2;
  mv2 *= mk2;

  Mat33 featureVectors = {Arr3{mu0, mu1, mu2}, Arr3{mv0, mv1, mv2}, Arr3{mk0, mk1, mk2}};
  Mat33 worldPoints = {Arr3{X0, X1, X2}, Arr3{Y0, Y1, Y2}, Arr3{Z0, Z1, Z2}};

  return computePoses(featureVectors, worldPoints, R, t);
}

}  // namespace c8
