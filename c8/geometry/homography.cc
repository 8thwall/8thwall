// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "homography.h",
  };
  deps = {
    "//c8:c8-log",
    "//c8:hmatrix",
    "//c8:hpoint",
    "//c8:hvector",
    "//c8:quaternion",
    "//c8:random-numbers",
    "//c8:set",
    "//c8/geometry:egomotion",
    "//c8/geometry:vectors",
    "//c8/stats:scope-timer",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xc759c2af);

#include <algorithm>
#include <cfloat>

#include "c8/c8-log.h"
#include "c8/geometry/egomotion.h"
#include "c8/geometry/homography.h"
#include "c8/geometry/vectors.h"
#include "c8/stats/scope-timer.h"

namespace c8 {

namespace {
constexpr float FLOAT_EPS = 1e-5;
}  // namespace

float HORIZON_EXPANSION_THRESHOLD = 0.2f;
size_t MIN_HOMOGRAPHY_INLIERS = 5;
float HOMOGRAPHY_FAILED_RESIDUAL_SQ_DIST = 200e-6;  // 1.414cm err @ 1m
size_t MIN_POINTS_FOR_ROBUST_HOMOGRAPHY = 15;
size_t MAX_POINT_SAMPLES_FOR_ROBUST_HOMOGRAPHY = 100;

HVector3 cross3d(HPoint3 x1, HPoint3 x2, HPoint3 x3) {
  float a1 = x2.x() - x1.x();
  float a2 = x2.y() - x1.y();
  float a3 = x2.z() - x1.z();
  float b1 = x3.x() - x1.x();
  float b2 = x3.y() - x1.y();
  float b3 = x3.z() - x1.z();

  return HVector3(a2 * b3 - a3 * b2, a3 * b1 - a1 * b3, a1 * b2 - a2 * b1);
}

bool areSame(HPoint3 x1, HPoint3 x2) {
  return x1.x() == x2.x() && x1.y() == x2.y() && x1.z() == x2.z();
}

bool areCollinear(HPoint3 x1, HPoint3 x2, HPoint3 x3) {
  HVector3 cp = cross3d(x1, x2, x3);
  float sqnorm = cp.x() * cp.x() + cp.y() * cp.y() + cp.z() * cp.z();
  return sqnorm == 0.0f;
}

HMatrix centeringTransform(const Vector<HPoint2> &pts) {
  float mx = 0.0f;
  float my = 0.0f;
  float norm = 1.0 / pts.size();
  for (auto pt : pts) {
    mx += pt.x();
    my += pt.y();
  }
  mx *= norm;
  my *= norm;

  float dist = 0.0f;
  for (auto pt : pts) {
    float dx = pt.x() - mx;
    float dy = pt.y() - my;
    dist += std::sqrt(dx * dx + dy * dy);
  }

  if (dist <= FLOAT_EPS) {
    return HMatrixGen::i();
  }

  dist *= norm;
  float scale = dist / std::sqrt(2);  // / dist;
  float is = 1.0f / scale;
  float tx = -mx;
  float ty = -my;
  return HMatrix{
    {1.0f, 0.0f, tx, 0.0f},
    {0.0f, 1.0f, ty, 0.0f},
    {0.0f, 0.0f, scale, 0.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
    {1.0f, 0.0f, -tx * is, 0.0f},
    {0.0f, 1.0f, -ty * is, 0.0f},
    {0.0f, 0.0f, is, 0.0f},
    {0.0f, 0.0f, 0.0f, 1.0f}};
}

HVector3 getPlaneNormal(HPoint3 x1, HPoint3 x2, HPoint3 x3) {
  // Three identical points.
  bool x12Same = areSame(x1, x2);
  bool x23Same = areSame(x2, x3);
  if (x12Same && x23Same) {
    return HVector3(0.0f, 1.0f, 0.0f);
  } else if (x23Same) {
    // Make sure that x2 and x3 are not identical.
    std::swap(x1, x3);
  }

  HVector3 cp = cross3d(x1, x2, x3);
  float sqnorm = cp.x() * cp.x() + cp.y() * cp.y() + cp.z() * cp.z();

  // If the points are collinear, pick an axis as the other line for the plane. We have already
  // ensured that x2 and x3 are not identical.
  if (sqnorm == 0.0f) {
    if (areCollinear(HPoint3(1.0f, 0.0f, 0.0f), x2, x3)) {
      if (areCollinear(HPoint3(0.0f, 1.0f, 0.0f), x2, x3)) {
        x1 = HPoint3(0.0f, 0.0f, 1.0f);
      } else {
        x1 = HPoint3(0.0f, 1.0f, 0.0f);
      }
    } else {
      x1 = HPoint3(1.0f, 0.0f, 0.0f);
    }

    cp = cross3d(x1, x2, x3);
    sqnorm = cp.x() * cp.x() + cp.y() * cp.y() + cp.z() * cp.z();
  }

  // Plane: a x + b y + cz + d = 0
  float d = -(x1.x() * cp.x() + x1.y() * cp.y() + x1.z() * cp.z());

  float sign = 1.0f;
  if (d == 0.0f) {
    if (cp.y() == 0.0f) {
      if (cp.z() == 0.0f) {
        sign = cp.x() < 0 ? -1.0f : 1.0f;
      } else {
        sign = cp.z() < 0 ? -1.0f : 1.0f;
      }
    } else {
      sign = cp.y() < 0 ? -1.0f : 1.0f;
    }
  } else {
    sign = d < 0 ? -1.0f : 1.0f;
  }

  float inorm = sign / std::sqrt(sqnorm);

  return HVector3(inorm * cp.x(), inorm * cp.y(), inorm * cp.z());
}

HVector3 getScaledPlaneNormal(HPoint3 x1, HPoint3 x2, HPoint3 x3) {
  // Three identical points.
  bool x12Same = areSame(x1, x2);
  bool x23Same = areSame(x2, x3);
  if (x12Same && x23Same) {
    return HVector3(0.0f, 1.0f, 0.0f);
  } else if (x23Same) {
    // Make sure that x2 and x3 are not identical.
    std::swap(x1, x3);
  }

  HVector3 cp = cross3d(x1, x2, x3);
  float sqnorm = cp.x() * cp.x() + cp.y() * cp.y() + cp.z() * cp.z();

  // If the points are collinear, pick an axis as the other line for the plane. We have already
  // ensured that x2 and x3 are not identical.
  if (sqnorm == 0.0f) {
    if (areCollinear(HPoint3(1.0f, 0.0f, 0.0f), x2, x3)) {
      if (areCollinear(HPoint3(0.0f, 1.0f, 0.0f), x2, x3)) {
        x1 = HPoint3(0.0f, 0.0f, 1.0f);
      } else {
        x1 = HPoint3(0.0f, 1.0f, 0.0f);
      }
    } else {
      x1 = HPoint3(1.0f, 0.0f, 0.0f);
    }

    cp = cross3d(x1, x2, x3);
    sqnorm = cp.x() * cp.x() + cp.y() * cp.y() + cp.z() * cp.z();
  }

  // Plane: a x + b y + cz + d = 0
  float d = (x1.x() * cp.x() + x1.y() * cp.y() + x1.z() * cp.z());
  if (d == 0.0f) {
    d = 1.0f;
  }

  float inorm = 1.0f / d;

  return HVector3(inorm * cp.x(), inorm * cp.y(), inorm * cp.z());
}

HMatrix homographyForPlane(const HMatrix &camDelta, HVector3 planeNormal) {
  auto m = camDelta.inv();
  HVector3 t = HVector3(m(0, 3), m(1, 3), m(2, 3));
  auto n = planeNormal;
  HMatrix unscaled = HMatrix{
    {m(0, 0) + t.x() * n.x(), m(0, 1) + t.x() * n.y(), m(0, 2) + t.x() * n.z(), 0.0f},
    {m(1, 0) + t.y() * n.x(), m(1, 1) + t.y() * n.y(), m(1, 2) + t.y() * n.z(), 0.0f},
    {m(2, 0) + t.z() * n.x(), m(2, 1) + t.z() * n.y(), m(2, 2) + t.z() * n.z(), 0.0f},
    {0.00000000000000000000f, 0.00000000000000000000f, 0.00000000000000000000f, 1.0f}};

  float sqscale = unscaled(0, 0) * unscaled(0, 0) + unscaled(0, 1) * unscaled(0, 1)
    + unscaled(0, 2) * unscaled(0, 2) + unscaled(1, 0) * unscaled(1, 0)
    + unscaled(1, 1) * unscaled(1, 1) + unscaled(1, 2) * unscaled(1, 2)
    + unscaled(2, 0) * unscaled(2, 0) + unscaled(2, 1) * unscaled(2, 1)
    + unscaled(2, 2) * unscaled(2, 2);

  if (sqscale == 0.0f) {
    return HMatrixGen::i();
  }

  float is = 1.0f / std::sqrt(sqscale);
  return HMatrix{
    {is * unscaled(0, 0), is * unscaled(0, 1), is * unscaled(0, 2), 0.0f},
    {is * unscaled(1, 0), is * unscaled(1, 1), is * unscaled(1, 2), 0.0f},
    {is * unscaled(2, 0), is * unscaled(2, 1), is * unscaled(2, 2), 0.0f},
    {0.0000000000000000f, 0.0000000000000000f, 0.0000000000000000f, 1.0f}};
}

HMatrix gravityNormalRotation(Quaternion pose) {
  // To rotate points such that that are downward, we need to compute the egocentric rotation to
  // make that happen, and then invert it.
  return egomotion(pose.toRotationMat(), HMatrixGen::rotationD(90.0f, 0.0f, 0.0f)).inv();
}

namespace {

void homographyGroundPlane(
  const HMatrix &QA,  // gravity normal rotation
  HPoint2 const (&as)[2],
  const HMatrix &QB,  // gravity normal rotation
  HPoint2 const (&bs)[2],
  HMatrix *homography,
  HMatrix *camMotion) {
  *homography = HMatrixGen::i();
  *camMotion = HMatrixGen::i();
  // Warp points to be on the ground in both images.
  auto as3 = QA * extrude<3>(Vector<HPoint2>{as[0], as[1]});
  auto bs3 = QB * extrude<3>(Vector<HPoint2>{bs[0], bs[1]});
  auto as2 = flatten<2>(as3);
  auto bs2 = flatten<2>(bs3);

  // If points are too close to the horizon, we can't reliably estimate their location on the
  // ground.
  if (
    as3[0].z() <= HORIZON_EXPANSION_THRESHOLD || bs3[0].z() <= HORIZON_EXPANSION_THRESHOLD
    || as3[1].z() <= HORIZON_EXPANSION_THRESHOLD || bs3[1].z() <= HORIZON_EXPANSION_THRESHOLD) {
    return;
  }

  // Compute the homography for a plane facing the camera.
  //-----------------------------------------------------------------------------------------------

  if (
    (as2[0].x() == as2[1].x() && as2[0].y() == as2[1].y())
    || (bs2[0].x() == bs2[1].x() && bs2[0].y() == bs2[1].y())) {
    return;
  }

  // Compute centering transform for a0 and a1.
  HMatrix Ta = HMatrixGen::i();
  {
    float mx = (as2[0].x() + as2[1].x()) * .5;
    float my = (as2[0].y() + as2[1].y()) * .5;

    float dx = as2[0].x() - mx;
    float dy = as2[0].y() - my;
    float scale = 0.70710678118f * std::sqrt(dx * dx + dy * dy);

    if (scale <= FLOAT_EPS) {
      return;
    }

    float is = 1.0f / scale;
    Ta = HMatrix{
      {1.0f, 0.0f, -mx, 0.0f},
      {0.0f, 1.0f, -my, 0.0f},
      {0.0f, 0.0f, scale, 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f},
      {1.0f, 0.0f, mx * is, 0.0f},
      {0.0f, 1.0f, my * is, 0.0f},
      {0.0f, 0.0f, is, 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f}};
  }

  // Compute centering transform for b0 and b1.
  HMatrix Tb = HMatrixGen::i();
  {
    float mx = (bs2[0].x() + bs2[1].x()) * .5;
    float my = (bs2[0].y() + bs2[1].y()) * .5;

    float dx = bs2[0].x() - mx;
    float dy = bs2[0].y() - my;
    float scale = 0.70710678118f * std::sqrt(dx * dx + dy * dy);

    if (scale <= FLOAT_EPS) {
      return;
    }

    float is = 1.0f / scale;
    Tb = HMatrix{
      {1.0f, 0.0f, -mx, 0.0f},
      {0.0f, 1.0f, -my, 0.0f},
      {0.0f, 0.0f, scale, 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f},
      {1.0f, 0.0f, mx * is, 0.0f},
      {0.0f, 1.0f, my * is, 0.0f},
      {0.0f, 0.0f, is, 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f}};
  }

  // Centered points so that they lie on two lines with the same size, centered at the origin. Now
  // we just need to compute the roll.
  auto aCentered = (Ta * as3[0]).flatten();
  auto bCentered = (Tb * bs3[0]).flatten();

  // Compute the angle with respect to the axis for line a, the angle with respect to the axis for
  // line b, and subtract the angles.
  auto thetaA = std::atan2(aCentered.x(), aCentered.y());
  auto thetaB = std::atan2(bCentered.x(), bCentered.y());
  auto theta = thetaB - thetaA;

  HMatrix rotation = HMatrixGen::zRadians(theta);

  // Scale and translate the roll-only motions by undoing the centering transforms.
  HMatrix hs = Tb.inv() * rotation.inv() * Ta;

  // Compute the h & m, still assuming the camera is looking straight donw.
  float inorm = 1.0f / std::sqrt(hs(0, 0) * hs(0, 0) + hs(0, 1) * hs(0, 1));
  HMatrix h = HMatrix{
    {hs(0, 0) * inorm, hs(0, 1) * inorm, hs(0, 2) * inorm, 0.0f},
    {hs(1, 0) * inorm, hs(1, 1) * inorm, hs(1, 2) * inorm, 0.0f},
    {hs(2, 0) * inorm, hs(2, 1) * inorm, hs(2, 2) * inorm, 0.0f},
    {0.0000000000000f, 0.0000000000000f, 0.0000000000000f, 1.0f},
    true /* noInvert */};
  HMatrix m = rotation * HMatrixGen::translation(-h(0, 2), -h(1, 2), 1 - h(2, 2));

  //-----------------------------------------------------------------------------------------------

  // Unrotate the homography facing the camera.
  *homography = QB.inv() * h * QA;
  *camMotion = QA.inv() * m * QB;
}

}  // namespace

void homographyGroundPlane(
  Quaternion poseA,
  HPoint2 const (&as)[2],
  Quaternion poseB,
  HPoint2 const (&bs)[2],
  HMatrix *homography,
  HMatrix *camMotion) {
  // Gravity normal rotation.
  HMatrix QA = HMatrixGen::xDegrees(-90.0f) * poseA.toRotationMat();
  HMatrix QB = HMatrixGen::xDegrees(-90.0f) * poseB.toRotationMat();
  return homographyGroundPlane(QA, as, QB, bs, homography, camMotion);
}

// Estimates a homography from a pair of points known to be on a plane parralel to the x-y axis. The
// points are allowed to translate freely, and possibly roll, between frames. The returned
// homography is scaled so that the points in the first view are at distance 1 from the camera.
void homographyFacingCamera(
  HPoint2 const (&a)[2], HPoint2 const (&b)[2], HMatrix *homography, HMatrix *camMotion) {
  if (
    (a[0].x() == a[1].x() && a[0].y() == a[1].y())
    || (b[0].x() == b[1].x() && b[0].y() == b[1].y())) {
    *homography = HMatrixGen::i();
    *camMotion = HMatrixGen::i();
    return;
  }
  Vector<HPoint2> as2{a[0], a[1]};
  Vector<HPoint2> bs2{b[0], b[1]};

  HMatrix Ta = centeringTransform(as2);
  HMatrix Tb = centeringTransform(bs2);

  // Centering points gives appropriate weight to the homogenous coordinate.
  Vector<HPoint2> aCentered = flatten<2>(Ta * extrude<3>(as2));
  Vector<HPoint2> bCentered = flatten<2>(Tb * extrude<3>(bs2));

  // The centered points lie on two lines with the same size, centered at the origin. Now we just
  // need to compute the roll.
  auto ax = aCentered[0].x();
  auto ay = aCentered[0].y();
  auto bx = bCentered[0].x();
  auto by = bCentered[0].y();

  auto thetaA = std::atan2(ax, ay);
  auto thetaB = std::atan2(bx, by);
  auto theta = thetaB - thetaA;

  HMatrix rotation = HMatrixGen::zRadians(theta);
  HMatrix hr = homographyForPlane(rotation, HVector3(0.0f, 0.0f, 1.0f));
  HMatrix hs = Tb.inv() * hr * Ta;

  float inorm = 1.0f / std::sqrt(hs(0, 0) * hs(0, 0) + hs(0, 1) * hs(0, 1));
  *homography = HMatrix{
    {hs(0, 0) * inorm, hs(0, 1) * inorm, hs(0, 2) * inorm, 0.0f},
    {hs(1, 0) * inorm, hs(1, 1) * inorm, hs(1, 2) * inorm, 0.0f},
    {hs(2, 0) * inorm, hs(2, 1) * inorm, hs(2, 2) * inorm, 0.0f},
    {0.0000000000000f, 0.0000000000000f, 0.0000000000000f, 1.0f}};
  *camMotion = rotation
    * HMatrixGen::translation((*homography)(0, 2), (*homography)(1, 2), (*homography)(2, 2) - 1)
        .inv();
}

template <class T>
void reset(T &t, size_t capacity) {
  t.clear();
  t.reserve(capacity);
}

void RobustHomographyScratchSpace::reset(
  size_t numHSamples, size_t numPtSamples, size_t numPtsTotal) {
  // Things that scale with numHSamples
  c8::reset(hSampleInds, numHSamples);
  c8::reset(hSamples, numHSamples);
  c8::reset(cameraMotionSamples, numHSamples);

  // Things that scale with numPtSamples
  c8::reset(aPtsSample, numPtSamples);
  c8::reset(bPtsSample, numPtSamples);
  c8::reset(sampleResiduals, numPtSamples);
  c8::reset(aSampleExtruded, numPtSamples);
  c8::reset(bSampleEstimated3, numPtSamples);
  c8::reset(bSampleEstimated, numPtSamples);

  ptSampleSet.clear();

  // Things that scale with numPtsTotal
  c8::reset(ptSampleInds, numPtsTotal);
  c8::reset(aPtsExtruded, numPtsTotal);
  c8::reset(bPtsEstimated3, numPtsTotal);
  c8::reset(bPtsEstimated, numPtsTotal);
}

// Compute the plane equation ax + by + cz = d from a set of non-collinear points.
void planeFromThreePoints(HPoint3 const (&p)[3], HVector3 *normal, float *d) {
  HVector3 pv1(p[1].x() - p[0].x(), p[1].y() - p[0].y(), p[1].z() - p[0].z());
  HVector3 pv2(p[2].x() - p[0].x(), p[2].y() - p[0].y(), p[2].z() - p[0].z());
  *normal = pv1.cross(pv2);
  *d = normal->x() * p[0].x() + normal->y() * p[0].y() + normal->z() * p[0].z();
}

// Given rays above the horizon, return their 3D ray-sphere intersected position against a large
// encompassing sphere around the camera.
Vector<HPoint3> triangulatePointsOnSky(
  const HMatrix &camPos, float skyboxDistance, const Vector<HPoint2> &aboveHorizonRays) {
  // Find the location of points on the world's sky in the camera's reference frame.
  Vector<HPoint3> ptsOnSphereInCamera;
  ptsOnSphereInCamera.reserve(aboveHorizonRays.size());
  for (const auto &ray : aboveHorizonRays) {
    ptsOnSphereInCamera.push_back(
      asPoint(HVector3{ray.x(), ray.y(), 1.0f}.unit() * skyboxDistance));
  }
  return camPos * ptsOnSphereInCamera;
}

Vector<HPoint3> triangulatePointsOnGround(
  const HMatrix &camPos, float groundHeight, const Vector<HPoint2> &imPts) {
  // Find the location of points on the world ground in the camera's reference frame.
  Vector<HPoint3> groundPtsInWorld{
    HPoint3(0.0f, groundHeight, 0.0f),
    HPoint3(1.0f, groundHeight, 0.0f),
    HPoint3(0.0f, groundHeight, 1.0f)};

  // Transform ground point plane from world space to camera space.
  auto groundPtsInCameraSpace = camPos.inv() * groundPtsInWorld;

  // Get the plane equation from the three points.
  HVector3 n;
  float d;
  planeFromThreePoints(
    {groundPtsInCameraSpace[0], groundPtsInCameraSpace[1], groundPtsInCameraSpace[2]}, &n, &d);

  // Compute where each image ray intersects the specified plane.
  auto reconstructions = extrude<3>(imPts);
  Vector<HPoint3> estPts;
  estPts.reserve(reconstructions.size());
  for (auto r : reconstructions) {
    // Now we have sax + sby + scz = d and we want to solve for s.
    float den = n.x() * r.x() + n.y() * r.y() + n.z() * r.z();
    float rScale = den == 0 ? d : d / den;
    HPoint3 camPt3(r.x() * rScale, r.y() * rScale, r.z() * rScale);
    estPts.push_back(camPos * camPt3);
  }
  return estPts;
}

Quaternion groundPlaneRotation(Quaternion cameraOrientation) {
  return Quaternion::fromHMatrix(HMatrixGen::yRadians(groundPlaneRotationRads(cameraOrientation)));
}

// Rotates devicePose by some y rotation so that it's facing the same y-direction as the computed
// pose.
Quaternion posePlaneRotation(Quaternion computedPose, Quaternion devicePose) {
  auto computedYaw = groundPlaneRotationRads(computedPose);
  auto deviceYaw = groundPlaneRotationRads(devicePose);
  return Quaternion::yRadians(computedYaw - deviceYaw).times(devicePose);
}

HMatrix correctPoseForGravity(const HMatrix &proposed, Quaternion pose) {
  HMatrix translation = translationMat(proposed);
  auto rotationFixed = posePlaneRotation(rotation(proposed), pose);
  return translation * rotationFixed.toRotationMat();
}

bool isPureCameraRotation(const HMatrix &camPos1, const HMatrix &camPos2) {
  auto tr = translation(egomotion(camPos1, camPos2));
  // Set .5cm (0.005) as the minimum baseline for computing epipolar lines.
  return tr.dot(tr) <= 2.5e-5;
}

HMatrix homographyForCameraRotation(const HMatrix &camPos1, const HMatrix &camPos2) {
  return rotationMat(egomotion(camPos1, camPos2)).inv();
}

float distanceFromHomography(const HMatrix &homography, HPoint2 ptCam1, HPoint2 ptCam2) {
  auto projected = (homography * ptCam1.extrude()).flatten();
  auto xd = projected.x() - ptCam2.x();
  auto yd = projected.y() - ptCam2.y();
  return std::sqrt(xd * xd + yd * yd);
}

namespace {
constexpr float clip(float x) { return x < 0.0f ? 0.0f : (x > 1.0f ? 1.0f : x); }
}  // namespace

HMatrix decomposeImageTargetHomographyBuggyPrototype(const HMatrix &H) {
  // Prototype for custom homography decomposition for norm={0.0f, 0.0f, 1.0f};
  // WARNING: this code does not yet handle all edge cases robustly.

  // H = s * (R + t' * n)
  //
  // In the case where we have n = (0, 0, 1), we have:
  //
  //          r00  r01  r02+tx
  // is * H = r10  r11  r12+ty
  //          r20  r21  r22+tz
  //
  // Compute the scale factor s for H so that is * H = HS = (R + t'n). We do this by making use of
  // the observation that R * R' = I = R' * R, so in the case where n = {0, 0, 1}, we have
  // 1 = H[0, 0]^2 + H[1, 0]^2 + H[2, 0]^2 and 1 = H[0, 1]^2 + H[1, 1]^2 + H[2, 1]^2.
  auto s1 = std::sqrt(H(0, 0) * H(0, 0) + H(1, 0) * H(1, 0) + H(2, 0) * H(2, 0));
  auto s2 = std::sqrt(H(0, 1) * H(0, 1) + H(1, 1) * H(1, 1) + H(2, 1) * H(2, 1));
  auto s = 0.5f * (s1 + s2);
  auto is = 1.0f / s;

  // Scale the homography matrix.
  HMatrix HS{
    {is * H(0, 0), is * H(0, 1), is * H(0, 2), 0.0f},
    {is * H(1, 0), is * H(1, 1), is * H(1, 2), 0.0f},
    {is * H(2, 0), is * H(2, 1), is * H(2, 2), 0.0f},
    {0.000000000f, 0.000000000f, 0.000000000f, 1.0f},
  };

  // Compute the rotation matrix components for the third column, except for their signs, by making
  // use of the R * R' = I constraint, i.e.
  // 1 = R[0, 0] ^ 2 + R[0, 1]^2 + x^2
  // 1 = R[1, 0] ^ 2 + R[1, 1]^2 + y^2
  // 1 = R[2, 0] ^ 2 + R[2, 1]^2 + z^2
  auto r02 = std::sqrt(1.0f - clip(HS(0, 0) * HS(0, 0) + HS(0, 1) * HS(0, 1)));
  auto r12 = std::sqrt(1.0f - clip(HS(1, 0) * HS(1, 0) + HS(1, 1) * HS(1, 1)));
  auto r22 = std::sqrt(1.0f - clip(HS(2, 0) * HS(2, 0) + HS(2, 1) * HS(2, 1)));

  // Compute the relative signs of the rotation matrix third column by making use of R * R' = I,
  // 0 = R[0, 0] * R[1, 0] + R[0, 1] * R[1, 1] + xy
  // 0 = R[0, 0] * R[2, 0] + R[0, 1] * R[2, 1] + xz
  // 0 = R[1, 0] * R[2, 0] + R[1, 1] * R[2, 1] + yz
  std::array<float, 3> signs{1.0f, 1.0f, 1.0f};
  if (r02 > 1e-3) {
    signs[1] *= (HS(0, 0) * HS(1, 0) + HS(0, 1) * HS(1, 1)) > 0.0f ? -1.0f : 1.0f;
    signs[2] *= (HS(0, 0) * HS(2, 0) + HS(0, 1) * HS(2, 1)) > 0.0f ? -1.0f : 1.0f;
  } else {
    signs[2] *= (HS(1, 0) * HS(2, 0) + HS(1, 1) * HS(2, 1)) > 0.0f ? -1.0f : 1.0f;
  }

  // Now there is a single sign ambiguity. Compute a hypothetical rotation matrix assuming that R20
  // is positive.
  auto R = HMatrix{
    {HS(0, 0), HS(0, 1), signs[0] * r02, 0.0f},
    {HS(1, 0), HS(1, 1), signs[1] * r12, 0.0f},
    {HS(2, 0), HS(2, 1), signs[2] * r22, 0.0f},
    {0.00000f, 0.00000f, 0.00000000000f, 1.0f}};

  // Use the property that determinant(R) = 1 to check if we need to switch the signs.
  if (R.determinant() < 0) {
    R = HMatrix{
      {HS(0, 0), HS(0, 1), -signs[0] * r02, 0.0f},
      {HS(1, 0), HS(1, 1), -signs[1] * r12, 0.0f},
      {HS(2, 0), HS(2, 1), -signs[2] * r22, 0.0f},
      {0.00000f, 0.00000f, 0.000000000000f, 1.0f}};
  }

  // Convert R to a quaternion to force it to be a numerically valid rotation matrix.
  auto r = Quaternion::fromHMatrix(R);
  R = r.toRotationMat();

  // Extract the translation according to
  //
  //      r00  r01  r02+tx
  // HS = r10  r11  r12+ty
  //      r20  r21  r22+tz
  HPoint3 t(HS(0, 2) - R(0, 2), HS(1, 2) - R(1, 2), HS(2, 2) - R(2, 2));

  // Compute the fianl camera motion.
  auto finalMotion = cameraMotion(t, r);
  return finalMotion;
}

}  // namespace c8
