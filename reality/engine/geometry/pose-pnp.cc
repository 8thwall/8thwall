// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "pose-pnp.h",
  };
  deps = {
    ":ap3p",
    ":bundle",
    ":p2p",
    ":reprojection",
    "//c8:c8-log",
    "//c8:hmatrix",
    "//c8:hpoint",
    "//c8:hvector",
    "//c8:parameter-data",
    "//c8:random-numbers",
    "//c8:set",
    "//c8/geometry:egomotion",
    "//c8/geometry:homography",
    "//c8/stats:scope-timer",
    "//reality/engine/binning:linear-bin",
  };
}
cc_end(0x4ed37b11);

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <numeric>

#include "c8/c8-log.h"
#include "c8/geometry/egomotion.h"
#include "c8/geometry/homography.h"
#include "c8/parameter-data.h"
#include "c8/random-numbers.h"
#include "c8/stats/scope-timer.h"
#include "reality/engine/binning/linear-bin.h"
#include "reality/engine/geometry/ap3p.h"
#include "reality/engine/geometry/bundle.h"
#include "reality/engine/geometry/p2p.h"
#include "reality/engine/geometry/pose-pnp.h"
#include "reality/engine/geometry/reprojection.h"

namespace c8 {
namespace {
#ifdef NDEBUG
constexpr static int debug_ = 0;
#else
constexpr static int debug_ = 1;
#endif

struct Settings {
  bool solveOptimalPoseUseAnalytical;  // If true, solveOptimalPose() uses poseEstimationAnalytical,
                                       // else it uses poseEstimationSchur. Note this is currently
                                       // called just by curvy image targets.
  float relocalizationConfidenceThreshold;  // Confidence threshold for robustP2P, if 0.f, it will
                                            // not be actually used.
  float imageTrackingConfidenceThreshold;   // Confidence threshold for robustP2P in image tracking.
  float confidenceAreaRadius;
};

const Settings &settings() {
  static const Settings settings_{
    globalParams().getOrSet("PosePnP.solveOptimalPoseUseAnalytical", false),
    globalParams().getOrSet("PosePnP.relocalizationConfidenceThreshold", 0.3f),
    globalParams().getOrSet("PosePnP.imageTrackingConfidenceThreshold", 0.15f),
    // Radius in ray space around inlier points to consider during pose confidence
    globalParams().getOrSet("PosePnP.confidenceAreaRadius", 0.15f),
  };
  return settings_;
}

inline float getPointDepth(const HMatrix &worldToCam, const HPoint3 &worldPt) {
  // The z component of the world point in camera space, since camera is z-forward.
  return worldToCam(2, 0) * worldPt.x() + worldToCam(2, 1) * worldPt.y()
    + worldToCam(2, 2) * worldPt.z() + worldToCam(2, 3);
}

void sampleIndexTriples(
  int numPtsTotal,
  int neededSamples,
  RandomNumbers *random,
  Vector<IndexTriple> *sampleInds,
  TreeSet<IndexTriple> *ptSampleSet) {
  sampleInds->clear();
  ptSampleSet->clear();

  size_t maxSamples = (numPtsTotal - 2) * (numPtsTotal - 1) * numPtsTotal / 6;  // n choose 3

  // Generate the set of pairs to sample for computing pose.
  if (maxSamples <= 5 * neededSamples) {
    // Enumerate all pairs and then truncate to the desired amount.
    for (size_t i = 0; i < numPtsTotal - 2; ++i) {
      for (size_t j = i + 1; j < numPtsTotal - 1; ++j) {
        for (size_t k = j + 1; k < numPtsTotal; ++k) {
          sampleInds->push_back({i, j, k});
        }
      }
    }

    random->shuffle(sampleInds->begin(), sampleInds->end());
    sampleInds->resize(neededSamples);
  } else {
    // Sample unique pairs.
    while (ptSampleSet->size() < neededSamples) {
      size_t r1 = random->nextUnsignedInt() % numPtsTotal;
      size_t r2 = random->nextUnsignedInt() % numPtsTotal;
      size_t r3 = random->nextUnsignedInt() % numPtsTotal;
      // Run the iteration again if the point is paired with itself.
      if (r1 == r2 || r1 == r3 || r2 == r3) {
        continue;
      }
      // Avoid duplicates with reversed order by always putting the lower index first.
      if (r2 < r1) {
        std::swap(r1, r2);
      }
      if (r3 < r2) {
        std::swap(r2, r3);
        if (r2 < r1) {
          std::swap(r1, r2);
        }
      }
      // Insert is a no-op if the pair is already present, so this iteration will get run again.
      ptSampleSet->insert({r1, r2, r3});
    }
    // Convert the set to a vector with random access.
    for (auto p : *ptSampleSet) {
      sampleInds->push_back(p);
    }
  }
}

void sampleDataset(
  const Vector<HPoint3> &worldPts,
  const Vector<HPoint2> &camRays,
  int neededPtSamples,
  Vector<HPoint3> *aPtsSample,
  Vector<HPoint2> *bPtsSample,
  Vector<size_t> *ptSampleInds,
  RandomNumbers *rng) {
  int numPtsTotal = worldPts.size();

  aPtsSample->clear();
  bPtsSample->clear();

  // Sample the points that will be used to evaluate the pairs.
  if (numPtsTotal <= neededPtSamples) {
    for (size_t i = 0; i < numPtsTotal; ++i) {
      aPtsSample->push_back(worldPts[i]);
      bPtsSample->push_back(camRays[i]);
    }
  } else {
    // Shuffle the indices of the full list, and then take the first neededPtSamples.
    for (size_t i = 0; i < numPtsTotal; ++i) {
      ptSampleInds->push_back(i);
    }
    rng->shuffle(ptSampleInds->begin(), ptSampleInds->end());

    for (size_t i = 0; i < neededPtSamples; ++i) {
      aPtsSample->push_back(worldPts[ptSampleInds->at(i)]);
      bPtsSample->push_back(camRays[ptSampleInds->at(i)]);
    }
  }
}

bool sampleUniqueIndexPairWithTemperature(
  const size_t numPtsTotal,
  const Vector<size_t> &inliers,
  const TreeSet<IndexPair> &usedPairs,
  const float temperature,
  RandomNumbers *random,
  IndexPair *sampledPair) {
  if (inliers.size() < 2) {
    // There should always be at least 2 inliers with P2P
    return false;
  }

  int attempts;
  // Empirically, 50th/90th/99th percentile are 1/5/14 attempts so 20 is reasonable.
  for (attempts = 0; attempts < 20; attempts++) {
    // Low temperature means we are more likely to sample from the inliers.
    bool sampleFromInliers = random->nextUniform32f() > temperature;

    auto first = sampleFromInliers ? inliers[random->nextUniformInt(0, inliers.size())]
                                   : random->nextUniformInt(0, numPtsTotal);

    auto second = sampleFromInliers ? inliers[random->nextUniformInt(0, inliers.size())]
                                    : random->nextUniformInt(0, numPtsTotal);

    if (first > second) {
      std::swap(first, second);
    }

    *sampledPair = {first, second};
    if (first != second && !usedPairs.contains(*sampledPair)) {
      return true;
    }
  }
  return false;
}

void proposePoses(
  const Vector<HPoint3> &worldPts,
  const Vector<HPoint2> &camRays,
  const Vector<IndexTriple> &sampleInds,
  const HMatrix &worldTransform,
  RobustPnPParams params,
  Vector<HMatrix> *cameraMotionSamples) {
  ScopeTimer t1("propose-pose");
  cameraMotionSamples->clear();

  auto maxTranslationSq = params.maxTranslation * params.maxTranslation;
  HMatrix imat = HMatrixGen::i();
  bool pushedIdentity = false;
  HPoint3 rtest{0.0f, 0.0f, 1.0f};
  auto rz = (HMatrixGen::xDegrees(params.maxOutOfPlaneRotationDegrees) * rtest).z();
  for (size_t i = 0; i < sampleInds.size(); ++i) {
    auto inds = sampleInds[i];
    auto i0 = std::get<0>(inds);
    auto i1 = std::get<1>(inds);
    auto i2 = std::get<2>(inds);
    auto poses = ap3p::solve(
      std::array<HPoint3, 3>{worldPts[i0], worldPts[i1], worldPts[i2]},
      std::array<HPoint2, 3>{camRays[i0], camRays[i1], camRays[i2]});
    for (int j = 0; j < 4; ++j) {
      const auto &p = poses[j];

      if (p.data() == imat.data()) {
        if (!pushedIdentity) {
          pushedIdentity = true;
        } else {
          continue;
        }
      }
      auto z = p(2, 3);

      auto poseTranslation = translation(p);
      if (params.maxTranslation && poseTranslation.dot(poseTranslation) > maxTranslationSq) {
        continue;
      }

      // This will help prevent proposing poses underneath the ground.
      // Convert y from target space to worldspace.
      if (params.minY && (worldTransform * poseTranslation).y() < params.minY) {
        continue;
      }

      if (params.maxZ && z > params.maxZ) {
        continue;
      }

      if (params.minZ && z < params.minZ) {
        continue;
      }

      if (params.maxOutOfPlaneRotationDegrees && (rotationMat(p) * rtest).z() < rz) {
        continue;
      }

      cameraMotionSamples->push_back(p);
    }
  }
}

void findBestProposal(
  const Vector<HPoint3> &aPtsSample,
  const Vector<HPoint2> &bPtsSample,
  const Vector<HMatrix> &cameraMotionSamples,
  RobustPnPParams params,
  HMatrix *bestCamMotion,
  float *bestResidual,
  Vector<HPoint3> *bSampleEstimated3,
  Vector<float> *sampleResiduals) {

  // For each sample, compute a homography and the residual-at-percentile, keeping the best.
  // Compute the LMedS n for inlier cutoff.
  size_t residualN = aPtsSample.size() * params.inlierFraction;
  if (residualN < params.minPoseInliers) {
    residualN = std::min(params.minPoseInliers - 1, aPtsSample.size() - 1);
  }
  *bestResidual = FLT_MAX;
  *bestCamMotion = HMatrixGen::i();
  {
    ScopeTimer t1("fit-trios");

    sampleResiduals->resize(aPtsSample.size());
    for (size_t i = 0; i < cameraMotionSamples.size(); ++i) {
      HMatrix motion = cameraMotionSamples[i];

      // Compute the estimated b position from the homography.
      motion.inv().times(aPtsSample, bSampleEstimated3);

      // Compute the square difference between where we predicted the point to be, and where it
      // actually was.
      // We don't clear sampleResiduals because we are overwriting all the elements
      for (int r = 0; r < aPtsSample.size(); ++r) {
        auto &bSampleEstimated3AtR = (*bSampleEstimated3)[r];
        float xd = bSampleEstimated3AtR.x() / bSampleEstimated3AtR.z() - bPtsSample[r].x();
        float yd = bSampleEstimated3AtR.y() / bSampleEstimated3AtR.z() - bPtsSample[r].y();
        (*sampleResiduals)[r] = xd * xd + yd * yd;
      }

      // Find the nth best residual.
      std::nth_element(
        sampleResiduals->begin(), sampleResiduals->begin() + residualN, sampleResiduals->end());
      float residual = sampleResiduals->at(residualN);

      // If it's best so far, keep it.
      if (residual < *bestResidual) {
        *bestResidual = residual;
        *bestCamMotion = motion;
      }
    }
  }
}

void fillInliers(
  const Vector<HPoint3> &worldPts,
  const Vector<HPoint2> &camRays,
  const HMatrix &bestCamMotion,
  float bestResidual,
  Vector<uint8_t> *inliers,
  Vector<HPoint3> *bPtsEstimated3,
  Vector<HPoint2> *bPtsEstimated) {
  // Fill the mask of inliers, which are points that appear to match the estimated motion.
  bestCamMotion.inv().times(worldPts, bPtsEstimated3);
  flatten<2>(*bPtsEstimated3, bPtsEstimated);

  // Mark inliners.
  for (int i = 0; i < worldPts.size(); ++i) {
    float xd = bPtsEstimated->at(i).x() - camRays[i].x();
    float yd = bPtsEstimated->at(i).y() - camRays[i].y();
    // Since the residual above was computed with a sample of points, it might be an overfit;
    // raise the tolerance for inliers by ~10%.
    (*inliers)[i] = xd * xd + yd * yd <= bestResidual * 1.1;
  }
}

void solveOptimalPose(
  const Vector<HPoint3> &worldPts,
  const Vector<HPoint2> &camRays,
  const Vector<uint8_t> &inliers,
  HMatrix *camMotion,
  float cameraMotionWeight) {
  // Further optimize camera position with bundle adjustment.
  Vector<ObservedPoint> o;
  Vector<HPoint3> inlierWorldPts;
  o.reserve(worldPts.size());
  inlierWorldPts.reserve(worldPts.size());
  for (int i = 0; i < worldPts.size(); ++i) {
    if (!inliers[i]) {
      continue;
    }
    o.push_back({{camRays[i]}, 0 /* scale */, 0.0f /* descDist */, 1e0f /* weight */});
    inlierWorldPts.push_back(worldPts[i]);
  }

  if (settings().solveOptimalPoseUseAnalytical) {
    poseEstimationAnalytical(
      o, inlierWorldPts, {}, camMotion, {true, 500.0, true, cameraMotionWeight});
  } else {
    poseEstimationSchur(o, inlierWorldPts, cameraMotionWeight, camMotion, false);
  }
}

bool hasCloseInlier(const Vector<HPoint2> &inlierRays, const HPoint2 &pt, const BinMap &binMap) {
  static Vector<size_t> bins;
  float r2 = settings().confidenceAreaRadius * settings().confidenceAreaRadius;
  binMap.binsForPoint({pt.x(), pt.y()}, settings().confidenceAreaRadius, &bins);
  for (auto bin : bins) {
    for (auto idx : binMap.map_[bin]) {
      if (inlierRays[idx].sqdist(pt) < r2) {
        return true;
      }
    }
  }
  return false;
}

constexpr int GRID_X_STEPS = 25;

// Generates grid of uniformly distributed points within a specified rectangular area
Vector<HPoint2> uniformSampleGrid(const HPoint2 &lowerLeft, const HPoint2 &upperRight) {
  Vector<HPoint2> grid;
  // Use a step size that gives GRID_X_STEP = 25 samples along the x-axis
  float stepSize = (upperRight.x() - lowerLeft.x()) / GRID_X_STEPS;
  int xSteps = std::ceil((upperRight.x() - lowerLeft.x()) / stepSize);
  int ySteps = std::ceil((upperRight.y() - lowerLeft.y()) / stepSize);

  for (int i = 0; i < xSteps; ++i) {
    for (int j = 0; j < ySteps; ++j) {
      grid.push_back(
        {lowerLeft.x() + (i + 0.5f) * stepSize, lowerLeft.y() + (j + 0.5f) * stepSize});
    }
  }
  return grid;
}

inline float computePoseConfidence(
  const Vector<HPoint2> &camRays,
  const HPoint2 &lowerLeft,
  const HPoint2 &upperRight,
  const Vector<size_t> &inlierIds,
  const Vector<HPoint2> &sampleGrid,
  float confidenceThreshold,
  BinMap *binMap) {
  ScopeTimer t("compute-pose-confidence");
  static Vector<HPoint2> inlierRays;
  inlierRays.resize(inlierIds.size());
  for (size_t i = 0; i < inlierIds.size(); ++i) {
    inlierRays[i] = camRays[inlierIds[i]];
  }

  binMap->reset<HPoint2>(lowerLeft.x(), lowerLeft.y(), upperRight.x(), upperRight.y(), inlierRays);

  int count = 0;
  auto minCount = static_cast<int>(confidenceThreshold * sampleGrid.size());

  for (int i = 0; i < sampleGrid.size(); ++i) {
    if (count + sampleGrid.size() - i < minCount) {
      // Early exit when even if all remaining points are close to inliers, there will not be enough
      return 0.0f;
    }

    if (hasCloseInlier(inlierRays, sampleGrid[i], *binMap)) {
      ++count;
    }
  }

  return static_cast<float>(count) / static_cast<float>(sampleGrid.size());
}

}  // namespace

void RobustPoseScratchSpace::reset(size_t numSamples, size_t numPtSamples, size_t numPtsTotal) {
  // Things that scale with numHSamples
  RobustPoseScratchSpace::reset(sampleInds, numSamples);
  RobustPoseScratchSpace::reset(cameraMotionSamples, numSamples * 4);

  // Things that scale with numPtSamples
  RobustPoseScratchSpace::reset(aPtsSample, numPtSamples);
  RobustPoseScratchSpace::reset(bPtsSample, numPtSamples);
  RobustPoseScratchSpace::reset(sampleResiduals, numPtSamples);
  RobustPoseScratchSpace::reset(bSampleEstimated3, numPtSamples);

  ptSampleSet.clear();
  ptSamplePairSet.clear();

  // Things that scale with numPtsTotal
  RobustPoseScratchSpace::reset(ptSampleInds, numPtsTotal);
  RobustPoseScratchSpace::reset(aPtsExtruded, numPtsTotal);
  RobustPoseScratchSpace::reset(bPtsEstimated3, numPtsTotal);
  RobustPoseScratchSpace::reset(bPtsEstimated, numPtsTotal);
}

bool solvePnP(
  const Vector<HPoint3> &worldPts,
  const Vector<HPoint2> &camRays,
  SolvePnPParams params,
  HMatrix *camMotion,
  Vector<uint8_t> *inliers,
  RobustPoseScratchSpace *sc) {
  size_t numPtsTotal = worldPts.size();

  if (numPtsTotal < params.minPointsForRobustPose) {
    *camMotion = HMatrixGen::i();
    return false;
  }

  HMatrix bestCamMotion = *camMotion;
  ScopeTimer t("pose");
  t.addCounter("input-points", numPtsTotal);

  inliers->resize(worldPts.size());
  std::fill(inliers->begin(), inliers->end(), 1);

  solveOptimalPose(worldPts, camRays, *inliers, &bestCamMotion, params.cameraMotionWeight);

  // For each sample, compute a homography and the residual-at-percentile, keeping the best.
  int sufficientInliers = 0;

  // Fill the mask of inliers, which are points that appear to match the estimated motion.
  bestCamMotion.inv().times(worldPts, &sc->bPtsEstimated3);
  flatten<2>(sc->bPtsEstimated3, &sc->bPtsEstimated);

  // Mark inliners.
  for (int i = 0; i < numPtsTotal; ++i) {
    float xd = sc->bPtsEstimated[i].x() - camRays[i].x();
    float yd = sc->bPtsEstimated[i].y() - camRays[i].y();
    // Since the residual above was computed with a sample of points, it might be an overfit;
    // raise the tolerance for inliers by ~10%.
    auto in = (xd * xd + yd * yd) <= params.poseFailedResidualSqDist;
    (*inliers)[i] = in;
    sufficientInliers += in;
  }

  *camMotion = bestCamMotion;
  return (
    sufficientInliers >= params.minPoseInliers
    && sufficientInliers >= std::floor(params.minInlierFraction * numPtsTotal));
}

bool solveImageTargetHomography(
  const Vector<HPoint2> &imTargetRays,
  const Vector<HPoint2> &camRays,
  const Vector<float> &weights,
  SolvePnPParams params,
  HMatrix *camMotion,
  Vector<uint8_t> *inliers,
  RobustPoseScratchSpace *sc) {
  size_t numPtsTotal = imTargetRays.size();

  if (numPtsTotal < params.minPointsForRobustPose) {
    return false;
  }

  HMatrix bestCamMotion = *camMotion;
  ScopeTimer t("pose-image-homography");
  t.addCounter("input-points", numPtsTotal);

  inliers->resize(imTargetRays.size());
  std::fill(inliers->begin(), inliers->end(), 1);

  poseEstimationImageTarget(
    camRays, imTargetRays, weights, params.cauchyLoss, params.cameraMotionWeight, &bestCamMotion);

  // For each sample, compute a homography and the residual-at-percentile, keeping the best.
  int sufficientInliers = 0;

  // Fill the mask of inliers, which are points that appear to match the estimated motion.
  auto H = homographyForPlane(bestCamMotion, {0.0f, 0.0f, 1.0f}).inv();

  extrude<3>(camRays, &sc->aPtsExtruded);
  H.times(sc->aPtsExtruded, &sc->bPtsEstimated3);
  flatten<2>(sc->bPtsEstimated3, &sc->bPtsEstimated);

  // Mark inliners.
  for (int i = 0; i < numPtsTotal; ++i) {
    float xd = sc->bPtsEstimated[i].x() - imTargetRays[i].x();
    float yd = sc->bPtsEstimated[i].y() - imTargetRays[i].y();
    auto in = (xd * xd + yd * yd) <= params.poseFailedResidualSqDist;
    (*inliers)[i] = in;
    sufficientInliers += in;
  }

  bool succeeded = sufficientInliers >= params.minPoseInliers;
  if (succeeded) {
    *camMotion = bestCamMotion;
  }
  return succeeded;
}

bool robustPnP(
  const Vector<HPoint3> &worldPts,
  const Vector<HPoint2> &camRays,
  const HMatrix &worldTransform,
  RobustPnPParams params,
  HMatrix *camMotion,
  Vector<uint8_t> *inliers,
  RandomNumbers *random,
  RobustPoseScratchSpace *sc,
  RobustPnPDebugOut *debugOut) {
  size_t numPtsTotal = worldPts.size();
  inliers->resize(numPtsTotal);
  if (debug_) {
    C8Log("[pose-pnp@robustPnP] got input worldPts with size: %zu", numPtsTotal);
  }

  if (numPtsTotal > 0) {
    std::fill(inliers->begin(), inliers->end(), 0);
  }

  // RobustPnP does not work properly with far away points such as our skypoints. This is because
  // the algorithm proposes multiple camera poses and then optimizes the one with the lowest
  // residual. If all we have are far away points, then the translation of the proposed camera
  // position will not influence the residual. Therefore, a camera far off from the correct position
  // that is facing the right direction could incorrectly be chosen as the new extrinsic.
  int numClosePts = 0;
  const auto &camTranslation = translation(*camMotion);
  const auto &camPosition = HPoint3(camTranslation.x(), camTranslation.y(), camTranslation.z());
  for (const auto &worldPt : worldPts) {
    if (worldPt.dist(camPosition) < params.farPointDistance) {
      ++numClosePts;
    }

    // Early exit since we have a sufficient number of points.
    if (numClosePts >= params.minPointsForRobustPose) {
      break;
    }
  }

  if (debugOut) {
    debugOut->numPtsTotal = numPtsTotal;
    debugOut->numClosePts = numClosePts;
    debugOut->minPointsForRobustPose = params.minPointsForRobustPose;
  }

  if (numClosePts < params.minPointsForRobustPose) {
    if (debug_) {
      C8Log(
        "[pose-pnp@robustPnp] fail because numClosePts(%d) < params.minPointsForRobustPose(%d)",
        numClosePts,
        params.minPointsForRobustPose);
    }
    *camMotion = HMatrixGen::i();
    return false;
  }
  ScopeTimer t("pose-robust-pnp");
  t.addCounter("input-points", numPtsTotal);

  // Calculate the number of samples needed to ensure a 99.9% probability that three sampled points
  // are inliers. Note that this is independent of the number dataset size. For example:
  //
  // target: 99.9%, inlierFraction 0.9: 6 trios
  // target: 99.9%, inlierFraction 0.8: 10 trios
  // target: 99.9%, inlierFraction 0.7: 17 trios
  // target: 99.9%, inlierFraction 0.6: 29 trios
  // target: 99.9%, inlierFraction 0.5: 52 trios (LMedS)
  // target: 99.9%, inlierFraction 0.4: 105 trios
  // target: 99.9%, inlierFraction 0.3: 253 trios
  // target: 99.9%, inlierFraction 0.2: 861 trios
  // target: 99.9%, inlierFraction 0.1: 6905 trios
  size_t neededSamples = static_cast<size_t>(
    std::log(params.failureProbability)
    / std::log(1.0f - params.inlierFraction * params.inlierFraction * params.inlierFraction));

  size_t maxSamples = (numPtsTotal - 2) * (numPtsTotal - 1) * numPtsTotal / 6;  // n choose 3
  neededSamples = std::min(neededSamples, maxSamples);

  size_t neededPtSamples = std::min(numPtsTotal, params.maxPointSamplesForRobustPose);

  sc->reset(neededSamples, neededPtSamples, numPtsTotal);

  t.addCounter("samples", neededSamples, maxSamples);

  // Generate the set of index triplets to sample for computing pose.
  sampleIndexTriples(numPtsTotal, neededSamples, random, &sc->sampleInds, &sc->ptSampleSet);

  // Sample a subset of points that will be used to evaluate each pose sample.
  sampleDataset(
    worldPts,
    camRays,
    neededPtSamples,
    &sc->aPtsSample,
    &sc->bPtsSample,
    &sc->ptSampleInds,
    random);

  // Compute the potential poses for each sampled triplet.
  proposePoses(worldPts, camRays, sc->sampleInds, worldTransform, params, &sc->cameraMotionSamples);

  // Find the pose from the proposals that has the best percentile residual.
  HMatrix bestCamMotion = HMatrixGen::i();
  float bestResidual = 0.0f;
  findBestProposal(
    sc->aPtsSample,
    sc->bPtsSample,
    sc->cameraMotionSamples,
    params,
    &bestCamMotion,
    &bestResidual,
    &sc->bSampleEstimated3,
    &sc->sampleResiduals);

  if (debugOut) {
    debugOut->bestResidual = bestResidual;
    debugOut->poseFailedResidualSqDist = params.poseFailedResidualSqDist;
  }
  // If no good match is found, return identity.
  if (bestResidual > params.poseFailedResidualSqDist) {
    if (debug_) {
      C8Log(
        "[pose-pnp@robustPnp] fail because bestResidual(%f) > params.poseFailedResidualSqDist(%f)",
        bestResidual,
        params.poseFailedResidualSqDist);
    }
    *camMotion = HMatrixGen::i();
    return false;
  }

  // Find the full set of points that are inliers for the best pose proposal.
  fillInliers(
    worldPts,
    camRays,
    bestCamMotion,
    bestResidual,
    inliers,
    &sc->bPtsEstimated3,
    &sc->bPtsEstimated);

  // Skip pose optimization if requested
  if (params.skipFinalPoseOptimization) {
    *camMotion = bestCamMotion;
    return true;
  }

  // Further optimize camera position with pose optimization.
  solveOptimalPose(worldPts, camRays, *inliers, &bestCamMotion, 1.f);

  // Return the best camera motion after pose optimization.
  *camMotion = bestCamMotion;
  return true;
}

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
  RobustP2PScratchSpace *sc) {
  size_t numPtsTotal = worldPts.size();
  if (debug_) {
    C8Log("[pose-pnp@robustP2P] got input worldPts with size: %zu", numPtsTotal);
  }

  *confidence = 0.0f;
  inliers->clear();
  inliers->resize(numPtsTotal);
  if (numPtsTotal < params.ransac.numSamplesPerSolve) {
    *camMotion = HMatrixGen::i();
    return false;
  }
  ScopeTimer t("pose-robust-p2p");
  t.addCounter("input-points", numPtsTotal);

  size_t maxSamples = (numPtsTotal - 1) * numPtsTotal / 2;  // n choose 2
  int trialCount = 0;
  auto neededTrials =
    std::min(params.ransac.computeNeededTrials(params.ransac.initialInlierGuess), maxSamples);
  float temperature = params.simAnn.initialTemperature;

  // Initializing the best confidence to minimum required confidence, which helps temperature-based
  // sampling not use inliers from bad poses.
  float confidenceThreshold = 0.0f;
  if (params.isImageTracking) {
    confidenceThreshold = settings().imageTrackingConfidenceThreshold;
  } else {
    confidenceThreshold = settings().relocalizationConfidenceThreshold;
  }
  auto bestConfidence = confidenceThreshold;
  size_t bestNumInliers = 0;
  HMatrix bestPoseProposal = HMatrixGen::i();
  sc->reset(numPtsTotal);
  sc->bestInlierSet.resize(numPtsTotal);
  std::iota(sc->bestInlierSet.begin(), sc->bestInlierSet.end(), 0);

  sc->indices.resize(numPtsTotal);
  std::iota(sc->indices.begin(), sc->indices.end(), 0);

  // If confidence area radius is large, use less bins for efficiency
  auto diameter = settings().confidenceAreaRadius * 2;
  auto xSegments = static_cast<int>(std::ceil((upperRight.x() - lowerLeft.x()) / diameter));
  auto ySegments = static_cast<int>(std::ceil((upperRight.y() - lowerLeft.y()) / diameter));
  BinMap binMap(xSegments, ySegments);
  // Confidence grid only needs to be computed once across robustP2P calls
  if (sc->confidenceGrid.empty()) {
    sc->confidenceGrid = uniformSampleGrid(lowerLeft, upperRight);
  }

  while (trialCount < neededTrials) {
    IndexPair sampledPair;
    if (!sampleUniqueIndexPairWithTemperature(
          numPtsTotal, sc->bestInlierSet, sc->usedPairs, temperature, random, &sampledPair)) {
      break;
    }
    sc->usedPairs.insert(sampledPair);
    auto i0 = sampledPair.first;
    auto i1 = sampledPair.second;

    // Fail in baseline check not consume a trial
    float baseline2d = camRays[i0].dist(camRays[i1]);
    if (baseline2d < params.ransac.minBaseline2d) {
      continue;
    }

    // If we count failure, as well as success, increasing count here
    if (params.ransac.countFailure) {
      ++trialCount;
    }

    bool validPoseProposal = false;

    // P2P gives 2 pose proposals for each pair
    if (!p2pImpl(
          worldPts[i0],
          worldPts[i1],
          camRays[i0],
          camRays[i1],
          trackingPose,
          &sc->poseProposals[0],
          &sc->poseProposals[1])) {
      continue;
    }

    for (size_t i = 0; i < 2; ++i) {
      const HMatrix &poseProposal = sc->poseProposals[i];
      if (
        getPointDepth(poseProposal, worldPts[i0]) < 0.f
        || getPointDepth(poseProposal, worldPts[i1]) < 0.f) {
        // Points behind the camera
        continue;
      }
      // If either proposals valid, this should count as a trial
      validPoseProposal = true;

      // Note: This is the bottleneck of the algorithm, since it does projection, flattening, and
      // residual checking
      {
        ScopeTimer t1("compute-inliers");
        reprojectionInliers(
          worldPts, poseProposal, camRays, params.ransac.inlierThreshold, &sc->inlierSet);
      }

      size_t numInliers = sc->inlierSet.size();
      if (numInliers < std::max(params.ransac.minInlierRequired, bestNumInliers)) {
        // Not a better pose proposal
        continue;
      }
      float proposalConfidence = computePoseConfidence(
        camRays,
        lowerLeft,
        upperRight,
        sc->inlierSet,
        sc->confidenceGrid,
        confidenceThreshold,
        &binMap);
      if (
        proposalConfidence < confidenceThreshold
        || (numInliers == bestNumInliers && proposalConfidence < bestConfidence)) {
        // Not a better pose proposal
        continue;
      }

      bestNumInliers = numInliers;
      bestConfidence = proposalConfidence;
      // Swap for quicker copy
      std::swap(bestPoseProposal, sc->poseProposals[i]);
      std::swap(sc->bestInlierSet, sc->inlierSet);
      float newInlierProb = static_cast<float>(bestNumInliers) / static_cast<float>(numPtsTotal);
      neededTrials = std::min(
        params.ransac.computeNeededTrials(newInlierProb), maxSamples - sc->usedPairs.size());
    }
    // TODO(Riyaan): Decay temperature only after we have a proposal with min inliers/confidence
    temperature *= params.simAnn.coolingRate;

    // If we don't count failure as a trial, only increase count when we have a valid pose proposal
    if (!params.ransac.countFailure && validPoseProposal) {
      ++trialCount;
    }
  }

  if (bestNumInliers < params.ransac.minInlierRequired || bestConfidence < confidenceThreshold) {
    t.addCounter("trial-count-fail", trialCount);
    *camMotion = HMatrixGen::i();
    return false;
  }
  t.addCounter("trial-count-success", trialCount);

  // Fill the mask of inliers, which are points that appear to match the estimated motion.
  for (auto idx : sc->bestInlierSet) {
    (*inliers)[idx] = 1;
  }

  *confidence = bestConfidence;
  *camMotion = bestPoseProposal.inv();

  // Further optimize camera position with pose optimization, if requested
  if (!params.skipFinalPoseOptimization) {
    solveOptimalPose(worldPts, camRays, *inliers, camMotion, 1.f);
  }

  return true;
}

}  // namespace c8
