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

/** @addtogroup imgproc
@{
*/

//! @addtogroup imgproc_filter
//! @{

//! type of morphological operation
enum MorphTypes {
  MORPH_ERODE = 0,     //!< see c8cv::erode
  MORPH_DILATE = 1,    //!< see c8cv::dilate
  MORPH_OPEN = 2,      //!< an opening operation
                       //!< \f[\texttt{dst} = \mathrm{open} ( \texttt{src} , \texttt{element} )=
                       //!\mathrm{dilate} ( \mathrm{erode} ( \texttt{src} , \texttt{element} ))\f]
  MORPH_CLOSE = 3,     //!< a closing operation
                       //!< \f[\texttt{dst} = \mathrm{close} ( \texttt{src} , \texttt{element} )=
                       //!\mathrm{erode} ( \mathrm{dilate} ( \texttt{src} , \texttt{element} ))\f]
  MORPH_GRADIENT = 4,  //!< a morphological gradient
                       //!< \f[\texttt{dst} = \mathrm{morph\_grad} ( \texttt{src} , \texttt{element}
                       //!)= \mathrm{dilate} ( \texttt{src} , \texttt{element} )- \mathrm{erode} (
                       //!\texttt{src} , \texttt{element} )\f]
  MORPH_TOPHAT = 5,    //!< "top hat"
                       //!< \f[\texttt{dst} = \mathrm{tophat} ( \texttt{src} , \texttt{element} )=
                       //!\texttt{src} - \mathrm{open} ( \texttt{src} , \texttt{element} )\f]
  MORPH_BLACKHAT = 6,  //!< "black hat"
                       //!< \f[\texttt{dst} = \mathrm{blackhat} ( \texttt{src} , \texttt{element} )=
                       //!\mathrm{close} ( \texttt{src} , \texttt{element} )- \texttt{src}\f]
  MORPH_HITMISS =
    7  //!< "hit and miss"
       //!<   .- Only supported for CV_8UC1 binary images. Tutorial can be found in [this
       //! page](https://web.archive.org/web/20160316070407/http://opencv-code.com/tutorials/hit-or-miss-transform-in-opencv/)
};

//! shape of the structuring element
enum MorphShapes {
  MORPH_RECT = 0,  //!< a rectangular structuring element:  \f[E_{ij}=1\f]
  MORPH_CROSS =
    1,  //!< a cross-shaped structuring element:
        //!< \f[E_{ij} =  \fork{1}{if i=\texttt{anchor.y} or j=\texttt{anchor.x}}{0}{otherwise}\f]
  MORPH_ELLIPSE = 2  //!< an elliptic structuring element, that is, a filled ellipse inscribed
                     //!< into the rectangle Rect(0, 0, esize.width, 0.esize.height)
};

//! @} imgproc_filter

//! @addtogroup imgproc_filter
//! @{

/** @brief Convolves an image with the kernel.

The function applies an arbitrary linear filter to an image. In-place operation is supported. When
the aperture is partially outside the image, the function interpolates outlier pixel values
according to the specified border mode.

The function does actually compute correlation, not the convolution:

\f[\texttt{dst} (x,y) =  \sum _{ \stackrel{0\leq x' < \texttt{kernel.cols},}{0\leq y' <
\texttt{kernel.rows}} }  \texttt{kernel} (x',y')* \texttt{src} (x+x'- \texttt{anchor.x} ,y+y'-
\texttt{anchor.y} )\f]

That is, the kernel is not mirrored around the anchor point. If you need a real convolution, flip
the kernel using c8cv::flip and set the new anchor to `(kernel.cols - anchor.x - 1, kernel.rows -
anchor.y - 1)`.

The function uses the DFT-based algorithm in case of sufficiently large kernels (~`11 x 11` or
larger) and the direct algorithm for small kernels.

@param src input image.
@param dst output image of the same size and the same number of channels as src.
@param ddepth desired depth of the destination image, see @ref filter_depths "combinations"
@param kernel convolution kernel (or rather a correlation kernel), a single-channel floating point
matrix; if you want to apply different kernels to different channels, split the image into
separate color planes using split and process them individually.
@param anchor anchor of the kernel that indicates the relative position of a filtered point within
the kernel; the anchor should lie within the kernel; default value (-1,-1) means that the anchor
is at the kernel center.
@param delta optional value added to the filtered pixels before storing them in dst.
@param borderType pixel extrapolation method, see c8cv::BorderTypes
@sa  sepFilter2D, dft, matchTemplate
 */
void filter2D(
  c8cv::InputArray src,
  c8cv::OutputArray dst,
  int ddepth,
  c8cv::InputArray kernel,
  c8cv::Point anchor = c8cv::Point(-1, -1),
  double delta = 0,
  int borderType = c8cv::BORDER_DEFAULT);

/** @brief Applies a separable linear filter to an image.

The function applies a separable linear filter to the image. That is, first, every row of src is
filtered with the 1D kernel kernelX. Then, every column of the result is filtered with the 1D
kernel kernelY. The final result shifted by delta is stored in dst .

@param src Source image.
@param dst Destination image of the same size and the same number of channels as src .
@param ddepth Destination image depth, see @ref filter_depths "combinations"
@param kernelX Coefficients for filtering each row.
@param kernelY Coefficients for filtering each column.
@param anchor Anchor position within the kernel. The default value \f$(-1,-1)\f$ means that the
anchor is at the kernel center.
@param delta Value added to the filtered results before storing them.
@param borderType Pixel extrapolation method, see c8cv::BorderTypes
@sa  filter2D, Sobel, GaussianBlur, boxFilter, blur
 */
void sepFilter2D(
  c8cv::InputArray src,
  c8cv::OutputArray dst,
  int ddepth,
  c8cv::InputArray kernelX,
  c8cv::InputArray kernelY,
  c8cv::Point anchor = c8cv::Point(-1, -1),
  double delta = 0,
  int borderType = c8cv::BORDER_DEFAULT);

}  // namespace c8cv
