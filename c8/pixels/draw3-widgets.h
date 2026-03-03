// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "c8/color.h"
#include "c8/hmatrix.h"
#include "c8/hpoint.h"
#include "c8/pixels/render/object8.h"
#include "c8/quaternion.h"

namespace c8 {

// Creates a group of three axis aligned arrows at the origin.
std::unique_ptr<Group> axis(const Color &xColor, const Color &yColor, const Color &zColor);

struct AxisColor {
  Color x = Color::CHERRY;
  Color y = Color::MANGO;
  Color z = Color::MINT;
};

// Creates a group of three axis aligned arrows showing the specified position and rotation.
// @param headHeight when > 0, add a head balloon at y = headHeight
std::unique_ptr<Group> orientedPoint(
  HPoint3 position,
  Quaternion rotation,
  float size,
  const AxisColor &axisColor = {},
  float headHeight = 0.f);

// Creates a grid of points along the ground (y=0), colored by quadrant.
std::unique_ptr<Group> groundPointGrid(int radius, float stepSize);

// Creates a grid of lines along the ground (y=0), colored by quadrant
std::unique_ptr<Group> groundLineGrid(
  int radius, float stepSize, float scale = 1.f, bool dark = false);

// Creates a group that displays a camera frustum.
std::unique_ptr<Group> frustum(c8_PixelPinholeCameraModel intrinsic, Color color, float scale);
std::unique_ptr<Group> quadFrustum(
  c8_PixelPinholeCameraModel intrinsic, Color color, float scale, bool connectFrameToOrigin = true);

// Add a lighting rig with lights in each overhead qudrant and ambient light.
std::unique_ptr<Group> quadrantLights(
  float scale = 5,
  float pointIntensity = 0.6f,
  float ambientIntensity = 0.6f,
  Color pointColor = Color::WHITE,
  Color ambientColor = Color::WHITE);

// A breadcrumb automatically adds an object to the ground in front of the camera if there is not
// a breadcrumb already in sight.  It will store older breadcrumbs that are out of sight.
class BreadCrumbs : public Group {
public:
  BreadCrumbs() { this->setName("bread-crumbs"); }

  void maybeAddCrumb(const HMatrix &extrinsic, c8_PixelPinholeCameraModel intrinsic);
  void maybeAddCrumb(
    const HMatrix &extrinsic,
    c8_PixelPinholeCameraModel intrinsic,
    const Vector<HPoint2> &featurePosInRay);

  // Add a breadcrumb at one of the points provided in world space. This is useful for example if
  // you want to add a breadcrumb at one of the depth point positions.
  // @param extrinsic pose in world
  // @param intrinsic
  // @param pointsInWorld position of each point in world space.
  void maybeAddCrumb(
    const HMatrix &extrinsic,
    c8_PixelPinholeCameraModel intrinsic,
    const Vector<HPoint3> &pointsInWorld);

  // Overwrite the content of another BreadCrumbs. Used to keep two breadcrumbs in sync
  void mirror(BreadCrumbs *other) const;

private:
  bool shouldAddCrumb(const HMatrix &extrinsic, c8_PixelPinholeCameraModel intrinsic) const;
  /** Add breadcrumb at the bottom 1/3 of the screen
   */
  void addInView(const HMatrix &extrinsic, c8_PixelPinholeCameraModel intrinsic);

  // Add a breadcrumb near one of the featurePosInRay. If optionalPointsInWorld exists, use the 3d
  // corresponding location instead.
  // @param extrinsic pose in world
  // @param intrinsic
  // @param featurePosInRay the location of the features in ray space
  // @param optionalPointsInWorld the corresponding position in world space for each feature.
  // @return index in featurePosInRay that we picked
  int addInViewClosestToCorner(
    const HMatrix &extrinsic,
    c8_PixelPinholeCameraModel intrinsic,
    const Vector<HPoint2> &featurePosInRay,
    const Vector<HPoint3> *optionalFeaturePosInWorld = nullptr);

  void addOrientedPoint(const HPoint2 &ray, const HMatrix &extrinsic);
  void addOrientedPoint(const HPoint3 &position);
  void addOrientedPoint(const HMatrix &transform);
};

std::unique_ptr<BreadCrumbs> breadCrumbs();

}  // namespace c8
