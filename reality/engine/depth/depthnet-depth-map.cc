// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Lynn Dang (lynn@8thwall.com)

#include "bzl/inliner/rules2.h"
cc_library {
  hdrs = {
    "depthnet-depth-map.h",
  };
  deps = {
    "//c8:vector",
    "//reality/engine/deepnets:tflite-interpreter",
    "//c8/io:file-io",
    "//c8/pixels:pixel-transforms",
    "//c8/pixels:pixel-buffer",
    "//c8/pixels:pixels",
    "//c8/stats:scope-timer",
  };
  visibility = {
    "//visibility:public",
  };
  data = {
    "//reality/engine/depth/data:depthnet",
  };
}
cc_end(0x96dfcf55);

#include <cmath>

#include "c8/pixels/pixel-buffer.h"
#include "c8/pixels/pixel-transforms.h"
#include "c8/pixels/pixels.h"
#include "reality/engine/depth/depthnet-depth-map.h"
#include "c8/stats/scope-timer.h"

namespace c8 {
constexpr size_t DEPTHNET_CACHE_SIZE = 7900000;

Depthnet::Depthnet(const uint8_t *modelData, int modelSize)
    : interpreter_(modelData, modelSize, DEPTHNET_CACHE_SIZE) {}

// Constructor reads in the TFLite model
Depthnet::Depthnet(const Vector<uint8_t> &modelData) : interpreter_(modelData, DEPTHNET_CACHE_SIZE) {}


DepthFloatPixels Depthnet::detectDepth(
  // The input is (1, 256, 192, 3) to match the dimensions of the datarecorder depth maps which is
  // 192x256.  It does not expect a letterbox input.
  // Copy the image pixels to the floating point tensor input, scaling 0:255 to 0:1.
  ConstRGBA8888PlanePixels image) {
  ScopeTimer t("depthnet-depth-map");
  {
    ScopeTimer t1("depthnet-depth-map-input");

    float *dst = interpreter_->typed_input_tensor<float>(0);
    const uint8_t *src = image.pixels();
    const uint8_t *srcStart = src;
    const uint8_t *srcEnd = srcStart + image.cols() * 4;

    for (int r = 0; r < image.rows(); ++r) {
      src = srcStart;
      while (src != srcEnd) {
        // Scale the images to 0:1.
        dst[0] = src[0] / 255.f;
        dst[1] = src[1] / 255.f;
        dst[2] = src[2] / 255.f;

        src += 4;
        dst += 3;
      }
      srcStart += image.rowBytes();
      srcEnd += image.rowBytes();
    }
  }

  {
    // Run the tflite model on the input.
    ScopeTimer t1("depthnet-depth-map-invoke");
    interpreter_->Invoke();
  }

  {
    ScopeTimer t1("depthnet-depth-map-output");
    int width = image.cols();
    int height = image.rows();

    // Output will be in the shape (1, 256, 192, 2).  The depth is stored in the first channel and
    // is copied over to float pixels to be converted into greyscale.
    float *depthOutput = interpreter_->typed_output_tensor<float>(0);

    // Reallocate buffer if necessary.
    if (depthDataBuffer_.pixels().rows() != height && depthDataBuffer_.pixels().cols() != width) {
      depthDataBuffer_ = DepthFloatPixelBuffer(height, width);
    }
    auto depthPixels = depthDataBuffer_.pixels();
    ConstFloatPixels depthOutputPixels = ConstFloatPixels(height, width, width, depthOutput);
    copyFloatPixels(depthOutputPixels, &depthPixels);

    return depthDataBuffer_.pixels();
  }
}
}  // namespace c8
