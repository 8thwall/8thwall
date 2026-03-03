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
    "landmarks-to-transform-matrix.h",
  };
  deps = {
    "@org_tensorflow//tensorflow/lite/delegates/gpu/common:types",
    "@org_tensorflow//tensorflow/lite/delegates/gpu/common:shape",
    "@org_tensorflow//tensorflow/lite/kernels:builtin_ops",
    "@org_tensorflow//tensorflow/lite/kernels:kernel_util",
    "@com_google_absl//absl/strings",
    "@com_google_absl//absl/types:any",
    "@com_google_absl//absl/status",
    "@org_tensorflow//tensorflow/lite/schema:schema_fbs",
    "@org_tensorflow//tensorflow/lite/schema:schema_fbs",
    "@org_tensorflow//tensorflow/lite/delegates/gpu/common/mediapipe:landmarks_to_transform_matrix",
  };
}
cc_end(0x732fdafe);

#include "absl/status/status.h"
#include "absl/types/any.h"
#include "tensorflow/lite/delegates/gpu/common/mediapipe/landmarks_to_transform_matrix.h"
#include "tensorflow/lite/delegates/gpu/common/shape.h"
#include "tensorflow/lite/delegates/gpu/common/types.h"
#include "tensorflow/lite/error_reporter.h"
#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/internal/common.h"
#include "tensorflow/lite/kernels/internal/compatibility.h"
#include "tensorflow/lite/kernels/internal/tensor.h"
#include "tensorflow/lite/kernels/kernel_util.h"
#include "tensorflow/lite/kernels/padding.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/model.h"
#include "tensorflow/lite/schema/schema_generated.h"

namespace c8 {
namespace {

constexpr int kDataInputTensor = 0;
constexpr int kOutputTensor = 0;
constexpr tflite::gpu::int3 kTensformMatrixShape(1, 4, 4);

tflite::gpu::float2 Read3DLandmarkXY(const float *data, int idx) {
  tflite::gpu::float2 result;
  result.x = data[idx * 3];
  result.y = data[idx * 3 + 1];
  return result;
}

tflite::gpu::float3 Read3DLandmarkXYZ(const float *data, int idx) {
  tflite::gpu::float3 result;
  result.x = data[idx * 3];
  result.y = data[idx * 3 + 1];
  result.z = data[idx * 3 + 2];
  return result;
}

struct Mat3 {
  Mat3() { data.resize(9); }
  Mat3(
    float x00,
    float x01,
    float x02,
    float x10,
    float x11,
    float x12,
    float x20,
    float x21,
    float x22)
      : data{x00, x01, x02, x10, x11, x12, x20, x21, x22} {}

  Mat3 operator*(const Mat3 &other) {
    Mat3 result;
    for (int r = 0; r < 3; r++) {
      for (int c = 0; c < 3; c++) {
        float sum = 0;
        for (int k = 0; k < 3; k++) {
          sum += this->Get(r, k) * other.Get(k, c);
        }
        result.Set(r, c, sum);
      }
    }
    return result;
  }
  tflite::gpu::float3 operator*(const tflite::gpu::float3 &vec) const {
    tflite::gpu::float3 result;
    for (int r = 0; r < 3; r++) {
      float sum = 0;
      for (int k = 0; k < 3; k++) {
        sum += this->Get(r, k) * vec[k];
      }
      result[r] = sum;
    }
    return result;
  }
  float Get(int x, int y) const { return data[x * 3 + y]; }
  void Set(int x, int y, float val) { data[x * 3 + y] = val; }

  std::vector<float> data;
};

struct Mat4 {
  Mat4() { data.resize(16); }
  Mat4(
    float x00,
    float x01,
    float x02,
    float x03,
    float x10,
    float x11,
    float x12,
    float x13,
    float x20,
    float x21,
    float x22,
    float x23,
    float x30,
    float x31,
    float x32,
    float x33)
      : data{x00, x01, x02, x03, x10, x11, x12, x13, x20, x21, x22, x23, x30, x31, x32, x33} {}
  void operator*=(const Mat4 &other) {
    Mat4 result;
    for (int r = 0; r < 4; r++) {
      for (int c = 0; c < 4; c++) {
        float sum = 0;
        for (int k = 0; k < 4; k++) {
          sum += this->Get(r, k) * other.Get(k, c);
        }
        result.Set(r, c, sum);
      }
    }
    std::memcpy(this->data.data(), result.data.data(), result.data.size() * sizeof(float));
  }
  float Get(int x, int y) const { return data[x * 4 + y]; }
  void Set(int x, int y, float val) { data[x * 4 + y] = val; }

