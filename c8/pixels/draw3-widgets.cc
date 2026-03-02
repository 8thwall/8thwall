// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "draw3-widgets.h",
  };
  deps = {
    "//c8:color",
    "//c8:hmatrix",
    "//c8:hpoint",
    "//c8:quaternion",
    "//c8:vector",
    "//c8/geometry:egomotion",
    "//c8/geometry:homography",
    "//c8/geometry:vectors",
    "//c8/pixels/render:object8",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x812b9f89);

#include "c8/geometry/egomotion.h"
#include "c8/geometry/homography.h"
#include "c8/geometry/vectors.h"
#include "c8/pixels/draw3-widgets.h"
#include "c8/vector.h"

namespace c8 {
constexpr float BALLOON_STRING_LENGTH = 2.f;

// Creates a group of three axis aligned arrows at the origin.
std::unique_ptr<Group> axis(const Color &xColor, const Color &yColor, const Color &zColor) {
  auto g = ObGen::named(ObGen::group(), "axis");
  g->add(ObGen::named(
    ObGen::positioned(
      ObGen::arrow(xColor), HMatrixGen::translateX(0.5f) * HMatrixGen::yDegrees(90.0f)),
    "x-axis"));
  g->add(ObGen::named(
    ObGen::positioned(
      ObGen::arrow(yColor), HMatrixGen::translateY(0.5f) * HMatrixGen::xDegrees(-90.0f)),
    "y-axis"));
  g->add(
    ObGen::named(ObGen::positioned(ObGen::arrow(zColor), HMatrixGen::translateZ(0.5f)), "z-axis"));
  return g;
}

// Creates a group of three axis aligned arrows showing the specified position and rotation.
std::unique_ptr<Group> orientedPoint(
  HPoint3 position, Quaternion rotation, float size, const AxisColor &axisColor, float headHeight) {
  auto pos = HMatrixGen::translation(position.x(), position.y(), position.z())
    * rotation.toRotationMat() * HMatrixGen::scale(size);
  auto g = ObGen::named(ObGen::positioned(ObGen::group(), pos), "oriented-point");
  g->add(axis(axisColor.x, axisColor.y, axisColor.z));
  if (headHeight > 0.f) {
    // The head balloon always point toward +y
    auto headPos = rotation.inverse().toRotationMat() * HMatrixGen::translation(0, headHeight, 0);
    g->add(ObGen::positioned(ObGen::pointCloud({{0.f, 0.f, 0.f}}, axisColor.y), headPos));
    g->add(ObGen::positioned(
      ObGen::barCloud(
        {{{0.f, 0.f, 0.f}, {0.f, BALLOON_STRING_LENGTH + headHeight, 0.f}}}, axisColor.y, 0.003),
      pos));
  }
  return g;
}

bool BreadCrumbs::shouldAddCrumb(
  const HMatrix &extrinsic, c8_PixelPinholeCameraModel intrinsic) const {
  // Given the camera pose and K, determine if any of the breadcrumbs are in view.
  HPoint2 lowerLeft;
  HPoint2 upperRight;
  frameBounds(intrinsic, &lowerLeft, &upperRight);
  auto isInBound = [&](const HPoint3 &pos) -> bool {
    auto posInCamera = extrinsic.inv() * pos;
    if (posInCamera.z() < 0.f) {
      // behind the camera is not visible
      return false;
    }
    auto posInRay = posInCamera.flatten();
    return (
      lowerLeft.x() < posInRay.x() && posInRay.x() < upperRight.x() && lowerLeft.y() < posInRay.y()
      && posInRay.y() < upperRight.y());
  };

  for (const auto &child : children()) {
    // We have the translation on the "axis" group, which is wrapped by another "oriented-point"
    // group.
    auto crumbOriginPosition = asPoint(trsTranslation(child->find<Group>("axis").world()));
    HPoint3 crumbBalloonPosition = {
      crumbOriginPosition.x(),
      crumbOriginPosition.y() + BALLOON_STRING_LENGTH,
      crumbOriginPosition.z()};

    // Don't add a breadcrumb if we already have a breadcrumb in the image
    if (isInBound(crumbOriginPosition) || isInBound(crumbBalloonPosition)) {
      return false;
    }
  }
  return true;
}

void BreadCrumbs::maybeAddCrumb(const HMatrix &extrinsic, c8_PixelPinholeCameraModel intrinsic) {
  if (!shouldAddCrumb(extrinsic, intrinsic)) {
    return;
  }

  addInView(extrinsic, intrinsic);
}

void BreadCrumbs::maybeAddCrumb(
  const HMatrix &extrinsic,
  c8_PixelPinholeCameraModel intrinsic,
  const Vector<HPoint2> &featurePosInRay) {
  if (!shouldAddCrumb(extrinsic, intrinsic)) {
    return;
  }

  addInViewClosestToCorner(extrinsic, intrinsic, featurePosInRay);
}

void BreadCrumbs::maybeAddCrumb(
  const HMatrix &extrinsic,
  c8_PixelPinholeCameraModel intrinsic,
  const Vector<HPoint3> &pointsInWorld) {
  if (!shouldAddCrumb(extrinsic, intrinsic)) {
    return;
  }

  const auto pointInRay = flatten<2>(extrinsic.inv() * pointsInWorld);
  addInViewClosestToCorner(extrinsic, intrinsic, pointInRay, &pointsInWorld);
}

void BreadCrumbs::addInView(const HMatrix &extrinsic, c8_PixelPinholeCameraModel intrinsic) {
  HPoint2 lowerLeft;
  HPoint2 upperRight;
  frameBounds(intrinsic, &lowerLeft, &upperRight);
  // add a breadcrumb in the middle of the lower third of the image.
  auto rayY = (std::abs(lowerLeft.y() - upperRight.y()) / 3.0f) + lowerLeft.y();
  addOrientedPoint({0.f, rayY}, extrinsic);
}

void BreadCrumbs::addOrientedPoint(const HPoint2 &ray, const HMatrix &extrinsic) {
  auto newPointOnGround = triangulatePointsOnGround(extrinsic, -1.0f, {ray})[0];

  auto newPointOnGroundInCamera = extrinsic.inv() * newPointOnGround;
  // Check that the point is in front of the camera.  If the camera is looking up, then the
  // point returned by triangulatePointsOnGround will be behind the camera, which we don't want.
  if (newPointOnGroundInCamera.z() > 0.0f) {
    addOrientedPoint(newPointOnGround);
  } else {
    auto balloonHeight = -1.f + BALLOON_STRING_LENGTH;
    if (translation(extrinsic).y() >= balloonHeight) {
      // camera is too high, tracking is likely bad
      return;
    }

    auto newPointOnSky = triangulatePointsOnGround(extrinsic, balloonHeight, {ray})[0];
    HPoint3 newPointOnSkyGroundShadow = {newPointOnSky.x(), -1.f, newPointOnSky.z()};
    addOrientedPoint(newPointOnSkyGroundShadow);
  }
}

void BreadCrumbs::addOrientedPoint(const HPoint3 &positionInWorld) {
  addOrientedPoint(
    HMatrixGen::translation(positionInWorld.x(), positionInWorld.y(), positionInWorld.z()));
}

void BreadCrumbs::addOrientedPoint(const HMatrix &transform) {
  int numCrumb = children().size();
  Color zColor = Color::viridis((numCrumb * 32 % 255) / 255.f);
  Color yColor = Color::turbo((numCrumb * 16 % 255) / 255.f);
  AxisColor orientedPointColor{Color::CHERRY, yColor, zColor};

  auto t = translation(transform);
  auto r = rotation(transform);
  add(orientedPoint({t.x(), t.y(), t.z()}, r, 0.5f, orientedPointColor, 2.f));
}

int BreadCrumbs::addInViewClosestToCorner(
  const HMatrix &extrinsic,
  c8_PixelPinholeCameraModel intrinsic,
  const Vector<HPoint2> &featurePosInRay,
  const Vector<HPoint3> *optionalFeaturePosInWorld) {
  HPoint2 lowerLeft;
  HPoint2 upperRight;
  frameBounds(intrinsic, &lowerLeft, &upperRight);
  // starting add a breadcrumb in the middle of the lower sixth of the image.
  auto rayY = (std::abs(lowerLeft.y() - upperRight.y()) / 6.0f) + lowerLeft.y();
  HPoint2 targetLocation{0.f, rayY};
  int pickedLoc = -1;
  float smallestDist = std::numeric_limits<float>::max();
  for (int i = 0; i < featurePosInRay.size(); i++) {
    const auto &loc = featurePosInRay[i];
    float dist = loc.sqdist(targetLocation);
    if (dist < smallestDist) {
      pickedLoc = i;
      smallestDist = dist;
    }
  }

  if (pickedLoc >= 0) {
    if (optionalFeaturePosInWorld == nullptr) {
      addOrientedPoint(featurePosInRay[pickedLoc], extrinsic);
    } else {
      addOrientedPoint((*optionalFeaturePosInWorld)[pickedLoc]);
    }
  }
  return pickedLoc;
}

void BreadCrumbs::mirror(BreadCrumbs *other) const {
  for (auto &otherChild : other->mutableChildren()) {
    // NOTE(dat): This can be faster if we support object8::clearChildren()
    otherChild->remove();
  }
  for (const auto &child : children()) {
    other->addOrientedPoint(child->local());
  }
}

// Creates a group of three axis aligned arrows showing the specified position and rotation.
std::unique_ptr<BreadCrumbs> breadCrumbs() {
  return std::unique_ptr<BreadCrumbs>(new BreadCrumbs());
}

static Color BL_COLOR = Color::GREEN;
static Color BR_COLOR = Color::BLUE;
static Color TL_COLOR = Color::YELLOW;
static Color TR_COLOR = Color::RED;
static Color BACK_COLOR = Color::DARK_RED;
static Color FRONT_COLOR = Color::PURPLE;
static Color LEFT_COLOR = Color::DARK_BLUE;
static Color RIGHT_COLOR = Color::DARK_YELLOW;

static Color DARK_BL_COLOR = Color::GRAY_04;
static Color DARK_BR_COLOR = Color::GRAY_04;
static Color DARK_TL_COLOR = Color::GRAY_04;
static Color DARK_TR_COLOR = Color::GRAY_04;
static Color DARK_BACK_COLOR = Color::RED;
static Color DARK_FRONT_COLOR = Color::LIGHT_PURPLE;
static Color DARK_LEFT_COLOR = Color::BLUE;
static Color DARK_RIGHT_COLOR = Color::MANGO;
// Creates a grid of points along the ground (y=0), colored by quadrant.
std::unique_ptr<Group> groundPointGrid(int radius, float stepSize) {
  Vector<HPoint3> compassPoints;
  Vector<Color> colors;
  Vector<PointIndices> inds;
  for (int x = -radius; x <= radius; ++x) {
    for (int z = -radius; z <= radius; ++z) {
      inds.push_back({static_cast<uint32_t>(inds.size())});
      compassPoints.push_back(
        HPoint3(static_cast<float>(stepSize * x), 0.0f, static_cast<float>(stepSize * z)));
      if (x < 0 && z < 0) {
        // bottom left
        colors.push_back(BL_COLOR);
      } else if (x == 0 && z < 0) {
        // back
        colors.push_back(BACK_COLOR);
      } else if (x > 0 && z < 0) {
        // bottom right
        colors.push_back(BR_COLOR);
      } else if (x < 0 && z == 0) {
        // left
        colors.push_back(LEFT_COLOR);
      } else if (x > 0 && z == 0) {
        // right
        colors.push_back(RIGHT_COLOR);
      } else if (x < 0 && z > 0) {
        // top left
        colors.push_back(TL_COLOR);
      } else if (x > 0 && z > 0) {
        // top right
        colors.push_back(TR_COLOR);
      } else {
        // front
        colors.push_back(FRONT_COLOR);
      }
    }
  }
  auto g = ObGen::named(ObGen::group(), "ground-point-grid");
  g->add(ObGen::pointCloud(compassPoints, colors, 0.043f));
  return g;
}

// Creates a grid of lines along the ground (y=0), colored by quadrant
static float constexpr FLOOR_HEIGHT = 0.f;
static float constexpr GROUND_LINE_RADIUS = 0.005f;
std::unique_ptr<Group> groundLineGrid(int radius, float stepSize, float scale, bool dark) {
  float fRadius = radius;
  Vector<std::pair<HPoint3, HPoint3>> lines;
  Vector<Color> colors;
  // Draw the 4 main lines
  lines.emplace_back(HPoint3{0.f, FLOOR_HEIGHT, 0.f}, HPoint3{0.f, FLOOR_HEIGHT, -fRadius});
  colors.push_back(dark ? DARK_BACK_COLOR : BACK_COLOR);
  lines.emplace_back(HPoint3{0.f, FLOOR_HEIGHT, 0.f}, HPoint3{0.f, FLOOR_HEIGHT, fRadius});
  colors.push_back(dark ? DARK_FRONT_COLOR : FRONT_COLOR);
  lines.emplace_back(HPoint3{0.f, FLOOR_HEIGHT, 0.f}, HPoint3{-fRadius, FLOOR_HEIGHT, 0.f});
  colors.push_back(dark ? DARK_LEFT_COLOR : LEFT_COLOR);
  lines.emplace_back(HPoint3{0.f, FLOOR_HEIGHT, 0.f}, HPoint3{fRadius, FLOOR_HEIGHT, 0.f});
  colors.push_back(dark ? DARK_RIGHT_COLOR : RIGHT_COLOR);

  // Draw the 4 quadrants lines
  for (int i = 1; i <= radius; i++) {
    float fI = i;
    lines.emplace_back(HPoint3{-fRadius, FLOOR_HEIGHT, fI}, HPoint3{0.f, FLOOR_HEIGHT, fI});
    colors.push_back(dark ? DARK_TL_COLOR : TL_COLOR);
    lines.emplace_back(HPoint3{-fI, FLOOR_HEIGHT, fRadius}, HPoint3{-fI, FLOOR_HEIGHT, 0.f});
    colors.push_back(dark ? DARK_TL_COLOR : TL_COLOR);

    lines.emplace_back(HPoint3{fRadius, FLOOR_HEIGHT, fI}, HPoint3{0.f, FLOOR_HEIGHT, fI});
    colors.push_back(dark ? DARK_TR_COLOR : TR_COLOR);
    lines.emplace_back(HPoint3{fI, FLOOR_HEIGHT, fRadius}, HPoint3{fI, FLOOR_HEIGHT, 0.f});
    colors.push_back(dark ? DARK_TR_COLOR : TR_COLOR);

    lines.emplace_back(HPoint3{-fRadius, FLOOR_HEIGHT, -fI}, HPoint3{0.f, FLOOR_HEIGHT, -fI});
    colors.push_back(dark ? DARK_BL_COLOR : BL_COLOR);
    lines.emplace_back(HPoint3{-fI, FLOOR_HEIGHT, -fRadius}, HPoint3{-fI, FLOOR_HEIGHT, 0.f});
    colors.push_back(dark ? DARK_BL_COLOR : BL_COLOR);

    lines.emplace_back(HPoint3{fRadius, FLOOR_HEIGHT, -fI}, HPoint3{0.f, FLOOR_HEIGHT, -fI});
    colors.push_back(dark ? DARK_BR_COLOR : BR_COLOR);
    lines.emplace_back(HPoint3{fI, FLOOR_HEIGHT, -fRadius}, HPoint3{fI, FLOOR_HEIGHT, 0.f});
    colors.push_back(dark ? DARK_BR_COLOR : BR_COLOR);
  }

  auto g = ObGen::named(ObGen::group(), "ground-line-grid");
  g->add(ObGen::positioned(
    ObGen::barCloud(lines, colors, GROUND_LINE_RADIUS), HMatrixGen::scale(scale)));
  auto &obgenQuad = g->add(ObGen::named(
    ObGen::positioned(ObGen::quad(), HMatrixGen::xDegrees(90) * HMatrixGen::scale(radius)),
    "transparent-surface"));
  obgenQuad.setMaterial(MatGen::physical());
  ObGen::setTransparent(obgenQuad, 0.8f);
  obgenQuad.geometry().setColor(Color::DUSTY_VIOLET);

  return g;
}

// Creates a group that displays a camera frustum.
std::unique_ptr<Group> frustum(c8_PixelPinholeCameraModel k, Color color, float scale) {
  // Find the corners of the camera frustum at distance 1 from the camera.
  float w = static_cast<float>(k.pixelsWidth - 1);
  float h = static_cast<float>(k.pixelsHeight - 1);
  Vector<HPoint3> pixCorners = {{0.0f, 0.0f, 1.0f}, {0.0f, h, 1.0f}, {w, h, 1.0f}, {w, 0.0f, 1.0f}};
  auto corners = HMatrixGen::intrinsic(k).inv() * pixCorners;

  // Create lines linking the corners with each other, and linking each corner to the origin.
  Vector<std::pair<HPoint3, HPoint3>> lines = {
    std::pair<HPoint3, HPoint3>{corners[0], corners[1]},
    std::pair<HPoint3, HPoint3>{corners[1], corners[2]},
    std::pair<HPoint3, HPoint3>{corners[2], corners[3]},
    std::pair<HPoint3, HPoint3>{corners[3], corners[0]},
    std::pair<HPoint3, HPoint3>{corners[0], HPoint3{0.0f, 0.0f, 0.0f}},
    std::pair<HPoint3, HPoint3>{corners[1], HPoint3{0.0f, 0.0f, 0.0f}},
    std::pair<HPoint3, HPoint3>{corners[2], HPoint3{0.0f, 0.0f, 0.0f}},
    std::pair<HPoint3, HPoint3>{corners[3], HPoint3{0.0f, 0.0f, 0.0f}},
  };

  auto g = ObGen::named(ObGen::group(), "frustum");
  g->add(ObGen::positioned(ObGen::barCloud(lines, color), HMatrixGen::scale(scale)));

  return g;
}

// Creates a group that displays a camera frustum with a quad attached to the base.
std::unique_ptr<Group> quadFrustum(
  c8_PixelPinholeCameraModel k, Color color, float scale, bool connectFrameToOrigin) {
  // Find the corners of the camera frustum at distance 1 from the camera.
  float w = static_cast<float>(k.pixelsWidth - 1);
  float h = static_cast<float>(k.pixelsHeight - 1);
  Vector<HPoint3> pixCorners = {{0.0f, 0.0f, 1.0f}, {0.0f, h, 1.0f}, {w, h, 1.0f}, {w, 0.0f, 1.0f}};
  auto corners = HMatrixGen::intrinsic(k).inv() * pixCorners;

  // Create lines linking the corners with each other, and linking each corner to the origin.
  Vector<std::pair<HPoint3, HPoint3>> lines = {
    std::pair<HPoint3, HPoint3>{corners[0], corners[1]},
    std::pair<HPoint3, HPoint3>{corners[1], corners[2]},
    std::pair<HPoint3, HPoint3>{corners[2], corners[3]},
    std::pair<HPoint3, HPoint3>{corners[3], corners[0]}};

  // If we don't only want the frame, connect each corner to the origin.
  if (connectFrameToOrigin) {
    Vector<std::pair<HPoint3, HPoint3>> toOrigin = {
      std::pair<HPoint3, HPoint3>{corners[0], HPoint3{0.0f, 0.0f, 0.0f}},
      std::pair<HPoint3, HPoint3>{corners[1], HPoint3{0.0f, 0.0f, 0.0f}},
      std::pair<HPoint3, HPoint3>{corners[2], HPoint3{0.0f, 0.0f, 0.0f}},
      std::pair<HPoint3, HPoint3>{corners[3], HPoint3{0.0f, 0.0f, 0.0f}}};
    lines.insert(lines.end(), toOrigin.begin(), toOrigin.end());
  }

  auto g = ObGen::named(ObGen::group(), "quad-frustum");
  g->add(ObGen::positioned(ObGen::barCloud(lines, color), HMatrixGen::scale(scale)));

  auto &quad = g->add(ObGen::positioned(ObGen::quad(), HMatrixGen::scale(scale)));
  quad.setMaterial(MatGen::image());
  quad.geometry().setVertices({corners[1], corners[0], corners[3], corners[2]});

  return g;
}

// Add a lighting rig with lights in each overhead qudrant and ambient light.
std::unique_ptr<Group> quadrantLights(
  float scale, float pointIntensity, float ambientIntensity, Color pointColor, Color ambientColor) {
  auto g = ObGen::named(ObGen::group(), "quadrant-lights");
  auto &g2 = g->add(ObGen::positioned(ObGen::group(), HMatrixGen::scale(scale)));
  g2.add(ObGen::ambientLight(ambientColor, ambientIntensity));

  g2.add(ObGen::positioned(
    ObGen::pointLight(pointColor, pointIntensity), HMatrixGen::translation(1.0f, 1.0f, 0.0f)));
  g2.add(ObGen::positioned(
    ObGen::pointLight(pointColor, pointIntensity), HMatrixGen::translation(-1.0f, 1.0f, 0.0f)));
  g2.add(ObGen::positioned(
    ObGen::pointLight(pointColor, pointIntensity), HMatrixGen::translation(0.0f, 1.0f, 1.0f)));
  g2.add(ObGen::positioned(
    ObGen::pointLight(pointColor, pointIntensity), HMatrixGen::translation(0.0f, 1.0f, -1.0f)));
  return g;
}

}  // namespace c8
