// Copyright (c) 2025 Niantic, Inc.
// Original Author: Erik Murphy-Chutorian (mc@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":circular-memcpy",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xd1afab05);

#include "c8/command/circular-memcpy.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace c8 {

class CircularMemcpyTest : public ::testing::Test {};


TEST_F(CircularMemcpyTest, TestCircularMemcpyStore) {
  char buffer[9];
  char *bufferStart = buffer;
  char *bufferEnd = buffer + 8;

  // Fill buffer with x's followed by a '\0' to support string comparisons.
  std::memset(buffer, 'x', sizeof(buffer));
  buffer[8] = '\0';

  // Write 5 bytes to the buffer.
  char *marker = circularMemcpyStore(bufferStart, "Hello", 5, bufferStart, bufferEnd);
  EXPECT_EQ(marker, bufferStart + 5);
  EXPECT_STREQ(bufferStart, "Helloxxx");

  // Write 4 more bytes to the buffer, wrapping around.
  marker = circularMemcpyStore(marker, "there", 4, bufferStart, bufferEnd);
  EXPECT_EQ(marker, bufferStart + 1);
  EXPECT_STREQ(bufferStart, "rellothe");

  // Write 3 more bytes to the buffer.
  marker = circularMemcpyStore(marker, "here", 3, bufferStart, bufferEnd);
  EXPECT_EQ(marker, bufferStart + 4);
  EXPECT_STREQ(bufferStart, "rherothe");
}

TEST_F(CircularMemcpyTest, TestCircularMemcpyLoad) {
  const char srcBuffer[] = "Hello, World!";
  const char *src = srcBuffer;
  const char *srcStart = srcBuffer;
  const char *srcEnd = src + strlen(srcBuffer);

  char destBuffer[33];
  char *dest = destBuffer;
  // Fill buffer with x's followed by a '\0' to support string comparisons.
  std::memset(destBuffer, 'x', sizeof(destBuffer));
  dest[32] = '\0';

  // Read 5 bytes from the buffer.
  const char *marker = circularMemcpyLoad(dest, src, 5, srcStart, srcEnd);
  EXPECT_EQ(marker, src + 5);
  EXPECT_STREQ(dest, "Helloxxxxxxxxxxxxxxxxxxxxxxxxxxx");

  // Read 7 bytes from the buffer.
  marker = circularMemcpyLoad(dest + 5, marker, 7, srcStart, srcEnd);
  EXPECT_EQ(marker, src + 12);
  EXPECT_STREQ(dest, "Hello, Worldxxxxxxxxxxxxxxxxxxxx");

  // Read 9 bytes from the buffer and wrap around.
  marker = circularMemcpyLoad(dest + 12, marker, 9, srcStart, srcEnd);
  EXPECT_EQ(marker, src + 8);
  EXPECT_STREQ(dest, "Hello, World!Hello, Wxxxxxxxxxxx");
}

}  // namespace c8
