// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "c8/hmatrix.h"
#include "c8/hpoint.h"
#include "c8/hvector.h"
#include "c8/quaternion.h"
#include "c8/random-numbers.h"
#include "c8/set.h"

namespace c8 {

// Parameters of robust homograhy are exposed for unit tests.
extern float HORIZON_EXPANSION_THRESHOLD;
extern size_t MIN_HOMOGRAPHY_INLIERS;
extern float HOMOGRAPHY_FAILED_RESIDUAL_SQ_DIST;  // 1.414cm err @ 1m
extern size_t MIN_POINTS_FOR_ROBUST_HOMOGRAPHY;
extern size_t MAX_POINT_SAMPLES_FOR_ROBUST_HOMOGRAPHY;

enum HorizonLocation {
  HORIZON_LOCATION_UNSPECIFIED,
  BELOW_HORIZON,
  NEAR_BELOW_HORIZON,
  ON_HORIZON,
  NEAR_ABOVE_HORIZON,
  ABOVE_HORIZON,
};

// Gets the 3d vector cross product of three points.
HVector3 cross3d(HPoint3 x1, HPoint3 x2, HPoint3 x3);

// Check whether two points in 3d are the same.
bool areSame(HPoint3 x1, HPoint3 x2);

// Check whether three points in 3d are collinear.
bool areCollinear(HPoint3 x1, HPoint3 x2, HPoint3 x3);

// Get a matrix that centers and scales 2-d coordinates so that the center is at 0, 0 and the
// average distance from the origin is sqrt(2). This is used to condition points to increase the
// stability of homography computation.
HMatrix centeringTransform(const Vector<HPoint2> &pts);

// Gets a unit vector perpendicular to the plane formed by three points, with the unit normal facing
// in the direction of the origin.
HVector3 getPlaneNormal(HPoint3 x1, HPoint3 x2, HPoint3 x3);

// Gets a vector perpendicular to the plan formed by three points, with the normal scaled by the
// distance to the plane.
HVector3 getScaledPlaneNormal(HPoint3 x1, HPoint3 x2, HPoint3 x3);

// For a known camera motion and 3d plane normal (scaled with getScaledPlaneNormal), computes the
// analytical homography transform that will be induced by the geometry.
HMatrix homographyForPlane(const HMatrix &camDelta, HVector3 planeNormal);

// Returns a rotation matrix that rotates points in the world to be parallel to the ground plane
// from the camera's reference frame.
HMatrix gravityNormalRotation(Quaternion pose);

// Scratch space for allocated structures needed by robust homography, which can be passed in across
// calls to avoid reallocations.
struct RobustHomographyScratchSpace {
  void reset(size_t numHSamples, size_t numPtSamples, size_t numPtsTotal);

  // Things that scale with numHSamples
  Vector<std::pair<size_t, size_t>> hSampleInds;
  Vector<HMatrix> hSamples;
  Vector<HMatrix> cameraMotionSamples;

  // Things that scale with numPtSamples
  Vector<HPoint2> aPtsSample;
  Vector<HPoint2> bPtsSample;
  Vector<float> sampleResiduals;
  Vector<HPoint3> aSampleExtruded;
  Vector<HPoint3> bSampleEstimated3;
  Vector<HPoint2> bSampleEstimated;

  TreeSet<std::pair<size_t, size_t>> ptSampleSet;

  // Things that scale with numPtsTotal
  Vector<size_t> ptSampleInds;
  Vector<HPoint3> aPtsExtruded;
  Vector<HPoint3> bPtsEstimated3;
  Vector<HPoint2> bPtsEstimated;
};

// Compute the plane equation ax + by + cz = d from a set of non-collinear points.
void planeFromThreePoints(HPoint3 const (&a)[3], HVector3 *normal, float *d);

// Compute where points are in the world, assuming they lie on a ground at the provided height; the
// supplied points are in the image ray space for the provided camera.
Vector<HPoint3> triangulatePointsOnGround(
  const HMatrix &camPos, float groundHeight, const Vector<HPoint2> &imPts);

// Gets a the component of cameraOrientation projected onto the ground plane.
Quaternion groundPlaneRotation(Quaternion cameraOrientation);

// Returns true if the translational baseline between two cameras is close to zero; in this case,
// homography should be used to describe the relation of points in a pair of images, and not
// epipolar geometry.
bool isPureCameraRotation(const HMatrix &camPos1, const HMatrix &camPos2);

HMatrix decomposeImageTargetHomographyBuggyPrototype(const HMatrix &H);

}  // namespace c8
