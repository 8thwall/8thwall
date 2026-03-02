// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Interfaces for a media codec and factory.

#pragma once

#include <cstdint>
#include <functional>

#include <nlohmann/json_fwd.hpp>

#include "c8/media/media-status.h"

namespace c8 {

class Demuxer;
class Muxer;

class DecoderApi {
public:
  // Virtual destructor.
  virtual ~DecoderApi() {}

  // Initialize decoder and read any initial data from the Demuxer. Demuxer should be
  // 'open' and remain so until after the call to finish. The DecoderApi does
  // not own the Demuxer.
  virtual MediaStatus start(Demuxer *demuxer) = 0;

  // Decode a sample/frame of data. The decoding buffer is owned by the
  // class implementing DecoderApi, and remains valid until another call to the
  // api methods.
  //
  // When available and applicable, the implementing class should return each of
  // following metadata fields:
  //   format (string) - The ffmpeg-style video format, such as 'yuv420p'.
  //   timestamp (int) - The input timestamp of the sample/frame in timescale units.
  //   renderingOffset (int) - The offset between the input timestamp and the rendering timestamp.
  //   frameDuration (int) - The duration of the frame in timescape units.
  //   syncFrame (bool) - Whether this is a 'sync' frame, i.e., one that can be decoded by itself.
  //   width (int) - The width of a video frame.
  //   height (int) - The height of a video frame.
  //   data0 (uintptr_t) - A pointer to the first channel in multi-channel video.
  //   data1 (uintptr_t) - A pointer to the second channel in multi-channel video.
  //   data2 (uintptr_t) - A pointer to the third channel in multi-channel video.
  //   rowBytes0 (int) - The length in bytes of a row in the first channel in multi-channel video.
  //   rowBytes1 (int) - The length in bytes of a row in the second channel in multi-channel video.
  //   rowBytes2 (int) - The length in bytes of a row in the third channel in multi-channel video.
  virtual MediaStatus decode(
    const nlohmann::json &sampleConfig,
    const uint8_t **data,
    size_t *byteSize,
    nlohmann::json *sampleMetadata) = 0;

  // Finish decoding and clean up the decoder.
  virtual MediaStatus finish() = 0;
};

class EncoderApi {
public:
  // Virtual destructor.
  virtual ~EncoderApi() {}

  // Initialize encoder and write any initial data to the Muxer. Muxer should be
  // 'open' and remain so until after the call to finish. The EncoderApi does
  // not own the Muxer.
  virtual MediaStatus start(Muxer *muxer) = 0;

  // Encode new input data.
  virtual MediaStatus encode(
    const nlohmann::json &sampleConfig, const uint8_t *data, size_t byteSize) = 0;

  // Finish encoding and flush any remaining output to the muxer.
  virtual MediaStatus finish() = 0;
};

class MuxerApi {
public:
  // Virtual destructor.
  virtual ~MuxerApi() {}

  // Open new media file for muxing.
  virtual MediaStatus open(const char *path) = 0;

  // Add a new track to the muxer.
  virtual MediaStatus addTrack(const nlohmann::json &trackConfig) = 0;

  // The Codec should be calling write on the muxer...
  // Lazy create tracks on first write. Duration, packet timestamp, synchronized, rendering offset.
  virtual MediaStatus write(
    const nlohmann::json &writeConfig, const uint8_t *data, size_t byteSize) = 0;

  // Close and finish writing media file.
  virtual MediaStatus close() = 0;
};

class DemuxerApi {
public:
  // Virtual destructor.
  virtual ~DemuxerApi() {}

  // Open new media file for demuxing.
  virtual MediaStatus open(const char *path) = 0;

  // Run the provided callback once per track in the open media file, providing
  // the caller with information about each track.
  //
  // When available and applicable, the implementing class should return each of
  // following fields:
  //   name (string) - The identifier of the track used for future calls to the demuxer
  //   codec (string) - The unique name of the codec in registry.cc
  //   profile (int) - The profile identifier of the codec when applicable
  //   level (int) - The level identifier of the codec when applicable
  //   timescale (int) - The timescale units of the track
  //   samples (int) - The number of samples in the drack
  //   duration (int) - The duration of the full track in timescale units
  //   width (int) - The width of a video track as written in the media container.
  //   height (int) - The height of a video track as written in the media container.
  virtual MediaStatus forEachTrack(std::function<MediaStatus(const nlohmann::json &)> callback) = 0;

  // Read a sample/frame of data from a track in the demuxer. The read buffer is owned by the
  // class implementing DemuxerApi, and remains valid until another call to the api methods.
  //
  // When available and applicable, the implementing class should return each of
  // following metadata fields:
  //   timestamp (int) - The input timestamp of the sample/frame in timescale units.
  //   renderingOffset (int) - The offset between the input timestamp and the rendering timestamp.
  //   frameDuration (int) - The duration of the frame in timescape units.
  //   syncFrame (bool) - Whether this is a 'sync' frame, i.e., one that can be decoded by itself.
  virtual MediaStatus read(
    const nlohmann::json &readConfig,
    const uint8_t **data,
    size_t *byteSize,
    nlohmann::json *sampleMetadata) = 0;

  // Close the media file.
  virtual MediaStatus close() = 0;
};

}  // namespace c8
