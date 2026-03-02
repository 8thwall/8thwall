// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":embedded-images",
    ":embedded-limericks",
    "//c8/io:image-io",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x31af4a8e);

#include "bzl/examples/embed/embedded-images.h"
#include "bzl/examples/embed/embedded-limericks.h"
#include "c8/io/image-io.h"
#include "gtest/gtest.h"

namespace c8 {

class EmbeddedDataTest : public ::testing::Test {};

TEST_F(EmbeddedDataTest, BinaryData) {
  EXPECT_EQ(395909, embeddedFlowerBigJpgView.size());
  EXPECT_EQ(116524, embeddedAmityUpJpgSize);

  auto flower = readJpgToRGB(embeddedFlowerBigJpgData, embeddedFlowerBigJpgSize);
  auto amity = readJpgToRGBA(
    reinterpret_cast<const uint8_t *>(embeddedAmityUpJpgView.data()),
    embeddedAmityUpJpgView.size());

  EXPECT_EQ(1920, flower.pixels().rows());
  EXPECT_EQ(1440, flower.pixels().cols());
  EXPECT_EQ(640, amity.pixels().rows());
  EXPECT_EQ(480, amity.pixels().cols());
}

TEST_F(EmbeddedDataTest, TextData) {
  EXPECT_EQ(169, strlen(embeddedLimerickTxtCStr));
  EXPECT_EQ(169, embeddedLimerickTxtView.size());

  EXPECT_EQ(194, strlen(embeddedLimerick2TxtCStr));
  EXPECT_EQ(194, embeddedLimerick2TxtView.size());

  EXPECT_EQ(embeddedLimerickTxtCStr[169], '\0');
  EXPECT_EQ(embeddedLimerick2TxtCStr[194], '\0');

  EXPECT_EQ(
    std::string_view(R"(There was an Old Man in a tree,
Who was horribly bored by a bee.
When they said "Does it buzz?"
He replied "Yes, it does!
It's a regular brute of a bee!"
—Edward Lear
)"),
    embeddedLimerickTxtView);

  EXPECT_STREQ(
    R"(There was a small boy of Quebec
Who was buried in snow to his neck
When they said, ‘Are you friz?’
He replied, ‘Yes, I is —
But we don’t call this cold in Quebec.'
– Rudyard Kipling
)",
    embeddedLimerick2TxtCStr);
}

}  // namespace c8
