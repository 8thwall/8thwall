// Copyright (c) 2024 Niantic, Inc.
// Original Author: Nicholas Butko (nbutko@nianticlabs.com)
//
// Half precision (16 bit) floating point numbers.

#include "bzl/inliner/rules2.h"

cc_library {
  visibility = {
    "//visibility:public",
  };
  deps = {};
  hdrs = {
    "half.h",
  };
}
cc_end(0x46e98fb7);

#include <cmath>

#include "c8/half.h"

namespace c8 {

float halfToFloat(uint16_t h) {
  auto sgn = ((h >> 15) & 0x1);
  auto exponent = ((h >> 10) & 0x1f);
  auto mantissa = h & 0x3ff;

  float signMul = sgn == 1 ? -1.0 : 1.0;
  if (exponent == 0) {
    // Subnormal numbers (no exponent, 0 in the mantissa decimal).
    return signMul * std::pow(2.0f, -14.0f) * static_cast<float>(mantissa) / 1024.0f;
  }

  if (exponent == 31) {
    // Infinity or NaN.
    return mantissa != 0 ? 0.0f / 0.0f : signMul * 1.0f / 0.0f;
  }

  // non-zero exponent implies 1 in the mantissa decimal.
  return signMul * std::pow(2.0f, static_cast<float>(exponent) - 15.0f)
    * (1.0f + static_cast<float>(mantissa) / 1024.0f);
}

uint16_t floatToHalf(float f) {
  uint32_t f32 = *reinterpret_cast<uint32_t *>(&f);
  int sign = (f32 >> 31) & 0x01;        // 1 bit   -> 1 bit
  int exponent = ((f32 >> 23) & 0xff);  // 8 bits  -> 5 bits
  int mantissa = f32 & 0x7fffff;        // 23 bits -> 10 bits

  // Handle inf and nan from float.
  if (exponent == 0xFF) {
    if (mantissa == 0) {
      return (sign << 15) | 0x7C00; // Inf
    }

    return (sign << 15) | 0x7C01;  // Nan
  }

  // If the exponent is greater than the range of half, return +/- Inf.
  int centeredExp = exponent - 127;
  if (centeredExp > 15) {
    return (sign << 15) | 0x7C00;
  }

  // Normal numbers. centeredExp = [-15, 15]
  if (centeredExp > -15) {
    return (sign << 15) | ((centeredExp + 15) << 10) | (mantissa >> 13);
  }

  // Subnormal numbers.
  int fullMantissa = 0x800000 | mantissa;
  int shift = -(centeredExp + 14);  // Shift is in [-1 to -113]
  int newMantissa = fullMantissa >> shift;
  return (sign << 15) | (newMantissa >> 13);
}

uint32_t packHalf2x16(std::array<float, 2> v) { 
  return (floatToHalf(v[1]) << 16) | (floatToHalf(v[0])); 
}

std::array<float, 2> unpackHalf2x16(uint32_t v) {
  return {halfToFloat(v & 0xFFFF), halfToFloat(v >> 16)};
}

}  // namespace c8
