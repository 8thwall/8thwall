// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  visibility = {
    "//apps/client/exploratory/headsets:__subpackages__",
    "//apps/client/exploratory/hellojsxr:__subpackages__",
    "//apps/client/internalqa:__subpackages__",
    "//reality/engine/executor:__subpackages__",
    "//reality/app:__subpackages__",
    "//reality/quality:__subpackages__",
  };
  hdrs = {
    "xr-engine.h",
  };
  deps = {
    ":xr-request-executor",
    "//c8:c8-log",
    "//c8:c8-log-proto",
    "//c8:parameter-data",
    "//c8:random-numbers",
    "//c8/io:capnp-messages",
    "//c8/stats:scope-timer",
    "//reality/engine/api:reality.capnp-cc",
    "//reality/engine/executor/proto:state.capnp-cc",
    "//reality/engine/geometry:orientation",
    "//reality/engine/hittest:hit-test-performer",
    "//reality/engine/imagedetection:detection-image",
    "//reality/engine/tracking:tracker",
  };
}
cc_end(0x2cd10db6);

#include "c8/c8-log-proto.h"
#include "c8/c8-log.h"
#include "c8/io/capnp-messages.h"
#include "c8/parameter-data.h"
#include "c8/stats/scope-timer.h"
#include "reality/engine/executor/proto/state.capnp.h"
#include "reality/engine/executor/xr-engine.h"
#include "reality/engine/geometry/orientation.h"

using ConstInternalRealityState = c8::ConstRootMessage<c8::InternalRealityState>;
using MutableInternalRealityRequest = c8::MutableRootMessage<c8::InternalRealityRequest>;
using MutableInternalRealityState = c8::MutableRootMessage<c8::InternalRealityState>;
using MutableInternalRealityExecutionContext =
  c8::MutableRootMessage<c8::InternalRealityExecutionContext>;
using MutableXrHitTestRequest = c8::MutableRootMessage<c8::XrHitTestRequest>;

// TODO(nb): Remove this and replace with a better logging framework.
namespace {
int64_t frameCounter = 0;
}

