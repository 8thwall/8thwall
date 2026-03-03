// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "third_party/cvlite/calib3d/ptsetreg.h"
#include "third_party/cvlite/core/core.hpp"

namespace c8cv {

/** @brief Calculates an essential matrix from the corresponding points in two images.

@param points1 Array of N (N \>= 5) 2D points from the first image. The point coordinates should
be floating-point (single or double precision).
@param points2 Array of the second image points of the same size and format as points1 .
@param cameraMatrix Camera matrix \f$K = \vecthreethree{f_x}{0}{c_x}{0}{f_y}{c_y}{0}{0}{1}\f$ .
Note that this function assumes that points1 and points2 are feature points from cameras with the
same camera matrix.
@param method Method for computing a fundamental matrix.
-   **RANSAC** for the RANSAC algorithm.
-   **MEDS** for the LMedS algorithm.
@param prob Parameter used for the RANSAC or LMedS methods only. It specifies a desirable level of
confidence (probability) that the estimated matrix is correct.
@param threshold Parameter used for RANSAC. It is the maximum distance from a point to an epipolar
line in pixels, beyond which the point is considered an outlier and is not used for computing the
final fundamental matrix. It can be set to something like 1-3, depending on the accuracy of the
point localization, image resolution, and the image noise.
@param mask Output array of N elements, every element of which is set to 0 for outliers and to 1
for the other points. The array is computed only in the RANSAC and LMedS methods.

This function estimates essential matrix based on the five-point algorithm solver in @cite Nister03
.
@cite SteweniusCFS is also a related. The epipolar geometry is described by the following equation:

\f[[p_2; 1]^T K^{-T} E K^{-1} [p_1; 1] = 0\f]

where \f$E\f$ is an essential matrix, \f$p_1\f$ and \f$p_2\f$ are corresponding points in the first
and the second images, respectively. The result of this function may be passed further to
decomposeEssentialMat or recoverPose to recover the relative pose between cameras.
 */
CV_EXPORTS_W Mat findEssentialMat(
  InputArray points1,
  InputArray points2,
  InputArray cameraMatrix,
  int method = RANSAC,
  double prob = 0.999,
  double threshold = 1.0,
  OutputArray mask = noArray());

/** @overload
@param points1 Array of N (N \>= 5) 2D points from the first image. The point coordinates should
be floating-point (single or double precision).
@param points2 Array of the second image points of the same size and format as points1 .
@param focal focal length of the camera. Note that this function assumes that points1 and points2
are feature points from cameras with same focal length and principal point.
@param pp principal point of the camera.
@param method Method for computing a fundamental matrix.
-   **RANSAC** for the RANSAC algorithm.
-   **LMEDS** for the LMedS algorithm.
@param threshold Parameter used for RANSAC. It is the maximum distance from a point to an epipolar
line in pixels, beyond which the point is considered an outlier and is not used for computing the
final fundamental matrix. It can be set to something like 1-3, depending on the accuracy of the
point localization, image resolution, and the image noise.
@param prob Parameter used for the RANSAC or LMedS methods only. It specifies a desirable level of
confidence (probability) that the estimated matrix is correct.
@param mask Output array of N elements, every element of which is set to 0 for outliers and to 1
for the other points. The array is computed only in the RANSAC and LMedS methods.

This function differs from the one above that it computes camera matrix from focal length and
principal point:

\f[K =
\begin{bmatrix}
f & 0 & x_{pp}  \\
0 & f & y_{pp}  \\
0 & 0 & 1
\end{bmatrix}\f]
 */
CV_EXPORTS_W Mat findEssentialMat(
  InputArray points1,
  InputArray points2,
  double focal = 1.0,
  Point2d pp = Point2d(0, 0),
  int method = RANSAC,
  double prob = 0.999,
  double threshold = 1.0,
  OutputArray mask = noArray());

/** @overload
@param E The input essential matrix.
@param points1 Array of N 2D points from the first image. The point coordinates should be
floating-point (single or double precision).
@param points2 Array of the second image points of the same size and format as points1 .
@param R Recovered relative rotation.
@param t Recoverd relative translation.
@param focal Focal length of the camera. Note that this function assumes that points1 and points2
are feature points from cameras with same focal length and principal point.
@param pp principal point of the camera.
@param mask Input/output mask for inliers in points1 and points2.
:   If it is not empty, then it marks inliers in points1 and points2 for then given essential
matrix E. Only these inliers will be used to recover pose. In the output mask only inliers
which pass the cheirality check.

This function differs from the one above that it computes camera matrix from focal length and
principal point:

\f[K =
\begin{bmatrix}
f & 0 & x_{pp}  \\
0 & f & y_{pp}  \\
0 & 0 & 1
\end{bmatrix}\f]
 */
CV_EXPORTS_W int recoverPose(
  InputArray E,
  InputArray points1,
  InputArray points2,
  OutputArray R,
  OutputArray t,
  double focal = 1.0,
  Point2d pp = Point2d(0, 0),
  InputOutputArray mask = noArray());

/** @brief Recover relative camera rotation and translation from an estimated essential matrix and
the corresponding points in two images, using cheirality check. Returns the number of inliers which
pass the check.

@param E The input essential matrix.
@param points1 Array of N 2D points from the first image. The point coordinates should be
floating-point (single or double precision).
@param points2 Array of the second image points of the same size and format as points1 .
@param cameraMatrix Camera matrix \f$K = \vecthreethree{f_x}{0}{c_x}{0}{f_y}{c_y}{0}{0}{1}\f$ .
Note that this function assumes that points1 and points2 are feature points from cameras with the
same camera matrix.
@param R Recovered relative rotation.
@param t Recoverd relative translation.
@param mask Input/output mask for inliers in points1 and points2.
:   If it is not empty, then it marks inliers in points1 and points2 for then given essential
matrix E. Only these inliers will be used to recover pose. In the output mask only inliers
which pass the cheirality check.
This function decomposes an essential matrix using decomposeEssentialMat and then verifies possible
pose hypotheses by doing cheirality check. The cheirality check basically means that the
triangulated 3D points should have positive depth. Some details can be found in @cite Nister03 .

This function can be used to process output E and mask from findEssentialMat. In this scenario,
points1 and points2 are the same input for findEssentialMat. :
@code
    // Example. Estimation of fundamental matrix using the RANSAC algorithm
    int point_count = 100;
    vector<Point2f> points1(point_count);
    vector<Point2f> points2(point_count);

    // initialize the points here ...
    for( int i = 0; i < point_count; i++ )
    {
        points1[i] = ...;
        points2[i] = ...;
    }

    // cametra matrix with both focal lengths = 1, and principal point = (0, 0)
    Mat cameraMatrix = Mat::eye(3, 3, CV_64F);

    Mat E, R, t, mask;

    E = findEssentialMat(points1, points2, cameraMatrix, RANSAC, 0.999, 1.0, mask);
    recoverPose(E, points1, points2, cameraMatrix, R, t, mask);
@endcode
 */
CV_EXPORTS_W int recoverPose(
  InputArray E,
  InputArray points1,
  InputArray points2,
  InputArray cameraMatrix,
  OutputArray R,
  OutputArray t,
  InputOutputArray mask = noArray());

/** @brief Decompose an essential matrix to possible rotations and translation.

@param E The input essential matrix.
@param R1 One possible rotation matrix.
@param R2 Another possible rotation matrix.
@param t One possible translation.

This function decompose an essential matrix E using svd decomposition @cite HartleyZ00 . Generally 4
possible poses exists for a given E. They are \f$[R_1, t]\f$, \f$[R_1, -t]\f$, \f$[R_2, t]\f$,
\f$[R_2, -t]\f$. By decomposing E, you can only get the direction of the translation, so the
function returns unit t.
 */
CV_EXPORTS_W void decomposeEssentialMat(
  InputArray E, OutputArray R1, OutputArray R2, OutputArray t);

}  // namespace c8cv
