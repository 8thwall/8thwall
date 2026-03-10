// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Decoder class encapsulates the implementation of an media Decoder.

#pragma once

#include "c8/media/media-status.h"

#include <nlohmann/json_fwd.hpp>

#include "c8/media/codec/codec-api.h"

namespace c8 {

class Demuxer;

class Decoder {
public:
  // Default constructed Decoder is unusable and can represent an invalid
  // constructed decoder.
  Decoder() = default;

  // Default move constructors.
  Decoder(Decoder &&) = default;
  Decoder &operator=(Decoder &&) = default;

  // Disallow copying.
  Decoder(const Decoder &) = delete;
  Decoder &operator=(const Decoder &) = delete;

  // Start decoder and add a new track to the Demuxer. Demuxer should be 'open' and
  // remain so until after the call to finish. The Decoder does not own the Demuxer.
  MediaStatus start(Demuxer *demuxer);

  // Decode a single sample/frame of data.
  MediaStatus decode(
    const nlohmann::json &sampleConfig,
    const uint8_t **data,
    size_t * byteSize,
    nlohmann::json *sampleMetadata);

  // Finish decoding and flush any remaining output to the demuxer.
  MediaStatus finish();

  // Returns true if the Decoder is valid.
  bool isValid() const;

private:
  friend class DecoderRegistry;
  friend class DecoderTest;

  // Construct a Decoder with an implementation of a DecoderApi.
  Decoder(DecoderApi *decoder);

  std::unique_ptr<DecoderApi> decoder_;
};

}  // namespace c8
