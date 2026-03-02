// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "third_party/cvlite/core/core.hpp"
#include "third_party/cvlite/core/core_c.h"

namespace c8cv {

#define CV_CALIB_USE_INTRINSIC_GUESS 1
#define CV_CALIB_FIX_ASPECT_RATIO 2
#define CV_CALIB_FIX_PRINCIPAL_POINT 4
#define CV_CALIB_ZERO_TANGENT_DIST 8
#define CV_CALIB_FIX_FOCAL_LENGTH 16
#define CV_CALIB_FIX_K1 32
#define CV_CALIB_FIX_K2 64
#define CV_CALIB_FIX_K3 128
#define CV_CALIB_FIX_K4 2048
#define CV_CALIB_FIX_K5 4096
#define CV_CALIB_FIX_K6 8192
#define CV_CALIB_RATIONAL_MODEL 16384
#define CV_CALIB_THIN_PRISM_MODEL 32768
#define CV_CALIB_FIX_S1_S2_S3_S4 65536
#define CV_CALIB_TILTED_MODEL 262144
#define CV_CALIB_FIX_TAUX_TAUY 524288

#define CV_CALIB_NINTRINSIC 18

#define CV_CALIB_FIX_INTRINSIC 256
#define CV_CALIB_SAME_FOCAL_LENGTH 512

#define CV_CALIB_ZERO_DISPARITY 1024

enum {
  CALIB_USE_INTRINSIC_GUESS = 0x00001,
  CALIB_FIX_ASPECT_RATIO = 0x00002,
  CALIB_FIX_PRINCIPAL_POINT = 0x00004,
  CALIB_ZERO_TANGENT_DIST = 0x00008,
  CALIB_FIX_FOCAL_LENGTH = 0x00010,
  CALIB_FIX_K1 = 0x00020,
  CALIB_FIX_K2 = 0x00040,
  CALIB_FIX_K3 = 0x00080,
  CALIB_FIX_K4 = 0x00800,
  CALIB_FIX_K5 = 0x01000,
  CALIB_FIX_K6 = 0x02000,
  CALIB_RATIONAL_MODEL = 0x04000,
  CALIB_THIN_PRISM_MODEL = 0x08000,
  CALIB_FIX_S1_S2_S3_S4 = 0x10000,
  CALIB_TILTED_MODEL = 0x40000,
  CALIB_FIX_TAUX_TAUY = 0x80000,
  CALIB_USE_QR = 0x100000,  //!< use QR instead of SVD decomposition for solving. Faster but
                            //!< potentially less precise
  // only for stereo
  CALIB_FIX_INTRINSIC = 0x00100,
  CALIB_SAME_FOCAL_LENGTH = 0x00200,
  // for stereo rectification
  CALIB_ZERO_DISPARITY = 0x00400,
  CALIB_USE_LU = (1 << 17),  //!< use LU instead of SVD decomposition for solving. much faster but
                             //!< potentially less precise
};

/* Computes d(AB)/dA and d(AB)/dB */
CVAPI(void) cvCalcMatMulDeriv(const CvMat *A, const CvMat *B, CvMat *dABdA, CvMat *dABdB);

/* Computes r3 = rodrigues(rodrigues(r2)*rodrigues(r1)),
   t3 = rodrigues(r2)*t1 + t2 and the respective derivatives */
CVAPI(void)
cvComposeRT(
  const CvMat *_rvec1,
  const CvMat *_tvec1,
  const CvMat *_rvec2,
  const CvMat *_tvec2,
  CvMat *_rvec3,
  CvMat *_tvec3,
  CvMat *dr3dr1 CV_DEFAULT(0),
  CvMat *dr3dt1 CV_DEFAULT(0),
  CvMat *dr3dr2 CV_DEFAULT(0),
  CvMat *dr3dt2 CV_DEFAULT(0),
  CvMat *dt3dr1 CV_DEFAULT(0),
  CvMat *dt3dt1 CV_DEFAULT(0),
  CvMat *dt3dr2 CV_DEFAULT(0),
  CvMat *dt3dt2 CV_DEFAULT(0));

/* Converts rotation vector to rotation matrix or vice versa */
CVAPI(int) cvRodrigues2(const CvMat *src, CvMat *dst, CvMat *jacobian CV_DEFAULT(0));

/* Projects object points to the view plane using
   the specified extrinsic and intrinsic camera parameters */
CVAPI(void)
cvProjectPoints2(
  const CvMat *object_points,
  const CvMat *rotation_vector,
  const CvMat *translation_vector,
  const CvMat *camera_matrix,
  const CvMat *distortion_coeffs,
  CvMat *image_points,
  CvMat *dpdrot CV_DEFAULT(NULL),
  CvMat *dpdt CV_DEFAULT(NULL),
  CvMat *dpdf CV_DEFAULT(NULL),
  CvMat *dpdc CV_DEFAULT(NULL),
  CvMat *dpddist CV_DEFAULT(NULL),
  double aspect_ratio CV_DEFAULT(0));

/* Finds extrinsic camera parameters from
   a few known corresponding point pairs and intrinsic parameters */
CVAPI(void)
cvFindExtrinsicCameraParams2(
  const CvMat *object_points,
  const CvMat *image_points,
  const CvMat *camera_matrix,
  const CvMat *distortion_coeffs,
  CvMat *rotation_vector,
  CvMat *translation_vector,
  int use_extrinsic_guess CV_DEFAULT(0));

/* Computes initial estimate of the intrinsic camera parameters
   in case of planar calibration target (e.g. chessboard) */
CVAPI(void)
cvInitIntrinsicParams2D(
  const CvMat *object_points,
  const CvMat *image_points,
  const CvMat *npoints,
  CvSize image_size,
  CvMat *camera_matrix,
  double aspect_ratio CV_DEFAULT(1.));

/* Finds intrinsic and extrinsic camera parameters
   from a few views of known calibration pattern */
CVAPI(double)
cvCalibrateCamera2(
  const CvMat *object_points,
  const CvMat *image_points,
  const CvMat *point_counts,
  CvSize image_size,
  CvMat *camera_matrix,
  CvMat *distortion_coeffs,
  CvMat *rotation_vectors CV_DEFAULT(NULL),
  CvMat *translation_vectors CV_DEFAULT(NULL),
  int flags CV_DEFAULT(0),
  CvTermCriteria term_crit
    CV_DEFAULT(cvTermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 30, DBL_EPSILON)));

/* Computes various useful characteristics of the camera from the data computed by
   cvCalibrateCamera2 */
