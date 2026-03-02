// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":fast-frame-hash",
    ":pixel-buffer",
    ":test-image",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xa7c7b78d);

#include "c8/pixels/fast-frame-hash.h"

#include <gtest/gtest.h>
#include "c8/pixels/pixel-buffer.h"
#include "c8/pixels/test-image.h"

namespace c8 {

class FastFrameHashTest : public ::testing::Test {};

TEST_F(FastFrameHashTest, TestMeSomethingGood) {
  RGBA8888PlanePixelBuffer a(320, 240);
  RGBA8888PlanePixelBuffer b(320, 240);
  RGBA8888PlanePixelBuffer c(320, 240);
  RGBA8888PlanePixelBuffer d(320, 240);
  RGBA8888PlanePixelBuffer e(320, 240);
  RGBA8888PlanePixelBuffer f(320, 240);

  fillTestImage(a.pixels(), 0);
  fillTestImage(b.pixels(), 1);
  fillTestImage(c.pixels(), 2);
  fillTestImage(d.pixels(), 3);
  fillTestImage(e.pixels(), 4);
  fillTestImage(f.pixels(), 5);

  FastFrameHashSet<5> s;
  EXPECT_EQ(0, s.checkAndAdd(a.pixels()));

  EXPECT_EQ(1, s.checkAndAdd(a.pixels()));

  EXPECT_EQ(0, s.checkAndAdd(b.pixels()));

  EXPECT_EQ(1, s.checkAndAdd(b.pixels()));
  EXPECT_EQ(2, s.checkAndAdd(a.pixels()));

  EXPECT_EQ(0, s.checkAndAdd(c.pixels()));

  EXPECT_EQ(1, s.checkAndAdd(c.pixels()));
  EXPECT_EQ(2, s.checkAndAdd(b.pixels()));
  EXPECT_EQ(3, s.checkAndAdd(a.pixels()));

  EXPECT_EQ(0, s.checkAndAdd(d.pixels()));

  EXPECT_EQ(1, s.checkAndAdd(d.pixels()));
  EXPECT_EQ(2, s.checkAndAdd(c.pixels()));
  EXPECT_EQ(3, s.checkAndAdd(b.pixels()));
  EXPECT_EQ(4, s.checkAndAdd(a.pixels()));

  EXPECT_EQ(0, s.checkAndAdd(e.pixels()));

  EXPECT_EQ(1, s.checkAndAdd(e.pixels()));
  EXPECT_EQ(2, s.checkAndAdd(d.pixels()));
  EXPECT_EQ(3, s.checkAndAdd(c.pixels()));
  EXPECT_EQ(4, s.checkAndAdd(b.pixels()));
  EXPECT_EQ(5, s.checkAndAdd(a.pixels()));

  EXPECT_EQ(0, s.checkAndAdd(f.pixels()));

  EXPECT_EQ(1, s.checkAndAdd(f.pixels()));
  EXPECT_EQ(2, s.checkAndAdd(e.pixels()));
  EXPECT_EQ(3, s.checkAndAdd(d.pixels()));
  EXPECT_EQ(4, s.checkAndAdd(c.pixels()));
  EXPECT_EQ(5, s.checkAndAdd(b.pixels()));

  EXPECT_EQ(0, s.checkAndAdd(a.pixels()));

  EXPECT_EQ(1, s.checkAndAdd(a.pixels()));
  EXPECT_EQ(2, s.checkAndAdd(f.pixels()));
  EXPECT_EQ(3, s.checkAndAdd(e.pixels()));
  EXPECT_EQ(4, s.checkAndAdd(d.pixels()));
  EXPECT_EQ(5, s.checkAndAdd(c.pixels()));

  EXPECT_EQ(0, s.checkAndAdd(b.pixels()));

  EXPECT_EQ(1, s.checkAndAdd(b.pixels()));
  EXPECT_EQ(2, s.checkAndAdd(a.pixels()));
  EXPECT_EQ(3, s.checkAndAdd(f.pixels()));
  EXPECT_EQ(4, s.checkAndAdd(e.pixels()));
  EXPECT_EQ(5, s.checkAndAdd(d.pixels()));

  EXPECT_EQ(0, s.checkAndAdd(c.pixels()));

  EXPECT_EQ(1, s.checkAndAdd(c.pixels()));
  EXPECT_EQ(2, s.checkAndAdd(b.pixels()));
  EXPECT_EQ(3, s.checkAndAdd(a.pixels()));
  EXPECT_EQ(4, s.checkAndAdd(f.pixels()));
  EXPECT_EQ(5, s.checkAndAdd(e.pixels()));

  EXPECT_EQ(0, s.checkAndAdd(d.pixels()));

  EXPECT_EQ(1, s.checkAndAdd(d.pixels()));
  EXPECT_EQ(2, s.checkAndAdd(c.pixels()));
  EXPECT_EQ(3, s.checkAndAdd(b.pixels()));
  EXPECT_EQ(4, s.checkAndAdd(a.pixels()));
  EXPECT_EQ(5, s.checkAndAdd(f.pixels()));

  EXPECT_EQ(0, s.checkAndAdd(e.pixels()));

  EXPECT_EQ(1, s.checkAndAdd(e.pixels()));
  EXPECT_EQ(2, s.checkAndAdd(d.pixels()));
  EXPECT_EQ(3, s.checkAndAdd(c.pixels()));
  EXPECT_EQ(4, s.checkAndAdd(b.pixels()));
  EXPECT_EQ(5, s.checkAndAdd(a.pixels()));

  EXPECT_EQ(0, s.checkAndAdd(f.pixels()));

  EXPECT_EQ(1, s.checkAndAdd(f.pixels()));
  EXPECT_EQ(2, s.checkAndAdd(e.pixels()));
  EXPECT_EQ(3, s.checkAndAdd(d.pixels()));
  EXPECT_EQ(4, s.checkAndAdd(c.pixels()));
  EXPECT_EQ(5, s.checkAndAdd(b.pixels()));
}

}  // namespace c8
