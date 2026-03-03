// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "c8/geometry/face-types.h"
#include "c8/geometry/mesh-types.h"
#include "c8/hmatrix.h"
#include "c8/hpoint.h"
#include "c8/vector.h"

namespace c8 {

// Return an upright bounding box for the potentially rotated passed bounding box, so it can be used
// for our intersection of union code.
BoundingBox uprightBBox(const BoundingBox &box1);

// Returns the IOU for two bounding boxes. Known caveats are that it only works with two upright
// bounding boxes and in a space where x+ is right and y+ is up.
float intersectionOverUnion(const BoundingBox &box1, const BoundingBox &box2);

// Calculates the minimum distance between two bounding boxes. Known caveats are that it only works
// with two upright bounding boxes and in a space where x+ is right and y+ is up.
float minBBoxDistance(const BoundingBox &a, const BoundingBox &b);

void calculateBoundingCube(const Vector<HPoint3> &points, HPoint3 *min, HPoint3 *max);

// Transform from rendered texture (in 0-1) to image texture (in 0-1).
HMatrix renderTexToImageTex(const DetectionRoi &roi);

// Transform from rendered texture in fromRoi to rendered texture in toRoi
HMatrix renderTexToRenderTex(const DetectionRoi &fromRoi, const DetectionRoi &toRoi);

// Transform from rendered texture (in 0-1) to ray space.
HMatrix renderTexToRaySpace(const DetectionRoi &roi, const c8_PixelPinholeCameraModel &k);

// Compute the ROI that should be used to anaylze faces on the next frame based on detections in
// the current frame. This includes adding a suitable margin to the detection in order to account
// for frame-frame face motion.
DetectionRoi detectionRoi(const DetectedPoints &detection);

// Compute the detection ROI but without added padding.
DetectionRoi detectionRoiNoPadding(const DetectedPoints &detection);

// Convert the points for a detection from rendered space to source image space.
DetectedImagePoints detectionToImageSpace(const DetectedPoints &pts);

// Convert the points for each detection from the rendered space to source image space.
Vector<DetectedImagePoints> detectionToImageSpace(const Vector<DetectedPoints> &pts);

// Convert the points for a detection bounding box from rendered space to source image space.
BoundingBox detectionBoundingBoxToImageSpace(const BoundingBox &bbox, const DetectionRoi &roi);

// Convert the points for each detection from the rendered space to ray space, preserving depth info
// from the detections unaltered.
DetectedRayPoints detectionToRaySpace(const DetectedPoints &pts);

// Get the reference anchor for this detection result for the detected face position and size.
FaceAnchorTransform computeAnchorTransform(const DetectedRayPoints &rayPoints);

// Use the refence anchor to compute the location of the mesh points in 3d relative to the camera.
Vector<HPoint3> worldPoints(
  const DetectedRayPoints &rayPoints,
  const FaceAnchorTransform &headAnchor,
  const DetectedPoints &pts);

// Convert world points to an anchor's local space.
Vector<HPoint3> worldToLocal(const Vector<HPoint3> &worldPoints, const FaceAnchorTransform &anchor);

// Converts the anchor to a matrix.
HMatrix anchorMatrix(const FaceAnchorTransform &t);

// Return the corners of roi region in the source image.
Vector<HPoint2> roiCornersInImage(const DetectedPoints &pts);

// Adjust the refence anchor based on a computed pose in the anchor space.
FaceAnchorTransform adjustAnchor(const FaceAnchorTransform &anchor, const HMatrix &localPose);

Vector<HPoint3> computeHeadMesh(
  const DetectedRayPoints &rayPoints,
  const FaceAnchorTransform &headAnchor,
  const HMatrix &headAnchorTransform,
  const DetectedPoints &pts);

// Returns a subset of the face points that will be used for determining the pose
Vector<HPoint3> getPosePointsSubset(const Vector<HPoint3> &facePoints);

// Returns a subset of the points specified by the subset indices
Vector<HPoint3> getSubset(const Vector<HPoint3> &points, const Vector<int> &subsetIndices);

// Creates a single anchor
AttachmentPoint createAttachmentPoint(
  const AttachmentPointMsg::AttachmentName &name,
  const HPoint3 &localVertex,
  const HVector3 &vertexNormal);

// Creates a single anchor
AttachmentPoint createAttachmentPoint(
  const AttachmentPointMsg::AttachmentName &name,
  int index,
  const Vector<HPoint3> &localVertices,
  const Vector<HVector3> &vertexNormals);

// Create a face anchor given two indices that are averaged together
AttachmentPoint createAttachmentPoint(
  const AttachmentPointMsg::AttachmentName &name,
  int indexA,
  int indexB,
  const Vector<HPoint3> &localVertices,
  const Vector<HVector3> &vertexNormals);

// Gets key anchor positions and rotations
Vector<AttachmentPoint> getAttachmentPoints(
  const Vector<HPoint3> &localVertices, const Vector<HVector3> &vertexNormals);

// Converts a detected point into 3D space.  This function should not be used in production, but
// should only be used when you want a Face3d struct that doesn't contain smoothing of the points
// via filtering.  Otherwise, you should be using TrackedFaceState::locateFace.
Face3d detectionToMeshNoFilter(const DetectedPoints &faceDetection);

// Given the meshGeometry configuration options, return the corresponding mesh indices.
Vector<MeshIndices> meshIndicesFromMeshGeometry(const FaceMeshGeometryConfig &config);

}  // namespace c8
