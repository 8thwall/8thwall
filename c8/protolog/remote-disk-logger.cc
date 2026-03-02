#include "bzl/inliner/rules.h"

cc_library {
  hdrs = {"remote-disk-logger.h"};
  deps = {
    "//bzl/inliner:rules",
    "//c8:c8-log",
    "//c8:c8-log-proto",
    "//c8:exceptions",
    "//c8:string",
    "//c8:vector",
    "//c8/events:kj-event-listener",
    "//c8/events:lev-event-listener",
    "//c8/io:capnp-messages",
    "//c8/network:dns-service-discovery",
    "//c8/protolog:remote-service-discovery",
    "//c8/protolog/api:remote-service.capnp-cc",
    "@capnproto//:capnp-lib",
    "@capnproto//:kj",
  };
  visibility = {":protolog-pkgs"};
}

#include "c8/protolog/remote-disk-logger.h"

#if defined(_WIN32) || defined(WIN32)
#error "This file cannot be compiled under Windows. This code is only used on mobile."
#endif

#include <capnp/rpc-twoparty.h>
#include <capnp/serialize.h>
#include <kj/async-io.h>
#include <kj/async-unix.h>
#include <kj/async.h>
#include <kj/common.h>

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <algorithm>
#include <ctime>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <thread>

#include "c8/c8-log-proto.h"
#include "c8/c8-log.h"
#include "c8/events/kj-event-listener.h"
#include "c8/events/lev-event-listener.h"
#include "c8/exceptions.h"
#include "c8/network/dns-service-discovery.h"
#include "c8/protolog/api/remote-request.capnp.h"
#include "c8/protolog/api/remote-service-interface.capnp.h"
#include "c8/string.h"
#include "c8/vector.h"
#include "reality/engine/api/base/geo-types.capnp.h"

using std::string;

