// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "tflite-interpreter.h",
  };
  deps = {
    "//c8:vector",
    "//reality/engine/deepnets/operations:landmarks-to-transform-matrix",
    "//reality/engine/deepnets/operations:transform-tensor-bilinear",
    "//reality/engine/deepnets/operations:transform-landmarks",
    "@org_tensorflow//tensorflow/lite:framework",
    "@org_tensorflow//tensorflow/lite/delegates/xnnpack:xnnpack_delegate",
    "@org_tensorflow//tensorflow/lite/kernels:builtin_ops",
  };
}
cc_end(0xb048911b);

#include <iostream>

#include "reality/engine/deepnets/operations/landmarks-to-transform-matrix.h"
#include "reality/engine/deepnets/operations/transform-landmarks.h"
#include "reality/engine/deepnets/operations/transform-tensor-bilinear.h"
#include "reality/engine/deepnets/tflite-interpreter.h"
#include "tensorflow/lite/error_reporter.h"
#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/model.h"
namespace c8 {

TFLiteInterpreter::TFLiteInterpreter(
  const uint8_t *modelData, int modelSize, size_t cacheSize, int numThreads) {
  op_resolver_.AddCustom(
    "Landmarks2TransformMatrix",
    RegisterLandmarksToTransformMatrixV2(),
    /*version=*/2);
  op_resolver_.AddCustom(
    "TransformTensorBilinear",
    RegisterTransformTensorBilinearV2(),
    /*version=*/2);
  op_resolver_.AddCustom(
    "TransformLandmarks",
    RegisterTransformLandmarksV2(),
    /*version=*/2);
  data_.reset(new uint8_t[modelSize]);
  std::memcpy(data_.get(), modelData, modelSize);

  model_ = tflite::FlatBufferModel::BuildFromBuffer(
    reinterpret_cast<const char *>(data_.get()), modelSize);

  auto interpreterBuilder = tflite::InterpreterBuilder(*model_, op_resolver_);
  // Set the builder before building. Then we can use interpreter_
  // NOTE(dat): Do not set numthreads using the builder. It will double since we are asking in
  // XNNPACK opts
  interpreterBuilder(&interpreter_);

  // Provide cache with a fixed sized since we don't have more than 1 interpreter using this cache
  xnnpackCache_ = TfLiteXNNPackDelegateWeightsCacheCreateWithSize(cacheSize);

#if !defined(JAVASCRIPT) || defined(__EMSCRIPTEN_PTHREADS__)
  xnnpack_opts_.num_threads = numThreads;
#endif
  xnnpack_opts_.flags = TFLITE_XNNPACK_DELEGATE_FLAG_QS8 | TFLITE_XNNPACK_DELEGATE_FLAG_QU8;
  xnnpack_opts_.weights_cache = xnnpackCache_;
  delegate_ =
    TfLiteDelegatePtr(TfLiteXNNPackDelegateCreate(&xnnpack_opts_), &TfLiteXNNPackDelegateDelete);
  interpreter_->ModifyGraphWithDelegate(delegate_.get());

  // weight cache finalization
  TfLiteXNNPackDelegateWeightsCacheFinalizeHard(xnnpackCache_);

  interpreter_->AllocateTensors();
}

TFLiteInterpreter::~TFLiteInterpreter() { TfLiteXNNPackDelegateWeightsCacheDelete(xnnpackCache_); }

std::vector<int> TFLiteInterpreter::getTensorDims(const TfLiteTensor *tensor) {
  if (tensor == nullptr) {
    return {};
  }

  const TfLiteIntArray *dims = tensor->dims;
  if (dims == nullptr) {
    return {};
  }

  std::vector<int> tensorDims;
  for (int i = 0; i < dims->size; ++i) {
    tensorDims.push_back(dims->data[i]);
  }
  return tensorDims;
}

std::vector<int> TFLiteInterpreter::getInputTensorDims(size_t index) {
  if (interpreter_ == nullptr) {
    return {};
  }

  // Get the number of input tensors
  const std::vector<int> &inputs = interpreter_->inputs();
  if (index >= inputs.size()) {
    return {};
  }

  const TfLiteTensor *inputTensor = interpreter_->input_tensor(index);
  return getTensorDims(inputTensor);
}

std::vector<int> TFLiteInterpreter::getOutputTensorDims(size_t index) {
 if (interpreter_ == nullptr) {
    return {};
  }

  // Get the number of input tensors
  const std::vector<int> &outputs = interpreter_->outputs();
  if (index >= outputs.size()) {
    return {};
  }

  const TfLiteTensor *outputTensor = interpreter_->output_tensor(index);
  return getTensorDims(outputTensor);
}

}  // namespace c8
