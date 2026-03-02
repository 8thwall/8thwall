// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":mp4v2-demuxer",
    ":openh264-decoder",
    "//c8/media:demuxer",
    "@com_google_googletest//:gtest_main",
    "@json//:json",
  };
  data = {
    "//c8/media/testdata:reference-media",
  };
}
cc_end(0xde958e05);

#include "c8/media/codec/openh264-decoder.h"

#include "gtest/gtest.h"
#include "c8/media/codec/mp4v2-demuxer.h"
#include "c8/media/demuxer.h"

#include <nlohmann/json.hpp>

namespace c8 {

class OpenH264DecoderTest : public ::testing::Test {};

#define ASSERT_MEDIA(result)                         \
  {                                                  \
    MediaStatus status = (result);                   \
    ASSERT_EQ(0, status.code()) << status.message(); \
  }

TEST_F(OpenH264DecoderTest, decodeFromMp4) {
  nlohmann::json muxerConfig;
  nlohmann::json decoderConfig = nlohmann::json::parse(R"({
    "name": "video"
  })");
  Demuxer demuxer(new Mp4v2Demuxer(muxerConfig));
  OpenH264Decoder decoder(decoderConfig);

  // Open the .mp4 file for reading.
  ASSERT_MEDIA(demuxer.open("c8/media/testdata/reference-0.mp4"));

  // Start the decoder.
  ASSERT_MEDIA(decoder.start(&demuxer));

  const uint8_t *data = nullptr;
  size_t byteSize;
  nlohmann::json sampleMetadata;

  MediaStatus result;

  int videoFrames = -1;

  // Read all of the frames in the video track.
  while (result.code() == MediaStatus::SUCCESS) {
    result = decoder.decode(nlohmann::json::parse(R"({})"), &data, &byteSize, &sampleMetadata);
    ++videoFrames;
    if (result.code() == MediaStatus::SUCCESS) {
      EXPECT_STREQ("yuv420p", sampleMetadata["format"].get<std::string>().c_str());
      EXPECT_EQ(3000, sampleMetadata["frameDuration"].get<int>());

      const uintptr_t data0 = sampleMetadata["data0"].get<uintptr_t>();
      const uintptr_t data1 = sampleMetadata["data1"].get<uintptr_t>();
      const uintptr_t data2 = sampleMetadata["data2"].get<uintptr_t>();

      const int rowBytes0 = sampleMetadata["rowBytes0"].get<int>();
      const int rowBytes1 = sampleMetadata["rowBytes1"].get<int>();
      const int rowBytes2 = sampleMetadata["rowBytes2"].get<int>();

      const int height = sampleMetadata["height"].get<int>();

      EXPECT_EQ(640, height);
      EXPECT_EQ(480, sampleMetadata["width"].get<int>());

      uintptr_t end = reinterpret_cast<uintptr_t>(data) + byteSize;

      EXPECT_LE(reinterpret_cast<uintptr_t>(data), data0);
      EXPECT_LE(reinterpret_cast<uintptr_t>(data), data1);
      EXPECT_LE(reinterpret_cast<uintptr_t>(data), data2);

      EXPECT_GE(end, data0 + rowBytes0 * height);
      EXPECT_GE(end, data1 + rowBytes1 * (height >> 1));
      EXPECT_GE(end, data2 + rowBytes2 * (height >> 1));

      EXPECT_LE(480, sampleMetadata["rowBytes0"].get<int>());
      EXPECT_LE(240, sampleMetadata["rowBytes1"].get<int>());
      EXPECT_LE(240, sampleMetadata["rowBytes2"].get<int>());

      EXPECT_EQ(3000 * videoFrames, sampleMetadata["timestamp"].get<uint64_t>());
      EXPECT_EQ(0, sampleMetadata["renderingOffset"].get<int>());
      EXPECT_EQ(videoFrames == 0, sampleMetadata["syncFrame"].get<int>());
    }
  }
  EXPECT_EQ(MediaStatus::NO_MORE_FRAMES, result.code()) << result.message();
  EXPECT_EQ(128, videoFrames);

  ASSERT_MEDIA(decoder.finish());
  ASSERT_MEDIA(demuxer.close());
}

}  // namespace c8
