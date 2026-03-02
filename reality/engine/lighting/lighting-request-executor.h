// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)
//
//

#pragma once

#include "c8/task-queue.h"
#include "c8/thread-pool.h"
#include "reality/engine/api/reality.capnp.h"
#include "reality/engine/api/request/sensor.capnp.h"

namespace c8 {

class LightingRequestExecutor {
public:
  // Default constructor.
  LightingRequestExecutor() = default;

  // Default move constructors.
  LightingRequestExecutor(LightingRequestExecutor &&) = default;
  LightingRequestExecutor &operator=(LightingRequestExecutor &&) = default;

  // Main method to execute a request.
  void execute(
    const RequestSensor::Reader &sensor,
    ResponseLighting::Builder *response,
    TaskQueue *taskQueue,
    ThreadPool *threadPool) const;

  // Disallow copying.
  LightingRequestExecutor(const LightingRequestExecutor &) = delete;
  LightingRequestExecutor &operator=(const LightingRequestExecutor &) = delete;

private:
  static constexpr int PYRAMID_NUM_CHANNELS = 4;
};

}  // namespace c8
