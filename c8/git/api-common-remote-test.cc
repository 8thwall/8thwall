#include "bzl/inliner/rules2.h"

cc_test {
  size = "medium";
  deps = {
    ":g8-api.capnp-cc",
    ":api-common",
    "//c8:c8-log",
    "//c8:c8-log-proto",
    "//c8/io:capnp-messages",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xeb70ecd7);

#include "c8/c8-log-proto.h"
#include "c8/c8-log.h"
#include "c8/git/api-common.h"
#include "c8/git/g8-api.capnp.h"
#include "c8/io/capnp-messages.h"
#include "gmock/gmock.h"

namespace c8 {

TEST(ApiCommonRemote, Gitlab) {
  MutableRootMessage<G8GitRemote> remoteInfo;
  g8::setRemoteInfo(
    remoteInfo.builder(), "https://gitlab.<REMOVED_BEFORE_OPEN_SOURCING>.com/repo/niantic.git");
  EXPECT_EQ(remoteInfo.reader().getApi(), G8GitRemote::Api::GITLAB);
  EXPECT_EQ(remoteInfo.reader().getRepoName(), "repo/niantic.git");
  EXPECT_EQ(remoteInfo.reader().getHost(), "gitlab.<REMOVED_BEFORE_OPEN_SOURCING>.com");
}

TEST(ApiCommonRemote, Github) {
  MutableRootMessage<G8GitRemote> remoteInfo;
  g8::setRemoteInfo(remoteInfo.builder(), "https://github.com/8thwall/prod8.git");
  EXPECT_EQ(remoteInfo.reader().getApi(), G8GitRemote::Api::GITHUB);
  EXPECT_EQ(remoteInfo.reader().getRepoName(), "8thwall/prod8.git");
  EXPECT_EQ(remoteInfo.reader().getHost(), "github.com");
}

TEST(ApiCommonRemote, 8thWall) {
  MutableRootMessage<G8GitRemote> remoteInfo;
  g8::setRemoteInfo(remoteInfo.builder(), "https://www.8thwall.com/v1/repos/example.repo.id.git");
  // TODO(christoph): Switch to
  EXPECT_EQ(remoteInfo.reader().getApi(), G8GitRemote::Api::UNKNOWN);
  EXPECT_EQ(remoteInfo.reader().getRepoName(), "");
  EXPECT_EQ(remoteInfo.reader().getHost(), "www.8thwall.com");
}

TEST(ApiCommonRemote, Unknown) {
  MutableRootMessage<G8GitRemote> remoteInfo;
  g8::setRemoteInfo(remoteInfo.builder(), "https://example.com/8thwall/prod8.git");
  EXPECT_EQ(remoteInfo.reader().getApi(), G8GitRemote::Api::UNKNOWN);
  EXPECT_EQ(remoteInfo.reader().getRepoName(), "");
  EXPECT_EQ(remoteInfo.reader().getHost(), "example.com");
}

TEST(ApiCommonRemote, GitlabWithExtraInfo) {
  MutableRootMessage<G8GitRemote> remoteInfo;
  g8::setRemoteInfo(
    remoteInfo.builder(),
    "https://example%40nianticlabs.com@gitlab.<REMOVED_BEFORE_OPEN_SOURCING>.com/repo/niantic.git");
  EXPECT_EQ(remoteInfo.reader().getApi(), G8GitRemote::Api::GITLAB);
  EXPECT_EQ(remoteInfo.reader().getRepoName(), "repo/niantic.git");
  EXPECT_EQ(remoteInfo.reader().getHost(), "gitlab.<REMOVED_BEFORE_OPEN_SOURCING>.com");
}

TEST(ApiCommonRemote, GithubWithAtSymbol) {
  MutableRootMessage<G8GitRemote> remoteInfo;
  g8::setRemoteInfo(remoteInfo.builder(), "https://github.com/example/@repo.git");
  EXPECT_EQ(remoteInfo.reader().getApi(), G8GitRemote::Api::GITHUB);
  EXPECT_EQ(remoteInfo.reader().getRepoName(), "example/@repo.git");
  EXPECT_EQ(remoteInfo.reader().getHost(), "github.com");
}

TEST(ApiCommonRemote, GithubWithNoPath) {
  MutableRootMessage<G8GitRemote> remoteInfo;
  g8::setRemoteInfo(remoteInfo.builder(), "https://github.com");
  EXPECT_EQ(remoteInfo.reader().getApi(), G8GitRemote::Api::GITHUB);
  EXPECT_EQ(remoteInfo.reader().getRepoName(), "");
  EXPECT_EQ(remoteInfo.reader().getHost(), "github.com");
}

TEST(ApiCommonRemote, GithubWithAtSymbolNoPath) {
  MutableRootMessage<G8GitRemote> remoteInfo;
  g8::setRemoteInfo(remoteInfo.builder(), "https://user:pass@github.com");
  EXPECT_EQ(remoteInfo.reader().getApi(), G8GitRemote::Api::GITHUB);
  EXPECT_EQ(remoteInfo.reader().getRepoName(), "");
  EXPECT_EQ(remoteInfo.reader().getHost(), "github.com");
}

}  // namespace c8
