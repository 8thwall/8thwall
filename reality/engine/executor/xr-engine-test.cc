// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":xr-engine",
    "@com_google_googletest//:gtest_main",
    "//c8/io:capnp-messages",
  };
}
cc_end(0xff8a7e2f);

#include "reality/engine/executor/xr-engine.h"

#include <capnp/message.h>
#include <gtest/gtest.h>
#include "c8/io/capnp-messages.h"
#include "reality/engine/api/reality.capnp.h"

using MutableRealityRequest = c8::MutableRootMessage<c8::RealityRequest>;
using MutableRealityResponse = c8::MutableRootMessage<c8::RealityResponse>;

namespace c8 {

class XREngineTest : public ::testing::Test {};

TEST_F(XREngineTest, TestPose) {
  MutableRealityRequest requestMessage;
  MutableRealityResponse responseMessage;

  RealityRequest::Builder requestBuilder = requestMessage.builder();
  RealityResponse::Builder responseBuilder = responseMessage.builder();

  requestBuilder.getMask().setPose(true);

  XREngine executor;
  executor.execute(requestBuilder.asReader(), &responseBuilder);

  EXPECT_TRUE(responseBuilder.hasStatus());
  EXPECT_FALSE(responseBuilder.getStatus().hasError());

  EXPECT_TRUE(responseBuilder.getPose().hasStatus());
  EXPECT_FALSE(responseBuilder.getPose().getStatus().hasError());
}

}  // namespace c8
