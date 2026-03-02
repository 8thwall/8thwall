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

#pragma once

#include <array>
#include "c8/hmatrix.h"
#include "c8/hpoint.h"

namespace c8 {

using Arr3 = std::array<double, 3>;
using Mat33 = std::array<Arr3, 3>;
using Mat43 = std::array<Arr3, 4>;
using Mat433 = std::array<Mat33, 4>;

class ap3p {
public:
  // Solve for 3 points, which returns 4 valid solutions. Image points should be in ray space.
  static std::array<HMatrix, 4> solve(
    const std::array<HPoint3, 3> &mapPts, const std::array<HPoint2, 3> &imPts);

  // Solve for 3 points, and then use a 4th point to disambiguate between the solutions. Image
  // points should be in ray space.
  static HMatrix solve(const std::array<HPoint3, 4> &mapPts, const std::array<HPoint2, 4> &imPts);

private:
  // clang-format off
  // Solve for three points, return the four ambgiuous potential solutions. For each point the
  // inputs are (mu, mv), a 2d point in pixel space and (x, y, z), a 3d point.
  int solve(Mat433 &R, Mat43 &t,
    double mu0, double mv0, double X0, double Y0, double Z0,
    double mu1, double mv1, double X1, double Y1, double Z1,
    double mu2, double mv2, double X2, double Y2, double Z2);

  // Solve for 4 points: Use the first three points to determine four solutions, and use the fourth
  // point to check for consistency. For each point the inputs are (mu, mv), a 2d point in pixel
  // space and (x, y, z), a 3d point.
  bool solve(Mat33 &R, Arr3 &t,
    double mu0, double mv0, double X0, double Y0, double Z0,
    double mu1, double mv1, double X1, double Y1, double Z1,
    double mu2, double mv2, double X2, double Y2, double Z2,
    double mu3, double mv3, double X3, double Y3, double Z3);
  // clang-format on

  // This algorithm is from "Tong Ke, Stergios Roumeliotis, An Efficient Algebraic Solution to the
  // Perspective-Three-Point Problem" (Accepted by CVPR 2017) See
  // https://arxiv.org/pdf/1701.08237.pdf featureVectors: 3 bearing measurements (normalized) stored
  // as column vectors worldPoints: Positions of the 3 feature points stored as column vectors
  // solutionsR: 4 possible solutions of rotation matrix of the world w.r.t the camera frame
  // solutionsT: 4 possible solutions of translation of the world origin w.r.t the camera frame
  int computePoses(
    const Mat33 &featureVectors,
    const Mat33 &worldPoints,
    Mat433 &solutionsR,
    Mat43 &solutionsT);

  ap3p() {}

  // Construct with a custom projection (K) matrix.
  ap3p(double _fx, double _fy, double _cx, double _cy)
      : fx(_fx),
        fy(_fy),
        cx(_cx),
        cy(_cy),
        inv_fx(1. / _fx),
        inv_fy(1. / _fy),
        cx_fx(_cx / _fx),
        cy_fy(_cy / _fy) {}

  double fx = 1.0, fy = 1.0, cx = 0.0, cy = 0.0;
  double inv_fx = 1.0, inv_fy = 1.0, cx_fx = 0.0, cy_fy = 0.0;
};

}  // namespace c8
