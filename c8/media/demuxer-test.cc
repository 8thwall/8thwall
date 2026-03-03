// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":demuxer",
    ":media-status",
    "//c8/media/codec:codec-api",
    "@com_google_googletest//:gtest_main",
    "@json//:json",
  };
}
cc_end(0xe5ebee92);

#include "c8/media/codec/codec-api.h"
#include "c8/media/demuxer.h"
#include "c8/media/media-status.h"

#include <nlohmann/json.hpp>
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace c8 {

using ::testing::_;
using ::testing::Ref;
using ::testing::Return;

class MockDemuxerApi : public DemuxerApi {
public:
  MOCK_METHOD1(open, MediaStatus(const char *path));

  MOCK_METHOD1(
    forEachTrack, MediaStatus(std::function<MediaStatus(const nlohmann::json &)> callback));

  MOCK_METHOD4(
    read,
    MediaStatus(
      const nlohmann::json &readConfig,
      const uint8_t **data,
      size_t *byteSize,
      nlohmann::json *sampleMetadata));

  MOCK_METHOD0(close, MediaStatus());
};

class DemuxerTest : public ::testing::Test {
public:
  MockDemuxerApi *mockApi() {
    MockDemuxerApi *api = new MockDemuxerApi();
    demuxer = Demuxer(api);
    return api;
  }

protected:
  Demuxer demuxer;
};

TEST_F(DemuxerTest, openTest) {
  constexpr char PATH[] = "/mock/path.mp3";
  // Start returns an error if no demuxer api is registered.
  EXPECT_FALSE(demuxer.isValid());
  auto result = demuxer.open(PATH);
  EXPECT_EQ(1, result.code());

  MockDemuxerApi *api = mockApi();
  EXPECT_TRUE(demuxer.isValid());

  // Calling open will return the correct status.
  EXPECT_CALL(*api, open(PATH))
    .WillOnce(Return(MediaStatus{0, ""}))
    .WillOnce(Return(MediaStatus{3, "Error"}));

  result = demuxer.open(PATH);
  EXPECT_EQ(0, result.code());

  result = demuxer.open(PATH);
  EXPECT_EQ(3, result.code());
}

TEST_F(DemuxerTest, forEachTrackTest) {
  auto callback = [](const nlohmann::json &) -> MediaStatus { return {}; };

  // Start returns an error if no demuxer api is registered.
  EXPECT_FALSE(demuxer.isValid());
  auto result = demuxer.forEachTrack(callback);
  EXPECT_EQ(1, result.code());

  MockDemuxerApi *api = mockApi();
  EXPECT_TRUE(demuxer.isValid());

  // Calling open will return the correct status.
  EXPECT_CALL(*api, forEachTrack(_))
    .WillOnce(Return(MediaStatus{0, ""}))
    .WillOnce(Return(MediaStatus{3, "Error"}));

  result = demuxer.forEachTrack(callback);
  EXPECT_EQ(0, result.code());

  result = demuxer.forEachTrack(callback);
  EXPECT_EQ(3, result.code());
}

TEST_F(DemuxerTest, readTest) {
  nlohmann::json readConfig;
  const uint8_t *data;
  size_t byteSize;
  nlohmann::json sampleMetadata;

  // Start returns an error if no demuxer api is registered.
  EXPECT_FALSE(demuxer.isValid());
  auto result = demuxer.read(readConfig, &data, &byteSize, &sampleMetadata);
  EXPECT_EQ(1, result.code());

  MockDemuxerApi *api = mockApi();
  EXPECT_TRUE(demuxer.isValid());

  // Calling start will return the correct status.
  EXPECT_CALL(*api, read(Ref(readConfig), &data, &byteSize, &sampleMetadata))
    .WillOnce(Return(MediaStatus{0, ""}))
    .WillOnce(Return(MediaStatus{3, "Error"}));

  result = demuxer.read(readConfig, &data, &byteSize, &sampleMetadata);
  EXPECT_EQ(0, result.code());

  result = demuxer.read(readConfig, &data, &byteSize, &sampleMetadata);
  EXPECT_EQ(3, result.code());
}

TEST_F(DemuxerTest, closeTest) {
  // Start returns an error if no demuxer api is registered.
  EXPECT_FALSE(demuxer.isValid());
  auto result = demuxer.close();
  EXPECT_EQ(1, result.code());

  MockDemuxerApi *api = mockApi();
  EXPECT_TRUE(demuxer.isValid());

  // Calling close will return the correct status.
  EXPECT_CALL(*api, close())
    .WillOnce(Return(MediaStatus{0, ""}))
    .WillOnce(Return(MediaStatus{3, "Error"}));

  result = demuxer.close();
  EXPECT_EQ(0, result.code());

  result = demuxer.close();
  EXPECT_EQ(3, result.code());
}

}  // namespace c8
