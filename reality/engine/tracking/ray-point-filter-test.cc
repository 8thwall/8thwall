// Copyright (c) 2023 Niantic, Inc.
// Original Author: Nathan Waters (nathan@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":ray-point-filter",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x9ef962d5);

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "c8/hpoint.h"
#include "reality/engine/tracking/ray-point-filter.h"

using testing::Pointwise;

MATCHER_P(AreWithin, eps, "") { return fabs(testing::get<0>(arg) - testing::get<1>(arg)) < eps; }

namespace c8 {
constexpr bool PRINT_PTS = false;

decltype(auto) equalsPoint(const HPoint3 &pt, float threshold = 1e-6) {
  return Pointwise(AreWithin(threshold), pt.data());
}

decltype(auto) equalsPoint(const HPoint2 &pt, float threshold = 1e-6) {
  return Pointwise(AreWithin(threshold), pt.data());
}

class RayPointFilter3Test : public ::testing::Test {
protected:
  void SetUp() override {
    prevPoint_ = {0.f, 0.f, 0.f};
    newPoint_ = {0.001f, 0.001f, 0.001f};
    config_ = createRayPointFilterConfig();
  }

  HPoint3 prevPoint_;
  HPoint3 newPoint_;

  RayPointFilterConfig config_;
};

TEST_F(RayPointFilter3Test, TestDefault) {
  RayPointFilter3 filter(prevPoint_, config_);
  auto filteredPoint = filter.filter(newPoint_);
  EXPECT_THAT(filteredPoint.data(), equalsPoint(HPoint3{0.000450305f, 0.000450305f, 0.000450305f}));
}

TEST_F(RayPointFilter3Test, TestVAlpha) {
  // By decreasing vAlpha, we react less to motion.
  config_ = createRayPointFilterConfig(0.01f, 0.02f, 0.50f);
  RayPointFilter3 filter(prevPoint_, config_);
  auto filteredPoint = filter.filter(newPoint_);
  EXPECT_THAT(filteredPoint.data(), equalsPoint({0.000314879f, 0.000314879f, 0.000314879f}));
}

TEST_F(RayPointFilter3Test, TestUpdate90V) {
  // By increasing update90v, we react to low motion less.
  config_ = createRayPointFilterConfig(0.01f, 0.5f, 0.90f);
  RayPointFilter3 filter(prevPoint_, config_);
  auto filteredPoint = filter.filter(newPoint_);
  EXPECT_THAT(filteredPoint.data(), equalsPoint({0.0000407348f, 0.0000407348f, 0.0000407348f}));
}

TEST_F(RayPointFilter3Test, TestMinAlpha) {
  // By decreasing minAlpha, we react less in low motion
  config_ = createRayPointFilterConfig(0.00001f, 0.02f, 0.90f);
  RayPointFilter3 filter(prevPoint_, config_);
  auto filteredPoint = filter.filter(newPoint_);
  EXPECT_THAT(filteredPoint.data(), equalsPoint({0.000447517f, 0.000447517f, 0.000447517f}));
}

TEST_F(RayPointFilter3Test, TestDefault3a) {
  HPoint3 prevPoint{0.01f, 0.0f, 0.09f};
  auto config = createRayPointFilterConfig(0.005, 0.02f, 0.9f);
  RayPointFilter3a filter(prevPoint, config);

  // x does not change, y has change higher than 90v (0.9), z has small jitter in the range (0.005)
  Vector<HPoint3> pts = {
    {0.01f, 0.9f, 0.0912f},
    {0.01f, 0.1f, 0.0885f},
    {0.01f, -0.9f, 0.0895f},
    {0.01f, 1.0f, 0.0902f},
    {0.01f, 1.7f, 0.0890f},
    {0.01f, 0.8f, 0.0910f},
    {0.01f, -0.1f, 0.0905f},
    {0.01f, 0.7f, 0.0889f},
    {0.01f, 1.5f, 0.0892f},
    {0.01f, 0.2f, 0.0895f},
  };

  Vector<HPoint3> filteredPts;
  for (const auto &pt3 : pts) {
    filteredPts.push_back(filter.filter(pt3));

    if (PRINT_PTS) {
      std::cout << "pt " << pt3.x() << ", " << pt3.y() << ", " << pt3.z() << " filtered "
                << filteredPts.back().x() << ", " << filteredPts.back().y() << ", "
                << filteredPts.back().z() << std::endl;
    }
  }

  EXPECT_EQ(10, filteredPts.size());
  for (size_t i = 0; i < filteredPts.size(); i++) {
    // x does not change after filtering
    EXPECT_FLOAT_EQ(filteredPts.at(i).x(), 0.01f) << i;

    // y and z are not completely off
    EXPECT_NEAR(filteredPts.at(i).y(), pts.at(i).y(), 0.01f) << i;
    EXPECT_NEAR(filteredPts.at(i).z(), pts.at(i).z(), 0.01f) << i;
  }

  // Y motion is dampened less than Z motion
  float totalYDiff = 0.f;
  float totalZDiff = 0.f;
  for (size_t i = 1; i < filteredPts.size(); i++) {
    auto filteredDiffY = filteredPts.at(i).y() - filteredPts.at(i - 1).y();
    totalYDiff += std::abs(filteredDiffY);

    auto filteredDiffZ = filteredPts.at(i).z() - filteredPts.at(i - 1).z();
    totalZDiff += std::abs(filteredDiffZ);
  }
  EXPECT_GT(totalYDiff, totalZDiff);
}

class RayPointFilter2Test : public ::testing::Test {
protected:
  void SetUp() override {
    prevPoint_ = {0.f, 0.f};
    newPoint_ = {0.001f, 0.001f};
  }

