// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Demuxer class encapsulates the implementation of a media Demuxer.

#pragma once

#include "c8/media/media-status.h"

#include <nlohmann/json_fwd.hpp>

#include "c8/media/codec/codec-api.h"

namespace c8 {

class Demuxer {
public:
  // Default constructed Demuxer is unusable and can represent an invalid
  // constructed demuxer.
  Demuxer() = default;

  // Construct a Demuxer with an implementation of a DemuxerApi. The Demuxer
  // will take ownership of the DemuxerApi.
  Demuxer(DemuxerApi *muxer);

  // Default move constructors.
  Demuxer(Demuxer &&) = default;
  Demuxer &operator=(Demuxer &&) = default;

  // Disallow copying.
  Demuxer(const Demuxer &) = delete;
  Demuxer &operator=(const Demuxer &) = delete;

  // Open media file for demuxing.
  MediaStatus open(const char *path);

  // Call the provided callback once per track, providing the caller with
  // track-based metadata.
  MediaStatus forEachTrack(std::function<MediaStatus(const nlohmann::json &)> callback);

  // Read one sample/frame from the track.
  MediaStatus read(
    const nlohmann::json &readConfig,
    const uint8_t **data,
    size_t *byteSize,
    nlohmann::json *sampleMetadata);

  // Close media file.
  MediaStatus close();

  // Returns true if the Demuxer is valid.
  bool isValid() const;

private:
  std::unique_ptr<DemuxerApi> demuxer_;
};

}  // namespace c8
