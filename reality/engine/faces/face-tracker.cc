// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "face-tracker.h",
  };
  deps = {
    ":face-detector-global",
    ":face-detector-local",
    ":face-geometry",
    ":tracked-face-state",
    "//c8:map",
    "//c8:parameter-data",
    "//c8:set",
    "//c8:vector",
    "//c8/geometry:face-types",
    "//c8/geometry:facemesh-data",
    "//c8/geometry:pixel-pinhole-camera-model",
    "//c8/io:capnp-messages",
    "//c8/stats:scope-timer",
    "//reality/engine/api:face.capnp-cc",
    "//reality/engine/binning:linear-bin",
    "//reality/engine/ears:ear-detector-local",
    "//reality/engine/ears:ear-types",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xec54d87e);

#include <cmath>

#include "c8/geometry/facemesh-data.h"
#include "c8/io/capnp-messages.h"
#include "c8/parameter-data.h"
#include "c8/set.h"
#include "c8/stats/scope-timer.h"
#include "reality/engine/ears/ear-types.h"
#include "reality/engine/faces/face-geometry.h"
#include "reality/engine/faces/face-tracker.h"

namespace c8 {

int FaceTracker::MAX_TRACKED_FACES = 1;

namespace {

constexpr int GLOBAL_DETECTOR_INTERVAL = 20;

struct Settings {
  // when true, we will cache ear data by averaging results into bins (by z and y rotation)
  bool toCacheEarData;
  // compute ear data each 'earCacheCheckInterval' frames to see if we need to reset the bucket
  int earCacheCheckInterval;
  // ratio to compute drift threshold based on ear anchor point distances
  float offsetThresholdRatio;
  // when true, we will cache ear data across multiple predictions. A single model for all angles.
  // designed to be used when `toCacheEarData` is false.
  bool persistentEarData;
};

const Settings &settings() {
  static const Settings settings_{
    globalParams().getOrSet("FaceTracker.toCacheEarData", false),
    globalParams().getOrSet("FaceTracker.earCacheCheckInterval", 10),
    globalParams().getOrSet("FaceTracker.offsetThresholdRatio", 0.1f),
    globalParams().getOrSet("FaceTracker.persistentEarData", true),
  };
  return settings_;
}

}  // namespace

void recordFaceBBoxInRay(
  const Vector<DetectedPoints> &detectedFaces, TreeMap<int, BoundingBox> *faceToUprightBBoxInRay) {
  for (const auto &detectedFace : detectedFaces) {
    (*faceToUprightBBoxInRay)[detectedFace.roi.faceId] =
      uprightBBox(detectionToRaySpace(detectedFace).boundingBox);
  }
}

Vector<DetectedPoints> filterGlobalAndAssignIds(
  const Vector<DetectedPoints> &globalDetections,
  const TreeMap<int, TrackedFaceState> &facesById,
  const Vector<DetectedPoints> &previousGlobalFaces,
  int maxTrackedFaces,
  int currentFrameNum,
  TreeMap<int, BoundingBox> *faceToUprightBBoxInRay,
  TreeMap<int, int> *faceIdToFirstFrame) {
  // A redudant global detection is one that we already have an active local detection for. It's not
  // redundant if:
  // 1) We have a matching global detection from the previous frame.
  // 2) It's a new global detection.
  // 3) It's from a previously lost detection.
  Vector<DetectedPoints> nonRedundantGlobalDetections;
  nonRedundantGlobalDetections.reserve(globalDetections.size());

  // Records which faceIds have already been matched.
  TreeSet<int> matchedFaceIds;
  // Records which global detections have already been matched.
  TreeSet<int> matchedGlobalDetectionIndices;

  // This is a O(N)^3 greedy algorithm. The good news is that at worst this is a 3x3x3 operation
  // since there are at max 3 face detections at a time. We go through each possible global -
  // (local/lost/previously found global) pairing and iteratively determine the pairing with the
  // least distance and max IOU. If there's a global detection left over, then it's a new face.

  // TODO(nathan): in a followup PR, I want to further improve face tracking consistency by using
  // concept that the faces in a screen typically stay in a left to right ordering. Right now using
  // the bounding box can be unreliable at times such as when the camera moves quickly.

  // TODO(nathan): could also look at creating signatures for faces based off the mesh coordinates.

  for (int i = 0; i < globalDetections.size(); ++i) {
    int matchedGlobalDetectionIndex = -1;
    int matchedFaceId = -1;
    float minDistance = std::numeric_limits<float>::max();
    float maxIOU = 0.f;

    for (int globalIdx = 0; globalIdx < globalDetections.size(); ++globalIdx) {
      if (matchedGlobalDetectionIndices.find(globalIdx) != matchedGlobalDetectionIndices.end()) {
        // this global detection has already been matched.
        continue;
      }
      auto &globalDetection = globalDetections.at(globalIdx);

      for (const auto &[faceId, bbox] : (*faceToUprightBBoxInRay)) {
        if (matchedFaceIds.find(faceId) != matchedFaceIds.end()) {
          // this local/lost/previous global detection has already been matched.
          continue;
        }

        auto uprightDetection = uprightBBox(detectionToRaySpace(globalDetection).boundingBox);
        auto distance = minBBoxDistance(uprightDetection, bbox);
        auto iou = intersectionOverUnion(uprightDetection, bbox);
        if (distance <= minDistance && iou >= maxIOU) {
          maxIOU = iou;
          minDistance = distance;
          matchedGlobalDetectionIndex = globalIdx;
          matchedFaceId = faceId;
        }
      }
    }

    if (matchedGlobalDetectionIndex != -1) {
      matchedGlobalDetectionIndices.insert(matchedGlobalDetectionIndex);
      matchedFaceIds.insert(matchedFaceId);

      auto &globalDetection = globalDetections[matchedGlobalDetectionIndex];

      // If we match with an active local detection, this is a redundant global detection.
      if (facesById.find(matchedFaceId) != facesById.end()) {
        continue;
      }

      // Otherwise we matched against either a previously found global detection or a lost
      // detection.
      (*faceIdToFirstFrame)[matchedFaceId] = currentFrameNum;
      nonRedundantGlobalDetections.push_back(globalDetection);
      nonRedundantGlobalDetections.back().roi.faceId = matchedFaceId;
      recordFaceBBoxInRay({nonRedundantGlobalDetections.back()}, faceToUprightBBoxInRay);
    }
  }

  // If a global detection did not get matched, then we give it a new id.
  for (int i = 0; i < globalDetections.size(); ++i) {
    if (matchedGlobalDetectionIndices.find(i) == matchedGlobalDetectionIndices.end()) {
      int availableFaceId = 1;
      for (; availableFaceId <= maxTrackedFaces; ++availableFaceId) {
        if (faceToUprightBBoxInRay->find(availableFaceId) == faceToUprightBBoxInRay->end()) {
          // We have found an un-used id.
          break;
        }
      }

      // Record when the face was first found.
      (*faceIdToFirstFrame)[availableFaceId] = currentFrameNum;

      auto &globalDetection = globalDetections[i];
      nonRedundantGlobalDetections.push_back(globalDetection);
      nonRedundantGlobalDetections.back().roi.faceId = availableFaceId;
      recordFaceBBoxInRay({nonRedundantGlobalDetections.back()}, faceToUprightBBoxInRay);
    }
  }

  auto interval = currentFrameNum % GLOBAL_DETECTOR_INTERVAL;
  // There are two scenarios this is fixing, which are both due to there being two interleaving
  // face pipelines running at a time. Global detections runs on frame N and N+1.
  // 1) We find two faces on frame N.  We only find one face on frame N+1. This would cause the
  //    missed face on frame N+1 to pop in and out every other frame.
  // 2) We find one face on frame N.  We find two faces on frame N+1. This would cause the
  //    missed face on frame N to pop in and out every other frame.
  //
  // Add the non-duplicate. If it's starting date was the previous frame and it isn't in the matched
  // global detections or currently traced local faces, then we can re-add it. This doesn't solve
  // all types of flicker though. For example, if we lose local detection in only one pipeline we
  // can still get flicker again until the face is re-found in a global detection.
  if (interval == 1 || interval == 2) {
    for (const auto &previousGlobalFace : previousGlobalFaces) {
      auto id = previousGlobalFace.roi.faceId;
      if (
        // The global detection needed to have been created in the previous frame.
        (*faceIdToFirstFrame)[id] == currentFrameNum - 1
        && matchedGlobalDetectionIndices.find(id) == matchedGlobalDetectionIndices.end()
        && facesById.find(id) == facesById.end()) {
        nonRedundantGlobalDetections.push_back(previousGlobalFace);
      }
    }
  }

  return nonRedundantGlobalDetections;
}

void FaceTracker::reset() {
  facesById_.clear();
  globalFaces_.clear();
  faceData_.clear();
  lostIds_.clear();
  faceToUprightBBoxInRay_.clear();
}

void FaceTracker::setMaxTrackedFaces(int n) { MAX_TRACKED_FACES = std::max(0, std::min(n, 3)); }

void FaceTracker::setFaceDetectModel(const uint8_t *model, int modelSize) {
  faceDetector_.reset(new FaceDetectorGlobal(model, modelSize));
}

void FaceTracker::setFaceMeshModel(const uint8_t *model, int modelSize) {
  meshDetector_.reset(new FaceDetectorLocal(model, modelSize, earConfig_));
}

void FaceTracker::setEarCacheBinNums(EarPoseBinIdx binDim) {
  earCacheBinDims_ = binDim;
  earYBin_ = LinearBin{binDim.yBin, -180, 180};
  earZBin_ = LinearBin{binDim.zBin, -180, 180};
}

void FaceTracker::setEarModel(const uint8_t *model, int modelSize) {
  earDetector_.reset(new EarDetectorLocal(model, modelSize));

  setEarCacheBinNums(earCacheBinDims_);
}

FaceTrackingResult FaceTracker::track(
  const FaceRenderResult &faceRenderResult, c8_PixelPinholeCameraModel intrinsics) {
  ConstRootMessage<DebugOptions> opts;
  return track(faceRenderResult, intrinsics, opts.reader());
}

FaceTrackingResult FaceTracker::track(
  const FaceRenderResult &faceRenderResult,
  c8_PixelPinholeCameraModel intrinsics,
  DebugOptions::Reader opts) {
  ScopeTimer t("track-faces");

  // Do computations
  globalFaces_.clear();
  localFaces_.clear();
  faceData_.clear();
  lostIds_.clear();

  localEars_.clear();
  earData_.clear();

  {
    ScopeTimer t1("detectors");
    if (!opts.getOnlyDetectFaces()) {
      if (faceRenderResult.faceMeshImages.size() > 0 && meshDetector_ != nullptr) {
        for (const auto &im : faceRenderResult.faceMeshImages) {
          // Mesh detector will return 1 or 0 results.
          auto f = meshDetector_->analyzeFace(im, intrinsics);
          if (!f.empty()) {
            localFaces_.push_back(f[0]);
            if (getIsTrackingEars()) {
              for (size_t k = 1; k < f.size(); ++k) {
                if (
                  f[k].roi.source == ImageRoi::Source::EAR_LEFT
                  || f[k].roi.source == ImageRoi::Source::EAR_RIGHT) {
                  localEars_.push_back(f[k]);
                }
              }
            }

            // skip extra faces
            if (localFaces_.size() >= MAX_TRACKED_FACES) {
              break;
            }
          }
        }
      }
    }

    // Store the detected faces location so that we only have to look in that area of the frame
    // for a face in the next frame
    auto interval = frameCounter_ % GLOBAL_DETECTOR_INTERVAL;
    if (
      faceDetector_ != nullptr
      // We don't need to run the global detector if we already have the max number of local faces.
      && localFaces_.size() < MAX_TRACKED_FACES
      // We need to the global detector to run N and N+1 so that the two concurrent face pipelines
      // are in sync.
      // TODO(nathan): there's a bug that can happen if we detect a face in one frame but not in the
      // next so the face will appear every other frame until its detected by the global detector.
      // This can be handled by checking the pipeline state to see if we detected a global frame in
      // the previous frame in which case we can just re-use it.
      && (interval == 0 || interval == 1)) {
      globalFaces_ = faceDetector_->detectFaces(faceRenderResult.faceDetectImage, intrinsics);
      if (globalFaces_.size() > MAX_TRACKED_FACES) {
        globalFaces_.resize(MAX_TRACKED_FACES);
      }
    }
  }

  {
    ScopeTimer t1("geometry");
    // For all of our existing state, mark that the face could have been found so that we can find
    // lost ones later.
    for (auto &state : facesById_) {
      state.second.markFrame();
    }

    // Set the faces field for FaceResponse if they have mesh data
    faceData_.reserve(localFaces_.size());
    for (const auto &faceDetection : localFaces_) {
      auto &faceState = facesById_[faceDetection.roi.faceId];  // creates if new.
      // Convert the orthographic faceDetection information from Facemesh into 3D space
      faceData_.push_back(faceState.locateFace(faceDetection));
    }

    // Find lost elements and remove them.
    lostIds_.reserve(facesById_.size());
    for (const auto &state : facesById_) {
      if (state.second.status() == Face3d::TrackingStatus::LOST) {
        lostIds_.push_back(state.first);
      }
    }

    for (auto id : lostIds_) {
      // Mark lost
      faceIdToFirstFrame_[id] = -1;
      Face3d lostFace;
      lostFace.id = id;
      lostFace.status = Face3d::TrackingStatus::LOST;
      faceData_.push_back(lostFace);
      facesById_.erase(id);

      faceIdToSampledEar3d_.erase(id);
    }
  }

  recordFaceBBoxInRay(localFaces_, &faceToUprightBBoxInRay_);

  globalFaces_ = filterGlobalAndAssignIds(
    globalFaces_,
    facesById_,
    previousGlobalFaces_,
    MAX_TRACKED_FACES,
    frameCounter_,
    &faceToUprightBBoxInRay_,
    &faceIdToFirstFrame_);

  previousGlobalFaces_ = globalFaces_;
  frameCounter_++;

  return {&globalFaces_, &localFaces_, &faceData_, &lostIds_, &localEars_};
}

EarPoseBinIdx FaceTracker::getEarRotationBinNum(const Quaternion &rotation) const {
  float rx, ry, rz;
  rotation.toEulerAngleDegrees(&rx, &ry, &rz);
  return {earYBin_.binNum(ry), earZBin_.binNum(rz)};
}

EarTrackingResult FaceTracker::trackEars(
  const EarRenderResult &earRenderResult, c8_PixelPinholeCameraModel intrinsics) {
  if (!earDetector_) {
    return {&localEars_, &earData_, &lostIds_};
  }

  // for every GLOBAL_DETECTOR_INTERVAL frames, we check to see
  // if the ear vertices are too far from the cached positions
  // if too far, we will reset the cache and re-average the vertices
  earFrameCounter_ = (earFrameCounter_ + 1) % settings().earCacheCheckInterval;

  // build TreeMap for faster fetching of related data
  TreeMap<int, int> faceIdToLocalFaceIdxMap;
  for (size_t f = 0; f < localFaces_.size(); ++f) {
    faceIdToLocalFaceIdxMap[localFaces_[f].roi.faceId] = f;
  }
  TreeMap<int, int> faceIdToFace3dIdxMap;
  for (size_t f = 0; f < faceData_.size(); ++f) {
    faceIdToFace3dIdxMap[faceData_[f].id] = f;
  }
  TreeMap<int, int> faceIdToLocalEarIdxMap;
  for (size_t f = 0; (f < localEars_.size()) && (f + 1 < localEars_.size()); f += 2) {
    faceIdToLocalEarIdxMap[localEars_[f].roi.faceId] = f;
  }

  // The ear detector should have been initialized already
  const auto &earImages = earRenderResult.earDetectImages;
  for (size_t i = 0; (i < earImages.size() && i + 1 < earImages.size()); i += 2) {
    // find related data by faceId
    auto faceId = earImages[i].roi.faceId;
    int localFaceIndex = -1;
    if (faceIdToLocalFaceIdxMap.find(faceId) != faceIdToLocalFaceIdxMap.end()) {
      localFaceIndex = faceIdToLocalFaceIdxMap[faceId];
    }
    int faceDataIndex = -1;
    if (faceIdToFace3dIdxMap.find(faceId) != faceIdToFace3dIdxMap.end()) {
      faceDataIndex = faceIdToFace3dIdxMap[faceId];
    }
    int localEarIndex = -1;
    if (faceIdToLocalEarIdxMap.find(faceId) != faceIdToLocalEarIdxMap.end()) {
      localEarIndex = faceIdToLocalEarIdxMap[faceId];
    }
    bool hasFaceState = (facesById_.find(faceId) != facesById_.end());
    if (localFaceIndex < 0 || faceDataIndex < 0 || localEarIndex < 0 || !hasFaceState) {
      continue;
    }

    EarPoseBinIdx binIdx = getEarRotationBinNum(faceData_[faceDataIndex].transform.rotation);

    // see if this position the ear vertices are already computed
    if (
      settings().toCacheEarData && earFrameCounter_ != 0
      && isEar3DSampled(faceIdToSampledEar3d_, faceId, binIdx)) {
      // vertices are already computed, return the cached results
      Ear3d avgEar;
      getEar3DSampled(faceIdToSampledEar3d_, faceData_[faceDataIndex], binIdx, &avgEar);

      // update attachment points and states
      computeEarAttachmentPoints(&avgEar);
      updateEarStates(&avgEar);

      earData_.push_back(avgEar);
      continue;
    }

    Vector<DetectedPoints> ears =
      earDetector_->analyzeEars(earImages[i], earImages[i + 1], intrinsics);

    // append ear landmark data to ears with the same faceId
    for (const HPoint3 &pt : ears[0].points) {
      localEars_[localEarIndex].points.push_back(pt);
    }
    for (const HPoint3 &pt : ears[1].points) {
      localEars_[localEarIndex + 1].points.push_back(pt);
    }

    // Lift local ears (x, y, visibility) to 3d (x, y, z) in ear3d. visibility is stored separately.
    // Invisible ear vertices are mirrored from the visible side.
    Ear3d ear3d = earLiftLandmarksTo3D(
      localFaces_[localFaceIndex],
      localEars_[localEarIndex],
      localEars_[localEarIndex + 1],
      faceData_[faceDataIndex],
      intrinsics);

    // Compute ear vertex offset thresholds. Cached points that are too far from NN outputs based
    // on this threshold will be reset.
    auto &localFaceVerts = faceData_[faceDataIndex].vertices;
    const float leftSideLength =
      localFaceVerts[FACEMESH_L_EAR].dist(localFaceVerts[FACEMESH_L_EAR_LOW]);
    const float leftSideThreshold = settings().offsetThresholdRatio * leftSideLength;
    const float rightSideLength =
      localFaceVerts[FACEMESH_R_EAR].dist(localFaceVerts[FACEMESH_R_EAR_LOW]);
    const float rightSideThreshold = settings().offsetThresholdRatio * rightSideLength;

    // average the ear vertices
    Ear3d avgEar = ear3d;
    if (settings().toCacheEarData) {
      avgEar = averageEar3d(
        ear3d,
        binIdx,
        earCacheBinDims_,
        leftSideThreshold,
        rightSideThreshold,
        &faceIdToSampledEar3d_);
    }

    if (settings().persistentEarData) {
      avgEar = updateAverageEar(ear3d, &faceIdToPersistentEar3d_);
    }

    // update attachment points and states
    computeEarAttachmentPoints(&avgEar);
    updateEarStates(&avgEar);

    earData_.push_back(avgEar);
  }

  return {&localEars_, &earData_, &lostIds_};
}

}  // namespace c8
