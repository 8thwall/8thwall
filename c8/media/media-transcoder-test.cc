// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":media-transcoder",
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
cc_end(0x9a8540f8);

#include "c8/media/media-transcoder.h"

#include "c8/pixels/pixel-buffer.h"
#include "c8/pixels/pixels.h"

#include "c8/string.h"
#include "c8/vector.h"

#include "gtest/gtest.h"

namespace c8 {

class MediaTranscoderTest : public ::testing::Test {};

#define ASSERT_MEDIA(result)                         \
  {                                                  \
    MediaStatus status = (result);                   \
    ASSERT_EQ(0, status.code()) << status.message(); \
  }

TEST_F(MediaTranscoderTest, TestMp4Transcoding) {
  MediaTranscoder transcoder;
  ASSERT_MEDIA(transcoder.open(
    R"({
    "path": "c8/media/testdata/reference-0.mp4"
  })",
    R"({
    "path": "/tmp/transcoded.mp4",
    "container": "mp4",
    "timescale": 90000,
    "optimize": true,
    "tracks": [{
      "name": "video",
      "codec": "h264",
      "format": "yuv420p",
      "profile": 66,
      "timescale": 90000,
      "qp": 23,
      "fps": 30,
      "width": 480,
      "height": 640,
      "bitrate": 5000000,
      "rcMode": 3
    }, {
      "name": "audio",
      "codec": "aac",
      "profile": 2,
      "sampleRate": 44100
    }]
  })"));

  int samples = -1;

  MediaStatus result;
  String metadata;
  while (result.code() == MediaStatus::SUCCESS) {
    result = transcoder.transcode(&metadata);
    samples++;
  }
  EXPECT_EQ(MediaStatus::NO_MORE_FRAMES, result.code()) << result.message();
  EXPECT_EQ(159 + 128, samples);

  // Close the transcoder.
  ASSERT_MEDIA(transcoder.close());
}

TEST_F(MediaTranscoderTest, TranscodeWebmToMp4) {
  MediaTranscoder transcoder;
  ASSERT_MEDIA(transcoder.open(
    R"({
    "path": "c8/media/testdata/reference-vp9-opus.webm"
  })",
    R"({
    "path": "/tmp/transcoded.mp4",
    "container": "mp4",
    "timescale": 90000,
    "optimize": true,
    "tracks": [{
      "name": "video",
      "codec": "h264",
      "format": "yuv420p",
      "profile": 66,
      "timescale": 90000,
      "qp": 23,
      "fps": 30,
      "width": 480,
      "height": 640,
      "bitrate": 5000000,
      "rcMode": 3
    }, {
      "name": "audio",
      "codec": "aac",
      "profile": 2,
      "sampleRate": 44100
    }]
  })"));

  int samples = -1;

  MediaStatus result;
  String metadata;
  while (result.code() == MediaStatus::SUCCESS) {
    result = transcoder.transcode(&metadata);
    samples++;
  }
  EXPECT_EQ(MediaStatus::NO_MORE_FRAMES, result.code()) << result.message();
  EXPECT_EQ(128 + 185, samples);

  // Close the transcoder.
  ASSERT_MEDIA(transcoder.close());
}

}  // namespace c8
