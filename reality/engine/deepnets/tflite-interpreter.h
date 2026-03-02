// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)
//
// Holds state and does boiler plate for invoking tflite models.

#pragma once

#include "c8/vector.h"
#include "tensorflow/lite/delegates/xnnpack/xnnpack_delegate.h"
#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/model.h"

namespace c8 {
using TfLiteDelegatePtr = std::unique_ptr<TfLiteDelegate, std::function<void(TfLiteDelegate *)>>;
// Default number of threads to use when we are running in pthread mode.
// This number is set conservatively to 2 because we might have multiple intepreters (e.g. face has
// global, local and ears) running at the same time. If we set to 4+, we exceeds the max threads (8)
// pre-allocated by wasm.
//
// It is best that for your usage, determines the number of threads explicitly for each instance
// of TFLiteInterpreter. e.g. 1 for global face detector, 2 for local face detector, 2 for ears.
//
// Previous testing shows that you can run 2 for ears, 3 for global and 4 for local and the face
// tracking still works. It's likely best to only add threads to your slowest model.
// TODO(dat): Allow threadpool to be shared across multiple interpreters.
constexpr int DEFAULT_NUM_THREADS = 2;

// A wrapper for initializing and working with a tflite::Interpreter object. The object is
// constructed and initialized with a model; access to the underlying model pointer is provided
// through operator ->.
//
// NOTE: Before knowing the typical access patterns across several models, it's hard to know if
// there is a better or more abstract interface. For now we are passing through the
// tflite::Interpreter interface directly, in spite of the general best practice of isolating
// third party APIs.  After we have some more experience here, it would probably be good to
// redesign this class with a more targeted interface.
//
// Typical usage:
//
//   TFLiteInterpreter interpreter(readFile("/path/to/model.tflite"));
//
//   while (true) {
//     auto* input_buf = interpreter->typed_input_tensor<float>(0);
//     // copy data to input.
//     ...
//     interpreter->Invoke();
//     auto* output_buf = intpereter->typed_output_tensor<float>(0);
//     // process output.
//   }
class TFLiteInterpreter {
public:
  /** Initialize the interpreter with a XNNPack cache.

  It's important to set the right size for cacheSizeInBytes. Too low and initialization requires
  multiple memory resize. Too high and the memory is wasted. On web, the memory is not partially
  freed on construction finishing. On native, memory is partially freed but this take extra time.

  Find the right amount of cache size by running in debug mode in your target platform, then
  set this value based on XNNPACK debug output.

  @param numThreads only take effect when not on wasm or on wasm pthread
  */
  TFLiteInterpreter(
    const uint8_t *modelData,
    int modelSize,
    size_t cacheSizeInBytes,
    int numThreads = DEFAULT_NUM_THREADS);
  TFLiteInterpreter(
    const Vector<uint8_t> &modelData, size_t cacheSizeInBytes, int numThreads = DEFAULT_NUM_THREADS)
      : TFLiteInterpreter(modelData.data(), modelData.size(), cacheSizeInBytes, numThreads) {}

  tflite::Interpreter *operator->() { return interpreter_.get(); };

  // Default move constructors.
  TFLiteInterpreter(TFLiteInterpreter &&) = default;
  TFLiteInterpreter &operator=(TFLiteInterpreter &&) = default;

  // Disallow copying.
  TFLiteInterpreter(const TFLiteInterpreter &) = delete;
  TFLiteInterpreter &operator=(const TFLiteInterpreter &) = delete;

  ~TFLiteInterpreter();

  // Input 'index' is the tensor index from the input/output tensors.
  // 'index' should be in the range of [0, number_of_inputs - 1] or [0, number_of_outputs - 1].
  std::vector<int> getInputTensorDims(size_t index = 0);
  std::vector<int> getOutputTensorDims(size_t index = 0);

private:
  std::unique_ptr<uint8_t> data_;
  std::unique_ptr<tflite::FlatBufferModel> model_;
  std::unique_ptr<tflite::Interpreter> interpreter_;
  tflite::ops::builtin::BuiltinOpResolver op_resolver_;
  TfLiteDelegatePtr delegate_;
  TfLiteXNNPackDelegateOptions xnnpack_opts_;
  TfLiteXNNPackDelegateWeightsCache *xnnpackCache_;

  std::vector<int> getTensorDims(const TfLiteTensor *tensor);
};

}  // namespace c8
