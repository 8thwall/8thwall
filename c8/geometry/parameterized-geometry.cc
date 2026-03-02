// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Dat Chu (dat@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"parameterized-geometry.h"};
  deps = {
    "//c8:c8-log",
    "//c8:color",
    "//c8:hmatrix",
    "//c8:hpoint",
    "//c8:hvector",
    "//c8:vector",
    "//c8/geometry:egomotion",
    "//c8/geometry:homography",
    "//c8/geometry:line",
    "//c8/geometry:interpolation",
    "//c8/geometry:two-d",
    "//c8/pixels:pixels",
    "//c8/string:format",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x0ad0e597);

#ifdef _WIN32
#define _USE_MATH_DEFINES
#endif
#include <math.h>

#include <cmath>

#include "c8/c8-log.h"
#include "c8/geometry/egomotion.h"
#include "c8/geometry/homography.h"
#include "c8/geometry/interpolation.h"
#include "c8/geometry/two-d.h"
#include "c8/geometry/line.h"
#include "c8/hmatrix.h"
#include "c8/string/format.h"
#include "c8/geometry/parameterized-geometry.h"

namespace c8 {

namespace {
// DEPRECATED, create CurvyImageGeometry with curvyForTarget
// This method and corresponding fromPixels and fromWidthHeight method
// will not work for cones
CurvyImageGeometry build(
  int pixelRows,
  int pixelCols,
  float width,
  float height,
  const ActivationRegion &activationRegion) {
  // We scale radius by the height so that height can be set to 1.0f
  const float radius =
    width / (height * (activationRegion.right - activationRegion.left) * 2 * M_PI);
  // TODO(dat): Can we scale height between 0 and 1.f so the activationRegion.bottom
  // .          and top is used
  return {radius, 1.0f, activationRegion, pixelRows, pixelCols};
}
}  // namespace

CurvyImageGeometry CurvyImageGeometry::fromPixels(
  ConstPixels p, float width, float height, ActivationRegion activationRegion) {
  return build(p.rows(), p.cols(), width, height, activationRegion);
}

// Does not initialize srcRows and srcCols. Same as fromPixels
CurvyImageGeometry CurvyImageGeometry::fromWidthHeight(
  float width, float height, ActivationRegion activationRegion) {
  return build(0, 0, width, height, activationRegion);
}

c8::String CurvyImageGeometry::toString() const noexcept {
  return c8::format(
    "%s (r: %04.2f, rBottom: %04.2f, h: %04.2f, srcRows: %d, srcCols: %d, activationRegion: (l: "
    "%04.2f, r: %04.2f, t: %04.2f, b: %04.2f))",
    isCone ? "Cone" : "Cylinder",
    radius,
    radiusBottom,
    height,
    srcRows,
    srcCols,
    activationRegion.left,
    activationRegion.right,
    activationRegion.top,
    activationRegion.bottom);
}

// Build a cropped and a full image geometry. The cropped image geometry can be
// used for tracking while the full one can be used for returning to the user who
// is only aware of the full geometry.
void curvyForTarget(
  int cropWidth,
  int cropHeight,
  CurvySpec s,
  CurvyImageGeometry *geom,
  CurvyImageGeometry *fullGeom) {
  float arcAngleRadians = s.arc * M_PI * 2.0f;

  if (s.isRotated) {
    std::swap(cropWidth, cropHeight);
    std::swap(s.crop.outerWidth, s.crop.outerHeight);
    std::swap(s.crop.cropStartX, s.crop.cropStartY);
    s.crop.cropStartY = 1.f - s.crop.cropStartY - (1.f / s.crop.outerHeight);
  }
  // Calculations require r > rBottom, so flip our cone upside down to calculate radiuses.
  // Won't affect the activation region.
  bool isConeFlipped = false;
  if (s.base < 1.0f) {
    s.base = 1.0f / s.base;
    isConeFlipped = true;
  }

  float pixelWidthToHeightRatio =
    (cropWidth * s.crop.outerWidth) / (cropHeight * s.crop.outerHeight);

  // Let's compute radius with respect to height
  // r / rBottom = base
  // s = targetCircumference / pixelWidthToHeightRatio
  // targetCircumference = arc * 2 * M_PI * r
  // s**2 - (r - rBottom)**2 = h**2
  //   solving for r
  // s = arc * 2 * M_PI * r / pixelWidthToHeightRatio
  // (arc * 2 * M_PI * r / pixelWidthToHeightRatio) ** 2 - r**2 * (1 - 1 / base) **2 = h ** 2
  // Let A = arc * 2 * M_PI / pixelWidthToHeightRatio
  // .   B = 1 - 1 / base
  // Then, A**2 * r**2 - B**2 * r**2 = h**2
  // since all values are positive
  //     r = h / sqrt(A**2 - B**2)

  float A = arcAngleRadians / pixelWidthToHeightRatio;
  float B = 1.f - 1.f / s.base;

  // we want the cropped height to be 1.0f
  // target circumference is related to side length and not cropped height
  float uncroppedRadiusTop = (1.f * s.crop.outerHeight) / std::sqrt(A * A - B * B);
  float uncroppedRadiusBottom = uncroppedRadiusTop / s.base;

  float radiusTopAlpha = 1.f - s.crop.cropStartY;
  float radiusTop = lerp(uncroppedRadiusTop, uncroppedRadiusBottom, radiusTopAlpha);
  float radiusBottomAlpha = s.crop.cropStartY + 1.f / s.crop.outerHeight;
  float radiusBottom =
    uncroppedRadiusTop * (1.f - radiusBottomAlpha) + uncroppedRadiusBottom * radiusBottomAlpha;

  ActivationRegion activationRegion = {
    0.5f - s.arc * 0.5f + s.arc * s.crop.cropStartX,
    0.5f - s.arc / 2.f + s.arc * s.crop.cropStartX + s.arc * 1.0f / s.crop.outerWidth,
    s.crop.cropStartY,
    s.crop.cropStartY + 1.f / s.crop.outerHeight};

  // If we calculated radiuses for a flipped cone, swap top and bottom.
  if (isConeFlipped) {
    std::swap(radiusTop, radiusBottom);
    std::swap(uncroppedRadiusTop, uncroppedRadiusBottom);
  }

  // when the difference is too small, consider it a cylinder
  bool isCone = std::fabs(s.base - 1) > 1e-3;
  *geom = {radiusTop, 1.0f, activationRegion, cropHeight, cropWidth, isCone, radiusBottom};

  ActivationRegion uncroppedActivationRegion = {0.5f - s.arc * 0.5f, 0.5f + s.arc * 0.5f, 0.f, 1.f};
  *fullGeom = {
    uncroppedRadiusTop,
    s.crop.outerHeight,
    uncroppedActivationRegion,
    static_cast<int>(cropHeight * s.crop.outerHeight),
    static_cast<int>(cropWidth * s.crop.outerWidth),
    isCone,
    uncroppedRadiusBottom};
}

// Project points in the image space to 3d points.
// the curvy is up-right along the y axis with the center at (0, 0, 0)
// ymax of the curvy = 0.5
// ymin of the curvy = -0.5
// The nice thing about a curvy at origin is its model location give you knowledge of the
// normals
void mapToGeometry(
  CurvyImageGeometry geom,
  const Vector<HPoint2> &imPts,
  Vector<HPoint3> *modelPts,
  Vector<HVector3> *modelNormals) {
  mapToGeometryPoints(geom, imPts, modelPts);
  mapToGeometryNormals(geom, imPts, modelNormals);
}

MapGeometryData prepareMapToGeometryData(CurvyImageGeometry geom) {
  return {
    (geom.activationRegion.right - geom.activationRegion.left) / (geom.srcCols - 1),
    1.0f / (geom.srcRows - 1),
    geom.activationRegion.left};
}

MapGeometryData prepareMapFromGeometryData(CurvyImageGeometry geom) {
  // These sx and sy are inverse of mapToGeometry sx, sy
  float scaleX = (geom.srcCols - 1) / (geom.activationRegion.right - geom.activationRegion.left);
  float scaleY = (geom.srcRows - 1);
  float shift = geom.activationRegion.left;

  return {scaleX, scaleY, shift};
}

float radiusAtY(const CurvyImageGeometry &geom, int y) {
  return geom.isCone
    ? (geom.radiusBottom * y + geom.radius * (geom.srcRows - 1 - y)) / (geom.srcRows - 1)
    : geom.radius;
}

// This assumes that it is a cup and not a fez
// This assumes that geom.srcCols and geom.srcRows are pre-cropped. i.e. size of the full rainbow
// image
RainbowMetadata buildRainbowMetadata(
  const CurvyImageGeometry &geom, float largeRadiusOverSrcRows, float smallRadiusOverSrcRows) {
  float width = geom.srcCols - 1;
  float height = geom.srcRows - 1;
  if (width < height) {
    // rainbow image is always landscape
    std::swap(width, height);
  }
  float largeRadius = largeRadiusOverSrcRows * (height + 1);
  float smallRadius = smallRadiusOverSrcRows * (height + 1);

  const float x0 = width / 2.0f * smallRadius / largeRadius;
  const float theta = std::asin(width / 2.f / largeRadius) * 2.f;
  const float topY = 2.f * largeRadius * std::pow(std::sin(theta / 4.f), 2);
  const HPoint2 tl{0.f, topY};
  const HPoint2 bl{width / 2.f - x0, height};
  const HPoint2 tr{width, topY};
  const HPoint2 apex{width / 2.f, 1.0f * largeRadius};
  const float r1 = tl.dist(apex);
  const float r2 = bl.dist(apex);
  return {tl, bl, tr, apex, r1, r2, theta};
}

HPoint2 mapRainbowToUnconed(
  const RainbowMetadata &metadata,
  const float &outputWidth,
  const float &outputHeight,
  const HPoint2 &rainbowPoint) {
  float rAtPoint = rainbowPoint.dist(metadata.apex);
  float y =
    std::max(metadata.r1 - rAtPoint, 0.f) / (metadata.r1 - metadata.r2) * (outputHeight - 1);

  // TODO(dat): Switch to atan2 which remove ambiguity in sign and allows a fan larger than pi
  auto apexTl = line(metadata.apex, metadata.tl);
  auto apexPoint = line(metadata.apex, rainbowPoint);
  float cosineOfAngle = std::min(apexTl.dot(apexPoint), 1.f);
  float phi = std::acos(cosineOfAngle);
  float x = phi / metadata.theta * (outputWidth - 1);
  return HPoint2{x, y};
}

HPoint3 mapToGeometryPoint(
  const MapGeometryData &mapGeometryData, const CurvyImageGeometry &geom, const HPoint2 &imPt) {
  const float v_x = 2 * M_PI * (mapGeometryData.shift + mapGeometryData.scaleX * imPt.x());
  // compute the radius at this point
  float radius = radiusAtY(geom, imPt.y());

  return HPoint3{// x is positive pointing left to right, but the label wraps from left to right
                 -radius * std::sin(v_x),
                 // image has y pointing down, model has y pointing up
                 -mapGeometryData.scaleY * imPt.y() + 0.5f,
                 radius * std::cos(v_x)};
};

// TODO(dat): Make this work on fez
// map to geometry for rainbow image point
// We assume that the points are shown in a CW-rotated image (since the rainbow image is always
// rotated to 4/3 when features are extracted)
HPoint3 mapToGeometryPoint(
  const MapGeometryData &mapGeometryData,
  const RainbowMetadata &rainbowMetadata,
  const CurvyImageGeometry &geom,
  const HPoint2 &imPt) {
  HPoint2 rotatedPt = rotateCCW(imPt, geom.srcCols);
  // TODO(dat): convert imPt from cropped coordinate to full rainbow coordinate
  // They are the same when there is no crop
  float alpha = (rainbowMetadata.r1 - rainbowMetadata.apex.dist(rotatedPt))
    / (rainbowMetadata.r1 - rainbowMetadata.r2);
  float y = lerp(-0.5, 0.5, alpha);
  float radius = lerp(geom.radiusBottom, geom.radius, alpha);

  // angle in radian between line (apex, tl) and (apex, imPt)
  float conedAngle = angleBetweenABAndAC(rainbowMetadata.apex, rainbowMetadata.tl, rotatedPt);

  // TODO(dat): Make this work on cropped geometry as well
  // angle in radian in the xz-plane between the z-axis and the the line (origin, rotatedPt)
  const float v_x = 2 * M_PI * conedAngle / rainbowMetadata.theta;

  return HPoint3{// x is positive pointing left to right, but the label wraps from left to right
                 -radius * std::sin(v_x),
                 // image has y pointing down, model has y pointing up
                 y,
                 radius * std::cos(v_x)};
};

void mapRainbowToGeometryPoints(
  CurvyImageGeometry geom,
  const RainbowMetadata &rainbowMetadata,
  const Vector<HPoint2> &imPts,
  Vector<HPoint3> *modelPts) {
  auto mapGeometryData = prepareMapToGeometryData(geom);

  modelPts->reserve(imPts.size());
  std::transform(
    imPts.begin(),
    imPts.end(),
    std::back_inserter(*modelPts),
    [&mapGeometryData, &rainbowMetadata, &geom](HPoint2 p) -> HPoint3 {
      return mapToGeometryPoint(mapGeometryData, rainbowMetadata, geom, p);
    });
}

void mapToGeometryPoints(
  CurvyImageGeometry geom, const Vector<HPoint2> &imPts, Vector<HPoint3> *modelPts) {
  auto mapGeometryData = prepareMapToGeometryData(geom);

  modelPts->reserve(imPts.size());
  std::transform(
    imPts.begin(),
    imPts.end(),
    std::back_inserter(*modelPts),
    [&mapGeometryData, &geom](HPoint2 p) -> HPoint3 {
      return mapToGeometryPoint(mapGeometryData, geom, p);
    });
}

// the opening angle of the cone in radian
float computeTheta(const CurvyImageGeometry &geom) {
  if (!geom.isCone) {
    // a cylinder has parallel lines, theta is undefined
    return std::numeric_limits<float>::infinity();
  }

  return std::atan2(std::abs(geom.radius - geom.radiusBottom), geom.height) * 2.f;
}

void mapToGeometryNormals(
  CurvyImageGeometry geom, const Vector<HPoint2> &imPts, Vector<HVector3> *modelNormals) {
  float scaleX = (geom.activationRegion.right - geom.activationRegion.left) / (geom.srcCols - 1);
  // float scaleY = 1.0f / (geom.srcRows - 1);
  float shift = geom.activationRegion.left;
  float halfTheta = computeTheta(geom) / 2;
  float ySign = (geom.radius < geom.radiusBottom) ? 1.f : -1.f;

  // Compute model normals on the curvy
  modelNormals->reserve(imPts.size());
  std::transform(
    imPts.begin(),
    imPts.end(),
    std::back_inserter(*modelNormals),
    [scaleX, shift, ySign, halfTheta, &geom](HPoint2 p) -> HVector3 {
      const float v_x = 2 * M_PI * (shift + scaleX * p.x());
      float r = radiusAtY(geom, p.y());
      HVector3 normal{
        -r * std::sin(v_x),
        // note that this can be simplified to (r * (geom.radiusBottom - geom.radius) / geom.height)
        std::isfinite(halfTheta) ? static_cast<float>(ySign * r * tan(halfTheta)) : 0.f,
        r * std::cos(v_x)};
      return normal.unit();
    });
}

HPoint2 mapFromGeometry(
  const MapGeometryData &mapFromGeometryData,
  const CurvyImageGeometry &geom,
  const HPoint3 &modelPt) {
  // The zero point of the angle is at +z, and then proceeding through -x. From the point of view of
  // atan2, this is like mapping z to x and -x to +y. Note that atan2 takes (y, x) and returns
  // values in -pi:pi. If we backtrack 180 to x/-z, we can just always add M_PI and be in the right
  // range.
  float i2pi = 1.0f / (2.0f * M_PI);  // shorten cast from double to float.
  return {
    (std::atan2(modelPt.x(), -modelPt.z()) * i2pi + 0.5f - mapFromGeometryData.shift)
      * mapFromGeometryData.scaleX,
    (modelPt.y() - 0.5f) * -mapFromGeometryData.scaleY};
}

Vector<HPoint2> mapFromGeometry(CurvyImageGeometry geom, const Vector<HPoint3> &modelPt) {
  Vector<HPoint2> mappedPts;
  mappedPts.reserve(modelPt.size());
  auto data = prepareMapFromGeometryData(geom);
  std::transform(
    modelPt.begin(), modelPt.end(), std::back_inserter(mappedPts), [&data, &geom](auto pt) {
      return mapFromGeometry(data, geom, pt);
    });
  return mappedPts;
}

Vector<HPoint3> cameraRaysToTargetWorld(
  CurvyImageGeometry geom, const Vector<HPoint2> &raysInCamera, const HMatrix &cameraToTarget) {
  auto rays3InCamera = extrude<3>(raysInCamera);
  // rays rotated so that the curvy is straight up
  Vector<HPoint3> rotatedRays3InTarget = rotationMat(cameraToTarget) * rays3InCamera;

  // we are trying to find intersection between our rays (lines) and the curvy
  // A ray (a, b, c) going through point (x0, y0, z0) has the equations
  //   x = x0 + at, y = y0 + bt, z = z0 + ct
  // . solving for x from z
  // .   t = (x - x0) / a
  // .   t = (z - z0) / c
  // .   t = (y - y0) / b
  // . so, (x - x0) / a = (z - z0) / c
  // .   x = (a*z - a*z0 + c*x0) / c
  // also, (y - y0) / b = (z - z0) / c
  // .   y = (b*z - b*z0 + c*y0) / c
  //
  // A curvy at the origin, aligned along the y axis, with radius r has the equation
  //   x**2 + z**2 = r**2
  // for cylinder, we know r = geom.radius
  // for cone, we know r = geom.radius * (y + 0.5) + geom.radiusBottom * (0.5 - y)
  //   r = y * (geom.radius - geom.radiusBottom) + 0.5 (geom.radius + geom.radiusBottom)
  // . r = (b*z - b*z0 + c*y0) / c * (geom.radius - geom.radiusBottom) + 0.5 * (geom.radius +
  // geom.radiusBottom)

  // for cylinder
  // substitute into x
  //  ((a*z - a*z0 + c*x0) / c)**2 + z**2 = r**2
  // you can solve this with Wolfram Alpha:
  //  ((a*z - a*z0 + c*x0) / c)^2 + z^2 = r^2, solve for z
  // for cylinder where r = geom.radius:
  // A = a*c*x0
  // B = a*a*z0
  // C = a*a + c*c
  // D*D = c*c * (c*c * (r*r - x0*x0) + 2*a*c*x0*z0 + a*a * (r*r - z0*z0))
  // solutions:
  // z1 = (B - A - D) / C
  // z2 = (B - A + D) / C

  // for cone
  // let T1 = geom.radius + geom.radiusBottom
  //     T2 = geom.radius - geom.radiusBottom
  // solve with Sympy
  //   from sympy import *
  //   a, b, c, x0, y0, z0, r, T1, T2, z = symbols('a b c x0 y0 z0 r T1 T2 z')
  //   sols = solve(Eq(((a*z - a*z0 + c*x0) / c)**2 + z**2, ((b*z - b*z0 + c*y0) / c * T2 + 0.5 *
  //   T1)**2), z)

  // The solutions can then be generated into C code with
  //   ccode(sols[0], standard='C99')
  //   ccode(sols[1], standard='C99')

  Vector<HPoint3> ret;
  ret.reserve(rotatedRays3InTarget.size());
  auto t = translation(cameraToTarget);
  float r = geom.radius;
  for (HPoint3 &point : rotatedRays3InTarget) {
    float a = point.x() / point.z();
    float b = point.y() / point.z();
    float c = point.z() / point.z();

    float x0 = t.x();
    float y0 = t.y();
    float z0 = t.z();

    float a2 = a * a;
    float b2 = b * b;
    float ac = a * c;
    float c2 = c * c;
    float r2 = r * r;
    float x02 = x0 * x0;
    float y02 = y0 * y0;
    float z02 = z0 * z0;

    float pz, nz;
    if (geom.isCone) {
      float T1 = geom.radius + geom.radiusBottom;
      float T2 = geom.radius - geom.radiusBottom;
      float T1sq = T1 * T1;
      float T2sq = T2 * T2;

      float A = 0.5 * T1 * T2 * b * c - T2sq * b2 * z0 + T2sq * b * c * y0 + a2 * z0 - a * c * x0;
      float B = T1sq * a2 + T1sq * c2 + 4.0 * T1 * T2 * a2 * y0 - 4.0 * T1 * T2 * a * b * x0
        - 4.0 * T1 * T2 * b * c * z0 + 4.0 * T1 * T2 * c2 * y0 + 4.0 * T2sq * a2 * y02
        - 8.0 * T2sq * a * b * x0 * y0 + 4.0 * T2sq * b2 * x02 + 4.0 * T2sq * b2 * z02
        - 8.0 * T2sq * b * c * y0 * z0 + 4.0 * T2sq * c2 * y02 - 4.0 * a2 * z02
        + 8.0 * a * c * x0 * z0 - 4.0 * c2 * x02;
      float C = -T2sq * b2 + a2 + c2;

      if (B < 0 || C == 0) {
        // When we don't intersect the curvy
        ret.push_back({-1000.f, -1000.f, 1.f});
        continue;
      }
      pz = (A - 0.5 * c * std::sqrt(B)) / C;
      nz = (A + 0.5 * c * std::sqrt(B)) / C;
    } else {
      float A = ac * x0;
      float B = a2 * z0;
      float C = a2 + c2;
      float Dsq = c2 * (c2 * (r2 - x0 * x0) + 2 * ac * x0 * z0 + a2 * (r2 - z0 * z0));

      if (Dsq < 0 || C < 1e-6) {
        // When we don't intersect the curvy
        ret.push_back({-1000.f, -1000.f, 1.f});
        continue;
      }
      float D = std::sqrt(Dsq);
      pz = (B - A - D) / C;
      nz = (B - A + D) / C;
    }

    float pt = pz - z0 / c;
    float px = x0 + a * pt;
    float py = y0 + b * pt;

    float nt = nz - z0 / c;
    float nx = x0 + a * nt;
    float ny = y0 + b * nt;

    // Check for a valid height, adding an epsilon to deal with floating point comparison.
    bool pValid = py <= 0.501f && py >= -0.501f;
    bool nValid = ny <= 0.501f && ny >= -0.501f;
    if (!pValid && !nValid) {
      ret.push_back({-1000.f, -1000.f, 1.f});
      continue;
    }

    // Pick the solution closer to the camera.
    HVector3 delta1 = {px - t.x(), py - t.y(), pz - t.z()};
    HVector3 delta2 = {nx - t.x(), ny - t.y(), nz - t.z()};
    auto d1 = delta1.dot(delta1);
    auto d2 = delta2.dot(delta2);

    // it is possible that d1 == d2 when the curvy is in its natural state
    ret.push_back((d1 < d2 && pValid) ? HPoint3{px, py, pz} : HPoint3{nx, ny, nz});
  }
  return ret;
}

Vector<HPoint2> cameraPointsSearchWorldToTargetPixel(
  CurvyImageGeometry geom,
  const Vector<HPoint2> &raysInSearch,
  const HMatrix &cameraPoseRelativeToModel) {
  Vector<HPoint3> pointsInModel =
    cameraRaysToTargetWorld(geom, raysInSearch, cameraPoseRelativeToModel);
  return mapFromGeometry(geom, pointsInModel);
}

void cameraRaysToVisiblePointsInCameraCurvy(
  const CurvyImageGeometry &geom,
  const Vector<HPoint2> &raysInCamera,
  const HMatrix &cameraToTarget,
  Vector<HPoint3> *searchPtsInCamera,
  Vector<size_t> *visibleSearchPtIndices) {
  // TODO(paris): Add a modelPointToModelNormals method to avoid these roundabout projections.
  Vector<HPoint2> searchPtsInTargetPixel =
    cameraPointsSearchWorldToTargetPixel(geom, raysInCamera, cameraToTarget);
  Vector<HPoint3> searchPtsInModel;
  Vector<HVector3> searchNormalsInModel;
  mapToGeometry(geom, searchPtsInTargetPixel, &searchPtsInModel, &searchNormalsInModel);
  *searchPtsInCamera = cameraToTarget.inv() * searchPtsInModel;
  Vector<HVector3> searchNormalsInCamera = cameraToTarget.inv() * searchNormalsInModel;

  visibleSearchPtIndices->clear();
  visibleSearchPtIndices->reserve(searchPtsInModel.size());
  for (int i = 0; i < searchPtsInModel.size(); ++i) {
    // Filter search points that are not on the curvy or are not visible
    if (
      searchPtsInModel[i].y() != -1000
      && searchNormalsInCamera[i].dot(
           {searchPtsInCamera->at(i).x(), searchPtsInCamera->at(i).y(), 1.0f})
        < 0) {
      visibleSearchPtIndices->push_back(i);
    }
  }
}

void cameraRaysToVisiblePointsInCameraPlanar(
  c8_PixelPinholeCameraModel targetK,
  c8_PixelPinholeCameraModel camK,
  const Vector<HPoint2> &raysInCamera,
  const Vector<HPoint2> &searchPtsInSearchPixel,
  const HMatrix &cameraToTarget,
  Vector<HPoint3> *searchPtsInCamera,
  Vector<size_t> *visibleSearchPtIndices) {
  Vector<HPoint2> corners = imageTargetCornerPixels(targetK, camK, cameraToTarget);
  Poly2 poly = Poly2(corners);

  visibleSearchPtIndices->clear();
  visibleSearchPtIndices->reserve(searchPtsInSearchPixel.size());
  for (int i = 0; i < searchPtsInSearchPixel.size(); ++i) {
    if (poly.containsPointAssumingConvex(searchPtsInSearchPixel[i])) {
      visibleSearchPtIndices->push_back(i);
    }
  }
  // Transform search rays to be on the image target plane.
  // Equation of a plane: a * (x - x0) + b * (y - y0) + c * (z - z0) = 0
  // (x0, y0, z0) is a known point on a plane. For us it is the center of the image target.
  // [a, b, c] is the normal to the plane.
  // (x, y, z) is the other point on the plane. For us this is (s * x, s * y, s).
  // s represents the depth of this point in the camera, so this point tells us where our ray is
  // in space on the target plane.
  // So then equation of our plane: a * (s * x - x0) + b * (s * y - y0) + c * (s - z0) = 0
  // Simplified to: s = (a * x0 + b * y0 + c * z0) / (a * x + b * y + c)
  // We solve for scale factor and then use this transform our points to search points in camera.

  // The center of the image target is our known point on the plane (x0, y0, z0).
  // It is not at the origin of the model coordinate system because the model origin is a camera
  // viewing the image target from dist 1 along z
  HPoint3 targetOriginInModel{0.f, 0.f, 1.f};
  // This is the model origin, which we use to compute the normal for the plane.
  HPoint3 modelOriginPtInModel{0.f, 0.f, 0.f};

  HPoint3 modelOriginPtInCamera = cameraToTarget.inv() * modelOriginPtInModel;
  HPoint3 targetOriginInCamera = cameraToTarget.inv() * targetOriginInModel;

  HVector3 normal = {
    modelOriginPtInCamera.x() - targetOriginInCamera.x(),
    modelOriginPtInCamera.y() - targetOriginInCamera.y(),
    modelOriginPtInCamera.z() - targetOriginInCamera.z(),
  };
  float a = normal.x();
  float b = normal.y();
  float c = normal.z();

  auto x0 = targetOriginInCamera.x();
  auto y0 = targetOriginInCamera.y();
  auto z0 = targetOriginInCamera.z();

  searchPtsInCamera->clear();
  searchPtsInCamera->reserve(raysInCamera.size());
  float n = (a * x0 + b * y0 + c * z0);
  for (int i = 0; i < raysInCamera.size(); ++i) {
    float x = raysInCamera[i].x();
    float y = raysInCamera[i].y();
    float s = n / (a * x + b * y + c);

    searchPtsInCamera->push_back({s * x, s * y, s});
  }
}

Vector<HPoint3> cameraFrameCorners(c8_PixelPinholeCameraModel k, float depth) {
  float imwd = static_cast<float>(k.pixelsWidth - 1) * depth;
  float imht = static_cast<float>(k.pixelsHeight - 1) * depth;
  Vector<HPoint3> corners = {
    {0.0f, 0.0f, depth},
    {0.0f, imht, depth},
    {imwd, imht, depth},
    {imwd, 0.0f, depth},
  };
  auto TK = HMatrixGen::intrinsic(k);
  return TK.inv() * corners;
}

Vector<HPoint2> imageTargetCornerPixels(
  c8_PixelPinholeCameraModel targetK, c8_PixelPinholeCameraModel camK, const HMatrix &camMotion) {
  auto K = HMatrixGen::intrinsic(camK);
  auto corners3 = cameraFrameCorners(targetK, 1.0f);
  auto imCorners = flatten<2>(K * camMotion.inv() * corners3);
  return {
    {imCorners[0].x(), imCorners[0].y()},
    {imCorners[1].x(), imCorners[1].y()},
    {imCorners[2].x(), imCorners[2].y()},
    {imCorners[3].x(), imCorners[3].y()}};
}

HPoint2 imageTargetCenterPixel(c8_PixelPinholeCameraModel camK, const HMatrix &camMotion) {
  return (HMatrixGen::intrinsic(camK) * camMotion.inv() * HPoint3(0.0f, 0.0f, 1.0f)).flatten();
}

Vector<HPoint2> curvyTargetEdgePixels(
  CurvyImageGeometry geom, c8_PixelPinholeCameraModel camK, const HMatrix &camMotion) {
  auto K = HMatrixGen::intrinsic(camK);

  Vector<HPoint2> targetEdgesInTargetPixel;
  for (int i = 0; i < geom.srcCols; i++) {
    HPoint2 p1{static_cast<float>(i), static_cast<float>(0)};
    HPoint2 p2{static_cast<float>(i), static_cast<float>(geom.srcRows)};
    targetEdgesInTargetPixel.push_back(p1);
    targetEdgesInTargetPixel.push_back(p2);
  }
  for (int j = 0; j < geom.srcRows; j++) {
    HPoint2 p1{static_cast<float>(0), static_cast<float>(j)};
    HPoint2 p2{static_cast<float>(geom.srcCols), static_cast<float>(j)};
    targetEdgesInTargetPixel.push_back(p1);
    targetEdgesInTargetPixel.push_back(p2);
  }

  Vector<HPoint3> modelPoints;
  mapToGeometryPoints(geom, targetEdgesInTargetPixel, &modelPoints);
  return flatten<2>(K * camMotion.inv() * modelPoints);
}

// Find point on the curvy that is closest to the camera. The y-coordinate specifies what the height
// on the curvy of the point we are querying. Does not work for conical targets.
HPoint3 closestCurvySurfacePointAtHeight(const HMatrix &cam, CurvyImageGeometry im, float y) {

  auto t = translation(cam);
  // Get the normal of the plane through the curvy's y-axis and the camera. By construction,
  // n.y() is always 0 because this plane that passes through the core of the cone/curvy, which
  // is always the y-axis in both cases.
  auto n = getPlaneNormal(
    {0.0f, im.height * -.5f, 0.0f}, {0.0f, im.height * .5f, 0.0f}, {t.x(), t.y(), t.z()});
  // This plane goes through the origin, so nx * x + ny * y + nz * z = 0; in general, we would need
  // to find an offset parameter.
  //
  // We need to find x/z at y such that
  // nx * x + ny * y + nz * z = 0
  //          nx * x + nz * z = 0  // because ny is 0.
  // and
  // x^2 + z^2 = radiusAtHeight ^2
  //
  // let v = -y * ny
  // solve for x:
  //   x = - nz/nx * z
  //
  // substitute x:
  //   (- nz/nx * z) ^2 + z^2 = topR^2
  //
  // solve for z:
  //   (nz^2/nx^2 + 1) z^2 - radiusAtHeight^2 = 0
  //
  // quadratic formula:
  //   a = (nz^2/nx^2 + 1)
  //   b = 0
  //   c = - radiusAtHeight^2
  //
  // z = +/- sqrt(- 4 * a * c) / (2 * a)
  float r2 = im.radius * im.radius;  // TODO(nb): radius at height
  float nx2 = n.x() * n.x();
  float nz2 = n.z() * n.z();

  // quadratic formula coefficients (see above).
  float a = nz2 / nx2 + 1;
  float c = -r2;
  float ac4 = 4 * a * c;
  float i2a = 0.5f / a;

  // Solve quadratic formula for z.
  float pz = std::sqrt(-ac4) * i2a;
  float nz = -pz;

  // Solve plane formula for x at y / z.
  float px = -pz * n.z() / n.x();
  float nx = -nz * n.z() / n.x();

  // Two solutions
  auto p1 = HPoint3(px, y, pz);
  auto p2 = HPoint3(nx, y, nz);

  // Pick the solution closer to the camera.
  HVector3 delta1 = {p1.x() - t.x(), p1.y() - t.y(), p1.z() - t.z()};
  HVector3 delta2 = {p2.x() - t.x(), p2.y() - t.y(), p2.z() - t.z()};
  auto d1 = delta1.dot(delta1);
  auto d2 = delta2.dot(delta2);
  return d1 < d2 ? p1 : p2;
}

}  // namespace c8
