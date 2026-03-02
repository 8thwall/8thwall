// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "c8/geometry/box2.h"
#include "c8/geometry/pixel-pinhole-camera-model.h"
#include "c8/hmatrix.h"
#include "c8/hpoint.h"
#include "c8/hvector.h"
#include "c8/pixels/image-roi.h"
#include "c8/pixels/pixel-buffer.h"
#include "c8/pixels/pixels.h"
#include "c8/quaternion.h"
#include "c8/string.h"
#include "c8/vector.h"
#include "reality/engine/api/face.capnp.h"

namespace c8 {

// Corner points of a logical box. These could be rotated from upright.
struct BoundingBox {
  HPoint2 upperLeft;
  HPoint2 upperRight;
  HPoint2 lowerLeft;
  HPoint2 lowerRight;

  float width() const {
    float xd = upperRight.x() - upperLeft.x();
    float yd = upperRight.y() - upperLeft.y();
    return std::sqrt(xd * xd + yd * yd);
  }

  float height() const {
    float xd = lowerLeft.x() - upperLeft.x();
    float yd = lowerLeft.y() - upperLeft.y();
    return std::sqrt(xd * xd + yd * yd);
  }

  HPoint2 center() const {
    return {
      .25f * (upperRight.x() + upperLeft.x() + lowerRight.x() + lowerLeft.x()),
      .25f * (upperRight.y() + upperLeft.y() + lowerRight.y() + lowerLeft.y()),
    };
  }
};

// Config for specifying which indices of the face mesh you want returned.
struct FaceMeshGeometryConfig {
  bool showFace = false;
  bool showEyes = false;
  bool showMouth = false;
  bool showIris = false;
};

// Contains world transformation information about the face
struct FaceAnchorTransform {
  HPoint3 position;
  Quaternion rotation;
  float scale;
  float scaledWidth;
  float scaledHeight;
  float scaledDepth;
};

struct AttachmentPoint {
  AttachmentPointMsg::AttachmentName name;
  HPoint3 position;
  Quaternion rotation;
};

// Stores state about the face's expressions.
struct FaceExpressionOutput {
  bool mouthOpen = false;
  bool leftEyeOpen = false;
  bool rightEyeOpen = false;
  bool leftEyebrowRaised = false;
  bool rightEyebrowRaised = false;

  float lipsDistance = 0.0f;
  float leftEyelidDistance = 0.0f;
  float rightEyelidDistance = 0.0f;
  // Compares the current middle point of the eyebrow to the user's middle point when in a neutral
  // state.
  float currentLeftEyebrowMiddleToRest = 0.0f;
  float currentRightEyebrowMiddleToRest = 0.0f;
  // TODO(nathan): remove this two.
  float nasalBridgeToLeftEyebrowDistance = 0.0f;
  float nasalBridgeToRightEyebrowDistance = 0.0f;
};

// This contains the face information that we provide to the user
struct Face3d {
  enum TrackingStatus {
    UNSPECIFIED = 0,
    FOUND = 1,
    UPDATED = 2,
    LOST = 3,
  };

  FaceAnchorTransform transform;
  // vertices are in local space w.r.t. the head transform
  Vector<HPoint3> vertices;
  // vertex normals are also in local space
  Vector<HVector3> normals;

  Face3d::TrackingStatus status = Face3d::TrackingStatus::UNSPECIFIED;
  int id = -1;

  // Key points for the user to attach objects to easily
  Vector<AttachmentPoint> attachmentPoints;

  // The face mesh points projected onto the original camera feed. This means you can put the user's
  // face onto the mesh for face morphing effects.
  Vector<HVector2> uvsInCameraSpace;

  FaceExpressionOutput expressionOutput;

  // Final estimated distance between the center of the two pupils in millimeters
  float interpupillaryDistanceInMM = 0.0f;
};

// States for ears
struct EarStatesOutput {
  bool leftEarFound = false;   // true if all 3 points of left ear are visible
  bool rightEarFound = false;  // true if all 3 points of right ear are visible

  bool leftLobeFound = false;
  bool leftCanalFound = false;
  bool leftHelixFound = false;

  bool rightLobeFound = false;
  bool rightCanalFound = false;
  bool rightHelixFound = false;
};

struct Ear3d {
  int faceId = -1;
  FaceAnchorTransform transform;

  // keep track of ear points' visibilities
  Vector<float> leftVisibilities;
  Vector<float> rightVisibilities;

  // vertices are in local space w.r.t. the head transform
  Vector<HPoint3> leftVertices;
  Vector<HPoint3> rightVertices;