CVAPI(void)
cvCalibrationMatrixValues(
  const CvMat *camera_matrix,
  CvSize image_size,
  double aperture_width CV_DEFAULT(0),
  double aperture_height CV_DEFAULT(0),
  double *fovx CV_DEFAULT(NULL),
  double *fovy CV_DEFAULT(NULL),
  double *focal_length CV_DEFAULT(NULL),
  CvPoint2D64f *principal_point CV_DEFAULT(NULL),
  double *pixel_aspect_ratio CV_DEFAULT(NULL));

/* Computes the transformation from one camera coordinate system to another one
   from a few correspondent views of the same calibration target. Optionally, calibrates
   both cameras */
CVAPI(double)
cvStereoCalibrate(
  const CvMat *object_points,
  const CvMat *image_points1,
  const CvMat *image_points2,
  const CvMat *npoints,
  CvMat *camera_matrix1,
  CvMat *dist_coeffs1,
  CvMat *camera_matrix2,
  CvMat *dist_coeffs2,
  CvSize image_size,
  CvMat *R,
  CvMat *T,
  CvMat *E CV_DEFAULT(0),
  CvMat *F CV_DEFAULT(0),
  int flags CV_DEFAULT(CV_CALIB_FIX_INTRINSIC),
  CvTermCriteria term_crit
    CV_DEFAULT(cvTermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 30, 1e-6)));

/* Computes 3D rotations (+ optional shift) for each camera coordinate system to make both
   views parallel (=> to make all the epipolar lines horizontal or vertical) */
CVAPI(void)
cvStereoRectify(
  const CvMat *camera_matrix1,
  const CvMat *camera_matrix2,
  const CvMat *dist_coeffs1,
  const CvMat *dist_coeffs2,
  CvSize image_size,
  const CvMat *R,
  const CvMat *T,
  CvMat *R1,
  CvMat *R2,
  CvMat *P1,
  CvMat *P2,
  CvMat *Q CV_DEFAULT(0),
  int flags CV_DEFAULT(CV_CALIB_ZERO_DISPARITY),
  double alpha CV_DEFAULT(-1),
  CvSize new_image_size CV_DEFAULT(cvSize(0, 0)),
  CvRect *valid_pix_ROI1 CV_DEFAULT(0),
  CvRect *valid_pix_ROI2 CV_DEFAULT(0));

/* Computes the optimal new camera matrix according to the free scaling parameter alpha:
   alpha=0 - only valid pixels will be retained in the undistorted image
   alpha=1 - all the source image pixels will be retained in the undistorted image
*/
CVAPI(void)
cvGetOptimalNewCameraMatrix(
  const CvMat *camera_matrix,
  const CvMat *dist_coeffs,
  CvSize image_size,
  double alpha,
  CvMat *new_camera_matrix,
  CvSize new_imag_size CV_DEFAULT(cvSize(0, 0)),
  CvRect *valid_pixel_ROI CV_DEFAULT(0),
  int center_principal_point CV_DEFAULT(0));

/* Computes rectification transformations for uncalibrated pair of images using a set
   of point correspondences */
CVAPI(int)
cvStereoRectifyUncalibrated(
  const CvMat *points1,
  const CvMat *points2,
  const CvMat *F,
  CvSize img_size,
  CvMat *H1,
  CvMat *H2,
  double threshold CV_DEFAULT(5));

/* Reprojects the computed disparity image to the 3D space using the specified 4x4 matrix */
CVAPI(void)
cvReprojectImageTo3D(
  const CvArr *disparityImage,
  CvArr *_3dImage,
  const CvMat *Q,
  int handleMissingValues CV_DEFAULT(0));

/* Computes RQ decomposition for 3x3 matrices */
CVAPI(void)
cvRQDecomp3x3(
  const CvMat *matrixM,
  CvMat *matrixR,
  CvMat *matrixQ,
  CvMat *matrixQx CV_DEFAULT(NULL),
  CvMat *matrixQy CV_DEFAULT(NULL),
  CvMat *matrixQz CV_DEFAULT(NULL),
  CvPoint3D64f *eulerAngles CV_DEFAULT(NULL));

/* Computes projection matrix decomposition */
CVAPI(void)
cvDecomposeProjectionMatrix(
  const CvMat *projMatr,
  CvMat *calibMatr,
  CvMat *rotMatr,
  CvMat *posVect,
  CvMat *rotMatrX CV_DEFAULT(NULL),
  CvMat *rotMatrY CV_DEFAULT(NULL),
  CvMat *rotMatrZ CV_DEFAULT(NULL),
  CvPoint3D64f *eulerAngles CV_DEFAULT(NULL));

/** @brief Reprojects a disparity image to 3D space.

@param disparity Input single-channel 8-bit unsigned, 16-bit signed, 32-bit signed or 32-bit
floating-point disparity image. If 16-bit signed format is used, the values are assumed to have no
fractional bits.
@param _3dImage Output 3-channel floating-point image of the same size as disparity . Each
element of _3dImage(x,y) contains 3D coordinates of the point (x,y) computed from the disparity
map.
@param Q \f$4 \times 4\f$ perspective transformation matrix that can be obtained with stereoRectify.
@param handleMissingValues Indicates, whether the function should handle missing values (i.e.
points where the disparity was not computed). If handleMissingValues=true, then pixels with the
minimal disparity that corresponds to the outliers (see StereoMatcher::compute ) are transformed
to 3D points with a very large Z value (currently set to 10000).
@param ddepth The optional output array depth. If it is -1, the output image will have CV_32F
depth. ddepth can also be set to CV_16S, CV_32S or CV_32F.

The function transforms a single-channel disparity map to a 3-channel image representing a 3D
surface. That is, for each pixel (x,y) andthe corresponding disparity d=disparity(x,y) , it
computes:

\f[\begin{array}{l} [X \; Y \; Z \; W]^T =  \texttt{Q} *[x \; y \; \texttt{disparity} (x,y) \; 1]^T
\\ \texttt{\_3dImage} (x,y) = (X/W, \; Y/W, \; Z/W) \end{array}\f]

The matrix Q can be an arbitrary \f$4 \times 4\f$ matrix (for example, the one computed by
stereoRectify). To reproject a sparse set of points {(x,y,d),...} to 3D space, use
perspectiveTransform .
 */
CV_EXPORTS_W void reprojectImageTo3D(
  InputArray disparity,
  OutputArray _3dImage,
  InputArray Q,
  bool handleMissingValues = false,
  int ddepth = -1);

