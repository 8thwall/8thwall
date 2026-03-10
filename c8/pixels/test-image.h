// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include <array>
#include <cstdint>

#include "c8/pixels/pixels.h"

namespace c8 {

void runTestImageBenchmark(int r, int c, std::array<float, 4> *meanPixelValues, int64_t *micros);

void fillTestImage(RGBA8888PlanePixels tp);

void fillTestImage(RGBA8888PlanePixels tp, int64_t seed);

bool eqImg(RGBA8888PlanePixels a, RGBA8888PlanePixels b, int thresh);

}  // namespace c8
