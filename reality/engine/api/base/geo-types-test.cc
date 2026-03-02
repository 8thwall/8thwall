#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":geo-types.capnp-cc",
    "//c8/io:capnp-messages",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x7bce47c9);

#include <gtest/gtest.h>
#include "c8/io/capnp-messages.h"
#include "reality/engine/api/base/geo-types.capnp.h"

using MutablePosition32f = c8::MutableRootMessage<c8::Position32f>;
using MutableQuaternion32f = c8::MutableRootMessage<c8::Quaternion32f>;
using MutableTransform32f = c8::MutableRootMessage<c8::Transform32f>;

namespace c8 {

class GeoTypesTest : public ::testing::Test {};

TEST_F(GeoTypesTest, Quaternion32fTest) {
  MutableQuaternion32f builder;

  auto quatBuilder = builder.builder();
  quatBuilder.setW(1.0f);
  quatBuilder.setX(2.0f);
  quatBuilder.setY(3.0f);
  quatBuilder.setZ(4.0f);

  auto quat = builder.reader();
  EXPECT_FLOAT_EQ(1.0f, quat.getW());
  EXPECT_FLOAT_EQ(2.0f, quat.getX());
  EXPECT_FLOAT_EQ(3.0f, quat.getY());
  EXPECT_FLOAT_EQ(4.0f, quat.getZ());
}

TEST_F(GeoTypesTest, Position32fTest) {
  MutablePosition32f builder;

  Position32f::Builder input = builder.builder();
  input.setX(1.0f);
  input.setY(2.0f);
  input.setZ(3.0f);

  Position32f::Reader pos = builder.reader();
  EXPECT_FLOAT_EQ(1.0f, pos.getX());
  EXPECT_FLOAT_EQ(2.0f, pos.getY());
  EXPECT_FLOAT_EQ(3.0f, pos.getZ());
}

TEST_F(GeoTypesTest, Transform32fTest) {
  MutableTransform32f builder;

  auto input = builder.builder();
  input.getPosition().setX(1.0f);
  input.getRotation().setZ(3.0f);

  auto transform = builder.reader();
  EXPECT_FLOAT_EQ(1.0f, transform.getPosition().getX());
  EXPECT_FLOAT_EQ(3.0f, transform.getRotation().getZ());
}

}  // namespace c8
