// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)
//
//

#pragma once

#include "c8/geometry/pixel-pinhole-camera-model.h"
#include "reality/engine/api/device/info.capnp.h"
#include "reality/engine/api/reality.capnp.h"
#include "reality/engine/api/request/sensor.capnp.h"

namespace c8 {

c8_PixelPinholeCameraModel toPinholeModelStruct(const capnp::List<float>::Reader &p, int w, int h);

class CameraRequestExecutor {
public:
  // Default constructor.
  CameraRequestExecutor() = default;

  // Default move constructors.
  CameraRequestExecutor(CameraRequestExecutor &&) = default;
  CameraRequestExecutor &operator=(CameraRequestExecutor &&) = default;

  // Main method to execute a request.
  void execute(
    const RequestSensor::Reader &sensor,
    const XRConfiguration::Reader &config,
    const ResponsePose::Reader &computedPose,
    const DeviceInfo::Reader &deviceInfo,
    ResponseCamera::Builder *response) const;

  // Disallow copying.
  CameraRequestExecutor(const CameraRequestExecutor &) = delete;
  CameraRequestExecutor &operator=(const CameraRequestExecutor &) = delete;

private:
};

}  // namespace c8