/** @brief Converts a rotation matrix to a rotation vector or vice versa.

@param src Input rotation vector (3x1 or 1x3) or rotation matrix (3x3).
@param dst Output rotation matrix (3x3) or rotation vector (3x1 or 1x3), respectively.
@param jacobian Optional output Jacobian matrix, 3x9 or 9x3, which is a matrix of partial
derivatives of the output array components with respect to the input array components.

\f[\begin{array}{l} \theta \leftarrow norm(r) \\ r  \leftarrow r/ \theta \\ R =  \cos{\theta} I +
(1- \cos{\theta} ) r r^T +  \sin{\theta} \vecthreethree{0}{-r_z}{r_y}{r_z}{0}{-r_x}{-r_y}{r_x}{0}
\end{array}\f]

Inverse transformation can be also done easily, since

\f[\sin ( \theta ) \vecthreethree{0}{-r_z}{r_y}{r_z}{0}{-r_x}{-r_y}{r_x}{0} = \frac{R - R^T}{2}\f]

A rotation vector is a convenient and most compact representation of a rotation matrix (since any
rotation matrix has just 3 degrees of freedom). The representation is used in the global 3D geometry
optimization procedures like calibrateCamera, stereoCalibrate, or solvePnP .
 */
CV_EXPORTS_W void Rodrigues(InputArray src, OutputArray dst, OutputArray jacobian = noArray());

/** @brief Computes partial derivatives of the matrix product for each multiplied matrix.

@param A First multiplied matrix.
@param B Second multiplied matrix.
@param dABdA First output derivative matrix d(A\*B)/dA of size
\f$\texttt{A.rows*B.cols} \times {A.rows*A.cols}\f$ .
@param dABdB Second output derivative matrix d(A\*B)/dB of size
\f$\texttt{A.rows*B.cols} \times {B.rows*B.cols}\f$ .

The function computes partial derivatives of the elements of the matrix product \f$A*B\f$ with
regard to the elements of each of the two input matrices. The function is used to compute the
Jacobian matrices in stereoCalibrate but can also be used in any other similar optimization
function.
 */
CV_EXPORTS_W void matMulDeriv(InputArray A, InputArray B, OutputArray dABdA, OutputArray dABdB);

/** @brief Combines two rotation-and-shift transformations.

@param rvec1 First rotation vector.
@param tvec1 First translation vector.
@param rvec2 Second rotation vector.
@param tvec2 Second translation vector.
@param rvec3 Output rotation vector of the superposition.
@param tvec3 Output translation vector of the superposition.
@param dr3dr1
@param dr3dt1
@param dr3dr2
@param dr3dt2
@param dt3dr1
@param dt3dt1
@param dt3dr2
@param dt3dt2 Optional output derivatives of rvec3 or tvec3 with regard to rvec1, rvec2, tvec1 and
tvec2, respectively.

The functions compute:

\f[\begin{array}{l} \texttt{rvec3} =  \mathrm{rodrigues} ^{-1} \left ( \mathrm{rodrigues} (
\texttt{rvec2} )  \cdot \mathrm{rodrigues} ( \texttt{rvec1} ) \right )  \\ \texttt{tvec3} =
\mathrm{rodrigues} ( \texttt{rvec2} )  \cdot \texttt{tvec1} +  \texttt{tvec2} \end{array} ,\f]

where \f$\mathrm{rodrigues}\f$ denotes a rotation vector to a rotation matrix transformation, and
\f$\mathrm{rodrigues}^{-1}\f$ denotes the inverse transformation. See Rodrigues for details.

Also, the functions can compute the derivatives of the output vectors with regards to the input
vectors (see matMulDeriv ). The functions are used inside stereoCalibrate but can also be used in
your own code where Levenberg-Marquardt or another gradient-based solver is used to optimize a
function that contains a matrix multiplication.
 */
CV_EXPORTS_W void composeRT(
  InputArray rvec1,
  InputArray tvec1,
  InputArray rvec2,
  InputArray tvec2,
  OutputArray rvec3,
  OutputArray tvec3,
  OutputArray dr3dr1 = noArray(),
  OutputArray dr3dt1 = noArray(),
  OutputArray dr3dr2 = noArray(),
  OutputArray dr3dt2 = noArray(),
  OutputArray dt3dr1 = noArray(),
  OutputArray dt3dt1 = noArray(),
  OutputArray dt3dr2 = noArray(),
  OutputArray dt3dt2 = noArray());

/** @brief Projects 3D points to an image plane.

@param objectPoints Array of object points, 3xN/Nx3 1-channel or 1xN/Nx1 3-channel (or
vector\<Point3f\> ), where N is the number of points in the view.
@param rvec Rotation vector. See Rodrigues for details.
@param tvec Translation vector.
@param cameraMatrix Camera matrix \f$A = \vecthreethree{f_x}{0}{c_x}{0}{f_y}{c_y}{0}{0}{_1}\f$ .
@param distCoeffs Input vector of distortion coefficients
\f$(k_1, k_2, p_1, p_2[, k_3[, k_4, k_5, k_6 [, s_1, s_2, s_3, s_4[, \tau_x, \tau_y]]]])\f$ of
4, 5, 8, 12 or 14 elements. If the vector is empty, the zero distortion coefficients are assumed.
@param imagePoints Output array of image points, 2xN/Nx2 1-channel or 1xN/Nx1 2-channel, or
vector\<Point2f\> .
@param jacobian Optional output 2Nx(10+\<numDistCoeffs\>) jacobian matrix of derivatives of image
points with respect to components of the rotation vector, translation vector, focal lengths,
coordinates of the principal point and the distortion coefficients. In the old interface different
components of the jacobian are returned via different output parameters.
@param aspectRatio Optional "fixed aspect ratio" parameter. If the parameter is not 0, the
function assumes that the aspect ratio (*fx/fy*) is fixed and correspondingly adjusts the jacobian
matrix.

The function computes projections of 3D points to the image plane given intrinsic and extrinsic
camera parameters. Optionally, the function computes Jacobians - matrices of partial derivatives of
image points coordinates (as functions of all the input parameters) with respect to the particular
parameters, intrinsic and/or extrinsic. The Jacobians are used during the global optimization in
calibrateCamera, solvePnP, and stereoCalibrate . The function itself can also be used to compute a
re-projection error given the current intrinsic and extrinsic parameters.

@note By setting rvec=tvec=(0,0,0) or by setting cameraMatrix to a 3x3 identity matrix, or by
passing zero distortion coefficients, you can get various useful partial cases of the function. This
means that you can compute the distorted coordinates for a sparse set of points or apply a
perspective transformation (and also compute the derivatives) in the ideal zero-distortion setup.
 */
