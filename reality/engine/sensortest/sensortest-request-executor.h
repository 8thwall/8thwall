// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)
//
//

#pragma once

#include <random>
#include "capnp/message.h"
#include "reality/engine/api/request/sensor.capnp.h"
#include "reality/engine/api/response/sensor-test.capnp.h"

namespace c8 {

class SensorTestRequestExecutor {
 public:
  // Default constructor.
  SensorTestRequestExecutor() = default;

  // Default move constructors.
  SensorTestRequestExecutor(SensorTestRequestExecutor &&) = default;
  SensorTestRequestExecutor &operator=(SensorTestRequestExecutor &&) = default;

  // Main method to execute a request.
  void execute(
    const RequestSensor::Reader &sensor,
    ResponseSensorTest::Builder *response) const;

  // Disallow copying.
  SensorTestRequestExecutor(const SensorTestRequestExecutor &) = delete;
  SensorTestRequestExecutor &operator=(const SensorTestRequestExecutor &) = delete;

 private:
};

}  // namespace c8
