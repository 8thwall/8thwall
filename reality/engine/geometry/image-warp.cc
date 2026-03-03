// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"image-warp.h"};
  deps = {
    "//c8/geometry:egomotion",
    "//c8/geometry:homography",
    "//c8:hmatrix",
    "//c8:quaternion",
    "//c8/geometry:parameterized-geometry",
    "//c8/pixels:image-roi",
  };
}
cc_end(0x1ff2d988);

#include "c8/geometry/egomotion.h"
#include "c8/geometry/homography.h"
#include "c8/pixels/image-roi.h"
#include "reality/engine/geometry/image-warp.h"

namespace c8 {

HMatrix rotationToHorizontal(Quaternion pose) {
  // To rotate points such that that are downward, we need to compute the egocentric rotation to
  // make that happen, and then invert it.
  auto gpr = groundPlaneRotation(pose);
  auto corrpose = egomotion(gpr, pose);
  return egomotion(corrpose.toRotationMat(), HMatrixGen::rotationD(90.0f, 0.0f, 0.0f)).inv();
}

HMatrix rotationToVertical(Quaternion pose) {
  // To rotate points such that that are downward, we need to compute the egocentric rotation to
  // make that happen, and then invert it.
  auto gpr = groundPlaneRotation(pose);
  auto corrpose = egomotion(gpr, pose);
  return egomotion(corrpose.toRotationMat(), HMatrixGen::rotationD(0.0f, 0.0f, 0.0f)).inv();
}

HMatrix twoDProjectionMat(const HMatrix &h) {
  auto ih = h.inv();
  return HMatrix{{h(0, 0), h(0, 1), 0.0f, h(0, 2)},
                 {h(1, 0), h(1, 1), 0.0f, h(1, 2)},
                 {0.0000f, 0.0000f, 1.0f, 0.0000f},
                 {h(2, 0), h(2, 1), 0.0f, h(2, 2)},
                 {ih(0, 0), ih(0, 1), 0.0f, ih(0, 2)},
                 {ih(1, 0), ih(1, 1), 0.0f, ih(1, 2)},
                 {0.00000f, 0.00000f, 1.0f, 0.00000f},
                 {ih(2, 0), ih(2, 1), 0.0f, ih(2, 2)}};
}

HMatrix glRotationHomography(c8_PixelPinholeCameraModel k1, const HMatrix &rotationMat) {
  auto k = HMatrixGen::intrinsic(glPinholeCameraModel(k1));
  auto h = k * rotationMat * k.inv();
  return twoDProjectionMat(h);
}

c8_PixelPinholeCameraModel glPinholeCameraModel(c8_PixelPinholeCameraModel k) {
  auto fx = 2 * k.focalLengthHorizontal / (k.pixelsWidth - 1.0f);
  auto fy = 2 * k.focalLengthVertical / (k.pixelsHeight - 1.0f);
  auto cx = 2.0f * ((k.pixelsWidth - 1) / 2.0f - k.centerPointX) / (k.pixelsWidth - 1);
  auto cy = 2.0f * ((k.pixelsHeight - 1) / 2.0f - k.centerPointY) / (k.pixelsHeight - 1);
  return {2, 2, cx, cy, fx, fy};
}

HMatrix glUnscaledHorizontalWarp(c8_PixelPinholeCameraModel k, Quaternion xrPose) {
  return glRotationHomography(k, rotationToHorizontal(xrPose));
}

HMatrix glUnscaledVerticalWarp(c8_PixelPinholeCameraModel k, Quaternion xrPose) {
  return glRotationHomography(k, rotationToVertical(xrPose));
}

HMatrix glScaledHorizontalWarp(c8_PixelPinholeCameraModel k, Quaternion xrPose) {
  auto h = glUnscaledHorizontalWarp(k, xrPose);
  return HMatrixGen::translation(0.0f, 1.0f, 0.0f) * HMatrixGen::scale(0.2f, 0.2f, 1.0f) * h;
}

HMatrix glScaledVerticalWarp(c8_PixelPinholeCameraModel k, Quaternion xrPose) {
  auto v = glUnscaledVerticalWarp(k, xrPose);
  return HMatrixGen::scale(0.33f, 0.33f, 1.0f) * v;
}

HMatrix glImageTargetWarp(
  c8_PixelPinholeCameraModel targetK,
  c8_PixelPinholeCameraModel camK,
  const HMatrix &camMotion,
  const PlanarImageGeometry &geom) {
  // Find the homography that rotates the image points to be a rectangle.
  float cAspectY = static_cast<float>(camK.pixelsWidth) / camK.pixelsHeight;
  auto rotCorrection = geom.isRotated ? HMatrixGen::scale(cAspectY, 1.f, 1.f).inv()
      * HMatrixGen::rotationD(0.f, 0.f, 90.f) * HMatrixGen::scale(cAspectY, 1.f, 1.f)
                                      : HMatrixGen::i();
  auto glh = rotCorrection * glRotationHomography(camK, rotationMat(camMotion));

  // Find the scale that makes the rectangle fill 90% of the viewport along the max dimension.
  constexpr float targetScale = 0.9f;

  auto corners = imageTargetCornerPixels(targetK, camK, camMotion);
  auto ul = geom.isRotated ?
      HPoint2{
        2.0f * (corners[1].x() / (camK.pixelsWidth - 1.00f) - 0.5f),
        2.0f * (corners[1].y() / (camK.pixelsHeight - 1.0f) - 0.5f)}
    : HPoint2{
        2.0f * (corners[0].x() / (camK.pixelsWidth - 1.00f) - 0.5f),
        2.0f * (corners[0].y() / (camK.pixelsHeight - 1.0f) - 0.5f)};
  auto lr = geom.isRotated ?
      HPoint2{
        2.0f * (corners[3].x() / (camK.pixelsWidth - 1.00f) - 0.5f),
        2.0f * (corners[3].y() / (camK.pixelsHeight - 1.0f) - 0.5f)}
    : HPoint2{
        2.0f * (corners[2].x() / (camK.pixelsWidth - 1.00f) - 0.5f),
        2.0f * (corners[2].y() / (camK.pixelsHeight - 1.0f) - 0.5f)};

  auto glul = glh * ul.extrude();
  auto gllr = glh * lr.extrude();

  float tAspectY = static_cast<float>(targetK.pixelsWidth) / targetK.pixelsHeight;
  if (geom.isRotated) {
    tAspectY = 1.0f / tAspectY;
  }
  float scaleScaleY = tAspectY > cAspectY ? cAspectY / tAspectY : 1.0f;
  auto scaleY = 2.0f * targetScale * scaleScaleY / (gllr.y() - glul.y());

  // Fix a slight bias in the x-dimension for non-3x4 aspect ratios.
  float tAspectX = static_cast<float>(targetK.pixelsWidth - 1.0f) / (targetK.pixelsHeight - 1.0f);
  if (geom.isRotated) {
    tAspectX = 1.0f / tAspectX;
  }
  float cAspectX = static_cast<float>(camK.pixelsWidth - 1.0f) / (camK.pixelsHeight - 1.0f);
  float scaleScaleX =
    tAspectX > cAspectX ? cAspectX / tAspectX : (cAspectX / tAspectX) / (cAspectY / tAspectY);
  auto scaleX = 2.0f * targetScale * scaleScaleX / (gllr.y() - glul.y());

  // Adjust aspect ratio to fit ROI aspect
  // Here, our search camera aspect ratio is (aC,aR), and we want to make it into (bC,bR)
  //   (aC, aR * (aC/aR) / (bC/bR))
  // = (aC, aC * (bR/bC))
  // which has the aspect ratio of (bC, bR)
  // Our transformation is the inverse of this scaling
  auto glsh = HMatrixGen::scale(scaleX, scaleY / cAspectY * ROI_ASPECT, 1.0f) * glh;

  // Find the translation that centers the target in the viewport.
  auto center = imageTargetCenterPixel(camK, camMotion);
  auto c = HPoint2{2.0f * (center.x() / (camK.pixelsWidth - 1.00f) - 0.5f),
                   2.0f * (center.y() / (camK.pixelsHeight - 1.0f) - 0.5f)};
  auto glc = glsh * c.extrude();
  auto tx = -glc.x();
  auto ty = -glc.y();

  return HMatrixGen::translation(tx, ty, 0.0f) * glsh;
}

HMatrix glImageCurvyWarp(c8_PixelPinholeCameraModel camK, const HMatrix &camMotion) {
  // Find the homography that rotates the image points to be a rectangle.
  auto glh = glRotationHomography(camK, rotationMat(camMotion));

  // Set the target margin, so that the target fills x% of the roi.
  constexpr float targetScale = 0.9f;

  // Compute the border points of the roi in image space.
  Vector<HPoint3> borderPts{{-0.375f, 0.5f, 0.0f}, {0.375f, -0.5f, 0.0f}};
  auto proj = HMatrixGen::intrinsic(camK) * camMotion.inv();
  auto corners = flatten<2>(proj * borderPts);

  // Convert these to clip space.
  auto ul = HPoint2{2.0f * (corners[0].x() / (camK.pixelsWidth - 1.00f) - 0.5f),
                    2.0f * (corners[0].y() / (camK.pixelsHeight - 1.0f) - 0.5f)};
  auto lr = HPoint2{2.0f * (corners[1].x() / (camK.pixelsWidth - 1.00f) - 0.5f),
                    2.0f * (corners[1].y() / (camK.pixelsHeight - 1.0f) - 0.5f)};

  // Apply a rotation-only homography to these points. They are now rotated in the right orientation
  // but not at the correct center or scale.
  auto glul = glh * ul.extrude();
  auto gllr = glh * lr.extrude();

  // Compute the scale needed to get the corners to fill the target margin, and compute their
  // new location, which is not centered correctly.
  auto scale = 2.0f * targetScale / (gllr.y() - glul.y());
  auto glsh = HMatrixGen::scale(scale, scale, 1.0f) * glh;

  // Find the translation that centers the target in the viewport.
  auto center = (proj * HPoint3(0.0f, 0.0f, 0.0f)).flatten();
  auto c = HPoint2{2.0f * (center.x() / (camK.pixelsWidth - 1.00f) - 0.5f),
                   2.0f * (center.y() / (camK.pixelsHeight - 1.0f) - 0.5f)};
  auto glc = glsh * c.extrude();
  auto tx = -glc.x();
  auto ty = -glc.y();

  // Return the stacked rotate->scale->translate homography matrix.
  return HMatrixGen::translation(tx, ty, 0.0f) * glsh;
}

}  // namespace c8
