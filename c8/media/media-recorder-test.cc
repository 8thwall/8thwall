// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":media-recorder",
    "//c8/pixels:pixels",
    "//c8/pixels:pixel-buffer",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x18fe7062);

#include "c8/media/media-recorder.h"

#include "c8/pixels/pixel-buffer.h"
#include "c8/pixels/pixels.h"

#include "gtest/gtest.h"

namespace c8 {

class MediaRecorderTest : public ::testing::Test {};

#define ASSERT_MEDIA(result)                         \
  {                                                  \
    MediaStatus status = (result);                   \
    ASSERT_EQ(0, status.code()) << status.message(); \
  }

TEST_F(MediaRecorderTest, TestFixedFramerateMp4Recording) {
  MediaRecorder recorder;
  ASSERT_MEDIA(recorder.start(R"({
    "path": "/tmp/fixed.mp4",
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
      "width": 640,
      "height": 480,
      "bitrate": 5000000
    }, {
      "name": "audio",
      "codec": "aac",
      "profile": 2,
      "sampleRate": 44100
    }]
  })"));

  int rows = 480;
  int cols = 640;

  YPlanePixelBuffer yBuffer(rows, cols);
  PixelBuffer<UPlanePixels, 1> uBuffer(rows >> 1, cols >> 1);
  PixelBuffer<VPlanePixels, 1> vBuffer(rows >> 1, cols >> 1);

  ASSERT_MEDIA(recorder.encodePlanarYUV(
    R"({"name": "video"})", yBuffer.pixels(), uBuffer.pixels(), vBuffer.pixels()));

  uint16_t audioData[20000] = {};
  ASSERT_MEDIA(recorder.encode(
    R"({"name": "audio"})", reinterpret_cast<unsigned char *>(audioData), sizeof(audioData)));

  ASSERT_MEDIA(recorder.finish());
}

TEST_F(MediaRecorderTest, TestVariableFramerateMp4Recording) {
  MediaRecorder recorder;
  ASSERT_MEDIA(recorder.start(R"({
    "path": "/tmp/variable.mp4",
    "container": "mp4",
    "timescale": 90000,
    "tracks": [{
      "name": "video",
      "codec": "h264",
      "format": "yuv420p",
      "profile": 66,
      "timescale": 90000,
      "qp": 23,
      "width": 640,
      "height": 480,
      "bitrate": 5000000
    }, {
      "name": "audio",
      "codec": "aac",
      "profile": 5,
      "sampleRate": 44100
    }]
  })"));

  int rows = 480;
  int cols = 640;

  YPlanePixelBuffer yBuffer(rows, cols);
  PixelBuffer<UPlanePixels, 1> uBuffer(rows >> 1, cols >> 1);
  PixelBuffer<VPlanePixels, 1> vBuffer(rows >> 1, cols >> 1);

  // Encode 4 frames at t=[0s, 0.5s, 2s, 5s]. The last frame duration should be 2s.
  ASSERT_MEDIA(recorder.encodePlanarYUV(
    R"({"name": "video", "timestamp": 0})", yBuffer.pixels(), uBuffer.pixels(), vBuffer.pixels()));
  ASSERT_MEDIA(recorder.encodePlanarYUV(
    R"({"name": "video", "timestamp": 45000})",
    yBuffer.pixels(),
    uBuffer.pixels(),
    vBuffer.pixels()));
  ASSERT_MEDIA(recorder.encodePlanarYUV(
    R"({"name": "video", "timestamp": 180000})",
    yBuffer.pixels(),
    uBuffer.pixels(),
    vBuffer.pixels()));
  ASSERT_MEDIA(recorder.encodePlanarYUV(
    R"({"name": "video", "timestamp": 450000, "frameDuration": 180000})",
    yBuffer.pixels(),
    uBuffer.pixels(),
    vBuffer.pixels()));

  uint16_t audioData[20000] = {};
  ASSERT_MEDIA(recorder.encode(
    R"({"name": "audio"})", reinterpret_cast<unsigned char *>(audioData), sizeof(audioData)));

  ASSERT_MEDIA(recorder.finish());
}

}  // namespace c8
