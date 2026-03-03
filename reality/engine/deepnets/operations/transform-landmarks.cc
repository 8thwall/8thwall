// Copyright (c) 2023 Niantic Labs, Inc.
// Original Author: Nathan Waters (nathan@nianticlabs.com)

// Original License: Apache 2.0
// github.com/8thwall/mediapipe/blob/d68f5e416903e3d756ebe07d08c4a3b911741a91/LICENSE; "
// Modifications by Niantic Labs, Inc.
//
// Copyright 2021 The MediaPipe Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "transform-landmarks.h",
  };
  deps = {
    "@com_google_absl//absl/status",
    "@org_tensorflow//tensorflow/lite/delegates/gpu/common:types",
    "@org_tensorflow//tensorflow/lite/delegates/gpu/common:shape",
    "@org_tensorflow//tensorflow/lite/kernels:builtin_ops",
    "@org_tensorflow//tensorflow/lite/kernels:kernel_util",
    "@org_tensorflow//tensorflow/lite/schema:schema_fbs",
    "@org_tensorflow//tensorflow/lite/delegates/gpu/common/mediapipe:transform_landmarks",
  };
}
cc_end(0x0ad79249);

#include "tensorflow/lite/delegates/gpu/common/mediapipe/transform_landmarks.h"
#include "tensorflow/lite/delegates/gpu/common/types.h"
#include "tensorflow/lite/kernels/internal/common.h"
#include "tensorflow/lite/kernels/internal/compatibility.h"
#include "tensorflow/lite/kernels/internal/tensor.h"
#include "tensorflow/lite/kernels/padding.h"
#include "tensorflow/lite/schema/schema_generated.h"

