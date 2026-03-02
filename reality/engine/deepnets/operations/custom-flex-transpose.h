// Copyright (c) 2023 Niantic Labs, Inc.
// Original Author: Yuyan Song (yuyansong@nianticlabs.com)
//
// Custom TFLite operation for FlexTranspose.

#include "c8/vector.h"
#include "tensorflow/lite/kernels/internal/common.h"

namespace c8 {

void customFlexTransposeF32(
  const tflite::RuntimeShape &inputShape,
  const float *inputData,
  const Vector<int32_t> &perm,
  const tflite::RuntimeShape &outputShape,
  float *outputData);

}  // namespace c8
