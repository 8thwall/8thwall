// Copyright (c) 2024 Niantic, Inc.
// Original Author: Nicholas Butko (nbutko@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":arc4",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xa30ff86f);

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "c8/io/arc4.h"

using testing::Eq;
using testing::Pointwise;

namespace c8 {

class Arc4Test : public ::testing::Test {};

TEST_F(Arc4Test, TestEmpty) {
  auto result = arc4(nullptr, 0, nullptr, 0, 0);
  Vector<uint8_t> expected;
  EXPECT_THAT(result, Pointwise(Eq(), expected));
}

TEST_F(Arc4Test, TestPlaintext) {
  Vector<uint8_t> expected = {0xbb, 0xf3, 0x16, 0xe8, 0xd9, 0x40, 0xaf, 0x0a, 0xd3};
  const char *msg = "Plaintext";
  const char *key = "Key";
  auto encoded = arc4(
    reinterpret_cast<const uint8_t *>(msg),
    strlen(msg),
    reinterpret_cast<const uint8_t *>(key),
    strlen(key),
    0);
  EXPECT_THAT(encoded, Pointwise(Eq(), expected));

  auto decoded =
    arc4(encoded.data(), encoded.size(), reinterpret_cast<const uint8_t *>(key), strlen(key), 0);

  EXPECT_THAT(decoded, Pointwise(Eq(), Vector<uint8_t>(msg, msg + strlen(msg))));
}

TEST_F(Arc4Test, TestWikipedia) {
  Vector<uint8_t> expected = {0x10, 0x21, 0xbf, 0x04, 0x20};
  const char *key = "Wiki";
  const char *msg = "pedia";

  auto encoded = arc4(
    reinterpret_cast<const uint8_t *>(msg),
    strlen(msg),
    reinterpret_cast<const uint8_t *>(key),
    strlen(key),
    0);
  EXPECT_THAT(encoded, Pointwise(Eq(), expected));

  auto decoded =
    arc4(encoded.data(), encoded.size(), reinterpret_cast<const uint8_t *>(key), strlen(key), 0);

  EXPECT_THAT(decoded, Pointwise(Eq(), Vector<uint8_t>(msg, msg + strlen(msg))));
}

TEST_F(Arc4Test, TestSecret) {
  Vector<uint8_t> expected = {
    0x45, 0xa0, 0x1f, 0x64, 0x5f, 0xc3, 0x5b, 0x38, 0x35, 0x52, 0x54, 0x4b, 0x9b, 0xf5};

  const char *key = "Secret";
  const char *msg = "Attack at dawn";

  auto encoded = arc4(
    reinterpret_cast<const uint8_t *>(msg),
    strlen(msg),
    reinterpret_cast<const uint8_t *>(key),
    strlen(key),
    0);
  EXPECT_THAT(encoded, Pointwise(Eq(), expected));

  auto decoded =
    arc4(encoded.data(), encoded.size(), reinterpret_cast<const uint8_t *>(key), strlen(key), 0);

  EXPECT_THAT(decoded, Pointwise(Eq(), Vector<uint8_t>(msg, msg + strlen(msg))));
}

TEST_F(Arc4Test, TestSkip) {
  const char *key = "Skip";
  const char *msg = "this message";
  int skip = 42;

  auto encoded = arc4(
    reinterpret_cast<const uint8_t *>(msg),
    strlen(msg),
    reinterpret_cast<const uint8_t *>(key),
    strlen(key),
    skip);

  auto decoded =
    arc4(encoded.data(), encoded.size(), reinterpret_cast<const uint8_t *>(key), strlen(key), skip);

  EXPECT_THAT(decoded, Pointwise(Eq(), Vector<uint8_t>(msg, msg + strlen(msg))));
}

}  // namespace c8
