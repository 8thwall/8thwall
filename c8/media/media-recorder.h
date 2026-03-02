// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// MediaRecorder is an extensible video creation library, intended to be lightweight and
// client-side. The video and audio formats are specified with JSON strings, along with encoding
// perameters. At its inception, MediaRecorder had support for creating .mp4 files with H.264 video
// and AAC audio.
//
// Example Usage:
//
//   #define CHECK_MEDIA(result)                              \
//     {                                                      \
//      MediaStatus status = (result);                        \
//      if (status.code() != 0) {                             \
//        C8Log("MediaRecorder error: %s", status.message()); \
//      }                                                     \
//     }
//
//    MediaRecorder recorder;
//
//    // Start the media recorder.
//    CHECK_MEDIA(recorder.start(R"({
//      "path": "/tmp/hello.mp4",  // Output video location.
//      "container": "mp4",        // Container type.
//      "timescale": 90000,        // Timescale for video container (units per second).
//      "optimize": true,          // Rewrite the finished video for more optimal playback.
//      "tracks": [{               // List of tracks
//        "name": "video",         // Track name and identifier.
//        "codec": "h264",         // Video codec type.
//        "format": "yuv420p",     // Input video format, yuv420p = YUV420 planar.
//        "profile": 66,           // Codec profile, for H.264, 66 = 'Baseline'
//        "timescale": 90000,      // Video track timescale (units per second).
//        "qp": 23,                // Quality parameter for Quality-based rate limiting.
//        "fps": 30,               // Framerate when using fixed-duration frames.
//        "width": 640,            // Width of the output video. Should match input.
//        "height": 480,           // Height of the output video. Should match input.
//        "bitrate": 5000000       // Target bitrate using provided rate control method.
//      }, {
//        "name": "audio",         // Track name and identifier.
//        "codec": "aac",          // Audio Codec type.
//        "profile": 2,            // Codec profile, for AAC, 2 = 'LC'
//        "sampleRate": 44100,     // Sample Rate for audio. Track timescale will be set to match.
//        "bitrate": 131072        // Target bitrate.
//      }]
//    })"));
//
//    // Encode a video frame in the media recorder.
//    int rows = 480;
//    int cols = 640;
//    YPlanePixelBuffer yBuffer(rows, cols);
//    PixelBuffer<UPlanePixels, 1> uBuffer(rows >> 1, cols >> 1);
//    PixelBuffer<VPlanePixels, 1> vBuffer(rows >> 1, cols >> 1);
//    CHECK_MEDIA(recorder.encodePlanarYUV(R"({"name": "video"})", yBuffer.pixels(),
//    uBuffer.pixels(), vBuffer.pixels()));
//
//    // Encode an audio frame in the media recorder.
//    CHECK_MEDIA(recorder.encode(R"({"name": "audio"})", reinterpret_cast<unsigned char
//    *>(audioData), sizeof(audioData)));
//
//    // Finish the recording.
//    ASSERT_MEDIA(recorder.finish())

#pragma once

#include "c8/map.h"
#include "c8/pixels/pixels.h"
#include "c8/string-view.h"
#include "c8/string.h"

#include "c8/media/encoder.h"
#include "c8/media/media-status.h"
#include "c8/media/muxer.h"

namespace c8 {

class MediaRecorder {
public:
  MediaRecorder() = default;

  ~MediaRecorder();

