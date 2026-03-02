// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":box3",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xccc20669);

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "c8/geometry/box3.h"

using testing::Pointwise;

namespace c8 {

MATCHER_P(AreWithin, eps, "") { return fabs(testing::get<0>(arg) - testing::get<1>(arg)) < eps; }

decltype(auto) equalsPoint(const HPoint3 &point) {
  return Pointwise(AreWithin(0.0001), point.data());
}

class Box3Test : public ::testing::Test {};

TEST_F(Box3Test, TestBox3) {
  // construction.
  auto box = Box3::from({{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}});
  auto halfBox = Box3::from({{-0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, 0.5f}});

  // operators.
  EXPECT_EQ(box, box);
  EXPECT_NE(box, halfBox);

  // merge.
  EXPECT_EQ(halfBox.merge(box), box);
  EXPECT_EQ(box.merge(halfBox), box);
  EXPECT_EQ(box.merge(box), box);
  EXPECT_EQ(halfBox.merge(halfBox), halfBox);

  // center.
  EXPECT_THAT(box.center().data(), equalsPoint({0.0f, 0.0f, 0.0f}));
  EXPECT_THAT(halfBox.center().data(), equalsPoint({0.0f, 0.0f, 0.0f}));

  // transform.
  EXPECT_EQ(halfBox.transform(HMatrixGen::scale(2.0f)), box)
    << halfBox.transform(HMatrixGen::scale(2.0f)).toString();
  EXPECT_EQ(box.transform(HMatrixGen::scale(0.5f)), halfBox)
    << box.transform(HMatrixGen::scale(0.5f)).toString();

  // dimension
  auto dim = box.dimensions();
  EXPECT_FLOAT_EQ(2.f, dim.x());
  EXPECT_FLOAT_EQ(2.f, dim.y());
  EXPECT_FLOAT_EQ(2.f, dim.z());

  auto halfDim = halfBox.dimensions();
  EXPECT_FLOAT_EQ(1.f, halfDim.x());
  EXPECT_FLOAT_EQ(1.f, halfDim.y());
  EXPECT_FLOAT_EQ(1.f, halfDim.z());
}

TEST_F(Box3Test, TestIntersects) {
  auto box = Box3::from({{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}});
  // a intersects a
  EXPECT_TRUE(box.intersects(box));
  // b inside a
  EXPECT_TRUE(box.intersects(Box3::from({{0.25f, 0.25f, 0.25f}, {0.75f, 0.75f, 0.75f}})));
  // a inside b
  EXPECT_TRUE(box.intersects(Box3::from({{-1.0f, -1.0f, -1.0f}, {2.0f, 2.0f, 2.0f}})));
  // partial intersection
  EXPECT_TRUE(box.intersects(Box3::from({{-1.0f, -1.0f, -1.0f}, {0.5f, 2.0f, 2.0f}})));
  EXPECT_TRUE(box.intersects(Box3::from({{-1.0f, -1.0f, -1.0f}, {2.0f, 0.5f, 2.0f}})));
  EXPECT_TRUE(box.intersects(Box3::from({{-1.0f, -1.0f, -1.0f}, {2.0f, 2.0f, 0.5f}})));
  EXPECT_TRUE(box.intersects(Box3::from({{-1.0f, -1.0f, -1.0f}, {0.5f, 0.5f, 2.0f}})));
  EXPECT_TRUE(box.intersects(Box3::from({{-1.0f, -1.0f, -1.0f}, {0.5f, 2.0f, 0.5f}})));
  EXPECT_TRUE(box.intersects(Box3::from({{-1.0f, -1.0f, -1.0f}, {2.0f, 0.5f, 0.5f}})));
  EXPECT_TRUE(box.intersects(Box3::from({{-1.0f, -1.0f, -1.0f}, {0.5f, 0.5f, 0.5f}})));
  EXPECT_TRUE(box.intersects(Box3::from({{-1.0f, 0.5f, -1.0f}, {0.5f, 2.0f, 2.0f}})));
  EXPECT_TRUE(box.intersects(Box3::from({{0.5f, -1.0f, -1.0f}, {2.0f, 0.5f, 2.0f}})));
  EXPECT_TRUE(box.intersects(Box3::from({{0.5f, 0.5f, -1.0f}, {2.0f, 2.0f, 0.5f}})));
  EXPECT_TRUE(box.intersects(Box3::from({{-1.0f, -1.0f, 0.5f}, {0.5f, 0.5f, 2.0f}})));
  EXPECT_TRUE(box.intersects(Box3::from({{-1.0f, 0.5f, -1.0f}, {0.5f, 2.0f, 0.5f}})));
  EXPECT_TRUE(box.intersects(Box3::from({{0.5f, -1.0f, -1.0f}, {2.0f, 0.5f, 0.5f}})));

  // Does not intersect
  EXPECT_FALSE(box.intersects(Box3::from({{-1.0f, -1.0f, 1.5f}, {0.5f, 2.0f, 2.0f}})));
  EXPECT_FALSE(box.intersects(Box3::from({{1.5f, -1.0f, -1.0f}, {2.0f, 0.5f, 2.0f}})));
  EXPECT_FALSE(box.intersects(Box3::from({{-1.0f, 1.5f, -1.0f}, {2.0f, 2.0f, 0.5f}})));
  EXPECT_FALSE(box.intersects(Box3::from({{-1.0f, -1.0f, -1.0f}, {-0.5f, 0.5f, 2.0f}})));
  EXPECT_FALSE(box.intersects(Box3::from({{-1.0f, -1.0f, -1.0f}, {0.5f, 2.0f, -0.5f}})));
  EXPECT_FALSE(box.intersects(Box3::from({{-1.0f, -1.0f, -1.0f}, {2.0f, -0.5f, 0.5f}})));
}

TEST_F(Box3Test, Make8HalfBoxes) {
  // construction.
  auto box = Box3::from({{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}});
  auto smallerBoxes = box.splitOctants();
  EXPECT_EQ(8, smallerBoxes.size());

  // new boxes have to be half as big
  auto boxDims = box.dimensions();
  for (const auto &smallerBox : smallerBoxes) {
    auto smallerBoxDims = smallerBox.dimensions();
    EXPECT_FLOAT_EQ(smallerBoxDims.x(), boxDims.x() * 0.5f);
    EXPECT_FLOAT_EQ(smallerBoxDims.y(), boxDims.y() * 0.5f);
    EXPECT_FLOAT_EQ(smallerBoxDims.z(), boxDims.z() * 0.5f);
  }

  // new boxes have to follow the convention of
  // child:  0 1 2 3 4 5 6 7
  // x:      - - - - + + + +
  // y:      - - + + - - + +
  // z:      - + - + - + - +

  // Deal with easy cases at the min and max corner first
  EXPECT_THAT(smallerBoxes[0].min.data(), equalsPoint(box.min));
  EXPECT_THAT(smallerBoxes[0].max.data(), equalsPoint(box.center()));
  EXPECT_THAT(smallerBoxes[7].min.data(), equalsPoint(box.center()));
  EXPECT_THAT(smallerBoxes[7].max.data(), equalsPoint(box.max));

  EXPECT_THAT(smallerBoxes[1].min.data(), equalsPoint({-1.f, -1.f, 0.f}));
  EXPECT_THAT(smallerBoxes[1].max.data(), equalsPoint({0.f, 0.f, 1.f}));
  EXPECT_THAT(smallerBoxes[2].min.data(), equalsPoint({-1.f, 0.f, -1.f}));
  EXPECT_THAT(smallerBoxes[2].max.data(), equalsPoint({0.f, 1.f, 0.f}));
  EXPECT_THAT(smallerBoxes[3].min.data(), equalsPoint({-1.f, 0.f, 0.f}));
  EXPECT_THAT(smallerBoxes[3].max.data(), equalsPoint({0.f, 1.f, 1.f}));
  EXPECT_THAT(smallerBoxes[4].min.data(), equalsPoint({0.f, -1.f, -1.f}));
  EXPECT_THAT(smallerBoxes[4].max.data(), equalsPoint({1.f, 0.f, 0.f}));
  EXPECT_THAT(smallerBoxes[5].min.data(), equalsPoint({0.f, -1.f, 0.f}));
  EXPECT_THAT(smallerBoxes[5].max.data(), equalsPoint({1.f, 0.f, 1.f}));
  EXPECT_THAT(smallerBoxes[6].min.data(), equalsPoint({0.f, 0.f, -1.f}));
  EXPECT_THAT(smallerBoxes[6].max.data(), equalsPoint({1.f, 1.f, 0.f}));
}

TEST_F(Box3Test, SplitOctantsOnBoxNotIsometricAtOrigin) {
  // construction.
  auto box = Box3::from({{-1.0f, -1.0f, -1.0f}, {3.0f, 5.0f, 7.0f}});
  auto smallerBoxes = box.splitOctants();
  EXPECT_EQ(8, smallerBoxes.size());

  // new boxes have to be half as big
  auto boxDims = box.dimensions();
  for (const auto &smallerBox : smallerBoxes) {
    auto smallerBoxDims = smallerBox.dimensions();
    EXPECT_FLOAT_EQ(smallerBoxDims.x(), boxDims.x() * 0.5f);
    EXPECT_FLOAT_EQ(smallerBoxDims.y(), boxDims.y() * 0.5f);
    EXPECT_FLOAT_EQ(smallerBoxDims.z(), boxDims.z() * 0.5f);
  }

  // Deal with easy cases at the min and max corner first
  EXPECT_THAT(smallerBoxes[0].min.data(), equalsPoint(box.min));
  EXPECT_THAT(smallerBoxes[0].max.data(), equalsPoint(box.center()));
  EXPECT_THAT(smallerBoxes[7].min.data(), equalsPoint(box.center()));
  EXPECT_THAT(smallerBoxes[7].max.data(), equalsPoint(box.max));

  EXPECT_THAT(smallerBoxes[1].min.data(), equalsPoint({-1.f, -1.f, 3.f}));
  EXPECT_THAT(smallerBoxes[1].max.data(), equalsPoint({1.f, 2.f, 7.f}));
  EXPECT_THAT(smallerBoxes[2].min.data(), equalsPoint({-1.f, 2.f, -1.f}));
  EXPECT_THAT(smallerBoxes[2].max.data(), equalsPoint({1.f, 5.f, 3.f}));
  EXPECT_THAT(smallerBoxes[3].min.data(), equalsPoint({-1.f, 2.f, 3.f}));
  EXPECT_THAT(smallerBoxes[3].max.data(), equalsPoint({1.f, 5.f, 7.f}));
  EXPECT_THAT(smallerBoxes[4].min.data(), equalsPoint({1.f, -1.f, -1.f}));
  EXPECT_THAT(smallerBoxes[4].max.data(), equalsPoint({3.f, 2.f, 3.f}));
  EXPECT_THAT(smallerBoxes[5].min.data(), equalsPoint({1.f, -1.f, 3.f}));
  EXPECT_THAT(smallerBoxes[5].max.data(), equalsPoint({3.f, 2.f, 7.f}));
  EXPECT_THAT(smallerBoxes[6].min.data(), equalsPoint({1.f, 2.f, -1.f}));
  EXPECT_THAT(smallerBoxes[6].max.data(), equalsPoint({3.f, 5.f, 3.f}));
}

TEST_F(Box3Test, Contains) {
  auto box = Box3::from({2.f, 3.f, -1.f}, {2.f, 3.f, 1.f});
  EXPECT_TRUE(box.contains({2.f, 3.f, -1.f}));
  EXPECT_TRUE(box.contains({2.9f, 4.4f, -0.9f}));
  EXPECT_FALSE(box.contains({3.f, 7.f, 0.f}));
  EXPECT_FALSE(box.contains({-4.5f, 4.5f, 0.f}));
  EXPECT_FALSE(box.contains({3.f, 4.5f, 0.1f}));
}

}  // namespace c8
