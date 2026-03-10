// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Paris Morgan (paris@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":statistics",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xd8c848e3);

#include <cmath>

#include "c8/statistics.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using testing::Eq;
using testing::Pointwise;

namespace c8 {

class StatisticsTest : public ::testing::Test {};

MATCHER_P(AreWithin, eps, "") { return fabs(testing::get<0>(arg) - testing::get<1>(arg)) < eps; }
decltype(auto) equalsVector(const Vector<float> &v, float threshold = 1e-5f) {
  return Pointwise(AreWithin(threshold), v);
}

TEST_F(StatisticsTest, MeanStdDev) {
  // Share objects.
  Vector<float> v;
  float m;
  float sd;

  // Empty array.
  m = mean(v);
  sd = stdDev(v, m);
  EXPECT_FLOAT_EQ(m, 0.f);
  EXPECT_FLOAT_EQ(sd, 0.f);

  // Single element array.
  v = {99.f};
  m = mean(v);
  sd = stdDev(v, m);
  EXPECT_FLOAT_EQ(m, 99.f);
  EXPECT_FLOAT_EQ(sd, 0.f);

  // Odd number of elements in array.
  v = {1.f, 2.f, 3.f, 4.f, 5.f};
  m = mean(v);
  sd = stdDev(v, m);
  EXPECT_FLOAT_EQ(m, 3.f);
  EXPECT_FLOAT_EQ(sd, std::sqrt(10.f / 5));

  // Even number of elements in array.
  v = {1.f, 2.f, 4.f, 5.f};
  m = mean(v);
  sd = stdDev(v, m);
  EXPECT_FLOAT_EQ(m, 3.f);
  EXPECT_FLOAT_EQ(sd, std::sqrt(10.f / 4));

  // Array with all of the same value.
  v = {2.f, 2.f, 2.f, 2.f};
  m = mean(v);
  sd = stdDev(v, m);
  EXPECT_FLOAT_EQ(m, 2.f);
  EXPECT_FLOAT_EQ(sd, 0.f);
}

TEST_F(StatisticsTest, CrossCorrelateEdgeCases) {
  auto x = {1.f, 2.f, 3.f, 4.f, 5.f};
  auto y = {1.f, 2.f, 3.f, 4.f, 5.f};
  EXPECT_THAT(crossCorrelateFull({}, y), equalsVector({}));
  EXPECT_THAT(crossCorrelateFull(x, {}), equalsVector({}));
}

TEST_F(StatisticsTest, CrossCorrelate) {
  // Based on:
  // from scipy import signal
  // x = [1., 2., 3., 4., 5.]
  // y = [1., 2., 3., 4., 5.]
  // z = signal.correlate(x, y, mode='full', method='direct')
  // print("x = {" + "f, ".join(str(v) for v in x) + "f};")
  // print("y = {" + "f, ".join(str(v) for v in y) + "f};")
  // print("vals = {" + "f, ".join(str(v) for v in z) + "f};")
  // print("EXPECT_THAT(crossCorrelateFull(x, y), equalsVector(vals));")

  Vector<float> x = {1.f, 2.f, 3.f, 4.f, 5.f};
  Vector<float> y = {1.f, 2.f, 3.f, 4.f, 5.f};
  Vector<float> vals = {5.f, 14.f, 26.f, 40.f, 55.f, 40.f, 26.f, 14.f, 5.f};
  EXPECT_THAT(crossCorrelateFull(x, y), equalsVector(vals));

  x = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
  y = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f};
  // clang-format off
  vals = {10.0f, 29.0f, 56.0f, 90.0f, 130.0f, 115.0f, 100.0f, 85.0f, 70.0f, 55.0f, 40.0f, 26.0f, 14.0f, 5.0f};
  // clang-format on
  EXPECT_THAT(crossCorrelateFull(x, y), equalsVector(vals));

  x = {1.0f};
  y = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f};
  vals = {10.0f, 9.0f, 8.0f, 7.0f, 6.0f, 5.0f, 4.0f, 3.0f, 2.0f, 1.0f};
  EXPECT_THAT(crossCorrelateFull(x, y), equalsVector(vals));

  x = {-10.1f, -9.2f, -8.3f, 100.0f, 1000.0f, 0.0f, 16.0f};
  y = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f};
  // clang-format off
  vals = {-101.0f, -182.89999999999998f, -246.6f, 781.0f, 10708.6f, 9636.2f, 8723.8f, 7635.4f, 6547.0f, 5458.6f, 4370.2f, 3271.7f, 2164.0f, 1048.0f, 32.0f, 16.0f};
  // clang-format on
  EXPECT_THAT(crossCorrelateFull(x, y), equalsVector(vals));

  x = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
  y = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f};
  vals = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
  EXPECT_THAT(crossCorrelateFull(x, y), equalsVector(vals));

  x = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
  y = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
  vals = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
  EXPECT_THAT(crossCorrelateFull(x, y), equalsVector(vals));

  x = {1.0f};
  y = {-1.0f};
  vals = {-1.0f};
  EXPECT_THAT(crossCorrelateFull(x, y), equalsVector(vals));
}

}  // namespace c8
