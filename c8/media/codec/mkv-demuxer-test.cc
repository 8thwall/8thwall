// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":mkv-demuxer",
    "@com_google_googletest//:gtest_main",
    "@json//:json",
  };
  data = {
    "//c8/media/testdata:reference-media",
  };
}
cc_end(0x9129b522);

#include "gtest/gtest.h"
#include "c8/media/codec/mkv-demuxer.h"

#include <nlohmann/json.hpp>

namespace c8 {

class MkvDemuxerTest : public ::testing::Test {};

#define ASSERT_MEDIA(result)                         \
  {                                                  \
    MediaStatus status = (result);                   \
    ASSERT_EQ(0, status.code()) << status.message(); \
  }

TEST_F(MkvDemuxerTest, readTrackMetadata) {
  nlohmann::json muxerConfig;
  MkvDemuxer demuxer(muxerConfig);
  ASSERT_MEDIA(demuxer.open("c8/media/testdata/big-buck-bunny.webm"));

  Vector<nlohmann::json> tracks;
  ASSERT_MEDIA(demuxer.forEachTrack([&tracks](const nlohmann::json &trackInfo) -> MediaStatus {
    tracks.push_back(trackInfo);
    return {};
  }));

  EXPECT_EQ(2, tracks.size());

  EXPECT_EQ(tracks[0], nlohmann::json::parse(R"({
    "name": "audio",
    "codec": "vorbis",
    "sampleRate": 44100,
    "timescale": 44100,
    "duration": 1432853,
    "channels": 1,
    "samples":1496
  })"));

  EXPECT_EQ(tracks[1], nlohmann::json::parse(R"({
    "name": "video",
    "codec": "vp8",
    "timescale": 1000000,
    "height": 360,
    "width": 640,
    "samples": 812,
    "duration": 32480000,
    "fps": 25.0
  })"));

  ASSERT_MEDIA(demuxer.close());
}

TEST_F(MkvDemuxerTest, readAudioVideoTracks) {
  nlohmann::json muxerConfig;
  MkvDemuxer demuxer(muxerConfig);
  ASSERT_MEDIA(demuxer.open("c8/media/testdata/big-buck-bunny.webm"));

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
      EXPECT_EQ(40000, sampleMetadata["frameDuration"].get<int>());
      EXPECT_EQ(40000 * videoFrames, sampleMetadata["timestamp"].get<int>());
      EXPECT_EQ(0, sampleMetadata["renderingOffset"].get<int>());
      EXPECT_LT(0, sampleMetadata.count("syncFrame"));
    }
  }
  EXPECT_EQ(MediaStatus::NO_MORE_FRAMES, result.code()) << result.message();
  EXPECT_EQ(812, videoFrames);

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
      EXPECT_EQ(0, sampleMetadata["renderingOffset"].get<int>());
      EXPECT_TRUE(sampleMetadata["syncFrame"].get<int>());
    }
  }
  EXPECT_EQ(MediaStatus::NO_MORE_FRAMES, result.code()) << result.message();
  EXPECT_EQ(1496, audioSamples);

  ASSERT_MEDIA(demuxer.close());
}

}  // namespace c8
