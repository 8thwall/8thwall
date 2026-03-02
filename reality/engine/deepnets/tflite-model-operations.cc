// Copyright (c) 2022 Niantic Labs, Inc.
// Original Author: Lynn Dang & Yuyan Song (yuyansong@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "tflite-model-operations.h",
  };
  deps = {
    "//c8:color",
    "//c8/pixels:pixels",
    "//c8/pixels:pixel-buffer",
    "//c8:string",
    "//c8:vector",
    "//reality/engine/deepnets:tflite-debug",
    "//third_party/half:half",
    "@org_tensorflow//tensorflow/lite/schema:schema_fbs",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x4c5dd80f);

#include "c8/color.h"
#include "c8/pixels/pixel-buffer.h"
#include "c8/pixels/pixels.h"
#include "c8/vector.h"
#include "reality/engine/deepnets/tflite-debug.h"
#include "reality/engine/deepnets/tflite-model-operations.h"

namespace c8 {

void rotateSquareTensorFP16(half_float::half *weights, int numIn, int numOut, int dim, bool rotCW) {
  Vector<half_float::half> tempWeights(numOut);
  half_float::half *curWeight = weights;

  for (int inIdx = 0; inIdx < numIn; ++inIdx) {
    // Perform 2D rotation for each convolution.
    for (int i = 0; i < (dim + 1) / 2; ++i) {
      for (int j = 0; j < (dim / 2); ++j) {
        // Calculating rows and columns to move.
        int lowestLeftR = dim - 1 - j;
        int lowestRightR = dim - 1 - i;
        int topRightR = j;
        int topleftR = i;
        int lowestLeftC = i;
        int lowestRightC = dim - j - 1;
        int topRightC = dim - 1 - i;
        int topleftC = j;

        // Set temp to lower left.
        std::memcpy(
          tempWeights.data(),
          (curWeight + (lowestLeftR * dim * numOut) + lowestLeftC * numOut),
          numOut * 2);

        if (rotCW) {  // ROTATE CLOCKWISE (portrait to landscape)
          // Set lower left to lower right.
          std::memcpy(
            curWeight + (lowestLeftR * dim * numOut) + lowestLeftC * numOut,
            curWeight + (lowestRightR * dim * numOut) + lowestRightC * numOut,
            numOut * 2);

          // Set lower right to top right.
          std::memcpy(
            curWeight + (lowestRightR * dim * numOut) + lowestRightC * numOut,
            curWeight + (topRightR * dim * numOut) + topRightC * numOut,
            numOut * 2);

          // Set top right to top left.
          std::memcpy(
            curWeight + (topRightR * dim * numOut) + topRightC * numOut,
            curWeight + (topleftR * dim * numOut) + topleftC * numOut,
            numOut * 2);

          // Set top left to temp (original lower left).
          std::memcpy(
            curWeight + (topleftR * dim * numOut) + topleftC * numOut,
            tempWeights.data(),
            numOut * 2);
        } else {  // ROTATE COUNTER-CLOCKWISE (landscape to portrait)
          // Set lower left to top left.
          std::memcpy(
            curWeight + (lowestLeftR * dim * numOut) + lowestLeftC * numOut,
            curWeight + (topleftR * dim * numOut) + topleftC * numOut,
            numOut * 2);

          // Set top left to top right.
          std::memcpy(
            curWeight + (topleftR * dim * numOut) + topleftC * numOut,
            curWeight + (topRightR * dim * numOut) + topRightC * numOut,
            numOut * 2);

          // Set top right to bottom right.
          std::memcpy(
            curWeight + (topRightR * dim * numOut) + topRightC * numOut,
            curWeight + (lowestRightR * dim * numOut) + lowestRightC * numOut,
            numOut * 2);

          // Set bottom right to temp (original lower left).
          std::memcpy(
            curWeight + (lowestRightR * dim * numOut) + lowestRightC * numOut,
            tempWeights.data(),
            numOut * 2);
        }
      }
    }
    curWeight += (dim * dim * numOut);
  }
}

Vector<uint8_t> rotateModel(uint8_t *modelData, bool rotCW) {
  std::unique_ptr<tflite::ModelT> model(tflite::GetModel(modelData)->UnPack());

  for (auto &g : model->subgraphs) {
    if (g == nullptr) {
      continue;
    }

    auto numTensors = g->tensors.size();

    // Loop through the tensors
    for (int t = 0; t < numTensors; ++t) {
      auto shape = g->tensors[t]->shape;
      auto bufferIndex = g->tensors[t]->buffer;

      // Only selects tensors with shape 4. Convolutions are square, so checks the width and height.
      // If the convolution is 1x1, ignore. Also checks if the tensor has weights to rotate. If
      // there is no buffer, the bufferIndex will be set to 0 leading to an empty buffer size 0.
      if (
        (shape.size() != 4) || (shape[1] != shape[2]) || (shape[1] == 1)
        || (model->buffers[bufferIndex]->data.size() == 0)) {
        continue;
      }

      // Get the weights from the buffer and read as float16.
      auto *intWeights = &(model->buffers[bufferIndex]->data[0]);
      half_float::half *weights = reinterpret_cast<half_float::half *>(intWeights);

      int numInput = shape[0];
      int dim = shape[2];
      int numOutput = shape[3];

      rotateSquareTensorFP16(weights, numInput, numOutput, dim, rotCW);
    }
  }

  flatbuffers::FlatBufferBuilder fbb;
  FinishModelBuffer(fbb, tflite::ModelT::TableType::Pack(fbb, model.get()));
  Vector<uint8_t> data(fbb.GetSize());
  std::copy(fbb.GetBufferPointer(), fbb.GetBufferPointer() + fbb.GetSize(), data.begin());
  return data;
}

}  // namespace c8
