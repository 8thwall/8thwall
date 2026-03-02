// Copyright (c) 2019 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)
//
// Encoding and decoding of pixel data in base64
// This uses the same alphabet and padding scheme that is compatible with dataURL scheme

#pragma once

#include <string>
#include <vector>

namespace c8 {
std::string encode(const std::vector<uint8_t> &data);
std::vector<uint8_t> decode(const std::string &base64Data);
}
