// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Lynn Dang (lynn@8thwall.com)
//
// API Wrapper for working with Pydnet DepthNet model.

#pragma once

#include "c8/io/file-io.h"
#include "c8/pixels/pixels.h"
#include "c8/vector.h"
#include "reality/engine/deepnets/tflite-interpreter.h"

namespace c8 {

// Depthnet is based off of Pydnet which provides an abstraction layer above the deep net model for
// determining a relative depth given a single image. It is a MIT licensed deep learning model that
// is considered state-of-the-art in terms of realtime depthnets. Pydnet is about 8mb and runs much
// faster that Midas, but it's also is in relative scale, meaning the authors scaled the output
// depending on the dataset's min and max depth.
// https://github.com/mattpoggi/pydnet
class Depthnet {
public:
  // Construct from a tflite model
  Depthnet(const uint8_t *modelData, int modelSize);
  Depthnet(const Vector<uint8_t> &modelData);

  // Default move constructors.
  Depthnet(Depthnet &&) = default;
  Depthnet &operator=(Depthnet &&) = default;

  // Disallow copying.
  Depthnet(const Depthnet &) = delete;
  Depthnet &operator=(const Depthnet &) = delete;

  // Given an image that is 256x192, it will return a depth map in relative scale and in the given
  // image's dimensions.
  DepthFloatPixels detectDepth(ConstRGBA8888PlanePixels image);

private:
  TFLiteInterpreter interpreter_;
  DepthFloatPixelBuffer depthDataBuffer_;
};

}  // namespace c8
