// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Registries for supported Codecs and Muxers.

#pragma once

#include <nlohmann/json_fwd.hpp>

#include "c8/media/decoder.h"
#include "c8/media/encoder.h"
#include "c8/media/muxer.h"
#include "c8/media/demuxer.h"

namespace c8 {

class DecoderRegistry {
public:
  static Decoder create(const nlohmann::json &trackConfig);
};

class EncoderRegistry {
public:
  static Encoder create(const nlohmann::json &trackConfig);
};

class MuxerRegistry {
public:
  static Muxer create(const nlohmann::json &config);
};

class DemuxerRegistry {
public:
  static Demuxer create(const nlohmann::json &config);
};

}  // namespace c8
