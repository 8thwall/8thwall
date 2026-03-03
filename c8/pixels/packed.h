// Copyright (c) 2022 Niantic, Inc.
// Original Author: Dat Chu (datchu@nianticlabs.com)

#pragma once

namespace c8 {

// Recover a float value from four 8-bit unsigned ints encoded with encodeFloat01
float decodeFloat(uint8_t x, uint8_t y, uint8_t z, uint8_t a);

// Encode a float value in [0, 1) into four 8-bit unsigned ints
// @param value a float value in [0, 1)
void encodeFloat01(float value, uint8_t *x, uint8_t *y, uint8_t *z, uint8_t *w);

}  // namespace c8
