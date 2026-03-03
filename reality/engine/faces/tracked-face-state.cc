// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"
cc_library {
  hdrs = {
    "tracked-face-state.h",
  };
  deps = {
    ":face-geometry",
    "//c8:hmatrix",
    "//c8:vector",
    "//c8/geometry:face-types",
    "//c8/geometry:facemesh-data",
    "//c8/geometry:mesh",
    "//c8/geometry:vectors",
    "//c8/string:containers",
    "//c8/stats:scope-timer",
    "//reality/engine/geometry:bundle",
    "//reality/engine/tracking:ray-point-filter",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x9eade3df);

#include <cmath>

#include "c8/geometry/facemesh-data.h"
#include "c8/geometry/mesh.h"
#include "c8/geometry/vectors.h"
#include "c8/stats/scope-timer.h"
#include "c8/string/containers.h"
#include "reality/engine/faces/face-geometry.h"
#include "reality/engine/faces/tracked-face-state.h"
#include "reality/engine/geometry/bundle.h"

namespace c8 {

namespace {
constexpr float MIN_LIP_DISTANCE_FOR_MOUTH_OPEN = 0.02f;
constexpr float MIN_EYELID_DISTANCE_FOR_EYE_OPEN = 0.034f;
constexpr float MIN_CURRENT_TO_REST_MIDDLE_EYEBROW_Y_FOR_EYEBROW_RAISED = 0.025;
constexpr int MIN_VALID_IPD_ESTIMATES = 10;
constexpr float MAX_HEAD_ANGLE_IPD_ESTIMATES = 0.21f;
// Artificially increases the top most facemesh points' y by this value, and raises the points just
// below the top by half this value. Used to cover up more of the user's forehead.
constexpr float FACE_TOP_INCREASE = 0.1f;
}  // namespace

void FacePointsFilter::apply(DetectedPoints *raySpacePoints) {
  if (filters_.empty()) {
    filters_.reserve(raySpacePoints->points.size());
    for (const auto &pt : raySpacePoints->points) {
      filters_.push_back({pt, config_});
    }
    return;
  }

  for (int i = 0; i < raySpacePoints->points.size(); ++i) {
    auto &pt = raySpacePoints->points[i];
    pt = filters_[i].filter(pt);
  }
}

void TrackedFaceState::markFrame() { framesSinceLocated_++; }

Face3d::TrackingStatus TrackedFaceState::status() const {
  if (framesSinceLocated_ > 0) {
    return Face3d::TrackingStatus::LOST;
  }

  if (framesTracked_ > 1) {
    return Face3d::TrackingStatus::UPDATED;
  }

  return Face3d::TrackingStatus::FOUND;
}

Face3d TrackedFaceState::locateFace(const DetectedPoints &localFaceDetection) {
  ScopeTimer t("detection-to-mesh");
  ++framesTracked_;
  framesSinceLocated_ = 0;

  // Convert detections to camera ray space.
  auto raySpaceDetection = detectionToRaySpace(localFaceDetection);
  filter_.apply(&raySpaceDetection);

  // TODO(nb): Apply a filter to raySpaceDetection before computing headTransform.

  // Get the reference anchor for this detection result for the detected face position and size.
  auto referenceTransform = computeAnchorTransform(raySpaceDetection);

  // Use the refence anchor to compute the location of the mesh points in 3d relative to the camera.
  auto worldVertices = worldPoints(raySpaceDetection, referenceTransform, localFaceDetection);

  // Convert world points to the anchor's local space.
  auto vertices = worldToLocal(worldVertices, referenceTransform);

  // by only using a subset of the points, we're able to get ~37x performance boost
  poseEstimationFull3d(
    getPosePointsSubset(vertices), getPosePointsSubset(FACEMESH_SAMPLE_VERTICES), &localPose_);

  // Adjust the refence anchor based on the computed pose in the anchor space.
  auto headTransform = adjustAnchor(referenceTransform, localPose_);

  // Re-compute local points with rotation in head transform.
  vertices = worldToLocal(worldVertices, headTransform);

  Vector<HVector3> normals;
  computeVertexNormals(vertices, FACEMESH_INDICES, &normals);

  // Get face anchors
  const auto attachmentPoints = getAttachmentPoints(vertices, normals);

  FaceExpressionOutput expressionOutput;
  expressionOutput.lipsDistance =
    vertices[FACEMESH_MOUTH_BOTTOM_MIDDLE].dist(vertices[FACEMESH_MOUTH_TOP_MIDDLE]);
  expressionOutput.leftEyelidDistance =
    vertices[FACEMESH_UPPER_L_EYE].dist(vertices[FACEMESH_LOWER_L_EYE]);
  expressionOutput.rightEyelidDistance =
    vertices[FACEMESH_UPPER_R_EYE].dist(vertices[FACEMESH_LOWER_R_EYE]);
  // TODO(nathan): remove these two:
  expressionOutput.nasalBridgeToLeftEyebrowDistance =
    vertices[FACEMESH_NOSE_TOP].dist(vertices[FACEMESH_L_EYEBROW_MIDDLE]);
  expressionOutput.nasalBridgeToRightEyebrowDistance =
    vertices[FACEMESH_NOSE_TOP].dist(vertices[FACEMESH_R_EYEBROW_MIDDLE]);

  // Handle eyebrows.
  auto &gestureState = faceToGestureState_[localFaceDetection.roi.faceId];
  if (!gestureState.determinedRestState) {
    // TODO(nathan): don't just base the rest state on the first frame. This can be easily fooled
    // if the user has their eyebrows raised or lowered on the first frame.
    gestureState.restLeftMiddleEyebrowY = vertices[FACEMESH_L_EYEBROW_MIDDLE].y();
    gestureState.restRightMiddleEyebrowY = vertices[FACEMESH_R_EYEBROW_MIDDLE].y();
    gestureState.determinedRestState = true;
  }
  // Note that we don't use abs().
  expressionOutput.currentLeftEyebrowMiddleToRest =
    vertices[FACEMESH_L_EYEBROW_MIDDLE].y() - gestureState.restLeftMiddleEyebrowY;
  expressionOutput.currentRightEyebrowMiddleToRest =
    vertices[FACEMESH_R_EYEBROW_MIDDLE].y() - gestureState.restRightMiddleEyebrowY;

  expressionOutput.mouthOpen = expressionOutput.lipsDistance > MIN_LIP_DISTANCE_FOR_MOUTH_OPEN;
  expressionOutput.leftEyeOpen =
    expressionOutput.leftEyelidDistance > MIN_EYELID_DISTANCE_FOR_EYE_OPEN;
  expressionOutput.rightEyeOpen =
    expressionOutput.rightEyelidDistance > MIN_EYELID_DISTANCE_FOR_EYE_OPEN;
  expressionOutput.leftEyebrowRaised = expressionOutput.currentLeftEyebrowMiddleToRest
    > MIN_CURRENT_TO_REST_MIDDLE_EYEBROW_Y_FOR_EYEBROW_RAISED;
  expressionOutput.rightEyebrowRaised = expressionOutput.currentRightEyebrowMiddleToRest
    > MIN_CURRENT_TO_REST_MIDDLE_EYEBROW_Y_FOR_EYEBROW_RAISED;

  auto leftIrisDiameter =
    (vertices[FACEMESH_L_IRIS_LEFT_CORNER].dist(vertices[FACEMESH_L_IRIS_RIGHT_CORNER])
     + vertices[FACEMESH_L_IRIS_TOP_CORNER].dist(vertices[FACEMESH_L_IRIS_BOTTOM_CORNER]))
    / 2.f;
  auto rightIrisDiameter =
    (vertices[FACEMESH_R_IRIS_LEFT_CORNER].dist(vertices[FACEMESH_R_IRIS_RIGHT_CORNER])
     + vertices[FACEMESH_R_IRIS_TOP_CORNER].dist(vertices[FACEMESH_R_IRIS_BOTTOM_CORNER]))
    / 2.f;
  // 11.7mm is the average iris diameter.
  auto scaleFromIris = ((0.0117f / leftIrisDiameter) + (0.0117f / rightIrisDiameter)) / 2.f;
  auto interpupillaryDistance = vertices[FACEMESH_L_IRIS].dist(vertices[FACEMESH_R_IRIS]);
  auto interpupillaryDistanceInMM = interpupillaryDistance * scaleFromIris * 1000.f;

  auto &ipdState = faceToIPDState_[localFaceDetection.roi.faceId];
  if (
    ipdState.needsMoreData
    // Scale from IPD is highly inaccurate if the user is not facing the camera.
    && headTransform.rotation.radians(Quaternion(0.f, 0.f, 1.f, 0.f)) < MAX_HEAD_ANGLE_IPD_ESTIMATES
    // Scale from IPD is highly inaccurate if the user's eye is closed.
    && expressionOutput.leftEyeOpen && expressionOutput.rightEyeOpen) {
    ipdState.validIPDEstimates.push_back(interpupillaryDistanceInMM);
    if (ipdState.validIPDEstimates.size() > MIN_VALID_IPD_ESTIMATES) {
      ipdState.needsMoreData = false;
      std::sort(ipdState.validIPDEstimates.begin(), ipdState.validIPDEstimates.end());

      // Remove the 20% outliers on either end.
      int numOutliers = 2;
      ipdState.finalIPDEstimate = std::accumulate(
        ipdState.validIPDEstimates.begin() + numOutliers,
        ipdState.validIPDEstimates.end() - numOutliers,
        0.f);
      ipdState.finalIPDEstimate = ipdState.finalIPDEstimate
        / static_cast<float>(ipdState.validIPDEstimates.size() - (numOutliers * 2));
    }
  }

  increaseFaceMeshHeight(TOP_FACE_POINTS, NEARLY_TOP_FACE_POINTS, FACE_TOP_INCREASE, &vertices);

  // Convert the detected points (with increased artificial height) into UV space so users can show
  // the detected face in the camera feed as a texture on the face mesh.
  const auto pointsInCameraSpace = flatten<2>(
    HMatrixGen::intrinsic(localFaceDetection.intrinsics)
    * (anchorMatrix(headTransform) * vertices));
  const auto uvsInCameraSpace =
    map<HPoint2, HVector2>(pointsInCameraSpace, [localFaceDetection](const HPoint2 &pt) {
      return HVector2{
        pt.x() / static_cast<float>(localFaceDetection.intrinsics.pixelsWidth),
        pt.y() / static_cast<float>(localFaceDetection.intrinsics.pixelsHeight)};
    });

  return {
    headTransform,
    vertices,
    normals,
    status(),
    localFaceDetection.roi.faceId,
    attachmentPoints,
    uvsInCameraSpace,
    expressionOutput,
    ipdState.finalIPDEstimate};
}

}  // namespace c8
