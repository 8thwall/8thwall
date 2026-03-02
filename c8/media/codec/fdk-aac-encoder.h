// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// FDK AAC Codec implementation.

#pragma once

#include "c8/string-view.h"
#include "c8/vector.h"
#include "c8/media/codec/codec-api.h"

#include <nlohmann/json.hpp>

#include <fdk-aac/aacenc_lib.h>

namespace c8 {

class Muxer;

class FdkAacEncoder : public EncoderApi {
public:
  // Construct encoder with track config.
  FdkAacEncoder(const nlohmann::json &config);

  // Virtual destructor.
  virtual ~FdkAacEncoder() override;

  // Default move constructors.
  FdkAacEncoder(FdkAacEncoder &&) = default;
  FdkAacEncoder &operator=(FdkAacEncoder &&) = default;

  // Disallow copying.
  FdkAacEncoder(const FdkAacEncoder &) = delete;
  FdkAacEncoder &operator=(const FdkAacEncoder &) = delete;

  // Start encoder and add a new track to the Muxer. Muxer should be
  // 'open' and remain so until after the call to finish. The EncoderApi does
  // not own the Muxer.
  virtual MediaStatus start(Muxer *muxer) override;

  // Encode new input data.
  virtual MediaStatus encode(
    const nlohmann::json &sampleConfig, const uint8_t *data, size_t byteSize) override;

  // Finish encoding and flush any remaining output to the muxer.
  virtual MediaStatus finish() override;

private:
  nlohmann::json config_;

  Muxer *muxer_;

  HANDLE_AACENCODER encoder_;

  uint32_t timescale_;
  int profile_;

  Vector<UCHAR> outputBuffer_;
};

}  // namespace c8
