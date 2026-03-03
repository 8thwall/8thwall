// Copyright (c) 2019 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include <utility>

#include "c8/geometry/parameterized-geometry.h"
#include "c8/hmatrix.h"
#include "c8/hpoint.h"
#include "c8/vector.h"
#include "reality/engine/features/frame-point.h"
#include "reality/engine/features/image-point.h"
#include "reality/engine/imagedetection/detection-image.h"

using namespace c8;

namespace c8 {

FrameWithPoints projectToTargetSpace(
  const DetectionImage &im, const FrameWithPoints &points, const HMatrix &pose);

FrameWithPoints projectSearchRayToTargetRay(
  const DetectionImage &im,
  const FrameWithPoints &points,
  const HMatrix &pose,
  const CurvyImageGeometry &geom);

void getPointsAndRays(
  const Vector<PointMatch> &matches,
  const DetectionImage &target,
  const FrameWithPoints &inputPoints,
  Vector<HPoint3> *worldPts,
  Vector<HPoint2> *camRays);

void getPointsOnGeometry(
  const Vector<PointMatch> &matches,
  const Vector<HPoint2> &targetPointsInPixel,
  const CurvyImageGeometry &geom,
  Vector<HPoint3> *worldPts);

void getRainbowPointsOnGeometry(
  const Vector<PointMatch> &matches,
  const Vector<HPoint2> &targetPointsInPixel,
  const CurvyImageGeometry &geom,
  const RainbowMetadata &rainbowMetadata,
  Vector<HPoint3> *worldPts);

void getMatchedCamRays(
  const Vector<PointMatch> &matches, const FrameWithPoints &inputPoints, Vector<HPoint2> *camRays);

void getMatchedRays(
  const Vector<PointMatch> &matches,
  const DetectionImage &target,
  const FrameWithPoints &inputPoints,
  Vector<HPoint2> *imTargetRays,
  Vector<HPoint2> *camRays,
  Vector<float> *weights);

}  // namespace c8
