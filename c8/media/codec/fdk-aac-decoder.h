// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// AAC Codec decoder implementation.

#pragma once

#include "c8/string.h"
#include "c8/vector.h"
#include "c8/media/codec/codec-api.h"

#include <nlohmann/json.hpp>

#include <fdk-aac/aacdecoder_lib.h>

namespace c8 {

class Demuxer;

class FdkAacDecoder : public DecoderApi {
public:
  // Construct decoder with track config.
  FdkAacDecoder(const nlohmann::json &config);

  // Virtual destructor.
  virtual ~FdkAacDecoder() override;

  // Default move constructors.
  FdkAacDecoder(FdkAacDecoder &&) = default;
  FdkAacDecoder &operator=(FdkAacDecoder &&) = default;

  // Disallow copying.
  FdkAacDecoder(const FdkAacDecoder &) = delete;
  FdkAacDecoder &operator=(const FdkAacDecoder &) = delete;

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

  HANDLE_AACDECODER decoder_;

  String trackName_;

  Vector<INT_PCM> outputBuffer_;
};

}  // namespace c8
