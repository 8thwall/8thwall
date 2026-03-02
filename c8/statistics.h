// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Paris Morgan (paris@8thwall.com)
//
// Common statistical methods.

#pragma once

#include "c8/vector.h"

namespace c8 {

// Computes the mean.
float mean(const Vector<float> &v);

// Computes the population standard deviation.
float stdDev(const Vector<float> &v, float mean);

// Computes the full discrete linear cross-correlation of two one-dimensional arrays. Careful with
// use in prod as this is slow. Can speed up by only comparing every nth data point with an offset,
// or only comparing every mth offset
Vector<float> crossCorrelateFull(const Vector<float> &x, const Vector<float> &y);

}  // namespace c8
