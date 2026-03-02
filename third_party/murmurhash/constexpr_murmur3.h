// The MIT License (MIT)
//
// Copyright (c) 2015, Tamás Szelei
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
#pragma once

#ifndef CONSTEXPR_MURMUR3_HASH_CONSTEXPR_MURMUR3_H
#define CONSTEXPR_MURMUR3_HASH_CONSTEXPR_MURMUR3_H

#include <stdint.h>
#include <stdexcept>

namespace ce_mm3 {

// Heavily based on sample code from
// http://en.cppreference.com/w/cpp/concept/LiteralType
class str_view {
public:
  // NOTE(mc): Added const char* with size constructor.
  constexpr str_view(const char *a, std::size_t size) : p(a), sz(size) {}

  template <std::size_t N>
  constexpr str_view(const char (&a)[N]) : p(a), sz(N - 1) {}

  constexpr char operator[](std::size_t n) const {
    if (n >= sz) {
      std::abort();
    }
    return p[n];
  }

  constexpr uint32_t get_block(int idx) {
    int i = (block_size() + idx) * 4;
    uint32_t b0 = (*this)[i];
    uint32_t b1 = (*this)[i + 1];
    uint32_t b2 = (*this)[i + 2];
    uint32_t b3 = (*this)[i + 3];
    return (b3 << 24) | (b2 << 16) | (b1 << 8) | b0;
  }

  constexpr std::size_t size() const { return sz; }

  constexpr std::size_t block_size() const { return sz / 4; }

  constexpr char tail(const int n) const {
    int tail_size = sz % 4;
    return p[sz - tail_size + n];
  }

private:
  const char *p;
  std::size_t sz;
};

constexpr uint32_t mm3_x86_32(str_view key, uint32_t seed) {
  uint32_t h1 = seed;

  const uint32_t c1 = 0xcc9e2d51;
  const uint32_t c2 = 0x1b873593;

  const int nblocks = key.size() / 4;
  for (int i = -nblocks; i; i++) {
    uint32_t k1 = key.get_block(i);
    k1 *= c1;
    k1 = (k1 << 15) | (k1 >> (32 - 15));
    k1 *= c2;
    h1 ^= k1;

    h1 = (h1 << 13) | (h1 >> (32 - 13));
    h1 = h1 * 5 + 0xe6546b64;
  }

  uint32_t k1 = 0;

  switch (key.size() & 3) {
    case 3:
      k1 ^= key.tail(2) << 16;
    case 2:
      k1 ^= key.tail(1) << 8;
    case 1:
      k1 ^= key.tail(0);
      k1 *= c1;
      k1 = (k1 << 15) | (k1 >> (32 - 15));
      k1 *= c2;
      h1 ^= k1;
  };

  h1 ^= key.size();

  h1 ^= h1 >> 16;
  h1 *= 0x85ebca6b;
  h1 ^= h1 >> 13;
  h1 *= 0xc2b2ae35;
  h1 ^= h1 >> 16;

  return h1;
}

constexpr uint32_t mm3_x86_32(const char *keyStr, std::size_t keySize, uint32_t seed) {
  str_view key(keyStr, keySize);
  return mm3_x86_32(key, seed);
}

}  // namespace ce_mm3

#endif
