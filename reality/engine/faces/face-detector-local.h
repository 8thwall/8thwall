// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "c8/vector.h"
#include "c8/geometry/face-types.h"
#include "reality/engine/deepnets/tflite-interpreter.h"
#include "reality/engine/ears/ear-types.h"

namespace c8 {
constexpr size_t FACE_LOCAL_CACHE_SIZE = 2380000;

// FaceDetectorLocal provides an abstraction layer above the deep net model for analyzing a face
// in detail from a high res region of interest.
class FaceDetectorLocal {
public:
  // Construct from a tflite model.
  FaceDetectorLocal(const uint8_t *modelData, int modelSize, const EarConfig &earConfig)
      : earConfig_(earConfig), interpreter_(modelData, modelSize, FACE_LOCAL_CACHE_SIZE,
      NUM_THREADS) {}
  FaceDetectorLocal(const Vector<uint8_t> &modelData, const EarConfig &earConfig)
      : earConfig_(earConfig), interpreter_(modelData, FACE_LOCAL_CACHE_SIZE, NUM_THREADS) {}

  // Default move constructors.
  FaceDetectorLocal(FaceDetectorLocal &&) = default;
  FaceDetectorLocal &operator=(FaceDetectorLocal &&) = default;

  // Disallow copying.
  FaceDetectorLocal(const FaceDetectorLocal &) = delete;
  FaceDetectorLocal &operator=(const FaceDetectorLocal &) = delete;

  // Detect faces in a zoomed in crop of an image. The src should be 192x192.
  Vector<DetectedPoints> analyzeFace(const RenderedSubImage &src, c8_PixelPinholeCameraModel k);

private:
  static constexpr int NUM_THREADS = 2;
  EarConfig earConfig_;
  TFLiteInterpreter interpreter_;
};

}  // namespace c8
