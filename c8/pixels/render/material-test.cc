// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":material",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x432e66a8);

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "c8/pixels/render/material.h"

namespace c8 {

class MockMaterialBinder : public MaterialBinder {
public:
  void bind() override {}
};

class MaterialTest : public ::testing::Test {};

TEST_F(MaterialTest, TestColorOnly) {
  auto mat = MatGen::colorOnly();
  // Check basic stats.
  EXPECT_EQ(Shaders::COLOR_ONLY, mat->shader());
  EXPECT_EQ(0, mat->sceneMaterialSpecs().size());
}

TEST_F(MaterialTest, TestGetDependentSceneMaterialSpec) {
  auto mat = MatGen::subsceneMaterial("subscene1", "renderSpec1");
  EXPECT_EQ(1, mat->sceneMaterialSpecs().size());
}

TEST_F(MaterialTest, TestBinder) {
  auto mat = MatGen::colorOnly();
  // Check setBinder. Befor setBinder, the material's binder is null.
  EXPECT_EQ(nullptr, mat->binder());

  // After setBinder, the material's binder is non-null and is the same class that was set.
  mat->setBinder(std::make_unique<MockMaterialBinder>());
  EXPECT_NE(nullptr, dynamic_cast<MockMaterialBinder *>(mat->binder()));
}

}  // namespace c8
