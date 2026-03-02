// Copyright (c) 2022 Niantic, Inc.
// Original Author: Dat Chu (datchu@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"packed.h"};
  visibility = {
    "//visibility:public",
  };
  deps = {
    "//c8:exceptions"
  };
}
cc_end(0xfbcea514);

#include "c8/pixels/packed.h"
#include "c8/exceptions.h"
#include <cmath>

namespace c8 {
// You can pick a BASE that is smaller than 256 as well but it won't use all the bits in a uint8_t
// and thus the error is slightly higher. 256 results in error less than 1e-8 while 255 has an error
// around as high as 1e-5
constexpr float BASE = 256.f;

inline float fract(float x) {
  return x - floor(x);
}

float decodeFloat(uint8_t x, uint8_t y, uint8_t z, uint8_t a) {
  return (x + y / BASE + z / (BASE * BASE) + a / (BASE * BASE * BASE))
    / BASE;
}

// How this works. Note that this method uses multiplication instead of bit-shifting to emulate what
// GPU would do.
// Let's assume that we have a number between [0, 1) for example 0.41235. To encode this number with
// only 8 bits, we multiply the number by 2**8: 0.41235 * 256 = 105.5616. So we can store 105 into
// our first 8-bit memory and have an encoding error of 0.5616. To encode this error value 0.5616
// into the next 8 bits, we do the same procedure again: 0.5616 * 256 = 143.7696. We thus store 143
// into our second 8-bit memory and have an encoding error of 0.7696. We repeat this a total of 4
// times since we have 4 uint8_t. This is the same as storing a number using in base 256.
void encodeFloat01(float value, uint8_t *x, uint8_t *y, uint8_t *z, uint8_t *w) {
  if (value < 0 || value >= 1) {
    C8_THROW("encode float only support float in [0, 1)");
  }
  float floatX = fract(value * 1.0f);
  float floatY = fract(value * BASE);
  float floatZ = fract(value * BASE * BASE);
  float floatW = fract(value * BASE * BASE * BASE);
  *x = BASE * (floatX - floatY / BASE);
  *y = BASE * (floatY - floatZ / BASE);
  *z = BASE * (floatZ - floatW / BASE);
  *w = BASE * (floatW);
}


}