CV_EXPORTS_W void projectPoints(
  InputArray objectPoints,
  InputArray rvec,
  InputArray tvec,
  InputArray cameraMatrix,
  InputArray distCoeffs,
  OutputArray imagePoints,
  OutputArray jacobian = noArray(),
  double aspectRatio = 0);

/** @brief Finds an initial camera matrix from 3D-2D point correspondences.

@param objectPoints Vector of vectors of the calibration pattern points in the calibration pattern
coordinate space. In the old interface all the per-view vectors are concatenated. See
calibrateCamera for details.
@param imagePoints Vector of vectors of the projections of the calibration pattern points. In the
old interface all the per-view vectors are concatenated.
@param imageSize Image size in pixels used to initialize the principal point.
@param aspectRatio If it is zero or negative, both \f$f_x\f$ and \f$f_y\f$ are estimated
independently. Otherwise, \f$f_x = f_y * \texttt{aspectRatio}\f$ .

The function estimates and returns an initial camera matrix for the camera calibration process.
Currently, the function only supports planar calibration patterns, which are patterns where each
object point has z-coordinate =0.
 */
CV_EXPORTS_W Mat initCameraMatrix2D(
  InputArrayOfArrays objectPoints,
  InputArrayOfArrays imagePoints,
  Size imageSize,
  double aspectRatio = 1.0);

/** @brief Finds the camera intrinsic and extrinsic parameters from several views of a calibration
pattern.

@param objectPoints In the new interface it is a vector of vectors of calibration pattern points in
the calibration pattern coordinate space (e.g. std::vector<std::vector<cv::Vec3f>>). The outer
vector contains as many elements as the number of the pattern views. If the same calibration pattern
is shown in each view and it is fully visible, all the vectors will be the same. Although, it is
possible to use partially occluded patterns, or even different patterns in different views. Then,
the vectors will be different. The points are 3D, but since they are in a pattern coordinate system,
then, if the rig is planar, it may make sense to put the model to a XY coordinate plane so that
Z-coordinate of each input object point is 0.
In the old interface all the vectors of object points from different views are concatenated
together.
@param imagePoints In the new interface it is a vector of vectors of the projections of calibration
pattern points (e.g. std::vector<std::vector<cv::Vec2f>>). imagePoints.size() and
objectPoints.size() and imagePoints[i].size() must be equal to objectPoints[i].size() for each i.
In the old interface all the vectors of object points from different views are concatenated
together.
@param imageSize Size of the image used only to initialize the intrinsic camera matrix.
@param cameraMatrix Output 3x3 floating-point camera matrix
\f$A = \vecthreethree{f_x}{0}{c_x}{0}{f_y}{c_y}{0}{0}{1}\f$ . If CV\_CALIB\_USE\_INTRINSIC\_GUESS
and/or CV_CALIB_FIX_ASPECT_RATIO are specified, some or all of fx, fy, cx, cy must be
initialized before calling the function.
@param distCoeffs Output vector of distortion coefficients
\f$(k_1, k_2, p_1, p_2[, k_3[, k_4, k_5, k_6 [, s_1, s_2, s_3, s_4[, \tau_x, \tau_y]]]])\f$ of
4, 5, 8, 12 or 14 elements.
@param rvecs Output vector of rotation vectors (see Rodrigues ) estimated for each pattern view
(e.g. std::vector<cv::Mat>>). That is, each k-th rotation vector together with the corresponding
k-th translation vector (see the next output parameter description) brings the calibration pattern
from the model coordinate space (in which object points are specified) to the world coordinate
space, that is, a real position of the calibration pattern in the k-th pattern view (k=0.. *M* -1).
@param tvecs Output vector of translation vectors estimated for each pattern view.
@param stdDeviationsIntrinsics Output vector of standard deviations estimated for intrinsic
parameters. Order of deviations values:
\f$(f_x, f_y, c_x, c_y, k_1, k_2, p_1, p_2, k_3, k_4, k_5, k_6 , s_1, s_2, s_3,
 s_4, \tau_x, \tau_y)\f$ If one of parameters is not estimated, it's deviation is equals to zero.
@param stdDeviationsExtrinsics Output vector of standard deviations estimated for extrinsic
parameters. Order of deviations values: \f$(R_1, T_1, \dotsc , R_M, T_M)\f$ where M is number of
pattern views,
 \f$R_i, T_i\f$ are concatenated 1x3 vectors.
 @param perViewErrors Output vector of the RMS re-projection error estimated for each pattern view.
@param flags Different flags that may be zero or a combination of the following values:
-   **CV_CALIB_USE_INTRINSIC_GUESS** cameraMatrix contains valid initial values of
fx, fy, cx, cy that are optimized further. Otherwise, (cx, cy) is initially set to the image
center ( imageSize is used), and focal distances are computed in a least-squares fashion.
Note, that if intrinsic parameters are known, there is no need to use this function just to
estimate extrinsic parameters. Use solvePnP instead.
-   **CV_CALIB_FIX_PRINCIPAL_POINT** The principal point is not changed during the global
optimization. It stays at the center or at a different location specified when
CV_CALIB_USE_INTRINSIC_GUESS is set too.
-   **CV_CALIB_FIX_ASPECT_RATIO** The functions considers only fy as a free parameter. The
ratio fx/fy stays the same as in the input cameraMatrix . When
CV_CALIB_USE_INTRINSIC_GUESS is not set, the actual input values of fx and fy are
ignored, only their ratio is computed and used further.
-   **CV_CALIB_ZERO_TANGENT_DIST** Tangential distortion coefficients \f$(p_1, p_2)\f$ are set
to zeros and stay zero.
-   **CV_CALIB_FIX_K1,...,CV_CALIB_FIX_K6** The corresponding radial distortion
coefficient is not changed during the optimization. If CV_CALIB_USE_INTRINSIC_GUESS is
set, the coefficient from the supplied distCoeffs matrix is used. Otherwise, it is set to 0.
-   **CV_CALIB_RATIONAL_MODEL** Coefficients k4, k5, and k6 are enabled. To provide the
backward compatibility, this extra flag should be explicitly specified to make the
calibration function use the rational model and return 8 coefficients. If the flag is not
set, the function computes and returns only 5 distortion coefficients.
-   **CALIB_THIN_PRISM_MODEL** Coefficients s1, s2, s3 and s4 are enabled. To provide the
backward compatibility, this extra flag should be explicitly specified to make the
calibration function use the thin prism model and return 12 coefficients. If the flag is not
set, the function computes and returns only 5 distortion coefficients.
-   **CALIB_FIX_S1_S2_S3_S4** The thin prism distortion coefficients are not changed during
the optimization. If CV_CALIB_USE_INTRINSIC_GUESS is set, the coefficient from the
supplied distCoeffs matrix is used. Otherwise, it is set to 0.
-   **CALIB_TILTED_MODEL** Coefficients tauX and tauY are enabled. To provide the
backward compatibility, this extra flag should be explicitly specified to make the
calibration function use the tilted sensor model and return 14 coefficients. If the flag is not
set, the function computes and returns only 5 distortion coefficients.
-   **CALIB_FIX_TAUX_TAUY** The coefficients of the tilted sensor model are not changed during
the optimization. If CV_CALIB_USE_INTRINSIC_GUESS is set, the coefficient from the
supplied distCoeffs matrix is used. Otherwise, it is set to 0.
@param criteria Termination criteria for the iterative optimization algorithm.

@return the overall RMS re-projection error.

The function estimates the intrinsic camera parameters and extrinsic parameters for each of the
views. The algorithm is based on @cite Zhang2000 and @cite BouguetMCT . The coordinates of 3D object
points and their corresponding 2D projections in each view must be specified. That may be achieved
by using an object with a known geometry and easily detectable feature points. Such an object is
called a calibration rig or calibration pattern, and OpenCV has built-in support for a chessboard as
a calibration rig (see findChessboardCorners ). Currently, initialization of intrinsic parameters
(when CV_CALIB_USE_INTRINSIC_GUESS is not set) is only implemented for planar calibration
patterns (where Z-coordinates of the object points must be all zeros). 3D calibration rigs can also
be used as long as initial cameraMatrix is provided.

The algorithm performs the following steps:

-   Compute the initial intrinsic parameters (the option only available for planar calibration
    patterns) or read them from the input parameters. The distortion coefficients are all set to
    zeros initially unless some of CV_CALIB_FIX_K? are specified.

-   Estimate the initial camera pose as if the intrinsic parameters have been already known. This is
    done using solvePnP .

-   Run the global Levenberg-Marquardt optimization algorithm to minimize the reprojection error,
    that is, the total sum of squared distances between the observed feature points imagePoints and
    the projected (using the current estimates for camera parameters and the poses) object points
    objectPoints. See projectPoints for details.

@note
   If you use a non-square (=non-NxN) grid and findChessboardCorners for calibration, and
    calibrateCamera returns bad values (zero distortion coefficients, an image center very far from
    (w/2-0.5,h/2-0.5), and/or large differences between \f$f_x\f$ and \f$f_y\f$ (ratios of 10:1 or
more)), then you have probably used patternSize=cvSize(rows,cols) instead of using
    patternSize=cvSize(cols,rows) in findChessboardCorners .

@sa
   findChessboardCorners, solvePnP, initCameraMatrix2D, stereoCalibrate, undistort
 */
