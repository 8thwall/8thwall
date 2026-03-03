// Copyright (c) 2023 Niantic, Inc.
// Original Author: Lynn Dang (lynndang@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "tflite-multiclassifier.h",
  };
  deps = {
    ":tflite-interpreter",
    ":tflite-model-operations",
    "//c8:c8-log",
    "//c8/pixels:pixels",
    "//c8/pixels:pixel-buffer",
    "//c8/pixels:pixel-transforms",
    "//c8/stats:scope-timer",
    "//c8:vector",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x293f368a);

#include <cmath>
#include <numeric>

#include "c8/c8-log.h"
#include "c8/pixels/pixel-buffer.h"
#include "c8/pixels/pixel-transforms.h"
#include "c8/pixels/pixels.h"
#include "c8/stats/scope-timer.h"
#include "reality/engine/deepnets/tflite-model-operations.h"
#include "reality/engine/deepnets/tflite-multiclassifier.h"

namespace c8 {

namespace {}  // namespace

TfLiteMultiClassifier::TfLiteMultiClassifier(
  const uint8_t *modelData,
  int modelSize,
  size_t modelCacheSize,
  int inputHeight,
  int inputWidth,
  int numClasses)
    : interpreter_(modelData, modelSize, modelCacheSize),
      inputWidth_(inputWidth),
      inputHeight_(inputHeight),
      numClasses_(numClasses) {
  initInputOutputDims();
}

// Constructor reads in the TFLite model.
TfLiteMultiClassifier::TfLiteMultiClassifier(
  const Vector<uint8_t> &modelData,
  size_t modelCacheSize,
  int inputHeight,
  int inputWidth,
  int numClasses)
    : interpreter_(modelData, modelCacheSize),
      inputWidth_(inputWidth),
      inputHeight_(inputHeight),
      numClasses_(numClasses) {
  initInputOutputDims();
}

void TfLiteMultiClassifier::initInputOutputDims() {
  // get model input & output tensor dims
  auto inputDims = interpreter_.getInputTensorDims(0);
  auto outputDims = interpreter_.getOutputTensorDims(0);

  if (inputWidth_ != inputDims[2]) {
    C8Log(
      "ERROR: [tflite-multiclassifier] model input width is %d while the customized width is %d",
      inputDims[2],
      inputWidth_);
  }
  if (inputHeight_ != inputDims[1]) {
    C8Log(
      "ERROR: [tflite-multiclassifier] model input height is %d while the customized height is %d",
      inputDims[1],
      inputHeight_);
  }
  if (numClasses_ != outputDims[3]) {
    C8Log(
      "ERROR: [tflite-multiclassifier] model number of classes is %d while the customized number "
      "of classes is %d",
      outputDims[3],
      numClasses_);
  }
}

void TfLiteMultiClassifier::processOutput(ConstRGBA8888PlanePixels inputImage) {
  // The tflite expects the input tensor size (1, height, width, 3).
  {
    ScopeTimer t("process-multiclass-input");

    float *dst = interpreter_->typed_input_tensor<float>(0);
    const uint8_t *src = inputImage.pixels();
    const uint8_t *srcStart = src;
    const uint8_t *srcEnd = srcStart + inputImage.cols() * 4;

    float *outpix = dst;
    for (int r = 0; r < inputImage.rows(); ++r) {
      src = srcStart;
      while (src != srcEnd) {
        // Scale the images to 0:1.
        outpix[0] = src[0] / 255.f;
        outpix[1] = src[1] / 255.f;
        outpix[2] = src[2] / 255.f;

        src += 4;
        outpix += 3;
      }
      srcStart += inputImage.rowBytes();
      srcEnd += inputImage.rowBytes();
    }
  }

  // Run the tflite on the input.
  {
    ScopeTimer t("process-multiclass-invoke");

    interpreter_->Invoke();
  }
}

// Multi-class model outputs several outputs to different classes:
// The output tensor has dimension [1, height, width, numClasses].
void TfLiteMultiClassifier::generateOutput(
  ConstRGBA8888PlanePixels inputImage,
  Vector<FloatPixels> &multiclassResults,
  const Vector<int> outputClassIds) {
  ScopeTimer t("generate-multiclass-output");

  Vector<int> selectedClassIds = outputClassIds;
  // if empty, output all classes
  if (outputClassIds.empty()) {
    selectedClassIds.resize(numClasses_);
    std::iota(selectedClassIds.begin(), selectedClassIds.end(), 0);
  }

  // Vector of image scores for each classification.
  const float *multiclassOutput = interpreter_->typed_output_tensor<float>(0);

  // Check buffer dimensions if they match tensor dimensions.
  if (
    !multiclassBuffers_.empty()
    && ((multiclassBuffers_[0].pixels().rows() != inputHeight_) || (multiclassBuffers_[0].pixels().cols() != inputWidth_))) {
    multiclassBuffers_.clear();
  }

  // Check if there are enough buffers for each class.
  while (multiclassBuffers_.size() <= numClasses_) {
    multiclassBuffers_.push_back(OneChannelFloatPixelBuffer(inputHeight_, inputWidth_));
  }

  // Separate the image scores for each class into the image buffers.
  multiclassResults.clear();
  for (size_t i = 0; i < selectedClassIds.size(); ++i) {
    const int classId = selectedClassIds[i];
    const float *src = multiclassOutput + classId;
    auto classPix = multiclassBuffers_[classId].pixels();
    outTensorToClassImage(src, classPix);
    multiclassResults.push_back(classPix);
  }
}

ConstOneChannelFloatPixels TfLiteMultiClassifier::obtainSingleOutput(
  ConstRGBA8888PlanePixels inputImage, int classId) {
  ScopeTimer t("obtain-single-output");

  // TODO(yuyan): select semantics results by class IDs
  if (
    !singleclassBuffer_ || singleclassBuffer_->pixels().rows() != inputHeight_
    || singleclassBuffer_->pixels().cols() != inputWidth_) {
    singleclassBuffer_.reset(new OneChannelFloatPixelBuffer(inputHeight_, inputWidth_));
  }

  // Get inference results.
  const float *multiclassOutput = interpreter_->typed_output_tensor<float>(0);
  const float *src = multiclassOutput + classId;

  // Get image from tensor. Note that here we implicitly get the first channel. To get other
  // channels see how other callers of outTensorToClassImage() update the semanticOutput.
  outTensorToClassImage(src, singleclassBuffer_->pixels());

  return singleclassBuffer_->pixels();
}

void TfLiteMultiClassifier::outTensorToClassImage(const float *src, FloatPixels dest) {
  float *out = dest.pixels();
  for (int i = 0; i < dest.rows(); ++i) {
    for (int j = 0; j < dest.cols(); ++j) {
      *out = *src;
      ++out;
      src += numClasses_;
    }
  }
}

}  // namespace c8
