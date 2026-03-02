// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "c8/color.h"
#include "c8/geometry/parameterized-geometry.h"
#include "c8/hmatrix.h"
#include "c8/hpoint.h"
#include "c8/pixels/gr8-pyramid.h"
#include "c8/pixels/pixels.h"
#include "c8/string.h"
#include "c8/vector.h"
#include "reality/engine/features/frame-point.h"
#include "reality/engine/features/image-point.h"
#include "reality/engine/imagedetection/target-point.h"

namespace c8 {

void drawFeatures(const ImagePoints &feats, RGBA8888PlanePixels dest, bool useGravityAngle = false);
void drawFeatures(
  const FrameWithPoints &feats, RGBA8888PlanePixels dest, bool useGravityAngle = false);
void drawFeatures(
  const TargetWithPoints &feats,
  RGBA8888PlanePixels dest,
  bool useGravityAngle = false,
  bool rotateFeatures = false,
  int imageWidth = 0);

void drawFeatureGravityDirections(
  const HMatrix &extrinsic,
  const HMatrix &intrinsic,
  const FrameWithPoints &feats,
  float len,
  Color color,
  RGBA8888PlanePixels dest);

void drawPyramidChannel(Gr8Pyramid pyr, int channel, RGBA8888PlanePixels dest);

void textBox(
  const Vector<String> &lines,
  HPoint2 topLeft,
  int width,
  RGBA8888PlanePixels dest,
  Color textColor = Color::PURPLE,
  Color bgColor = Color::OFF_WHITE);

// Extracts the colors for each pixel in the list.
Vector<Color> pixelColors(ConstRGBA8888PlanePixels im, const Vector<HPoint2> &pixels);

// Image 3d points and draw them with a specified color.
void drawPointCloud(
  c8_PixelPinholeCameraModel k,
  const HMatrix &cam,
  const Vector<HPoint3> &pts,
  const Vector<Color> &colors,
  RGBA8888PlanePixels out);

void drawPointCloudWithBackCulling(
  c8_PixelPinholeCameraModel k,
  const HMatrix &cam,
  const Vector<HPoint3> &pts,
  const Vector<HVector3> &normals,
  const Vector<Color> &colors,
  RGBA8888PlanePixels out);

void drawImageFrustum(
  const c8_PixelPinholeCameraModel &targetK,
  const c8_PixelPinholeCameraModel &intrinsics,
  const HMatrix &extrinsic,
  const HMatrix &imPose,
  float scale,
  Color color,
  RGBA8888PlanePixels cp);

void drawCameraFrustum(
  c8_PixelPinholeCameraModel intrinsic,
  const HMatrix &extrinsic,
  c8_PixelPinholeCameraModel worldIntrinsics,
  const HMatrix &worldPos,
  Color color,
  RGBA8888PlanePixels dest);

void drawFloor(
  c8_PixelPinholeCameraModel intrinsic,
  const HMatrix &extrinsic,
  HPoint3 origin,
  float scale,
  int xpts,
  int ypts,
  Color color,
  RGBA8888PlanePixels dest);

// Draw the image target boundaries in 3d for a specified CurvyImageGeometry.
void drawCurvyImage(
  c8_PixelPinholeCameraModel k, const HMatrix &cam, CurvyImageGeometry im, RGBA8888PlanePixels out);

Vector<HPoint3> curvyModelPoints(CurvyImageGeometry im, float scale);

void drawCurvyImageAtScale(
  c8_PixelPinholeCameraModel k,
  const HMatrix &cam,
  CurvyImageGeometry im,
  float scale,
  RGBA8888PlanePixels out);

void drawCubeImageAtScale(
  c8_PixelPinholeCameraModel k, const HMatrix &cam, float scale, Color c, RGBA8888PlanePixels out);

}  // namespace c8
