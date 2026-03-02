// Copyright (c) 2022 8th Wall, Inc.
// Original Author: Dat Chu (datchu@nianticlabs.com)

#include "c8/network/fetch.h"

#include "c8/map.h"

namespace c8 {

void FetchInternalState::removeCallbacks() {
  onReceiveFuncs_.clear();
  onErrorFuncs_.clear();
}

void FetchInternalState::maybeFlush() {
  if (!finished_) {
    return;
  }

  if (success_) {
    // executing then removing all receive callbacks
    for (const auto &cb : onReceiveFuncs_) {
      cb(successData_);
    }
    onReceiveFuncs_.clear();

    // remove all error callbacks without invoking
    onErrorFuncs_.clear();
  } else {
    // remove all receive callbacks without invoking
    onReceiveFuncs_.clear();

    // executing then removing all error callbacks
    for (const auto &cb : onErrorFuncs_) {
      cb(errorData_);
    }
    onErrorFuncs_.clear();
  }
}

void FetchInternalState::setFinishSuccessfully() {
  finished_ = true;
  success_ = true;
  maybeFlush();
}

void FetchInternalState::setFinishFailed() {
  finished_ = true;
  success_ = false;
  maybeFlush();
}

static FetchId prevFetchId = 0;

FetchId generateFetchId() { return ++prevFetchId; }

void tryRemove(FetchId id) {
  if (!hasFetch(id)) {
    return;
  }

  auto *requestState = getFetch(id);
  if (requestState->inScope()) {
    return;
  }

  if (requestState->onReceiveFuncs().empty() && requestState->onErrorFuncs().empty()) {
    cleanupFetch(id);
  }
}

Fetch &Fetch::then(OnReceiveFunc &&onReceive) {
  getFetch(id_)->onReceiveFuncs().push_back(std::move(onReceive));
  getFetch(id_)->maybeFlush();
  return *this;
}

Fetch &Fetch::error(OnErrorFunc &&onError) {
  getFetch(id_)->onErrorFuncs().push_back(std::move(onError));
  getFetch(id_)->maybeFlush();
  return *this;
}

Fetch::~Fetch() {
  getFetch(id_)->setInScope(false);
  tryRemove(id_);
}

void tryRemoveCallbacks(FetchId id) {
  if (!hasFetch(id)) {
    return;
  }

  getFetch(id)->removeCallbacks();
}

}  // namespace c8