  HPoint2 prevPoint_;
  HPoint2 newPoint_;

  RayPointFilterConfig config_;
};

TEST_F(RayPointFilter2Test, TestDefault) {
  RayPointFilter2 filter(prevPoint_);
  auto filteredPoint = filter.filter(newPoint_);
  EXPECT_THAT(filteredPoint.data(), equalsPoint(HPoint2{0.000450305f, 0.000450305f}));
}

TEST_F(RayPointFilter2Test, TestVAlpha) {
  // By decreasing vAlpha, we react less to motion.
  config_ = createRayPointFilterConfig(0.01f, 0.02f, 0.50f);
  RayPointFilter2 filter(prevPoint_, config_);
  auto filteredPoint = filter.filter(newPoint_);
  EXPECT_THAT(filteredPoint.data(), equalsPoint({0.000314879f, 0.000314879f}));
}

TEST_F(RayPointFilter2Test, TestUpdate90V) {
  // By increasing update90v, we react to low motion less.
  config_ = createRayPointFilterConfig(0.01f, 0.5f, 0.90f);
  RayPointFilter2 filter(prevPoint_, config_);
  auto filteredPoint = filter.filter(newPoint_);
  EXPECT_THAT(filteredPoint.data(), equalsPoint({0.0000407348f, 0.0000407348f}));
}

TEST_F(RayPointFilter2Test, TestMinAlpha) {
  // By decreasing minAlpha, we react less in low motion
  config_ = createRayPointFilterConfig(0.00001f, 0.02f, 0.90f);
  RayPointFilter2 filter(prevPoint_, config_);
  auto filteredPoint = filter.filter(newPoint_);
  EXPECT_THAT(filteredPoint.data(), equalsPoint({0.000447517f, 0.000447517f}));
}

class RayPointFilter1Test : public ::testing::Test {
protected:
  void SetUp() override {
    prevVal_ = 0.f;
    // Previously, we use vectors like {0.001, 0.001}. This is a vector of length ~ 0.00141.
    // By using this value, we can make sure that we're getting roughly the same values as the
    // RayPointFilter2 and RayPointFilter3 tests.
    newVal_ = 0.0014142135623f;
  }

  float prevVal_;
  float newVal_;

  RayPointFilterConfig config_;
};

TEST_F(RayPointFilter1Test, TestDefault) {
  RayPointFilter1 filter(prevVal_);
  auto filteredVal = filter.filter(newVal_);
  EXPECT_FLOAT_EQ(filteredVal, 0.00052039442f);
}

TEST_F(RayPointFilter1Test, TestVAlpha) {
  // By decreasing vAlpha, we react less to motion.
  config_ = createRayPointFilterConfig(0.01f, 0.02f, 0.50f);
  RayPointFilter1 filter(prevVal_, config_);
  auto filteredVal = filter.filter(newVal_);
  EXPECT_FLOAT_EQ(filteredVal, 0.00034924707f);
}

TEST_F(RayPointFilter1Test, TestUpdate90V) {
  // By increasing update90v, we react to low motion less.
  config_ = createRayPointFilterConfig(0.01f, 0.5f, 0.90f);
  RayPointFilter1 filter(prevVal_, config_);
  auto filteredVal = filter.filter(newVal_);
  EXPECT_FLOAT_EQ(filteredVal, 0.000045159009f);
}

TEST_F(RayPointFilter1Test, TestMinAlpha) {
  // By decreasing minAlpha, we react less in low motion
  config_ = createRayPointFilterConfig(0.00001f, 0.02f, 0.90f);
  RayPointFilter1 filter(prevVal_, config_);
  auto filteredVal = filter.filter(newVal_);
  EXPECT_FLOAT_EQ(filteredVal, 0.00051502453f);
}

}  // namespace c8