CV_EXPORTS_AS(calibrateCameraExtended)
double calibrateCamera(
  InputArrayOfArrays objectPoints,
  InputArrayOfArrays imagePoints,
  Size imageSize,
  InputOutputArray cameraMatrix,
  InputOutputArray distCoeffs,
  OutputArrayOfArrays rvecs,
  OutputArrayOfArrays tvecs,
  OutputArray stdDeviationsIntrinsics,
  OutputArray stdDeviationsExtrinsics,
  OutputArray perViewErrors,
  int flags = 0,
  TermCriteria criteria = TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 30, DBL_EPSILON));

/** @overload double calibrateCamera( InputArrayOfArrays objectPoints,
                                     InputArrayOfArrays imagePoints, Size imageSize,
                                     InputOutputArray cameraMatrix, InputOutputArray distCoeffs,
                                     OutputArrayOfArrays rvecs, OutputArrayOfArrays tvecs,
                                     OutputArray stdDeviations, OutputArray perViewErrors,
                                     int flags = 0, TermCriteria criteria = TermCriteria(
                                        TermCriteria::COUNT + TermCriteria::EPS, 30, DBL_EPSILON) )
 */
CV_EXPORTS_W double calibrateCamera(
  InputArrayOfArrays objectPoints,
  InputArrayOfArrays imagePoints,
  Size imageSize,
  InputOutputArray cameraMatrix,
  InputOutputArray distCoeffs,
  OutputArrayOfArrays rvecs,
  OutputArrayOfArrays tvecs,
  int flags = 0,
  TermCriteria criteria = TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 30, DBL_EPSILON));

/** @brief Computes useful camera characteristics from the camera matrix.

@param cameraMatrix Input camera matrix that can be estimated by calibrateCamera or
stereoCalibrate .
@param imageSize Input image size in pixels.
@param apertureWidth Physical width in mm of the sensor.
@param apertureHeight Physical height in mm of the sensor.
@param fovx Output field of view in degrees along the horizontal sensor axis.
@param fovy Output field of view in degrees along the vertical sensor axis.
@param focalLength Focal length of the lens in mm.
@param principalPoint Principal point in mm.
@param aspectRatio \f$f_y/f_x\f$

The function computes various useful camera characteristics from the previously estimated camera
matrix.

@note
   Do keep in mind that the unity measure 'mm' stands for whatever unit of measure one chooses for
    the chessboard pitch (it can thus be any value).
 */
CV_EXPORTS_W void calibrationMatrixValues(
  InputArray cameraMatrix,
  Size imageSize,
  double apertureWidth,
  double apertureHeight,
  CV_OUT double &fovx,
  CV_OUT double &fovy,
  CV_OUT double &focalLength,
  CV_OUT Point2d &principalPoint,
  CV_OUT double &aspectRatio);

