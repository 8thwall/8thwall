// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "third_party/cvlite/core/core.hpp"

#pragma once

namespace c8cv {

enum {
  SOLVEPNP_ITERATIVE = 0,
  SOLVEPNP_EPNP =
    1,  //!< EPnP: Efficient Perspective-n-Point Camera Pose Estimation @cite lepetit2009epnp
  SOLVEPNP_P3P = 2,  //!< Complete Solution Classification for the Perspective-Three-Point Problem
                     //!< @cite gao2003complete
  SOLVEPNP_DLS = 3,  //!< A Direct Least-Squares (DLS) Method for PnP  @cite hesch2011direct
  SOLVEPNP_UPNP = 4  //!< Exhaustive Linearization for Robust Camera Pose and Focal Length
                     //!< Estimation @cite penate2013exhaustive

};

/** @brief Finds an object pose from 3D-2D point correspondences.

@param objectPoints Array of object points in the object coordinate space, Nx3 1-channel or
1xN/Nx1 3-channel, where N is the number of points. vector\<Point3f\> can be also passed here.
@param imagePoints Array of corresponding image points, Nx2 1-channel or 1xN/Nx1 2-channel,
where N is the number of points. vector\<Point2f\> can be also passed here.
@param cameraMatrix Input camera matrix \f$A = \vecthreethree{fx}{0}{cx}{0}{fy}{cy}{0}{0}{1}\f$ .
@param distCoeffs Input vector of distortion coefficients
\f$(k_1, k_2, p_1, p_2[, k_3[, k_4, k_5, k_6 [, s_1, s_2, s_3, s_4[, \tau_x, \tau_y]]]])\f$ of
4, 5, 8, 12 or 14 elements. If the vector is NULL/empty, the zero distortion coefficients are
assumed.
@param rvec Output rotation vector (see Rodrigues ) that, together with tvec , brings points from
the model coordinate system to the camera coordinate system.
@param tvec Output translation vector.
@param useExtrinsicGuess Parameter used for SOLVEPNP_ITERATIVE. If true (1), the function uses
the provided rvec and tvec values as initial approximations of the rotation and translation
vectors, respectively, and further optimizes them.
@param flags Method for solving a PnP problem:
-   **SOLVEPNP_ITERATIVE** Iterative method is based on Levenberg-Marquardt optimization. In
this case the function finds such a pose that minimizes reprojection error, that is the sum
of squared distances between the observed projections imagePoints and the projected (using
projectPoints ) objectPoints .
-   **SOLVEPNP_P3P** Method is based on the paper of X.S. Gao, X.-R. Hou, J. Tang, H.-F. Chang
"Complete Solution Classification for the Perspective-Three-Point Problem". In this case the
function requires exactly four object and image points.
-   **SOLVEPNP_EPNP** Method has been introduced by F.Moreno-Noguer, V.Lepetit and P.Fua in the
paper "EPnP: Efficient Perspective-n-Point Camera Pose Estimation".
-   **SOLVEPNP_DLS** Method is based on the paper of Joel A. Hesch and Stergios I. Roumeliotis.
"A Direct Least-Squares (DLS) Method for PnP".
-   **SOLVEPNP_UPNP** Method is based on the paper of A.Penate-Sanchez, J.Andrade-Cetto,
F.Moreno-Noguer. "Exhaustive Linearization for Robust Camera Pose and Focal Length
Estimation". In this case the function also estimates the parameters \f$f_x\f$ and \f$f_y\f$
assuming that both have the same value. Then the cameraMatrix is updated with the estimated
focal length.

The function estimates the object pose given a set of object points, their corresponding image
projections, as well as the camera matrix and the distortion coefficients.

@note
   -   An example of how to use solvePnP for planar augmented reality can be found at
        opencv_source_code/samples/python/plane_ar.py
   -   If you are using Python:
        - Numpy array slices won't work as input because solvePnP requires contiguous
        arrays (enforced by the assertion using cv::Mat::checkVector() around line 55 of
        modules/calib3d/src/solvepnp.cpp version 2.4.9)
        - The P3P algorithm requires image points to be in an array of shape (N,1,2) due
        to its calling of cv::undistortPoints (around line 75 of modules/calib3d/src/solvepnp.cpp
version 2.4.9) which requires 2-channel information.
        - Thus, given some data D = np.array(...) where D.shape = (N,M), in order to use a subset of
        it as, e.g., imagePoints, one must effectively copy it into a new array: imagePoints =
        np.ascontiguousarray(D[:,:2]).reshape((N,1,2))
   -   The methods **SOLVEPNP_DLS** and **SOLVEPNP_UPNP** cannot be used as the current
implementations are unstable and sometimes give completly wrong results. If you pass one of these
two flags,
       **SOLVEPNP_EPNP** method will be used instead.
 */
CV_EXPORTS_W bool solvePnP(
  InputArray objectPoints,
  InputArray imagePoints,
  InputArray cameraMatrix,
  InputArray distCoeffs,
  OutputArray rvec,
  OutputArray tvec,
  bool useExtrinsicGuess = false,
  int flags = SOLVEPNP_ITERATIVE);

/** @brief Finds an object pose from 3D-2D point correspondences using the RANSAC scheme.

@param objectPoints Array of object points in the object coordinate space, Nx3 1-channel or
1xN/Nx1 3-channel, where N is the number of points. vector\<Point3f\> can be also passed here.
@param imagePoints Array of corresponding image points, Nx2 1-channel or 1xN/Nx1 2-channel,
where N is the number of points. vector\<Point2f\> can be also passed here.
@param cameraMatrix Input camera matrix \f$A = \vecthreethree{fx}{0}{cx}{0}{fy}{cy}{0}{0}{1}\f$ .
@param distCoeffs Input vector of distortion coefficients
\f$(k_1, k_2, p_1, p_2[, k_3[, k_4, k_5, k_6 [, s_1, s_2, s_3, s_4[, \tau_x, \tau_y]]]])\f$ of
4, 5, 8, 12 or 14 elements. If the vector is NULL/empty, the zero distortion coefficients are
assumed.
@param rvec Output rotation vector (see Rodrigues ) that, together with tvec , brings points from
the model coordinate system to the camera coordinate system.
@param tvec Output translation vector.
@param useExtrinsicGuess Parameter used for SOLVEPNP_ITERATIVE. If true (1), the function uses
the provided rvec and tvec values as initial approximations of the rotation and translation
vectors, respectively, and further optimizes them.
@param iterationsCount Number of iterations.
@param reprojectionError Inlier threshold value used by the RANSAC procedure. The parameter value
is the maximum allowed distance between the observed and computed point projections to consider it
an inlier.
@param confidence The probability that the algorithm produces a useful result.
@param inliers Output vector that contains indices of inliers in objectPoints and imagePoints .
@param flags Method for solving a PnP problem (see solvePnP ).

The function estimates an object pose given a set of object points, their corresponding image
projections, as well as the camera matrix and the distortion coefficients. This function finds such
a pose that minimizes reprojection error, that is, the sum of squared distances between the observed
projections imagePoints and the projected (using projectPoints ) objectPoints. The use of RANSAC
makes the function resistant to outliers.

@note
   -   An example of how to use solvePNPRansac for object detection can be found at
        opencv_source_code/samples/cpp/tutorial_code/calib3d/real_time_pose_estimation/
 */
CV_EXPORTS_W bool solvePnPRansac(
  InputArray objectPoints,
  InputArray imagePoints,
  InputArray cameraMatrix,
  InputArray distCoeffs,
  OutputArray rvec,
  OutputArray tvec,
  bool useExtrinsicGuess = false,
  int iterationsCount = 100,
  float reprojectionError = 8.0,
  double confidence = 0.99,
  OutputArray inliers = noArray(),
  int flags = SOLVEPNP_ITERATIVE);

}  // namespace c8cv
