// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":two-d",
    "@com_google_googletest//:gtest_main",
    "//c8:random-numbers",
  };
}
cc_end(0xcf0ff562);

#include <algorithm>
#include <cstdlib>

#include "c8/geometry/two-d.h"
#include "c8/random-numbers.h"
#include "gtest/gtest.h"

namespace c8 {

class TwoDTest : public ::testing::Test {};

TEST_F(TwoDTest, TestConvexHull) {
  HPoint2 ll(0.0f, 0.0f);
  HPoint2 lr(1.0f, 0.0f);
  HPoint2 ur(1.0f, 1.0f);
  HPoint2 ul(0.0f, 1.0f);

  Vector<HPoint2> pts{ll, lr, ur, ul};

  // TODO(nb): This test fails for i = 100000 and i = 5000; why?
  // NOTE(paris): With --config=jsrun we also fail with 10000 and many values in the 9000's; why?
  for (int i = 0; i < 3000; ++i) {
    int64_t r1 = std::rand() % 1024;
    int64_t r2 = std::rand() % 1024;
    float x = (r1 + 1) / 1025.0f;
    float y = (r2 + 1) / 1025.0f;
    pts.push_back(HPoint2(x, y));
  }

  RandomNumbers rng;
  rng.shuffle(pts.begin(), pts.end());

  Poly2 poly = Poly2::convexHull(pts);

  auto lines = poly.poly();

  EXPECT_EQ(4, lines.size());

  EXPECT_EQ(ul.x(), lines[0].start().x());
  EXPECT_EQ(ul.y(), lines[0].start().y());
  EXPECT_EQ(ur.x(), lines[0].end().x());
  EXPECT_EQ(ur.y(), lines[0].end().y());

  EXPECT_EQ(ur.x(), lines[1].start().x());
  EXPECT_EQ(ur.y(), lines[1].start().y());
  EXPECT_EQ(lr.x(), lines[1].end().x());
  EXPECT_EQ(lr.y(), lines[1].end().y());

  EXPECT_EQ(lr.x(), lines[2].start().x());
  EXPECT_EQ(lr.y(), lines[2].start().y());
  EXPECT_EQ(ll.x(), lines[2].end().x());
  EXPECT_EQ(ll.y(), lines[2].end().y());

  EXPECT_EQ(ll.x(), lines[3].start().x());
  EXPECT_EQ(ll.y(), lines[3].start().y());
  EXPECT_EQ(ul.x(), lines[3].end().x());
  EXPECT_EQ(ul.y(), lines[3].end().y());
}

TEST_F(TwoDTest, TestLine2LinearProjection) {
  HPoint2 p1{1.0f, 1.0f};
  HPoint2 p2{2.0f, 2.0f};
  Line2 line(p1, p2);

  // point on the line
  HPoint2 pOnLine{1.75f, 1.75f};
  float a = line.linearProjection(pOnLine);
  EXPECT_FLOAT_EQ(a, 0.75f);

  // point on right side
  HPoint2 pRight{2.0f, 1.0f};
  a = line.linearProjection(pRight);
  EXPECT_FLOAT_EQ(a, 0.5f);

  // point on left side
  HPoint2 pLeft{0.0f, 1.0f};
  a = line.linearProjection(pLeft);
  EXPECT_FLOAT_EQ(a, -0.5f);
}

}  // namespace c8
