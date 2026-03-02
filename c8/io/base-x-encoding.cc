// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Alvin Portillo (alvin@8thwall.com)

#include "bzl/inliner/rules.h"

cc_library {
  hdrs = {
    "base-x-encoding.h",
  };
  deps = {
    "//bzl/inliner:rules",
    "//c8:map",
    "//c8:string",
    "//c8:vector",
  };
  visibility = {
    "//visibility:public",
  };
}

#include <string.h>
#include <memory>
#include "c8/io/base-x-encoding.h"
#include "c8/vector.h"

namespace c8 {

BaseXEncoding::BaseXEncoding(const char *alphabet) {
  alphabet_ = String(alphabet);
  base_ = alphabet_.length();
  setUpAlphabetMap();
}

BaseXEncoding::BaseXEncoding(String alphabet) : alphabet_(alphabet) {
  base_ = alphabet_.length();
  setUpAlphabetMap();
}

void BaseXEncoding::setUpAlphabetMap() {
  for (int i = 0; i < alphabet_.length(); ++i) {
    char c = alphabet_.at(i);
    alphabetMap_.insert({c, i});
  }
}

std::unique_ptr<Vector<uint8_t>> BaseXEncoding::encode(const char *cStr) const {
  if (cStr == nullptr) {
    auto outBuffer = std::make_unique<Vector<uint8_t>>();
    return outBuffer;
  }

  String str = String(cStr);
  return encode(str);
}

std::unique_ptr<Vector<uint8_t>> BaseXEncoding::encode(String str) const {
  auto outBuffer = std::make_unique<Vector<uint8_t>>();
  if (str.length() == 0) {
    return outBuffer;
  }

  Vector<uint32_t> digits;
  digits.push_back(0);

  uint32_t carry = 0;
  for (int i = 0; i < str.length(); ++i) {
    carry = str.at(i);
    for (int j = 0; j < digits.size(); ++j) {
      carry += digits[j] << 8;
      digits[j] = (carry % base_);
      carry = (carry / base_) | 0;
    }

    while (carry > 0) {
      digits.push_back(carry % base_);
      carry = (carry / base_) | 0;
    }
  }

  for (int i = 0; str.at(i) == 0 && i < str.length() - 1; ++i) {
    outBuffer->push_back(alphabet_.at(0));
  }

  for (int i = digits.size() - 1; i >= 0; --i) {
    outBuffer->push_back(alphabet_.at(digits[i]));
  }

  return outBuffer;
}

std::unique_ptr<Vector<uint8_t>> BaseXEncoding::decode(const char *cStr) {
  if (cStr == nullptr) {
    auto outBuffer = std::make_unique<Vector<uint8_t>>();
    return outBuffer;
  }

  String str = String(cStr);
  return decode(str);
}

std::unique_ptr<Vector<uint8_t>> BaseXEncoding::decode(String str) {
  auto outBuffer = std::make_unique<Vector<uint8_t>>();
  if (str.length() == 0) {
    return outBuffer;
  }

  Vector<uint8_t> bytes;
  bytes.push_back(0);

  for (int i = 0; i < str.length(); ++i) {
    auto value = alphabetMap_.find(str.at(i));

    if (value == alphabetMap_.end()) {
      return outBuffer;
    }

    uint32_t carry = value->second;
    for (int j = 0; j < bytes.size(); ++j) {
      carry += bytes[j] * base_;
      uint8_t b = carry & 0xff;
      bytes[j] = b;
      carry >>= 8;
    }

    while (carry > 0) {
      uint8_t b = carry & 0xff;
      bytes.push_back(b);
      carry >>= 8;
    }
  }

  for (int i = 0; str.at(i) == alphabet_.at(0) && i < str.length() - 1; ++i) {
    bytes.push_back(0);
  }

  for (auto it = bytes.rbegin(); it != bytes.rend(); it++) {
    outBuffer->push_back(*it);
  }

  return outBuffer;
}
}  // namespace c8