/** @brief Calibrates the stereo camera.

@param objectPoints Vector of vectors of the calibration pattern points.
@param imagePoints1 Vector of vectors of the projections of the calibration pattern points,
observed by the first camera.
@param imagePoints2 Vector of vectors of the projections of the calibration pattern points,
observed by the second camera.
@param cameraMatrix1 Input/output first camera matrix:
\f$\vecthreethree{f_x^{(j)}}{0}{c_x^{(j)}}{0}{f_y^{(j)}}{c_y^{(j)}}{0}{0}{1}\f$ , \f$j = 0,\, 1\f$ .
If any of CV_CALIB_USE_INTRINSIC_GUESS , CV_CALIB_FIX_ASPECT_RATIO , CV_CALIB_FIX_INTRINSIC , or
CV_CALIB_FIX_FOCAL_LENGTH are specified, some or all of the matrix components must be initialized.
See the flags description for details.
@param distCoeffs1 Input/output vector of distortion coefficients
\f$(k_1, k_2, p_1, p_2[, k_3[, k_4, k_5, k_6 [, s_1, s_2, s_3, s_4[, \tau_x, \tau_y]]]])\f$ of
4, 5, 8, 12 or 14 elements. The output vector length depends on the flags.
@param cameraMatrix2 Input/output second camera matrix. The parameter is similar to cameraMatrix1
@param distCoeffs2 Input/output lens distortion coefficients for the second camera. The parameter
is similar to distCoeffs1 .
@param imageSize Size of the image used only to initialize intrinsic camera matrix.
@param R Output rotation matrix between the 1st and the 2nd camera coordinate systems.
@param T Output translation vector between the coordinate systems of the cameras.
@param E Output essential matrix.
@param F Output fundamental matrix.
@param flags Different flags that may be zero or a combination of the following values:
-   **CV_CALIB_FIX_INTRINSIC** Fix cameraMatrix? and distCoeffs? so that only R, T, E , and F
matrices are estimated.
-   **CV_CALIB_USE_INTRINSIC_GUESS** Optimize some or all of the intrinsic parameters
according to the specified flags. Initial values are provided by the user.
-   **CV_CALIB_FIX_PRINCIPAL_POINT** Fix the principal points during the optimization.
-   **CV_CALIB_FIX_FOCAL_LENGTH** Fix \f$f^{(j)}_x\f$ and \f$f^{(j)}_y\f$ .
-   **CV_CALIB_FIX_ASPECT_RATIO** Optimize \f$f^{(j)}_y\f$ . Fix the ratio \f$f^{(j)}_x/f^{(j)}_y\f$
.
-   **CV_CALIB_SAME_FOCAL_LENGTH** Enforce \f$f^{(0)}_x=f^{(1)}_x\f$ and \f$f^{(0)}_y=f^{(1)}_y\f$ .
-   **CV_CALIB_ZERO_TANGENT_DIST** Set tangential distortion coefficients for each camera to
zeros and fix there.
-   **CV_CALIB_FIX_K1,...,CV_CALIB_FIX_K6** Do not change the corresponding radial
distortion coefficient during the optimization. If CV_CALIB_USE_INTRINSIC_GUESS is set,
the coefficient from the supplied distCoeffs matrix is used. Otherwise, it is set to 0.
-   **CV_CALIB_RATIONAL_MODEL** Enable coefficients k4, k5, and k6. To provide the backward
compatibility, this extra flag should be explicitly specified to make the calibration
function use the rational model and return 8 coefficients. If the flag is not set, the
function computes and returns only 5 distortion coefficients.
-   **CALIB_THIN_PRISM_MODEL** Coefficients s1, s2, s3 and s4 are enabled. To provide the
backward compatibility, this extra flag should be explicitly specified to make the
calibration function use the thin prism model and return 12 coefficients. If the flag is not
set, the function computes and returns only 5 distortion coefficients.
-   **CALIB_FIX_S1_S2_S3_S4** The thin prism distortion coefficients are not changed during
the optimization. If CV_CALIB_USE_INTRINSIC_GUESS is set, the coefficient from the
supplied distCoeffs matrix is used. Otherwise, it is set to 0.
-   **CALIB_TILTED_MODEL** Coefficients tauX and tauY are enabled. To provide the
backward compatibility, this extra flag should be explicitly specified to make the
calibration function use the tilted sensor model and return 14 coefficients. If the flag is not
set, the function computes and returns only 5 distortion coefficients.
-   **CALIB_FIX_TAUX_TAUY** The coefficients of the tilted sensor model are not changed during
the optimization. If CV_CALIB_USE_INTRINSIC_GUESS is set, the coefficient from the
supplied distCoeffs matrix is used. Otherwise, it is set to 0.
@param criteria Termination criteria for the iterative optimization algorithm.

The function estimates transformation between two cameras making a stereo pair. If you have a stereo
camera where the relative position and orientation of two cameras is fixed, and if you computed
poses of an object relative to the first camera and to the second camera, (R1, T1) and (R2, T2),
respectively (this can be done with solvePnP ), then those poses definitely relate to each other.
This means that, given ( \f$R_1\f$,\f$T_1\f$ ), it should be possible to compute (
\f$R_2\f$,\f$T_2\f$ ). You only need to know the position and orientation of the second camera
relative to the first camera. This is what the described function does. It computes (
\f$R\f$,\f$T\f$ ) so that:

\f[R_2=R*R_1
T_2=R*T_1 + T,\f]

Optionally, it computes the essential matrix E:

\f[E= \vecthreethree{0}{-T_2}{T_1}{T_2}{0}{-T_0}{-T_1}{T_0}{0} *R\f]

where \f$T_i\f$ are components of the translation vector \f$T\f$ : \f$T=[T_0, T_1, T_2]^T\f$ . And
the function can also compute the fundamental matrix F:

\f[F = cameraMatrix2^{-T} E cameraMatrix1^{-1}\f]

Besides the stereo-related information, the function can also perform a full calibration of each of
two cameras. However, due to the high dimensionality of the parameter space and noise in the input
data, the function can diverge from the correct solution. If the intrinsic parameters can be
estimated with high accuracy for each of the cameras individually (for example, using
calibrateCamera ), you are recommended to do so and then pass CV_CALIB_FIX_INTRINSIC flag to the
function along with the computed intrinsic parameters. Otherwise, if all the parameters are
estimated at once, it makes sense to restrict some parameters, for example, pass
CV_CALIB_SAME_FOCAL_LENGTH and CV_CALIB_ZERO_TANGENT_DIST flags, which is usually a
reasonable assumption.

Similarly to calibrateCamera , the function minimizes the total re-projection error for all the
points in all the available views from both cameras. The function returns the final value of the
re-projection error.
 */
CV_EXPORTS_W double stereoCalibrate(
  InputArrayOfArrays objectPoints,
  InputArrayOfArrays imagePoints1,
  InputArrayOfArrays imagePoints2,
  InputOutputArray cameraMatrix1,
  InputOutputArray distCoeffs1,
  InputOutputArray cameraMatrix2,
  InputOutputArray distCoeffs2,
  Size imageSize,
  OutputArray R,
  OutputArray T,
  OutputArray E,
  OutputArray F,
  int flags = CALIB_FIX_INTRINSIC,
  TermCriteria criteria = TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 30, 1e-6));

