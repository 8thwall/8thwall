// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Open H.264 Codec implementation.

#pragma once

#include "c8/string-view.h"
#include "c8/string.h"
#include "c8/vector.h"
#include "c8/media/codec/codec-api.h"

#include <nlohmann/json.hpp>

#include <openh264/codec_api.h>

class ISVCDecoder;

namespace c8 {

class Demuxer;

class OpenH264Decoder : public DecoderApi {
public:
  // Construct decoder with track config.
  OpenH264Decoder(const nlohmann::json &config);

  // Virtual destructor.
  virtual ~OpenH264Decoder() override;

  // Default move constructors.
  OpenH264Decoder(OpenH264Decoder &&) = default;
  OpenH264Decoder &operator=(OpenH264Decoder &&) = default;

  // Disallow copying.
  OpenH264Decoder(const OpenH264Decoder &) = delete;
  OpenH264Decoder &operator=(const OpenH264Decoder &) = delete;

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
  ISVCDecoder *decoder_;
  int logLevel_;
  String trackName_;
  Vector<uint8_t> outputBuffer_;
};

}  // namespace c8
