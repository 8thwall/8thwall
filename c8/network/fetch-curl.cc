// Copyright (c) 2022 8th Wall, Inc.
// Original Author: Dat Chu (datchu@nianticlabs.com)

// Looks like algorithm is needed for std::min?
#include <algorithm>
#include <sstream>
#include <cstring>

#include "c8/c8-log.h"
#include "c8/map.h"
#include "c8/network/fetch.h"
#include "c8/string.h"
#include "c8/string/format.h"
#include "c8/vector.h"
#include "curl/curl.h"

namespace c8 {
namespace {
#ifdef NDEBUG
constexpr static int debug_ = 0;
#else
constexpr static int debug_ = 1;
#endif
}  // namespace

// Perform the request. Without curl_multi, this will be synchronous.
#define DO_CURL(curl)                                               \
  CURLcode res = CURLE_OK;                                          \
  if (!isMockNetwork_) {                                            \
    res = curl_easy_perform(curl);                                  \
  }                                                                 \
  if (res != CURLE_OK) {                                            \
    C8Log("[fetch-curl] curl failed: %s", curl_easy_strerror(res)); \
    setFinishFailed(fetchId);                                       \
  } else {                                                          \
    setFinishSuccessfully(fetchId);                                 \
  }

String makeHeader(const String &headerName, const String &headerValue) {
  if (headerValue.empty()) {
    return format("%s;", headerName.c_str());
  }
  return format("%s: %s", headerName.c_str(), headerValue.c_str());
}

// Keep this in scope for the duration of the curl request.
class CurlHeaders {
public:
  void append(const String &name, const String &value) {
    curlHeaders_ = curl_slist_append(curlHeaders_, makeHeader(name, value).c_str());
  }
  void remove(const String &name) {
    curlHeaders_ = curl_slist_append(curlHeaders_, format("%s:", name.c_str()).c_str());
  }
  curl_slist *get() { return curlHeaders_; }

  ~CurlHeaders() { curl_slist_free_all(curlHeaders_); }

private:
  curl_slist *curlHeaders_ = nullptr;
};

class CurlFetchInternalState : public FetchInternalState {
public:
  ~CurlFetchInternalState() {
    // Curl manual recommends that previous CURL objects be kept if another request is being made
    // to the same host.
    // TODO(dat): Consider using a pool of CURL* if they all hit the same API gateway endpoints.
    if (curl_ != nullptr) {
      curl_easy_cleanup(curl_);
    }
  }

  void setFinishSuccessfully(FetchId id) {
    long responseCode;
    curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &responseCode);
    successData_ = {id, (uint16_t)responseCode, responseData_.c_str(), responseData_.size()};
    FetchInternalState::setFinishSuccessfully();
  }

  void setFinishFailed(FetchId id) {
    long responseCode;
    curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &responseCode);
    errorData_ = {id, (uint16_t)responseCode, responseData_.c_str(), responseData_.size()};
    FetchInternalState::setFinishFailed();
  }

  void setCurl(CURL *curl) { curl_ = curl; }

  void write(void *data, size_t size) { responseData_.append((const char *)data, size); }

  CurlHeaders &headers() { return headers_; }

private:
  CURL *curl_ = nullptr;
  String responseData_;  // Data we received.
  CurlHeaders headers_;
};

static TreeMap<FetchId, CurlFetchInternalState> idToFetch;
static TreeMap<FetchId, CallbackUserData> idToUserData;

FetchInternalState *getFetch(FetchId id) { return &idToFetch[id]; }

bool hasFetch(FetchId id) { return idToFetch.count(id) > 0; }

void cleanupFetch(FetchId id) {
  idToFetch.erase(id);
  idToUserData.erase(id);
}

void setFinishSuccessfully(FetchId id) {
  static_cast<CurlFetchInternalState *>(getFetch(id))->setFinishSuccessfully(id);
  tryRemove(id);
}

void setFinishFailed(FetchId id) {
  static_cast<CurlFetchInternalState *>(getFetch(id))->setFinishFailed(id);
  tryRemove(id);
}

// Writing the content that we are receiving (the response)
size_t writeCallback(void *data, size_t size, size_t nmemb, void *userp) {
  auto *userData = (CallbackUserData *)userp;
  size_t realsize = size * nmemb;
  idToFetch[userData->id].write(data, realsize);
  return realsize;
}

