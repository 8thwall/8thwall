// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "openh264-decoder.h",
  };
  visibility = {
    "//visibility:private",
  };
  deps = {
    ":codec-api",
    "//c8/media:demuxer",
    "//c8:string",
    "//c8:string-view",
    "//c8:vector",
    "@openh264//:openh264",
    "@json//:json",
  };
}
cc_end(0xaf35c74f);

#include "c8/media/codec/openh264-decoder.h"

#include "c8/string.h"
#include "c8/vector.h"
#include "c8/media/demuxer.h"

namespace c8 {

namespace {

int parseJsonInt(const nlohmann::json &config, const char *field, int defaultValue) {
  if (config.count(field) > 0 && config[field].is_number_integer()) {
    return config[field].get<int>();
  }
  return defaultValue;
}

}  // namespace

OpenH264Decoder::OpenH264Decoder(const nlohmann::json &config)
    : config_(config), demuxer_(nullptr), decoder_(nullptr) {}

OpenH264Decoder::~OpenH264Decoder() {}

MediaStatus OpenH264Decoder::start(Demuxer *demuxer) {
  demuxer_ = demuxer;
  if (config_.count("name") == 0 || !config_["name"].is_string()) {
    return {"H.264 decoder must have a string 'name' field"};
  }
  trackName_ = config_["name"].get<std::string>();

  auto result = WelsCreateDecoder(&decoder_);
  if (result != 0 || decoder_ == nullptr) {
    return {"Could not allocate H.264 Decoder"};
  }

  SDecodingParam decodingParams = {0};
  decodingParams.sVideoProperty.eVideoBsType = VIDEO_BITSTREAM_AVC;
  decoder_->Initialize(&decodingParams);

  // Set the log level. Default=2 (WARNINGS + ERRORS). Quiet is 0.
  logLevel_ = parseJsonInt(config_, "verbosity", WELS_LOG_DEFAULT);
  decoder_->SetOption(DECODER_OPTION_TRACE_LEVEL, &logLevel_);

  return {};
}

MediaStatus OpenH264Decoder::decode(
  const nlohmann::json &sampleConfig,
  const uint8_t **data,
  size_t *byteSize,
  nlohmann::json *sampleMetadata) {
  const uint8_t *muxerData = nullptr;
  size_t muxerDataSize = 0;

  nlohmann::json &metadata = *sampleMetadata;
  metadata.clear();

  SBufferInfo bufferInfo;

  muxerData = nullptr;
  muxerDataSize = 0;

  nlohmann::json readConfig = sampleConfig;
  readConfig["name"] = trackName_;

  if (auto readResult = demuxer_->read(readConfig, &muxerData, &muxerDataSize, &metadata);
      readResult.code() != 0) {
    return readResult;
  }

  uint8_t *ptrs[3];
  auto result = decoder_->DecodeFrameNoDelay(muxerData, muxerDataSize, ptrs, &bufferInfo);
  if (result != dsErrorFree) {
    return {result, "H.264 failed to decode frame"};
  }

  if (!bufferInfo.iBufferStatus) {
    return {"H.264 sample is not a valid frame"};
  }

  if (bufferInfo.UsrData.sSystemBuffer.iFormat != EVideoFormatType::videoFormatI420) {
    // Note: future decoders versions might support Mono 4:0:0 or High 4:4:4 and
    // this constrain could be lifted if desired.
    return {"H.264 sample contains frame format that is not yuv420p"};
  }

  // Note: bufferInfo.uiInBsTimeStamp and bufferInfo.uiOutYuvTimeStamp are timestamps
  // in the decoded stream, but their units and non-hermicity are a mystery to
  // me. Container timestamps are better anyway, since they sync across tracks
  // and we rely on them being set in the demuxer.

  metadata["format"] = "yuv420p";
  metadata["width"] = bufferInfo.UsrData.sSystemBuffer.iWidth;
  metadata["height"] = bufferInfo.UsrData.sSystemBuffer.iHeight;

  metadata["data0"] = reinterpret_cast<uintptr_t>(bufferInfo.pDst[0]);
  metadata["data1"] = reinterpret_cast<uintptr_t>(bufferInfo.pDst[1]);
  metadata["data2"] = reinterpret_cast<uintptr_t>(bufferInfo.pDst[2]);

  // The first stride is the Y-stride.
  metadata["rowBytes0"] = bufferInfo.UsrData.sSystemBuffer.iStride[0];

  // The second stride is the U-stride and V-stride.
  metadata["rowBytes1"] = bufferInfo.UsrData.sSystemBuffer.iStride[1];
  metadata["rowBytes2"] = bufferInfo.UsrData.sSystemBuffer.iStride[1];

  *data = ptrs[0];
  *byteSize = ptrs[2] - ptrs[0]
    + (bufferInfo.UsrData.sSystemBuffer.iHeight >> 1) * bufferInfo.UsrData.sSystemBuffer.iStride[1];

  return {};
}

MediaStatus OpenH264Decoder::finish() {
  if (decoder_) {
    decoder_->Uninitialize();
    WelsDestroyDecoder(decoder_);
    decoder_ = nullptr;
  }

  demuxer_ = nullptr;

  return {};
}

}  // namespace c8
