// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)
//
//

#pragma once

#include <random>

#include "c8/camera/device-infos.h"
#include "c8/random-numbers.h"
#include "capnp/message.h"
#include "reality/engine/api/request/app.capnp.h"
#include "reality/engine/api/request/flags.capnp.h"
#include "reality/engine/api/request/sensor.capnp.h"
#include "reality/engine/api/response/features.capnp.h"
#include "reality/engine/api/response/pose.capnp.h"
#include "reality/engine/tracking/tracker.h"

namespace c8 {

class PoseRequestExecutor {
public:
  // Default constructor.
  PoseRequestExecutor() = default;

  // Default move constructors.
  PoseRequestExecutor(PoseRequestExecutor &&) = default;
  PoseRequestExecutor &operator=(PoseRequestExecutor &&) = default;

  // Main method to execute a request.
  void execute(
    const long lastEventTimeMicros,
    const long eventTimeMicros,
    DeviceInfos::DeviceModel deviceModel,
    const String &deviceManufacturer,
    c8_PixelPinholeCameraModel intrinsic,
    const RequestFlags::Reader &requestFlags,
    const RequestSensor::Reader &lastRequest,
    const RealityResponse::Reader &lastResponse,
    const AppContext::Reader &appContext,
    const RequestSensor::Reader &requestSensor,
    const capnp::List<DebugData>::Reader &debugData,
    const DeprecatedResponseFeatures::Reader &responseFeatures,
    ResponsePose::Builder *response,
    Tracker *tracker,
    DetectionImageMap *targets,
    RandomNumbers *random) const;

  // Disallow copying.
  PoseRequestExecutor(const PoseRequestExecutor &) = delete;
  PoseRequestExecutor &operator=(const PoseRequestExecutor &) = delete;

private:
};

}  // namespace c8
