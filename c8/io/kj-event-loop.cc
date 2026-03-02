// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules.h"

cc_library {
  hdrs = {
    "kj-event-loop.h",
  };
  deps = {
    "//bzl/inliner:rules",
    "//c8:exceptions",
    "@capnproto//:kj",
  };
  visibility = {
    "//visibility:public",
  };
}

#include "c8/io/kj-event-loop.h"

#include "c8/exceptions.h"

using namespace c8;

std::unique_ptr<KjIoContextHolder> KjEventLoop::ioContext_;
kj::AsyncIoProvider *KjEventLoop::ioProvider_ = nullptr;
kj::WaitScope *KjEventLoop::waitScope_ = nullptr;

void KjEventLoop::initIoContext() {
  ioContext_.reset(new KjIoContextHolder());
  ioProvider_ = ioContext_->ioContext.provider;
  waitScope_ = &ioContext_->ioContext.waitScope;
}

void KjEventLoop::setKjEventLoop(kj::AsyncIoProvider &ioProvider, kj::WaitScope &waitScope) {
  if (ioContext_ != nullptr) {
    C8_THROW(
      "setKjEventLoop must be called before getIoProvider and getWaitScope; make sure your EzRpcs "
      "are created before other async objects");
  }
  ioProvider_ = &ioProvider;
  waitScope_ = &waitScope;
}

kj::AsyncIoProvider &KjEventLoop::getIoProvider() {
  if (ioProvider_ == nullptr) {
    initIoContext();
  }
  return *ioProvider_;
}

kj::WaitScope &KjEventLoop::getWaitScope() {
  if (waitScope_ == nullptr) {
    initIoContext();
  }
  return *waitScope_;
}