namespace c8 {

namespace {

AppContext::RealityTextureRotation textureRotation(AppContext::Reader requestAppContext) {
  if (
    requestAppContext.getRealityTextureRotation()
    != AppContext::RealityTextureRotation::UNSPECIFIED) {
    return requestAppContext.getRealityTextureRotation();
  }
  switch (requestAppContext.getDeviceOrientation()) {
    case AppContext::DeviceOrientation::PORTRAIT:
      return AppContext::RealityTextureRotation::R0;
    case AppContext::DeviceOrientation::LANDSCAPE_LEFT:
      return AppContext::RealityTextureRotation::R90;
    case AppContext::DeviceOrientation::PORTRAIT_UPSIDE_DOWN:
      return AppContext::RealityTextureRotation::R180;
    case AppContext::DeviceOrientation::LANDSCAPE_RIGHT:
      return AppContext::RealityTextureRotation::R270;
    default:
      return AppContext::RealityTextureRotation::UNSPECIFIED;
  }
}

}  // namespace

// Default constructor.
XREngine::XREngine()
    : internalStateMessage_(new MutableInternalRealityState()),
      threadPool_(new ThreadPool(std::max(2u, std::thread::hardware_concurrency()) - 1u)) {}

void XREngine::query(XrQueryRequest::Reader request, XrQueryResponse::Builder *response) {
  ScopeTimer rt("xr-query", resetLoggingTreeRoot_);

  if (!request.hasHitTest()) {
    // Check that the query request has at least one query request type.
    return;
  }

  ConstInternalRealityState stateMessage;
  {
    // Copy over internal state to use when executing each query request.
    std::lock_guard<std::mutex> lock(internalStateLock_);
    stateMessage = ConstInternalRealityState(*internalStateMessage_);
  }

  if (!stateMessage.reader().hasLastCall()) {
    // We have no previous data to query from, so we will drop this request.
    return;
  }

  auto lastResponse = stateMessage.reader().getLastCall().getResponse();

  // Rotate hit test request based on orientation
  MutableXrHitTestRequest message(request.getHitTest());
  auto hitTestRequest = message.builder();

  auto inputX = request.getHitTest().getX();
  auto inputY = request.getHitTest().getY();
  if (coordinateConfiguration_.reader().getMirroredDisplay()) {
    inputX = 1 - inputX;
    hitTestRequest.setX(inputX);
  }
  switch (lastResponse.getAppContext().getDeviceOrientation()) {
    case AppContext::DeviceOrientation::LANDSCAPE_RIGHT:
      hitTestRequest.setX(inputY);
      hitTestRequest.setY(1 - inputX);
      break;
    case AppContext::DeviceOrientation::PORTRAIT_UPSIDE_DOWN:
      hitTestRequest.setX(1 - inputX);
      hitTestRequest.setY(1 - inputY);
      break;
    case AppContext::DeviceOrientation::LANDSCAPE_LEFT:
      hitTestRequest.setX(1 - inputY);
      hitTestRequest.setY(inputX);
      break;
    default:
      // No correction needed.
      break;
  }

  auto featuresForHitTest = lastResponse.getFeatureSet();

  MutableRootMessage<ResponseFeatureSet> calcFeatures;
  auto lastRequest = stateMessage.reader().getLastCall().getRequest();

  // If features were not previously requested, compute them now.
  if (!lastRequest.getXRConfiguration().getMask().getFeatureSet()) {
    auto calcFeaturesBuilder = calcFeatures.builder();
    executor_.featureSetExecutor().execute(
      lastRequest, lastResponse.getPose(), &tracker_, &calcFeaturesBuilder);
    featuresForHitTest = calcFeatures.reader();
  }

  // Execute query.
  auto hitTestResponse = response->getHitTest();
  hitTestPerformer_.performHitTest(
    lastResponse.getXRResponse().getCamera(),
    featuresForHitTest,
    lastResponse.getXRResponse().getSurfaces(),
    hitTestRequest.asReader(),
    &hitTestResponse);

  rewriteCoordinateSystemPostprocess(coordinateConfiguration_.reader(), *response);
}

void XREngine::configure(XRConfiguration::Reader config) {
  executor_.configure(config);
  if (config.hasCoordinateConfiguration()) {
    coordinateConfiguration_ =
      ConstRootMessage<CoordinateSystemConfiguration>(config.getCoordinateConfiguration());
  }
  if (config.hasMask()) {
    tracker_.setImageTrackingDisabled(config.getMask().getDisableImageTargets());
  }
}

void XREngine::execute(RealityRequest::Reader request, RealityResponse::Builder *response) {
  ScopeTimer rt("xr-engine", resetLoggingTreeRoot_);
  MutableInternalRealityRequest internalRequestMessage;

  auto requestBuilder = internalRequestMessage.builder();
  requestBuilder.setRequest(request);

  MutableInternalRealityExecutionContext executionContextMessage;
  {
    std::lock_guard<std::mutex> stateReadLock(internalStateLock_);
    // Make a mem copy of the execution context, so we can carry it over to the next state
    // message.
    executionContextMessage.setRoot(internalStateMessage_->reader().getExecutionContext());
    // Copy over previous state to the new request.
    requestBuilder.setState(internalStateMessage_->reader());
  }

  deviceToPortraitOrientationPreprocess(
    request.getAppContext().getDeviceOrientation(), requestBuilder.getRequest());

  executor_.execute(
    requestBuilder.asReader(),
    response,
    &tracker_,
    detectionImages_,
    &random_,
    &taskQueue_,
    threadPool_.get());

  response->setAppContext(request.getAppContext());

  {
    // Copy raw internal response to internal state before correcting for application context
    // (e.g. device pose).
    auto nextState = new MutableInternalRealityState();
    auto nextStateBuilder = nextState->builder();
    nextStateBuilder.getLastCall().setRequest(request);
    nextStateBuilder.getLastCall().setResponse(*response);

    // Update the execution contexted copied from the last state, and add it to the next state.
    auto updatedContext = executionContextMessage.builder();
    updatedContext.setFrameNum(updatedContext.getFrameNum() + 1);
    nextStateBuilder.setExecutionContext(updatedContext);

    // store the system configuration
    nextStateBuilder.setCoordinateConfiguration(coordinateConfiguration_.reader());

    internalStateMessage_.reset(nextState);
  }

  {
    // Correct for application context (e.g. device pose) after copying to internal state.
    portraitToDeviceOrientationPostprocess(
      request.getAppContext().getDeviceOrientation(), *response);

    // Set the texture rotation automatically if it wasn't specified on the request.
    response->getAppContext().setRealityTextureRotation(textureRotation(request.getAppContext()));

    // Recenter, scale and change axes if requested.
    rewriteCoordinateSystemPostprocess(coordinateConfiguration_.reader(), *response);
  }

  if (loopLoggingContext_.get() != nullptr) {
    loopLoggingContext_->markCompletionTimepoint();
    ScopeTimer::summarize(*loopLoggingContext_);
  }
  loopLoggingContext_.reset(LoggingContext::createRootLoggingTreePtr("reality-loop").release());

  // TODO(nb): Remove this and replace with a better logging framework.
  if (!disableSummaryLog_ && frameCounter++ % 1000 == 0) {
    ScopeTimer::logDetailedSummary();
  }
}

}  // namespace c8