namespace c8 {

namespace {

static constexpr char updateChar = 'u';
static constexpr char terminateChar = 't';

}  // namespace

RemoteDiskLogger::RemoteDiskLogger() {}

void RemoteDiskLogger::logToDisk(int numFrames, int fd) {
  if (numFrames <= 0) {
    framesRemaining_ = 0;
    return;
  }
  framesRemaining_ = numFrames + 1;

  int pipefds[2];
  KJ_SYSCALL(pipe(pipefds));
  int readFd = pipefds[0];
  writeFd_ = pipefds[1];

  queues_.fillingQueue = &requestQueueA;
  queues_.drainingQueue = &requestQueueB;

  auto &msgQueueRef = queues_;

  // iOS will supply a file descriptor, Android gives -1
  int diskFd = fd;
  bool closeDiskFd = false;
  if (diskFd > 0) {
    C8Log("[remote-service-connection] writing to file descriptor %d", diskFd);
  } else {
    // On Android we write directly to the Download directory
    char filename[80];
    sprintf(filename, "/sdcard/Download/log.%ld-%d", time(0), numFrames);

    diskFd = open(filename, O_CREAT | O_WRONLY);
    if (diskFd < 0) {
      C8Log(
        "[remote-service-connection] Could not open %s for logging %d frames.",
        filename,
        numFrames);
    } else {
      C8Log("[remote-service-connection] Opened %s for logging %d frames.", filename, numFrames);
      closeDiskFd = true;
    }
  }

  thrd = std::thread([this, readFd, diskFd, closeDiskFd, &queues = msgQueueRef]() {
    KjEventListener fdListener;
    kj::Vector<kj::Promise<void>> tasks;

    auto terminate = [&fdListener, &tasks, readFd]() {
      // Remove the inter-thread event listener and stop listening for
      // events, but defer since this will be called from within the event
      // read callback;
      auto defer = kj::evalLater([&fdListener, readFd]() {
                     fdListener.removeFdEvent(readFd);
                     fdListener.stop();
                   })
                     .eagerlyEvaluate(nullptr);
      tasks.add(std::move(defer));
    };

    fdListener.addFdEvent(
      readFd,
      EventFlag::READ | EventFlag::EDGE_TRIGGER | EventFlag::PERSIST,
      [this, readFd, diskFd, &queues, terminate]() {
        this->drainFdToDisk(readFd, diskFd, queues, terminate);
      });

    fdListener.wait();
    close(readFd);
    if (closeDiskFd) {
      close(diskFd);
    }
  });  // end std::thread
}

RemoteDiskLogger::~RemoteDiskLogger() {
  C8Log("[remote-disk-logger] %s", "~RemoteDiskLogger");

  // Detach thread for simplicity, but alternatively could add support for
  // a clean termination and join.
  KJ_SYSCALL(write(writeFd_, &terminateChar, sizeof(terminateChar)));
  if (thrd.joinable()) {
    thrd.join();
  }
  close(writeFd_);
  C8Log("[remote-disk-logger %s", "~RemoteDiskLogger#DONE");
}

void RemoteDiskLogger::log(const RemoteServiceRequest::Reader &request) {
  if (framesRemaining_ <= 0) {
    C8Log("[remote-disk-logger] %s", "tried to log when not writing to disk");
    return;
  }
  queues_.enqueue(request);
  if (framesRemaining_ >= 0) {
    --framesRemaining_;
  }

  // Notify networking thread to wake up and send update by sending a byte
  C8Log("[remote-disk-logger] log frame %d remaining", framesRemaining_);
  KJ_SYSCALL(write(writeFd_, &updateChar, sizeof(updateChar)));
}

bool RemoteDiskLogger::hasPendingRequests() {
  // Note: We aren't locking here, but I'm not sure it makes sense to lock since a swap could
  // happen while we're inspecting the result.
  return queues_.fillingQueue->size() > 0;
}

bool RemoteDiskLogger::hasWrittenEverything() {
  return queues_.fillingQueue->empty() && queues_.drainingQueue->empty();
}

// Drain any bytes on the read File descriptor to disk,
void RemoteDiskLogger::drainFdToDisk(
  evutil_socket_t readFd, int diskFd, RequestQueuePair &queues, std::function<void()> terminate) {

  char readBuffer[4096];
  ssize_t n = 0;
  do {
    KJ_SYSCALL(n = read(readFd, &readBuffer, sizeof(readBuffer)));
    char *endByte = readBuffer + n;
    if (std::find(readBuffer, endByte, terminateChar) != endByte) {
      // End all processing tasks if the terminate char is encountered.
      terminate();
      return;
    }
  } while (n == sizeof(readBuffer));

  // Make sure the draining queue is empty before swap.
  while (!queues.drainingQueue->empty()) {
    std::unique_ptr<MutableRootMessage<RemoteServiceRequest>> msg;
    msg.swap(queues.drainingQueue->front());
    queues.drainingQueue->pop_front();
    c8::RemoteServiceRequest::Reader rdr = msg->reader();
    int i = 0;
    for (const auto &record : rdr.getRecords()) {
      ++i;
      MutableRootMessage<XrRemoteRequest> message(record);
      capnp::writeMessageToFd(diskFd, *message.message());
    }
    // C8Log("[remote-disk-logger] logged %d frames", i);
  }

  // Swap the draining and filling queues.
  queues.swap();

  // Drain the previously filling queue.
  while (!queues.drainingQueue->empty()) {
    std::unique_ptr<MutableRootMessage<RemoteServiceRequest>> msg;
    msg.swap(queues.drainingQueue->front());
    queues.drainingQueue->pop_front();
    c8::RemoteServiceRequest::Reader rdr = msg->reader();
    int i = 0;
    for (const auto &record : rdr.getRecords()) {
      ++i;
      MutableRootMessage<XrRemoteRequest> message(record);
      capnp::writeMessageToFd(diskFd, *message.message());
    }
    // C8Log("[remote-disk-logger] logged %d frames", i);
  }
}

}  // namespace c8