namespace c8 {

namespace {

constexpr int kDataInput0Tensor = 0;
constexpr int kDataInput1Tensor = 1;
constexpr int kOutputTensor = 0;

float DotProduct(const tflite::gpu::float4 &l, const tflite::gpu::float4 &r) {
  return l.x * r.x + l.y * r.y + l.z * r.z + l.w * r.w;
}

inline void TransformLandmarksV2(
  const tflite::gpu::TransformLandmarksAttributes &params,
  const tflite::RuntimeShape &input0_shape,
  const float *landmarks,
  const float *transform_matrix,  // transformation matrix
  const tflite::RuntimeShape &output_shape,
  float *output_data) {
  TFLITE_CHECK_EQ(input0_shape.DimensionsCount(), 3);
  TFLITE_CHECK_EQ(output_shape.DimensionsCount(), 3);
  const int output_width = output_shape.Dims(1);
  TFLITE_CHECK_EQ(input0_shape.Dims(2) % params.dimensions, 0);

  tflite::RuntimeShape input_shape_with_batch{
    /*batch=*/1, input0_shape.Dims(0), input0_shape.Dims(1), input0_shape.Dims(2)};
  tflite::RuntimeShape output_shape_with_batch{
    /*batch=*/1, output_shape.Dims(0), output_shape.Dims(1), output_shape.Dims(2)};

  // Read first two rows of transformation matrix
  tflite::gpu::float4 x_transform(
    transform_matrix[0], transform_matrix[1], transform_matrix[2], transform_matrix[3]);
  tflite::gpu::float4 y_transform(
    transform_matrix[4], transform_matrix[5], transform_matrix[6], transform_matrix[7]);

  for (int landmark = 0; landmark < output_width; ++landmark) {
    const int offset = Offset(input_shape_with_batch, 0, 0, landmark, 0);

    if (params.dimensions == 2) {
      tflite::gpu::float4 lv(
        landmarks[offset], landmarks[offset + 1], static_cast<float>(0.0), static_cast<float>(1.0));
      tflite::gpu::float2 transformed(DotProduct(x_transform, lv), DotProduct(y_transform, lv));
      output_data[offset] = transformed.x;
      output_data[offset + 1] = transformed.y;
    }
    if (params.dimensions == 3) {
      tflite::gpu::float4 lv(
        landmarks[offset], landmarks[offset + 1], static_cast<float>(0.0), static_cast<float>(1.0));
      tflite::gpu::float3 transformed(
        DotProduct(x_transform, lv), DotProduct(y_transform, lv), lv.z);
      output_data[offset] = transformed.x;
      output_data[offset + 1] = transformed.y;
      output_data[offset + 2] = landmarks[offset + 2];
    }
  }
}

TfLiteStatus Prepare(TfLiteContext *context, TfLiteNode *node) {
  TF_LITE_ENSURE_EQ(context, tflite::NumInputs(node), 2);
  TF_LITE_ENSURE_EQ(context, tflite::NumOutputs(node), 1);
  const TfLiteTensor *input = tflite::GetInput(context, node, kDataInput0Tensor);
  TF_LITE_ENSURE(context, input != nullptr);
  TfLiteTensor *output = tflite::GetOutput(context, node, kOutputTensor);
  TF_LITE_ENSURE(context, output != nullptr);

  TF_LITE_ENSURE_EQ(context, tflite::NumDimensions(input), 3);
  TF_LITE_ENSURE_EQ(context, input->type, kTfLiteFloat32);
  TF_LITE_ENSURE_EQ(context, output->type, kTfLiteFloat32);

  TfLiteIntArray *output_size = TfLiteIntArrayCreate(3);
  output_size->data[0] = input->dims->data[0];
  output_size->data[1] = input->dims->data[1];
  output_size->data[2] = input->dims->data[2];

  return context->ResizeTensor(context, output, output_size);
}

TfLiteStatus Eval(TfLiteContext *context, TfLiteNode *node) {
  tflite::gpu::TransformLandmarksAttributes op_params;

  TfLiteTensor *output = tflite::GetOutput(context, node, kOutputTensor);
  TF_LITE_ENSURE(context, output != nullptr);
  tflite::RuntimeShape runtime_output_shape = tflite::GetTensorShape(output);
  tflite::gpu::BHWC output_shape(
    1, runtime_output_shape.Dims(0), runtime_output_shape.Dims(1), runtime_output_shape.Dims(2));
  auto status = tflite::gpu::ParseTransformLandmarksV2Attributes(
    node->custom_initial_data, node->custom_initial_data_size, &op_params, &output_shape);
  if (!status.ok()) {
    context->ReportError(context, status.message().data());
    return kTfLiteError;
  }
  if (op_params.dimensions != 3 && op_params.dimensions != 2) {
    context->ReportError(context, "Incorrect dimensions size: %d", op_params.dimensions);
    return kTfLiteError;
  }

  const TfLiteTensor *input0 = tflite::GetInput(context, node, kDataInput0Tensor);
  TF_LITE_ENSURE(context, input0 != nullptr);
  const TfLiteTensor *input1 = tflite::GetInput(context, node, kDataInput1Tensor);
  TF_LITE_ENSURE(context, input1 != nullptr);

  TransformLandmarksV2(
    op_params,
    tflite::GetTensorShape(input0),
    tflite::GetTensorData<float>(input0),
    tflite::GetTensorData<float>(input1),
    tflite::GetTensorShape(output),
    tflite::GetTensorData<float>(output));
  return kTfLiteOk;
}

}  // namespace

TfLiteRegistration *RegisterTransformLandmarksV2() {
  static TfLiteRegistration reg = {
    /*.init=*/nullptr,
    /*.free=*/nullptr,
    /*.prepare=*/Prepare,
    /*.invoke=*/Eval,
    /*.profiling_string=*/nullptr,
    /*.builtin_code=*/tflite::BuiltinOperator_CUSTOM,
    /*.custom_name=*/"TransformLandmarks",
    /*.version=*/2,
  };
  return &reg;
}

}  // namespace c8
