// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "third_party/cvlite/core/core.hpp"

namespace c8cv {

//! type of the robust estimation algorithm
enum {
  LMEDS = 4,   //!< least-median algorithm
  RANSAC = 8,  //!< RANSAC algorithm
  RHO = 16     //!< RHO algorithm
};

class CV_EXPORTS PointSetRegistrator : public Algorithm {
public:
  class CV_EXPORTS Callback {
  public:
    virtual ~Callback() {}
    virtual int runKernel(InputArray m1, InputArray m2, OutputArray model) const = 0;
    virtual void computeError(
      InputArray m1, InputArray m2, InputArray model, OutputArray err) const = 0;
    virtual bool checkSubset(InputArray, InputArray, int) const { return true; }
  };

  virtual void setCallback(const Ptr<PointSetRegistrator::Callback> &cb) = 0;
  virtual bool run(InputArray m1, InputArray m2, OutputArray model, OutputArray mask) const = 0;
};

int RANSACUpdateNumIters(double p, double ep, int modelPoints, int maxIters);

CV_EXPORTS Ptr<PointSetRegistrator> createRANSACPointSetRegistrator(
  const Ptr<PointSetRegistrator::Callback> &cb,
  int modelPoints,
  double threshold,
  double confidence = 0.99,
  int maxIters = 1000);

CV_EXPORTS Ptr<PointSetRegistrator> createLMeDSPointSetRegistrator(
  const Ptr<PointSetRegistrator::Callback> &cb,
  int modelPoints,
  double confidence = 0.99,
  int maxIters = 1000);

/** @brief Computes an optimal affine transformation between two 3D point sets.

@param src First input 3D point set.
@param dst Second input 3D point set.
@param out Output 3D affine transformation matrix \f$3 \times 4\f$ .
@param inliers Output vector indicating which points are inliers.
@param ransacThreshold Maximum reprojection error in the RANSAC algorithm to consider a point as
an inlier.
@param confidence Confidence level, between 0 and 1, for the estimated transformation. Anything
between 0.95 and 0.99 is usually good enough. Values too close to 1 can slow down the estimation
significantly. Values lower than 0.8-0.9 can result in an incorrectly estimated transformation.

The function estimates an optimal 3D affine transformation between two 3D point sets using the
RANSAC algorithm.
 */
CV_EXPORTS_W int estimateAffine3D(
  InputArray src,
  InputArray dst,
  OutputArray out,
  OutputArray inliers,
  double ransacThreshold = 3,
  double confidence = 0.99);

/** @brief Computes an optimal affine transformation between two 2D point sets.

@param from First input 2D point set.
@param to Second input 2D point set.
@param inliers Output vector indicating which points are inliers.
@param method Robust method used to compute tranformation. The following methods are possible:
-   RANSAC - RANSAC-based robust method
-   LMEDS - Least-Median robust method
RANSAC is the default method.
@param ransacReprojThreshold Maximum reprojection error in the RANSAC algorithm to consider
a point as an inlier. Applies only to RANSAC.
@param maxIters The maximum number of robust method iterations, 2000 is the maximum it can be.
@param confidence Confidence level, between 0 and 1, for the estimated transformation. Anything
between 0.95 and 0.99 is usually good enough. Values too close to 1 can slow down the estimation
significantly. Values lower than 0.8-0.9 can result in an incorrectly estimated transformation.
@param refineIters Maximum number of iterations of refining algorithm (Levenberg-Marquardt).
Passing 0 will disable refining, so the output matrix will be output of robust method.

@return Output 2D affine transformation matrix \f$2 \times 3\f$ or empty matrix if transformation
could not be estimated.

The function estimates an optimal 2D affine transformation between two 2D point sets using the
selected robust algorithm.

The computed transformation is then refined further (using only inliers) with the
Levenberg-Marquardt method to reduce the re-projection error even more.

@note
The RANSAC method can handle practically any ratio of outliers but need a threshold to
distinguish inliers from outliers. The method LMeDS does not need any threshold but it works
correctly only when there are more than 50% of inliers.

@sa estimateAffinePartial2D, getAffineTransform
*/
CV_EXPORTS_W Mat estimateAffine2D(
  InputArray from,
  InputArray to,
  OutputArray inliers = noArray(),
  int method = RANSAC,
  double ransacReprojThreshold = 3,
  size_t maxIters = 2000,
  double confidence = 0.99,
  size_t refineIters = 10);

/** @brief Computes an optimal limited affine transformation with 4 degrees of freedom between
two 2D point sets.

@param from First input 2D point set.
@param to Second input 2D point set.
@param inliers Output vector indicating which points are inliers.
@param method Robust method used to compute tranformation. The following methods are possible:
-   RANSAC - RANSAC-based robust method
-   LMEDS - Least-Median robust method
RANSAC is the default method.
@param ransacReprojThreshold Maximum reprojection error in the RANSAC algorithm to consider
a point as an inlier. Applies only to RANSAC.
@param maxIters The maximum number of robust method iterations, 2000 is the maximum it can be.
@param confidence Confidence level, between 0 and 1, for the estimated transformation. Anything
between 0.95 and 0.99 is usually good enough. Values too close to 1 can slow down the estimation
significantly. Values lower than 0.8-0.9 can result in an incorrectly estimated transformation.
@param refineIters Maximum number of iterations of refining algorithm (Levenberg-Marquardt).
Passing 0 will disable refining, so the output matrix will be output of robust method.

@return Output 2D affine transformation (4 degrees of freedom) matrix \f$2 \times 3\f$ or
empty matrix if transformation could not be estimated.

The function estimates an optimal 2D affine transformation with 4 degrees of freedom limited to
combinations of translation, rotation, and uniform scaling. Uses the selected algorithm for robust
estimation.

The computed transformation is then refined further (using only inliers) with the
Levenberg-Marquardt method to reduce the re-projection error even more.

Estimated transformation matrix is:
\f[ \begin{bmatrix} \cos(\theta)s & -\sin(\theta)s & tx \\
                \sin(\theta)s & \cos(\theta)s & ty
\end{bmatrix} \f]
Where \f$ \theta \f$ is the rotation angle, \f$ s \f$ the scaling factor and \f$ tx, ty \f$ are
translations in \f$ x, y \f$ axes respectively.

@note
The RANSAC method can handle practically any ratio of outliers but need a threshold to
distinguish inliers from outliers. The method LMeDS does not need any threshold but it works
correctly only when there are more than 50% of inliers.

@sa estimateAffine2D, getAffineTransform
*/
CV_EXPORTS_W Mat estimateAffinePartial2D(
  InputArray from,
  InputArray to,
  OutputArray inliers = noArray(),
  int method = RANSAC,
  double ransacReprojThreshold = 3,
  size_t maxIters = 2000,
  double confidence = 0.99,
  size_t refineIters = 10);

}  // namespace c8cv
