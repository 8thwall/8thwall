// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Alvin Portillo (alvin@8thwall.com)
//
// Unit tests for base-x-encoding.cc

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":base-x-encoding",
    "//c8:string",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xe5b1d6f3);

#include "c8/io/base-x-encoding.h"
#include "c8/string.h"

#include "gtest/gtest.h"

namespace c8 {

class BaseXEncodingTest : public ::testing::Test {};
String encodeToString(const BaseXEncoding& encoding, const String& text) {
  auto encodedBuffer = encoding.encode(text);
  return String(encodedBuffer->begin(), encodedBuffer->end());
}

TEST_F(BaseXEncodingTest, TestEncodeBase62) {
  BaseXEncoding encoding("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
  EXPECT_STREQ("7mJG2Rg", encodeToString(encoding, "alvin").c_str());
}

TEST_F(BaseXEncodingTest, TestEncodeBase58) {
  BaseXEncoding encoding("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUV");
  EXPECT_STREQ("75p7uvTyxe3x7duGKxzz8640d8HMw2GlCsPOQIRtREab", encodeToString(encoding, "if you ain\'t first, you\'re last'").c_str());
}

TEST_F(BaseXEncodingTest, TestEncodeBase16) {
  BaseXEncoding encoding("0123456789abcdef");
  EXPECT_STREQ("7368616b6520616e642062616b65", encodeToString(encoding, "shake and bake").c_str());
}

TEST_F(BaseXEncodingTest, TestEncodeReturnsEmptyStringOnNullString) {
  BaseXEncoding encoding("0123456789abcdef");
  auto encodedBuffer = encoding.encode(nullptr);
  EXPECT_TRUE(encodedBuffer->empty());
}

TEST_F(BaseXEncodingTest, testEncodeReturnsEmptyStringOnEmptyString) {
  BaseXEncoding encoding("0123456789abcdef");
  auto encodedBuffer = encoding.encode("");
  EXPECT_TRUE(encodedBuffer->empty());
}

TEST_F(BaseXEncodingTest, TestDecodeBase62) {
  BaseXEncoding encoding("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
  auto decodedBuffer = encoding.decode("007mJG2Rg");
  String decodedStr(decodedBuffer->begin(), decodedBuffer->end());
  EXPECT_STREQ("\u0000\u0000alvin", decodedStr.c_str());
}

TEST_F(BaseXEncodingTest, TestDecodeBase58) {
  BaseXEncoding encoding("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUV");
  auto decodedBuffer = encoding.decode("75p7uvTyxe3x7duGKxzz8640d8HMw2GlCsPOQIRtREab");
  String decodedStr(decodedBuffer->begin(), decodedBuffer->end());
  EXPECT_STREQ("if you ain\'t first, you\'re last'", decodedStr.c_str());
}

TEST_F(BaseXEncodingTest, TestDecodeBase16) {
  BaseXEncoding encoding("0123456789abcdef");
  auto decodedBuffer = encoding.decode("7368616b6520616e642062616b65");
  String decodedStr(decodedBuffer->begin(), decodedBuffer->end());
  EXPECT_STREQ("shake and bake", decodedStr.c_str());
}

TEST_F(BaseXEncodingTest, TestDecodeReturnsEmptyVectorOnNullString) {
  BaseXEncoding encoding("0123456789abcdef");
  auto decodedBuffer = encoding.decode(nullptr);
  EXPECT_TRUE(decodedBuffer->empty());
}

TEST_F(BaseXEncodingTest, TestDecodeReturnsEmptyVectorOnEmptyString) {
  BaseXEncoding encoding("0123456789abcdef");
  auto decodedBuffer = encoding.decode("");
  EXPECT_TRUE(decodedBuffer->empty());
}

TEST_F(BaseXEncodingTest, TestDecodeReturnsEmptyVectorOnInvalidString) {
  BaseXEncoding encoding("0123456789abcdef");
  auto decodedBuffer = encoding.decode("This contains non-hex values.");
  EXPECT_TRUE(decodedBuffer->empty());
}

}  // namespace c8
