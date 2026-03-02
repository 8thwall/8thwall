// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "c8/geometry/parameterized-geometry.h"
#include "c8/hmatrix.h"
#include "c8/hpoint.h"
#include "c8/quaternion.h"
#include "c8/vector.h"

namespace c8 {

HMatrix rotationToHorizontal(Quaternion xrPose);
HMatrix rotationToVertical(Quaternion xrPose);

c8_PixelPinholeCameraModel glPinholeCameraModel(c8_PixelPinholeCameraModel k);

HMatrix glRotationHomography(c8_PixelPinholeCameraModel k, const HMatrix &rotationMat);

HMatrix glUnscaledHorizontalWarp(c8_PixelPinholeCameraModel k, Quaternion xrPose);
HMatrix glUnscaledVerticalWarp(c8_PixelPinholeCameraModel k, Quaternion xrPose);

HMatrix glScaledHorizontalWarp(c8_PixelPinholeCameraModel k, Quaternion xrPose);
HMatrix glScaledVerticalWarp(c8_PixelPinholeCameraModel k, Quaternion xrPose);

HMatrix glImageTargetWarp(
  c8_PixelPinholeCameraModel targetK,
  c8_PixelPinholeCameraModel camK,
  const HMatrix &camMotion,
  const PlanarImageGeometry &geom = PlanarImageGeometry{});
HMatrix glImageCurvyWarp(c8_PixelPinholeCameraModel camK, const HMatrix &camMotion);
HMatrix twoDProjectionMat(const HMatrix &src);

}  // namespace c8
