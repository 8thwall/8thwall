// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "c8/hmatrix.h"
#include "c8/hvector.h"
#include "c8/quaternion.h"

namespace c8 {

// Extract just the rotational component of a TR matrix.
HMatrix rotationMat(const HMatrix &cam);
Quaternion rotation(const HMatrix &cam);

// Extract just the translational component of a TR matrix.
HMatrix translationMat(const HMatrix &cam);
HVector3 translation(const HMatrix &cam);

// Extract just the scale component of a TRS matrix. If any scales are negative, this problem is
// inherently ill-posed, since any pair of negative scales is equivalent to a 180 degree rotation
// on the third axis. This method resolves ambiguities in favor of positive scales with an optional
// single negative scale on the x-axis.
HMatrix trsScaleMat(const HMatrix &cam);
HVector3 trsScale(const HMatrix &cam);

// Extract just the rotational component of a TRS matrix.
HMatrix trsRotationMat(const HMatrix &cam);
Quaternion trsRotation(const HMatrix &cam);

// Extract just the translational component of a TRS matrix.
HMatrix trsTranslationMat(const HMatrix &cam);
HVector3 trsTranslation(const HMatrix &cam);

// Combine a pure camera rotation and a pure camera translation into a full 6DoF camera motion.
HMatrix cameraMotion(const HMatrix &translation, const HMatrix &rotation);
HMatrix cameraMotion(HVector3 translation, Quaternion rotation);
HMatrix cameraMotion(HPoint3 translation, Quaternion rotation);
HMatrix cameraMotion(float x, float y, float z, float qw, float qx, float qy, float qz);

// Combine a full Translation/Rotation/Scale matrix.
HMatrix trsMat(HPoint3 translation, Quaternion rotation, float scale);

// Scales the translation component of the camera matrix.
HMatrix scaleTranslation(float scale, const HMatrix &cameraMotion);

// Gets the relative motion from pos1 to pos2 in the reference frame where pos1 is the origin.
HMatrix egomotion(const HMatrix &worldPos1, const HMatrix &worldPos2);
Quaternion egomotion(Quaternion worldPos1, Quaternion worldPos2);

// Updates a world position with an ego-reference-frame delta.
HMatrix updateWorldPosition(const HMatrix &worldPos, const HMatrix &egodelta);
Quaternion updateWorldPosition(Quaternion worldPos, Quaternion egodelta);

// Computes the delta such that delta * worldPos1 = worldPos2 for a known ego-reference-frame delta.
HMatrix worldDelta(const HMatrix &worldPos, const HMatrix &egodelta);
Quaternion worldDelta(Quaternion worldPos, Quaternion egodelta);

// Return matrix t and r components in a single line string.
String extrinsicToString(const HMatrix &mat);
String extrinsicToPrettyString(const HMatrix &mat);
String extrinsicToPrettyStringPYR(const HMatrix &mat);  // Pitch Yaw Roll instead of Quaternion.
// Return matrix t, r, and s components in a single line string.
String extrinsicToTrsString(const HMatrix &mat);
String extrinsicToPrettyTrsString(const HMatrix &mat);

// Gets a the component of cameraOrientation projected onto the ground plane.
float groundPlaneRotationRads(Quaternion cameraOrientation);

// Get the frame boundary (in ray space) for the supplied intrinsics.
void frameBounds(c8_PixelPinholeCameraModel intrinsic, HPoint2 *lowerLeft, HPoint2 *upperRight);

// Get the frame boundary (in ray space) assuming that some pixels are excluded from processing,
// typically because of how features are extracted.
void frameBounds(
  c8_PixelPinholeCameraModel intrinsic,
  int excludedEdgePixels,
  HPoint2 *lowerLeft,
  HPoint2 *upperRight);

// Get the frame boundary (in ray space) assuming that some pixels are excluded from processing,
// but allowing for an extra expanded ray-space search radius for fuzzy matching.
void frameBounds(
  c8_PixelPinholeCameraModel intrinsic,
  int excludedEdgePixels,
  float rayRadius,
  HPoint2 *lowerLeft,
  HPoint2 *upperRight);

HMatrix gravityAlignedTr(const HMatrix &m);

bool equalMatrices(const HMatrix &a, const HMatrix &b, float epsilon = 1e-6f);
bool isIdentity(const HMatrix &a, float epsilon = 1e-6f);

}  // namespace c8
