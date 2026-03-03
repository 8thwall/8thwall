// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"egomotion.h"};
  deps = {
    "//c8:hmatrix",
    "//c8:hvector",
    "//c8:quaternion",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x2a28e16c);

#include <sstream>

#include "c8/geometry/egomotion.h"

namespace c8 {

namespace {
constexpr float FLOAT_EPS = 1e-5;
constexpr float sign(float f) { return f < 0.0f ? -1.0f : 1.0f; }
constexpr float det2(float m00, float m01, float m10, float m11) { return m00 * m11 - m01 * m10; }

constexpr float det3(
  float m00,
  float m01,
  float m02,
  float m10,
  float m11,
  float m12,
  float m20,
  float m21,
  float m22) {
  return m00 * det2(m11, m12, m21, m22)  // Col 0
    - m01 * det2(m10, m12, m20, m22)     // Col 1
    + m02 * det2(m10, m11, m20, m21);    // Col 2
}
}  // namespace

HMatrix rotationMat(const HMatrix &cam) {
  return HMatrix{
    {cam(0, 0), cam(0, 1), cam(0, 2), 0.0f},
    {cam(1, 0), cam(1, 1), cam(1, 2), 0.0f},
    {cam(2, 0), cam(2, 1), cam(2, 2), 0.0f},
    {0.000000f, 0.000000f, 0.000000f, 1.0f},
    {cam(0, 0), cam(1, 0), cam(2, 0), 0.0f},
    {cam(0, 1), cam(1, 1), cam(2, 1), 0.0f},
    {cam(0, 2), cam(1, 2), cam(2, 2), 0.0f},
    {0.000000f, 0.000000f, 0.000000f, 1.0f}};
}

Quaternion rotation(const HMatrix &cam) { return Quaternion::fromHMatrix(cam); }

HMatrix translationMat(const HMatrix &cam) {
  return HMatrixGen::translation(cam(0, 3), cam(1, 3), cam(2, 3));
}

HVector3 translation(const HMatrix &cam) { return HVector3(cam(0, 3), cam(1, 3), cam(2, 3)); }

// Extract just the scale component of a TRS matrix.
HMatrix trsScaleMat(const HMatrix &cam) {
  auto s = trsScale(cam);
  return HMatrixGen::scale(s.x(), s.y(), s.z());
}

HVector3 trsScale(const HMatrix &cam) {
  auto sx = HVector3{cam(0, 0), cam(1, 0), cam(2, 0)}.l2Norm();
  auto sy = HVector3{cam(0, 1), cam(1, 1), cam(2, 1)}.l2Norm();
  auto sz = HVector3{cam(0, 2), cam(1, 2), cam(2, 2)}.l2Norm();
  // Detect a scale inversion, and negate the scale of the x-axis arbitrarily.
  auto s = sign(det3(
    cam(0, 0),
    cam(0, 1),
    cam(0, 2),
    cam(1, 0),
    cam(1, 1),
    cam(1, 2),
    cam(2, 0),
    cam(2, 1),
    cam(2, 2)));
  return {s * sx, sy, sz};
}

// Extract just the rotational component of a TRS matrix.
HMatrix trsRotationMat(const HMatrix &cam) { return rotationMat(cam * trsScaleMat(cam).inv()); }
Quaternion trsRotation(const HMatrix &cam) { return rotation(cam * trsScaleMat(cam).inv()); }

// Extract just the translational component of a TRS matrix. This is the same as extracting just
// the the translational component of the TR matrix.
HMatrix trsTranslationMat(const HMatrix &cam) { return translationMat(cam); }
HVector3 trsTranslation(const HMatrix &cam) { return translation(cam); }

HMatrix cameraMotion(const HMatrix &translation, const HMatrix &rotation) {
  return translation * rotation;
}

HMatrix cameraMotion(HVector3 translation, Quaternion rotation) {
  return cameraMotion(
    HMatrixGen::translation(translation.x(), translation.y(), translation.z()),
    rotation.toRotationMat());
}

HMatrix cameraMotion(HPoint3 translation, Quaternion rotation) {
  return cameraMotion(
    HMatrixGen::translation(translation.x(), translation.y(), translation.z()),
    rotation.toRotationMat());
}

HMatrix cameraMotion(float x, float y, float z, float qw, float qx, float qy, float qz) {
  return cameraMotion(HMatrixGen::translation(x, y, z), Quaternion(qw, qx, qy, qz).toRotationMat());
}

HMatrix trsMat(HPoint3 translation, Quaternion rotation, float scale) {
  return cameraMotion(translation, rotation) * HMatrixGen::scale(scale);
}

HMatrix scaleTranslation(float s, const HMatrix &cam) {
  return cameraMotion(translation(cam) * s, rotation(cam));
}

HMatrix egomotion(const HMatrix &worldPos1, const HMatrix &worldPos2) {
  return worldPos1.inv() * worldPos2;
}

Quaternion egomotion(Quaternion worldPos1, Quaternion worldPos2) {
  return worldPos1.inverse().times(worldPos2);
}

HMatrix updateWorldPosition(const HMatrix &worldPos, const HMatrix &egodelta) {
  return worldPos * egodelta;
}

Quaternion updateWorldPosition(Quaternion worldPos, Quaternion egodelta) {
  return worldPos.times(egodelta);
}

// Computes the delta such that delta * worldPos1 = worldPos2 for a known ego-reference-frame delta.
HMatrix worldDelta(const HMatrix &worldPos, const HMatrix &egodelta) {
  return updateWorldPosition(worldPos, egodelta) * worldPos.inv();
}

Quaternion worldDelta(Quaternion worldPos, Quaternion egodelta) {
  return updateWorldPosition(worldPos, egodelta).times(worldPos.inverse());
}

String extrinsicToString(const HMatrix &mat) {
  std::stringstream sStream;
  const auto r = rotation(mat);
  const auto t = translation(mat);
  sStream << "(" << t.x() << ", " << t.y() << ", " << t.z() << ") (w: " << r.w() << ", x: " << r.x()
          << ", y: " << r.y() << ", z: " << r.z() << ")";
  return sStream.str();
}

String extrinsicToPrettyString(const HMatrix &mat) {
  std::stringstream sStream;
  return format(
    "t: %s r: %s",
    trsTranslation(mat).toPrettyString().c_str(),
    trsRotation(mat).toPrettyString().c_str());
}

String extrinsicToPrettyStringPYR(const HMatrix &mat) {
  std::stringstream sStream;
  return format(
    "t: %s r: %s",
    trsTranslation(mat).toPrettyString().c_str(),
    trsRotation(mat).toPitchYawRollString().c_str());
}

String extrinsicToTrsString(const HMatrix &mat) {
  std::stringstream sStream;
  return format(
    "t: %s r: %s, s: %s",
    trsTranslation(mat).toString().c_str(),
    trsRotation(mat).toString().c_str(),
    trsScale(mat).toString().c_str());
}

String extrinsicToPrettyTrsString(const HMatrix &mat) {
  std::stringstream sStream;
  return format(
    "t: %s r: %s, s: %s",
    trsTranslation(mat).toPrettyString().c_str(),
    trsRotation(mat).toPrettyString().c_str(),
    trsScale(mat).toPrettyString().c_str());
}

float groundPlaneRotationRads(Quaternion cameraOrientation) {
  // Find a point just in front of the camera in world coordinates.
  HPoint3 cameraNorm{0.0f, 0.0f, 1.0f};
  HPoint3 cameraNormInWorldCoords = cameraOrientation.toRotationMat() * cameraNorm;

  // Project the point onto the groundplane.
  HVector2 arc(cameraNormInWorldCoords.x(), cameraNormInWorldCoords.z());

  // If the point falls too close to the origin, we are already facing toward or away from the
  // ground plane. Imagine looking up 90 degrees from the ground and choose that direction.
  if (arc.l2Norm() <= FLOAT_EPS) {
    return groundPlaneRotationRads(updateWorldPosition(
      cameraOrientation, Quaternion(0.70710678118f, -0.70710678118f, 0.0f, 0.0f)));
  }

  return std::atan2(arc.x(), arc.y());
}

void frameBounds(c8_PixelPinholeCameraModel intrinsic, HPoint2 *lowerLeft, HPoint2 *upperRight) {
  return frameBounds(intrinsic, 0, 0.0f, lowerLeft, upperRight);
}

// Get the frame boundary (in ray space) assuming that some pixels are excluded from processing,
// typically because of how features are extracted.
void frameBounds(
  c8_PixelPinholeCameraModel intrinsic,
  int excludedEdgePixels,
  HPoint2 *lowerLeft,
  HPoint2 *upperRight) {
  return frameBounds(intrinsic, excludedEdgePixels, 0.0f, lowerLeft, upperRight);
}

// Get the frame boundary (in ray space) assuming that some pixels are excluded from processing,
// but allowing for an extra expanded ray-space search radius for fuzzy matching.
void frameBounds(
  c8_PixelPinholeCameraModel intrinsic,
  int excludedEdgePixels,
  float rayRadius,
  HPoint2 *lowerLeft,
  HPoint2 *upperRight) {
  auto Kinv = HMatrixGen::intrinsic(intrinsic).inv();
  // Pixel centers go from 0 -> width - 1 inclusive. The boundary of the pixels is .5 to the left
  // and right respectively. This scheme make sure the viewable width matches the viewable width
  // of the image.
  // TODO(nb): Add a correction for feature point border.
  auto a = (Kinv * HPoint3{-0.5f + excludedEdgePixels, -0.5f + excludedEdgePixels, 1.0f}).flatten();
  auto b = (Kinv
            * HPoint3{static_cast<float>(intrinsic.pixelsWidth - excludedEdgePixels) - 0.5f,
                      static_cast<float>(intrinsic.pixelsHeight - excludedEdgePixels) - 0.5f,
                      1.0f})
             .flatten();
  float lx = a.x();
  float ly = a.y();
  float ux = b.x();
  float uy = b.y();
  if (lx > ux) {
    std::swap(lx, ux);
  }
  if (ly > uy) {
    std::swap(ly, uy);
  }

  *lowerLeft = HPoint2{lx - rayRadius, ly - rayRadius};
  *upperRight = HPoint2{ux + rayRadius, uy + rayRadius};
}

HMatrix gravityAlignedTr(const HMatrix &m) {
  auto t = translation(m);
  auto r = rotation(m);
  // TODO(nbutko): Investigate whether yawRadians or groundPlaneRotation gives a more robust result
  // when the camera is looking straight up or down.
  return cameraMotion(t, Quaternion::yRadians(groundPlaneRotationRads(r)));
}

bool equalMatrices(const HMatrix &a, const HMatrix &b, float epsilon) {
  auto aData = a.data();
  auto bData = b.data();
  for (int i = 0; i < aData.size(); ++i) {
    if (std::abs(aData[i] - bData[i]) > epsilon) {
      return false;
    }
  }
  return true;
}

bool isIdentity(const HMatrix &a, float epsilon) {
  return equalMatrices(a, HMatrixGen::i(), epsilon);
}

}  // namespace c8
