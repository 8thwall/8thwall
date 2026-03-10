// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  visibility = {
    "//reality/engine/executor:__subpackages__",
    "//reality/engine/sensortest:__subpackages__",
  };
  hdrs = {
    "sensortest-request-executor.h",
  };
  deps = {
    "//reality/engine/api/request:sensor.capnp-cc",
    "//reality/engine/api/response:sensor-test.capnp-cc",
  };
}
cc_end(0x51db87dc);

#include "reality/engine/sensortest/sensortest-request-executor.h"

namespace c8 {

// definitions
void SensorTestRequestExecutor::execute(
  const RequestSensor::Reader &sensor,
  ResponseSensorTest::Builder *response) const {
  // Echo the pose sensor to the output.

  int rows, cols, bytesPerRow;
  // If we are getting a pyramid, then the test is done on the lowest layer: layer 3.
  auto currentFrame = sensor.getCamera().getCurrentFrame();
  uint8_t *rowStart;
  int numChannels;
  if (currentFrame.hasPyramid()) {
    numChannels = 4;
    auto pyramid = currentFrame.getPyramid();
    auto pyramidImage = pyramid.getImage();
    auto levels = pyramid.getLevels();
    auto lastLevel = levels[levels.size() - 1];
    // We don't care about rotation because mean value s geometric-insensitive
    rows = lastLevel.getH();
    cols = lastLevel.getW();
    bytesPerRow = pyramidImage.getBytesPerRow();

    rowStart = reinterpret_cast<uint8_t *>(pyramidImage.getUInt8PixelDataPointer());
    rowStart += lastLevel.getR() * bytesPerRow + lastLevel.getC() * numChannels;
  } else {
    numChannels = 1;
    // Cast a long to a size_t, possibly downcasting, before reinterpreting as a
    // pointer.
    auto grayImagePtr = currentFrame.getImage().getOneOf().getGrayImagePointer();
    rows = grayImagePtr.getRows();
    cols = grayImagePtr.getCols();
    bytesPerRow = grayImagePtr.getBytesPerRow();
    size_t imageAddr = static_cast<size_t>(grayImagePtr.getUInt8PixelDataPointer());
    rowStart = reinterpret_cast<uint8_t *>(imageAddr);
  }

  int64_t sum = 0;
  for (int i = 0; i < rows; ++i) {
    uint8_t *rowEnd = rowStart + cols * numChannels;
    for (uint8_t *pix = rowStart; pix < rowEnd; pix += numChannels) {
      sum += *pix;
    }
    rowStart += bytesPerRow;
  }

  double meanPixelValue = rows * cols > 0 ? sum * 1.0 / (rows * cols) : 0;

  response->getStatus();
  response->getCamera().setMeanPixelValue(meanPixelValue);
  response->getPose().setDevicePose(sensor.getPose().getDevicePose());
  response->getPose().setDeviceAcceleration(sensor.getPose().getDeviceAcceleration());
}

}  // namespace c8
