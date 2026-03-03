// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Paris Morgan (paris@8thwall.com)
//
// Unit tests for ParameterData.

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":string",
    ":parameter-data",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xd786f6d9);

#include <gtest/gtest.h>

#include "c8/parameter-data.h"
#include "c8/string.h"

namespace c8 {

class ParameterDataTest : public ::testing::Test {};

TEST_F(ParameterDataTest, SetGetTest) {
  ParameterData pd;
  // Test setting values.
  EXPECT_EQ(pd.version(), 0);
  pd.set<bool>("bool", true);
  EXPECT_EQ(pd.version(), 1);
  pd.set<int>("int", 5);
  EXPECT_EQ(pd.version(), 2);
  pd.set<int64_t>("int64_t", std::numeric_limits<int64_t>::max());
  pd.set<uint32_t>("uint32_t", std::numeric_limits<uint32_t>::max());
  pd.set<uint64_t>("uint64_t", std::numeric_limits<uint64_t>::max());
  pd.set<float>("float", 3.14f);
  pd.set<double>("double", 66.1);
  pd.set<String>("string", "set-string");
  auto numSets = 8;
  EXPECT_EQ(pd.version(), numSets);

  // Test getting values.
  EXPECT_EQ(pd.get<bool>("bool"), true);
  EXPECT_EQ(pd.get<int>("int"), 5);
  EXPECT_EQ(pd.get<int64_t>("int64_t"), std::numeric_limits<int64_t>::max());
  EXPECT_EQ(pd.get<uint32_t>("uint32_t"), std::numeric_limits<uint32_t>::max());
  EXPECT_EQ(pd.get<uint64_t>("uint64_t"), std::numeric_limits<uint64_t>::max());
  EXPECT_FLOAT_EQ(pd.get<float>("float"), 3.14f);
  EXPECT_DOUBLE_EQ(pd.get<double>("double"), 66.1);
  EXPECT_STREQ(pd.get<String>("string").c_str(), "set-string");
  EXPECT_EQ(pd.version(), numSets) << "get() should not increase version number.";

  // Test getOrSet getting the correct value.
  EXPECT_EQ(pd.getOrSet<bool>("bool", false), true);
  EXPECT_EQ(pd.getOrSet<int>("int", 1), 5);
  EXPECT_EQ(pd.getOrSet<int64_t>("int64_t", 1), std::numeric_limits<int64_t>::max());
  EXPECT_EQ(pd.getOrSet<uint32_t>("uint32_t", 1), std::numeric_limits<uint32_t>::max());
  EXPECT_EQ(pd.getOrSet<uint64_t>("uint64_t", 1), std::numeric_limits<uint64_t>::max());
  EXPECT_FLOAT_EQ(pd.getOrSet<float>("float", 1.f), 3.14f);
  EXPECT_DOUBLE_EQ(pd.getOrSet<double>("double", 1.0), 66.1);
  EXPECT_STREQ(pd.getOrSet<String>("string", "unused").c_str(), "set-string");
  EXPECT_EQ(pd.version(), numSets) << "getOrSet() should not increase version number.";

  // Test getOrSet setting the correct value.
  // You can let the template deduction does it work
  EXPECT_EQ(pd.getOrSet("bool2", false), false);
  EXPECT_EQ(pd.getOrSet("int2", 1), 1);
  EXPECT_EQ(pd.getOrSet<int64_t>("int64_t2", 2), 2);
  EXPECT_EQ(pd.getOrSet<uint32_t>("uint32_t2", 3), 3);
  EXPECT_EQ(pd.getOrSet<uint64_t>("uint64_t2", 4), 4);
  EXPECT_FLOAT_EQ(pd.getOrSet("float2", 1.f), 1.f);
  EXPECT_DOUBLE_EQ(pd.getOrSet("double2", 2.0), 2.0);
  String usedStr{"used"};
  EXPECT_EQ(pd.getOrSet("string2", usedStr), usedStr);
  EXPECT_EQ(pd.version(), numSets);

  // Test getOrSet can then retrieve the correct value it just set.
  EXPECT_EQ(pd.getOrSet("bool2", true), false);
  EXPECT_EQ(pd.getOrSet<int>("int2", 100), 1);
  EXPECT_EQ(pd.getOrSet<int64_t>("int64_t2", 100), 2);
  EXPECT_EQ(pd.getOrSet<uint32_t>("uint32_t2", 100), 3);
  EXPECT_EQ(pd.getOrSet<uint64_t>("uint64_t2", 100), 4);
  EXPECT_FLOAT_EQ(pd.getOrSet<float>("float2", 100.f), 1.f);
  EXPECT_DOUBLE_EQ(pd.getOrSet<double>("double2", 100.0), 2.0);
  EXPECT_STREQ(pd.getOrSet<String>("string2", "unused").c_str(), "used");
  EXPECT_EQ(pd.version(), numSets);

  // Test throwing when the value doesn't exist.
  EXPECT_THROW(pd.get<bool>("fake1"), std::exception);
  EXPECT_THROW(pd.get<int>("fake2"), std::exception);
  EXPECT_THROW(pd.get<int64_t>("fake3"), std::exception);
  EXPECT_THROW(pd.get<uint32_t>("fake4"), std::exception);
  EXPECT_THROW(pd.get<uint64_t>("fake5"), std::exception);
  EXPECT_THROW(pd.get<float>("fake6"), std::exception);
  EXPECT_THROW(pd.get<double>("fake7"), std::exception);
  EXPECT_THROW(pd.get<String>("fake8"), std::exception);
  EXPECT_EQ(pd.version(), numSets);

  // Test throwing when the value is of the wrong type.
  EXPECT_THROW(pd.get<String>("bool"), std::exception);
  EXPECT_THROW(pd.get<String>("int"), std::exception);
  EXPECT_THROW(pd.get<String>("int64_t"), std::exception);
  EXPECT_THROW(pd.get<String>("uint32_t"), std::exception);
  EXPECT_THROW(pd.get<String>("uint64_t"), std::exception);
  EXPECT_THROW(pd.get<String>("float"), std::exception);
  EXPECT_THROW(pd.get<String>("double"), std::exception);

  EXPECT_THROW(pd.get<bool>("string"), std::exception);
  EXPECT_THROW(pd.get<int>("string"), std::exception);
  EXPECT_THROW(pd.get<int64_t>("string"), std::exception);
  EXPECT_THROW(pd.get<uint32_t>("string"), std::exception);
  EXPECT_THROW(pd.get<uint64_t>("string"), std::exception);
  EXPECT_THROW(pd.get<float>("string"), std::exception);
  EXPECT_THROW(pd.get<double>("string"), std::exception);
  EXPECT_EQ(pd.version(), numSets);
}

TEST_F(ParameterDataTest, ContainsAndDeleteKeyTest) {
  EXPECT_FALSE(globalParams().contains("someKey.keyToBeDeleted"));
  globalParams().set<int>("someKey.keyToBeDeleted", 1);
  EXPECT_TRUE(globalParams().contains("someKey.keyToBeDeleted"));
  globalParams().deleteKey("someKey.keyToBeDeleted");
  EXPECT_FALSE(globalParams().contains("someKey.keyToBeDeleted"));
}

TEST_F(ParameterDataTest, MergeJsonTest) {
  ParameterData pd;
  EXPECT_EQ(pd.version(), 0);
  pd.set<bool>("bool", true);
  pd.set<int>("int", 1);
  pd.set<double>("double", 3.3);
  pd.set<String>("string", "old-string");
  auto jsonString = pd.toJsonString();
  EXPECT_EQ(pd.version(), 4);

  ParameterData pd2;
  EXPECT_EQ(pd2.version(), 0);
  pd2.set<int>("int", 99);
  pd2.set<int>("int2", 2);
  pd2.set<String>("string", "new-string");
  pd2.set<String>("string2", "new-string");
  EXPECT_EQ(pd2.version(), 4);
  pd2.mergeJsonString(jsonString);
  EXPECT_EQ(pd2.version(), 4) << "mergeJsonString() should not increase version number.";

  // The original maintains its value during mergeJson() conflicts - i.e. "int" and "string".
  EXPECT_FLOAT_EQ(pd2.get<bool>("bool"), true);
  EXPECT_EQ(pd2.get<int>("int"), 1);
  EXPECT_EQ(pd2.get<int>("int2"), 2);
  EXPECT_DOUBLE_EQ(pd2.get<double>("double"), 3.3);
  EXPECT_STREQ(pd2.get<String>("string").c_str(), "old-string");
  EXPECT_STREQ(pd2.get<String>("string2").c_str(), "new-string");
  EXPECT_EQ(pd2.version(), 4);
}

TEST_F(ParameterDataTest, ScopedParameterChangeLocalPd) {
  constexpr const char *KEY = "someKey.someItem";
  ParameterData pd;
  pd.set<int>(KEY, 4);
  {
    EXPECT_EQ(4, pd.get<int>(KEY));
    ScopedParameterUpdate update(&pd, KEY, 5);
    EXPECT_EQ(5, pd.get<int>(KEY));
  }
  EXPECT_EQ(4, pd.get<int>(KEY));
}

TEST_F(ParameterDataTest, ScopedParameterChangeGlobalPd) {
  constexpr const char *KEY = "someKey.someItem2";
  globalParams().set<int>(KEY, 6);
  {
    EXPECT_EQ(6, globalParams().get<int>(KEY));
    ScopedParameterUpdate update(KEY, 5);
    EXPECT_EQ(5, globalParams().get<int>(KEY));
  }
  EXPECT_EQ(6, globalParams().get<int>(KEY));
}

TEST_F(ParameterDataTest, ScopedParameterUpdates) {
  constexpr const char *KEY = "someKey.someItem3";
  globalParams().set<int>(KEY, 7);
  {
    EXPECT_EQ(7, globalParams().get<int>(KEY));
    ScopedParameterUpdates updates{
      {KEY, 8},
      {KEY, 9},
      {KEY, 10},
    };
    EXPECT_EQ(10, globalParams().get<int>(KEY));
    {
      ScopedParameterUpdates updates2{
        {KEY, 11},
        {KEY, 12},
      };
      updates2.set(KEY, 13);
      EXPECT_EQ(13, globalParams().get<int>(KEY));
    }
    EXPECT_EQ(10, globalParams().get<int>(KEY));
  }
  EXPECT_EQ(7, globalParams().get<int>(KEY));
}

TEST_F(ParameterDataTest, ScopedParameterUpdatesJson) {
  constexpr const char *KEY = "someKey.someItem4";
  globalParams().set<int>(KEY, 14);
  {
    EXPECT_EQ(14, globalParams().get<int>(KEY));
    ScopedParameterUpdates updates{
      {KEY, 15},
      {KEY, 16},
      {KEY, 17},
    };
    EXPECT_EQ(17, globalParams().get<int>(KEY));
    {
      ScopedParameterUpdates updates2{
        {KEY, 18},
        {KEY, 19},
      };
      updates2.mergeJsonString("{\"someKey.someItem4\": 20}");
      EXPECT_EQ(20, globalParams().get<int>(KEY));
    }
    EXPECT_EQ(17, globalParams().get<int>(KEY));
  }
}

TEST_F(ParameterDataTest, ScopedParameterUpdateNewKey) {
  constexpr const char *KEY = "someKey.keyWithoutPreviousValue";
  EXPECT_FALSE(globalParams().contains(KEY));
  {
    ScopedParameterUpdates updates;
    updates.set(KEY, 21);
    EXPECT_EQ(21, globalParams().get<int>(KEY));
  }
  EXPECT_FALSE(globalParams().contains(KEY));
}

TEST_F(ParameterDataTest, ScopedParameterUpdateDeletedKey) {
  constexpr const char *KEY = "someKey.keyToBeDeleted2";
  EXPECT_FALSE(globalParams().contains(KEY));
  {
    ScopedParameterUpdates updates;
    updates.set(KEY, 23);
    // Avoid setting globalParams and ScopedParameterUpdates in same scope, but it shouldn't crash
    globalParams().deleteKey(KEY);
    EXPECT_FALSE(globalParams().contains(KEY));
  }
  EXPECT_FALSE(globalParams().contains(KEY));
}

}  // namespace c8
