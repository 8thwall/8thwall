// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":gr8",
    "//c8:c8-log",
    "//third_party/cvlite/core:core",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x1a22c026);

#include "reality/engine/features/gr8.h"

#include "c8/c8-log.h"
#include <queue>
#include <vector>
#include "gtest/gtest.h"

namespace c8 {

namespace {
  struct KeypointResponseMore {
  inline bool operator()(const c8cv::KeyPoint &kp1, const c8cv::KeyPoint &kp2) const {
    return kp1.response > kp2.response;
  }
};
struct KeypointResponsePairMore {
  inline bool operator()(
    const std::pair<int, c8cv::KeyPoint> &kp1,
    const std::pair<int, c8cv::KeyPoint> &kp2) const {
    return kp1.first != kp2.first ? kp1.first < kp2.first
                                  : kp1.second.response > kp2.second.response;
  }
};
}

class Gr8Test : public ::testing::Test {
public:
  const float size = -1.0f;
  const float angle = -1.0f;
  int numRows = 32*3, numCols = 32*3;
  // Expected sort: 9, 7, 6, 4, 3, 1, 8, 5, 2
  // /-----------------------------------------\
  // |      ;      |      ;      |      ;      |
  // |      ;      |      ;      |      ;      |
  // |      ;     2|      ;      |      ;      |
  // |~~~~~~;~~~~~~|~~~~~~;~~~~~~|~~~~~~;~~~~~~|
  // |      ;      |      ;      |      ;      |
  // |      ;      |      ;      |      ;      |
  // |      ;     7|      ;      |      ;      |
  // |-------------|-------------|-------------|
  // |      ;      |6     ;5     |      ;      |
  // |      ;      |      ;      |      ;      |
  // |      ;      |      ;      |      ;      |
  // |~~~~~~;~~~~~~|~~~~~~;~~~~~~|~~~~~~;~~~~~~|
  // |      ;      |      ;      |    8 ;      |
  // |      ;      |      ;      |      ;      |
  // |      ;      |      ;      | 9    ;      |
  // |-------------|-------------|-------------|
  // |      ;      |      ; 4    |      ;      |
  // |      ;      |      ;      |      ;      |
  // |     3;      |      ;      |      ;      |
  // |~~~~~~;~~~~~~|~~~~~~;~~~~~~|~~~~~~;~~~~~~|
  // |      ;      |      ;      |      ;      |
  // |      ;      |      ;      |      ;      |
  // |      ;      |      ;      |      ;     1|
  // \-------------|-------------|-------------/
  std::vector<c8cv::KeyPoint> allKeypoints = {
    c8cv::KeyPoint(30, 10, size, angle, 2),  // 0 -> 4
    c8cv::KeyPoint(52, 64, size, angle, 4),  // 1 -> 8
    c8cv::KeyPoint(94, 95, size, angle, 1),  // 2 -> 5
    c8cv::KeyPoint(49, 38, size, angle, 5),  // 3 -> 2
    c8cv::KeyPoint(70, 51, size, angle, 8),  // 4 -> 6
    c8cv::KeyPoint(65, 63, size, angle, 9),  // 5 -> 0
    c8cv::KeyPoint(31, 29, size, angle, 7),  // 6 -> 1
    c8cv::KeyPoint(15, 78, size, angle, 3),  // 7 -> 3
    c8cv::KeyPoint(35, 33, size, angle, 6),  // 8 -> 7
  };

  // Bin -> Feature Score Map Round 0:
  // (0, 0): [ 2, 7 ]
  // (1, 1): [ 5, 6 ]
  // (2, 1): [ 8, 9 ]
  // (0, 2): [ 3 ]
  // (1, 2): [ 4 ]
  // (2, 2): [ 1 ]
  std::vector<int> IDX_SEQUENCE = {5, 6, 8, 1, 7, 2, 4, 3, 0};
  std::vector<c8cv::KeyPoint> keypoints;
  void sequenceCheck(int correctUpTo) {
    KeypointResponseMore comp;
    EXPECT_EQ(correctUpTo, keypoints.size());
    // Since nth element doesn't sort items before the n-th element
    // for comparison, we need to sort them both
    std::vector<c8cv::KeyPoint> gtKeypoints(correctUpTo);
    for (size_t i = 0; i < correctUpTo; i++) {
      gtKeypoints.push_back(allKeypoints[IDX_SEQUENCE[i]]);
    }

    std::sort(keypoints.begin(), keypoints.end(), comp);
    std::sort(gtKeypoints.begin(), gtKeypoints.end(), comp);
    for (size_t i = 0; i < correctUpTo; i++) {
      EXPECT_EQ(gtKeypoints[i].pt, keypoints[i].pt) <<
        i << "th element should be key point " << IDX_SEQUENCE[i];
    }
  }
};


TEST_F(Gr8Test, TestRetainBest) {

  C8Log("Run test", "");
  keypoints = allKeypoints;
  Gr8::retainBest(keypoints, 0, numRows, numCols);
  EXPECT_TRUE(keypoints.empty());

  // Round 1
  // 5, 6, 8, 1,
  // Round 2
  // 7, 2, 4, 3, 0
  for (int i = 1; i <= 8; i++) {
    keypoints = allKeypoints;
    Gr8::retainBest(keypoints, i, numRows, numCols);
    sequenceCheck(i);
  }

  // Then we are done; As an optimization, when returning 9 or greater, the original list is
  // returned.
  keypoints = allKeypoints;
  Gr8::retainBest(keypoints, 9, numRows, numCols);
  EXPECT_EQ(9, keypoints.size());
  EXPECT_EQ(allKeypoints[0].pt, keypoints[0].pt);
  EXPECT_EQ(allKeypoints[1].pt, keypoints[1].pt);
  EXPECT_EQ(allKeypoints[2].pt, keypoints[2].pt);
  EXPECT_EQ(allKeypoints[3].pt, keypoints[3].pt);
  EXPECT_EQ(allKeypoints[4].pt, keypoints[4].pt);
  EXPECT_EQ(allKeypoints[5].pt, keypoints[5].pt);
  EXPECT_EQ(allKeypoints[6].pt, keypoints[6].pt);
  EXPECT_EQ(allKeypoints[7].pt, keypoints[7].pt);
  EXPECT_EQ(allKeypoints[8].pt, keypoints[8].pt);

  // As an optimization, when returning 9 or greater, the original list is returned.
  keypoints = allKeypoints;
  Gr8::retainBest(keypoints, 10, numRows, numCols);
  EXPECT_EQ(9, keypoints.size());
  EXPECT_EQ(allKeypoints[0].pt, keypoints[0].pt);
  EXPECT_EQ(allKeypoints[1].pt, keypoints[1].pt);
  EXPECT_EQ(allKeypoints[2].pt, keypoints[2].pt);
  EXPECT_EQ(allKeypoints[3].pt, keypoints[3].pt);
  EXPECT_EQ(allKeypoints[4].pt, keypoints[4].pt);
  EXPECT_EQ(allKeypoints[5].pt, keypoints[5].pt);
  EXPECT_EQ(allKeypoints[6].pt, keypoints[6].pt);
  EXPECT_EQ(allKeypoints[7].pt, keypoints[7].pt);
  EXPECT_EQ(allKeypoints[8].pt, keypoints[8].pt);
}

}  // namespace c8
