// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Open H.264 Codec implementation.

#pragma once

#include "c8/string.h"
#include "c8/string-view.h"
#include "c8/vector.h"
#include "c8/media/codec/codec-api.h"

#include <nlohmann/json.hpp>

#include <openh264/codec_api.h>

class ISVCEncoder;

namespace c8 {

class Muxer;

class OpenH264Encoder : public EncoderApi {
public:
  // Construct encoder with track config.
  OpenH264Encoder(const nlohmann::json &config);

  // Virtual destructor.
  virtual ~OpenH264Encoder() override;

  // Default move constructors.
  OpenH264Encoder(OpenH264Encoder &&) = default;
  OpenH264Encoder &operator=(OpenH264Encoder &&) = default;

  // Disallow copying.
  OpenH264Encoder(const OpenH264Encoder &) = delete;
  OpenH264Encoder &operator=(const OpenH264Encoder &) = delete;

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
  // Write data for the previous variable fps frame now that we can determine
  // the frame duration.
  MediaStatus flushPreviousFrame(uint32_t frameDuration);

  nlohmann::json config_;

  Muxer *muxer_;

  ISVCEncoder *encoder_;

  EProfileIdc profile_;
  ELevelIdc level_;

  int logLevel_;
  int videoFormat_;

  uint64_t lastTimestamp_;
  uint32_t lastDuration_;
  EVideoFrameType lastFrameType_;

  String name_;

  uint16_t width_;
  uint16_t height_;

  uint32_t defaultFrameDuration_;

  Vector<uint8_t> outputBuffer_;
};

}  // namespace c8
