// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":capnp-messages",
    ":io-test.capnp-cc",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x6e8f52f7);

#include "c8/io/capnp-messages.h"

#include <gtest/gtest.h>
#include "c8/io/io-test.capnp.h"

using MutableTestStruct = c8::MutableRootMessage<c8::TestStruct>;
using ConstTestStruct = c8::ConstRootMessage<c8::TestStruct>;
using ConstTestStructView = c8::ConstRootMessageView<c8::TestStruct>;

namespace c8 {

class CapnpMessagesTest : public ::testing::Test {};

TEST_F(CapnpMessagesTest, TestDefault) {
  // Default values for Mutable
  MutableTestStruct src;
  EXPECT_STREQ(src.reader().getName().cStr(), "");
  EXPECT_EQ(src.reader().getInt(), 0);
  EXPECT_EQ(src.reader().getFloat(), 0.0f);

  // Default values for Const.
  ConstTestStruct dest;
  EXPECT_STREQ(dest.reader().getName().cStr(), "");
  EXPECT_EQ(dest.reader().getInt(), 0);
  EXPECT_EQ(dest.reader().getFloat(), 0.0f);
}

TEST_F(CapnpMessagesTest, TestMove) {
  MutableTestStruct src;
  src.builder().setName("name");
  src.builder().setInt(1492);
  src.builder().setFloat(3.14159f);

  // Test move from a builder.
  ConstTestStruct dest;
  dest = ConstTestStruct(src);
  EXPECT_STREQ(dest.reader().getName().cStr(), "name");
  EXPECT_EQ(dest.reader().getInt(), 1492);
  EXPECT_EQ(dest.reader().getFloat(), 3.14159f);

  // Test move from an array.
  ConstTestStruct intermediate(src);
  auto arr = intermediate.bytes();
  dest = ConstTestStruct(static_cast<const void *>(arr.begin()), arr.size());
  EXPECT_STREQ(dest.reader().getName().cStr(), "name");
  EXPECT_EQ(dest.reader().getInt(), 1492);
  EXPECT_EQ(dest.reader().getFloat(), 3.14159f);

  MutableTestStruct mdest = std::move(src);
  EXPECT_STREQ(mdest.reader().getName().cStr(), "name");
  EXPECT_EQ(mdest.reader().getInt(), 1492);
  EXPECT_EQ(mdest.reader().getFloat(), 3.14159f);
}

TEST_F(CapnpMessagesTest, TestDirect) {
  MutableTestStruct src;
  src.builder().setName("name");
  src.builder().setInt(1492);
  src.builder().setFloat(3.14159f);

  // Serialize the mutable struct to a readonly flat array backed copy.
  ConstTestStruct dest(src);

  EXPECT_STREQ(dest.reader().getName().cStr(), "name");
  EXPECT_EQ(dest.reader().getInt(), 1492);
  EXPECT_EQ(dest.reader().getFloat(), 3.14159f);
}

TEST_F(CapnpMessagesTest, TestIndirect) {
  MutableTestStruct src;
  src.builder().setName("name");
  src.builder().setInt(1492);
  src.builder().setFloat(3.14159f);

  // Serialize the mutable struct to a readonly flat array backed copy.
  ConstTestStruct intermediate(src);
  auto arr = intermediate.bytes();

  // Deserialize the flat array into a new readonly struct.
  ConstTestStruct dest(static_cast<const void *>(arr.begin()), arr.size());

  EXPECT_STREQ(dest.reader().getName().cStr(), "name");
  EXPECT_EQ(dest.reader().getInt(), 1492);
  EXPECT_EQ(dest.reader().getFloat(), 3.14159f);
}

TEST_F(CapnpMessagesTest, TestMakeMutable) {
  MutableTestStruct src;
  src.builder().setName("name");
  src.builder().setInt(1492);
  src.builder().setFloat(3.14159f);

  // Initialize a second builder as a copy of a reader.
  MutableTestStruct dest(src.reader());
  EXPECT_STREQ(dest.reader().getName().cStr(), "name");
  EXPECT_EQ(dest.reader().getInt(), 1492);
  EXPECT_EQ(dest.reader().getFloat(), 3.14159f);
}

TEST_F(CapnpMessagesTest, TestFreezeReader) {
  MutableTestStruct src;
  src.builder().setName("name");
  src.builder().setInt(1492);
  src.builder().setFloat(3.14159f);

  // Initialize a const as a copy of a reader.
  ConstTestStruct dest(src.reader());
  EXPECT_STREQ(dest.reader().getName().cStr(), "name");
  EXPECT_EQ(dest.reader().getInt(), 1492);
  EXPECT_EQ(dest.reader().getFloat(), 3.14159f);
}

TEST_F(CapnpMessagesTest, TestCopyDisownedInnerStruct) {
  constexpr int NUM_ITEMS = 500000;
  MutableRootMessage<InnerStruct> inner;
  inner.builder().initData(NUM_ITEMS);
  inner.builder().initListStruct(NUM_ITEMS);
  int sizeWithDataAndList = 0;
  int sizeWithNoData = 0;
  int sizeWithNothing = 0;
  {
    MutableTestStruct src;
    src.builder().setNestedStruct(inner.reader());
    ConstRootMessage<TestStruct> flat(src);
    sizeWithDataAndList = flat.bytes().size();
  }

  inner.builder().disownData();
  {
    MutableTestStruct src;
    src.builder().setNestedStruct(inner.reader());
    ConstRootMessage<TestStruct> flat(src);
    sizeWithNoData = flat.bytes().size();
  }

  inner.builder().disownListStruct();
  {
    MutableTestStruct src;
    src.builder().setNestedStruct(inner.reader());
    ConstRootMessage<TestStruct> flat(src);
    sizeWithNothing = flat.bytes().size();
  }

  EXPECT_GT(sizeWithDataAndList, sizeWithNoData);
  EXPECT_GT(sizeWithNoData, sizeWithNothing);
}

TEST_F(CapnpMessagesTest, TestConstRootDontCopyDisownedData) {
  constexpr int NUM_ITEMS = 10000;
  MutableRootMessage<InnerStruct> src;
  auto srcBuilder = src.builder();
  srcBuilder.initData(NUM_ITEMS);

  ConstRootMessage<InnerStruct> constMessage(src.reader());
  int sizeWithData = constMessage.bytes().size();

  MutableRootMessage<InnerStruct> copy1(constMessage.reader());
  copy1.builder().disownData();

  ConstRootMessage<InnerStruct> constMessageSmall(copy1.reader());
  EXPECT_LT(constMessageSmall.bytes().size(), sizeWithData);
}

TEST_F(CapnpMessagesTest, TestClone) {
  MutableTestStruct src;
  src.builder().setName("name");
  src.builder().setInt(1492);
  src.builder().setFloat(3.14159f);

  // Initialize a const as a copy of a reader.
  ConstTestStruct intermediate(src);

  ConstTestStruct dest = intermediate.clone();
  EXPECT_STREQ(dest.reader().getName().cStr(), "name");
  EXPECT_EQ(dest.reader().getInt(), 1492);
  EXPECT_EQ(dest.reader().getFloat(), 3.14159f);
}

TEST_F(CapnpMessagesTest, TestArrayMove) {
  MutableTestStruct src;
  src.builder().setName("name");
  src.builder().setInt(1492);
  src.builder().setFloat(3.14159f);

  // Flatten to a byte array.
  ConstTestStruct intermediate(src);

  // Allocate a kj::Array from the contents of the byte array, and move it into a constrootmessage.
  kj::Array<capnp::word> arrayToMove =
    kj::heapArray<capnp::word>(intermediate.bytes().size() / sizeof(capnp::word));
  std::memcpy(arrayToMove.begin(), intermediate.bytes().begin(), intermediate.bytes().size());

  ConstTestStruct dest(std::move(arrayToMove));
  EXPECT_STREQ(dest.reader().getName().cStr(), "name");
  EXPECT_EQ(dest.reader().getInt(), 1492);
  EXPECT_EQ(dest.reader().getFloat(), 3.14159f);
}

TEST_F(CapnpMessagesTest, TestView) {
  MutableTestStruct src;
  src.builder().setName("name");
  src.builder().setInt(1492);
  src.builder().setFloat(3.14159f);

  // Initialize a const as a copy of a reader.
  ConstTestStruct dest(src.reader());
  EXPECT_STREQ(dest.reader().getName().cStr(), "name");
  EXPECT_EQ(dest.reader().getInt(), 1492);
  EXPECT_EQ(dest.reader().getFloat(), 3.14159f);

  auto buffer = dest.bytes();
  ConstTestStructView view(buffer.begin(), buffer.size());
  EXPECT_STREQ(view.reader().getName().cStr(), "name");
  EXPECT_EQ(view.reader().getInt(), 1492);
  EXPECT_EQ(view.reader().getFloat(), 3.14159f);

  ConstTestStructView view2 = std::move(view);
  EXPECT_STREQ(view2.reader().getName().cStr(), "name");
  EXPECT_EQ(view2.reader().getInt(), 1492);
  EXPECT_EQ(view2.reader().getFloat(), 3.14159f);
}

}  // namespace c8
