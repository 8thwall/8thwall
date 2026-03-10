// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include <kj/async-io.h>

#include <memory>

#pragma once

namespace c8 {

// A wrapper for kj::AsyncIoContext. This can typically only be created as a class or stack variable
// (not as a pointer), so this wrapper effictively provides the ability to call "new" on an io
// context and hold it in a unique pointer.
class KjIoContextHolder {
public:
  kj::AsyncIoContext ioContext;
  KjIoContextHolder() : ioContext(kj::setupAsyncIo()) {}
};

// This static class is a workaround for some rough edges in the KJ api.  There can only be one
// AsyncIoContext per thread, and the EzRpc interafce both creates and then hides that interface
// making it difficult to share the resource with other classes.
//
// This static class will create an AsyncIoContext and provide hooks to its AsyncIoProvider and its
// WaitScope if it does not know that one exists. But if you want to use EzRpc, you can tell it to
// use the AsyncIoProvider and WaitScope from that.
//
// As a consequence of this, apps that want to share AsyncIoContext in this way must create the
// EzRpc interface and call setKjEventLoop first, before trying to call getIoProvider and
// getWaitScope.
class KjEventLoop {
public:
  static void setKjEventLoop(kj::AsyncIoProvider &ioProvider, kj::WaitScope &waitScope);
  static kj::AsyncIoProvider &getIoProvider();
  static kj::WaitScope &getWaitScope();

  // Don't construct.
  KjEventLoop() = delete;
  KjEventLoop(KjEventLoop &&) = delete;
  KjEventLoop &operator=(KjEventLoop &&) = delete;
  KjEventLoop(const KjEventLoop &) = delete;
  KjEventLoop &operator=(const KjEventLoop &) = delete;

private:
  static void initIoContext();
  static std::unique_ptr<KjIoContextHolder> ioContext_;
  static kj::AsyncIoProvider *ioProvider_;
  static kj::WaitScope *waitScope_;
};

}  // namespace c8
