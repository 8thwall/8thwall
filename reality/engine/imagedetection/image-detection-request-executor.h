// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "reality/engine/api/reality.capnp.h"
#include "reality/engine/imagedetection/detection-image.h"
#include "reality/engine/tracking/tracker.h"

namespace c8 {

class ImageDetectionRequestExecutor {
public:
  ImageDetectionRequestExecutor() = default;

  // Default move constructors.
  ImageDetectionRequestExecutor(ImageDetectionRequestExecutor &&) = default;
  ImageDetectionRequestExecutor &operator=(ImageDetectionRequestExecutor &&) = default;

  // Main method to execute a request.
  void execute(
    Tracker *tracker,
    DetectionImageMap *targets,
    RandomNumbers *random,
    const ResponseDetection::Reader &lastResponse,
    RequestSensor::Reader sensor,
    ResponsePose::Reader computedPose,
    ResponseDetection::Builder *response,
    EngineExport::Builder *engineExport) const;

  // Update the configuration parameters of the image detection executor.
  void configure(XRConfiguration::Reader config);

  // Disallow copying.
  ImageDetectionRequestExecutor(const ImageDetectionRequestExecutor &) = delete;
  ImageDetectionRequestExecutor &operator=(const ImageDetectionRequestExecutor &) = delete;

private:
  // Update the configuration parameters of the C8 image detection executor.
  void configureC8(XRConfiguration::Reader config);

  void fillDetectedImagesC8(
    Tracker *tracker,
    DetectionImageMap *targets,
    RandomNumbers *random,
    const ResponseDetection::Reader &lastResponse,
    RequestSensor::Reader sensor,
    ResponsePose::Reader computedPose,
    ResponseDetection::Builder *response,
    EngineExport::Builder *engineExport) const;
};

}  // namespace c8
