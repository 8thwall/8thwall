// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules.h"

cc_library {
  hdrs = {
    "kj-event-listener.h",
  };
  deps = {
    ":event-listener",
    "//c8:exceptions",
    "//bzl/inliner:rules",
    "@capnproto//:kj",
  };
}

#include <kj/vector.h>

#include "c8/events/kj-event-listener.h"

#include "c8/exceptions.h"

using kj::UnixEventPort;

namespace c8 {

namespace {

// Wait for the fd to become readable, then call the user-supplied callback.
// Loop forever if persist=true.
kj::Promise<void> waitForRead(
  kj::Own<UnixEventPort::FdObserver> observer,
  bool persist,
  std::function<void()> done,
  std::function<void()> callback) {
  return observer->whenBecomesReadable().then([
    persist,
    callback(std::move(callback)),
    done,
    observer(std::move(observer))
  ]() mutable->kj::Promise<void> {
    // Call the user-provided callback function.
    callback();
    if (persist) {
      // Continue with tail recursion.
      return waitForRead(
        std::move(observer), persist, done, std::move(callback));
    } else {
      done();
      return kj::READY_NOW;
    }
  });
}

// Wait for the fd to become writable, then call the user-supplied callback.
// Loop forever if persist=true.
kj::Promise<void> waitForWrite(
  kj::Own<kj::UnixEventPort::FdObserver> observer,
  bool persist,
  std::function<void()> done,
  std::function<void()> callback) {
  return observer->whenBecomesWritable().then([
    persist,
    callback(std::move(callback)),
    done,
    observer(std::move(observer))
  ]() mutable->kj::Promise<void> {
    // Call the user-provided callback function.
    callback();
    if (persist) {
      // Continue with tail recursion.
      return waitForWrite(
        std::move(observer), persist, done, std::move(callback));
    } else {
      done();
      return kj::READY_NOW;
    }
  });
}
}

KjEventListener::KjEventListener()
    : ioContext(kj::setupAsyncIo()),
      donePair(kj::newPromiseAndFulfiller<void>()) {}

void KjEventListener::addFdEvent(
  int fd, EventFlags flags, std::function<void()> callback) {
  if (!(flags & EventFlag::EDGE_TRIGGER)) {
    C8_THROW("KJ only supports Edge-triggered I/O");
  }

  UnixEventPort::FdObserver::Flags kjFdFlags =
    static_cast<UnixEventPort::FdObserver::Flags>(0);

  if (flags & EventFlag::READ) {
    kjFdFlags = static_cast<UnixEventPort::FdObserver::Flags>(
      kjFdFlags | UnixEventPort::FdObserver::OBSERVE_READ);
  }
  if (flags & EventFlag::WRITE) {
    kjFdFlags = static_cast<UnixEventPort::FdObserver::Flags>(
      kjFdFlags | UnixEventPort::FdObserver::OBSERVE_WRITE);
  }
  if (kjFdFlags == 0) {
    C8_THROW("EventFlags have neither READ nor WRITE");
  }

  bool persist = flags & EventFlag::PERSIST;

  kj::Vector<kj::Promise<void>> newPromises;

  auto done = [fd, this]() {
    kj::evalLater([fd, this]() {
      removeFdEvent(fd);
    }).detach([](kj::Exception&& e) { throw e; });
  };

  if (flags & EventFlag::READ) {
    kj::Own<UnixEventPort::FdObserver> fdObserver =
      kj::heap<UnixEventPort::FdObserver>(
        ioContext.unixEventPort, fd, kjFdFlags);
    newPromises.add(
      waitForRead(std::move(fdObserver), persist, done, callback));
  }
  if (flags & EventFlag::WRITE) {
    kj::Own<UnixEventPort::FdObserver> fdObserver =
      kj::heap<UnixEventPort::FdObserver>(
        ioContext.unixEventPort, fd, kjFdFlags);
    newPromises.add(
      waitForWrite(std::move(fdObserver), persist, done, callback));
  }

  if (newPromises.size() > 0) {
    // Add any read/write to the list of promises.
    promises.emplace(
      fd,
      kj::joinPromises(newPromises.releaseAsArray()).eagerlyEvaluate(nullptr));
  }
}

void KjEventListener::removeFdEvent(int fd) {
  promises.erase(fd);
}

void KjEventListener::wait() {
  donePair = kj::newPromiseAndFulfiller<void>();
  donePair.promise.wait(ioContext.waitScope);
}

void KjEventListener::stop() {
  donePair.fulfiller->fulfill();
}

}  // namespace c8
