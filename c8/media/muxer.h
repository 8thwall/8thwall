// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Muxer class encapsulates the implementation of an media Muxer.

#pragma once

#include "c8/media/media-status.h"

#include <nlohmann/json_fwd.hpp>

#include "c8/media/codec/codec-api.h"

namespace c8 {

class Muxer {
public:
  // Default constructed Muxer is unusable and can represent an invalid
  // constructed muxer.
  Muxer() = default;

  // Default move constructors.
  Muxer(Muxer&&) = default;
  Muxer& operator=(Muxer&&) = default;

  // Disallow copying.
  Muxer(const Muxer&) = delete;
  Muxer& operator=(const Muxer&) = delete;

  // Open new media file for muxing.
  MediaStatus open(const char* path);

  // Add a new track to the muxer.
  MediaStatus addTrack(const nlohmann::json& trackConfig);

  // The Codec should be calling write on the muxer...
  // Lazy create tracks on first write. Duration, packet timestamp, synchronized, rendering offset.
  MediaStatus write(const nlohmann::json& writeConfig, const uint8_t* data, size_t byteSize);

  // Close and finish writing media file.
  MediaStatus close();

  // Returns true if the Muxer is valid.
  bool isValid() const;

private:
  friend class MuxerRegistry;

  // Construct a Muxer with an implementation of a MuxerApi.
  Muxer(MuxerApi* muxer);

  std::unique_ptr<MuxerApi> muxer_;
};

}  // namespace c8
