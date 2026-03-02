// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Alvin Portillo (alvin@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "xr-log-preparer.h",
  };
  deps = {
    "//c8:c8-log",
    "//c8:c8-log-proto",
    "//c8/io:capnp-messages",
    "//c8/protolog/api:log-request.capnp-cc",
    "//c8/stats:latency-summarizer",
    "//c8/stats:logging-context",
    "//reality/engine/api:reality.capnp-cc",
    "//reality/engine/api/device:info.capnp-cc",
    "@capnproto//:json",
    "@zlib//:zlib",
  };
}
cc_end(0xbe9df335);

#include <capnp/serialize-packed.h>
#include <kj/io.h>
#include <zlib.h>

#include "c8/c8-log-proto.h"
#include "c8/c8-log.h"
#include "c8/io/capnp-messages.h"
#include "c8/protolog/api/log-request.capnp.h"
#include "c8/release-config.h"
#include "c8/time/now.h"
#include "reality/engine/logging/xr-log-preparer.h"

using MutableLogServiceRequest = c8::MutableRootMessage<c8::LogServiceRequest>;

namespace c8 {

namespace {

static constexpr uint32_t INITIAL_LOG_BUFFER_SIZE = 2500;

void exportEngineHeader(RealityEngineLogRecordHeader::Builder *builder) {
  if (!builder->hasEngineVersion()) {
    // Don't overwrite engine version if it already got set elsewhere.
    auto versionString = releaseConfigBuildId();
    builder->setEngineVersion(versionString);
  }
  // TODO(alvin): Maybe set session id, frame id, etc.
}

}  // namespace

int64_t Timer::getNowMicros() const { return nowMicros(); }

void Timer::on() {
  if (isOn()) {
    return;
  }
  if (isPaused()) {
    // Reset our pause time to start again now that we're turning on. This works because in off()
    // we recorded how long we had been paused for. And we don't count time while we're off. Instead
    // we just start counting again now that we're on.
    pauseTime_ = getNowMicros();
  }
  onTime_ = getNowMicros();
}

int64_t Timer::current() const {
  auto nowMicros = getNowMicros();
  auto onMicros = isOn() ? (nowMicros - onTime_) : 0;
  // We only count paused time if we are on. If we are off then currentPauseTotal_ will hold the
  // pause time (it gets set in off()).
  auto pauseMicros = isPaused() && isOn() ? (nowMicros - pauseTime_) : 0;
  return onMicros + total_ - currentPauseTotal_ - pauseMicros;
}

void Timer::off() {
  if (isOff()) {
    return;
  }
  if (isPaused()) {
    // Save how long we've been paused for when turning off.
    // Don't reset pauseTime_ because we want to remember that we're paused so that in on() we can
    // set pauseTime_ to the current time and start counting the pause again.
    currentPauseTotal_ += getNowMicros() - pauseTime_;
  }
  total_ = isOn() ? (getNowMicros() - onTime_) : 0;
  onTime_ = 0;
}

void Timer::pause() { pauseTime_ = getNowMicros(); }

void Timer::unpause() {
  if (!isPaused()) {
    return;
  }
  if (isOn()) {
    // If we unpause while off do not increment the total pause time. This is because when we turn
    // off we save the pause time but do not reset pauseTime_. See Timer::off() for more details.
    currentPauseTotal_ += getNowMicros() - pauseTime_;
  }
  pauseTime_ = 0;
}

void Timer::reset() {
  total_ = 0;
  currentPauseTotal_ = 0;
  auto nowMicros = getNowMicros();
  if (isOn()) {
    onTime_ = nowMicros;
  }
  if (isPaused()) {
    pauseTime_ = nowMicros;
  }
}

const Vector<String> &metrics() {
  static Vector<String> metrics = {
    "onPage",
    "camera",
    "AR",
    "VR",
    "desktop3d",
    "worldEffects",
    "worldEffectsAbsolute",
    "worldEffectsResponsive",
    "faceEffects",
    "imageTargets",
    "curvyImageTargets",
    "vps",
    "vpsTimeToNode",
    "vpsNodeLocalized",
    "vpsTimeToWayspotAnchor",
    "vpsWayspotAnchorLocalized",
    "vpsTimeToFallback"};
  return metrics;
}

void XRSessionStats::initializeTimers() {
  for (const auto &i : metrics()) {
    timing_.insert({i, Timer()});
  }
  timing_["onPage"].on();
}

void XRSessionStats::pauseAll() {
  for (const auto &i : metrics()) {
    if (timing_.count(i) > 0 && !timing_[i].isPaused()) {
      timing_[i].pause();
    }
  }
}
void XRSessionStats::unpauseAll() {
  for (const auto &i : metrics()) {
    if (timing_[i].isPaused()) {
      timing_[i].unpause();
    }
  }
}

void XRSessionStats::stopAll() {
  for (const auto &i : metrics()) {
    if (i != "onPage" && !timing_[i].isOff()) {
      timing_[i].off();
    }
  }
}

void XRSessionStats::reset() {
  facesFoundTotal_ = 0;
  handsFoundTotal_ = 0;
  imageTargetsFoundTotal_ = 0;
  imageTargetsFoundUnique_ = 0;

  wayspotAnchorsFoundTotal_ = 0;
  wayspotAnchorsFoundUnique_ = 0;
  wayspotAnchorsUpdated_ = 0;

  nodesFoundTotal_ = 0;
  nodesFoundUnique_ = 0;
  nodesUpdated_ = 0;

  for (const auto &i : metrics()) {
    timing_[i].reset();
  }
}

void XRSessionStats::incrementFacesFound() { facesFoundTotal_++; }

void XRSessionStats::incrementHandsFound() { handsFoundTotal_++; }

void XRSessionStats::incrementImageTargetsFound() { imageTargetsFoundTotal_++; }
void XRSessionStats::incrementImageTargetsFoundUnique() { imageTargetsFoundUnique_++; }

void XRSessionStats::incrementWayspotAnchorsFound() { wayspotAnchorsFoundTotal_++; }
void XRSessionStats::incrementWayspotAnchorsFoundUnique() { wayspotAnchorsFoundUnique_++; }
void XRSessionStats::incrementWayspotAnchorsUpdated() { wayspotAnchorsUpdated_++; }

void XRSessionStats::incrementNodesFound() { nodesFoundTotal_++; }
void XRSessionStats::incrementNodesFoundUnique() { nodesFoundUnique_++; }
void XRSessionStats::incrementNodesUpdated() { nodesUpdated_++; }

void XRSessionStats::exportRecord(LogRecord::Builder logRecord) {
  auto dwellTime = logRecord.getDwellTime();

  auto &onPage = timer("onPage");
  dwellTime.setPageMillis(onPage.current() / 1000);

  auto &camera = timer("camera");
  dwellTime.setCameraMillis(camera.current() / 1000);

  auto &desktop3d = timer("desktop3d");
  dwellTime.setDesktop3DMillis(desktop3d.current() / 1000);

  auto &worldEffects = timer("worldEffects");
  dwellTime.setWorldEffectMillis(worldEffects.current() / 1000);
  auto &worldEffectsResponsive = timer("worldEffectsResponsive");
  dwellTime.setWorldEffectResponsiveMillis(worldEffectsResponsive.current() / 1000);
  auto &worldEffectsAbsolute = timer("worldEffectsAbsolute");
  dwellTime.setWorldEffectAbsoluteMillis(worldEffectsAbsolute.current() / 1000);

  auto &faceEffects = timer("faceEffects");
  auto faceEffectsTime = dwellTime.getFaceEffectTrackingTime();
  faceEffectsTime.setTotalMillis(faceEffects.current() / 1000);

  auto &AR = timer("AR");
  dwellTime.setArMillis(AR.current() / 1000);

  auto &VR = timer("VR");
  dwellTime.setVrMillis(VR.current() / 1000);

  auto &imageTargets = timer("imageTargets");
  dwellTime.getImageTargetTrackingTime().setTotalMillis(imageTargets.current() / 1000);

  auto &curvyImageTargets = timer("curvyImageTargets");
  dwellTime.getCurvyImageTargetTrackingTime().setTotalMillis(curvyImageTargets.current() / 1000);

  auto vpsTime = dwellTime.getVpsTrackingTime();
  vpsTime.setTotalMillis(timer("vps").current() / 1000);
  vpsTime.setTimeToNodeMillis(timer("vpsTimeToNode").current() / 1000);
  vpsTime.setNodeMillis(timer("vpsNodeLocalized").current() / 1000);
  vpsTime.setTimeToWayspotAnchorMillis(timer("vpsTimeToWayspotAnchor").current() / 1000);
  vpsTime.setWayspotAnchorMillis(timer("vpsWayspotAnchorLocalized").current() / 1000);
  if (timer("vpsTimeToFallback").isOff()) {
    vpsTime.setTimeToFallbackMillis(timer("vpsTimeToFallback").current() / 1000);
  }

  auto eventCounts = logRecord.getEventCounts();

  eventCounts.getFacesFound().setTotal(facesFoundTotal_);
  eventCounts.getImageTargetsFound().setTotal(imageTargetsFoundTotal_);
  eventCounts.getImageTargetsFound().setUnique(imageTargetsFoundUnique_);

  eventCounts.getWayspotAnchorsFound().setUnique(wayspotAnchorsFoundUnique_);
  eventCounts.getWayspotAnchorsFound().setTotal(wayspotAnchorsFoundTotal_);
  eventCounts.getWayspotAnchorsFound().setUpdated(wayspotAnchorsUpdated_);

  eventCounts.getNodesFound().setUnique(nodesFoundUnique_);
  eventCounts.getNodesFound().setTotal(nodesFoundTotal_);
  eventCounts.getNodesFound().setUpdated(nodesUpdated_);

  reset();
}

std::unique_ptr<Vector<uint8_t>> XRLogPreparer::prepareLogForUpload(
  const LogRecordHeader::Reader &logRecordHeader, LatencySummarizer *summarizer) {
  MutableLogServiceRequest logServiceRequestMessage;
  auto logServiceRequestBuilder = logServiceRequestMessage.builder();
  auto requestRecords = logServiceRequestBuilder.initRecords(1);
  auto logRecordBuilder = requestRecords[0];

  logRecordBuilder.setHeader(logRecordHeader);

  auto engineHeaderBuilder = logRecordBuilder.getHeader().getReality();
  exportEngineHeader(&engineHeaderBuilder);

  auto summary = logRecordBuilder.getRealityEngine().getStats().getSummary();
  summarizer->exportSummary(&summary);

  stats_.exportRecord(logRecordBuilder);

  // Serialize capnp struct in packed format.
  kj::VectorOutputStream packedOutputStream(INITIAL_LOG_BUFFER_SIZE);
  capnp::writePackedMessage(packedOutputStream, *(logServiceRequestMessage.message()));

  // Compress the packed capnp struct to further reduce bandwidth overhead when sending to server.
  auto uncompressedBufferSize = packedOutputStream.getArray().size();
  auto compressedBufferSize = compressBound(uncompressedBufferSize);
  auto compressedBytes = std::make_unique<Vector<uint8_t>>(compressedBufferSize);
  compress(
    compressedBytes->data(),
    &compressedBufferSize,
    packedOutputStream.getArray().begin(),
    uncompressedBufferSize);
  compressedBytes->resize(compressedBufferSize);
  return compressedBytes;
}

void XRLogPreparer::startNewLoggingSession() {
  sessionLoggingContext_.reset(LoggingContext::createRootLoggingTreePtr("log-session").release());
}

void XRLogPreparer::endLoggingSession(LatencySummarizer *summarizer) {
  if (sessionLoggingContext_.get() == nullptr) {
    C8Log("[xr-log-preparer] Error: Attempting to end a logging session before one has started.");
    return;
  }

  sessionLoggingContext_->markCompletionTimepoint();
  summarizer->summarize(*sessionLoggingContext_);
}

}  // namespace c8
