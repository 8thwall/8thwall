// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include <list>
#include <memory>
#include <mutex>
#include <string>

#include "c8/protolog/api/log-request.capnp.h"
#include "c8/protolog/api/remote-request.capnp.h"
#include "c8/protolog/remote-disk-logger.h"
#include "c8/string.h"
#include "c8/vector.h"

namespace c8 {

class RemoteDiskRecorder {
public:
  void stream(
    const RealityRequest::Reader &request,
    const RealityResponse::Reader &response,
    const XrRemoteApp::Reader &xrRemote);
  void stop();
  bool isLogging();
  void logToDisk(int numFrames, int fd);
  void setEncodeJpg(bool encodeJpg) { encodeJpg_ = encodeJpg; }
  size_t framesLogged() const { return msgQueue_.size(); }

  // Default constructor.
  RemoteDiskRecorder() = default;
  ~RemoteDiskRecorder();

  // Disallow moving.
  RemoteDiskRecorder(RemoteDiskRecorder &&) = delete;
  RemoteDiskRecorder &operator=(RemoteDiskRecorder &&) = delete;

  // Disallow copying.
  RemoteDiskRecorder(const RemoteDiskRecorder &) = delete;
  RemoteDiskRecorder &operator=(const RemoteDiskRecorder &) = delete;

private:
  void flush();
  void doStop();
  std::mutex implLock_;
  std::unique_ptr<RemoteDiskLogger> remoteDiskLogger_;
  Vector<std::unique_ptr<MutableRootMessage<XrRemoteRequest>>> msgQueue_;
  int numFrames_ = 0;
  bool encodeJpg_ = false;
};

}  // namespace c8
