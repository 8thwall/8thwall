// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Dat Chu (dat@8thwall.com)

#pragma once

#include <utility>

#include "c8/hmatrix.h"
#include "c8/hpoint.h"
#include "c8/hvector.h"
#include "c8/pixels/pixels.h"
#include "c8/vector.h"

using namespace c8;

namespace c8 {

struct PlanarImageGeometry {
  bool isRotated = false;
};

/** Info needed for mapping from image space to 3d model space.
 * An activation region is the quad on the surface of the object
 * i.e. it denotes the location and size of a sticker on a curvy
 */
struct ActivationRegion {
  float left = 0.f;    // (-1, 1) 0 and 1 are back of curvy from a left-right perspective, 0.5 is front.
  float right = 0.f;   // (0:1] right > left, right - left <= 1
  float top = 0.f;     // [0,1] ymin
  float bottom = 0.f;  // [0,1] ymax
};

// TODO(dat): Handle the general case of cones
/** The curvy image geometry output always have a height of 1.0. Its radius is scaled so that
 * the height can stay at 1.0f. Since we scale the radius, the unit of for width and height does
 * not matter. Measurements are generally in mm.
 *
 * Note that we generally crop the image target that the user uploads. At the construction of a
 * CurvyImageGeometry the curved image target is referring to the cropped one. The engine
 * only knows about this curved image target mapped onto a curvy with height 1.0f. To derive
 * the pose of the original curved image target precropped, you will need to use DetectionImage
 * knowledge of this crop. @see DetectionImage.toPrecroppedPose()
 *
 * An example calculation for the Dumela African Brew:
 *   15.3cm x 10cm spanning the front ~270 degrees (t,r,b,l) = (0.1, 0.8625, 0.9, 0.1375)
 * so, 2*pi*r = 15.3 / (0.8625-0.1375) => r = 3.35872
 * since h = 10 (but only cover activation region 0.1 to 0.9),
 *   r is thus 3.35872 / 10 / (0.9 - 0.1) = 0.41984
 */
struct CurvyImageGeometry {
  // NOTE(dat): height is kept at 1.0f while radius is between [0.f, 1.f]
  //            so if we need to resize srcRows and srcCols, these values don't change
  float radius = 0.f;  // radiusTop
  float height = 0.f;
  ActivationRegion activationRegion;

  // These values are for the pixel size of the texture wrapping this geometry. i.e. if you have a
  // landscape texture of size 1440x1080, then srcCols = 1440, srcRows=1080. Pixel coordinates on
  // this landscape texture can then be mapped to geometry.
  // When you use DetectionImageLoader, the loader will write the pyramid level 0
  // dimension into these values taking into account whether your image is rotated. This allows
  // feature points computed to have their locations used with this geometry to perform
  // mapTo/FromGeometry. See detection-image-loader-test.cc for these values.
  int srcRows = 0;  // input height after rotation has been applied
  int srcCols = 0;  // input width after rotation has been applied

  bool isCone = false;
  float radiusBottom = 0;  // is available only when isCone is true

  /** Derive the geometry of the image target.
   *
   * @param p the image target texture: needed only for its size
   * @param width width of the image target (targetCircumference in mm)
   * @param height height of the image target (sideLength in mm)
   * @param activationRegion where is the image target (p) within the curvy
   */
  static CurvyImageGeometry fromPixels(
    ConstPixels p, float width, float height, ActivationRegion activationRegion);

  // Does not initialize srcRows and srcCols. Same as fromPixels
  static CurvyImageGeometry fromWidthHeight(
    float width, float height, ActivationRegion activationRegion);

  String toString() const noexcept;
};

struct MapGeometryData {
  const float scaleX = 0.f;
  const float scaleY = 0.f;
  const float shift = 0.f;
};

// metadata that helps mapping between unconed and rainbow
struct RainbowMetadata {
  HPoint2 tl;
  HPoint2 bl;
  HPoint2 tr;
  HPoint2 apex;
  float r1 = 0.f;
  float r2 = 0.f;
  float theta = 0.f;
};

RainbowMetadata buildRainbowMetadata(
  const CurvyImageGeometry &geom, float largeRadiusOverSrcRows, float smallRadiusOverSrcRows);

HPoint2 mapRainbowToUnconed(
  const RainbowMetadata &metadata,
  const float &outputWidth,
  const float &outputHeight,
  const HPoint2 &rainbowPoint);

// Defines the relation of the image target to its outer crop in relative coordinates so that
// generic defaults can be specified.
struct CurvyOuterCrop {
  float outerWidth = 1.0;   // ratio of full image width to crop width, must 1 or greater.
  float outerHeight = 1.0;  // ratio of the full image height to crop height, must be 1 or greater.
  float cropStartX =
    0.0;  // fraction of x dimension for the upper left of the crop in the outer geometry.
  float cropStartY =
    0.0;  // fraction of y dimension for the upper left of the crop in the outer geometry.

  c8::String toString() const noexcept {
    return c8::format(
      "(outerWidth: %04.4f, outerHeight: %04.4f, cropStartX: %04.4f, cropStartY: %04.4f)",
      outerWidth,
      outerHeight,
      cropStartX,
      cropStartY);
  }
};
/** Spec allows for quick and easy construction of curvies in various configurations
 *
 * CurvySpec arcOnly = {0.383636f};
 * CurvySpec activationRegion = {0.383636f, false, {670.0f / 646.0f, 1.0f, 7.0f / 670.0f, 0.0f}};
 * CurvySpec basicCone =  {0.383636f, false, {}, 0.9f};
 *
 */
struct CurvySpec {
  float arc = 0.5f;        // arc length of the outer geometry in [0, 1]
  bool isRotated = false;  // Indicates that the detection image and crop geometry are relative to a
                           // rotated input source.
  CurvyOuterCrop crop;
  float base = 1.0f;  // ratio of the top to the bottom radius

