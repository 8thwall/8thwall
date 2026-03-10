// Copyright (c) 2022 8th Wall, Inc.
// Original Author: Dat Chu (datchu@nianticlabs.com)

#include <functional>

#include "c8/c8-log.h"
#include "c8/string.h"
#include "c8/vector.h"

#pragma once

namespace c8 {
using FetchId = uint64_t;

struct FetchResponse {
  FetchId fetchId = 0;

  // status code of the response
  uint16_t status = 0;

  // response data
  const char *data = nullptr;
  uint64_t numBytes = 0;
};

using OnReceiveFunc = std::function<void(const FetchResponse &)>;
using OnErrorFunc = std::function<void(const FetchResponse &)>;

class Fetch {
public:
  Fetch(const Fetch &) = delete;
  Fetch &operator=(const Fetch &) = delete;

  Fetch(Fetch &&) = default;
  Fetch &operator=(Fetch &&) = default;

  ~Fetch();

  // Initialize global resources. When isMockNetwork=true, should not use network (e.g. tests)
  // @return bool true if initialization was successful and you can start Fetch::Post
  static bool init(bool isMockNetwork = false);
  // Clean up global resources.
  static void cleanUp();

  // Create a post request. Take ownership of the payload.
  // @param headers pairs of header key and header value, size should be even.
  // TODO(dat): headers can benefit from a type that explicitly model key+value pair
  static Fetch Post(
    const String &url,
    const Vector<String> &headers,
    String &&payload,
    bool withCredentials = false);

  static Fetch Get(const String &url, const Vector<String> &headers, bool makeSynchronous = false);

  static Fetch Put(const String &url, const Vector<String> &headers, String &&payload);

  // Chain your response callback. Data sharing between callbacks need to be explicit.
  Fetch &then(OnReceiveFunc &&onReceive);

  // Chain your error callback. Data sharing between callbacks need to be explicit.
  //
  // Note that with fetch-js (using emscripten_fetch) we will get this even with 400 errors, unlike
  // curl or the JS Fetch API which will call then(). So you should do appropriate handling for
  // 400's in both then() and error(). If you are interested in unifying the API you could consider
  // modifying fetch-js's onsuccess and onerror callbacks to route through a single handler, parse
  // the result, and then route to either then() or error() in order to better match fetch-curl.
  // https://github.com/emscripten-core/emscripten/issues/12684#issue-734889750
  Fetch &error(OnErrorFunc &&onError);

  FetchId id() const { return id_; };

private:
  static bool isMockNetwork_;
  Fetch() = default;
  FetchId id_ = 0;
};

class FetchInternalState {
public:
  virtual ~FetchInternalState() = default;

  // see if we are finished, if so, flush by invoking receive/error functions
  void maybeFlush();

  void setInScope(bool inScope) { inScope_ = inScope; }
  bool inScope() const { return inScope_; }
  Vector<OnReceiveFunc> &onReceiveFuncs() { return onReceiveFuncs_; }
  Vector<OnErrorFunc> &onErrorFuncs() { return onErrorFuncs_; }

  // Removes response and error callbacks for this fetch.
  void removeCallbacks();

protected:
  // call after handling the successful fetch
  void setFinishSuccessfully();
  // call after handling the failed fetch
  void setFinishFailed();
  Vector<OnReceiveFunc> onReceiveFuncs_;
  Vector<OnErrorFunc> onErrorFuncs_;
  // is the Fetch object that is associated with this internal state still around
  bool inScope_ = true;
  FetchResponse successData_;
  FetchResponse errorData_;
  // Has the server responded?
  bool finished_ = false;
  // Was the request successful, HTTP 2xx status
  bool success_ = false;
};

struct CallbackUserData {
  FetchId id = 0;
  // Payload to persist so fetch request will have access to it asynchronously
  String payload;
  // Keep track of where we are when reading the data to upload.
  size_t readPos_ = 0;
};

//////////////////////////////////////////////////////////////////////////////////////////////////
// Should only be called by fetch-cc and fetch-js
//////////////////////////////////////////////////////////////////////////////////////////////////
// Generate a new fetch id.
FetchId generateFetchId();

// Try to remove a fetch from static storage.
void tryRemove(FetchId id);
FetchInternalState *getFetch(FetchId id);
bool hasFetch(FetchId id);
void cleanupFetch(FetchId id);

//////////////////////////////////////////////////////////////////////////////////////////////////
// Can be called externally
//////////////////////////////////////////////////////////////////////////////////////////////////
// Removes response and error callbacks for this fetch. Can be used in place of an abort() method.
// While it won't abort the request itself, it will prevent callbacks from being called.
//
// Ideally we have support for aborting network requests, but:
// 1) For fetch-js, emscripten_fetch has a bug where aborting an async request causes a crash
// (which is what was seen when running the engine) or undefined behavior:
// https://github.com/emscripten-core/emscripten/issues/8234
// 2) For fetch-curl, we only support synchronous requests, so there is no notion of a cancel.
void tryRemoveCallbacks(FetchId);
}  // namespace c8
