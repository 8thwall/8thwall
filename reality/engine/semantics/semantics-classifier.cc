// Copyright (c) 2022 Niantic, Inc.
// Original Author: Lynn Dang (lynndang@nianticlabs.com)

#include "bzl/inliner/rules2.h"
cc_library {
  hdrs = {
    "semantics-classifier.h",
  };
  deps = {
    "//c8/pixels:pixels",
    "//c8/pixels:pixel-buffer",
    "//c8/pixels:pixel-transforms",
    "//c8/stats:scope-timer",
    "//c8:vector",
    "//reality/engine/deepnets:tflite-multiclassifier",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x5288b52d);

#include <cmath>
#include <numeric>

#include "c8/pixels/pixel-buffer.h"
#include "c8/pixels/pixel-transforms.h"
#include "c8/pixels/pixels.h"
#include "c8/stats/scope-timer.h"
#include "reality/engine/semantics/semantics-classifier.h"

namespace c8 {

SemanticsClassifier::SemanticsClassifier(
  const uint8_t *modelData,
  int modelSize,
  int numClasses,
  InputOrientation inputOrient,
  size_t modelCacheSize)
    : multiclassifier_(
      modelData,
      modelSize,
      modelCacheSize,
      SEMANTICS_INPUT_HEIGHT,
      SEMANTICS_INPUT_WIDTH,
      numClasses) {
  setOrientation(inputOrient);
}

// Constructor reads in the TFLite model.
SemanticsClassifier::SemanticsClassifier(
  const Vector<uint8_t> &modelData,
  int numClasses,
  InputOrientation inputOrient,
  size_t modelCacheSize)
    : multiclassifier_(
      modelData, modelCacheSize, SEMANTICS_INPUT_HEIGHT, SEMANTICS_INPUT_WIDTH, numClasses) {
  setOrientation(inputOrient);
}

void SemanticsClassifier::setOrientation(InputOrientation inputOrient) {
  inputOrientation_ = inputOrient;
}

void SemanticsClassifier::processSemantics(ConstRGBA8888PlanePixels inputImage) {
  // The tflite expects the input tensor size (1, 256, 144, 3).
  multiclassifier_.processOutput(inputImage);
}

// Multi-class semantics model is in landscape mode with 20 different classes:
// [“sky”, “ground”, “natural_ground”, “artificial_ground”, “water”, “person”, “building”,
// “flower”, “foliage”, “tree_trunk”, “pet”, “sand”, “grass”, “tv”, “dirt”, “vehicle”, “road”,
// “food”, “loungeable”, “snow”].
// The output tensor has dimension [1, 256, 144, 20].
// Channel 0 is depth. Since it is currently not being trained, we skip it and only output the 20
// classes.
void SemanticsClassifier::generateSemantics(
  ConstRGBA8888PlanePixels inputImage,
  Vector<FloatPixels> &semanticsResults,
  const Vector<int> outputClassIds) {
  ScopeTimer t("generate-semantics");
  processSemantics(inputImage);

  multiclassifier_.generateOutput(inputImage, semanticsResults, outputClassIds);
}

ConstOneChannelFloatPixels SemanticsClassifier::obtainSemantics(
  ConstRGBA8888PlanePixels inputImage) {
  ScopeTimer t("obtain-semantics");
  processSemantics(inputImage);

  return multiclassifier_.obtainSingleOutput(inputImage, 0);
}

}  // namespace c8
