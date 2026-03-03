// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "third_party/cvlite/core/core.hpp"
#include "third_party/cvlite/core/core_c.h"

namespace c8cv {

//! cv::undistort mode
enum UndistortTypes { PROJ_SPHERICAL_ORTHO = 0, PROJ_SPHERICAL_EQRECT = 1 };

/** @brief Transforms the input image to compensate lens distortion
@see undistort
*/
CVAPI(void)
cvUndistort2(
  const CvArr *src,
  CvArr *dst,
  const CvMat *camera_matrix,
  const CvMat *distortion_coeffs,
  const CvMat *new_camera_matrix CV_DEFAULT(0));

/** @brief Computes transformation map from intrinsic camera parameters
   that can used by cvRemap
*/
CVAPI(void)
cvInitUndistortMap(
  const CvMat *camera_matrix, const CvMat *distortion_coeffs, CvArr *mapx, CvArr *mapy);

/** @brief Computes undistortion+rectification map for a head of stereo camera
@see initUndistortRectifyMap
*/
CVAPI(void)
cvInitUndistortRectifyMap(
  const CvMat *camera_matrix,
  const CvMat *dist_coeffs,
  const CvMat *R,
  const CvMat *new_camera_matrix,
  CvArr *mapx,
  CvArr *mapy);

/** @brief Computes the original (undistorted) feature coordinates
   from the observed (distorted) coordinates
@see undistortPoints
*/
CVAPI(void)
cvUndistortPoints(
  const CvMat *src,
  CvMat *dst,
  const CvMat *camera_matrix,
  const CvMat *dist_coeffs,
  const CvMat *R CV_DEFAULT(0),
  const CvMat *P CV_DEFAULT(0));

/** @brief Returns the default new camera matrix.

The function returns the camera matrix that is either an exact copy of the input cameraMatrix (when
centerPrinicipalPoint=false ), or the modified one (when centerPrincipalPoint=true).

In the latter case, the new camera matrix will be:

\f[\begin{bmatrix} f_x && 0 && ( \texttt{imgSize.width} -1)*0.5  \\ 0 && f_y && (
\texttt{imgSize.height} -1)*0.5  \\ 0 && 0 && 1 \end{bmatrix} ,\f]

where \f$f_x\f$ and \f$f_y\f$ are \f$(0,0)\f$ and \f$(1,1)\f$ elements of cameraMatrix,
respectively.

By default, the undistortion functions in OpenCV (see initUndistortRectifyMap, undistort) do not
move the principal point. However, when you work with stereo, it is important to move the principal
points in both views to the same y-coordinate (which is required by most of stereo correspondence
algorithms), and may be to the same x-coordinate too. So, you can form the new camera matrix for
each view where the principal points are located at the center.

@param cameraMatrix Input camera matrix.
@param imgsize Camera view image size in pixels.
@param centerPrincipalPoint Location of the principal point in the new camera matrix. The
parameter indicates whether this location should be at the image center or not.
 */
CV_EXPORTS_W Mat getDefaultNewCameraMatrix(
  InputArray cameraMatrix, Size imgsize = Size(), bool centerPrincipalPoint = false);

/** @brief Computes the undistortion and rectification transformation map.

The function computes the joint undistortion and rectification transformation and represents the
result in the form of maps for remap. The undistorted image looks like original, as if it is
captured with a camera using the camera matrix =newCameraMatrix and zero distortion. In case of a
monocular camera, newCameraMatrix is usually equal to cameraMatrix, or it can be computed by
getOptimalNewCameraMatrix for a better control over scaling. In case of a stereo camera,
newCameraMatrix is normally set to P1 or P2 computed by stereoRectify .

Also, this new camera is oriented differently in the coordinate space, according to R. That, for
example, helps to align two heads of a stereo camera so that the epipolar lines on both images
become horizontal and have the same y- coordinate (in case of a horizontally aligned stereo camera).

The function actually builds the maps for the inverse mapping algorithm that is used by remap. That
is, for each pixel \f$(u, v)\f$ in the destination (corrected and rectified) image, the function
computes the corresponding coordinates in the source image (that is, in the original image from
camera). The following process is applied:
\f[
\begin{array}{l}
x  \leftarrow (u - {c'}_x)/{f'}_x  \\
y  \leftarrow (v - {c'}_y)/{f'}_y  \\
{[X\,Y\,W]} ^T  \leftarrow R^{-1}*[x \, y \, 1]^T  \\
x'  \leftarrow X/W  \\
y'  \leftarrow Y/W  \\
r^2  \leftarrow x'^2 + y'^2 \\
x''  \leftarrow x' \frac{1 + k_1 r^2 + k_2 r^4 + k_3 r^6}{1 + k_4 r^2 + k_5 r^4 + k_6 r^6}
+ 2p_1 x' y' + p_2(r^2 + 2 x'^2)  + s_1 r^2 + s_2 r^4\\
y''  \leftarrow y' \frac{1 + k_1 r^2 + k_2 r^4 + k_3 r^6}{1 + k_4 r^2 + k_5 r^4 + k_6 r^6}
+ p_1 (r^2 + 2 y'^2) + 2 p_2 x' y' + s_3 r^2 + s_4 r^4 \\
s\vecthree{x'''}{y'''}{1} =
\vecthreethree{R_{33}(\tau_x, \tau_y)}{0}{-R_{13}((\tau_x, \tau_y)}
{0}{R_{33}(\tau_x, \tau_y)}{-R_{23}(\tau_x, \tau_y)}
{0}{0}{1} R(\tau_x, \tau_y) \vecthree{x''}{y''}{1}\\
map_x(u,v)  \leftarrow x''' f_x + c_x  \\
map_y(u,v)  \leftarrow y''' f_y + c_y
\end{array}
\f]
where \f$(k_1, k_2, p_1, p_2[, k_3[, k_4, k_5, k_6[, s_1, s_2, s_3, s_4[, \tau_x, \tau_y]]]])\f$
are the distortion coefficients.

In case of a stereo camera, this function is called twice: once for each camera head, after
stereoRectify, which in its turn is called after stereoCalibrate. But if the stereo camera
was not calibrated, it is still possible to compute the rectification transformations directly from
the fundamental matrix using stereoRectifyUncalibrated. For each camera, the function computes
homography H as the rectification transformation in a pixel domain, not a rotation matrix R in 3D
space. R can be computed from H as
\f[\texttt{R} = \texttt{cameraMatrix} ^{-1} \cdot \texttt{H} \cdot \texttt{cameraMatrix}\f]
where cameraMatrix can be chosen arbitrarily.

@param cameraMatrix Input camera matrix \f$A=\vecthreethree{f_x}{0}{c_x}{0}{f_y}{c_y}{0}{0}{1}\f$ .
@param distCoeffs Input vector of distortion coefficients
\f$(k_1, k_2, p_1, p_2[, k_3[, k_4, k_5, k_6[, s_1, s_2, s_3, s_4[, \tau_x, \tau_y]]]])\f$
of 4, 5, 8, 12 or 14 elements. If the vector is NULL/empty, the zero distortion coefficients are
assumed.
@param R Optional rectification transformation in the object space (3x3 matrix). R1 or R2 ,
computed by stereoRectify can be passed here. If the matrix is empty, the identity transformation
is assumed. In cvInitUndistortMap R assumed to be an identity matrix.
@param newCameraMatrix New camera matrix
\f$A'=\vecthreethree{f_x'}{0}{c_x'}{0}{f_y'}{c_y'}{0}{0}{1}\f$.
@param size Undistorted image size.
@param m1type Type of the first output map that can be CV_32FC1 or CV_16SC2, see convertMaps
@param map1 The first output map.
@param map2 The second output map.
 */
CV_EXPORTS_W void initUndistortRectifyMap(
  InputArray cameraMatrix,
  InputArray distCoeffs,
  InputArray R,
  InputArray newCameraMatrix,
  Size size,
  int m1type,
  OutputArray map1,
  OutputArray map2);

//! initializes maps for remap() for wide-angle
CV_EXPORTS_W float initWideAngleProjMap(
  InputArray cameraMatrix,
  InputArray distCoeffs,
  Size imageSize,
  int destImageWidth,
  int m1type,
  OutputArray map1,
  OutputArray map2,
  int projType = PROJ_SPHERICAL_EQRECT,
  double alpha = 0);

/** @brief Transforms an image to compensate for lens distortion.

The function transforms an image to compensate radial and tangential lens distortion.

The function is simply a combination of initUndistortRectifyMap (with unity R ) and remap
(with bilinear interpolation). See the former function for details of the transformation being
performed.

Those pixels in the destination image, for which there is no correspondent pixels in the source
image, are filled with zeros (black color).

A particular subset of the source image that will be visible in the corrected image can be regulated
by newCameraMatrix. You can use getOptimalNewCameraMatrix to compute the appropriate
newCameraMatrix depending on your requirements.

The camera matrix and the distortion parameters can be determined using calibrateCamera. If
the resolution of images is different from the resolution used at the calibration stage, \f$f_x,
f_y, c_x\f$ and \f$c_y\f$ need to be scaled accordingly, while the distortion coefficients remain
the same.

@param src Input (distorted) image.
@param dst Output (corrected) image that has the same size and type as src .
@param cameraMatrix Input camera matrix \f$A = \vecthreethree{f_x}{0}{c_x}{0}{f_y}{c_y}{0}{0}{1}\f$
.
@param distCoeffs Input vector of distortion coefficients
\f$(k_1, k_2, p_1, p_2[, k_3[, k_4, k_5, k_6[, s_1, s_2, s_3, s_4[, \tau_x, \tau_y]]]])\f$
of 4, 5, 8, 12 or 14 elements. If the vector is NULL/empty, the zero distortion coefficients are
assumed.
@param newCameraMatrix Camera matrix of the distorted image. By default, it is the same as
cameraMatrix but you may additionally scale and shift the result by using a different matrix.
 */
CV_EXPORTS_W void undistort(
  InputArray src,
  OutputArray dst,
  InputArray cameraMatrix,
  InputArray distCoeffs,
  InputArray newCameraMatrix = noArray());

/** @brief Computes the ideal point coordinates from the observed point coordinates.

The function is similar to undistort and initUndistortRectifyMap but it operates on a
sparse set of points instead of a raster image. Also the function performs a reverse transformation
to projectPoints. In case of a 3D object, it does not reconstruct its 3D coordinates, but for a
planar object, it does, up to a translation vector, if the proper R is specified.

For each observed point coordinate \f$(u, v)\f$ the function computes:
\f[
\begin{array}{l}
x^{"}  \leftarrow (u - c_x)/f_x  \\
y^{"}  \leftarrow (v - c_y)/f_y  \\
(x',y') = undistort(x^{"},y^{"}, \texttt{distCoeffs}) \\
{[X\,Y\,W]} ^T  \leftarrow R*[x' \, y' \, 1]^T  \\
x  \leftarrow X/W  \\
y  \leftarrow Y/W  \\
\text{only performed if P is specified:} \\
u'  \leftarrow x {f'}_x + {c'}_x  \\
v'  \leftarrow y {f'}_y + {c'}_y
\end{array}
\f]

where *undistort* is an approximate iterative algorithm that estimates the normalized original
point coordinates out of the normalized distorted point coordinates ("normalized" means that the
coordinates do not depend on the camera matrix).

The function can be used for both a stereo camera head or a monocular camera (when R is empty).

@param src Observed point coordinates, 1xN or Nx1 2-channel (CV_32FC2 or CV_64FC2).
@param dst Output ideal point coordinates after undistortion and reverse perspective
transformation. If matrix P is identity or omitted, dst will contain normalized point coordinates.
@param cameraMatrix Camera matrix \f$\vecthreethree{f_x}{0}{c_x}{0}{f_y}{c_y}{0}{0}{1}\f$ .
@param distCoeffs Input vector of distortion coefficients
\f$(k_1, k_2, p_1, p_2[, k_3[, k_4, k_5, k_6[, s_1, s_2, s_3, s_4[, \tau_x, \tau_y]]]])\f$
of 4, 5, 8, 12 or 14 elements. If the vector is NULL/empty, the zero distortion coefficients are
assumed.
@param R Rectification transformation in the object space (3x3 matrix). R1 or R2 computed by
stereoRectify can be passed here. If the matrix is empty, the identity transformation is used.
@param P New camera matrix (3x3) or new projection matrix (3x4) \f$\begin{bmatrix} {f'}_x & 0 &
{c'}_x & t_x \\ 0 & {f'}_y & {c'}_y & t_y \\ 0 & 0 & 1 & t_z \end{bmatrix}\f$. P1 or P2 computed by
stereoRectify can be passed here. If the matrix is empty, the identity new camera matrix is used.
 */
CV_EXPORTS_W void undistortPoints(
  InputArray src,
  OutputArray dst,
  InputArray cameraMatrix,
  InputArray distCoeffs,
  InputArray R = noArray(),
  InputArray P = noArray());

}  // namespace c8cv
