// Copyright (c) 2022 8th Wall, Inc.
// Original Author: Paris Morgan (paris@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    "@com_google_googletest//:gtest_main",
    "//c8/network:fetch",
    "//c8:c8-log",
  };
}
cc_end(0x9883b275);

#include "c8/c8-log.h"
#include "c8/network/fetch.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace c8 {

constexpr bool DEBUG = false;

class FetchTest : public ::testing::Test {};

class Fetcher {
public:
  Fetcher(bool isMockNetwork = true) : isMockNetwork_(isMockNetwork) { Fetch::init(isMockNetwork); }

  ~Fetcher() {
    if (!isMockNetwork_) {
      Fetch::cleanUp();
    }
  }

  Fetch &get(
    const String &url,
    const Vector<String> &headers,
    std::function<void(const FetchResponse &)> onReceive,
    std::function<void(const FetchResponse &)> onError) {
    return Fetch::Get(url, headers)
      .then([onReceive](const FetchResponse &r) { onReceive(r); })
      .error([onError](const FetchResponse &e) { onError(e); });
  }

private:
  bool isMockNetwork_ = false;
};

// NOTE: Keep this test as the first test because FetchId is generated statically.
TEST_F(FetchTest, FetchId) {
  Fetcher f;
  auto onReceive1 = [](const FetchResponse &r) { EXPECT_EQ(r.fetchId, 1); };
  auto onError1 = [](const FetchResponse &r) { EXPECT_EQ(r.fetchId, 1); };
  f.get("https://example.com", {}, onReceive1, onError1);

  auto onReceive2 = [](const FetchResponse &r) { EXPECT_EQ(r.fetchId, 2); };
  auto onError2 = [](const FetchResponse &r) { EXPECT_EQ(r.fetchId, 2); };
  f.get("https://example.com", {}, onReceive2, onError2);
}

TEST_F(FetchTest, GetMockNetwork) {
  Fetcher f;
  auto onReceive = [](const FetchResponse &r) { EXPECT_EQ(r.status, 0); };
  auto onError = [](const FetchResponse &r) { FAIL(); };
  f.get("https://example.com", {}, onReceive, onError);
}

TEST_F(FetchTest, GetRealNetwork) {
  if (!DEBUG) {
    return;
  }
  Fetcher f(false);
  auto onReceive = [](const FetchResponse &r) { EXPECT_EQ(r.status, 200); };
  auto onError = [](const FetchResponse &r) { FAIL(); };
  f.get("https://example.com", {}, onReceive, onError);
}

TEST_F(FetchTest, TryRemoveCallbacks) {
  Fetcher f;
  auto onReceive = [](const FetchResponse &r) {};
  auto onError = [](const FetchResponse &r) {};
  const auto &fetch = f.get("https://example.com", {}, onReceive, onError);
  // No-op when using fetch-curl.
  tryRemoveCallbacks(fetch.id());
  // No crash when passed a bad id.
  tryRemoveCallbacks(1000);
}

}  // namespace c8
