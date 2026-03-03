// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":mp4v2-demuxer",
    ":fdk-aac-decoder",
    "//c8/media:demuxer",
    "@com_google_googletest//:gtest_main",
    "@json//:json",
  };
  data = {
    "//c8/media/testdata:reference-media",
  };
}
cc_end(0x73b7276a);

#include "c8/media/codec/fdk-aac-decoder.h"

#include "gtest/gtest.h"
#include "c8/media/codec/mp4v2-demuxer.h"
#include "c8/media/demuxer.h"

#include <nlohmann/json.hpp>

namespace c8 {

class FdkAacDecoderTest : public ::testing::Test {};

#define ASSERT_MEDIA(result)                         \
  {                                                  \
    MediaStatus status = (result);                   \
    ASSERT_EQ(0, status.code()) << status.message(); \
  }

TEST_F(FdkAacDecoderTest, decodeFromMp4) {
  nlohmann::json muxerConfig;
  nlohmann::json decoderConfig = nlohmann::json::parse(R"({
    "name": "audio"
  })");
  Demuxer demuxer(new Mp4v2Demuxer(muxerConfig));
  FdkAacDecoder decoder(decoderConfig);

  // Open the .mp4 file for reading.
  ASSERT_MEDIA(demuxer.open("c8/media/testdata/reference-0.mp4"));

  // Start the decoder.
  ASSERT_MEDIA(decoder.start(&demuxer));

  const uint8_t *data = nullptr;
  size_t byteSize;
  nlohmann::json sampleMetadata;

  MediaStatus result;

  int audioSamples = -1;

  // Read all of the frames in the audio track.
  while (result.code() == MediaStatus::SUCCESS) {
    result = decoder.decode(nlohmann::json::parse(R"({})"), &data, &byteSize, &sampleMetadata);
    ++audioSamples;
    if (result.code() == MediaStatus::SUCCESS) {
      // Expect PCM signed 16-bit little-endian format.
      EXPECT_STREQ("s16le", sampleMetadata["format"].get<std::string>().c_str());
      EXPECT_EQ(1024, sampleMetadata["frameDuration"].get<int>());
      EXPECT_EQ(1, sampleMetadata["channels"].get<int>());
      EXPECT_EQ(1024 * audioSamples, sampleMetadata["timestamp"].get<uint64_t>());
      EXPECT_EQ(0, sampleMetadata["renderingOffset"].get<int>());
      EXPECT_TRUE(sampleMetadata["syncFrame"].get<int>());
    }
  }
  EXPECT_EQ(MediaStatus::NO_MORE_FRAMES, result.code()) << result.message();
  EXPECT_EQ(159, audioSamples);

  ASSERT_MEDIA(decoder.finish());
  ASSERT_MEDIA(demuxer.close());
}

}  // namespace c8
