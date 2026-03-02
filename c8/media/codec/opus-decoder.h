// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Opus audio decoder implementation.

#pragma once

#include "c8/string.h"
#include "c8/vector.h"
#include "c8/media/codec/codec-api.h"

#include <nlohmann/json.hpp>

#include <opus.h>

namespace c8 {

class Demuxer;

class OpusDecoder : public DecoderApi {
public:
  // Construct decoder with track config.
  OpusDecoder(const nlohmann::json &config);

  // Virtual destructor.
  virtual ~OpusDecoder() override;

  // Default move constructors.
  OpusDecoder(OpusDecoder &&) = default;
  OpusDecoder &operator=(OpusDecoder &&) = default;

  // Disallow copying.
  OpusDecoder(const OpusDecoder &) = delete;
  OpusDecoder &operator=(const OpusDecoder &) = delete;

  // Start decoder and add a new track to the Demuxer. Demuxer should be
  // 'open' and remain so until after the call to finish. The DecoderApi does
  // not own the Demuxer.
  virtual MediaStatus start(Demuxer *demuxer) override;

  // Decode a single sample/frame of data.
  virtual MediaStatus decode(
    const nlohmann::json &sampleConfig,
    const uint8_t **data,
    size_t *byteSize,
    nlohmann::json *sampleMetadata) override;

  // Finish decoding and flush any remaining output to the demuxer.
  virtual MediaStatus finish() override;

private:
  nlohmann::json config_;
  Demuxer *demuxer_;

  ::OpusDecoder *decoder_;

  String trackName_;

  int channels_ = 0;
  int sampleRate_ = 0;

  Vector<opus_int16> decodeBuffer_;
  Vector<opus_int16> outputBuffer_;
};

}  // namespace c8
