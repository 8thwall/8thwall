// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "c8/vector.h"
#include "reality/engine/features/frame-point.h"

namespace c8 {

// Threshold for the hamming distance between points in a match.
constexpr int POINT_MATCHES_DIST_TH_LOCAL = 64;
constexpr int POINT_MATCHES_DIST_TH_GLOBAL = 30;
constexpr float POINT_MATCHES_GAUSSIAN_FILTER_DIST_TH_LOCAL = 5.0;
constexpr float POINT_MATCHES_GAUSSIAN_FILTER_DIST_TH_GLOBAL = 3.0;

// Filter point matches that are geometric outliers. This method builds a 2D gaussian motion model
// from all matches, and then removes points that are outliers according to those matches.
// NOTE(nb): this was early exploratory code and has not been used in production.
void filterMatches(
  const Vector<PointMatch> &in,
  const FrameWithPoints &first,
  const FrameWithPoints &second,
  int hammingDistThresh,
  float gaussFilterThresh,
  Vector<PointMatch> *out);

}  // namespace c8
