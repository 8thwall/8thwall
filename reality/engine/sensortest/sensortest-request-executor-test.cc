// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":sensortest-request-executor",
    "//c8/io:capnp-messages",
    "//c8/stats:scope-timer",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xbda016c2);

#include "reality/engine/sensortest/sensortest-request-executor.h"

#include <random>
#include <gtest/gtest.h>
#include "c8/io/capnp-messages.h"
#include "c8/stats/scope-timer.h"

using MutableRequestSensor = c8::MutableRootMessage<c8::RequestSensor>;
using MutableResponseSensorTest = c8::MutableRootMessage<c8::ResponseSensorTest>;

namespace c8 {

class SensorTestRequestExecutorTestTest : public ::testing::Test {};

TEST_F(SensorTestRequestExecutorTestTest, TestSensorTest) {
  ScopeTimer rt("test");
  MutableRequestSensor sensorMessage;
  MutableResponseSensorTest responseMessage;

  // This is a 5x5 pixel buffer, but we will tell the sensor it is a
  // 4x3 pixel buffer. The resulting image should contain
  // {48, 30, 10, 231, 255, 247, 252, 249, 244, 179, 169, 174}, and the mean
  //  value is 174.
  uint8_t pixelData[] = {
    48,  30,  10,  11,  23,   // Row 0
    231, 255, 247, 255, 255,  // Row 1
    252, 249, 244, 232, 69,   // Row 2
    179, 169, 174, 0,   17,   // Row 3
    36,  72,  69,  31,  43    // Row 4
  };
  size_t pixelDataAddr = reinterpret_cast<size_t>(pixelData);

  auto sensorBuilder = sensorMessage.builder();

  sensorBuilder.getCamera().getCurrentFrame().getImage().getOneOf().getGrayImagePointer().setRows(
    4);
  sensorBuilder.getCamera().getCurrentFrame().getImage().getOneOf().getGrayImagePointer().setCols(
    3);
  sensorBuilder.getCamera()
    .getCurrentFrame()
    .getImage()
    .getOneOf()
    .getGrayImagePointer()
    .setBytesPerRow(5);
  sensorBuilder.getCamera()
    .getCurrentFrame()
    .getImage()
    .getOneOf()
    .getGrayImagePointer()
    .setUInt8PixelDataPointer(pixelDataAddr);

  sensorBuilder.getPose().getDevicePose().setW(1.0);
  sensorBuilder.getPose().getDevicePose().setX(2.0);
  sensorBuilder.getPose().getDevicePose().setY(3.0);
  sensorBuilder.getPose().getDevicePose().setZ(4.0);
  sensorBuilder.getPose().getDeviceAcceleration().setX(-1.0);
  sensorBuilder.getPose().getDeviceAcceleration().setY(-2.0);
  sensorBuilder.getPose().getDeviceAcceleration().setZ(-3.0);

  auto responseBuilder = responseMessage.builder();

  SensorTestRequestExecutor executor;
  executor.execute(sensorBuilder.asReader(), &responseBuilder);

  EXPECT_TRUE(responseBuilder.hasStatus());
  EXPECT_FALSE(responseBuilder.getStatus().hasError());

  EXPECT_EQ(174.0, responseBuilder.getCamera().getMeanPixelValue());
  EXPECT_EQ(1.0, responseBuilder.getPose().getDevicePose().getW());
  EXPECT_EQ(2.0, responseBuilder.getPose().getDevicePose().getX());
  EXPECT_EQ(3.0, responseBuilder.getPose().getDevicePose().getY());
  EXPECT_EQ(4.0, responseBuilder.getPose().getDevicePose().getZ());
  EXPECT_EQ(-1.0, responseBuilder.getPose().getDeviceAcceleration().getX());
  EXPECT_EQ(-2.0, responseBuilder.getPose().getDeviceAcceleration().getY());
  EXPECT_EQ(-3.0, responseBuilder.getPose().getDeviceAcceleration().getZ());
}

TEST_F(SensorTestRequestExecutorTestTest, TestSensorTestPyramid) {
  ScopeTimer rt("test");
  MutableRequestSensor sensorMessage;
  MutableResponseSensorTest responseMessage;

  // This is a 3x12 pixel buffer, the pyramid points to the the bottom right 4 pixels
  // Note that pyramid processing takes in a 4-channel image (RGBA)
  // (2, 2
  //  2, 2)
  uint8_t pixelData[] = {
    0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,  // Row 0
    0, 0, 0, 1, 2, 3, 4, 5, 2, 7, 8, 9,  // Row 4
    0, 0, 0, 1, 2, 3, 4, 5, 2, 7, 8, 9,  // Row 8
  };
  size_t pixelDataAddr = reinterpret_cast<size_t>(pixelData);

  auto sensorBuilder = sensorMessage.builder();
  auto currentFramePyramid = sensorBuilder.getCamera().getCurrentFrame().getPyramid();
  auto pyramidImage = currentFramePyramid.getImage();
  pyramidImage.setRows(3);
  pyramidImage.setCols(3);
  pyramidImage.setBytesPerRow(3 * 4); // Pyramid always contains 4-channel images
  pyramidImage.setUInt8PixelDataPointer(pixelDataAddr);
  auto pyramidLevels = currentFramePyramid.initLevels(4);
  // we only compute data on the last level
  pyramidLevels[3].setC(1);
  pyramidLevels[3].setR(1);
  pyramidLevels[3].setW(2);
  pyramidLevels[3].setH(2);

  sensorBuilder.getPose().getDevicePose().setW(1.0);
  sensorBuilder.getPose().getDevicePose().setX(2.0);
  sensorBuilder.getPose().getDevicePose().setY(3.0);
  sensorBuilder.getPose().getDevicePose().setZ(4.0);
  sensorBuilder.getPose().getDeviceAcceleration().setX(-1.0);
  sensorBuilder.getPose().getDeviceAcceleration().setY(-2.0);
  sensorBuilder.getPose().getDeviceAcceleration().setZ(-3.0);

  auto responseBuilder = responseMessage.builder();
  ///////// START EXECUTING /////////

  SensorTestRequestExecutor executor;
  executor.execute(sensorBuilder.asReader(), &responseBuilder);

  EXPECT_TRUE(responseBuilder.hasStatus());
  EXPECT_FALSE(responseBuilder.getStatus().hasError());

  EXPECT_NEAR(2.0f, responseBuilder.getCamera().getMeanPixelValue(), 1e-7);
  EXPECT_EQ(1.0, responseBuilder.getPose().getDevicePose().getW());
  EXPECT_EQ(2.0, responseBuilder.getPose().getDevicePose().getX());
  EXPECT_EQ(3.0, responseBuilder.getPose().getDevicePose().getY());
  EXPECT_EQ(4.0, responseBuilder.getPose().getDevicePose().getZ());
  EXPECT_EQ(-1.0, responseBuilder.getPose().getDeviceAcceleration().getX());
  EXPECT_EQ(-2.0, responseBuilder.getPose().getDeviceAcceleration().getY());
  EXPECT_EQ(-3.0, responseBuilder.getPose().getDeviceAcceleration().getZ());
}

}  // namespace c8
