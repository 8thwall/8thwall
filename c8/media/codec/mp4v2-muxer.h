// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Muxer implementation for Mp4v2.

#pragma once

#include <mp4v2/mp4v2.h>
#include <nlohmann/json.hpp>

#include "c8/map.h"
#include "c8/string.h"
#include "c8/media/codec/codec-api.h"

namespace c8 {

class Mp4v2Muxer : public MuxerApi {
public:
  // Construct muxer with config.
  Mp4v2Muxer(const nlohmann::json &config);

  // Virtual destructor.
  virtual ~Mp4v2Muxer() override;

  // Default move constructors.
  Mp4v2Muxer(Mp4v2Muxer &&) = default;
  Mp4v2Muxer &operator=(Mp4v2Muxer &&) = default;

  // Disallow copying.
  Mp4v2Muxer(const Mp4v2Muxer &) = delete;
  Mp4v2Muxer &operator=(const Mp4v2Muxer &) = delete;

  // Open new media file for muxing.
  virtual MediaStatus open(const char *path) override;

  // Add a new track to the muxer. Actual track creation in the muxer output may be lazily deferred
  // to first write.
  virtual MediaStatus addTrack(const nlohmann::json &trackConfig) override;

  // Write to the muxer, frequenty driven by an encoder.
  MediaStatus write(
    const nlohmann::json &writeConfig, const uint8_t *data, size_t byteSize) override;

  // Close and finish writing media file.
  MediaStatus close() override;

private:
  nlohmann::json config_;

  struct TrackConfig {
    TrackConfig(MP4TrackId inId, String inFormat, uint32_t inTimescale, MP4Duration inFrameDuration)
        : id(inId), format(inFormat), timescale(inTimescale), frameDuration(inFrameDuration) {}
    MP4TrackId id;
    String format;
    uint32_t timescale;
    // Duration of one sample in track timescale. Set to MP4_INVALID_DURATION to
    // indicate variable duration samples.
    MP4Duration frameDuration;

    // Used for video tracks.
    uint16_t width = 0;
    uint16_t height = 0;
  };

  String path_;

  MP4FileHandle handle_;

  TreeMap<String, TrackConfig> tracks_;
};

}  // namespace c8
