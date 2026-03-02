// Copyright (c) 2023 Niantic Labs, Inc.
// Original Author: Yuyan Song (yuyansong@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "custom-flex-transpose.h",
  };
  deps = {
    "//c8:vector",
    "@com_google_absl//absl/status",
    "@org_tensorflow//tensorflow/lite/kernels:builtin_ops",
    "@org_tensorflow//tensorflow/lite/kernels:kernel_util",
  };
}
cc_end(0x4d44ae52);

#include "c8/vector.h"
#include "tensorflow/lite/kernels/internal/common.h"
#include "tensorflow/lite/kernels/internal/compatibility.h"
#include "tensorflow/lite/kernels/internal/tensor.h"

namespace c8 {

namespace {

// compute steps for each dimension
void computeStepVector(const tflite::RuntimeShape &tensorShape, Vector<int32_t> *tensorSteps) {
  int32_t dimCount = tensorShape.DimensionsCount();
  tensorSteps->resize(dimCount);
  for (int32_t i = 0; i < dimCount; ++i) {
    (*tensorSteps)[i] = tensorShape.Dims(i);
  }
  if (dimCount > 1) {
    for (int32_t i = dimCount - 2; i >= 0; i--) {
      (*tensorSteps)[i] *= (*tensorSteps)[i + 1];
    }
  }
}

// convert from raw offset to tensor index vector
inline void offsetToIndexVector(
  int32_t offset, int32_t dim, const int32_t *indexSteps, int32_t *indexVec) {
  int32_t res = offset;
  for (int32_t i = 0; i < dim - 1; ++i) {
    indexVec[i] = res / indexSteps[i + 1];
    res -= indexVec[i] * indexSteps[i + 1];
  }
  indexVec[dim - 1] = res;
}

// convert from tensor index vector to raw offset
inline int32_t indexVectorToOffset(
  int32_t dim, const int32_t *indexVec, const int32_t *indexSteps) {
  int32_t index = 0;
  for (int32_t i = 0; i < dim - 1; ++i) {
    index += indexVec[i] * indexSteps[i + 1];
  }
  index += indexVec[dim - 1];
  return index;
}

// permutate index vector
inline void permuteIndexVector(
  int32_t dim, const int32_t *originVec, const int32_t *perm, int32_t *permVec) {
  for (auto i = 0; i < dim; ++i) {
    permVec[i] = originVec[perm[i]];
  }
}

}  // namespace

// Transpose for Float32
void customFlexTransposeF32(
  const tflite::RuntimeShape &inputShape,
  const float *inputData,
  const Vector<int32_t> &perm,
  const tflite::RuntimeShape &outputShape,
  float *outputData) {
  if (!inputData || !outputData) {
    return;
  }

  // compute tensor steps
  int32_t dimCount = inputShape.DimensionsCount();
  Vector<int32_t> inputSteps;
  Vector<int32_t> outputSteps;
  computeStepVector(inputShape, &inputSteps);
  computeStepVector(outputShape, &outputSteps);

  auto totalCount = inputShape.Dims(0);
  for (int32_t i = 1; i < dimCount; ++i) {
    totalCount *= inputShape.Dims(i);
  }

  Vector<int32_t> inputIndex(dimCount);
  Vector<int32_t> outputIndex(dimCount);
  for (int32_t i = 0; i < totalCount; ++i) {
    offsetToIndexVector(i, dimCount, inputSteps.data(), inputIndex.data());

    permuteIndexVector(dimCount, inputIndex.data(), perm.data(), outputIndex.data());

    int32_t outOffset = indexVectorToOffset(dimCount, outputIndex.data(), outputSteps.data());

    outputData[outOffset] = inputData[i];
  }
}

}  // namespace c8
