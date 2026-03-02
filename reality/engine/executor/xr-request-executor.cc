// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "xr-request-executor.h",
  };
  deps = {
    "//c8:c8-log",
    "//c8:random-numbers",
    "//c8/camera:device-infos",
    "//c8/geometry:intrinsics",
    "//c8/stats:scope-timer",
    "//c8/time:now",
    "//reality/engine/api:reality.capnp-cc",
    "//reality/engine/camera:camera-request-executor",
    "//reality/engine/executor/proto:state.capnp-cc",
    "//reality/engine/features:feature-set-request-executor",
    "//reality/engine/imagedetection:detection-image",
    "//reality/engine/imagedetection:image-detection-request-executor",
    "//reality/engine/lighting:lighting-request-executor",
    "//reality/engine/pose:pose-request-executor",
    "//reality/engine/sensortest:sensortest-request-executor",
    "//reality/engine/tracking:tracker",
  };
}
cc_end(0x3cc1b370);

#include <capnp/pretty-print.h>

#include "c8/c8-log.h"
#include "c8/camera/device-infos.h"
#include "c8/geometry/intrinsics.h"
#include "c8/stats/scope-timer.h"
#include "c8/time/now.h"
#include "reality/engine/executor/xr-request-executor.h"
#include "reality/engine/pose/pose-request-executor.h"
#include "reality/engine/sensortest/sensortest-request-executor.h"

namespace c8 {

void XRRequestExecutor::execute(
  const InternalRealityRequest::Reader &request,
  RealityResponse::Builder *response,
  Tracker *tracker,
  DetectionImageMap *targets,
  RandomNumbers *random,
  TaskQueue *taskQueue,
  ThreadPool *threadPool) const {
  auto eventTimeMicros = nowMicros();
  auto internalRequestMask = request.getRequest().getMask();
  auto xrRequestMask = request.getRequest().getXRConfiguration().getMask();

  // Run requested executors.
  if (internalRequestMask.getSensorTest()) {
    response->setSensorTest(capnp::defaultValue<ResponseSensorTest>());
    ResponseSensorTest::Builder sensorTestBuilder = response->getSensorTest();
    sensorTestExecutor_.execute(request.getRequest().getSensors(), &sensorTestBuilder);
  }

  // Pose depends on the output of features.
  // TODO(nb): Define an execution DAG structure and schedule executors accordingly.
  if (internalRequestMask.getPose() || xrRequestMask.getCamera()) {
    ResponsePose::Builder poseBuilder = response->getPose();
    poseExecutor_.execute(
      request.getState().getLastCall().getResponse().getEventId().getEventTimeMicros(),
      eventTimeMicros,
      DeviceInfos::getDeviceModel(request.getRequest().getDeviceInfo()),
      request.getRequest().getDeviceInfo().getManufacturer(),
      Intrinsics::getProcessingIntrinsics(request.getRequest()),
      request.getRequest().getFlags(),
      request.getState().getLastCall().getRequest().getSensors(),
      request.getState().getLastCall().getResponse(),
      request.getRequest().getAppContext(),
      request.getRequest().getSensors(),
      request.getRequest().getDebugData(),
      response->getFeatures().asReader(),
      &poseBuilder,
      tracker,
      targets,
      random);
  }
  if (xrRequestMask.getLighting()) {
    // lighting
    auto lightingBuilder = response->getXRResponse().getLighting();
    lightingExecutor_.execute(
      request.getRequest().getSensors(), &lightingBuilder, taskQueue, threadPool);
  }

  // Camera depends on the output of pose.
  // TODO(nb): Define an execution DAG structure and schedule executors accordingly.
  if (xrRequestMask.getCamera()) {
    // camera
    auto cameraBuilder = response->getXRResponse().getCamera();
    cameraExecutor_.execute(
      request.getRequest().getSensors(),
      request.getRequest().getXRConfiguration(),
      response->getPose(),
      request.getRequest().getDeviceInfo(),
      &cameraBuilder);
  }

  auto poseInitializing = response->getPose().getTrackingState().getReason()
    == XrTrackingState::XrTrackingStatusReason::INITIALIZING;

  if (xrRequestMask.getFeatureSet()) {
    auto featureSetBuilder = response->getFeatureSet();
    featureSetExecutor_.execute(
      request.getRequest(), response->getPose().asReader(), tracker, &featureSetBuilder);
  }

  auto detectionBuilder = response->getXRResponse().getDetection();
  {
    auto engineExportBuilder = response->getEngineExport();
    auto lastDetection =
      request.getState().getLastCall().getResponse().getXRResponse().getDetection();
    imageDetectionExecutor_.execute(
      tracker,
      targets,
      random,
      lastDetection,
      request.getRequest().getSensors(),
      response->getPose().asReader(),
      &detectionBuilder,
      &engineExportBuilder);
  }

  // Clear fields in the response that weren't requested but which may have been set by
  // computing intermediate data.
  if (!internalRequestMask.getPose()) {
    // Needed for state.
    // response->setPose(capnp::defaultValue<ResponsePose>());
  }
  if (!internalRequestMask.getSensorTest()) {
    // Allow sensortest queries.
    // response->setSensorTest(capnp::defaultValue<ResponseSensorTest>());
  }
  if (!internalRequestMask.getFeatures()) {
    response->setFeatures(capnp::defaultValue<DeprecatedResponseFeatures>());
  }

  // Create an overall response OK status and event id.
  response->getStatus();
  response->getEventId().setEventTimeMicros(eventTimeMicros);
}

void XRRequestExecutor::configure(XRConfiguration::Reader config) {
  if (config.hasImageDetection()) {
    imageDetectionExecutor_.configure(config);
  }
}

}  // namespace c8
