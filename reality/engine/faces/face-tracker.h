// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "c8/geometry/face-types.h"
#include "c8/geometry/pixel-pinhole-camera-model.h"
#include "c8/map.h"
#include "c8/vector.h"
#include "reality/engine/api/face.capnp.h"
#include "reality/engine/binning/linear-bin.h"
#include "reality/engine/ears/ear-detector-local.h"
#include "reality/engine/ears/ear-types.h"
#include "reality/engine/faces/face-detector-global.h"
#include "reality/engine/faces/face-detector-local.h"
#include "reality/engine/faces/tracked-face-state.h"

namespace c8 {

// Records the location of the detections faces as upright bounding boxes in ray space.
void recordFaceBBoxInRay(
  const Vector<DetectedPoints> &detectedFaces, TreeMap<int, BoundingBox> *faceToUprightBBoxInRay);

// Handles both the removing of global detections that are redundant of local detections and
// correclty assigning faceIds to global detections.
Vector<DetectedPoints> filterGlobalAndAssignIds(
  const Vector<DetectedPoints> &globalDetections,
  const TreeMap<int, TrackedFaceState> &facesById,
  const Vector<DetectedPoints> &previousGlobalFaces,
  int maxTrackedFaces,
  int currentFrameNum,
  TreeMap<int, BoundingBox> *faceToUprightBBoxInRay,
  TreeMap<int, int> *faceIdToFirstFrame);

struct FaceTrackingResult {
  // globalFaces are made sure to not be redundant of existing localFaces.
  const Vector<DetectedPoints> *globalFaces;
  const Vector<DetectedPoints> *localFaces;
  const Vector<Face3d> *faceData;
  const Vector<int> *lostIds;
  const Vector<DetectedPoints> *localEars;
};

// FaceDetectorLocal provides an abstraction layer above the deep net model for analyzing a face
// in detail from a high res region of interest.
class FaceTracker {
public:
  // Construct from a tflite model.
  FaceTracker() = default;

  // Default move constructors.
  FaceTracker(FaceTracker &&) = default;
  FaceTracker &operator=(FaceTracker &&) = default;

  // Disallow copying.
  FaceTracker(const FaceTracker &) = delete;
  FaceTracker &operator=(const FaceTracker &) = delete;

  void setFaceDetectModel(const uint8_t *model, int modelSize);
  void setFaceDetectModel(Vector<uint8_t> model) { setFaceDetectModel(model.data(), model.size()); }

  void setFaceMeshModel(const uint8_t *model, int modelSize);
  void setFaceMeshModel(Vector<uint8_t> model) { setFaceMeshModel(model.data(), model.size()); }

  void setEarModel(const uint8_t *model, int modelSize);
  void setEarModel(Vector<uint8_t> model) { setEarModel(model.data(), model.size()); }

  FaceTrackingResult track(
    const FaceRenderResult &faceRenderResult, c8_PixelPinholeCameraModel intrinsics);

  FaceTrackingResult track(
    const FaceRenderResult &faceRenderResult,
    c8_PixelPinholeCameraModel intrinsics,
    DebugOptions::Reader options);

  static void setMaxTrackedFaces(int n);

  // Clear in-progress tracking state.
  void reset();

  // Ear ROI computations are done in the track() function.
  // trackEars() will run ear-detector-local inference to get local ear detections
  EarTrackingResult trackEars(
    const EarRenderResult &earRenderResult, c8_PixelPinholeCameraModel intrinsics);

  bool getIsTrackingEars() const { return earConfig_.isEnabled; }
  void setIsTrackingEars(bool enabled) { earConfig_.isEnabled = enabled; }

  // START Debugging method. We might remove this at any time.
  const TreeMap<int, BoundingBox> &faceToUprightBBoxInRay() const {
    return faceToUprightBBoxInRay_;
  }
  const HashMap<int, EarSampledVerticesFullView> &getFaceIdToSampledEar3d() const {
    return faceIdToSampledEar3d_;
  }
  EarPoseBinIdx getEarRotationBinNum(const Quaternion &rotation) const;
  // END Debugging methods
private:
  std::unique_ptr<FaceDetectorGlobal> faceDetector_;
  std::unique_ptr<FaceDetectorLocal> meshDetector_;
  // Keeps track of the faces actively tracked by the local detector.
  TreeMap<int, TrackedFaceState> facesById_;

  // ear landmark detector
  std::unique_ptr<EarDetectorLocal> earDetector_;

  // The global faces found this frame. Cleared on each frame.
  Vector<DetectedPoints> globalFaces_;
  Vector<DetectedPoints> previousGlobalFaces_;
  // The local faces found this frame. Cleared on each frame.
  Vector<DetectedPoints> localFaces_;

  EarConfig earConfig_;
  // The local ears found in this frame. Cleared on each frame.
  // localEars_ : local detections of ears
  Vector<DetectedPoints> localEars_;
  // Output representation for tracked and lost ears that we emit to the user.
  Vector<Ear3d> earData_;

  // Tracks face to bbox position across frames. Note that the faceIds are for:
  // - currently tracked local face detections (needed for making sure a new global detection isn't
  //   redundant with an actively tracked face)
  // - currently tracked global face detections (needed for associating a global detection in frame
  //   n with a global detecion in frame n+1)
  // - lost face detections (needed for re-attaching a faceId for a new global detection to a lost
  //   face)
  TreeMap<int, BoundingBox> faceToUprightBBoxInRay_;
  // Records when a face was first found. -1 means it was lost.
  TreeMap<int, int> faceIdToFirstFrame_;
  // Output representation for tracked and lost faces that we emit to the user.
  Vector<Face3d> faceData_;
  // Records which ids were lost this frame. Cleared on each frame.
  Vector<int> lostIds_;

  static int MAX_TRACKED_FACES;

  // Used for the global detection interval to potentially detect more faces.
  int frameCounter_ = 0;

  // Frame count to see if we need to check current ear data vs cached data.
  size_t earFrameCounter_ = 0;

  // Pitch Yaw degrees will be divided into bins for caching the ear vertices
  EarPoseBinIdx earCacheBinDims_ = {72, 72};
  LinearBin earYBin_;
  LinearBin earZBin_;
  void setEarCacheBinNums(EarPoseBinIdx binDim);

  // accumulate and average the vertices to provide more stable Ear3d results
  HashMap<int, EarSampledVerticesFullView> faceIdToSampledEar3d_;

  // Only used when persistentEarData settings is on
  HashMap<int, EarAveragedVertices> faceIdToPersistentEar3d_;
};

}  // namespace c8
