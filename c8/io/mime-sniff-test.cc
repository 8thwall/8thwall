// Copyright (c) 2024 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":file-io",
    ":mime-sniff",
    "//c8:string",
    "//c8:vector",
    "@com_google_googletest//:gtest_main",
  };
  data = {
    ":tiny-jpg",
    ":tiny-png",
    ":tiny-gif",
  };
}
cc_end(0xf8ffb9a2);

#include <span>
#include <string>

#include "c8/io/file-io.h"
#include "c8/io/mime-sniff.h"
#include "c8/string.h"
#include "c8/vector.h"
#include "gtest/gtest.h"

using c8::String;
using c8::Vector;

namespace {
template <int N>
std::span<const uint8_t> toBytes(const char (&str)[N]) {
  return std::span<const uint8_t>(reinterpret_cast<const uint8_t *>(str), N - 1);
}

static const char *tinyJpgPath() {
  static String dir = "c8/io/tiny-image.jpg";
  return dir.c_str();
}

static const char *tinyPngPath() {
  static String dir = "c8/io/tiny-image.png";
  return dir.c_str();
}

static const char *tinyGifPath() {
  static String dir = "c8/io/tiny-image.gif";
  return dir.c_str();
}
}  // namespace

namespace c8 {

class MimeSniffTest : public ::testing::Test {};

TEST_F(MimeSniffTest, testSupportedBitstreams) {
  EXPECT_EQ(MimeType::IMAGE_PNG, matchImageMimeType(toBytes("\x89\x50\x4E\x47\x0D\x0A\x1A\x0A")));
  EXPECT_EQ(MimeType::IMAGE_JPEG, matchImageMimeType(toBytes("\xFF\xD8\xFF")));
  EXPECT_EQ(
    MimeType::IMAGE_WEBP,
    matchImageMimeType(toBytes("\x52\x49\x46\x46\x00\x00\x00\x00\x57\x45\x42\x50\x56\x50")));
  EXPECT_EQ(MimeType::IMAGE_GIF, matchImageMimeType(toBytes("\x47\x49\x46\x38\x39\x61")));
  EXPECT_EQ(MimeType::IMAGE_X_ICON, matchImageMimeType(toBytes("\x00\x00\x01\x00")));
  EXPECT_EQ(MimeType::IMAGE_BMP, matchImageMimeType(toBytes("\x42\x4D")));
}

TEST_F(MimeSniffTest, testBadBitstreams) {
  EXPECT_EQ(MimeType::UNDEFINED, matchImageMimeType(toBytes("\x00\x00\x00\x00")));
  EXPECT_EQ(MimeType::UNDEFINED, matchImageMimeType(toBytes("\x00\x01\x02\x03\x04\x05")));
}

TEST_F(MimeSniffTest, testJpeg) {
  const Vector<uint8_t> jpegBytes = readFile(tinyJpgPath());
  EXPECT_EQ(MimeType::IMAGE_JPEG, matchImageMimeType(jpegBytes));
}

TEST_F(MimeSniffTest, testPng) {
  const Vector<uint8_t> pngBytes = readFile(tinyPngPath());
  EXPECT_EQ(MimeType::IMAGE_PNG, matchImageMimeType(pngBytes));
}

TEST_F(MimeSniffTest, testGif) {
  const Vector<uint8_t> gifBytes = readFile(tinyGifPath());
  EXPECT_EQ(MimeType::IMAGE_GIF, matchImageMimeType(gifBytes));
}

TEST_F(MimeSniffTest, testMimeStrings) {
  EXPECT_EQ("image/x-icon", mimeTypeString(MimeType::IMAGE_X_ICON));
  EXPECT_EQ("image/bmp", mimeTypeString(MimeType::IMAGE_BMP));
  EXPECT_EQ("image/gif", mimeTypeString(MimeType::IMAGE_GIF));
  EXPECT_EQ("image/webp", mimeTypeString(MimeType::IMAGE_WEBP));
  EXPECT_EQ("image/png", mimeTypeString(MimeType::IMAGE_PNG));
  EXPECT_EQ("image/jpeg", mimeTypeString(MimeType::IMAGE_JPEG));
}

}  // namespace c8
