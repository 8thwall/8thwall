// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "c8/color.h"
#include "c8/geometry/mesh-types.h"
#include "c8/hmatrix.h"
#include "c8/hpoint.h"
#include "c8/pixels/pixels.h"
#include "c8/vector.h"

namespace c8 {

void drawLine(HPoint2 fromPt, HPoint2 toPt, int width, Color color, RGBA8888PlanePixels dest);

void drawRectangle(HPoint2 topLeft, HPoint2 bottomRight, Color color, RGBA8888PlanePixels dest);

void drawShape(Vector<HPoint2> vertices, int width, Color color, RGBA8888PlanePixels dest);
void drawShape(Vector<HPoint2> pts, int width, Vector<Color> color, RGBA8888PlanePixels dest);

void drawPoints(Vector<HPoint2> pts, float radius, Color color, RGBA8888PlanePixels dest);
void drawPoints(
  Vector<HPoint2> pts, float radius, int thickness, Color color, RGBA8888PlanePixels dest);

void drawPoint(HPoint2 pt, float radius, Color color, RGBA8888PlanePixels dest);
void drawPoint(HPoint2 pt, float radius, int thickness, Color color, RGBA8888PlanePixels dest);

void drawPoint3(HPoint3 pt, Color color, RGBA8888PlanePixels dest);
void drawPoints3(const Vector<HPoint3> &pts, Color color, RGBA8888PlanePixels dest);

void heatMap(uint8_t val, uint8_t *r, uint8_t *g, uint8_t *b);

Color heatMap(uint8_t val);

// Draws a single channel of an image to a color image using heat map colorization to boost
// contrast.
void drawImageChannel(ConstRGBA8888PlanePixels src, int channel, RGBA8888PlanePixels dest);

// Draws a single channel of an image to a color image using grayscale.
void drawImageChannelGray(ConstRGBA8888PlanePixels src, int channel, RGBA8888PlanePixels dest);

void drawCompass(const HMatrix &extrinsic, const HMatrix &intrinsic, RGBA8888PlanePixels dest);

void drawAxis(
  const HMatrix &extrinsic,
  const HMatrix &intrinsic,
  HPoint3 origin,
  float len,
  RGBA8888PlanePixels dest);

void drawAxis(
  const HMatrix &extrinsic,
  const HMatrix &intrinsic,
  HPoint3 origin,
  float len,
  const std::array<Color, 3> &axisColors,
  RGBA8888PlanePixels dest);

void drawAnchor(
  const HMatrix &extrinsic,
  const HMatrix &intrinsic,
  const HMatrix &anchorPose,
  float len,
  RGBA8888PlanePixels dest);

void drawAnchor(
  const HMatrix &extrinsic,
  const HMatrix &intrinsic,
  const HMatrix &anchorPose,
  float len,
  const std::array<Color, 3> &axisColors,
  RGBA8888PlanePixels dest);

void fill(Color color, RGBA8888PlanePixels dest);

void putText(
  const String &text, HPoint2 pt, Color color, RGBA8888PlanePixels dest, bool dropShadow = true);
void putText(const String &text, HPoint2 pt, Color color, Color bgColor, RGBA8888PlanePixels dest);
void putText(
  const String &text,
  HPoint2 pt,
  Color color,
  Color bgColor,
  float fontSize,
  RGBA8888PlanePixels dest);

void drawVertexNormals(
  const Vector<HPoint3> &vertices,
  const Vector<MeshIndices> &indices,
  const float normalScalingFactor,
  const HMatrix &cam,
  const HMatrix &cameraMotion,
  Color color,
  int lineWidth,
  RGBA8888PlanePixels &dest);

void drawFaceNormals(
  const Vector<HPoint3> &vertices,
  const Vector<MeshIndices> &indices,
  const float normalScalingFactor,
  const HMatrix &cam,
  const HMatrix &cameraMotion,
  Color color,
  int lineWidth,
  RGBA8888PlanePixels &dest);

void drawMesh(
  const Vector<HPoint2> &projectedPoints,
  const Vector<MeshIndices> &indices,
  Color color,
  int lineWidth,
  RGBA8888PlanePixels &dest);

void drawMeshAndNormals(
  const Vector<HPoint3> &vertices,
  const Vector<MeshIndices> &indices,
  const float normalScalingFactor,
  const HMatrix &cam,
  const HMatrix &cameraMotion,
  const int lineWidth,
  RGBA8888PlanePixels &dest);

}  // namespace c8
