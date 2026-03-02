// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Paris Morgan (paris@8thwall.com)
//
// Hash helper functions.

#pragma once

#include <type_traits>

#include "third_party/smhasher/MurmurHash3.h"

namespace c8 {

// Compute the MurmurHash on the input object and combine with the seed. Note that
// the order of the calls matters. Note that we used a constant seed to MurmurHash3.
// @param seed a reference to the hash being incrementally added to.
template <typename T>
void murmurHash32Combine(const T &v, uint32_t *seed) {
  static_assert(std::is_pod<T>::value, "Only POD types are supported.");
  static_assert(
    !std::is_pointer<T>::value,
    "Can not use pointer since byte size of object pointed to is unknown");
  uint32_t h;
  MurmurHash3_x86_32(&v, sizeof(v), 123456789, &h);
  *seed ^= h + 0x9e3779b9 + (*seed << 6) + (*seed >> 2);
}

uint32_t murmurHash32(const char *v, int sizeOfV);

}  // namespace c8
