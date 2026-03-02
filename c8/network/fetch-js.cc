// Copyright (c) 2022 8th Wall, Inc.
// Original Author: Dat Chu (datchu@nianticlabs.com)

#include <emscripten/fetch.h>

#include "c8/c8-log.h"
#include "c8/map.h"
#include "c8/network/fetch.h"
#include "c8/vector.h"

namespace c8 {
class EmscriptenFetchInternalState : public FetchInternalState {
public:
  ~EmscriptenFetchInternalState() {
    if (emscriptenFetch_ != nullptr) {
      emscripten_fetch_close(emscriptenFetch_);
    }
  }

  void setFinishSuccessfully(FetchId id, emscripten_fetch_t *fetch) {
    successData_ = {id, fetch->status, fetch->data, fetch->numBytes};
    FetchInternalState::setFinishSuccessfully();
  }

  void setFinishFailed(FetchId id, emscripten_fetch_t *fetch) {
    errorData_ = {id, fetch->status, fetch->data, fetch->numBytes};
    FetchInternalState::setFinishFailed();
  }

  void setEmscriptenFetch(emscripten_fetch_t *emscriptenFetch) {
    emscriptenFetch_ = emscriptenFetch;
  }

private:
  emscripten_fetch_t *emscriptenFetch_ = nullptr;
};

static TreeMap<FetchId, EmscriptenFetchInternalState> idToFetch;
static TreeMap<FetchId, CallbackUserData> idToUserData;

FetchInternalState *getFetch(FetchId id) { return &idToFetch[id]; }

bool hasFetch(FetchId id) { return idToFetch.count(id) > 0; }

void cleanupFetch(FetchId id) {
  idToFetch.erase(id);
  idToUserData.erase(id);
}

void setFinishSuccessfully(emscripten_fetch_t *fetch) {
  auto id = ((CallbackUserData *)fetch->userData)->id;
  auto *fetchInternalState = getFetch(id);
  static_cast<EmscriptenFetchInternalState *>(fetchInternalState)->setFinishSuccessfully(id, fetch);
  tryRemove(id);
}

void setFinishFailed(emscripten_fetch_t *fetch) {
  auto id = ((CallbackUserData *)fetch->userData)->id;
  auto *fetchInternalState = getFetch(id);
  static_cast<EmscriptenFetchInternalState *>(fetchInternalState)->setFinishFailed(id, fetch);
  tryRemove(id);
}

Fetch Fetch::Post(
  const String &url, const Vector<String> &headers, String &&payload, bool withCredentials) {
  Fetch theFetch;
  auto fetchId = generateFetchId();
  theFetch.id_ = fetchId;

  emscripten_fetch_attr_t attrs;
  emscripten_fetch_attr_init(&attrs);
  strcpy(attrs.requestMethod, "POST");
  attrs.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
  attrs.withCredentials = withCredentials;

  // headers
  auto requestHeaders = std::make_unique<const char *[]>(headers.size() + 1);
  requestHeaders[headers.size()] = nullptr;
  for (size_t i = 0; i < headers.size(); i++) {
    requestHeaders[i] = headers[i].c_str();
  }
  attrs.requestHeaders = requestHeaders.get();

  // callbacks
  idToUserData[fetchId] = {fetchId};
  attrs.userData = (void *)(&idToUserData[fetchId]);
  attrs.onsuccess = &setFinishSuccessfully;
  attrs.onerror = &setFinishFailed;

  // body
  if (!payload.empty()) {
    // Need to copy the payload so it persists as long as our data. Otherwise the string
    // might get collected before the fetch starts.
    idToUserData[fetchId].payload = payload;
    attrs.requestData = idToUserData[fetchId].payload.c_str();
    attrs.requestDataSize = idToUserData[fetchId].payload.size();
  }

  auto *internalState = static_cast<EmscriptenFetchInternalState *>(getFetch(fetchId));
  if (!isMockNetwork_) {
    internalState->setEmscriptenFetch(emscripten_fetch(&attrs, url.c_str()));
  }
  return theFetch;
}

Fetch Fetch::Get(const String &url, const Vector<String> &headers, bool makeSynchronous) {
  Fetch theFetch;
  auto fetchId = generateFetchId();
  theFetch.id_ = fetchId;

  emscripten_fetch_attr_t attrs;
  emscripten_fetch_attr_init(&attrs);
  strcpy(attrs.requestMethod, "GET");
  if (makeSynchronous) {
    attrs.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY | EMSCRIPTEN_FETCH_WAITABLE;
  } else {
    attrs.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
  }

  // headers
  auto requestHeaders = std::make_unique<const char *[]>(headers.size() + 1);
  requestHeaders[headers.size()] = nullptr;
  for (size_t i = 0; i < headers.size(); i++) {
    requestHeaders[i] = headers[i].c_str();
  }
  attrs.requestHeaders = requestHeaders.get();

  // callbacks
  idToUserData[fetchId] = {fetchId};
  attrs.userData = (void *)(&idToUserData[fetchId]);
  attrs.onsuccess = &setFinishSuccessfully;
  attrs.onerror = &setFinishFailed;

  auto *internalState = static_cast<EmscriptenFetchInternalState *>(getFetch(fetchId));
  if (isMockNetwork_) {
    return theFetch;
  }
  auto *fetch = emscripten_fetch(&attrs, url.c_str());
  internalState->setEmscriptenFetch(fetch);

  if (makeSynchronous) {
    EMSCRIPTEN_RESULT ret = EMSCRIPTEN_RESULT_TIMED_OUT;
    while (ret == EMSCRIPTEN_RESULT_TIMED_OUT) {
      // Using 10 millisecond wait because 0 wait fails
      ret = emscripten_fetch_wait(fetch, 10);
    }
  }

  return theFetch;
}

bool Fetch::isMockNetwork_ = false;
bool Fetch::init(bool isMockNetwork) {
  isMockNetwork_ = isMockNetwork;
  // no global init required for emscripten_fetch
  return true;
}

void Fetch::cleanUp() {}

}  // namespace c8
