// Copyright (c) 2022 8th Wall, LLC
// Original Author: Yuyan Song (yuyansong@nianticlabs.com)

#pragma once

#include "c8/geometry/box2.h"
#include "c8/hvector.h"
#include "c8/vector.h"

namespace c8 {

////////////////////////////////////////// ANCHOR /////////////////////////////////////////

struct Anchor {
  float xCenter = 0.0f;
  float yCenter = 0.0f;
  float w = 0.0f;
  float h = 0.0f;
};

struct AnchorOptions {
  int inputSizeHeight = 128;
  int inputSizeWidth = 128;
  float minScale = 0.1484375f;
  float maxScale = 0.75f;
  float anchorOffsetX = 0.5f;
  float anchorOffsetY = 0.5f;
  float numLayers = 4;
  Vector<int> featureMapHeight;
  Vector<int> featureMapWidth;
  Vector<int> strides{8, 16, 16, 16};
  Vector<float> aspectRatios{1.0f};
  bool reduceBoxesInLowestLayer = false;
  float interpolatedScaleAspectRatio = 1.0f;
  bool fixedAnchorSize = true;
};

// Generate anchors for SSD object detection model.
// Input:
//   Anchor generation options
// Output:
//   ANCHORS: A list of anchors. Model generates predictions based on the
//   offsets of these anchors.
//
// @param options anchor generation options
// @return generated anchors
Vector<Anchor> generateAnchors(const AnchorOptions &options);

////////////////////////////////////////// Detections /////////////////////////////////////////

// types
using RelativeBoundingBox = Box2;
using RelativeKeypoint = HVector2;

struct LocationData {
  String format;
  RelativeBoundingBox relativeBoundingBox;
  Vector<RelativeKeypoint> relativeKeypoints;
};

struct Detection {
  float score = 0;
  int labelId = 0;
  LocationData locationData;
};

// Tensors to detections

struct ProcessOptions {
  int numClasses = 1;
  int numBoxes = 896;
  int numCoords = 16;
  int keypointCoordOffset = 4;
  int numKeypoints = 6;
  int numValuesPerKeypoint = 2;
  int boxCoordOffset = 0;
  float xScale = 128.0f;
  float yScale = 128.0f;
  float wScale = 128.0f;
  float hScale = 128.0f;
  bool applyExponentialOnBoxSize = false;
  bool reverseOutputOrder = true;
  Vector<int> ignoreClasses = {};
  bool sigmoidScore = true;
  float scoreClippingThresh = 100.0f;
  bool flipVertically = false;
  float minScoreThresh = 0.75f;
};

// Convert result TFLite tensors from object detection models into MediaPipe Detections.
// Input:
//  TENSORS - Vector of TfLiteTensor of type kTfLiteFloat32. The vector of tensors can have 2 or 3
//            tensors. First tensor is the predicted raw boxes/keypoints. The size of the values
//            must be (numBoxes * numPredictedValues). Second tensor is the score tensor. The
//            size of the values must be (numBoxes * numClasses). It's optional to pass in a third
//            tensor for anchors (e.g. for SSD models) depend on the outputs of the detection model.
//            The size of anchor tensor must be (numBoxes * 4).
// Output:
//  DETECTIONS - Result MediaPipe detections.
//
// @param rawBoxes raw box data
// @param rawScores raw confidence scores
// @param anchors detection anchors
// @param options detection processing options
// @return MediaPipe detections
Vector<Detection> processDetections(
  const float *rawBoxes,
  const float *rawScores,
  const Vector<Anchor> &anchors,
  const ProcessOptions &options);

////////////////////////////////////////// NMS /////////////////////////////////////////

// Weighted NonMaxSuppression (NMS) where the detections are weighted by their confidence
// Input: initial detections
// Output: Detections after NMS
//
// @param inDetections initial detections
// @param iouThreshold IOU threshold for NMS
Vector<Detection> weightedNonMaxSuppression(
  const Vector<Detection> &inDetections, float iouThreshold);

}  // namespace c8