  c8::String toString() const noexcept {
    return c8::format(
      "(arc: %04.4f, isRotated: %d, crop: %s, base: %04.4f)",
      arc,
      isRotated,
      crop.toString().c_str(),
      base);
  }
};
void curvyForTarget(
  int cropWidth,
  int cropHeight,
  CurvySpec s,
  CurvyImageGeometry *geom,
  CurvyImageGeometry *fullGeom);

// the opening angle of the cone in radian
float computeTheta(const CurvyImageGeometry &geom);

// Project points in the image space to 3d points in model space. Return both its location and
// normals. the curvy is up-right along the y axis with the center at (0, 0, 0)
void mapToGeometry(
  CurvyImageGeometry geom,
  const Vector<HPoint2> &imPts,
  Vector<HPoint3> *modelPts,
  Vector<HVector3> *modelNormals);

// Calculates data needed for mapToGeometryPoint()
MapGeometryData prepareMapToGeometryData(CurvyImageGeometry geom);
// Calculates data needed for mapFromGeometryPoint()
MapGeometryData prepareMapFromGeometryData(CurvyImageGeometry geom);

// Project point in the image space to 3d point in model space. Return only its location.
HPoint3 mapToGeometryPoint(
  const MapGeometryData &mapGeometryData, const CurvyImageGeometry &geom, const HPoint2 &imPt);

// Project points in the image space to 3d points in model space. Return only its location.
void mapToGeometryPoints(
  CurvyImageGeometry geom, const Vector<HPoint2> &imPts, Vector<HPoint3> *modelPts);

// Project points in the rainbow image space to 3d points in model space. Return only its location.
// Only support cup.
// TODO(dat): Support fez.
void mapRainbowToGeometryPoints(
  CurvyImageGeometry geom,
  const RainbowMetadata &rainbowMetadata,
  const Vector<HPoint2> &imPts,
  Vector<HPoint3> *modelPts);

// Project points in the image space to 3d normals in model space.
void mapToGeometryNormals(
  CurvyImageGeometry geom, const Vector<HPoint2> &imPts, Vector<HVector3> *modelNormals);

// Given a point in the model space. Convert it into pixel space
HPoint2 mapFromGeometry(
  const MapGeometryData &mapFromGeometryData,
  const CurvyImageGeometry &geom,
  const HPoint3 &modelPt);
Vector<HPoint2> mapFromGeometry(CurvyImageGeometry geom, const Vector<HPoint3> &modelPt);

/** Given the camera rays observing points on the curvy, find those points
 * on the curvy in model space. The curvy model pose is assumed to be upright (axis aligned
 * with the y axis) and at the origin point of (0, 0, 0) with the radius r.
 *
 * @param raysInCamera rays in the search image of points on the curvy
 * @param cameraToTarget pose of the camera which generated the rays wrt curvy model pose
 */
Vector<HPoint3> cameraRaysToTargetWorld(
  CurvyImageGeometry geom, const Vector<HPoint2> &raysInCamera, const HMatrix &cameraToTarget);

Vector<HPoint2> cameraPointsSearchWorldToTargetPixel(
  CurvyImageGeometry geom,
  const Vector<HPoint2> &raysInSearch,
  const HMatrix &cameraPoseRelativeModel);

/** Given the camera rays observing points on the curvy, find those points on the curvy in
 * camera space.
 *
 * @param raysInCamera rays in the search image of points on the curvy
 * @param cameraToTarget pose of the camera which generated the rays wrt curvy model pose
 * @param searchPtsInCamera all search points in camera space (including not-visible points)
 * @param visibleSearchPtIndices indices of visible points
 */
void cameraRaysToVisiblePointsInCameraCurvy(
  const CurvyImageGeometry &geom,
  const Vector<HPoint2> &raysInCamera,
  const HMatrix &cameraToTarget,
  Vector<HPoint3> *searchPtsInCamera,
  Vector<size_t> *visibleSearchPtIndices);

/** Given the camera rays observing points on a planar target, find those points on the planar
 * target in camera space.
 *
 * @param raysInCamera rays in the search image of points on the curvy
 * @param cameraToTarget pose of the camera which generated the rays wrt curvy model pose
 * @param searchPtsInCamera all search points in camera space (including not-visible points)
 * @param visibleSearchPtIndices indices of visible points
 */
void cameraRaysToVisiblePointsInCameraPlanar(
  c8_PixelPinholeCameraModel targetK,
  c8_PixelPinholeCameraModel camK,
  const Vector<HPoint2> &raysInCamera,
  const Vector<HPoint2> &searchPtsInSearchPixel,
  const HMatrix &cameraToTarget,
  Vector<HPoint3> *searchPtsInCamera,
  Vector<size_t> *visibleSearchPtIndices);

Vector<HPoint3> cameraFrameCorners(c8_PixelPinholeCameraModel k, float depth);

Vector<HPoint2> imageTargetCornerPixels(
  c8_PixelPinholeCameraModel targetK, c8_PixelPinholeCameraModel camK, const HMatrix &camMotion);

HPoint2 imageTargetCenterPixel(c8_PixelPinholeCameraModel camK, const HMatrix &camMotion);

Vector<HPoint2> curvyTargetEdgePixels(
  CurvyImageGeometry geom, c8_PixelPinholeCameraModel camK, const HMatrix &camMotion);

// Find point on the curvy that is closest to the camera. The y-coordinate specifies what the
// height on the curvy of the point we are querying.
HPoint3 closestCurvySurfacePointAtHeight(const HMatrix &cam, CurvyImageGeometry im, float y);

}  // namespace c8