  // Start the MediaRecorder with a JSON configuration.
  // The 'params' field is a JSON string, containing parameters to pass to the encoder.
  // Here are some common options:
  //   "name": (required string) matching the track name in which to encode
  //   "container": (required string) the type of container format to use.
  //   "frameDuration": (optional integer) frame duration in track timescale units, if unspecified,
  //                     it will use the fps or frameDuration from the track configuration.
  //   "optimize": (optional boolean) Rewrite the finished video for more optimal playback.
  //   "timescale": (optional integer) Timescale units for the media file. Defaults to 90000.
  //   "path": (required string) output video location.
  //   "tracks": (required List<Object>) configuration information for each track.
  //
  // Here are some common track options:
  //   "name": (required string) name and identifier for the track
  //   "codec": (required string) codec type, e.g. ("h264" or "aac").
  //   "format": (optional string) input video format, yuv420p = YUV420 planar. Currently supports
  //   'yuv420p'. "profile": (optional int) codec profile code. For example H.264 has [66 =
  //   'Baseline', 77 = 'Main', 100 = 'High'].
  //                             AAC has profiles like 2='AAC-LC', 5='HE-AAC', 29='HE-AACv2',
  //                             23='LD', 39='ELD'.
  //   "timescale": (optional int) track timescale (units per second). Defaults to encoder defaults.
  //   "fps": (optional int) framerate when using fixed-duration frames.
  //   "frameDuration": (optional int) used instead of fps to specify frame duration in timesale
  //                    units.
  //   "width": (required int for video) width of the output video.
  //   "height": (required int for video) height of the output video.
  //   "bitrate": (optional int) target bitrate passed to the encoder.
  //   "sampleRate": (optional int) sampling rate for audio tracks. Track timescale will be
  //                 set to match if not set.
  //
  //   Here are some H.264 track params:
  //   "rcMode": (optional int) H.264 rate control model. Default is 0 == quality-based rate
  //             limiting.
  //   "qp": (optional int) quality parameter for Quality-based rate limiting in H.264.
  //         Default=23.
  //   "minQp": (optional int) lower bound on quality param. Default=12.
  //   "maxQp": (optional int) upper bound on quality param. Default=42.
  //   "usageType": (optional int) default is 0 == CAMERA_VIDEO_REAL_TIME.
  //   "enableDenoise": (optional bool) enable denoising. Default=false.
  //   "useLoadBalancing": (optional bool) use load balancing. Default=false.
  //   "enableSceneChangeDetect": (optional bool) enable scene change detection. Default=false.
  //   "enableBackgroundDetection": (optional bool) enable background detection. Default=false.
  //   "enableAdaptiveQuant": (optional bool) enable adaptive quantization. Default=false.
  //   "enableFrameSkip": (optional bool) allow frames to be dropped if bitrate can't be achieved.
  //                      Default=true, since required for quality-based rate limiting.
  MediaStatus start(StringView config);

  // Encode an block of input data.
  // The 'params' field is a JSON string, containing parameters to pass to the encoder.
  // It supports the following fields:
  //   "name" - (required string) matching the track name in which to encode
  //   "timestamp" - (optional integer) the frame start time for variable-framerate encoding in
  //                 track timescale units.
  //   "frameDuration" - (optional integer) frame duration in track timescale units, if unspecified,
  //                     it will use the fps or frameDuration from the track configuration.
  MediaStatus encode(StringView params, const uint8_t *data, size_t byteSize);

  // Encode a planar YUV format. Helpful for video formats like 'yuv420p'.
  // The 'params' field is a JSON string, containing parameters to pass to the encoder.
  // It supports the following fields:
  //   "name": (required string) matching the track name in which to encode
  //   "timestamp": (optional integer) the frame start time for variable-framerate encoding in
  //                 track timescale units.
  //   "frameDuration" - (optional integer) frame duration in track timescale units, if unspecified,
  //                     it will use the fps or frameDuration from the track configuration.
  MediaStatus encodePlanarYUV(
    StringView params, ConstYPlanePixels y, ConstUPlanePixels u, ConstVPlanePixels v);

  // Flush any remaining encoding and finish writing the media.
  MediaStatus finish();

  // Default move constructors.
  MediaRecorder(MediaRecorder &&) = default;
  MediaRecorder &operator=(MediaRecorder &&) = default;

  // Disallow copying.
  MediaRecorder(const MediaRecorder &) = delete;
  MediaRecorder &operator=(const MediaRecorder &) = delete;

private:
  Muxer muxer_;
  TreeMap<String, Encoder> encoders_;
};

}  // namespace c8
