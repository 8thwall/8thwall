// Copyright (c) 2024 Niantic, Inc.
// Original Author: Nicholas Butko (nbutko@nianticlabs.com)
//
// Half precision (16 bit) floating point numbers.

#pragma once

#include <array>
#include <cstdint>

namespace c8 {

uint16_t floatToHalf(float f);
float halfToFloat(uint16_t h);

uint32_t packHalf2x16(std::array<float, 2> v);
std::array<float, 2> unpackHalf2x16(uint32_t v);

}  // namespace c8
