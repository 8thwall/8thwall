// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":xr-request-executor",
    "@com_google_googletest//:gtest_main",
    "//c8:random-numbers",
  };
}
cc_end(0xa3165ce1);

#include <gtest/gtest.h>

#include <random>

#include "c8/io/capnp-messages.h"
#include "reality/engine/api/reality.capnp.h"
#include "reality/engine/executor/proto/state.capnp.h"
#include "reality/engine/executor/xr-request-executor.h"

using MutableInternalRealityRequest = c8::MutableRootMessage<c8::InternalRealityRequest>;
using MutableRealityResponse = c8::MutableRootMessage<c8::RealityResponse>;

namespace c8 {

class XRRequestExecutorTest : public ::testing::Test {};

TEST_F(XRRequestExecutorTest, TestExecute) {
  ScopeTimer rt("test");
  Tracker tracker;
  DetectionImageMap targets;
  RandomNumbers random;
  TaskQueue taskQueue;
  ThreadPool threadPool(1);
  MutableInternalRealityRequest requestMessage;
  MutableRealityResponse responseMessage;

  auto requestBuilder = requestMessage.builder();
  auto responseBuilder = responseMessage.builder();

  requestBuilder.getRequest().getMask().setSensorTest(true);
  requestBuilder.getRequest().getMask().setPose(true);

  auto cameraBuilder = requestBuilder.getRequest().getSensors().getCamera();

  uint8_t pixelData[] = {
    48,
    30,
    10,
    231,
    255,
    247,
    252,
    249,
    244,
    179,
    169,
    174,
  };

  size_t pixelDataAddr = reinterpret_cast<size_t>(pixelData);

  cameraBuilder.getCurrentFrame().getImage().getOneOf().getGrayImagePointer().setRows(4);
  cameraBuilder.getCurrentFrame().getImage().getOneOf().getGrayImagePointer().setCols(3);
  cameraBuilder.getCurrentFrame().getImage().getOneOf().getGrayImagePointer().setBytesPerRow(3);
  cameraBuilder.getCurrentFrame()
    .getImage()
    .getOneOf()
    .getGrayImagePointer()
    .setUInt8PixelDataPointer(pixelDataAddr);

  XRRequestExecutor executor;
  executor.execute(
    requestBuilder.asReader(),
    &responseBuilder,
    &tracker,
    &targets,
    &random,
    &taskQueue,
    &threadPool);

  EXPECT_TRUE(responseBuilder.hasStatus());
  EXPECT_FALSE(responseBuilder.getStatus().hasError());

  EXPECT_TRUE(responseBuilder.getSensorTest().hasStatus());
  EXPECT_FALSE(responseBuilder.getSensorTest().getStatus().hasError());

  EXPECT_TRUE(responseBuilder.getPose().hasStatus());
  EXPECT_FALSE(responseBuilder.getPose().getStatus().hasError());
}

}  // namespace c8
