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

//! @addtogroup imgproc_filter
//! @{

/** @brief Returns Gaussian filter coefficients.

The function computes and returns the \f$\texttt{ksize} \times 1\f$ matrix of Gaussian filter
coefficients:

\f[G_i= \alpha *e^{-(i-( \texttt{ksize} -1)/2)^2/(2* \texttt{sigma}^2)},\f]

where \f$i=0..\texttt{ksize}-1\f$ and \f$\alpha\f$ is the scale factor chosen so that \f$\sum_i
G_i=1\f$.

Two of such generated kernels can be passed to sepFilter2D. Those functions automatically recognize
smoothing kernels (a symmetrical kernel with sum of weights equal to 1) and handle them accordingly.
You may also use the higher-level GaussianBlur.
@param ksize Aperture size. It should be odd ( \f$\texttt{ksize} \mod 2 = 1\f$ ) and positive.
@param sigma Gaussian standard deviation. If it is non-positive, it is computed from ksize as
`sigma = 0.3\*((ksize-1)\*0.5 - 1) + 0.8`.
@param ktype Type of filter coefficients. It can be CV_32F or CV_64F .
@sa  sepFilter2D, getDerivKernels, getStructuringElement, GaussianBlur
 */
c8cv::Mat getGaussianKernel(int ksize, double sigma, int ktype = CV_64F);

/** @brief Blurs an image using the median filter.

The function smoothes an image using the median filter with the \f$\texttt{ksize} \times
\texttt{ksize}\f$ aperture. Each channel of a multi-channel image is processed independently.
In-place operation is supported.

@note The median filter uses BORDER_REPLICATE internally to cope with border pixels, see
c8cv::BorderTypes

@param src input 1-, 3-, or 4-channel image; when ksize is 3 or 5, the image depth should be
CV_8U, CV_16U, or CV_32F, for larger aperture sizes, it can only be CV_8U.
@param dst destination array of the same size and type as src.
@param ksize aperture linear size; it must be odd and greater than 1, for example: 3, 5, 7 ...
@sa  bilateralFilter, blur, boxFilter, GaussianBlur
 */
void medianBlur(c8cv::InputArray src, c8cv::OutputArray dst, int ksize);

/** @brief Blurs an image using a Gaussian filter.

The function convolves the source image with the specified Gaussian kernel. In-place filtering is
supported.

@param src input image; the image can have any number of channels, which are processed
independently, but the depth should be CV_8U, CV_16U, CV_16S, CV_32F or CV_64F.
@param dst output image of the same size and type as src.
@param ksize Gaussian kernel size. ksize.width and ksize.height can differ but they both must be
positive and odd. Or, they can be zero's and then they are computed from sigma.
@param sigmaX Gaussian kernel standard deviation in X direction.
@param sigmaY Gaussian kernel standard deviation in Y direction; if sigmaY is zero, it is set to be
equal to sigmaX, if both sigmas are zeros, they are computed from ksize.width and ksize.height,
respectively (see c8cv::getGaussianKernel for details); to fully control the result regardless of
possible future modifications of all this semantics, it is recommended to specify all of ksize,
sigmaX, and sigmaY.
@param borderType pixel extrapolation method, see c8cv::BorderTypes

@sa  sepFilter2D, filter2D, blur, boxFilter, bilateralFilter, medianBlur
 */
void GaussianBlur(
  c8cv::InputArray src,
  c8cv::OutputArray dst,
  c8cv::Size ksize,
  double sigmaX,
  double sigmaY = 0,
  int borderType = c8cv::BORDER_DEFAULT);

/** @brief Applies the bilateral filter to an image.

The function applies bilateral filtering to the input image, as described in
http://www.dai.ed.ac.uk/CVonline/LOCAL_COPIES/MANDUCHI1/Bilateral_Filtering.html
bilateralFilter can reduce unwanted noise very well while keeping edges fairly sharp. However, it is
very slow compared to most filters.

_Sigma values_: For simplicity, you can set the 2 sigma values to be the same. If they are small (\<
10), the filter will not have much effect, whereas if they are large (\> 150), they will have a very
strong effect, making the image look "cartoonish".

_Filter size_: Large filters (d \> 5) are very slow, so it is recommended to use d=5 for real-time
applications, and perhaps d=9 for offline applications that need heavy noise filtering.

This filter does not work inplace.
@param src Source 8-bit or floating-point, 1-channel or 3-channel image.
@param dst Destination image of the same size and type as src .
@param d Diameter of each pixel neighborhood that is used during filtering. If it is non-positive,
it is computed from sigmaSpace.
@param sigmaColor Filter sigma in the color space. A larger value of the parameter means that
farther colors within the pixel neighborhood (see sigmaSpace) will be mixed together, resulting
in larger areas of semi-equal color.
@param sigmaSpace Filter sigma in the coordinate space. A larger value of the parameter means that
farther pixels will influence each other as long as their colors are close enough (see sigmaColor
). When d\>0, it specifies the neighborhood size regardless of sigmaSpace. Otherwise, d is
proportional to sigmaSpace.
@param borderType border mode used to extrapolate pixels outside of the image, see c8cv::BorderTypes
 */
