// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// VP8/VP9 Codec implementation.

#pragma once

#include "c8/string-view.h"
#include "c8/string.h"
#include "c8/vector.h"
#include "c8/media/codec/codec-api.h"

#include <nlohmann/json.hpp>

#include <vpx/vpx_decoder.h>

struct VpxInterface;

namespace c8 {

class Demuxer;

class VpxDecoder : public DecoderApi {
public:
  // Construct decoder with track config.
  VpxDecoder(const nlohmann::json &config);

  // Virtual destructor.
  virtual ~VpxDecoder() override;

  // Default move constructors.
  VpxDecoder(VpxDecoder &&) = default;
  VpxDecoder &operator=(VpxDecoder &&) = default;

  // Disallow copying.
  VpxDecoder(const VpxDecoder &) = delete;
  VpxDecoder &operator=(const VpxDecoder &) = delete;

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
  vpx_codec_ctx_t codec_;
  const VpxInterface *decoder_;
  String trackName_;
  Vector<uint8_t> outputBuffer_;
};

}  // namespace c8
