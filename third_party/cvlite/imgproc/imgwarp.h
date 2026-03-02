/*M///////////////////////////////////////////////////////////////////////////////////////
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
// Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
// Copyright (C) 2009, Willow Garage Inc., all rights reserved.
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
//
//M*/

#pragma once

#include "third_party/cvlite/core/core.hpp"

namespace c8cv {

//! @addtogroup imgproc_transform
//! @{

//! interpolation algorithm
enum InterpolationFlags {
  /** nearest neighbor interpolation */
  INTER_NEAREST = 0,
  /** bilinear interpolation */
  INTER_LINEAR = 1,
  /** bicubic interpolation */
  INTER_CUBIC = 2,
  /** resampling using pixel area relation. It may be a preferred method for image decimation, as
  it gives moire'-free results. But when the image is zoomed, it is similar to the INTER_NEAREST
  method. */
  INTER_AREA = 3,
  /** Lanczos interpolation over 8x8 neighborhood */
  INTER_LANCZOS4 = 4,
  /** mask for interpolation codes */
  INTER_MAX = 7,
  /** flag, fills all of the destination image pixels. If some of them correspond to outliers in the
  source image, they are set to zero */
  WARP_FILL_OUTLIERS = 8,
  /** flag, inverse transformation

  For example, @ref c8cv::linearPolar or @ref c8cv::logPolar transforms:
  - flag is __not__ set: \f$dst( \rho , \phi ) = src(x,y)\f$
  - flag is set: \f$dst(x,y) = src( \rho , \phi )\f$
  */
  WARP_INVERSE_MAP = 16
};

enum InterpolationMasks {
  INTER_BITS = 5,
  INTER_BITS2 = INTER_BITS * 2,
  INTER_TAB_SIZE = 1 << INTER_BITS,
  INTER_TAB_SIZE2 = INTER_TAB_SIZE * INTER_TAB_SIZE
};

//! @} imgproc_transform

//! @addtogroup imgproc_transform
//! @{

/** @brief Resizes an image.

The function resize resizes the image src down to or up to the specified size. Note that the
initial dst type or size are not taken into account. Instead, the size and type are derived from
the `src`,`dsize`,`fx`, and `fy`. If you want to resize src so that it fits the pre-created dst,
you may call the function as follows:
@code
    // explicitly specify dsize=dst.size(); fx and fy will be computed from that.
    resize(src, dst, dst.size(), 0, 0, interpolation);
@endcode
If you want to decimate the image by factor of 2 in each direction, you can call the function this
way:
@code
    // specify fx and fy and let the function compute the destination image size.
    resize(src, dst, c8cv::Size(), 0.5, 0.5, interpolation);
@endcode
To shrink an image, it will generally look best with c8cv::INTER_AREA interpolation, whereas to
enlarge an image, it will generally look best with c8cv::INTER_CUBIC (slow) or c8cv::INTER_LINEAR
(faster but still looks OK).

@param src input image.
@param dst output image; it has the size dsize (when it is non-zero) or the size computed from
src.size(), fx, and fy; the type of dst is the same as of src.
@param dsize output image size; if it equals zero, it is computed as:
 \f[\texttt{dsize = c8cv::Size(round(fx*src.cols), round(fy*src.rows))}\f]
 Either dsize or both fx and fy must be non-zero.
@param fx scale factor along the horizontal axis; when it equals 0, it is computed as
\f[\texttt{(double)dsize.width/src.cols}\f]
@param fy scale factor along the vertical axis; when it equals 0, it is computed as
\f[\texttt{(double)dsize.height/src.rows}\f]
@param interpolation interpolation method, see c8cv::InterpolationFlags

@sa  warpAffine, warpPerspective, remap
 */
void resize(
  c8cv::InputArray src,
  c8cv::OutputArray dst,
  c8cv::Size dsize,
  double fx = 0,
  double fy = 0,
  int interpolation = INTER_LINEAR);

/** @brief Applies an affine transformation to an image.

The function warpAffine transforms the source image using the specified matrix:

\f[\texttt{dst} (x,y) =  \texttt{src} ( \texttt{M} _{11} x +  \texttt{M} _{12} y +  \texttt{M}
_{13}, \texttt{M} _{21} x +  \texttt{M} _{22} y +  \texttt{M} _{23})\f]

when the flag WARP_INVERSE_MAP is set. Otherwise, the transformation is first inverted
with c8cv::invertAffineTransform and then put in the formula above instead of M. The function cannot
operate in-place.

@param src input image.
@param dst output image that has the size dsize and the same type as src .
@param M \f$2\times 3\f$ transformation matrix.
@param dsize size of the output image.
@param flags combination of interpolation methods (see c8cv::InterpolationFlags) and the optional
flag WARP_INVERSE_MAP that means that M is the inverse transformation (
\f$\texttt{dst}\rightarrow\texttt{src}\f$ ).
@param borderMode pixel extrapolation method (see c8cv::BorderTypes); when
borderMode=BORDER_TRANSPARENT, it means that the pixels in the destination image corresponding to
the "outliers" in the source image are not modified by the function.
@param borderValue value used in case of a constant border; by default, it is 0.

@sa  warpPerspective, resize, remap, getRectSubPix, transform
 */
void warpAffine(
  c8cv::InputArray src,
  c8cv::OutputArray dst,
  c8cv::InputArray M,
  c8cv::Size dsize,
  int flags = INTER_LINEAR,
  int borderMode = c8cv::BORDER_CONSTANT,
  const c8cv::Scalar &borderValue = c8cv::Scalar());

/** @brief Applies a perspective transformation to an image.

The function warpPerspective transforms the source image using the specified matrix:

\f[\texttt{dst} (x,y) =  \texttt{src} \left ( \frac{M_{11} x + M_{12} y + M_{13}}{M_{31} x + M_{32}
y + M_{33}} , \frac{M_{21} x + M_{22} y + M_{23}}{M_{31} x + M_{32} y + M_{33}} \right )\f]

when the flag WARP_INVERSE_MAP is set. Otherwise, the transformation is first inverted with invert
and then put in the formula above instead of M. The function cannot operate in-place.

@param src input image.
@param dst output image that has the size dsize and the same type as src .
@param M \f$3\times 3\f$ transformation matrix.
@param dsize size of the output image.
@param flags combination of interpolation methods (INTER_LINEAR or INTER_NEAREST) and the
optional flag WARP_INVERSE_MAP, that sets M as the inverse transformation (
\f$\texttt{dst}\rightarrow\texttt{src}\f$ ).
@param borderMode pixel extrapolation method (BORDER_CONSTANT or BORDER_REPLICATE).
@param borderValue value used in case of a constant border; by default, it equals 0.

@sa  warpAffine, resize, remap, getRectSubPix, perspectiveTransform
 */
void warpPerspective(
  c8cv::InputArray src,
  c8cv::OutputArray dst,
  c8cv::InputArray M,
  c8cv::Size dsize,
  int flags = INTER_LINEAR,
  int borderMode = c8cv::BORDER_CONSTANT,
  const c8cv::Scalar &borderValue = c8cv::Scalar());

/** @brief Applies a generic geometrical transformation to an image.

The function remap transforms the source image using the specified map:

\f[\texttt{dst} (x,y) =  \texttt{src} (map_x(x,y),map_y(x,y))\f]

where values of pixels with non-integer coordinates are computed using one of available
interpolation methods. \f$map_x\f$ and \f$map_y\f$ can be encoded as separate floating-point maps
in \f$map_1\f$ and \f$map_2\f$ respectively, or interleaved floating-point maps of \f$(x,y)\f$ in
\f$map_1\f$, or fixed-point maps created by using convertMaps. The reason you might want to
convert from floating to fixed-point representations of a map is that they can yield much faster
(\~2x) remapping operations. In the converted case, \f$map_1\f$ contains pairs (cvFloor(x),
cvFloor(y)) and \f$map_2\f$ contains indices in a table of interpolation coefficients.

This function cannot operate in-place.

@param src Source image.
@param dst Destination image. It has the same size as map1 and the same type as src .
@param map1 The first map of either (x,y) points or just x values having the type CV_16SC2 ,
CV_32FC1, or CV_32FC2. See convertMaps for details on converting a floating point
representation to fixed-point for speed.
@param map2 The second map of y values having the type CV_16UC1, CV_32FC1, or none (empty map
if map1 is (x,y) points), respectively.
@param interpolation Interpolation method (see c8cv::InterpolationFlags). The method INTER_AREA is
not supported by this function.
@param borderMode Pixel extrapolation method (see c8cv::BorderTypes). When
borderMode=BORDER_TRANSPARENT, it means that the pixels in the destination image that
corresponds to the "outliers" in the source image are not modified by the function.
@param borderValue Value used in case of a constant border. By default, it is 0.
@note
Due to current implementaion limitations the size of an input and output images should be less than
32767x32767.
 */
void remap(
  c8cv::InputArray src,
  c8cv::OutputArray dst,
  c8cv::InputArray map1,
  c8cv::InputArray map2,
  int interpolation,
  int borderMode = c8cv::BORDER_CONSTANT,
  const c8cv::Scalar &borderValue = c8cv::Scalar());

/** @brief Converts image transformation maps from one representation to another.

The function converts a pair of maps for remap from one representation to another. The following
options ( (map1.type(), map2.type()) \f$\rightarrow\f$ (dstmap1.type(), dstmap2.type()) ) are
supported:

- \f$\texttt{(CV_32FC1, CV_32FC1)} \rightarrow \texttt{(CV_16SC2, CV_16UC1)}\f$. This is the
most frequently used conversion operation, in which the original floating-point maps (see remap )
are converted to a more compact and much faster fixed-point representation. The first output array
contains the rounded coordinates and the second array (created only when nninterpolation=false )
contains indices in the interpolation tables.

- \f$\texttt{(CV_32FC2)} \rightarrow \texttt{(CV_16SC2, CV_16UC1)}\f$. The same as above but
the original maps are stored in one 2-channel matrix.

- Reverse conversion. Obviously, the reconstructed floating-point maps will not be exactly the same
as the originals.

@param map1 The first input map of type CV_16SC2, CV_32FC1, or CV_32FC2 .
@param map2 The second input map of type CV_16UC1, CV_32FC1, or none (empty matrix),
respectively.
@param dstmap1 The first output map that has the type dstmap1type and the same size as src .
@param dstmap2 The second output map.
@param dstmap1type Type of the first output map that should be CV_16SC2, CV_32FC1, or
CV_32FC2 .
@param nninterpolation Flag indicating whether the fixed-point maps are used for the
nearest-neighbor or for a more complex interpolation.

@sa  remap, undistort, initUndistortRectifyMap
 */
void convertMaps(
  c8cv::InputArray map1,
  c8cv::InputArray map2,
  c8cv::OutputArray dstmap1,
  c8cv::OutputArray dstmap2,
  int dstmap1type,
  bool nninterpolation = false);

/** @brief Calculates an affine matrix of 2D rotation.

The function calculates the following matrix:

\f[\begin{bmatrix} \alpha &  \beta & (1- \alpha )  \cdot \texttt{center.x} -  \beta \cdot
\texttt{center.y} \\ - \beta &  \alpha &  \beta \cdot \texttt{center.x} + (1- \alpha )  \cdot
\texttt{center.y} \end{bmatrix}\f]

where

\f[\begin{array}{l} \alpha =  \texttt{scale} \cdot \cos \texttt{angle} , \\ \beta =  \texttt{scale}
\cdot \sin \texttt{angle} \end{array}\f]

The transformation maps the rotation center to itself. If this is not the target, adjust the shift.

@param center Center of the rotation in the source image.
@param angle Rotation angle in degrees. Positive values mean counter-clockwise rotation (the
coordinate origin is assumed to be the top-left corner).
@param scale Isotropic scale factor.

@sa  getAffineTransform, warpAffine, transform
 */
c8cv::Mat getRotationMatrix2D(c8cv::Point2f center, double angle, double scale);

//! returns 3x3 perspective transformation for the corresponding 4 point pairs.
c8cv::Mat getPerspectiveTransform(const c8cv::Point2f src[], const c8cv::Point2f dst[]);

/** @brief Calculates an affine transform from three pairs of the corresponding points.

The function calculates the \f$2 \times 3\f$ matrix of an affine transform so that:

\f[\begin{bmatrix} x'_i \\ y'_i \end{bmatrix} = \texttt{map_matrix} \cdot \begin{bmatrix} x_i \\ y_i
\\ 1 \end{bmatrix}\f]

where

\f[dst(i)=(x'_i,y'_i), src(i)=(x_i, y_i), i=0,1,2\f]

@param src Coordinates of triangle vertices in the source image.
@param dst Coordinates of the corresponding triangle vertices in the destination image.

@sa  warpAffine, transform
 */
c8cv::Mat getAffineTransform(const c8cv::Point2f src[], const c8cv::Point2f dst[]);

/** @brief Inverts an affine transformation.

The function computes an inverse affine transformation represented by \f$2 \times 3\f$ matrix M:

\f[\begin{bmatrix} a_{11} & a_{12} & b_1  \\ a_{21} & a_{22} & b_2 \end{bmatrix}\f]

The result is also a \f$2 \times 3\f$ matrix of the same type as M.

@param M Original affine transformation.
@param iM Output reverse affine transformation.
 */
void invertAffineTransform(c8cv::InputArray M, c8cv::OutputArray iM);

/** @brief Calculates a perspective transform from four pairs of the corresponding points.

The function calculates the \f$3 \times 3\f$ matrix of a perspective transform so that:

\f[\begin{bmatrix} t_i x'_i \\ t_i y'_i \\ t_i \end{bmatrix} = \texttt{map_matrix} \cdot
\begin{bmatrix} x_i \\ y_i \\ 1 \end{bmatrix}\f]

where

\f[dst(i)=(x'_i,y'_i), src(i)=(x_i, y_i), i=0,1,2,3\f]

@param src Coordinates of quadrangle vertices in the source image.
@param dst Coordinates of the corresponding quadrangle vertices in the destination image.

@sa  findHomography, warpPerspective, perspectiveTransform
 */
c8cv::Mat getPerspectiveTransform(c8cv::InputArray src, c8cv::InputArray dst);

c8cv::Mat getAffineTransform(c8cv::InputArray src, c8cv::InputArray dst);

/** @example polar_transforms.cpp
An example using the c8cv::linearPolar and c8cv::logPolar operations
*/

/** @brief Remaps an image to semilog-polar coordinates space.

Transform the source image using the following transformation (See @ref polar_remaps_reference_image
"Polar remaps reference image"): \f[\begin{array}{l}
  dst( \rho , \phi ) = src(x,y) \\
  dst.size() \leftarrow src.size()
\end{array}\f]

where
\f[\begin{array}{l}
  I = (dx,dy) = (x - center.x,y - center.y) \\
  \rho = M \cdot log_e(\texttt{magnitude} (I)) ,\\
  \phi = Ky \cdot \texttt{angle} (I)_{0..360 deg} \\
\end{array}\f]

and
\f[\begin{array}{l}
  M = src.cols / log_e(maxRadius) \\
  Ky = src.rows / 360 \\
\end{array}\f]

The function emulates the human "foveal" vision and can be used for fast scale and
rotation-invariant template matching, for object tracking and so forth.
@param src Source image
@param dst Destination image. It will have same size and type as src.
@param center The transformation center; where the output precision is maximal
@param M Magnitude scale parameter. It determines the radius of the bounding circle to transform
too.
@param flags A combination of interpolation methods, see c8cv::InterpolationFlags

@note
-   The function can not operate in-place.
-   To calculate magnitude and angle in degrees @ref c8cv::cartToPolar is used internally thus angles
are measured from 0 to 360 with accuracy about 0.3 degrees.
*/
void logPolar(c8cv::InputArray src, c8cv::OutputArray dst, c8cv::Point2f center, double M, int flags);

/** @brief Remaps an image to polar coordinates space.

@anchor polar_remaps_reference_image
![Polar remaps reference](pics/polar_remap_doc.png)

Transform the source image using the following transformation:
\f[\begin{array}{l}
  dst( \rho , \phi ) = src(x,y) \\
  dst.size() \leftarrow src.size()
\end{array}\f]

where
\f[\begin{array}{l}
  I = (dx,dy) = (x - center.x,y - center.y) \\
  \rho = Kx \cdot \texttt{magnitude} (I) ,\\
  \phi = Ky \cdot \texttt{angle} (I)_{0..360 deg}
\end{array}\f]

and
\f[\begin{array}{l}
  Kx = src.cols / maxRadius \\
  Ky = src.rows / 360
\end{array}\f]


@param src Source image
@param dst Destination image. It will have same size and type as src.
@param center The transformation center;
@param maxRadius The radius of the bounding circle to transform. It determines the inverse magnitude
scale parameter too.
@param flags A combination of interpolation methods, see c8cv::InterpolationFlags

@note
-   The function can not operate in-place.
-   To calculate magnitude and angle in degrees @ref c8cv::cartToPolar is used internally thus angles
are measured from 0 to 360 with accuracy about 0.3 degrees.

*/
void linearPolar(
  c8cv::InputArray src, c8cv::OutputArray dst, c8cv::Point2f center, double maxRadius, int flags);

//! @} imgproc_transform

}  // namespace c8cv
