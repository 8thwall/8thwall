// Copyright (c) 2019 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "c8/geometry/parameterized-geometry.h"
#include "c8/hmatrix.h"

namespace c8 {
constexpr float ROI_ASPECT = 0.75f;

struct ImageRoi {
  enum class Source {
    UNSPECIFIED_SOURCE = 0,
    GRAVITY = 1,
    IMAGE_TARGET = 2,
    HIRES_SCAN = 3,
    FACE = 4,
    HAND = 5,
    CURVY_IMAGE_TARGET = 6,
    EAR_LEFT = 7,
    EAR_RIGHT = 8,
  };

  Source source = Source::UNSPECIFIED_SOURCE;
  int faceId = -1;
  String name;
  HMatrix warp = HMatrixGen::i();

  // Used instead of warp when source is CURVY_IMAGE_TARGET
  CurvyImageGeometry geom;
  HMatrix intrinsics = HMatrixGen::i();
  // Location of the camera with respect to the image target 3d model
  HMatrix globalPose = HMatrixGen::i();
};

}  // namespace c8
