// Copyright (c) 2016 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":staged-ring-buffer",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xbf2d2fbc);

#include "c8/staged-ring-buffer.h"

#include <thread>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::ElementsAre;

namespace c8 {

namespace {

enum class Stage { ADD, SUBTRACT, MULTIPLY, READ };
struct Data {
  Data(int inA, int inB, int inC) : a(inA), b(inB), c(inC) {
    // Test emplace construction.
    EXPECT_EQ(a, 1);
    EXPECT_EQ(b, 2);
    EXPECT_EQ(c, 3);
  }
  int a;
  int b;
  int c;
};

}  // namespace

class StagedRingBufferTest : public ::testing::Test {};

TEST_F(StagedRingBufferTest, GetTo50TheHardWay) {
  StagedRingBuffer<Data, Stage> ring(
    6, {Stage::ADD, Stage::SUBTRACT, Stage::MULTIPLY, Stage::READ}, 1, 2, 3);

  std::thread t0([&ring] {
    int a = 0;
    for (int i = 0; i < 10; ++i) {
      auto stage = ring.getStage(Stage::ADD);
      EXPECT_TRUE(stage.hasValue());
      Data &data = stage.get();
      // Increment the last frame value by 1. Even though this is a ring-buffer, we should be
      // blissfully unaware and ADD should count from 1 to 10.
      data.a = a + 1;
      a = data.a;
    }
  });
  std::thread t1([&ring] {
    int b = 0;
    for (int i = 0; i < 10; ++i) {
      auto stage = ring.getStage(Stage::SUBTRACT);
      EXPECT_TRUE(stage.hasValue());
      Data &data = stage.get();
      // Compute A_t+1 - B_t and store in B. Should yield the following.
      // 1-0=1; 2-1=1; 3-1=2; 4-2=2; 5-2=3; 6-3=3; 7-3=4; 8-4=4; 9-4=5; 10-5=5.
      data.b = data.a - b;
      b = data.b;
    }
  });
  std::thread t2([&ring] {
    for (int i = 0; i < 10; ++i) {
      auto stage = ring.getStage(Stage::MULTIPLY);
      EXPECT_TRUE(stage.hasValue());
      Data &data = stage.get();
      // Compute A*B from and store in C.
      data.c = data.a * data.b;
    }
  });

  Vector<int> resultA;
  Vector<int> resultB;
  Vector<int> resultC;

  {
    // Collect the results in the main thread.
    for (int i = 0; i < 10; ++i) {
      auto stage = ring.getStage(Stage::READ);
      EXPECT_TRUE(stage.hasValue());
      Data &data = stage.get();
      resultA.push_back(data.a);
      resultB.push_back(data.b);
      resultC.push_back(data.c);
    }
  }

  EXPECT_THAT(resultA, ElementsAre(1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
  EXPECT_THAT(resultB, ElementsAre(1, 1, 2, 2, 3, 3, 4, 4, 5, 5));
  EXPECT_THAT(resultC, ElementsAre(1, 2, 6, 8, 15, 18, 28, 32, 45, 50));

  t0.join();
  t1.join();
  t2.join();
}

TEST_F(StagedRingBufferTest, PauseResume) {
  StagedRingBuffer<Data, Stage> ring(
    6, {Stage::ADD, Stage::SUBTRACT, Stage::MULTIPLY, Stage::READ}, 1, 2, 3);

  for (int i = 0; i < 50; ++i) {
    std::thread t0([&ring] {
      int a = 0;
      for (int i = 0; i < 10; ++i) {
        auto stage = ring.getStage(Stage::ADD);
        if (!stage.hasValue()) {
          // Break vs continue shouldn't matter here.
          break;
        }
        Data &data = stage.get();
        // Increment the last frame value by 1. Even though this is a ring-buffer, we should be
        // blissfully unaware and ADD should count from 1 to 10.
        data.a = a + 1;
        a = data.a;
      }
    });
    std::thread t1([&ring] {
      int b = 0;
      for (int i = 0; i < 10; ++i) {
        auto stage = ring.getStage(Stage::SUBTRACT);
        if (!stage.hasValue()) {
          // Break vs continue shouldn't matter here.
          continue;
        }
        Data &data = stage.get();
        // Compute A_t+1 - B_t and store in B. Should yield the following.
        // 1-0=1; 2-1=1; 3-1=2; 4-2=2; 5-2=3; 6-3=3; 7-3=4; 8-4=4; 9-4=5; 10-5=5.
        data.b = data.a - b;
        b = data.b;
      }
    });
    std::thread t2([&ring] {
      for (int i = 0; i < 10; ++i) {
        auto stage = ring.getStage(Stage::MULTIPLY);
        if (!stage.hasValue()) {
          // Break vs continue shouldn't matter here.
          break;
        }
        Data &data = stage.get();
        // Compute A*B from and store in C.
        data.c = data.a * data.b;
      }
    });

    Vector<int> resultA;
    Vector<int> resultB;
    Vector<int> resultC;

    {
      // Collect the results in the main thread.
      for (int i = 0; i < 10; ++i) {
        if (i == 5) {
          ring.pauseAndClear();
        }

        auto stage = ring.getStage(Stage::READ);
        if (!stage.hasValue()) {
          // Break vs continue shouldn't matter here.
          continue;
        }
        Data &data = stage.get();
        resultA.push_back(data.a);
        resultB.push_back(data.b);
        resultC.push_back(data.c);
      }
    }

    EXPECT_THAT(resultC, ElementsAre(1, 2, 6, 8, 15));

    t0.join();
    t1.join();
    t2.join();

    ring.resume();
  }
}

}  // namespace c8
