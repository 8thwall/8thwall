// Copyright (c) 2023 Niantic Labs
// Original Author: Yuyan Song (yuyansong@nianticlabs.com)

#pragma once

#include "c8/geometry/face-types.h"
#include "c8/map.h"
#include "c8/vector.h"
#include "reality/engine/tracking/ray-point-filter.h"

constexpr int EAR_LANDMARK_DETECTION_NUM_PER_EAR = 3;

constexpr int EAR_LANDMARK_DETECTION_LOBE = 0;
constexpr int EAR_LANDMARK_DETECTION_CANAL = 1;
constexpr int EAR_LANDMARK_DETECTION_HELIX = 2;

// This input dimension will be used to do zoom-in rendering of the ears into a w112xh320 texture.
// There are 2 vertically stacked sections in this texture -
// - the top half is the zoom-in rendering of the LEFT ear with w112xh160 pixels.
// - the bottom half is the zoom-in & x-flipped rendering of the RIGHT ear with w112xh160 pixels.
constexpr int EAR_LANDMARK_DETECTION_INPUT_WIDTH = 112;
constexpr int EAR_LANDMARK_DETECTION_INPUT_HEIGHT = 160;

constexpr float EAR_LANDMARK_DETECTION_INPUT_ASPECT_RATIO =
  static_cast<float>(EAR_LANDMARK_DETECTION_INPUT_WIDTH)
  / static_cast<float>(EAR_LANDMARK_DETECTION_INPUT_HEIGHT);

constexpr float EAR_LANDMARK_DETECTION_VISIBILITY_THRESHOLD = 0.5f;

// face mesh vertex indices used for lift3D
// search for 'MediapipePairs' in
// https://gitlab.<REMOVED_BEFORE_OPEN_SOURCING>.com/niantic-ar/research/face/-/blob/main/src/face/utils/lift_3d.py
extern const c8::Vector<c8::Vector<int>> EAR_SIDES_MODELS_LEFT;
extern const c8::Vector<c8::Vector<int>> EAR_SIDES_MODELS_RIGHT;

namespace c8 {
struct EarConfig {
  // Should we do ear detection work
  bool isEnabled = false;
};

struct EarRenderResult {
  int srcTex;
  ConstRGBA8888PlanePixels rawPixels;
  Vector<RenderedSubImage> earDetectImages;
};

// There is a pair of ears per face with left ear being the first and right ear being the second.
struct EarTrackingResult {
  const Vector<DetectedPoints> *localEars;
  const Vector<Ear3d> *earData;
  const Vector<int> *lostIds;
};

// Config for specifying ear geometry related parameters.
struct EarGeometryConfig {
  bool showVertices = true;
  bool showVerticesVisibleOnly = true;
  float earPointSize = 0.01f;
};

// Return if the vertex is visible.
bool isEarVertexVisible(float visibility);
// Should we use this vertex data for averaging.
bool isEarVertexTrustable(float visibility);

struct EarBoundingBoxDebug {
  BoundingBox left;
  BoundingBox right;
};

/** Based on local face detection and ear side, return ear ROIs
 * @param earBb The bounding boxes of the ears in face ROI for debug. Optional.
 */
Vector<DetectedPoints> earRoisByFaceMesh(
  const DetectedPoints &face, EarBoundingBoxDebug *earBb = nullptr);

// Lift ear 2D landmark data into 3D vertices
// The ear point depths are computed in (0,1) image space.
// @param localFace the detected face points. If empty, this returns nothing.
// @param leftEar the detected left ear points (2 from face, 3 from ear)
// @param leftEar the detected right ear points (2 from face, 3 from ear)
// @param face face data in 3d
// @param intrinsics camera intrinsics
// @returns ear location in 3d
Ear3d earLiftLandmarksTo3D(
  const DetectedPoints &localFace,
  const DetectedPoints &leftEar,
  const DetectedPoints &rightEar,
  const Face3d &face,
  const c8_PixelPinholeCameraModel &intrinsics);

// Mirror invisible vertices from visible ones for Ear3d results
void mirrorInvisibleEarVertices(const HPoint3 &leftRefPt, const HPoint3 &rightRefPt, Ear3d *ear);

// compute attachment points
void computeEarAttachmentPoints(Ear3d *ear3d);
// update ear states
void updateEarStates(Ear3d *ear3d);

// Average the ear vertices to get the stable vertices
Ear3d averageEar3d(
  const Ear3d &ear3d,
  const EarPoseBinIdx &binIdx,
  const EarPoseBinIdx &binDims,
  float leftResetThreshold,
  float rightResetThreshold,
  HashMap<int, EarSampledVerticesFullView> *avgEarMap);

// Vertices are in local space w.r.t. the head transform. Each vertex has a single filter. Filters
// have different parameters based on the vertex index in the array.
struct EarAveragedVertices {
  Vector<HPoint3> leftVertices;
  Vector<RayPointFilter3a> leftFilters;

  Vector<HPoint3> rightVertices;
  Vector<RayPointFilter3a> rightFilters;
};
Ear3d updateAverageEar(const Ear3d &ear3d, HashMap<int, EarAveragedVertices> *faceIdToAverageEar);

// Test if we have sampled N times the ear vertices for this (faceId, face rotation bin y, face
// rotation bin z)
bool isEar3DSampled(
  HashMap<int, EarSampledVerticesFullView> &avgEarMap, int faceId, const EarPoseBinIdx &binIdx);

// Get sampled ear vertices for the 'face' and its rotation angle
void getEar3DSampled(
  HashMap<int, EarSampledVerticesFullView> &avgEarMap,
  const Face3d &face,
  const EarPoseBinIdx &binIdx,
  Ear3d *ear3d);

}  // namespace c8
