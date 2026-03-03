// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":mp4v2-demuxer",
    "@com_google_googletest//:gtest_main",
    "@json//:json",
  };
  data = {
    "//c8/media/testdata:reference-media",
  };
}
cc_end(0xee6afcd0);

#include "gtest/gtest.h"
#include "c8/media/codec/mp4v2-demuxer.h"

#include <nlohmann/json.hpp>

namespace c8 {

class Mp4v2DemuxerTest : public ::testing::Test {};

#define ASSERT_MEDIA(result)                         \
  {                                                  \
    MediaStatus status = (result);                   \
    ASSERT_EQ(0, status.code()) << status.message(); \
  }

TEST_F(Mp4v2DemuxerTest, readTrackMetadata) {
  nlohmann::json muxerConfig;
  Mp4v2Demuxer demuxer(muxerConfig);
  ASSERT_MEDIA(demuxer.open("c8/media/testdata/reference-0.mp4"));

  Vector<nlohmann::json> tracks;
  ASSERT_MEDIA(demuxer.forEachTrack([&tracks](const nlohmann::json &trackInfo) -> MediaStatus {
    tracks.push_back(trackInfo);
    return {};
  }));

  EXPECT_EQ(2, tracks.size());

  EXPECT_EQ(tracks[0], nlohmann::json::parse(R"({
    "name": "audio",
    "codec": "aac",
    "duration": 162816,
    "profile": 64,
    "samples": 159,
    "timescale": 44100
  })"));

  EXPECT_EQ(tracks[1], nlohmann::json::parse(R"({
    "name": "video",
    "codec": "h264",
    "duration": 384000,
    "width": 480,
    "profile": 66,
    "level": 42,
    "samples": 128,
    "timescale": 90000,
    "height": 640,
    "width": 480
  })"));

  ASSERT_MEDIA(demuxer.close());
}

TEST_F(Mp4v2DemuxerTest, readAudioVideoTracks) {
  nlohmann::json muxerConfig;
  Mp4v2Demuxer demuxer(muxerConfig);
  ASSERT_MEDIA(demuxer.open("c8/media/testdata/reference-0.mp4"));

  const uint8_t *data = nullptr;
  size_t byteSize;
  nlohmann::json sampleMetadata;

  MediaStatus result;

  int videoFrames = -1;
  int audioSamples = -1;

  // Read all of the frames in the video track.
  while (result.code() == MediaStatus::SUCCESS) {
    result = demuxer.read(
      nlohmann::json::parse(R"({
        "name": "video"
      })"),
      &data,
      &byteSize,
      &sampleMetadata);
    ++videoFrames;
    if (result.code() == MediaStatus::SUCCESS) {
      EXPECT_EQ(3000, sampleMetadata["frameDuration"].get<int>());
      EXPECT_EQ(3000 * videoFrames, sampleMetadata["timestamp"].get<int>());
      EXPECT_EQ(0, sampleMetadata["renderingOffset"].get<int>());
      EXPECT_EQ(videoFrames == 0, sampleMetadata["syncFrame"].get<int>());
    }
  }
  EXPECT_EQ(MediaStatus::NO_MORE_FRAMES, result.code()) << result.message();
  EXPECT_EQ(128, videoFrames);

  // Read all of the samples in the audio track.
  result = {};
  while (result.code() == MediaStatus::SUCCESS) {
    result = demuxer.read(
      nlohmann::json::parse(R"({
        "name": "audio"
      })"),
      &data,
      &byteSize,
      &sampleMetadata);
    ++audioSamples;
    if (result.code() == MediaStatus::SUCCESS) {
      EXPECT_EQ(1024, sampleMetadata["frameDuration"].get<int>());
      EXPECT_EQ(1024 * audioSamples, sampleMetadata["timestamp"].get<int>());
      EXPECT_EQ(0, sampleMetadata["renderingOffset"].get<int>());
      EXPECT_TRUE(sampleMetadata["syncFrame"].get<int>());
    }
  }
  EXPECT_EQ(MediaStatus::NO_MORE_FRAMES, result.code()) << result.message();
  EXPECT_EQ(159, audioSamples);

  ASSERT_MEDIA(demuxer.close());
}

}  // namespace c8
