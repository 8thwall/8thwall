// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":mkv-demuxer",
    ":opus-decoder",
    "//c8/media:demuxer",
    "@com_google_googletest//:gtest_main",
    "@json//:json",
  };
  data = {
    "//c8/media/testdata:reference-media",
  };
}
cc_end(0x80d7f9c4);

#include "c8/media/codec/opus-decoder.h"

#include "gtest/gtest.h"
#include "c8/media/codec/mkv-demuxer.h"
#include "c8/media/demuxer.h"

#include <nlohmann/json.hpp>

namespace c8 {

class OpusDecoderTest : public ::testing::Test {};

#define ASSERT_MEDIA(result)                         \
  {                                                  \
    MediaStatus status = (result);                   \
    ASSERT_EQ(0, status.code()) << status.message(); \
  }

TEST_F(OpusDecoderTest, decodeFromWebm) {
  nlohmann::json muxerConfig;
  nlohmann::json decoderConfig = nlohmann::json::parse(R"({
    "name": "audio"
  })");
  Demuxer demuxer(new MkvDemuxer(muxerConfig));
  OpusDecoder decoder(decoderConfig);

  // Open the .webm file for reading.
  ASSERT_MEDIA(demuxer.open("c8/media/testdata/reference-vp9-opus.webm"));

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
      EXPECT_EQ(960, sampleMetadata["frameDuration"].get<int>());
      EXPECT_EQ(1, sampleMetadata["channels"].get<int>());
      EXPECT_TRUE(sampleMetadata["syncFrame"].get<int>());
    }
  }
  EXPECT_EQ(MediaStatus::NO_MORE_FRAMES, result.code()) << result.message();
  EXPECT_EQ(185, audioSamples);

  ASSERT_MEDIA(decoder.finish());
  ASSERT_MEDIA(demuxer.close());
}

}  // namespace c8
