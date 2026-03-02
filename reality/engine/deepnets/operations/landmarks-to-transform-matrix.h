// Copyright (c) 2023 Niantic Labs, Inc.
// Original Author: Nathan Waters (nathan@nianticlabs.com)
//
// Custom TFLite operation for face_landmark_with_attention.tflite.

#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"

namespace c8 {

TfLiteRegistration *RegisterLandmarksToTransformMatrixV2();

}  // namespace c8
