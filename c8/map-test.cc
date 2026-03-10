// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Pooja Bansal (pooja@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":map", "//bzl/inliner:rules",
    "@com_google_googletest//:gtest_main",
    "//c8:string",
  };
}
cc_end(0x5aa1e57d);

#include "c8/map.h"
#include "c8/string.h"
#include "gmock/gmock.h"

#include "gtest/gtest.h"

using ::testing::ContainerEq;

namespace c8 {

class MapTest : public ::testing::Test {};

TEST_F(MapTest, CreateAndDestroyMap) {
  HashMap<int, String> hashMap({{1, "8th"}, {2, "Wall"}});
  EXPECT_THAT(hashMap, ContainerEq(HashMap<int, String>({{1, "8th"}, {2, "Wall"}})));
}

TEST_F(MapTest, CreateAndDestroyTreeMap) {
  TreeMap<int, String> treeMap({{1, "8th"}, {2, "Wall"}});
  EXPECT_THAT(treeMap, ContainerEq(TreeMap<int, String>({{1, "8th"}, {2, "Wall"}})));
}

TEST_F(MapTest, CreateAndDestroyHashMultiMap) {
  HashMultiMap<int, String> hashMultiMap({{1, "8th"}, {1, "Wall"}});
  EXPECT_THAT(hashMultiMap, ContainerEq(HashMultiMap<int, String>({{1, "8th"}, {1, "Wall"}})));
}

TEST_F(MapTest, CreateAndDestroyTreeMultiMap) {
  TreeMultiMap<int, String> treeMultiMap({{1, "8th"}, {1, "Wall"}});
  EXPECT_THAT(treeMultiMap, ContainerEq(TreeMultiMap<int, String>({{1, "8th"}, {1, "Wall"}})));
}

}  // namespace c8
