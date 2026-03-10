// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "opus-decoder.h",
  };
  visibility = {
    "//visibility:private",
  };
  deps = {
    ":codec-api",
    "//c8/media:demuxer",
    "//c8:string",
    "//c8:vector",
    "@opus//:opus",
    "@json//:json",
  };
}
cc_end(0x6bf065da);

#include "c8/media/codec/opus-decoder.h"

#include "c8/media/demuxer.h"

namespace c8 {

OpusDecoder::OpusDecoder(const nlohmann::json &config)
    : config_(config), demuxer_(nullptr), decoder_(nullptr) {}

OpusDecoder::~OpusDecoder() {}

MediaStatus OpusDecoder::start(Demuxer *demuxer) {
  demuxer_ = demuxer;
  if (config_.count("name") == 0 || !config_["name"].is_string()) {
    return {"Opus decoder must have a string 'name' field"};
  }
  trackName_ = config_["name"].get<std::string>();

  return {};
}

MediaStatus OpusDecoder::decode(
  const nlohmann::json &sampleConfig,
  const uint8_t **data,
  size_t *byteSize,
  nlohmann::json *sampleMetadata) {
  const uint8_t *muxerData = nullptr;
  size_t muxerDataSize = 0;

  nlohmann::json &metadata = *sampleMetadata;
  metadata.clear();

  muxerData = nullptr;
  muxerDataSize = 0;

  nlohmann::json readConfig = sampleConfig;
  readConfig["name"] = trackName_;

  if (auto readResult = demuxer_->read(readConfig, &muxerData, &muxerDataSize, &metadata);
      readResult.code() != 0) {
    return readResult;
  }

  constexpr int MAX_FRAME_SIZE = 6 * 960;

  if (!decoder_) {
    if (metadata.count("channels") == 0 || !metadata["channels"].is_number()) {
      return {"Opus decoder must be provided 'channels' field by demuxer"};
    }
    channels_ = metadata["channels"].get<int>();

    if (metadata.count("sampleRate") == 0 || !metadata["sampleRate"].is_number()) {
      return {"Opus decoder must be provided 'sampleRate' field by demuxer"};
    }
    sampleRate_ = metadata["sampleRate"].get<int>();

    int err = 0;
    decoder_ = opus_decoder_create(sampleRate_, channels_, &err);
    if (err < 0) {
      return {"Opus decoder failed to initialize"};
    }
    outputBuffer_.resize(MAX_FRAME_SIZE * channels_ * sizeof(opus_int16));
  }

  int frameSize =
    opus_decode(decoder_, muxerData, muxerDataSize, outputBuffer_.data(), MAX_FRAME_SIZE, 0);
  if (frameSize < 0) {
    return {frameSize, "Opus decoder failed to decode frame"};
  }

  metadata["format"] = "s16le";
  metadata["frameDuration"] = frameSize;

  *data = reinterpret_cast<uint8_t *>(outputBuffer_.data());
  *byteSize = channels_ * frameSize * sizeof(opus_int16);

  return {};
}

MediaStatus OpusDecoder::finish() {
  if (decoder_) {
    // Close OPUS encoder.
    opus_decoder_destroy(decoder_);
    decoder_ = nullptr;
  }

  demuxer_ = nullptr;
  sampleRate_ = 0;
  channels_ = 0;

  return {};
}

}  // namespace c8
