// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)
//
// Internal driver for the publicly released 8th Wall Reality Eninge, including maintaining all
// needed state. This manages calls to the executor, which is stateless.
//
// This is the main internal entry point for the public facing reality engine. External clients and
// first party applications should interact with this library indirectly through code in
// //reality/app/xr.

#pragma once

#include <memory>

#include "c8/io/capnp-messages.h"
#include "c8/random-numbers.h"
#include "reality/engine/executor/xr-request-executor.h"
#include "reality/engine/features/feature-detector.h"
#include "reality/engine/hittest/hit-test-performer.h"
#include "reality/engine/imagedetection/detection-image.h"
#include "reality/engine/tracking/tracker.h"

namespace c8 {

class XREngine {
public:
  // Default constructor.
  XREngine();

  // Disallow move constructors, since some member types are immovable.
  XREngine(XREngine &&) = delete;
  XREngine &operator=(XREngine &&) = delete;

  // When logging timers for the engine, reset the logging tree root so that /xr-engine and
  // xr-query are at the top level. This is useful for having consistent timers across outer calling
  // contexts, if that's desired.
  void setResetLoggingTreeRoot(bool reset) { resetLoggingTreeRoot_ = reset; }

  void setDisableSummaryLog(bool disableSummaryLog) { disableSummaryLog_ = disableSummaryLog; }

  // Main method to execute a request.
  void execute(RealityRequest::Reader request, RealityResponse::Builder *response);

  // Query the state of the engine.
  void query(XrQueryRequest::Reader request, XrQueryResponse::Builder *response);

  // Update the configuration parameters of the engine.
  void configure(XRConfiguration::Reader config);

  // Disallow copying.
  XREngine(const XREngine &) = delete;
  XREngine &operator=(const XREngine &) = delete;

  Tracker &tracker() { return tracker_; }

  void setDetectionImages(DetectionImageMap *detectionImages) {
    detectionImages_ = detectionImages;
  }

private:
  XRRequestExecutor executor_;
  Tracker tracker_;
  RandomNumbers random_;
  std::unique_ptr<MutableRootMessage<InternalRealityState>> internalStateMessage_;
  std::unique_ptr<LoggingContext> loopLoggingContext_;

  std::unique_ptr<ThreadPool> threadPool_;
  TaskQueue taskQueue_;

  bool disableSummaryLog_ = false;
  bool resetLoggingTreeRoot_ = false;

  HitTestPerformer hitTestPerformer_;
  ConstRootMessage<CoordinateSystemConfiguration> coordinateConfiguration_;

  std::mutex internalStateLock_;

  DetectionImageMap *detectionImages_ = nullptr;
};

}  // namespace c8
