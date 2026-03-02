// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  visibility = {
    "//reality/engine/executor:__subpackages__", "//reality/engine/lighting:__subpackages__",
  };
  hdrs = {
    "lighting-request-executor.h",
  };
  deps = {
    "//reality/engine/api:reality.capnp-cc",
    "//reality/engine/api/request:sensor.capnp-cc",
    ":lighting-estimator",
  };
}
cc_end(0xc4bdaabf);

#include "reality/engine/lighting/lighting-estimator.h"
#include "reality/engine/lighting/lighting-request-executor.h"

namespace c8 {

namespace {

void setExposureARKit(
  const ARKitLightingEstimate::Reader &lightEstimate, ResponseLighting::Builder *response) {
  // TODO(nb): Determine whether this linear model makes sense.
  float exposure = (lightEstimate.getAmbientIntensity() - 1000.0f) / 1000.0f;
  exposure = std::max(exposure, -1.0f);
  exposure = std::min(exposure, 1.0f);
  response->getGlobal().setExposure(exposure);

  if (lightEstimate.getAmbientColorTemperature() != 0.0f) {
    response->getGlobal().setTemperature(lightEstimate.getAmbientColorTemperature());
  } else {
    response->getGlobal().setTemperature(6500);
  }
}

void setExposureARCore(
  const ARCoreLightingEstimate::Reader &lightEstimate, ResponseLighting::Builder *response) {
  // Using 0.5f as ARCore's neutral light value until one is explicitly given by the API.
  float exposure = (lightEstimate.getPixelIntensity() - 0.5) / 0.5;
  exposure = std::max(exposure, -1.0f);
  exposure = std::min(exposure, 1.0f);
  response->getGlobal().setExposure(exposure);
  response->getGlobal().setTemperature(6500);
}

}  // namespace

// definitions
void LightingRequestExecutor::execute(
  const RequestSensor::Reader &sensor,
  ResponseLighting::Builder *response,
  TaskQueue *taskQueue,
  ThreadPool *threadPool) const {
  if (sensor.getARKit().hasLightEstimate()) {
    setExposureARKit(sensor.getARKit().getLightEstimate(), response);
    return;
  } else if (sensor.getARCore().hasLightEstimate()) {
    setExposureARCore(sensor.getARCore().getLightEstimate(), response);
    return;
  }

  // Use the native exposure estimation method
  // Get the Y plane image from the sensor and convert it to y-plane image.
  auto currentFrame = sensor.getCamera().getCurrentFrame();
  int rows, cols, bytesPerRow;
  uint8_t *rowStart;
  int numChannels;
  if (currentFrame.hasPyramid()) {
    numChannels = PYRAMID_NUM_CHANNELS;
    auto pyramid = currentFrame.getPyramid();
    auto pyramidImage = pyramid.getImage();
    auto levels = pyramid.getLevels();
    auto lastLevel = levels[levels.size() - 1];
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
  // NOTE: this is actually an RGBA8888 in the case of pyramid. See estimateLighting usage of numChannels
  ConstYPlanePixels yImg(rows, cols, bytesPerRow, rowStart);

  // Call the lighting::estimateLighting method using the y-plane image
  response->getGlobal().setExposure(LightingEstimator::estimateLighting(yImg, taskQueue, threadPool, numChannels));
  response->getGlobal().setTemperature(6500);
}

}  // namespace c8
