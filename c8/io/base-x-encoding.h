// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Alvin Portillo (alvin@8thwall.com)
//
// C++ implementation of the npm package, "base-x". (https://www.npmjs.com/package/base-x)
// Provides utilities for encoding and decoding a string into an arbitray base.
//
// The base is determined by the length of the alphabet provided:
// e.g. Base 16 -> new BaseXEncoding("0123456789abcdef")
//      Base 62 -> new
//      BaseXEncoding("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ")

#pragma once

#include "c8/map.h"
#include "c8/string.h"
#include "c8/vector.h"

namespace c8 {

class BaseXEncoding {
public:
  BaseXEncoding(const char *alphabet);
  BaseXEncoding(String alphabet);

  // Default move constructors.
  BaseXEncoding(BaseXEncoding &&) = default;
  BaseXEncoding &operator=(BaseXEncoding &&) = default;

  // Disallow copying.
  BaseXEncoding(const BaseXEncoding &) = delete;
  BaseXEncoding &operator=(const BaseXEncoding &) = delete;

  // Encodes the provided string into the alphabet / base specified at the time the encoder was created.
  std::unique_ptr<Vector<uint8_t>> encode(String str) const;
  std::unique_ptr<Vector<uint8_t>> encode(const char *cStr) const;

  // Decodes the provided string from the alphabet / base specified at the time the encoder was created.
  // If a character that is not in the encoder's alphabet is encountered, decoding will stop and an empty
  // vector will be returned.
  std::unique_ptr<Vector<uint8_t>> decode(String str);
  std::unique_ptr<Vector<uint8_t>> decode(const char *cStr);

private:
  String alphabet_;
  int base_;
  TreeMap<char, uint32_t> alphabetMap_;

  void setUpAlphabetMap();
};

}  // namespace c8
