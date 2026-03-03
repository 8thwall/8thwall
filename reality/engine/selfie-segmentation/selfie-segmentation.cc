// Copyright (c) 2023 Niantic, Inc.
// Original Author: Lynn Dang (lynndang@nianticlabs.com)

#include "bzl/inliner/rules2.h"
cc_library {
  hdrs = {
    "selfie-segmentation.h",
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
cc_end(0xd9fa3512);

#include <cmath>
#include <numeric>

#include "c8/pixels/pixel-buffer.h"
#include "c8/pixels/pixel-transforms.h"
#include "c8/pixels/pixels.h"
#include "c8/stats/scope-timer.h"
#include "reality/engine/deepnets/tflite-model-operations.h"
#include "reality/engine/selfie-segmentation/selfie-segmentation.h"

namespace c8 {

SelfieSegmentation::SelfieSegmentation(
  const uint8_t *modelData, int modelSize, size_t modelCacheSize)
    : multiclassifier_(
      modelData, modelSize, modelCacheSize, inputHeight_, inputWidth_, numClasses_) {}

// Constructor reads in the TFLite model.
SelfieSegmentation::SelfieSegmentation(const Vector<uint8_t> &modelData, size_t modelCacheSize)
    : multiclassifier_(modelData, modelCacheSize, inputHeight_, inputWidth_, numClasses_) {}

void SelfieSegmentation::processSegmentation(ConstRGBA8888PlanePixels inputImage) {
  // The tflite expects the input tensor size (1, 256, 256, 3).
  ScopeTimer t("process-segmentation-input");

  multiclassifier_.processOutput(inputImage);
}

// Multi-class selfie segmentation model is in square mode with 6 classes:
// ["background", "hair", "body skin", "face skin", "clothes", "others (accessories)"].
// The output tensor has dimension [1, 256, 256, 6].
void SelfieSegmentation::generateSegmentation(
  ConstRGBA8888PlanePixels inputImage,
  Vector<FloatPixels> &segmentationResults,
  const Vector<int> outputClassIds) {
  ScopeTimer t("generate-segmentations");
  processSegmentation(inputImage);

  multiclassifier_.generateOutput(inputImage, segmentationResults, outputClassIds);
}

}  // namespace c8
