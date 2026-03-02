// Copyright (c) 2019 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)
//
// Encoding and decoding of pixel data in base64

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "base64.h"
  };
  deps = {
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x413acf62);

#include "c8/pixels/base64.h"

#include <sstream>
using std::stringstream;

namespace c8 {
namespace {
  const char *alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  uint8_t charToNum(const char c) {
    if (c >= 97) {
      // a-z
      return c - 'a' + 26;
    }
    if (c >= 65) {
      // A-Z
      return c - 'A';
    }
    if (c == '+') {
      return 62;
    }
    if (c == '/') {
      return 63;
    }
    // 0-9
    return c - '0' + 52;
  }

  void charsToNums(const char *chars, uint8_t *nums) {
    for (int i = 0; i < 4; i++)
      nums[i] = charToNum(chars[i]);
  }
}


std::string encode(const std::vector<uint8_t> &data) {
  if (data.size() == 0) {
    return "";
  }

  std::stringstream outBuffer;
  int len = data.size();
  int i = 0;
  uint8_t c1, c2, c3;
  while (i < len) {
    c1 = data[i++];
    if (i == len) {
      outBuffer << alphabet[c1 >> 2];
      outBuffer << alphabet[(c1 & 0x3) << 4];
      outBuffer << "==";
      break;
    }
    c2 = data[i++];
    if (i == len) {
      outBuffer << alphabet[c1 >> 2];
      outBuffer << alphabet[((c1 & 0x3) << 4) | ((c2 & 0xF0) >> 4)];
      outBuffer << alphabet[(c2 & 0xF) << 2];
      outBuffer << "=";
      break;
    }
    c3 = data[i++];
    outBuffer << alphabet[c1 >> 2];
    outBuffer << alphabet[((c1 & 0x3) << 4) | ((c2 & 0xF0) >> 4)];
    outBuffer << alphabet[((c2 & 0xF) << 2) | ((c3 & 0xC0) >> 6)];
    outBuffer << alphabet[c3 & 0x3F];
  }
  return outBuffer.str();
}

std::vector<uint8_t> decode(const std::string &base64Data) {
  bool hasPadding = base64Data[base64Data.size() -1] == '=';
  if (!hasPadding && base64Data.size() % 4 != 0) {
    // Add at most two "==" for padding.
    return decode(base64Data + (base64Data.size() % 4 == 1 ? "=" : "=="));
  }
  const int outputTriplet = base64Data.size() / 4;
  std::vector<uint8_t> outputData;
  if (outputTriplet == 0) {
    return outputData;
  }

  outputData.reserve(outputTriplet * 3);

  uint8_t quadNums[4];
  const char* quad = base64Data.c_str();

  // process normally except for the last group if there is padding
  for (int i = 0; i < outputTriplet - hasPadding; i++) {
    charsToNums(quad, quadNums);
    // four characters to 3 bytes
    outputData.push_back((quadNums[0] << 2) | ((quadNums[1] & 0x30) >> 4));
    outputData.push_back(((quadNums[1] & 0xF) << 4) | ((quadNums[2] & 0x3C) >> 2));
    outputData.push_back(((quadNums[2] & 0x3) << 6) | quadNums[3]);
    quad += 4;
  }

  charsToNums(quad, quadNums);
  if (hasPadding) {
    if (quad[2] == '=') {
      // only have one byte to decode from 2 characters
      outputData.push_back((quadNums[0] << 2) | ((quadNums[1] & 0x30) >> 4));
    } else {
      // two bytes to decode from 3 characters
      outputData.push_back((quadNums[0] << 2) | ((quadNums[1] & 0x30) >> 4));
      outputData.push_back(((quadNums[1] & 0xF) << 4) | ((quadNums[2] & 0x3c) >> 2));
    }
  }

  return outputData;
}

}
