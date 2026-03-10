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

//! type of the threshold operation
//! ![threshold types](pics/threshold.png)
enum ThresholdTypes {
  THRESH_BINARY = 0,  //!< \f[\texttt{dst} (x,y) =  \fork{\texttt{maxval}}{if \(\texttt{src}(x,y) >
                      //!\texttt{thresh}\)}{0}{otherwise}\f]
  THRESH_BINARY_INV = 1,  //!< \f[\texttt{dst} (x,y) =  \fork{0}{if \(\texttt{src}(x,y) >
                          //!\texttt{thresh}\)}{\texttt{maxval}}{otherwise}\f]
  THRESH_TRUNC = 2,   //!< \f[\texttt{dst} (x,y) =  \fork{\texttt{threshold}}{if \(\texttt{src}(x,y)
                      //!> \texttt{thresh}\)}{\texttt{src}(x,y)}{otherwise}\f]
  THRESH_TOZERO = 3,  //!< \f[\texttt{dst} (x,y) =  \fork{\texttt{src}(x,y)}{if \(\texttt{src}(x,y)
                      //!> \texttt{thresh}\)}{0}{otherwise}\f]
  THRESH_TOZERO_INV = 4,  //!< \f[\texttt{dst} (x,y) =  \fork{0}{if \(\texttt{src}(x,y) >
                          //!\texttt{thresh}\)}{\texttt{src}(x,y)}{otherwise}\f]
  THRESH_MASK = 7,
  THRESH_OTSU = 8,      //!< flag, use Otsu algorithm to choose the optimal threshold value
  THRESH_TRIANGLE = 16  //!< flag, use Triangle algorithm to choose the optimal threshold value
};

//! adaptive threshold algorithm
//! see c8cv::adaptiveThreshold
enum AdaptiveThresholdTypes {
  /** the threshold value \f$T(x,y)\f$ is a mean of the \f$\texttt{blockSize} \times
  \texttt{blockSize}\f$ neighborhood of \f$(x, y)\f$ minus C */
  ADAPTIVE_THRESH_MEAN_C = 0,
  /** the threshold value \f$T(x, y)\f$ is a weighted sum (cross-correlation with a Gaussian
  window) of the \f$\texttt{blockSize} \times \texttt{blockSize}\f$ neighborhood of \f$(x, y)\f$
  minus C . The default sigma (standard deviation) is used for the specified blockSize . See
  c8cv::getGaussianKernel*/
  ADAPTIVE_THRESH_GAUSSIAN_C = 1
};

//! @addtogroup imgproc_misc
//! @{

/** @brief Applies a fixed-level threshold to each array element.

The function applies fixed-level thresholding to a single-channel array. The function is typically
used to get a bi-level (binary) image out of a grayscale image ( c8cv::compare could be also used for
this purpose) or for removing a noise, that is, filtering out pixels with too small or too large
values. There are several types of thresholding supported by the function. They are determined by
type parameter.

Also, the special values c8cv::THRESH_OTSU or c8cv::THRESH_TRIANGLE may be combined with one of the
above values. In these cases, the function determines the optimal threshold value using the Otsu's
or Triangle algorithm and uses it instead of the specified thresh . The function returns the
computed threshold value. Currently, the Otsu's and Triangle methods are implemented only for 8-bit
images.

@param src input array (single-channel, 8-bit or 32-bit floating point).
@param dst output array of the same size and type as src.
@param thresh threshold value.
@param maxval maximum value to use with the THRESH_BINARY and THRESH_BINARY_INV thresholding
types.
@param type thresholding type (see the c8cv::ThresholdTypes).

@sa  adaptiveThreshold, findContours, compare, min, max
 */
double threshold(c8cv::InputArray src, c8cv::OutputArray dst, double thresh, double maxval, int type);

/** @brief Applies an adaptive threshold to an array.

The function transforms a grayscale image to a binary image according to the formulae:
-   **THRESH_BINARY**
    \f[dst(x,y) =  \fork{\texttt{maxValue}}{if \(src(x,y) > T(x,y)\)}{0}{otherwise}\f]
-   **THRESH_BINARY_INV**
    \f[dst(x,y) =  \fork{0}{if \(src(x,y) > T(x,y)\)}{\texttt{maxValue}}{otherwise}\f]
where \f$T(x,y)\f$ is a threshold calculated individually for each pixel (see adaptiveMethod
parameter).

The function can process the image in-place.

@param src Source 8-bit single-channel image.
@param dst Destination image of the same size and the same type as src.
@param maxValue Non-zero value assigned to the pixels for which the condition is satisfied
@param adaptiveMethod Adaptive thresholding algorithm to use, see c8cv::AdaptiveThresholdTypes
@param thresholdType Thresholding type that must be either THRESH_BINARY or THRESH_BINARY_INV,
see c8cv::ThresholdTypes.
@param blockSize Size of a pixel neighborhood that is used to calculate a threshold value for the
pixel: 3, 5, 7, and so on.
@param C Constant subtracted from the mean or weighted mean (see the details below). Normally, it
is positive but may be zero or negative as well.

@sa  threshold, blur, GaussianBlur
 */
void adaptiveThreshold(
  c8cv::InputArray src,
  c8cv::OutputArray dst,
  double maxValue,
  int adaptiveMethod,
  int thresholdType,
  int blockSize,
  double C);

//! @} imgproc_misc

}  // namespace c8cv
