// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "c8/geometry/face-types.h"
#include "c8/vector.h"
#include "reality/engine/deepnets/detection-anchor-nms.h"
#include "reality/engine/deepnets/tflite-interpreter.h"

namespace c8 {
constexpr size_t FACE_GLOBAL_CACHE_SIZE = 380000;

// FaceDetectorGlobal provides an abstraction layer above the deep net model for detecting multiple
// faces in a full image. The data extracted about each face are relatively light-weight, and
// typically global face detection is a precurosor to local face detection which takes a zoomed-in
// image of each face and extracts detailed information.
class FaceDetectorGlobal {
public:
  // Construct from a tflite model.
  FaceDetectorGlobal(const uint8_t *modelData, int modelSize)
      : interpreter_(modelData, modelSize, FACE_GLOBAL_CACHE_SIZE, NUM_THREADS) {
    initializeDetection();
  }
  FaceDetectorGlobal(const Vector<uint8_t> &modelData)
      : interpreter_(modelData, FACE_GLOBAL_CACHE_SIZE, NUM_THREADS) {
    initializeDetection();
  }

  // Default move constructors.
  FaceDetectorGlobal(FaceDetectorGlobal &&) = default;
  FaceDetectorGlobal &operator=(FaceDetectorGlobal &&) = default;

  // Disallow copying.
  FaceDetectorGlobal(const FaceDetectorGlobal &) = delete;
  FaceDetectorGlobal &operator=(const FaceDetectorGlobal &) = delete;

  // Detect faces in a low resolution render of an image. The low resolution src should have its
  // longest dimension exactly equal to 128.
  Vector<DetectedPoints> detectFaces(RenderedSubImage src, c8_PixelPinholeCameraModel k);

private:
  static constexpr int NUM_THREADS = 2;
  TFLiteInterpreter interpreter_;

  Vector<Anchor> anchors_;
  ProcessOptions decodeOptions_;
  void initializeDetection();
};

}  // namespace c8
