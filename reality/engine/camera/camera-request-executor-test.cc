// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":camera-request-executor",
    "//c8:hmatrix",
    "//c8/io:capnp-messages",
    "//c8/pixels:pixels",
    "//c8/protolog:xr-requests",
    "//reality/engine/api/device:info.capnp-cc",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x90249d63);

#include "reality/engine/camera/camera-request-executor.h"

#include <capnp/message.h>
#include <gtest/gtest.h>
#include "c8/hmatrix.h"
#include "c8/io/capnp-messages.h"
#include "c8/pixels/pixels.h"
#include "c8/protolog/xr-requests.h"
#include "reality/engine/api/device/info.capnp.h"
#include "c8/stats/scope-timer.h"

using MutableDeviceInfo = c8::MutableRootMessage<c8::DeviceInfo>;
using MutableRequestSensor = c8::MutableRootMessage<c8::RequestSensor>;
using MutableResponseCamera = c8::MutableRootMessage<c8::ResponseCamera>;
using MutableResponsePose = c8::MutableRootMessage<c8::ResponsePose>;
using MutableXRConfiguration = c8::MutableRootMessage<c8::XRConfiguration>;

namespace c8 {

class CameraRequestExecutorTest : public ::testing::Test {};

TEST_F(CameraRequestExecutorTest, TestCamera) {
  ScopeTimer rt("test");
  MutableRequestSensor sensorMessage;
  MutableXRConfiguration configMessage;
  MutableDeviceInfo deviceInfoMessage;
  MutableResponsePose poseResponseMessage;
  MutableResponseCamera responseMessage;

  // This is a 5x5 pixel buffer, but we will tell the sensor it is a 4x3 pixel buffer. The resulting
  // image should contain {48, 30, 10, 231, 255, 247, 252, 249, 244, 179, 169, 174}.
  uint8_t pixelData[] = {
    48,  30,  10,  11,  23,   // Row 0
    231, 255, 247, 255, 255,  // Row 1
    252, 249, 244, 232, 69,   // Row 2
    179, 169, 174, 0,   17,   // Row 3
    36,  72,  69,  31,  43    // Row 4
  };

  float pixelDepthData[] = {
    48,  30,  10,  11,  23,   // Row 0
    231, 255, 247, 255, 255,  // Row 1
    252, 249, 244, 232, 69,   // Row 2
    179, 169, 174, 0,   17,   // Row 3
    36,  72,  69,  31,  43    // Row 4
  };

  YPlanePixels srcY(4, 3, 5, pixelData);
  UVPlanePixels srcUV(0, 0, 0, nullptr);
  DepthFloatPixels srcDepth(4, 3, 5, pixelDepthData);

  auto configBuilder = configMessage.builder();

  auto sensorBuilder = sensorMessage.builder();
  auto cameraBuilder = sensorBuilder.getCamera();
  auto arkitBuilder = sensorBuilder.getARKit();
  setCameraPixelPointers(srcY, srcUV, &cameraBuilder);
  setDepthMap(srcDepth, &arkitBuilder);

  auto devicePose = sensorBuilder.getPose().getDevicePose();
  auto deviceAcc = sensorBuilder.getPose().getDeviceAcceleration();
  setQuaternion32f(1.0, 2.0, 3.0, 4.0, &devicePose);
  setPosition32f(-1.0, -2.0, -3.0, &deviceAcc);

  auto poseResponseBuilder = poseResponseMessage.builder();
  auto responsePosition = poseResponseBuilder.getTransform().getPosition();
  auto responseRotation = poseResponseBuilder.getTransform().getRotation();
  setQuaternion32f(5.0f, 6.0f, 7.0f, 8.0f, &responseRotation);
  setPosition32f(-4.0f, -5.0f, -6.0f, &responsePosition);

  auto deviceInfoBuilder = deviceInfoMessage.builder();
  ResponseCamera::Builder responseBuilder = responseMessage.builder();

  CameraRequestExecutor executor;
  executor.execute(
    sensorBuilder,
    configBuilder,
    poseResponseBuilder,
    deviceInfoBuilder,
    &responseBuilder);

  EXPECT_EQ(5.0f, responseBuilder.getExtrinsic().getRotation().getW());
  EXPECT_EQ(6.0f, responseBuilder.getExtrinsic().getRotation().getX());
  EXPECT_EQ(7.0f, responseBuilder.getExtrinsic().getRotation().getY());
  EXPECT_EQ(8.0f, responseBuilder.getExtrinsic().getRotation().getZ());
  EXPECT_EQ(-4.0f, responseBuilder.getExtrinsic().getPosition().getX());
  EXPECT_EQ(-5.0f, responseBuilder.getExtrinsic().getPosition().getY());
  EXPECT_EQ(-6.0f, responseBuilder.getExtrinsic().getPosition().getZ());

  // HMatrix specifies construction in row-major order, but internally stores it in column-major
  // order.  We use it to test that the expected data is stored in column-major order.
  HMatrix expectedExtrinsic{{2.92424, 0.00000, 0.0000000, 0.000000},  // Row 0
                            {0.00000, 1.64488, 0.0015625, 0.000000},  // Row 1
                            {0.00000, 0.00000, -1.000600, -0.60018},  // Row 2
                            {0.00000, 0.00000, -1.000000, 0.000000}};

  for (int i = 0; i < 16; ++i) {
    EXPECT_EQ(expectedExtrinsic.data()[i], responseBuilder.getIntrinsic().getMatrix44f()[i]);
  }
}

TEST_F(CameraRequestExecutorTest, TestCameraWithKnownModel) {
  ScopeTimer rt("test");
  MutableRequestSensor sensorMessage;
  MutableXRConfiguration configMessage;
  MutableDeviceInfo deviceInfoMessage;
  MutableResponsePose poseResponseMessage;
  MutableResponseCamera responseMessage;

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

  auto configBuilder = configMessage.builder();
  configBuilder.setGraphicsIntrinsics(capnp::defaultValue<GraphicsPinholeCameraModel>());
  configBuilder.getGraphicsIntrinsics().setFarClip(5.0f);
  configBuilder.getGraphicsIntrinsics().setNearClip(0.0f);
  configBuilder.getGraphicsIntrinsics().setTextureWidth(480);
  configBuilder.getGraphicsIntrinsics().setTextureHeight(640);

  configBuilder.getCameraConfiguration().getCaptureGeometry().setWidth(480);
  configBuilder.getCameraConfiguration().getCaptureGeometry().setHeight(640);

  auto sensorBuilder = sensorMessage.builder();
  auto cameraBuilder = sensorBuilder.getCamera();
  setCameraPixelPointers(srcY, srcUV, &cameraBuilder);

  auto devicePose = sensorBuilder.getPose().getDevicePose();
  auto deviceAcc = sensorBuilder.getPose().getDeviceAcceleration();
  setQuaternion32f(1.0, 2.0, 3.0, 4.0, &devicePose);
  setPosition32f(-1.0, -2.0, -3.0, &deviceAcc);

  auto poseResponseBuilder = poseResponseMessage.builder();
  auto responsePosition = poseResponseBuilder.getTransform().getPosition();
  auto responseRotation = poseResponseBuilder.getTransform().getRotation();
  setQuaternion32f(5.0f, 6.0f, 7.0f, 8.0f, &responseRotation);
  setPosition32f(-4.0f, -5.0f, -6.0f, &responsePosition);

  auto deviceInfoBuilder = deviceInfoMessage.builder();
  deviceInfoBuilder.setManufacturer("APPLE");
  deviceInfoBuilder.setModel("IPHONE7,2");

  ResponseCamera::Builder responseBuilder = responseMessage.builder();
  CameraRequestExecutor executor;
  executor.execute(
    sensorBuilder,
    configBuilder,
    poseResponseBuilder,
    deviceInfoBuilder,
    &responseBuilder);

  // HMatrix specifies construction in row-major order, but internally stores it in column-major
  // order.  We use it to test that the expected data is stored in column-major order.
  HMatrix expectedIntrinsic{{2.3422587, 0.00000, 0.000000, 0.000000},  // Row 0
                            {0.00000, 1.7566941, 0.000000, 0.000000},  // Row 1
                            {0.00000, 0.00000, -1.000000, 0.00000},    // Row 2
                            {0.00000, 0.00000, -1.000000, 0.000000}};

  for (int i = 0; i < 16; ++i) {
    EXPECT_FLOAT_EQ(expectedIntrinsic.data()[i], responseBuilder.getIntrinsic().getMatrix44f()[i]);
  }
}

// This method is used to test how the image and grpahics intrinsics are changed to get the desired
// aspect ratio which is 3:4 for both.
TEST_F(CameraRequestExecutorTest, TestAspectRatioIntrinsics) {
  ScopeTimer rt("test");
  MutableRequestSensor sensorMessage;
  MutableXRConfiguration configMessage;
  MutableDeviceInfo deviceInfoMessage;
  MutableResponsePose poseResponseMessage;
  MutableResponseCamera responseMessage;
  CameraRequestExecutor executor;

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

  auto configBuilder = configMessage.builder();
  configBuilder.setGraphicsIntrinsics(capnp::defaultValue<GraphicsPinholeCameraModel>());

  // Set the parameters of the Graphics Intrinsics common to all cases in this unit test.
  configBuilder.getGraphicsIntrinsics().setFarClip(5.0f);

  auto sensorBuilder = sensorMessage.builder();
  auto cameraBuilder = sensorBuilder.getCamera();
  setCameraPixelPointers(srcY, srcUV, &cameraBuilder);

  auto devicePose = sensorBuilder.getPose().getDevicePose();
  auto deviceAcc = sensorBuilder.getPose().getDeviceAcceleration();
  setQuaternion32f(1.0, 2.0, 3.0, 4.0, &devicePose);
  setPosition32f(-1.0, -2.0, -3.0, &deviceAcc);

  auto deviceInfoBuilder = deviceInfoMessage.builder();

  auto poseResponseBuilder = poseResponseMessage.builder();
  auto responsePosition = poseResponseBuilder.getTransform().getPosition();
  auto responseRotation = poseResponseBuilder.getTransform().getRotation();
  setQuaternion32f(5.0f, 6.0f, 7.0f, 8.0f, &responseRotation);
  setPosition32f(-4.0f, -5.0f, -6.0f, &responsePosition);

  ResponseCamera::Builder responseBuilder = responseMessage.builder();

  // Set the pixel intrinsics and graphics intrinsics as per different test cases and test the
  // method.

  /* Test Case - 1:
      Image Aspect Ratio - 3:4
      Graphics Aspect Ratio - 3:4
  */
  cameraBuilder.getPixelIntrinsics().setPixelsWidth(480);
  cameraBuilder.getPixelIntrinsics().setPixelsHeight(640);
  cameraBuilder.getPixelIntrinsics().setCenterPointX(239.5f);
  cameraBuilder.getPixelIntrinsics().setCenterPointY(319.5f);
  cameraBuilder.getPixelIntrinsics().setFocalLengthHorizontal(562.1421f);
  cameraBuilder.getPixelIntrinsics().setFocalLengthVertical(562.1421f);
  configBuilder.getGraphicsIntrinsics().setTextureWidth(480);
  configBuilder.getGraphicsIntrinsics().setTextureHeight(640);

  executor.execute(
    sensorBuilder,
    configBuilder,
    poseResponseBuilder,
    deviceInfoBuilder,
    &responseBuilder);

  // HMatrix specifies construction in row-major order, but internally stores it in column-major
  // order.  We use it to test that the expected data is stored in column-major order.

  HMatrix expectedIntrinsic{{2.3422587, 0.00000, 0.000000, 0.000000},  // Row 0
                            {0.00000, 1.7566941, 0.000000, 0.000000},  // Row 1
                            {0.00000, 0.00000, -1.000000, 0.00000},    // Row 2
                            {0.00000, 0.00000, -1.000000, 0.000000}};

  for (int i = 0; i < 16; ++i) {
    EXPECT_FLOAT_EQ(expectedIntrinsic.data()[i], responseBuilder.getIntrinsic().getMatrix44f()[i]);
  }
  /* Test Case - 2:
      Image Aspect Ratio - 3:4
      Graphics Aspect Ratio - 9:16
  */
  cameraBuilder.getPixelIntrinsics().setPixelsWidth(480);
  cameraBuilder.getPixelIntrinsics().setPixelsHeight(640);
  cameraBuilder.getPixelIntrinsics().setCenterPointX(239.5f);
  cameraBuilder.getPixelIntrinsics().setCenterPointY(319.5f);
  cameraBuilder.getPixelIntrinsics().setFocalLengthHorizontal(562.1421f);
  cameraBuilder.getPixelIntrinsics().setFocalLengthVertical(562.1421f);
  configBuilder.getGraphicsIntrinsics().setTextureWidth(720);
  configBuilder.getGraphicsIntrinsics().setTextureHeight(1280);

  executor.execute(
    sensorBuilder,
    configBuilder,
    poseResponseBuilder,
    deviceInfoBuilder,
    &responseBuilder);

  HMatrix expectedIntrinsic2{{3.1230116, 0.00000, 0.000000, 0.000000},  // Row 0
                             {0.00000, 1.7566941, 0.000000, 0.000000},  // Row 1
                             {0.00000, 0.00000, -1.000000, 0.00000},    // Row 2
                             {0.00000, 0.00000, -1.000000, 0.000000}};

  for (int i = 0; i < 16; ++i) {
    EXPECT_FLOAT_EQ(expectedIntrinsic2.data()[i], responseBuilder.getIntrinsic().getMatrix44f()[i]);
  }

  /* Test Case - 3:
      Image Aspect Ratio - 9:16
      Graphics Aspect Ratio - 9:16
  */
  cameraBuilder.getPixelIntrinsics().setPixelsWidth(720);
  cameraBuilder.getPixelIntrinsics().setPixelsHeight(1280);
  cameraBuilder.getPixelIntrinsics().setCenterPointX(359.5f);
  cameraBuilder.getPixelIntrinsics().setCenterPointY(639.5f);
  cameraBuilder.getPixelIntrinsics().setFocalLengthHorizontal(1124.2842f);
  cameraBuilder.getPixelIntrinsics().setFocalLengthVertical(1124.2842f);
  configBuilder.getGraphicsIntrinsics().setTextureWidth(720);
  configBuilder.getGraphicsIntrinsics().setTextureHeight(1280);
  executor.execute(
    sensorBuilder,
    configBuilder,
    poseResponseBuilder,
    deviceInfoBuilder,
    &responseBuilder);

  HMatrix expectedIntrinsic3{{3.1230117, 0.00000, 0.000000, 0.000000},  // Row 0
                             {0.00000, 1.7566941, 0.000000, 0.000000},  // Row 1
                             {0.00000, 0.00000, -1.000000, 0.00000},    // Row 2
                             {0.00000, 0.00000, -1.000000, 0.000000}};

  for (int i = 0; i < 16; ++i) {
    EXPECT_FLOAT_EQ(expectedIntrinsic3.data()[i], responseBuilder.getIntrinsic().getMatrix44f()[i]);
  }

  /* Test Case - 4:
      Image Aspect Ratio - 9:16
      Graphics Aspect Ratio - 3:4
  */
  cameraBuilder.getPixelIntrinsics().setPixelsWidth(720);
  cameraBuilder.getPixelIntrinsics().setPixelsHeight(1280);
  cameraBuilder.getPixelIntrinsics().setCenterPointX(359.5f);
  cameraBuilder.getPixelIntrinsics().setCenterPointY(639.5f);
  cameraBuilder.getPixelIntrinsics().setFocalLengthHorizontal(1124.2842f);
  cameraBuilder.getPixelIntrinsics().setFocalLengthVertical(1124.2842f);
  configBuilder.getGraphicsIntrinsics().setTextureWidth(480);
  configBuilder.getGraphicsIntrinsics().setTextureHeight(640);
  executor.execute(
    sensorBuilder,
    configBuilder,
    poseResponseBuilder,
    deviceInfoBuilder,
    &responseBuilder);

  HMatrix expectedIntrinsic4{{3.1230116, 0.00000, 0.000000, 0.000000},  // Row 0
                             {0.00000, 2.3422587, 0.000000, 0.000000},  // Row 1
                             {0.00000, 0.00000, -1.000000, 0.00000},    // Row 2
                             {0.00000, 0.00000, -1.000000, 0.000000}};

  for (int i = 0; i < 16; ++i) {
    EXPECT_FLOAT_EQ(expectedIntrinsic4.data()[i], responseBuilder.getIntrinsic().getMatrix44f()[i]);
  }

  /* Test Case - 5:
      Image Aspect Ratio - 1:1
      Graphics Aspect Ratio - 3:4
  */
  cameraBuilder.getPixelIntrinsics().setPixelsWidth(480);
  cameraBuilder.getPixelIntrinsics().setPixelsHeight(480);
  cameraBuilder.getPixelIntrinsics().setCenterPointX(239.5f);
  cameraBuilder.getPixelIntrinsics().setCenterPointY(239.5f);
  cameraBuilder.getPixelIntrinsics().setFocalLengthHorizontal(562.1421f);
  cameraBuilder.getPixelIntrinsics().setFocalLengthVertical(562.1421f);
  configBuilder.getGraphicsIntrinsics().setTextureWidth(480);
  configBuilder.getGraphicsIntrinsics().setTextureHeight(640);

  executor.execute(
    sensorBuilder,
    configBuilder,
    poseResponseBuilder,
    deviceInfoBuilder,
    &responseBuilder);

  HMatrix expectedIntrinsic5{{3.1230116, 0.00000, 0.000000, 0.000000},  // Row 0
                             {0.00000, 2.3422587, 0.000000, 0.000000},  // Row 1
                             {0.00000, 0.00000, -1.000000, 0.00000},    // Row 2
                             {0.00000, 0.00000, -1.000000, 0.000000}};

  for (int i = 0; i < 16; ++i) {
    EXPECT_FLOAT_EQ(expectedIntrinsic5.data()[i], responseBuilder.getIntrinsic().getMatrix44f()[i]);
  }

  /* Test Case - 6:
      Image Aspect Ratio - 1:1
      Graphics Aspect Ratio - 9:16
  */
  cameraBuilder.getPixelIntrinsics().setPixelsWidth(480);
  cameraBuilder.getPixelIntrinsics().setPixelsHeight(480);
  cameraBuilder.getPixelIntrinsics().setCenterPointX(239.5f);
  cameraBuilder.getPixelIntrinsics().setCenterPointY(239.5f);
  cameraBuilder.getPixelIntrinsics().setFocalLengthHorizontal(562.1421f);
  cameraBuilder.getPixelIntrinsics().setFocalLengthVertical(562.1421f);
  configBuilder.getGraphicsIntrinsics().setTextureWidth(720);
  configBuilder.getGraphicsIntrinsics().setTextureHeight(1280);

  executor.execute(
    sensorBuilder,
    configBuilder,
    poseResponseBuilder,
    deviceInfoBuilder,
    &responseBuilder);

  HMatrix expectedIntrinsic6{{4.1640158, 0.00000, 0.000000, 0.000000},  // Row 0
                             {0.00000, 2.3422587, 0.000000, 0.000000},  // Row 1
                             {0.00000, 0.00000, -1.000000, 0.00000},    // Row 2
                             {0.00000, 0.00000, -1.000000, 0.000000}};

  for (int i = 0; i < 16; ++i) {
    EXPECT_FLOAT_EQ(expectedIntrinsic6.data()[i], responseBuilder.getIntrinsic().getMatrix44f()[i]);
  }

  /* Test Case - 7:
      Image Aspect Ratio - 3:4
      Graphics Aspect Ratio - 1:1
  */
  cameraBuilder.getPixelIntrinsics().setPixelsWidth(480);
  cameraBuilder.getPixelIntrinsics().setPixelsHeight(640);
  cameraBuilder.getPixelIntrinsics().setCenterPointX(239.5f);
  cameraBuilder.getPixelIntrinsics().setCenterPointY(319.5f);
  cameraBuilder.getPixelIntrinsics().setFocalLengthHorizontal(562.1421f);
  cameraBuilder.getPixelIntrinsics().setFocalLengthVertical(562.1421f);
  configBuilder.getGraphicsIntrinsics().setTextureWidth(480);
  configBuilder.getGraphicsIntrinsics().setTextureHeight(480);
  executor.execute(
    sensorBuilder,
    configBuilder,
    poseResponseBuilder,
    deviceInfoBuilder,
    &responseBuilder);

  HMatrix expectedIntrinsic7{{2.3422587, 0.00000, 0.000000, 0.000000},  // Row 0
                             {0.00000, 2.3422587, 0.000000, 0.000000},  // Row 1
                             {0.00000, 0.00000, -1.000000, 0.00000},    // Row 2
                             {0.00000, 0.00000, -1.000000, 0.000000}};

  for (int i = 0; i < 16; ++i) {
    EXPECT_FLOAT_EQ(expectedIntrinsic7.data()[i], responseBuilder.getIntrinsic().getMatrix44f()[i]);
  }
}  // TestAspectRatioIntrinsics

// This method is used to test if off-center center points are preserved
TEST_F(CameraRequestExecutorTest, TestCenterPointIntrinsics) {
  ScopeTimer rt("test");
  MutableRequestSensor sensorMessage;
  MutableXRConfiguration configMessage;
  MutableDeviceInfo deviceInfoMessage;
  MutableResponsePose poseResponseMessage;
  MutableResponseCamera responseMessage;
  CameraRequestExecutor executor;

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

  auto configBuilder = configMessage.builder();
  configBuilder.setGraphicsIntrinsics(capnp::defaultValue<GraphicsPinholeCameraModel>());

  // Set the parameters of the Graphics Intrinsics common to all cases in this unit test.
  configBuilder.getGraphicsIntrinsics().setFarClip(5.0f);

  auto sensorBuilder = sensorMessage.builder();
  auto cameraBuilder = sensorBuilder.getCamera();
  setCameraPixelPointers(srcY, srcUV, &cameraBuilder);

  auto devicePose = sensorBuilder.getPose().getDevicePose();
  auto deviceAcc = sensorBuilder.getPose().getDeviceAcceleration();
  setQuaternion32f(1.0, 2.0, 3.0, 4.0, &devicePose);
  setPosition32f(-1.0, -2.0, -3.0, &deviceAcc);

  auto deviceInfoBuilder = deviceInfoMessage.builder();

  auto poseResponseBuilder = poseResponseMessage.builder();
  auto responsePosition = poseResponseBuilder.getTransform().getPosition();
  auto responseRotation = poseResponseBuilder.getTransform().getRotation();
  setQuaternion32f(5.0f, 6.0f, 7.0f, 8.0f, &responseRotation);
  setPosition32f(-4.0f, -5.0f, -6.0f, &responsePosition);

  ResponseCamera::Builder responseBuilder = responseMessage.builder();

  // Set the pixel intrinsics and graphics intrinsics as per different test cases and test the
  // method.

  /* Test Case - 1:
    Center at 75% across diagonal, not 50%
  */
  cameraBuilder.getPixelIntrinsics().setPixelsWidth(480);
  cameraBuilder.getPixelIntrinsics().setPixelsHeight(640);
  cameraBuilder.getPixelIntrinsics().setCenterPointX(359.5f);
  cameraBuilder.getPixelIntrinsics().setCenterPointY(479.5f);
  cameraBuilder.getPixelIntrinsics().setFocalLengthHorizontal(562.1421f);
  cameraBuilder.getPixelIntrinsics().setFocalLengthVertical(562.1421f);
  configBuilder.getGraphicsIntrinsics().setTextureWidth(480);
  configBuilder.getGraphicsIntrinsics().setTextureHeight(640);

  executor.execute(
    sensorBuilder,
    configBuilder,
    poseResponseBuilder,
    deviceInfoBuilder,
    &responseBuilder);

  // HMatrix specifies construction in row-major order, but internally stores it in column-major
  // order.  We use it to test that the expected data is stored in column-major order.

  HMatrix expectedIntrinsic{{2.3422587, 0.00000, -0.500000, 0.000000},  // Row 0
                            {0.00000, 1.7566941, 0.500000, 0.000000},   // Row 1
                            {0.00000, 0.00000, -1.000000, 0.00000},     // Row 2
                            {0.00000, 0.00000, -1.000000, 0.000000}};

  for (int i = 0; i < 16; ++i) {
    EXPECT_FLOAT_EQ(expectedIntrinsic.data()[i], responseBuilder.getIntrinsic().getMatrix44f()[i]);
  }

  auto p1 =
    toPinholeModelStruct(responseBuilder.getIntrinsic().getMatrix44f().asReader(), 480, 640);
  EXPECT_EQ(cameraBuilder.getPixelIntrinsics().getPixelsHeight(), p1.pixelsHeight);
  EXPECT_EQ(cameraBuilder.getPixelIntrinsics().getPixelsWidth(), p1.pixelsWidth);
  EXPECT_EQ(
    cameraBuilder.getPixelIntrinsics().getFocalLengthHorizontal(), p1.focalLengthHorizontal);
  EXPECT_EQ(cameraBuilder.getPixelIntrinsics().getFocalLengthVertical(), p1.focalLengthVertical);
  EXPECT_EQ(cameraBuilder.getPixelIntrinsics().getCenterPointX(), p1.centerPointX);
  EXPECT_EQ(cameraBuilder.getPixelIntrinsics().getCenterPointY(), p1.centerPointY);

  /* Test Case - 2:
    CenterY at 25% down image, not 50%
  */
  cameraBuilder.getPixelIntrinsics().setPixelsWidth(480);
  cameraBuilder.getPixelIntrinsics().setPixelsHeight(640);
  cameraBuilder.getPixelIntrinsics().setCenterPointX(119.5f);
  cameraBuilder.getPixelIntrinsics().setCenterPointY(159.5f);
  cameraBuilder.getPixelIntrinsics().setFocalLengthHorizontal(562.1421f);
  cameraBuilder.getPixelIntrinsics().setFocalLengthVertical(562.1421f);
  configBuilder.getGraphicsIntrinsics().setTextureWidth(480);
  configBuilder.getGraphicsIntrinsics().setTextureHeight(640);

  executor.execute(
    sensorBuilder,
    configBuilder,
    poseResponseBuilder,
    deviceInfoBuilder,
    &responseBuilder);

  HMatrix expectedIntrinsic2{{2.3422587, 0.00000, 0.500000, 0.000000},   // Row 0
                             {0.00000, 1.7566941, -0.500000, 0.000000},  // Row 1
                             {0.00000, 0.00000, -1.000000, 0.00000},     // Row 2
                             {0.00000, 0.00000, -1.000000, 0.000000}};

  for (int i = 0; i < 16; ++i) {
    EXPECT_FLOAT_EQ(expectedIntrinsic2.data()[i], responseBuilder.getIntrinsic().getMatrix44f()[i]);
  }

  auto p2 =
    toPinholeModelStruct(responseBuilder.getIntrinsic().getMatrix44f().asReader(), 480, 640);
  EXPECT_EQ(cameraBuilder.getPixelIntrinsics().getPixelsHeight(), p2.pixelsHeight);
  EXPECT_EQ(cameraBuilder.getPixelIntrinsics().getPixelsWidth(), p2.pixelsWidth);
  EXPECT_EQ(
    cameraBuilder.getPixelIntrinsics().getFocalLengthHorizontal(), p2.focalLengthHorizontal);
  EXPECT_EQ(cameraBuilder.getPixelIntrinsics().getFocalLengthVertical(), p2.focalLengthVertical);
  EXPECT_EQ(cameraBuilder.getPixelIntrinsics().getCenterPointX(), p2.centerPointX);
  EXPECT_EQ(cameraBuilder.getPixelIntrinsics().getCenterPointY(), p2.centerPointY);
}  // TestCenterPointIntrinsics

}  // namespace c8
