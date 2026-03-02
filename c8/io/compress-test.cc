// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":compress",
    "//c8:string",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xd3096c91);

#include "c8/io/compress.h"
#include "gtest/gtest.h"

namespace c8 {

class CompressTest : public ::testing::Test {};

TEST_F(CompressTest, Compress) {
  // Create a test string.
  String contents = R"(
    What a to-do to die today, at a minute or two to two;
    a thing distinctly hard to say, but harder still to do.
    We'll beat a tattoo, at twenty to two
    a rat-tat-tat- tat-tat-tat- tat-tat-tattoo
    and the dragon will come when he hears the drum
    at a minute or two to two today, at a minute or two to two.
  )";

  Vector<uint8_t> compressed;
  EXPECT_TRUE(compressGzipped(contents, &compressed));
  EXPECT_LT(compressed.size(), contents.size());

  String decompressed;
  EXPECT_TRUE(decompressGzipped(compressed, &decompressed));
  EXPECT_EQ(contents, decompressed);
}

}  // namespace c8