// Uploading what we are reading.
size_t readCallback(char *buffer, size_t size, size_t nitems, void *userp) {
  auto *userData = reinterpret_cast<CallbackUserData *>(userp);
  size_t realSize = size * nitems;
  size_t actualRead = (std::min)(realSize, userData->payload.size() - userData->readPos_);
  std::memcpy(buffer, userData->payload.data() + userData->readPos_, actualRead);
  userData->readPos_ += actualRead;
  return actualRead;
}

CURL *createBasicCurl(FetchId fetchId, const String &url) {
  CURL *curl = curl_easy_init();
  idToUserData[fetchId] = {fetchId};
  if (curl) {
    if (debug_) {
      curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    }
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)(&idToUserData[fetchId]));
  }
  return curl;
}

void addHeaders(CURL *curl, CurlHeaders &headerState, const Vector<String> &headerList) {
  if (headerList.empty()) {
    return;
  }
  for (int i = 0; i < headerList.size() - 1; i += 2) {
    headerState.append(headerList[i], headerList[i + 1]);
  }
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerState.get());
}

Fetch Fetch::Post(
  const String &url, const Vector<String> &headers, String &&payload, bool withCredentials) {
  // Need to store this in our curl for the callback
  auto fetchId = generateFetchId();
  Fetch theFetch;
  theFetch.id_ = fetchId;

  CURL *curl = createBasicCurl(fetchId, url);

  // Immediately persist curl into the internal state so callbacks (success/fail) has an object
  // to read from
  auto *internalState = static_cast<CurlFetchInternalState *>(getFetch(fetchId));
  internalState->setCurl(curl);

  if (curl) {
    internalState->headers().remove("Accept");
    addHeaders(curl, internalState->headers(), headers);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);

    if (!payload.empty()) {
      // Need to copy the payload so it persists as long as our data. Otherwise the string
      // might get collected before the fetch starts.
      idToUserData[fetchId].payload = payload;

      // NOTE(dat): There are chunk support as well but we don't do this here
      /* Set the expected POST size. If you want to POST large amounts of data,
        consider CURLOPT_POSTFIELDSIZE_LARGE */
      curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)idToUserData[fetchId].payload.size());
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, idToUserData[fetchId].payload.c_str());
    }
    DO_CURL(curl);
  }
  return theFetch;
}

// Note(Riyaan): makeSynchronous isn't used since method is currently always synchronous
Fetch Fetch::Get(const String &url, const Vector<String> &headers, bool makeSynchronous) {
  auto fetchId = generateFetchId();
  Fetch theFetch;
  theFetch.id_ = fetchId;

  CURL *curl = createBasicCurl(fetchId, url);
  auto *internalState = static_cast<CurlFetchInternalState *>(getFetch(fetchId));
  internalState->setCurl(curl);
  if (curl) {
    internalState->headers().remove("Accept");
    addHeaders(curl, internalState->headers(), headers);
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    DO_CURL(curl);
  }
  return theFetch;
}

Fetch Fetch::Put(const String &url, const Vector<String> &headers, String &&payload) {
  auto fetchId = generateFetchId();
  Fetch theFetch;
  theFetch.id_ = fetchId;
  CURL *curl = createBasicCurl(fetchId, url);
  auto *internalState = static_cast<CurlFetchInternalState *>(getFetch(fetchId));
  internalState->setCurl(curl);

  if (curl) {
    internalState->headers().remove("Accept");
    internalState->headers().remove("Expect");
    addHeaders(curl, internalState->headers(), headers);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);  // Makes this a PUT request.

    if (!payload.empty()) {
      idToUserData[fetchId].payload = payload;
      idToUserData[fetchId].readPos_ = 0;
      curl_easy_setopt(curl, CURLOPT_READFUNCTION, readCallback);
      curl_easy_setopt(curl, CURLOPT_READDATA, (void *)(&idToUserData[fetchId]));
    }

    DO_CURL(curl);
  }
  return theFetch;
}

bool Fetch::isMockNetwork_ = false;
bool Fetch::init(bool isMockNetwork) {
  isMockNetwork_ = isMockNetwork;

  CURLcode res;
  if (!isMockNetwork_) {
    /* In windows, this will init the winsock stuff */
    res = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (res != CURLE_OK) {
      C8Log("[fetch-curl] Global init failed %s", curl_easy_strerror(res));
      return false;
    }
  }

  return true;
}

void Fetch::cleanUp() {
  if (!isMockNetwork_) {
    curl_global_cleanup();
  }
}

}  // namespace c8