void bilateralFilter(
  c8cv::InputArray src,
  c8cv::OutputArray dst,
  int d,
  double sigmaColor,
  double sigmaSpace,
  int borderType = c8cv::BORDER_DEFAULT);

/** @brief Blurs an image using the box filter.

The function smoothes an image using the kernel:

\f[\texttt{K} =  \alpha \begin{bmatrix} 1 & 1 & 1 &  \cdots & 1 & 1  \\ 1 & 1 & 1 &  \cdots & 1 & 1
\\ \hdotsfor{6} \\ 1 & 1 & 1 &  \cdots & 1 & 1 \end{bmatrix}\f]

where

\f[\alpha = \fork{\frac{1}{\texttt{ksize.width*ksize.height}}}{when
\texttt{normalize=true}}{1}{otherwise}\f]

Unnormalized box filter is useful for computing various integral characteristics over each pixel
neighborhood, such as covariance matrices of image derivatives (used in dense optical flow
algorithms, and so on). If you need to compute pixel sums over variable-size windows, use
c8cv::integral.

@param src input image.
@param dst output image of the same size and type as src.
@param ddepth the output image depth (-1 to use src.depth()).
@param ksize blurring kernel size.
@param anchor anchor point; default value c8cv::Point(-1,-1) means that the anchor is at the kernel
center.
@param normalize flag, specifying whether the kernel is normalized by its area or not.
@param borderType border mode used to extrapolate pixels outside of the image, see c8cv::BorderTypes
@sa  blur, bilateralFilter, GaussianBlur, medianBlur, integral
 */
void boxFilter(
  c8cv::InputArray src,
  c8cv::OutputArray dst,
  int ddepth,
  c8cv::Size ksize,
  c8cv::Point anchor = c8cv::Point(-1, -1),
  bool normalize = true,
  int borderType = c8cv::BORDER_DEFAULT);

/** @brief Calculates the normalized sum of squares of the pixel values overlapping the filter.

For every pixel \f$ (x, y) \f$ in the source image, the function calculates the sum of squares of
those neighboring pixel values which overlap the filter placed over the pixel \f$ (x, y) \f$.

The unnormalized square box filter can be useful in computing local image statistics such as the the
local variance and standard deviation around the neighborhood of a pixel.

@param _src input image
@param _dst output image of the same size and type as _src
@param ddepth the output image depth (-1 to use src.depth())
@param ksize kernel size
@param anchor kernel anchor point. The default value of c8cv::Point(-1, -1) denotes that the anchor is
at the kernel center.
@param normalize flag, specifying whether the kernel is to be normalized by it's area or not.
@param borderType border mode used to extrapolate pixels outside of the image, see c8cv::BorderTypes
@sa boxFilter
*/
void sqrBoxFilter(
  c8cv::InputArray _src,
  c8cv::OutputArray _dst,
  int ddepth,
  c8cv::Size ksize,
  c8cv::Point anchor = c8cv::Point(-1, -1),
  bool normalize = true,
  int borderType = c8cv::BORDER_DEFAULT);

/** @brief Blurs an image using the normalized box filter.

The function smoothes an image using the kernel:

\f[\texttt{K} =  \frac{1}{\texttt{ksize.width*ksize.height}} \begin{bmatrix} 1 & 1 & 1 &  \cdots & 1
& 1  \\ 1 & 1 & 1 &  \cdots & 1 & 1  \\ \hdotsfor{6} \\ 1 & 1 & 1 &  \cdots & 1 & 1  \\
\end{bmatrix}\f]

The call `blur(src, dst, ksize, anchor, borderType)` is equivalent to `boxFilter(src, dst,
src.type(), anchor, true, borderType)`.

@param src input image; it can have any number of channels, which are processed independently, but
the depth should be CV_8U, CV_16U, CV_16S, CV_32F or CV_64F.
@param dst output image of the same size and type as src.
@param ksize blurring kernel size.
@param anchor anchor point; default value c8cv::Point(-1,-1) means that the anchor is at the kernel
center.
@param borderType border mode used to extrapolate pixels outside of the image, see c8cv::BorderTypes
@sa  boxFilter, bilateralFilter, GaussianBlur, medianBlur
 */
void blur(
  c8cv::InputArray src,
  c8cv::OutputArray dst,
  c8cv::Size ksize,
  c8cv::Point anchor = c8cv::Point(-1, -1),
  int borderType = c8cv::BORDER_DEFAULT);

}  // namespace c8cv
