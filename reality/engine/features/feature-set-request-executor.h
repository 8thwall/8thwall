// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Alvin Portillo (alvin@8thwall.com)
//
// Executor which extract feature point data and insert into the reality response.

#pragma once

#include <capnp/message.h>

#include "c8/time/now.h"
#include "c8/vector.h"
#include "reality/engine/api/reality.capnp.h"
#include "reality/engine/api/response/features.capnp.h"
#include "reality/engine/tracking/tracker.h"

namespace c8 {

class FeatureSetRequestExecutor {
public:
  // Default constructor.
  FeatureSetRequestExecutor() : hashSeed_(static_cast<uint32_t>(nowMicros())) {}

  // Default move constructors.
  FeatureSetRequestExecutor(FeatureSetRequestExecutor &&) = default;
  FeatureSetRequestExecutor &operator=(FeatureSetRequestExecutor &&) = default;

  // Disallow copying.
  FeatureSetRequestExecutor(const FeatureSetRequestExecutor &) = delete;
  FeatureSetRequestExecutor &operator=(const FeatureSetRequestExecutor &) = delete;

  // Main method to execute a request.
  void execute(
    const RealityRequest::Reader &request,
    const ResponsePose::Reader &computedPose,
    Tracker *tracker,
    ResponseFeatureSet::Builder *response) const;

private:
  uint32_t hashSeed_;
};

}  // namespace c8
