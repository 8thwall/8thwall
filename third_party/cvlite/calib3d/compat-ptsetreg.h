// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "third_party/cvlite/core/core.hpp"
#include "third_party/cvlite/core/core_c.h"

namespace c8cv {

/* Calculates fundamental matrix given a set of corresponding points */
#define CV_FM_7POINT 1
#define CV_FM_8POINT 2

#define CV_LMEDS 4
#define CV_RANSAC 8

#define CV_FM_LMEDS_ONLY CV_LMEDS
#define CV_FM_RANSAC_ONLY CV_RANSAC
#define CV_FM_LMEDS CV_LMEDS
#define CV_FM_RANSAC CV_RANSAC

class CV_EXPORTS CvLevMarq {
public:
  CvLevMarq();
  CvLevMarq(
    int nparams,
    int nerrs,
    CvTermCriteria criteria = cvTermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, DBL_EPSILON),
    bool completeSymmFlag = false);
  ~CvLevMarq();
  void init(
    int nparams,
    int nerrs,
    CvTermCriteria criteria = cvTermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, DBL_EPSILON),
    bool completeSymmFlag = false);
  bool update(const CvMat *&param, CvMat *&J, CvMat *&err);
  bool updateAlt(const CvMat *&param, CvMat *&JtJ, CvMat *&JtErr, double *&errNorm);

  void clear();
  void step();
  enum { DONE = 0, STARTED = 1, CALC_J = 2, CHECK_ERR = 3 };

  Ptr<CvMat> mask;
  Ptr<CvMat> prevParam;
  Ptr<CvMat> param;
  Ptr<CvMat> J;
  Ptr<CvMat> err;
  Ptr<CvMat> JtJ;
  Ptr<CvMat> JtJN;
  Ptr<CvMat> JtErr;
  Ptr<CvMat> JtJV;
  Ptr<CvMat> JtJW;
  double prevErrNorm, errNorm;
  int lambdaLg10;
  CvTermCriteria criteria;
  int state;
  int iters;
  bool completeSymmFlag;
  int solveMethod;
};

/* updates the number of RANSAC iterations */
CVAPI(int) cvRANSACUpdateNumIters(double p, double err_prob, int model_points, int max_iters);

/* Finds perspective transformation between the object plane and image (view) plane */
CVAPI(int)
cvFindHomography(
  const CvMat *src_points,
  const CvMat *dst_points,
  CvMat *homography,
  int method CV_DEFAULT(0),
  double ransacReprojThreshold CV_DEFAULT(3),
  CvMat *mask CV_DEFAULT(0),
  int maxIters CV_DEFAULT(2000),
  double confidence CV_DEFAULT(0.995));

CVAPI(int)
cvFindFundamentalMat(
  const CvMat *points1,
  const CvMat *points2,
  CvMat *fundamental_matrix,
  int method CV_DEFAULT(CV_FM_RANSAC),
  double param1 CV_DEFAULT(3.),
  double param2 CV_DEFAULT(0.99),
  CvMat *status CV_DEFAULT(NULL));

/* For each input point on one of images
   computes parameters of the corresponding
   epipolar line on the other image */
CVAPI(void)
cvComputeCorrespondEpilines(
  const CvMat *points,
  int which_image,
  const CvMat *fundamental_matrix,
  CvMat *correspondent_lines);

CVAPI(void) cvConvertPointsHomogeneous(const CvMat *src, CvMat *dst);

}  // namespace c8cv
