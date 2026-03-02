// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "third_party/cvlite/core/core.hpp"

namespace c8cv {

//! the algorithm for finding fundamental matrix
enum {
  FM_7POINT = 1,  //!< 7-point algorithm
  FM_8POINT = 2,  //!< 8-point algorithm
  FM_LMEDS = 4,   //!< least-median algorithm
  FM_RANSAC = 8   //!< RANSAC algorithm
};

/** @brief Finds a perspective transformation between two planes.

@param srcPoints Coordinates of the points in the original plane, a matrix of the type CV_32FC2
or vector\<Point2f\> .
@param dstPoints Coordinates of the points in the target plane, a matrix of the type CV_32FC2 or
a vector\<Point2f\> .
@param method Method used to computed a homography matrix. The following methods are possible:
-   **0** - a regular method using all the points
-   **RANSAC** - RANSAC-based robust method
-   **LMEDS** - Least-Median robust method
-   **RHO**    - PROSAC-based robust method
@param ransacReprojThreshold Maximum allowed reprojection error to treat a point pair as an inlier
(used in the RANSAC and RHO methods only). That is, if
\f[\| \texttt{dstPoints} _i -  \texttt{convertPointsHomogeneous} ( \texttt{H} * \texttt{srcPoints}
_i) \|  >  \texttt{ransacReprojThreshold}\f] then the point \f$i\f$ is considered an outlier. If
srcPoints and dstPoints are measured in pixels, it usually makes sense to set this parameter
somewhere in the range of 1 to 10.
@param mask Optional output mask set by a robust method ( RANSAC or LMEDS ). Note that the input
mask values are ignored.
@param maxIters The maximum number of RANSAC iterations, 2000 is the maximum it can be.
@param confidence Confidence level, between 0 and 1.

The function finds and returns the perspective transformation \f$H\f$ between the source and the
destination planes:

\f[s_i  \vecthree{x'_i}{y'_i}{1} \sim H  \vecthree{x_i}{y_i}{1}\f]

so that the back-projection error

\f[\sum _i \left ( x'_i- \frac{h_{11} x_i + h_{12} y_i + h_{13}}{h_{31} x_i + h_{32} y_i + h_{33}}
\right )^2+ \left ( y'_i- \frac{h_{21} x_i + h_{22} y_i + h_{23}}{h_{31} x_i + h_{32} y_i + h_{33}}
\right )^2\f]

is minimized. If the parameter method is set to the default value 0, the function uses all the point
pairs to compute an initial homography estimate with a simple least-squares scheme.

However, if not all of the point pairs ( \f$srcPoints_i\f$, \f$dstPoints_i\f$ ) fit the rigid
perspective transformation (that is, there are some outliers), this initial estimate will be poor.
In this case, you can use one of the three robust methods. The methods RANSAC, LMeDS and RHO try
many different random subsets of the corresponding point pairs (of four pairs each), estimate the
homography matrix using this subset and a simple least-square algorithm, and then compute the
quality/goodness of the computed homography (which is the number of inliers for RANSAC or the median
re-projection error for LMeDs). The best subset is then used to produce the initial estimate of the
homography matrix and the mask of inliers/outliers.

Regardless of the method, robust or not, the computed homography matrix is refined further (using
inliers only in case of a robust method) with the Levenberg-Marquardt method to reduce the
re-projection error even more.

The methods RANSAC and RHO can handle practically any ratio of outliers but need a threshold to
distinguish inliers from outliers. The method LMeDS does not need any threshold but it works
correctly only when there are more than 50% of inliers. Finally, if there are no outliers and the
noise is rather small, use the default method (method=0).

The function is used to find initial intrinsic and extrinsic matrices. Homography matrix is
determined up to a scale. Thus, it is normalized so that \f$h_{33}=1\f$. Note that whenever an H
matrix cannot be estimated, an empty one will be returned.

@sa
getAffineTransform, estimateAffine2D, estimateAffinePartial2D, getPerspectiveTransform,
warpPerspective, perspectiveTransform


@note
   -   A example on calculating a homography for image matching can be found at
        opencv_source_code/samples/cpp/video_homography.cpp

 */
CV_EXPORTS_W Mat findHomography(
  InputArray srcPoints,
  InputArray dstPoints,
  int method = 0,
  double ransacReprojThreshold = 3,
  OutputArray mask = noArray(),
  const int maxIters = 2000,
  const double confidence = 0.995);

/** @overload */
CV_EXPORTS Mat findHomography(
  InputArray srcPoints,
  InputArray dstPoints,
  OutputArray mask,
  int method = 0,
  double ransacReprojThreshold = 3);

/** @brief Calculates a fundamental matrix from the corresponding points in two images.

@param points1 Array of N points from the first image. The point coordinates should be
floating-point (single or double precision).
@param points2 Array of the second image points of the same size and format as points1 .
@param method Method for computing a fundamental matrix.
-   **CV_FM_7POINT** for a 7-point algorithm. \f$N = 7\f$
-   **CV_FM_8POINT** for an 8-point algorithm. \f$N \ge 8\f$
-   **CV_FM_RANSAC** for the RANSAC algorithm. \f$N \ge 8\f$
-   **CV_FM_LMEDS** for the LMedS algorithm. \f$N \ge 8\f$
@param param1 Parameter used for RANSAC. It is the maximum distance from a point to an epipolar
line in pixels, beyond which the point is considered an outlier and is not used for computing the
final fundamental matrix. It can be set to something like 1-3, depending on the accuracy of the
point localization, image resolution, and the image noise.
@param param2 Parameter used for the RANSAC or LMedS methods only. It specifies a desirable level
of confidence (probability) that the estimated matrix is correct.
@param mask

The epipolar geometry is described by the following equation:

\f[[p_2; 1]^T F [p_1; 1] = 0\f]

where \f$F\f$ is a fundamental matrix, \f$p_1\f$ and \f$p_2\f$ are corresponding points in the first
and the second images, respectively.

The function calculates the fundamental matrix using one of four methods listed above and returns
the found fundamental matrix. Normally just one matrix is found. But in case of the 7-point
algorithm, the function may return up to 3 solutions ( \f$9 \times 3\f$ matrix that stores all 3
matrices sequentially).

The calculated fundamental matrix may be passed further to computeCorrespondEpilines that finds the
epipolar lines corresponding to the specified points. It can also be passed to
stereoRectifyUncalibrated to compute the rectification transformation. :
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

    Mat fundamental_matrix =
     findFundamentalMat(points1, points2, FM_RANSAC, 3, 0.99);
@endcode
 */
CV_EXPORTS_W Mat findFundamentalMat(
  InputArray points1,
  InputArray points2,
  int method = FM_RANSAC,
  double param1 = 3.,
  double param2 = 0.99,
  OutputArray mask = noArray());

/** @overload */
CV_EXPORTS Mat findFundamentalMat(
  InputArray points1,
  InputArray points2,
  OutputArray mask,
  int method = FM_RANSAC,
  double param1 = 3.,
  double param2 = 0.99);

/** @brief For points in an image of a stereo pair, computes the corresponding epilines in the other
image.

@param points Input points. \f$N \times 1\f$ or \f$1 \times N\f$ matrix of type CV_32FC2 or
vector\<Point2f\> .
@param whichImage Index of the image (1 or 2) that contains the points .
@param F Fundamental matrix that can be estimated using findFundamentalMat or stereoRectify .
@param lines Output vector of the epipolar lines corresponding to the points in the other image.
Each line \f$ax + by + c=0\f$ is encoded by 3 numbers \f$(a, b, c)\f$ .

For every point in one of the two images of a stereo pair, the function finds the equation of the
corresponding epipolar line in the other image.

From the fundamental matrix definition (see findFundamentalMat ), line \f$l^{(2)}_i\f$ in the second
image for the point \f$p^{(1)}_i\f$ in the first image (when whichImage=1 ) is computed as:

\f[l^{(2)}_i = F p^{(1)}_i\f]

And vice versa, when whichImage=2, \f$l^{(1)}_i\f$ is computed from \f$p^{(2)}_i\f$ as:

\f[l^{(1)}_i = F^T p^{(2)}_i\f]

Line coefficients are defined up to a scale. They are normalized so that \f$a_i^2+b_i^2=1\f$ .
 */
CV_EXPORTS_W void computeCorrespondEpilines(
  InputArray points, int whichImage, InputArray F, OutputArray lines);

/** @brief Converts points from Euclidean to homogeneous space.

@param src Input vector of N-dimensional points.
@param dst Output vector of N+1-dimensional points.

The function converts points from Euclidean to homogeneous space by appending 1's to the tuple of
point coordinates. That is, each point (x1, x2, ..., xn) is converted to (x1, x2, ..., xn, 1).
 */
CV_EXPORTS_W void convertPointsToHomogeneous(InputArray src, OutputArray dst);

/** @brief Converts points from homogeneous to Euclidean space.

@param src Input vector of N-dimensional points.
@param dst Output vector of N-1-dimensional points.

The function converts points homogeneous to Euclidean space using perspective projection. That is,
each point (x1, x2, ... x(n-1), xn) is converted to (x1/xn, x2/xn, ..., x(n-1)/xn). When xn=0, the
output point coordinates will be (0,0,0,...).
 */
CV_EXPORTS_W void convertPointsFromHomogeneous(InputArray src, OutputArray dst);

/** @brief Converts points to/from homogeneous coordinates.

@param src Input array or vector of 2D, 3D, or 4D points.
@param dst Output vector of 2D, 3D, or 4D points.

The function converts 2D or 3D points from/to homogeneous coordinates by calling either
convertPointsToHomogeneous or convertPointsFromHomogeneous.

@note The function is obsolete. Use one of the previous two functions instead.
 */
CV_EXPORTS void convertPointsHomogeneous(InputArray src, OutputArray dst);

/** @brief Calculates the Sampson Distance between two points.

The function sampsonDistance calculates and returns the first order approximation of the geometric
error as:
\f[sd( \texttt{pt1} , \texttt{pt2} )= \frac{(\texttt{pt2}^t \cdot \texttt{F} \cdot
\texttt{pt1})^2}{(\texttt{F} \cdot \texttt{pt1})(0) + (\texttt{F} \cdot \texttt{pt1})(1) +
(\texttt{F}^t \cdot \texttt{pt2})(0) + (\texttt{F}^t \cdot \texttt{pt2})(1)}\f] The fundamental
matrix may be calculated using the findFundamentalMat function. See HZ 11.4.3 for details.
@param pt1 first homogeneous 2d point
@param pt2 second homogeneous 2d point
@param F fundamental matrix
*/
CV_EXPORTS_W double sampsonDistance(InputArray pt1, InputArray pt2, InputArray F);

}  // namespace c8cv
