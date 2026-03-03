// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "c8/hmatrix.h"
#include "c8/hpoint.h"
#include "c8/hvector.h"
#include "c8/random-numbers.h"
#include "c8/set.h"

namespace c8 {

using IndexTriple = std::tuple<size_t, size_t, size_t>;
using IndexPair = std::pair<size_t, size_t>;

struct SolvePnPParams {
  size_t minPoseInliers = 7;
  size_t minPointsForRobustPose = 10;
  float poseFailedResidualSqDist = 200e-6;  // 1.414cm err @ 1m
  float cauchyLoss = 1e-2f;
  float cameraMotionWeight = 0.0f;
  float minInlierFraction = 0.0f;  // In addition to minPoseInliers
};

struct RansacParams {
  /// Inital inlier probability guess
  float initialInlierGuess = 0.1f;
  /// Number of data samples per solve.
  size_t numSamplesPerSolve = 2;
  /// Inliear probability.
  float confidence = 0.99f;
  /// Maximum error for an inlier.
  float inlierThreshold = 5e-5f;
  /// Set a maximum number of iterations.
  size_t maxNumIteration = 2500;
  /// Set a minimum number of iterations to try.
  size_t minNumIteration = 100;
  /// Set a minnimum number of inliers to accept a solution.
  size_t minInlierRequired = 10;
  // Minumum baseline for two points to avoid too close to each other
  // Threshold 0.01 is roughly 50 pixels
  float minBaseline2d = 0.01;
  // If count failure of pose proposal or getting mearningful pose as a trial
  // True means we count failure as an iteration
  bool countFailure = true;

  inline size_t computeNeededTrials(float inlierProbability) const {
    const float epsilon = 1e-6f;
    // Adding an epsilon here to avoid denominator going to 1
    const float denom = std::max(
      1.f - static_cast<float>(std::pow(inlierProbability, numSamplesPerSolve)) - epsilon,
      std::numeric_limits<float>::min());
    float neededIterations = std::ceil(log(1.f - confidence) / log(denom));
    neededIterations = std::min(neededIterations, static_cast<float>(maxNumIteration));
    return std::max(static_cast<size_t>(neededIterations), minNumIteration);
  }
};

struct SimulatedAnnealingParams {
  float initialTemperature = 1.0f;
  // Using 0.9847666f to reach 0.1 after 150 iterations
  float coolingRate = 0.9847666f;
};

struct RobustPnPParams {
  float inlierFraction = .35f;
  float failureProbability = 1e-2f;
  size_t minPoseInliers = 7;
  float poseFailedResidualSqDist = 200e-6;  // 1.414cm err @ 1m
  size_t minPointsForRobustPose = 10;
  size_t maxPointSamplesForRobustPose = 100;
  float minZ = -9.0f;
  float maxZ = 0.67f;
  float maxOutOfPlaneRotationDegrees = 45.0f;
  // when true, don't pose-opt and leave the user to do that on the returned inliers
  bool skipFinalPoseOptimization = false;
  // If a point is further than farPointDistance, then we don't consider it when checking against
  // minPointsForRobustPose since it won't help in determining translation for a proposed extrinsic.
  float farPointDistance = 750.0f;
  // NOTE(nathan): this is a hack that we can use to prevent RobustPnP from proposing camera
  // positions underneath the ground.
  float minY = 0.f;
  float maxTranslation = 0.f;
};

struct RobustPnPDebugOut {
  size_t numPtsTotal = 0;
  size_t numClosePts = 0;
  size_t minPointsForRobustPose = 0;
  float bestResidual = -1.f;
  float poseFailedResidualSqDist = -1.f;
};

struct RobustP2PParams {
  // If true, this is for image tracking, otherwise for relocation
  bool isImageTracking = false;
  // when true, don't pose-opt and leave the user to do that on the returned inliers
  bool skipFinalPoseOptimization = false;
  // Ransac parameters
  RansacParams ransac;
  // Simulated Annealing parameters
  SimulatedAnnealingParams simAnn;
};

// Scratch space for allocated structures needed by robust homography, which can be passed in across
// calls to avoid reallocations.
struct RobustPoseScratchSpace {
public:
  void reset(size_t numSamples, size_t numPtSamples, size_t numPtsTotal);
  // Things that scale with numSamples
  Vector<IndexTriple> sampleInds;
  Vector<HMatrix> cameraMotionSamples;

