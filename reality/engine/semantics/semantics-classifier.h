// Copyright (c) 2022 Niantic, Inc.
// Original Author: Lynn Dang (lynndang@nianticlabs.com)

#pragma once

#include "c8/pixels/pixel-buffer.h"
#include "c8/pixels/pixels.h"
#include "c8/vector.h"
#include "reality/engine/deepnets/tflite-multiclassifier.h"

namespace c8 {

constexpr int SEMANTICS_INPUT_WIDTH = 144;
constexpr int SEMANTICS_INPUT_HEIGHT = 256;
constexpr int SEMANTICS_CLASS_NUM = 20;

constexpr size_t SKY_XNNPACK_CACHE_SIZE = 6012928;

// Utilizes Niantic's semantics generator as a tflite.
// The input dimension is w144 x h256 for portrait mode for current sky semantics model.
// The model convolution kernel weights will be rotated for the landscape modes in order to get
// better inference results.
// The number of classes needs to be specified during this class's initialization.
class SemanticsClassifier {
public:
  enum class InputOrientation { PORTRAIT, PORTRAIT_UPSIDE_DOWN, LANDSCAPE_LEFT, LANDSCAPE_RIGHT };

  // Construct from a tflite model.
  SemanticsClassifier(
    const uint8_t *modelData,
    int modelSize,
    int numClasses = SEMANTICS_CLASS_NUM,
    InputOrientation inputOrient = InputOrientation::PORTRAIT,
    size_t modelCacheSize = SKY_XNNPACK_CACHE_SIZE);
  SemanticsClassifier(
    const Vector<uint8_t> &modelData,
    int numClasses = SEMANTICS_CLASS_NUM,
    InputOrientation inputOrient = InputOrientation::PORTRAIT,
    size_t modelCacheSize = SKY_XNNPACK_CACHE_SIZE);

  // Default move constructors.
  SemanticsClassifier(SemanticsClassifier &&) = default;
  SemanticsClassifier &operator=(SemanticsClassifier &&) = default;

  // Disallow copying.
  SemanticsClassifier(const SemanticsClassifier &) = delete;
  SemanticsClassifier &operator=(const SemanticsClassifier &) = delete;

  int getInputWidth() const { return multiclassifier_.getInputWidth(); }
  int getInputHeight() const { return multiclassifier_.getInputHeight(); }
  int getNumberOfClasses() const { return multiclassifier_.getNumberOfClasses(); }

  void setOrientation(InputOrientation inputOrient);
  InputOrientation getOrientation() const { return inputOrientation_; }

  // Given an image, return a vector of float pixels with the class scores from the model.
  // The outputClassIds is the vector of class Ids for semanticsResults.
  // If outputClassIds is empty, fill the semanticsResults with results from all classes.
  void generateSemantics(
    ConstRGBA8888PlanePixels inputImage,
    Vector<FloatPixels> &semanticsResults,
    const Vector<int> outputClassIds);

  // Given an image, return float pixels for the first class from the model.
  ConstOneChannelFloatPixels obtainSemantics(ConstRGBA8888PlanePixels inputImage);

private:
  void processSemantics(ConstRGBA8888PlanePixels inputImage);

  TfLiteMultiClassifier multiclassifier_;  // Muliticlass Interpreter for semantics tflite.

  // The orientation variable will be used to determine which tflite interpreter to use.
  InputOrientation inputOrientation_ = InputOrientation::PORTRAIT;
};

}  // namespace c8
