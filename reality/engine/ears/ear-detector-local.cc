// Copyright (c) 2023 Niantic Labs
// Original Author: Yuyan Song (yuyansong@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "ear-detector-local.h",
  };
  deps = {
    ":ear-types",
    "//c8:parameter-data",
    "//c8:vector",
    "//c8/geometry:face-types",
    "//c8/stats:scope-timer",
    "//c8/pixels:pixel-transforms",
    "//reality/engine/deepnets:tflite-interpreter",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x57e32434);

#include <cmath>

#include "c8/c8-log.h"
#include "c8/parameter-data.h"
#include "c8/pixels/pixel-transforms.h"
#include "c8/stats/scope-timer.h"
#include "reality/engine/ears/ear-detector-local.h"
#include "reality/engine/ears/ear-types.h"

namespace c8 {

namespace {

float averageFloats(const float *data, const int inputNum) {
  if (!data || inputNum == 0) {
    return 0;
  }
  float total = 0;
  for (int i = 0; i < inputNum; ++i) {
    total += data[i];
  }
  return total / static_cast<float>(inputNum);
}

}  // namespace

Vector<DetectedPoints> EarDetectorLocal::analyzeEars(
  const RenderedSubImage &srcLeft, const RenderedSubImage &srcRight, c8_PixelPinholeCameraModel k) {
  ScopeTimer t("ear-detect-local-analyze-ears");
  float *inputDst = interpreter_->typed_input_tensor<float>(0);
  toLetterboxRGBFloat0To1(
    srcLeft.image,
    EAR_LANDMARK_DETECTION_INPUT_WIDTH,
    EAR_LANDMARK_DETECTION_INPUT_HEIGHT,
    inputDst);

  inputDst += 3 * EAR_LANDMARK_DETECTION_INPUT_WIDTH * EAR_LANDMARK_DETECTION_INPUT_HEIGHT;
  toLetterboxRGBFloat0To1FlipX(
    srcRight.image,
    EAR_LANDMARK_DETECTION_INPUT_WIDTH,
    EAR_LANDMARK_DETECTION_INPUT_HEIGHT,
    inputDst);

  {
    ScopeTimer t1("ear-detect-local-invoke");
    interpreter_->Invoke();
  }

  // output tensor 0 - "PartitionedCall:2" float32[2,3] tensor for left and right ear landmark
  // visibilities
  // output tensor 1 - "PartitionedCall:1" float32[2,3,2] tensor for left and right ear
  // 2D landmarks
  const auto *visibilityTensor = interpreter_->typed_output_tensor<float>(0);
  const auto *landmarkTensor = interpreter_->typed_output_tensor<float>(1);

  // Get 2D landmarks and visibilities
  Vector<HPoint2> leftLandmarks;
  for (int i = 0; i < EAR_LANDMARK_DETECTION_NUM_PER_EAR; ++i) {
    leftLandmarks.push_back({landmarkTensor[2 * i], landmarkTensor[2 * i + 1]});
  }
  // for right ear, flip x to get the correct locations
  Vector<HPoint2> rightLandmarks;
  for (int i = EAR_LANDMARK_DETECTION_NUM_PER_EAR; i < 2 * EAR_LANDMARK_DETECTION_NUM_PER_EAR;
       ++i) {
    rightLandmarks.push_back({1.0f - landmarkTensor[2 * i], landmarkTensor[2 * i + 1]});
  }

  Vector<float> leftVis;
  for (int i = 0; i < EAR_LANDMARK_DETECTION_NUM_PER_EAR; ++i) {
    leftVis.push_back(visibilityTensor[i]);
  }
  Vector<float> rightVis;
  for (int i = EAR_LANDMARK_DETECTION_NUM_PER_EAR; i < 2 * EAR_LANDMARK_DETECTION_NUM_PER_EAR;
       ++i) {
    rightVis.push_back(visibilityTensor[i]);
  }

  float leftConfidence = averageFloats(visibilityTensor, EAR_LANDMARK_DETECTION_NUM_PER_EAR);
  float rightConfidence = averageFloats(
    visibilityTensor + EAR_LANDMARK_DETECTION_NUM_PER_EAR, EAR_LANDMARK_DETECTION_NUM_PER_EAR);

  DetectedPoints left{
    leftConfidence,
    0,
    srcLeft.viewport,
    srcLeft.roi,
    {},  // bounding box -- will fill later.
    {},  // points -- will fill later.
    k,
  };

  // each point is {landmark_x, landmark_y, visibility}
  for (int i = 0; i < EAR_LANDMARK_DETECTION_NUM_PER_EAR; ++i) {
    left.points.push_back({leftLandmarks[i].x(), leftLandmarks[i].y(), leftVis[i]});
  }

  DetectedPoints right{
    rightConfidence,
    1,
    srcRight.viewport,
    srcRight.roi,
    {},  // bounding box -- will fill later.
    {},  // points -- will fill later.
    k,
  };

  // each point is {landmark_x, landmark_y, visibility}
  for (int i = 0; i < EAR_LANDMARK_DETECTION_NUM_PER_EAR; ++i) {
    right.points.push_back({rightLandmarks[i].x(), rightLandmarks[i].y(), rightVis[i]});
  }

  return {left, right};
}

}  // namespace c8