/** @brief Computes rectification transforms for each head of a calibrated stereo camera.

@param cameraMatrix1 First camera matrix.
@param distCoeffs1 First camera distortion parameters.
@param cameraMatrix2 Second camera matrix.
@param distCoeffs2 Second camera distortion parameters.
@param imageSize Size of the image used for stereo calibration.
@param R Rotation matrix between the coordinate systems of the first and the second cameras.
@param T Translation vector between coordinate systems of the cameras.
@param R1 Output 3x3 rectification transform (rotation matrix) for the first camera.
@param R2 Output 3x3 rectification transform (rotation matrix) for the second camera.
@param P1 Output 3x4 projection matrix in the new (rectified) coordinate systems for the first
camera.
@param P2 Output 3x4 projection matrix in the new (rectified) coordinate systems for the second
camera.
@param Q Output \f$4 \times 4\f$ disparity-to-depth mapping matrix (see reprojectImageTo3D ).
@param flags Operation flags that may be zero or CV_CALIB_ZERO_DISPARITY . If the flag is set,
the function makes the principal points of each camera have the same pixel coordinates in the
rectified views. And if the flag is not set, the function may still shift the images in the
horizontal or vertical direction (depending on the orientation of epipolar lines) to maximize the
useful image area.
@param alpha Free scaling parameter. If it is -1 or absent, the function performs the default
scaling. Otherwise, the parameter should be between 0 and 1. alpha=0 means that the rectified
images are zoomed and shifted so that only valid pixels are visible (no black areas after
rectification). alpha=1 means that the rectified image is decimated and shifted so that all the
pixels from the original images from the cameras are retained in the rectified images (no source
image pixels are lost). Obviously, any intermediate value yields an intermediate result between
those two extreme cases.
@param newImageSize New image resolution after rectification. The same size should be passed to
initUndistortRectifyMap (see the stereo_calib.cpp sample in OpenCV samples directory). When (0,0)
is passed (default), it is set to the original imageSize . Setting it to larger value can help you
preserve details in the original image, especially when there is a big radial distortion.
@param validPixROI1 Optional output rectangles inside the rectified images where all the pixels
are valid. If alpha=0 , the ROIs cover the whole images. Otherwise, they are likely to be smaller
(see the picture below).
@param validPixROI2 Optional output rectangles inside the rectified images where all the pixels
are valid. If alpha=0 , the ROIs cover the whole images. Otherwise, they are likely to be smaller
(see the picture below).

The function computes the rotation matrices for each camera that (virtually) make both camera image
planes the same plane. Consequently, this makes all the epipolar lines parallel and thus simplifies
the dense stereo correspondence problem. The function takes the matrices computed by stereoCalibrate
as input. As output, it provides two rotation matrices and also two projection matrices in the new
coordinates. The function distinguishes the following two cases:

-   **Horizontal stereo**: the first and the second camera views are shifted relative to each other
    mainly along the x axis (with possible small vertical shift). In the rectified images, the
    corresponding epipolar lines in the left and right cameras are horizontal and have the same
    y-coordinate. P1 and P2 look like:

    \f[\texttt{P1} = \begin{bmatrix} f & 0 & cx_1 & 0 \\ 0 & f & cy & 0 \\ 0 & 0 & 1 & 0
\end{bmatrix}\f]

    \f[\texttt{P2} = \begin{bmatrix} f & 0 & cx_2 & T_x*f \\ 0 & f & cy & 0 \\ 0 & 0 & 1 & 0
\end{bmatrix} ,\f]

    where \f$T_x\f$ is a horizontal shift between the cameras and \f$cx_1=cx_2\f$ if
    CV_CALIB_ZERO_DISPARITY is set.

-   **Vertical stereo**: the first and the second camera views are shifted relative to each other
    mainly in vertical direction (and probably a bit in the horizontal direction too). The epipolar
    lines in the rectified images are vertical and have the same x-coordinate. P1 and P2 look like:

    \f[\texttt{P1} = \begin{bmatrix} f & 0 & cx & 0 \\ 0 & f & cy_1 & 0 \\ 0 & 0 & 1 & 0
\end{bmatrix}\f]

    \f[\texttt{P2} = \begin{bmatrix} f & 0 & cx & 0 \\ 0 & f & cy_2 & T_y*f \\ 0 & 0 & 1 & 0
\end{bmatrix} ,\f]

    where \f$T_y\f$ is a vertical shift between the cameras and \f$cy_1=cy_2\f$ if
CALIB_ZERO_DISPARITY is set.

As you can see, the first three columns of P1 and P2 will effectively be the new "rectified" camera
matrices. The matrices, together with R1 and R2 , can then be passed to initUndistortRectifyMap to
initialize the rectification map for each camera.

See below the screenshot from the stereo_calib.cpp sample. Some red horizontal lines pass through
the corresponding image regions. This means that the images are well rectified, which is what most
stereo correspondence algorithms rely on. The green rectangles are roi1 and roi2 . You see that
their interiors are all valid pixels.

![image](pics/stereo_undistort.jpg)
 */
CV_EXPORTS_W void stereoRectify(
  InputArray cameraMatrix1,
  InputArray distCoeffs1,
  InputArray cameraMatrix2,
  InputArray distCoeffs2,
  Size imageSize,
  InputArray R,
  InputArray T,
  OutputArray R1,
  OutputArray R2,
  OutputArray P1,
  OutputArray P2,
  OutputArray Q,
  int flags = CALIB_ZERO_DISPARITY,
  double alpha = -1,
  Size newImageSize = Size(),
  CV_OUT Rect *validPixROI1 = 0,
  CV_OUT Rect *validPixROI2 = 0);

/** @brief Computes a rectification transform for an uncalibrated stereo camera.

@param points1 Array of feature points in the first image.
@param points2 The corresponding points in the second image. The same formats as in
findFundamentalMat are supported.
@param F Input fundamental matrix. It can be computed from the same set of point pairs using
findFundamentalMat .
@param imgSize Size of the image.
@param H1 Output rectification homography matrix for the first image.
@param H2 Output rectification homography matrix for the second image.
@param threshold Optional threshold used to filter out the outliers. If the parameter is greater
than zero, all the point pairs that do not comply with the epipolar geometry (that is, the points
for which \f$|\texttt{points2[i]}^T*\texttt{F}*\texttt{points1[i]}|>\texttt{threshold}\f$ ) are
rejected prior to computing the homographies. Otherwise,all the points are considered inliers.

The function computes the rectification transformations without knowing intrinsic parameters of the
cameras and their relative position in the space, which explains the suffix "uncalibrated". Another
related difference from stereoRectify is that the function outputs not the rectification
transformations in the object (3D) space, but the planar perspective transformations encoded by the
homography matrices H1 and H2 . The function implements the algorithm @cite Hartley99 .

@note
   While the algorithm does not need to know the intrinsic parameters of the cameras, it heavily
    depends on the epipolar geometry. Therefore, if the camera lenses have a significant distortion,
    it would be better to correct it before computing the fundamental matrix and calling this
    function. For example, distortion coefficients can be estimated for each head of stereo camera
    separately by using calibrateCamera . Then, the images can be corrected using undistort , or
    just the point coordinates can be corrected with undistortPoints .
 */
