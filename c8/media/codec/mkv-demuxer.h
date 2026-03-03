// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Demuxer implementation for Mkv.

#pragma once

#include <mkvparser/mkvparser.h>
#include <mkvparser/mkvreader.h>
#include <nlohmann/json.hpp>

#include "c8/map.h"
#include "c8/string.h"
#include "c8/vector.h"
#include "c8/media/codec/codec-api.h"

namespace c8 {

class MkvDemuxer : public DemuxerApi {
public:
  // Construct demuxer with config.
  MkvDemuxer(const nlohmann::json &config);

  // Virtual destructor.
  virtual ~MkvDemuxer() override;

  // Default move constructors.
  MkvDemuxer(MkvDemuxer &&) = default;
  MkvDemuxer &operator=(MkvDemuxer &&) = default;

  // Disallow copying.
  MkvDemuxer(const MkvDemuxer &) = delete;
  MkvDemuxer &operator=(const MkvDemuxer &) = delete;

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
    TrackConfig(long long inId) : id(inId) {}

    ~TrackConfig() {
    }

    // Default move constructors.
    TrackConfig(TrackConfig &&) = default;
    TrackConfig &operator=(TrackConfig &&) = default;

    // Disallow copying.
    TrackConfig(const TrackConfig &) = delete;
    TrackConfig &operator=(const TrackConfig &) = delete;

    long long id = 0;

    String codec = "";
    long long timescale = 0;
    // Duration of track in track timescale.
    long long duration = 0;

    unsigned long long defaultFrameDuration = 0;

    enum State {UNSTARTED, ADVANCING, FINISHED};
    State state = UNSTARTED;

    unsigned long samples = 0;
    unsigned long currentSample = 0;

    long long width = 0;
    long long height = 0;
    double fps = 0.0;
    int samplingRate = 0;
    long long channels = 0;
    long long bitDepth = 0;

    const mkvparser::Cluster* cluster = nullptr;
    const mkvparser::BlockEntry* blockEntry = nullptr;
    const mkvparser::Block* block = nullptr;
    int blockFrame = 0;

    Vector<uint8_t> buffer;
    size_t sampleOffset = 0;

    uint8_t profile = 0;
    uint8_t level = 0;
  };

  MediaStatus advanceTrack(TrackConfig& track);

  String path_;

  std::unique_ptr<mkvparser::MkvReader> reader_;
  std::unique_ptr<mkvparser::Segment> segment_;

  TreeMap<String, TrackConfig> tracks_;
};

}  // namespace c8
