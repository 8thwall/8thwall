// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include <cfloat>

#include "c8/hmatrix.h"
#include "c8/map.h"
#include "c8/vector.h"

namespace c8 {

// Computes the Essential Matrix for two cameras that describe epipolar lines between the cameras,
// i.e. E such that x'Ex = 0 where x' is a point viewed in camera 2 (ray space) and x is the same
// point viewed in camera 1 (ray space). Note that in the case of pure rotation, the Essential
// Matrix is 0 and can't be used for computing epiplar lines.
HMatrix essentialMatrixForCameras(const HMatrix &camPos1, const HMatrix &camPos2);

// Computes the projected distance (in ray space) from ptCam2 to the epipolar line in camera 2
// induced by the same point in camera 1 and the essentialMat. This method only makes sense for
// non-zero translation (i.e. not pure rotation). For essential matrices induced by a pure rotation,
// it will return FLT_MAX for all pairs of point input. In these cases, homography should be used to
// test the goodness of fit instead.
inline float distanceFromEpipolar(const HMatrix &essentialMat, HPoint2 ptCam1, HPoint2 ptCam2) {
  // line ax + by + c = 0, where a = l.x(), b = l.y(), c = l.z()
  auto l = essentialMat * ptCam1.extrude();
  auto a = l.x();
  auto b = l.y();
  auto c = l.z();
  auto len = a * a + b * b;

  // The Essential matrix is 0 for pure rotations. In that case, homography should be used to test
  // the fit of points.
  if (len < 1e-9) {
    return FLT_MAX;
  }

  // TODO(nb): fast inverse sqrt.
  // https://en.wikipedia.org/wiki/Fast_inverse_square_root
  return std::abs(a * ptCam2.x() + b * ptCam2.y() + c) / std::sqrt(len);
}

// Intermediate values for computing the depth of epipolar points across a whole image. This makes
// it faster to compare multiple points for the same pair of images.
struct EpipolarDepthImagePrework {
  float nearDist = 0.0f;
  float viewOverlap = 0.0f;
  HMatrix projectToDictionary = HMatrixGen::i();
  HMatrix rotateToDictionary = HMatrixGen::i();
  HVector3 dictionaryClipPlaneNormal;
  bool isPureRotation = false;
};

// Intermediate values for computing the depth of a point on an epipolar line segment. This makes it
// faster to compare multiple points against the same line segment.
struct EpipolarDepthPointPrework {
  // Computes the line segment that a point in a words image lies on in a dictionary image. The
  // first end of the line segment is the location at distance infinity, and the second element is
  // the location at a distance close to the cameras. The true location of the point should lie
  // along this line segment, depending on the depth of the point. If the cameras do not share
  // sufficiently overlapping views, the returned line segment will have zero length and will be
  // located at (-float::max, -float::max).
  std::pair<HVector2, HVector2> lineSegment;
  HVector2 segmentVector;
  bool isPoint = false;
  HVector2 normSegmentVector;
  HVector3 rotatedWordsPoint;
  HVector2 dzScale;
};

// Given a point and a line segment, return the point on the line segment that is closest to the
// point.
HVector2 closestLineSegmentPoint(const EpipolarDepthPointPrework &w, HVector2 pt);

// Result of computing depth of a point on the epipolar line.
struct EpipolarDepthResult {
  enum class Status {
    OK = 0,
    TOO_UNCERTAIN = 1,
    PURE_ROTATION = 2,
    FOCUS_OF_EXPANSION = 3,
    TOO_FAR_AWAY = 4,
  };
  // Depth of the point in the words image.
  double depthInWords = 1.0;
  // A measure of how much this depth estimate can be trusted, based on how much small changes in
  // the input change the estimate of the depth.
  double certainty = 0.0;
  // Whether the depth should be considered valid. `depthInWords` and `certainty` are only valid
  // when `status` is `Status::OK`.
  Status status = Status::TOO_UNCERTAIN;
};

HVector3 groundPointTriangulationPrework(const HMatrix &extrinsic);

// Compute intermediate values for computing the depth of epipolar points across a whole image.
EpipolarDepthImagePrework epipolarDepthImagePrework(
  const HMatrix &wordsCam, const HMatrix &dictionaryCam);

// Compute intermediate values for computing the depth of a point on an epipolar line segment.
EpipolarDepthPointPrework epipolarDepthPointPrework(
  const EpipolarDepthImagePrework &w, HPoint2 wordsRay);

// Estimate the depth of a point in a words image corresponding to a point on an epipolar line
// segment in a dictionary image. The point should be computed using `closestLineSegmentPoint`.
// This method also returns a measure of certainty about the depth of the point which can be used
// for weighted averaging of results across views of the same point. If the depth of the point
// cannot be determined because the baseline is too small or the point lies along a focus of
// expansion, a non-OK `status` is returned.
EpipolarDepthResult depthOfEpipolarPoint(
  const EpipolarDepthImagePrework &iw,
  const EpipolarDepthPointPrework &pw,
  HVector2 dictionaryPt,
  uint8_t dictionaryPtScale);

// Compute a weighted average depth result which can itself be combined with other weighted results.
inline EpipolarDepthResult combineDepths(
  const EpipolarDepthResult &a, const EpipolarDepthResult &b) {
  auto newCertainty = a.certainty + b.certainty;
  bool hasData = newCertainty > 1e-10;
  return hasData
    ? EpipolarDepthResult{
      (a.depthInWords * a.certainty + b.depthInWords * b.certainty) / newCertainty,
      newCertainty,
      EpipolarDepthResult::Status::OK,
    } : EpipolarDepthResult{};
}

// Compute a weighted average depth result which can itself be combined with other weighted results.
inline EpipolarDepthResult combineDepths(const Vector<EpipolarDepthResult> &v) {
  double newDepth = 0.0;
  double newCertainty = 0.0;
  bool anyOk = false;

  for (const auto &d : v) {
    anyOk |= d.certainty > 1e-10;
    newDepth += d.depthInWords * d.certainty;
    newCertainty += d.certainty;
  }

  return anyOk
    ? EpipolarDepthResult{newDepth / newCertainty, newCertainty, EpipolarDepthResult::Status::OK}
    : EpipolarDepthResult{};
}

// Compute a weighted average depth result which can itself be combined with other weighted results.
template <typename T>
inline EpipolarDepthResult combineDepths(const TreeMap<T, EpipolarDepthResult> &m) {
  double newDepth = 0.0;
  double newCertainty = 0.0;
  bool anyOk = false;

  for (const auto &[k, d] : m) {
    anyOk |= d.certainty > 1e-10;
    newDepth += d.depthInWords * d.certainty;
    newCertainty += d.certainty;
  }

  return anyOk
    ? EpipolarDepthResult{newDepth / newCertainty, newCertainty, EpipolarDepthResult::Status::OK}
    : EpipolarDepthResult{};
}

// Remove a weighted average depth result from a combined depth result.
inline EpipolarDepthResult decombineDepths(
  const EpipolarDepthResult &a, const EpipolarDepthResult &b) {
  auto cDiff = a.certainty - b.certainty;
  auto hasData = cDiff > 1e-10;
  return hasData
    ? EpipolarDepthResult{
      (a.depthInWords * a.certainty - b.depthInWords * b.certainty) / cDiff,
      cDiff,
      a.status,
    } : EpipolarDepthResult{};
}

}  // namespace c8
