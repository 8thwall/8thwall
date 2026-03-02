// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":media-decoder",
    "//c8/pixels:pixels",
    "//c8/pixels:pixel-buffer",
    "//c8:string",
    "//c8:vector",
    "@com_google_googletest//:gtest_main",
  };
  data = {
    "//c8/media/testdata:reference-media",
  };
}
cc_end(0xf11d60ff);

#include "c8/media/media-decoder.h"

#include "c8/pixels/pixel-buffer.h"
#include "c8/pixels/pixels.h"

#include "c8/string.h"
#include "c8/vector.h"

#include "gtest/gtest.h"

namespace c8 {

class MediaDecoderTest : public ::testing::Test {};

#define ASSERT_MEDIA(result)                         \
  {                                                  \
    MediaStatus status = (result);                   \
    ASSERT_EQ(0, status.code()) << status.message(); \
  }

TEST_F(MediaDecoderTest, TestMp4Decoding) {
  MediaDecoder decoder;
  ASSERT_MEDIA(decoder.open(R"({
    "path": "c8/media/testdata/reference-0.mp4"
  })"));

  Vector<String> tracks;
  decoder.forEachTrack([&tracks](const char *track) -> MediaStatus {
    tracks.push_back(track);
    return {};
  });
  EXPECT_EQ(2, tracks.size());

  const uint8_t *data = nullptr;
  size_t byteSize;
  String metadata;

  int audioSamples = -1;
  int videoFrames = -1;

  // Decode audio track.
  MediaStatus result;
  while (result.code() == MediaStatus::SUCCESS) {
    result = decoder.decode(R"({ "name": "audio" })", &data, &byteSize, &metadata);
    audioSamples++;
  }
  EXPECT_EQ(MediaStatus::NO_MORE_FRAMES, result.code()) << result.message();
  EXPECT_EQ(159, audioSamples);

  // Decode video track.
  result = {};
  ConstYPlanePixels yPlane;
  ConstUPlanePixels uPlane;
  ConstVPlanePixels vPlane;
  while (result.code() == MediaStatus::SUCCESS) {
    result = decoder.decodePlanarYUV(R"({"name": "video"})", &yPlane, &uPlane, &vPlane, &metadata);
    EXPECT_EQ(640, yPlane.rows());
    EXPECT_EQ(480, yPlane.cols());
    EXPECT_EQ(320, uPlane.rows());
    EXPECT_EQ(240, uPlane.cols());
    EXPECT_EQ(320, vPlane.rows());
    EXPECT_EQ(240, vPlane.cols());
    videoFrames++;
  }
  EXPECT_EQ(MediaStatus::NO_MORE_FRAMES, result.code()) << result.message();
  EXPECT_EQ(128, videoFrames);

  // Close the decoder.
  ASSERT_MEDIA(decoder.close());
}

TEST_F(MediaDecoderTest, TestMkvDecoding) {
  MediaDecoder decoder;
  ASSERT_MEDIA(decoder.open(R"({
    "path": "c8/media/testdata/reference-0.mkv"
  })"));

  Vector<String> tracks;
  decoder.forEachTrack([&tracks](const char *track) -> MediaStatus {
    tracks.push_back(track);
    return {};
  });
  EXPECT_EQ(2, tracks.size());

  const uint8_t *data = nullptr;
  size_t byteSize;
  String metadata;

  int audioSamples = -1;
  int videoFrames = -1;

  // Decode audio track.
  MediaStatus result;
  while (result.code() == MediaStatus::SUCCESS) {
    result = decoder.decode(R"({ "name": "audio" })", &data, &byteSize, &metadata);
    audioSamples++;
  }
  EXPECT_EQ(MediaStatus::NO_MORE_FRAMES, result.code()) << result.message();
  EXPECT_EQ(159, audioSamples);

  // Decode video track.
  result = {};
  ConstYPlanePixels yPlane;
  ConstUPlanePixels uPlane;
  ConstVPlanePixels vPlane;
  while (result.code() == MediaStatus::SUCCESS) {
    result = decoder.decodePlanarYUV(R"({"name": "video"})", &yPlane, &uPlane, &vPlane, &metadata);
    EXPECT_EQ(640, yPlane.rows());
    EXPECT_EQ(480, yPlane.cols());
    EXPECT_EQ(320, uPlane.rows());
    EXPECT_EQ(240, uPlane.cols());
    EXPECT_EQ(320, vPlane.rows());
    EXPECT_EQ(240, vPlane.cols());
    videoFrames++;
  }
  EXPECT_EQ(MediaStatus::NO_MORE_FRAMES, result.code()) << result.message();
  EXPECT_EQ(128, videoFrames);

  // Close the decoder.
  ASSERT_MEDIA(decoder.close());
}

}  // namespace c8
