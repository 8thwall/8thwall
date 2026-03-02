// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Encoder class encapsulates the implementation of an media Encoder.

#pragma once

#include "c8/media/media-status.h"

#include <nlohmann/json_fwd.hpp>

#include "c8/media/codec/codec-api.h"

namespace c8 {

class Muxer;

class Encoder {
public:
  // Default constructed Encoder is unusable and can represent an invalid
  // constructed encoder.
  Encoder() = default;

  // Default move constructors.
  Encoder(Encoder &&) = default;
  Encoder &operator=(Encoder &&) = default;

  // Disallow copying.
  Encoder(const Encoder &) = delete;
  Encoder &operator=(const Encoder &) = delete;

  // Start encoder and add a new track to the Muxer. Muxer should be 'open' and
  // remain so until after the call to finish. The Encoder does not own the Muxer.
  MediaStatus start(Muxer *muxer);

  // Encode new input data.
  MediaStatus encode(const nlohmann::json& sampleConfig, const uint8_t *data, size_t byteSize);

  // Finish encoding and flush any remaining output to the muxer.
  MediaStatus finish();

  // Returns true if the Encoder is valid.
  bool isValid() const;

private:
  friend class EncoderRegistry;

  // Construct a Encoder with an implementation of a EncoderApi.
  Encoder(EncoderApi *encoder);

  std::unique_ptr<EncoderApi> encoder_;
};

}  // namespace c8
