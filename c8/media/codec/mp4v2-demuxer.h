// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Demuxer implementation for Mp4v2.

#pragma once

#include <mp4v2/mp4v2.h>
#include <nlohmann/json.hpp>

#include "c8/map.h"
#include "c8/string.h"
#include "c8/vector.h"
#include "c8/media/codec/codec-api.h"

namespace c8 {

class Mp4v2Demuxer : public DemuxerApi {
public:
  // Construct demuxer with config.
  Mp4v2Demuxer(const nlohmann::json &config);

  // Virtual destructor.
  virtual ~Mp4v2Demuxer() override;

  // Default move constructors.
  Mp4v2Demuxer(Mp4v2Demuxer &&) = default;
  Mp4v2Demuxer &operator=(Mp4v2Demuxer &&) = default;

  // Disallow copying.
  Mp4v2Demuxer(const Mp4v2Demuxer &) = delete;
  Mp4v2Demuxer &operator=(const Mp4v2Demuxer &) = delete;

  // Open new media file for demuxing.
  virtual MediaStatus open(const char *path) override;

  // Call the provided callback once per track.
  virtual MediaStatus forEachTrack(
    std::function<MediaStatus(const nlohmann::json &)> callback) override;

  // Get a pointer to a sample managed by the Demuxer.
  virtual MediaStatus read(
    const nlohmann::json &readConfig,
    const uint8_t **data,
    size_t *byteSize,
    nlohmann::json *sampleMetadata) override;

  // Close and finish writing media file.
  virtual MediaStatus close() override;

private:
  nlohmann::json config_;

  struct TrackConfig {
    TrackConfig(MP4TrackId inId) : id(inId) {}

    ~TrackConfig() {
      if (spsHeaders || ppsHeaders || spsHeaderSizes || ppsHeaderSizes) {
        // TODO(mc): Find best way to handle this with move wrapper.
        // MP4FreeH264SeqPictHeaders(spsHeaders, spsHeaderSizes, ppsHeaders, ppsHeaderSizes);
      }
    }

    // Default move constructors.
    TrackConfig(TrackConfig &&) = default;
    TrackConfig &operator=(TrackConfig &&) = default;

    // Disallow copying.
    TrackConfig(const TrackConfig &) = delete;
    TrackConfig &operator=(const TrackConfig &) = delete;

    MP4TrackId id;
    String codec = "";
    uint32_t timescale = 0;

    // Duration of track in track timescale.
    MP4Duration duration = MP4_INVALID_DURATION;
    uint8_t profile = 0;
    uint8_t level = 0;

    MP4SampleId samples = 0;
    MP4SampleId currentSample = 0;

    Vector<uint8_t> buffer;

    // This is the MP4 container's video width and height metadata. It may
    // differ from the codec's knowledge of video frame dimensions.
    uint16_t width = 0;
    uint16_t height = 0;

    // SPS and PPS headers for video tracks.
    uint8_t **spsHeaders = nullptr;
    uint32_t *spsHeaderSizes = nullptr;
    uint8_t **ppsHeaders = nullptr;
    uint32_t *ppsHeaderSizes = nullptr;
  };

  String path_;

  MP4FileHandle handle_;

  TreeMap<String, TrackConfig> tracks_;
};

}  // namespace c8
