// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "remote-disk-recorder.h",
  };
  deps = {
    ":reality-request-expand-image",
    "//c8:c8-log",
    "//c8:c8-log-proto",
    "//c8/io:capnp-messages",
    "//c8:vector",
    "//c8/protolog:remote-disk-logger",
    "//c8/protolog/api:log-request.capnp-cc",
    "//c8/protolog/api:remote-request.capnp-cc",
  };
}
cc_end(0xfc8cf484);

#include "reality/engine/logging/remote-disk-recorder.h"

#include <memory>
#include <queue>
#include <chrono>
#include <thread>
#include "c8/c8-log-proto.h"
#include "c8/c8-log.h"
#include "c8/io/capnp-messages.h"
#include "c8/string.h"
#include "c8/vector.h"
#include "c8/protolog/remote-disk-logger.h"
#include "reality/engine/logging/reality-request-expand-image.h"

using namespace std::chrono_literals;

namespace c8 {

RemoteDiskRecorder::~RemoteDiskRecorder() { stop(); }

void RemoteDiskRecorder::stream(
  const RealityRequest::Reader &request,
  const RealityResponse::Reader &response,
  const XrRemoteApp::Reader &xrRemote) {
  std::lock_guard<std::mutex> lock(implLock_);
  if (remoteDiskLogger_ == nullptr) {
    msgQueue_.clear();
    return;
  }

  std::unique_ptr<MutableRootMessage<XrRemoteRequest>> msg(
    new MutableRootMessage<XrRemoteRequest>());
  msgQueue_.push_back(std::move(msg));

  XrRemoteRequest::Builder record = msgQueue_.back()->builder();
  record.getRealityEngine().setRequest(request);
  record.getRealityEngine().setResponse(response);
  record.setXrRemote(xrRemote);
  if (encodeJpg_) {
    expandImagePtrsToJpg(request, record.getRealityEngine().getRequest());
  } else {
    expandImagePtrsToImages(request, record.getRealityEngine().getRequest());
  }

  if (msgQueue_.size() == numFrames_) {
    flush();
    doStop();
  }
}

void RemoteDiskRecorder::flush() {
  C8Log("[remote-disk-recorder] Flushing %d messages", msgQueue_.size());
  for (int i = 0; i < msgQueue_.size(); ++i) {
    MutableRootMessage<c8::RemoteServiceRequest> multimsg;
    RemoteServiceRequest::Builder multirequest = multimsg.builder();
    multirequest.initRecords(1);
    multirequest.getRecords().setWithCaveats(0, msgQueue_[i]->builder());
    remoteDiskLogger_->log(multirequest);
    while(remoteDiskLogger_->hasPendingRequests()) {
      std::this_thread::sleep_for(500us);
    }
  }
  C8Log("[remote-disk-recorder] %s", "Waiting for writing everything");
  while (!remoteDiskLogger_->hasWrittenEverything()) {
    std::this_thread::sleep_for(500us);
  }
  C8Log("[remote-disk-recorder] %s", "Done flushing.");
}

void RemoteDiskRecorder::stop() {
  std::lock_guard<std::mutex> lock(implLock_);
  doStop();
}

void RemoteDiskRecorder::doStop() {
  C8Log("[remote-disk-recorder] %s", "Do stop.");
  remoteDiskLogger_.reset();
  msgQueue_.clear();
  numFrames_ = 0;
}

bool RemoteDiskRecorder::isLogging() {
  // C8Log("[remote-disk-recorder] %s", "RemoteDiskRecorder::isLogging");
  std::lock_guard<std::mutex> lock(implLock_);
  if (remoteDiskLogger_ == nullptr) {
    return false;
  }
  return remoteDiskLogger_->getFramesRemaining() > 0;
}

void RemoteDiskRecorder::logToDisk(int numFrames, int fd) {
  C8Log("[remote-disk-recorder] %s frames=%d fd=%d", "RemoteDiskRecorder::logToDisk", numFrames, fd);
  if (remoteDiskLogger_ != nullptr) {
    stop();
  }
  std::lock_guard<std::mutex> lock(implLock_);
  remoteDiskLogger_.reset(new RemoteDiskLogger());
  remoteDiskLogger_->logToDisk(numFrames, fd);
  numFrames_ = numFrames;
}

};  // namespace c8
