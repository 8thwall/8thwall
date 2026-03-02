// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "fdk-aac-decoder.h",
  };
  visibility = {
    "//visibility:private",
  };
  deps = {
    ":codec-api",
    "//c8/media:demuxer",
    "//c8:string",
    "//c8:vector",
    "@fdkaac//:fdk-aac",
    "@json//:json",
  };
}
cc_end(0xcf1aafbe);

#include "c8/media/codec/fdk-aac-decoder.h"

#include "c8/string.h"
#include "c8/vector.h"
#include "c8/media/demuxer.h"

namespace c8 {

namespace {

#define CHECK_AACDEC(result)                               \
  if (int aacStatus = (result); aacStatus != AAC_DEC_OK) { \
    return {aacStatus, "AAC decoding failed"};             \
  }

}  // namespace

FdkAacDecoder::FdkAacDecoder(const nlohmann::json &config)
    : config_(config), demuxer_(nullptr), decoder_(nullptr) {}

FdkAacDecoder::~FdkAacDecoder() {}

MediaStatus FdkAacDecoder::start(Demuxer *demuxer) {
  demuxer_ = demuxer;
  if (config_.count("name") == 0 || !config_["name"].is_string()) {
    return {"AAC decoder must have a string 'name' field"};
  }
  trackName_ = config_["name"].get<std::string>();

  UINT nrOfLayers = 1;
  decoder_ = aacDecoder_Open(TT_MP4_RAW, nrOfLayers);
  if (decoder_ == nullptr) {
    return {"Could not open AAC Decoder"};
  }

  // Read the configuration buffer from the elementary stream in the demuxer.
  nlohmann::json readConfig;
  readConfig["name"] = trackName_;
  readConfig["type"] = "es";

  const uint8_t *muxerData = nullptr;
  size_t muxerDataSize = 0;
  nlohmann::json muxerMetadata;
  if (auto readResult = demuxer_->read(readConfig, &muxerData, &muxerDataSize, &muxerMetadata);
      readResult.code() != 0) {
    return readResult;
  }

  // Configure the decoder by reading the ASC or SMC from the elementary stream.
  UCHAR *ascData[] = {const_cast<UCHAR *>(muxerData)};  // logically const.
  UINT ascDataSize[] = {static_cast<UINT>(muxerDataSize)};
  CHECK_AACDEC(aacDecoder_ConfigRaw(decoder_, ascData, ascDataSize));

  // Start with a 1024 * sizeof(INT_PCM) buffer. This will grow as needed for
  // AAC with more channels or a bigger frame size.
  outputBuffer_.resize(1024);

  return {};
}

MediaStatus FdkAacDecoder::decode(
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

  UCHAR *fillData[] = {const_cast<UCHAR *>(muxerData)};  // logically const.
  UINT fillDataSize[] = {static_cast<UINT>(muxerDataSize)};
  UINT fillDataRemaining = static_cast<UINT>(muxerDataSize);

  AAC_DECODER_ERROR decodeResult;
  while (1) {
    CHECK_AACDEC(aacDecoder_Fill(decoder_, fillData, fillDataSize, &fillDataRemaining));
    decodeResult =
      aacDecoder_DecodeFrame(decoder_, outputBuffer_.data(), outputBuffer_.size(), 0 /* flags */);
    if (decodeResult != AAC_DEC_OUTPUT_BUFFER_TOO_SMALL) {
      break;
    }
    // Double the output buffer and try again.
    outputBuffer_.resize(outputBuffer_.size() << 1);
    fillDataRemaining = static_cast<UINT>(muxerDataSize);
  }

  if (decodeResult != AAC_DEC_OK) {
    return {decodeResult, "AAC decoding failed"};
  }

  metadata["format"] = "s16le";

  CStreamInfo *streamInfo = aacDecoder_GetStreamInfo(decoder_);
  metadata["channels"] = streamInfo->numChannels;

  *data = reinterpret_cast<uint8_t *>(outputBuffer_.data());
  *byteSize = streamInfo->numChannels * streamInfo->frameSize * sizeof(INT_PCM);

  return {};
}

MediaStatus FdkAacDecoder::finish() {
  if (decoder_) {
    // Close AAC encoder.
    aacDecoder_Close(decoder_);
    decoder_ = nullptr;
  }

  demuxer_ = nullptr;

  return {};
}

}  // namespace c8
