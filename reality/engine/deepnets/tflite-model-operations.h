// Copyright (c) 2022 Niantic Labs, Inc.
// Original Author: Lynn Dang & Yuyan Song (yuyansong@nianticlabs.com)
//
// Operations on tflite models.

#pragma once

#include "third_party/half/half.hpp"

namespace c8 {

// Rotate the tensor of [numIn, dim, dim, numOut] clockwise or counter clock-wise.
// Operation is in-place with FP16 weights.
// Notice that we only rotate square tensor, which is w = h = dim.
//
// @param weights pointer to FP16 tensor weight
// @param numIn batch number
// @param numOut color channel
// @param dim tensor dimension, which is w = h = dim
// @param rotCW True if rotate clockwise, otherwise rotate counter clockwise.
void rotateSquareTensorFP16(half_float::half *weights, int numIn, int numOut, int dim, bool rotCW);

// Rotate the model's convolution tensor CW or CCW and return the new model data.
// @param modelData model data from file
// @param rotCW True if rotate clockwise, otherwise rotate counter clockwise.
Vector<uint8_t> rotateModel(uint8_t *modelData, bool rotCW);

}  // namespace c8