  // Things that scale with numPtSamples
  Vector<HPoint3> aPtsSample;
  Vector<HPoint2> bPtsSample;
  Vector<float> sampleResiduals;
  Vector<HPoint3> bSampleEstimated3;

  TreeSet<IndexTriple> ptSampleSet;
  TreeSet<IndexPair> ptSamplePairSet;

  // Things that scale with numPtsTotal
  Vector<size_t> ptSampleInds;
  Vector<HPoint3> aPtsExtruded;
  Vector<HPoint3> bPtsEstimated3;
  Vector<HPoint2> bPtsEstimated;

private:
  template <class T>
  static void reset(T &t, size_t capacity) {
    t.clear();
    t.reserve(capacity);
  }
};

struct RobustP2PScratchSpace {
  Vector<size_t> inlierSet;
  Vector<size_t> bestInlierSet;
  Vector<size_t> indices;
  Vector<HPoint3> projectedWorldPts;
  Vector<HPoint2> projectedWorldRays;

  TreeSet<IndexPair> usedPairs;
  Vector<HMatrix> poseProposals;

  // Do not clear this:
  Vector<HPoint2> confidenceGrid;

  void reset(size_t numPtsTotal) {
    inlierSet.reserve(numPtsTotal);
    bestInlierSet.reserve(numPtsTotal);
    indices.reserve(numPtsTotal);
    projectedWorldPts.reserve(numPtsTotal);
    projectedWorldRays.reserve(numPtsTotal);

    inlierSet.clear();
    bestInlierSet.clear();
    indices.clear();
    projectedWorldPts.clear();
    projectedWorldRays.clear();

    usedPairs.clear();
    poseProposals.resize(2, HMatrixGen::i());
  }
};

bool solvePnP(
  const Vector<HPoint3> &worldPts,
  const Vector<HPoint2> &camRays,
  SolvePnPParams params,
  HMatrix *camMotion,
  Vector<uint8_t> *inliers,
  RobustPoseScratchSpace *sc);

bool solveImageTargetHomography(
  const Vector<HPoint2> &imTargetRays,
  const Vector<HPoint2> &camRays,
  const Vector<float> &weights,
  SolvePnPParams params,
  HMatrix *camMotion,
  Vector<uint8_t> *inliers,
  RobustPoseScratchSpace *sc);

// @param noFinalPoseOptimization when true, skip final pose optimization using inliers of RANSAC
// result. Used when you are going to perform poseOptimization right after this function anyway.
// @param worldTransform, the tranform to worldSpace if applicable, otherwise pass in identity
bool robustPnP(
  const Vector<HPoint3> &worldPts,
  const Vector<HPoint2> &camRays,
  const HMatrix &worldTransform,
  RobustPnPParams params,
  HMatrix *camMotion,
  Vector<uint8_t> *inliers,
  RandomNumbers *random,
  RobustPoseScratchSpace *sc,
  RobustPnPDebugOut *debugOut = nullptr);

// @param worldPts, map points in world/map coordinate space
// @param trackingPose, tracking pose in xr coordinate space
// @output camMotion, transformation from camera to world/map coordinate space
bool robustP2P(
  const Vector<HPoint3> &worldPts,
  const Vector<HPoint2> &camRays,
  const HPoint2 &lowerLeft,
  const HPoint2 &upperRight,
  const HMatrix &trackingPose,
  const RobustP2PParams &params,
  HMatrix *camMotion,
  float *confidence,
  Vector<uint8_t> *inliers,
  RandomNumbers *random,
  RobustP2PScratchSpace *sc);

}  // namespace c8
