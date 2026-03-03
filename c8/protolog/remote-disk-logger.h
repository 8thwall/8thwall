// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// C-wrapper functions for client code to communicate with the RemoteService server.

#pragma once

#include <capnp/message.h>
#include <capnp/rpc-twoparty.h>
#include <event2/event.h>
#include <kj/async-io.h>
#include <kj/async-unix.h>
#include <kj/async.h>
#include <kj/common.h>
#include <list>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include "c8/io/capnp-messages.h"
#include "c8/protolog/api/remote-request.capnp.h"
#include "c8/protolog/api/remote-service-interface.capnp.h"
#include "c8/protolog/remote-service-discovery.h"
#include "c8/string.h"
#include "c8/vector.h"

namespace c8 {

namespace {
size_t constexpr MAX_MESSAGE_IN_QUEUE = 100;
}

template <typename T>
using MessageQueue = std::deque<std::unique_ptr<MutableRootMessage<T>>>;

template <typename T>
struct DiskLoggerQueuePair {
  MessageQueue<T> *fillingQueue;
  MessageQueue<T> *drainingQueue;

  void swap() {
    std::lock_guard<std::mutex> lock(fillGuard);
    std::swap(fillingQueue, drainingQueue);
  }

  // Swap is called from the draining thread and doesn't need to be locked with draining operations.
  void enqueue(const typename T::Reader &request) {
    std::unique_ptr<MutableRootMessage<T>> msg(new MutableRootMessage<T>(request));
    {
      std::lock_guard<std::mutex> lock(fillGuard);
      // TODO(nb): Find a better way communicate that pushing a message onto the queue failed.
      if (fillingQueue->size() < MAX_MESSAGE_IN_QUEUE) {
        fillingQueue->push_back(std::move(msg));
      }
    }
  }

private:
  std::mutex fillGuard;  // TODO(nb): lockless for efficiency
};

using RequestQueuePair = DiskLoggerQueuePair<RemoteServiceRequest>;
using ResponseQueuePair = DiskLoggerQueuePair<RemoteServiceResponse>;

class RemoteDiskLogger {
public:
  RemoteDiskLogger();
  ~RemoteDiskLogger();

  // Log numFrames to disk.
  void logToDisk(int numFrames, int fd);

  // Log request
  void log(const RemoteServiceRequest::Reader &request);

  // Check whether there any unsent outgoing requests.
  bool hasPendingRequests();
  bool hasWrittenEverything();

  // Returns number of frames remaining to log to disk.
  int getFramesRemaining() { return framesRemaining_; }

private:
  int framesRemaining_ = 0;
  evutil_socket_t writeFd_ = 0;
  MessageQueue<RemoteServiceRequest> requestQueueA, requestQueueB;
  MessageQueue<RemoteServiceResponse> responseQueueA, responseQueueB;
  RequestQueuePair queues_;
  ResponseQueuePair responseQueues_;

  std::thread thrd;

  void drainFdToDisk(
    evutil_socket_t readFd, int writeFd, RequestQueuePair &queues, std::function<void()> terminate);
};

}  // namespace c8
