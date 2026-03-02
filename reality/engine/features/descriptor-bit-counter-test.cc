// Copyright (c) 2024 Niantic, Inc.
// Original Author: Haomin Zhu (hzhu@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    "@com_google_googletest//:gtest_main",
    ":descriptor-bit-counter",
  };
}
cc_end(0x65907830);

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <array>

#include "reality/engine/features/descriptor-bit-counter.h"

namespace c8 {

class DescriptorBitCounterTest : public ::testing::Test {
protected:
  static constexpr size_t N = 8;
};

TEST_F(DescriptorBitCounterTest, TestAddAndSubtractOneCounts) {
  constexpr uint8_t desc1Data[] = {0b000, 0b011, 0b110, 0b011, 0b100, 0b111, 0b010, 0b111};
  constexpr uint8_t desc2Data[] = {0b111, 0b110, 0b101, 0b100, 0b011, 0b010, 0b101, 0b000};
  constexpr std::array<size_t, N * 8> trueOneCounts = {
    0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 2, 1, 0, 0, 0, 0, 0, 2, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1,
    0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 2, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1};

  std::array<size_t, N * 8> oneCounts;
  oneCounts.fill(0);

  addOneCounts<N>(desc1Data, &oneCounts);
  EXPECT_THROW(subtractOneCounts<N>(desc2Data, &oneCounts), std::runtime_error);
  addOneCounts<N>(desc2Data, &oneCounts);

  for (size_t i = 0; i < oneCounts.size(); ++i) {
    EXPECT_EQ(trueOneCounts[i], oneCounts[i]);
  }

  subtractOneCounts<N>(desc2Data, &oneCounts);
  subtractOneCounts<N>(desc1Data, &oneCounts);
  EXPECT_THROW(subtractOneCounts<N>(desc1Data, &oneCounts), std::runtime_error);

  for (size_t i = 0; i < oneCounts.size(); ++i) {
    EXPECT_EQ(0, oneCounts[i]);
  }
}

TEST_F(DescriptorBitCounterTest, TestGetMajorityFromOneCounts) {
  constexpr uint8_t desc1Data[] = {0b000, 0b011, 0b110, 0b011, 0b100, 0b111, 0b010, 0b111};
  constexpr uint8_t desc2Data[] = {0b111, 0b110, 0b101, 0b100, 0b011, 0b010, 0b101, 0b000};
  constexpr uint8_t desc3Data[] = {0b000, 0b011, 0b110, 0b011, 0b100, 0b111, 0b010, 0b111};
  constexpr uint8_t desc4Data[] = {0b111, 0b110, 0b101, 0b100, 0b011, 0b010, 0b101, 0b000};
  constexpr uint8_t desc5Data[] = {0b000, 0b011, 0b110, 0b011, 0b100, 0b111, 0b010, 0b111};
  // nth bit below is set to 1 if majority of nth bits above are 1
  constexpr uint8_t trueMajor[] = {0b000, 0b011, 0b110, 0b011, 0b100, 0b111, 0b010, 0b111};

  std::array<size_t, N * 8> oneCounts;
  oneCounts.fill(0);

  // Add one counts.
  addOneCounts<N>(desc1Data, &oneCounts);
  addOneCounts<N>(desc2Data, &oneCounts);
  addOneCounts<N>(desc3Data, &oneCounts);
  addOneCounts<N>(desc4Data, &oneCounts);
  addOneCounts<N>(desc5Data, &oneCounts);

  uint8_t descTData[] = {0, 0, 0, 0, 0, 0, 0, 0};
  getMajorityFromOneCounts<N>(oneCounts, 5 / 2, descTData);

  for (size_t i = 0; i < N; ++i) {
    EXPECT_EQ(trueMajor[i], descTData[i]);
  }

  // Subtract one counts.
  subtractOneCounts<N>(desc1Data, &oneCounts);
  subtractOneCounts<N>(desc2Data, &oneCounts);
  subtractOneCounts<N>(desc3Data, &oneCounts);
  subtractOneCounts<N>(desc4Data, &oneCounts);
  subtractOneCounts<N>(desc5Data, &oneCounts);

  getMajorityFromOneCounts<N>(oneCounts, 5 / 2, descTData);

  // Should be all zeroes after subtractions.
  for (size_t i = 0; i < oneCounts.size(); ++i) {
    EXPECT_EQ(0, oneCounts[i]);
  }
  for (size_t i = 0; i < N; ++i) {
    EXPECT_EQ(0, descTData[i]);
  }
}

}  // namespace c8
