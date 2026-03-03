// Copyright (c) 2019 8th Wall, Inc.
// Original Author: Dat Chu (dat@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":base64",
    "//c8:string",
    "//c8:vector",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xa128687c);

#include <gtest/gtest.h>

#include <iostream>
#include <string>

#include "c8/pixels/base64.h"
#include "c8/string.h"
#include "c8/vector.h"

namespace c8 {

class Base64Test : public ::testing::Test {};

TEST_F(Base64Test, TestDecode) {
  auto decodedData = decode("YWJjMTIzIT8kKiYoKSctPUB+");
  const char *gt = "abc123!?$*&()'-=@~";
  ASSERT_EQ(18, decodedData.size());
  for (int i = 0; i < decodedData.size(); i++) {
    EXPECT_EQ(decodedData[i], gt[i])
      << "decoded=" << decodedData[i] << " gt=" << gt[i] << " at " << i;
  }
}

TEST_F(Base64Test, TestEncodeDecode) {
  std::vector<uint8_t> data = {11, 255, 1, 0, 38, 124};
  auto base64Data = encode(data);
  auto decodedData = decode(base64Data);
  for (int i = 0; i < data.size(); i++) {
    EXPECT_EQ(decodedData[i], data[i]);
  }
}

namespace {
String dataString(const Vector<uint8_t> &data) {
  return {reinterpret_cast<const char *>(data.data()), data.size()};
}
Vector<uint8_t> dataVector(const String &data) {
  Vector<uint8_t> vec(data.size());
  std::memcpy(vec.data(), data.data(), data.size());
  return vec;
}
int numTrailingEquals(const String &str) {
  if (str[str.size() - 1] != '=') {
    return 0;
  }
  return str[str.size() - 2] != '=' ? 1 : 2;
}
}  // namespace

TEST_F(Base64Test, TestEncodeDecodeStringNoEq) {
  String toEncode = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUFVWXZ!@#$%^&*()-_=+";
  // Test encoding and decoding each substring of the above string, with and without '=' character
  // padding.
  for (int i = 0; i < toEncode.size(); ++i) {
    // Encode the substring (with trailing "=")
    auto substr = toEncode.substr(0, i);
    auto substrBin = dataVector(substr);
    auto encoded = encode(substrBin);
    // Decode the encoded string and make sure that it's equal to the expected data.
    auto decoded = dataString(decode(encoded));
    EXPECT_EQ(toEncode.substr(0, i), decoded)
      << "Expected " << encoded << " to decode to " << substr << " but was " << decoded;
    // Trim trailing "=" from the encoded strings, and check that we can still decode.
    auto encodedSubstr = encoded.substr(0, encoded.size() - numTrailingEquals(encoded));
    auto decodedSubstr = dataString(decode(encodedSubstr));
    EXPECT_EQ(toEncode.substr(0, i), decodedSubstr)
      << "Expected " << encodedSubstr << " to decode to " << substr << "but was " << decodedSubstr;
  }
}

}  // namespace c8
