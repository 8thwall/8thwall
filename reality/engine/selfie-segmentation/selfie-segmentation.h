// Copyright (c) 2023 Niantic, Inc.
// Original Author: Lynn Dang (lynndang@nianticlabs.com)

#pragma once

#include "c8/pixels/pixel-buffer.h"
#include "c8/pixels/pixels.h"
#include "c8/vector.h"
#include "reality/engine/deepnets/tflite-multiclassifier.h"

namespace c8 {

// TODO(lynn): Not sure what to put here.
constexpr size_t SELFIE_SEGMENTATION_CACHE_SIZE = 16138240;

// Utilizes MediaPipe's Image Segmenter which takes in a w256 x h256 input and segment it into
// 0 - background
// 1 - hair
// 2 - body skin
// 3 - face skin
// 4 - clothes
// 5 - others (accessors)
// The number of classes needs to be specified during this class's initialization.
// Code is referencing semanticsClassifier.
class SelfieSegmentation {
public:
  // Construct from a tflite model.
  SelfieSegmentation(
    const uint8_t *modelData,
    int modelSize,
    size_t modelCacheSize = SELFIE_SEGMENTATION_CACHE_SIZE);
  SelfieSegmentation(
    const Vector<uint8_t> &modelData,
    size_t modelCacheSize = SELFIE_SEGMENTATION_CACHE_SIZE);

  // Default move constructors.
  SelfieSegmentation(SelfieSegmentation &&) = default;
  SelfieSegmentation &operator=(SelfieSegmentation &&) = default;

  // Disallow copying.
  SelfieSegmentation(const SelfieSegmentation &) = delete;
  SelfieSegmentation &operator=(const SelfieSegmentation &) = delete;

  int getInputWidth() const { return inputWidth_; }
  int getInputHeight() const { return inputHeight_; }
  int getNumberOfClasses() const { return numClasses_; }

  // Given an image, return a vector of float pixels with the class scores from the model.
  // The outputClassIds is the vector of class Ids for segmentationResults.
  // If outputClassIds is empty, fill the segmentationResults with results from all classes.
  void generateSegmentation(
    ConstRGBA8888PlanePixels inputImage,
    Vector<FloatPixels> &segmentationResults,
    const Vector<int> outputClassIds);

private:
  void processSegmentation(ConstRGBA8888PlanePixels inputImage);

  TfLiteMultiClassifier multiclassifier_;

  // Always expect square dimension input
  static constexpr int inputWidth_ = 256;
  static constexpr int inputHeight_ = 256;
  static constexpr int numClasses_ = 6;
};

}  // namespace c8