  // Key points for the user to attach objects to easily
  Vector<AttachmentPoint> leftAttachmentPoints;
  Vector<AttachmentPoint> rightAttachmentPoints;

  EarStatesOutput earStatesOutput;
};

struct EarPoseBinIdx {
  size_t yBin;
  size_t zBin;
};

// Sampled ear vertices per face
struct EarSampledVerticesFullView {
  // Sample ear vertices by averaging ear 3D vertices from the first batch of frames
  // there is one record per y-rotation angle range.
  struct EarSampledVertices {
    // vertices are in local space w.r.t. the head transform
    // averaged from the vertices in the first batch of frames
    Vector<HPoint3> leftVertices;
    Vector<HPoint3> rightVertices;

    // store average visibility scores to see if the vertex is visible on average for this angle
    Vector<float> leftVis;
    Vector<float> rightVis;

    // keep track of how many times the vertices have been averaged. One for each vertex in
    // leftVertices.
    Vector<size_t> leftVertCount;
    Vector<size_t> rightVertCount;
  };

  void resizeAngleCache(size_t yBinCount, size_t zBinCount) {
    sampledAngleMap =
      Vector<Vector<EarSampledVertices>>(zBinCount, Vector<EarSampledVertices>(yBinCount));
  }

  const EarSampledVertices &getSampledVertices(const EarPoseBinIdx &idx) const {
    return sampledAngleMap[idx.zBin][idx.yBin];
  }

  EarSampledVertices &getSampledVertices(const EarPoseBinIdx &idx) {
    return sampledAngleMap[idx.zBin][idx.yBin];
  }

  size_t numZBin() const {
    return sampledAngleMap.size();
  }

  size_t numYBin() const {
    return sampledAngleMap.size() > 0 ? sampledAngleMap[0].size() : 0;
  }

  int faceId = -1;

  // Map z-rotation degree to zBin as the first index
  // Map y-rotation degree to yBin as the second index
  Vector<Vector<EarSampledVertices>> sampledAngleMap;
};

// Mapping from vertices in the source clip space to points in the ROI viewport clip space.
// Particularly, to reconstruct the corners of the ROI in the source image, mulitiply
// roi.inv() * {{-1, -1, 1}, {1, -1, 1}, {1, 1, 1}, {-1, 1, 1}} and then convert from clip
// space to image space by adding 1, dividing by 2, and multiplying by image width and height.
using DetectionRoi = ImageRoi;

// Location where an image is drawn in a canvas.
using ImageViewport = Box2;

// Location of a point within its viewport in 0:1 along each direction.
using RoiPoint = HPoint3;

using MeshFace = std::array<int, 3>;

// Detected image points within an ROI. The points are scaled from 0 to 1 along each dimension of
// the ROI.
// detectedClass - for face detection, this value is always 0;
//                 for hand detection, this value reflects whether left or right hand is detected.
// viewport - ROI image viewport, see 'ImageViewport'
// roi - see 'ImageRoi'
// boundingBox - bounding box of the key points
// points - detected points
// intrinsics - camera intrinsics
// extendedBoundingBox - Optional. The box after applying paddings to the 'boundingBox'.
struct DetectedPoints {
  float confidence = 0;
  int detectedClass = 0;
  ImageViewport viewport;
  DetectionRoi roi;
  BoundingBox boundingBox;
  Vector<RoiPoint> points;
  c8_PixelPinholeCameraModel intrinsics;
  BoundingBox extendedBoundingBox;
};

// Detected image points in the space of the source image, scaled from 0 to 1 along each dimension.
using DetectedImagePoints = DetectedPoints;

// Detected points in ray space
using DetectedRayPoints = DetectedPoints;

// Constant for all detected models, should be used for one-time initialization.
struct ModelGeometry {
  int maxDetections = 0;
  int pointsPerDetection = 0;
  Vector<MeshFace> indices;
  Vector<HVector2> uvs;
};

struct RenderedSubImage {
  ImageViewport viewport;
  ConstRGBA8888PlanePixels image;
  DetectionRoi roi;
};

struct FaceRenderResult {
  int srcTex;
  ConstRGBA8888PlanePixels rawPixels;
  RenderedSubImage faceDetectImage;
  Vector<RenderedSubImage> faceMeshImages;
};

struct FaceMeshGpuResult {
  FaceRenderResult facerender;
};

struct FaceMeshCpuResult {
  Vector<DetectedImagePoints> faces;
  Vector<DetectedImagePoints> meshes;
};

ConstRGBA8888PlanePixels imageForViewport(ConstRGBA8888PlanePixels p, ImageViewport vp);

}  // namespace c8