  std::vector<float> data;
};

void EstimateRotationRadians(
  const float *input_data_0,
  int left_rotation_idx,
  int right_rotation_idx,
  float target_rotation_radians,
  float *rotation_radians) {
  const tflite::gpu::float3 left_landmark = Read3DLandmarkXYZ(input_data_0, left_rotation_idx);
  const tflite::gpu::float3 right_landmark = Read3DLandmarkXYZ(input_data_0, right_rotation_idx);
  const float left_x = left_landmark[0];
  const float left_y = left_landmark[1];
  const float right_x = right_landmark[0];
  const float right_y = right_landmark[1];
  float rotation = std::atan2(right_y - left_y, right_x - left_x);
  rotation = target_rotation_radians - rotation;
  *rotation_radians = rotation;
}

void EstimateCenterAndSize(
  const float *input_data_0,
  std::vector<tflite::gpu::int2> subset_idxs,
  float rotation_radians,
  float *crop_x,
  float *crop_y,
  float *crop_width,
  float *crop_height) {
  std::vector<tflite::gpu::float3> landmarks;
  landmarks.reserve(subset_idxs.size() * 2);
  for (int i = 0; i < subset_idxs.size(); i++) {
    landmarks.push_back(Read3DLandmarkXYZ(input_data_0, subset_idxs[i][0]));
    landmarks.push_back(Read3DLandmarkXYZ(input_data_0, subset_idxs[i][1]));
  }
  for (int i = 0; i < landmarks.size(); i++) {
    landmarks[i].z = 1.0;
  }
  const float &r = rotation_radians;
  // clang-format off
  const Mat3 t_rotation = Mat3(std::cos(r),  -std::sin(r), 0.0,
                               std::sin(r),   std::cos(r), 0.0,
                                       0.0,           0.0, 1.0);
  const Mat3 t_rotation_inverse =
                          Mat3(std::cos(-r), -std::sin(-r), 0.0,
                               std::sin(-r),  std::cos(-r), 0.0,
                                        0.0,           0.0, 1.0);
  // clang-format on
  for (int i = 0; i < landmarks.size(); i++) {
    landmarks[i] = t_rotation * landmarks[i];
  }
  tflite::gpu::float3 xy1_max = landmarks[0], xy1_min = landmarks[0];
  for (int i = 1; i < landmarks.size(); i++) {
    if (xy1_max.x < landmarks[i].x)
      xy1_max.x = landmarks[i].x;
    if (xy1_max.y < landmarks[i].y)
      xy1_max.y = landmarks[i].y;

    if (xy1_min.x > landmarks[i].x)
      xy1_min.x = landmarks[i].x;
    if (xy1_min.y > landmarks[i].y)
      xy1_min.y = landmarks[i].y;
  }
  *crop_width = xy1_max.x - xy1_min.x;
  *crop_height = xy1_max.y - xy1_min.y;
  tflite::gpu::float3 crop_xy1 = xy1_min;
  crop_xy1.x += xy1_max.x;
  crop_xy1.y += xy1_max.y;
  crop_xy1.x /= 2;
  crop_xy1.y /= 2;
  crop_xy1 = t_rotation_inverse * crop_xy1;
  *crop_x = crop_xy1.x;
  *crop_y = crop_xy1.y;
}

inline void LandmarksToTransformMatrixV2(
  const tflite::gpu::LandmarksToTransformMatrixV2Attributes &params,
  const tflite::RuntimeShape &input0_shape,
  const float *landmarks,
  const tflite::RuntimeShape &output_shape,
  float *output_data) {
  float rotation_radians = 0.0;
  EstimateRotationRadians(
    landmarks,
    params.left_rotation_idx,
    params.right_rotation_idx,
    params.target_rotation_radians,
    &rotation_radians);
  float crop_x = 0.0, crop_y = 0.0, crop_width = 0.0, crop_height = 0.0;
  EstimateCenterAndSize(
    landmarks, params.subset_idxs, rotation_radians, &crop_x, &crop_y, &crop_width, &crop_height);
  // Turn off clang formatting to make matrices initialization more readable.
  // clang-format off
  Mat4 t = Mat4(1.0, 0.0, 0.0, 0.0,
                0.0, 1.0, 0.0, 0.0,
                0.0, 0.0, 1.0, 0.0,
                0.0, 0.0, 0.0, 1.0);
  const Mat4 t_shift = Mat4(1.0, 0.0, 0.0, crop_x,
                            0.0, 1.0, 0.0, crop_y,
                            0.0, 0.0, 1.0,    0.0,
                            0.0, 0.0, 0.0,    1.0);
  t *= t_shift;
  const float& r = -rotation_radians;
  const Mat4 t_rotation = Mat4(std::cos(r), -std::sin(r), 0.0, 0.0,
                               std::sin(r),  std::cos(r), 0.0, 0.0,
                                       0.0,          0.0, 1.0, 0.0,
                                       0.0,          0.0, 0.0, 1.0);
  t *= t_rotation;
  const float scale_x = params.scale_x * crop_width / params.output_width;
  const float scale_y = params.scale_y * crop_height / params.output_height;
  const Mat4 t_scale = Mat4(scale_x,     0.0, 0.0, 0.0,
                                0.0, scale_y, 0.0, 0.0,
                                0.0,     0.0, 1.0, 0.0,
                                0.0,     0.0, 0.0, 1.0);
  t *= t_scale;
  const float shift_x = -1.0 * (params.output_width / 2.0);
  const float shift_y = -1.0 * (params.output_height / 2.0);
  const Mat4 t_shift2 = Mat4(1.0, 0.0, 0.0, shift_x,
                             0.0, 1.0, 0.0, shift_y,
                             0.0, 0.0, 1.0,     0.0,
                             0.0, 0.0, 0.0,     1.0);
  t *= t_shift2;
  std::memcpy(output_data, t.data.data(), 16 * sizeof(float));
  // clang-format on
}

TfLiteStatus Prepare(TfLiteContext *context, TfLiteNode *node) {
  TF_LITE_ENSURE_EQ(context, tflite::NumInputs(node), 1);
  TF_LITE_ENSURE_EQ(context, tflite::NumOutputs(node), 1);
  const TfLiteTensor *input = tflite::GetInput(context, node, kDataInputTensor);
  TF_LITE_ENSURE(context, input != nullptr);
  TfLiteTensor *output = tflite::GetOutput(context, node, kOutputTensor);
  TF_LITE_ENSURE(context, output != nullptr);

  TF_LITE_ENSURE_EQ(context, tflite::NumDimensions(input), 3);
  TF_LITE_ENSURE_EQ(context, input->type, kTfLiteFloat32);
  TF_LITE_ENSURE_EQ(context, output->type, kTfLiteFloat32);

  TfLiteIntArray *output_size = TfLiteIntArrayCreate(3);
  output_size->data[0] = kTensformMatrixShape.x;
  output_size->data[1] = kTensformMatrixShape.y;
  output_size->data[2] = kTensformMatrixShape.z;

  return context->ResizeTensor(context, output, output_size);
}

TfLiteStatus Eval(TfLiteContext *context, TfLiteNode *node) {
  tflite::gpu::LandmarksToTransformMatrixV2Attributes op_params;
  tflite::gpu::BHWC output_shape;
  auto status = tflite::gpu::ParseLandmarksToTransformMatrixV2Attributes(
    node->custom_initial_data, node->custom_initial_data_size, &op_params, &output_shape);
  if (!status.ok()) {
    context->ReportError(context, status.message().data());
    return kTfLiteError;
  }

  if (op_params.left_rotation_idx < 0) {
    context->ReportError(context, "Incorrect left_rotation_idx: %d", op_params.left_rotation_idx);
    return kTfLiteError;
  }

  if (op_params.right_rotation_idx < 0) {
    context->ReportError(context, "Incorrect right_rotation_idx: %d", op_params.right_rotation_idx);
    return kTfLiteError;
  }

  if (op_params.output_height <= 0) {
    context->ReportError(context, "Incorrect output_height: %d", op_params.output_height);
    return kTfLiteError;
  }

  if (op_params.output_width <= 0) {
    context->ReportError(context, "Incorrect output_width: %d", op_params.output_width);
    return kTfLiteError;
  }

  if (op_params.scale_x <= 0) {
    context->ReportError(context, "Incorrect scale_x: %d", op_params.scale_x);
    return kTfLiteError;
  }

  if (op_params.scale_y <= 0) {
    context->ReportError(context, "Incorrect scale_y: %d", op_params.scale_y);
    return kTfLiteError;
  }

  int counter = 0;
  for (auto &val : op_params.subset_idxs) {
    for (int i = 0; i < 2; i++) {
      if (val[i] < 0) {
        context->ReportError(
          context, "Incorrect subset value: index = %d, value = %d", counter, val[i]);
        return kTfLiteError;
      }
      counter++;
    }
  }

  const TfLiteTensor *input0 = tflite::GetInput(context, node, kDataInputTensor);
  TF_LITE_ENSURE(context, input0 != nullptr);
  TfLiteTensor *output = tflite::GetOutput(context, node, kOutputTensor);
  TF_LITE_ENSURE(context, output != nullptr);

  LandmarksToTransformMatrixV2(
    op_params,
    tflite::GetTensorShape(input0),
    tflite::GetTensorData<float>(input0),
    tflite::GetTensorShape(output),
    tflite::GetTensorData<float>(output));
  return kTfLiteOk;
}
}  // namespace

TfLiteRegistration *RegisterLandmarksToTransformMatrixV2() {
  static TfLiteRegistration reg = {
    /*.init=*/nullptr,
    /*.free=*/nullptr,
    /*.prepare=*/Prepare,
    /*.invoke=*/Eval,
    /*.profiling_string=*/nullptr,
    /*.builtin_code=*/tflite::BuiltinOperator_CUSTOM,
    /*.custom_name=*/"Landmarks2TransformMatrix",
    /*.version=*/2,
  };
  return &reg;
}

}  // namespace c8
