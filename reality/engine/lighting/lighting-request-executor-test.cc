// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":lighting-request-executor",
    "//c8/io:capnp-messages",
    "//c8/pixels:pixels",
    "//c8/protolog:xr-requests",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x52a80eae);

#include "reality/engine/lighting/lighting-request-executor.h"

#include <gtest/gtest.h>
#include "c8/io/capnp-messages.h"
#include "c8/pixels/pixels.h"
#include "c8/protolog/xr-requests.h"
#include "c8/stats/scope-timer.h"

using MutableRequestSensor = c8::MutableRootMessage<c8::RequestSensor>;
using MutableResponseLighting = c8::MutableRootMessage<c8::ResponseLighting>;

namespace c8 {

class LightingRequestExecutorTest : public ::testing::Test {};

TEST_F(LightingRequestExecutorTest, TestLighting) {
  ScopeTimer rt("test");
  MutableRequestSensor sensorMessage;
  MutableResponseLighting responseMessage;

  // This is a 5x5 pixel buffer, but we will tell the sensor it is a 4x3 pixel buffer. The resulting
  // image should contain {48, 30, 10, 231, 255, 247, 252, 249, 244, 179, 169, 174}.
  uint8_t pixelData[] = {
    48,  30,  10,  11,  23,   // Row 0
    231, 255, 247, 255, 255,  // Row 1
    252, 249, 244, 232, 69,   // Row 2
    179, 169, 174, 0,   17,   // Row 3
    36,  72,  69,  31,  43    // Row 4
  };

  YPlanePixels srcY(4, 3, 5, pixelData);
  UVPlanePixels srcUV(0, 0, 0, nullptr);

  auto sensorBuilder = sensorMessage.builder();
  auto cameraBuilder = sensorBuilder.getCamera();
  setCameraPixelPointers(srcY, srcUV, &cameraBuilder);

  auto devicePose = sensorBuilder.getPose().getDevicePose();
  auto deviceAcc = sensorBuilder.getPose().getDeviceAcceleration();
  setQuaternion32f(1.0, 2.0, 3.0, 4.0, &devicePose);
  setPosition32f(-1.0, -2.0, -3.0, &deviceAcc);

  ResponseLighting::Builder responseBuilder = responseMessage.builder();

  TaskQueue taskQueue;
  ThreadPool threadPool(1);
  LightingRequestExecutor executor;
  executor.execute(sensorBuilder, &responseBuilder, &taskQueue, &threadPool);

  EXPECT_FLOAT_EQ(0.41666667f, responseBuilder.getGlobal().getExposure());
}

TEST_F(LightingRequestExecutorTest, TestLightingPyramid) {
  ScopeTimer rt("test");
  MutableRequestSensor sensorMessage;
  MutableResponseLighting responseMessage;

  // This is a 3x12 pixel buffer, the pyramid points to the the bottom right 4 pixels
  // Note that pyramid processing takes in a 4-channel image (RGBA)
  // (255, 0
  //  34, 56)
  uint8_t pixelData[] = {
    0, 0, 0, 1,   2, 3, 4, 5,  6, 7, 8, 9,  // Row 0
    0, 0, 0, 1, 255, 3, 4, 5,  0, 7, 8, 9,  // Row 4
    0, 0, 0, 1,  34, 3, 4, 5, 56, 7, 8, 9,  // Row 8
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

  auto devicePose = sensorBuilder.getPose().getDevicePose();
  auto deviceAcc = sensorBuilder.getPose().getDeviceAcceleration();
  setQuaternion32f(1.0, 2.0, 3.0, 4.0, &devicePose);
  setPosition32f(-1.0, -2.0, -3.0, &deviceAcc);

  ResponseLighting::Builder responseBuilder = responseMessage.builder();

  TaskQueue taskQueue;
  ThreadPool threadPool(1);
  LightingRequestExecutor executor;
  executor.execute(sensorBuilder, &responseBuilder, &taskQueue, &threadPool);

  EXPECT_NEAR(0.0f, responseBuilder.getGlobal().getExposure(), 1e-6);
}

TEST_F(LightingRequestExecutorTest, TestLightingARKit) {
  ScopeTimer rt("test");
  TaskQueue taskQueue;
  ThreadPool threadPool(1);
  MutableRequestSensor sensorMessage;
  MutableResponseLighting responseMessage;
  LightingRequestExecutor executor;

  ResponseLighting::Builder responseBuilder = responseMessage.builder();
  auto sensorBuilder = sensorMessage.builder();
  auto cameraBuilder = sensorBuilder.getCamera();

  // This is a 5x5 pixel buffer, but we will tell the sensor it is a 4x3 pixel buffer. The resulting
  // image should contain {48, 30, 10, 231, 255, 247, 252, 249, 244, 179, 169, 174}.
  uint8_t pixelData[] = {
    48,  30,  10,  11,  23,   // Row 0
    231, 255, 247, 255, 255,  // Row 1
    252, 249, 244, 232, 69,   // Row 2
    179, 169, 174, 0,   17,   // Row 3
    36,  72,  69,  31,  43    // Row 4
  };

  YPlanePixels srcY(4, 3, 5, pixelData);
  UVPlanePixels srcUV(0, 0, 0, nullptr);
  auto arkit = sensorBuilder.getARKit();
  setCameraPixelPointers(srcY, srcUV, &cameraBuilder);

  arkit.getLightEstimate().setAmbientIntensity(-1000.0f);
  executor.execute(sensorBuilder, &responseBuilder, &taskQueue, &threadPool);
  EXPECT_EQ(-1.0f, responseBuilder.getGlobal().getExposure());

  arkit.getLightEstimate().setAmbientIntensity(0.0f);
  executor.execute(sensorBuilder, &responseBuilder, &taskQueue, &threadPool);
  EXPECT_EQ(-1.0f, responseBuilder.getGlobal().getExposure());

  arkit.getLightEstimate().setAmbientIntensity(500.0f);
  executor.execute(sensorBuilder, &responseBuilder, &taskQueue, &threadPool);
  EXPECT_EQ(-0.5f, responseBuilder.getGlobal().getExposure());

  arkit.getLightEstimate().setAmbientIntensity(1000.0f);
  executor.execute(sensorBuilder, &responseBuilder, &taskQueue, &threadPool);
  EXPECT_EQ(0.0f, responseBuilder.getGlobal().getExposure());

  arkit.getLightEstimate().setAmbientIntensity(1500.0f);
  executor.execute(sensorBuilder, &responseBuilder, &taskQueue, &threadPool);
  EXPECT_EQ(0.5f, responseBuilder.getGlobal().getExposure());

  arkit.getLightEstimate().setAmbientIntensity(2000.0f);
  executor.execute(sensorBuilder, &responseBuilder, &taskQueue, &threadPool);
  EXPECT_EQ(1.0f, responseBuilder.getGlobal().getExposure());

  arkit.getLightEstimate().setAmbientIntensity(3000.0f);
  executor.execute(sensorBuilder, &responseBuilder, &taskQueue, &threadPool);
  EXPECT_EQ(1.0f, responseBuilder.getGlobal().getExposure());
}

TEST_F(LightingRequestExecutorTest, TestLightingARCore) {
  ScopeTimer rt("test");
  TaskQueue taskQueue;
  ThreadPool threadPool(1);
  MutableRequestSensor sensorMessage;
  MutableResponseLighting responseMessage;
  LightingRequestExecutor executor;

  ResponseLighting::Builder responseBuilder = responseMessage.builder();
  auto sensorBuilder = sensorMessage.builder();
  auto cameraBuilder = sensorBuilder.getCamera();

  // This is a 5x5 pixel buffer, but we will tell the sensor it is a 4x3 pixel buffer. The resulting
  // image should contain {48, 30, 10, 231, 255, 247, 252, 249, 244, 179, 169, 174}.
  uint8_t pixelData[] = {
    48,  30,  10,  11,  23,   // Row 0
    231, 255, 247, 255, 255,  // Row 1
    252, 249, 244, 232, 69,   // Row 2
    179, 169, 174, 0,   17,   // Row 3
    36,  72,  69,  31,  43    // Row 4
  };

  YPlanePixels srcY(4, 3, 5, pixelData);
  UVPlanePixels srcUV(0, 0, 0, nullptr);
  auto arcore = sensorBuilder.getARCore();
  setCameraPixelPointers(srcY, srcUV, &cameraBuilder);

  arcore.getLightEstimate().setPixelIntensity(0.0f);
  executor.execute(sensorBuilder, &responseBuilder, &taskQueue, &threadPool);
  EXPECT_EQ(-1.0f, responseBuilder.getGlobal().getExposure());

  arcore.getLightEstimate().setPixelIntensity(0.125f);
  executor.execute(sensorBuilder, &responseBuilder, &taskQueue, &threadPool);
  EXPECT_EQ(-0.75f, responseBuilder.getGlobal().getExposure());

  arcore.getLightEstimate().setPixelIntensity(0.25f);
  executor.execute(sensorBuilder, &responseBuilder, &taskQueue, &threadPool);
  EXPECT_EQ(-0.5f, responseBuilder.getGlobal().getExposure());

  arcore.getLightEstimate().setPixelIntensity(0.375f);
  executor.execute(sensorBuilder, &responseBuilder, &taskQueue, &threadPool);
  EXPECT_EQ(-0.25f, responseBuilder.getGlobal().getExposure());

  arcore.getLightEstimate().setPixelIntensity(0.5f);
  executor.execute(sensorBuilder, &responseBuilder, &taskQueue, &threadPool);
  EXPECT_EQ(0.0f, responseBuilder.getGlobal().getExposure());

  arcore.getLightEstimate().setPixelIntensity(0.75f);
  executor.execute(sensorBuilder, &responseBuilder, &taskQueue, &threadPool);
  EXPECT_EQ(0.5f, responseBuilder.getGlobal().getExposure());

  arcore.getLightEstimate().setPixelIntensity(1.0f);
  executor.execute(sensorBuilder, &responseBuilder, &taskQueue, &threadPool);
  EXPECT_EQ(1.0f, responseBuilder.getGlobal().getExposure());
}

}  // namespace c8