CV_EXPORTS_W bool stereoRectifyUncalibrated(
  InputArray points1,
  InputArray points2,
  InputArray F,
  Size imgSize,
  OutputArray H1,
  OutputArray H2,
  double threshold = 5);

/** @brief Returns the new camera matrix based on the free scaling parameter.

@param cameraMatrix Input camera matrix.
@param distCoeffs Input vector of distortion coefficients
\f$(k_1, k_2, p_1, p_2[, k_3[, k_4, k_5, k_6 [, s_1, s_2, s_3, s_4[, \tau_x, \tau_y]]]])\f$ of
4, 5, 8, 12 or 14 elements. If the vector is NULL/empty, the zero distortion coefficients are
assumed.
@param imageSize Original image size.
@param alpha Free scaling parameter between 0 (when all the pixels in the undistorted image are
valid) and 1 (when all the source image pixels are retained in the undistorted image). See
stereoRectify for details.
@param newImgSize Image size after rectification. By default,it is set to imageSize .
@param validPixROI Optional output rectangle that outlines all-good-pixels region in the
undistorted image. See roi1, roi2 description in stereoRectify .
@param centerPrincipalPoint Optional flag that indicates whether in the new camera matrix the
principal point should be at the image center or not. By default, the principal point is chosen to
best fit a subset of the source image (determined by alpha) to the corrected image.
@return new_camera_matrix Output new camera matrix.

The function computes and returns the optimal new camera matrix based on the free scaling parameter.
By varying this parameter, you may retrieve only sensible pixels alpha=0 , keep all the original
image pixels if there is valuable information in the corners alpha=1 , or get something in between.
When alpha\>0 , the undistortion result is likely to have some black pixels corresponding to
"virtual" pixels outside of the captured distorted image. The original camera matrix, distortion
coefficients, the computed new camera matrix, and newImageSize should be passed to
initUndistortRectifyMap to produce the maps for remap .
 */
CV_EXPORTS_W Mat getOptimalNewCameraMatrix(
  InputArray cameraMatrix,
  InputArray distCoeffs,
  Size imageSize,
  double alpha,
  Size newImgSize = Size(),
  CV_OUT Rect *validPixROI = 0,
  bool centerPrincipalPoint = false);

/** @brief Computes an RQ decomposition of 3x3 matrices.

@param src 3x3 input matrix.
@param mtxR Output 3x3 upper-triangular matrix.
@param mtxQ Output 3x3 orthogonal matrix.
@param Qx Optional output 3x3 rotation matrix around x-axis.
@param Qy Optional output 3x3 rotation matrix around y-axis.
@param Qz Optional output 3x3 rotation matrix around z-axis.

The function computes a RQ decomposition using the given rotations. This function is used in
decomposeProjectionMatrix to decompose the left 3x3 submatrix of a projection matrix into a camera
and a rotation matrix.

It optionally returns three rotation matrices, one for each axis, and the three Euler angles in
degrees (as the return value) that could be used in OpenGL. Note, there is always more than one
sequence of rotations about the three principal axes that results in the same orientation of an
object, eg. see @cite Slabaugh . Returned tree rotation matrices and corresponding three Euler
angules are only one of the possible solutions.
 */
CV_EXPORTS_W Vec3d RQDecomp3x3(
  InputArray src,
  OutputArray mtxR,
  OutputArray mtxQ,
  OutputArray Qx = noArray(),
  OutputArray Qy = noArray(),
  OutputArray Qz = noArray());

/** @brief Decomposes a projection matrix into a rotation matrix and a camera matrix.

@param projMatrix 3x4 input projection matrix P.
@param cameraMatrix Output 3x3 camera matrix K.
@param rotMatrix Output 3x3 external rotation matrix R.
@param transVect Output 4x1 translation vector T.
@param rotMatrixX Optional 3x3 rotation matrix around x-axis.
@param rotMatrixY Optional 3x3 rotation matrix around y-axis.
@param rotMatrixZ Optional 3x3 rotation matrix around z-axis.
@param eulerAngles Optional three-element vector containing three Euler angles of rotation in
degrees.

The function computes a decomposition of a projection matrix into a calibration and a rotation
matrix and the position of a camera.

It optionally returns three rotation matrices, one for each axis, and three Euler angles that could
be used in OpenGL. Note, there is always more than one sequence of rotations about the three
principal axes that results in the same orientation of an object, eg. see @cite Slabaugh . Returned
tree rotation matrices and corresponding three Euler angules are only one of the possible solutions.

The function is based on RQDecomp3x3 .
 */
CV_EXPORTS_W void decomposeProjectionMatrix(
  InputArray projMatrix,
  OutputArray cameraMatrix,
  OutputArray rotMatrix,
  OutputArray transVect,
  OutputArray rotMatrixX = noArray(),
  OutputArray rotMatrixY = noArray(),
  OutputArray rotMatrixZ = noArray(),
  OutputArray eulerAngles = noArray());

//! computes the rectification transformations for 3-head camera, where all the heads are on the
//! same line.
CV_EXPORTS_W float rectify3Collinear(
  InputArray cameraMatrix1,
  InputArray distCoeffs1,
  InputArray cameraMatrix2,
  InputArray distCoeffs2,
  InputArray cameraMatrix3,
  InputArray distCoeffs3,
  InputArrayOfArrays imgpt1,
  InputArrayOfArrays imgpt3,
  Size imageSize,
  InputArray R12,
  InputArray T12,
  InputArray R13,
  InputArray T13,
  OutputArray R1,
  OutputArray R2,
  OutputArray R3,
  OutputArray P1,
  OutputArray P2,
  OutputArray P3,
  OutputArray Q,
  double alpha,
  Size newImgSize,
  CV_OUT Rect *roi1,
  CV_OUT Rect *roi2,
  int flags);

}  // namespace c8cv
