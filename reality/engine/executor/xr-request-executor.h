// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)
//
// The main executor for reality engine code. This class and all of the classes below it are
// stateless. State is exlplicitly maintained and managed by the XREngine, which passes state
// explicitly through the execute method.

#pragma once

#include "c8/random-numbers.h"
#include "reality/engine/api/reality.capnp.h"
#include "reality/engine/camera/camera-request-executor.h"
#include "reality/engine/executor/proto/state.capnp.h"
#include "reality/engine/features/feature-set-request-executor.h"
#include "reality/engine/imagedetection/detection-image.h"
#include "reality/engine/imagedetection/image-detection-request-executor.h"
#include "reality/engine/lighting/lighting-request-executor.h"
#include "reality/engine/pose/pose-request-executor.h"
#include "reality/engine/sensortest/sensortest-request-executor.h"
#include "reality/engine/tracking/tracker.h"

namespace c8 {

class XRRequestExecutor {
public:
  // Default constructor.
  XRRequestExecutor() = default;

  // Default move constructors.
  XRRequestExecutor(XRRequestExecutor &&) = default;
  XRRequestExecutor &operator=(XRRequestExecutor &&) = default;

  // Disallow copying.
  XRRequestExecutor(const XRRequestExecutor &) = delete;
  XRRequestExecutor &operator=(const XRRequestExecutor &) = delete;

  // Main method to execute a request.
  void execute(
    const InternalRealityRequest::Reader &request,
    RealityResponse::Builder *response,
    Tracker *tracker,
    DetectionImageMap *targets,
    RandomNumbers *random,
    TaskQueue *taskQueue,
    ThreadPool *threadPool) const;

  // Update the configuration parameters of the executor.
  void configure(XRConfiguration::Reader config);

  const FeatureSetRequestExecutor &featureSetExecutor() const { return featureSetExecutor_; }

private:
  CameraRequestExecutor cameraExecutor_;
  FeatureSetRequestExecutor featureSetExecutor_;
  ImageDetectionRequestExecutor imageDetectionExecutor_;
  LightingRequestExecutor lightingExecutor_;
  PoseRequestExecutor poseExecutor_;
  SensorTestRequestExecutor sensorTestExecutor_;
};

}  // namespace c8
