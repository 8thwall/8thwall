// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "fdk-aac-encoder.h",
  };
  visibility = {
    "//visibility:private",
  };
  deps = {
    ":codec-api",
    "//c8/media:muxer",
    "//c8:string",
    "//c8:string-view",
    "//c8:vector",
    "@fdkaac//:fdk-aac",
    "@json//:json",
  };
}
cc_end(0x7f808e8b);

#include "c8/media/codec/fdk-aac-encoder.h"

#include <fdk-aac/aacenc_lib.h>

#include "c8/string.h"
#include "c8/vector.h"
#include "c8/media/muxer.h"

#define CHECK_AACENC(result)                              \
  if (int aacStatus = (result); aacStatus != AACENC_OK) { \
    return {aacStatus, "AAC encoding failed"};            \
  }

namespace c8 {

namespace {
int parseJsonInt(const nlohmann::json &config, const char *field, int defaultValue) {
  if (config.count(field) > 0 && config[field].is_number_integer()) {
    return config[field].get<int>();
  }
  return defaultValue;
}
}  // namespace

FdkAacEncoder::FdkAacEncoder(const nlohmann::json &config)
    : config_(config), muxer_(nullptr), encoder_(nullptr), timescale_(0), profile_(0) {}

FdkAacEncoder::~FdkAacEncoder() {}

MediaStatus FdkAacEncoder::start(Muxer *muxer) {
  if (config_.count("name") == 0 || !config_["name"].is_string()) {
    return {"Track config must have a string 'name' field"};
  }
  const String &name = config_["name"].get<std::string>();

  uint32_t sampleRate = parseJsonInt(config_, "sampleRate", 44100);
  timescale_ = parseJsonInt(config_, "timescale", sampleRate);

  profile_ = parseJsonInt(config_, "profile", AOT_AAC_LC);

  int channelMode = parseJsonInt(config_, "channelMode", MODE_1);

  int numChannels = 0;
  switch (channelMode) {
    case MODE_1:
      numChannels = 1;
      break;
    case MODE_2:
      numChannels = 2;
      break;
    case MODE_1_2:
      numChannels = 3;
      break;
    case MODE_1_2_1:
      numChannels = 4;
      break;
    case MODE_1_2_2:
      numChannels = 5;
      break;
    case MODE_1_2_2_1:
      numChannels = 6;
      break;
    case MODE_6_1:
      numChannels = 7;
      break;
    case MODE_1_2_2_2_1:
    case MODE_7_1_REAR_SURROUND:
    case MODE_7_1_FRONT_CENTER:
    case MODE_7_1_BACK:
    case MODE_7_1_TOP_FRONT:
      numChannels = 8;
      break;
  }

  uint32_t outputBufferBytesPerChannel = parseJsonInt(config_, "outputBufferBytesPerChannel", 1024);

  // There are likely better ways to determine whether SBR or PS should be
  // allocated, but this is a strawman.
  int allocateCode = (profile_ == AOT_AAC_LC) ? 0x01 : (numChannels > 1 ? 0x07 : 0x03);
  CHECK_AACENC(aacEncOpen(&encoder_, allocateCode, numChannels));

  CHECK_AACENC(aacEncoder_SetParam(encoder_, AACENC_AOT, profile_));

  CHECK_AACENC(aacEncoder_SetParam(encoder_, AACENC_SAMPLERATE, sampleRate));
  CHECK_AACENC(aacEncoder_SetParam(encoder_, AACENC_CHANNELMODE, channelMode));
  CHECK_AACENC(aacEncoder_SetParam(
    encoder_, AACENC_CHANNELORDER, parseJsonInt(config_, "channelOrder", 0)));  // MPEG
  CHECK_AACENC(aacEncoder_SetParam(
    encoder_, AACENC_BITRATEMODE, parseJsonInt(config_, "bitrateMode", 0)));  // CBR
  CHECK_AACENC(
    aacEncoder_SetParam(encoder_, AACENC_BITRATE, parseJsonInt(config_, "bitrate", 131072)));
  CHECK_AACENC(
    aacEncoder_SetParam(encoder_, AACENC_TRANSMUX, parseJsonInt(config_, "transmux", TT_MP4_ADTS)));

  // Note(mc): Many more parameters exist in this AAC Encoder and could be added here if desired.

  // Initialize encoder.
  CHECK_AACENC(aacEncEncode(encoder_, nullptr, nullptr, nullptr, nullptr));
  // Compute and resize the output buffer.
  int outputBufferSize = numChannels * outputBufferBytesPerChannel;
  outputBuffer_.resize(outputBufferSize);

  nlohmann::json muxerConfig = config_;
  muxerConfig["timescale"] = timescale_;
  muxerConfig["frameDuration"] = aacEncoder_GetParam(encoder_, AACENC_GRANULE_LENGTH);

  // Add the track to the Muxer.
  muxer_ = muxer;
  if (auto rv = muxer_->addTrack(muxerConfig); rv.code() != 0) {
    return rv;
  }

  // Write the configuration block.
  AACENC_InfoStruct aacEncoderInfo;
  CHECK_AACENC(aacEncInfo(encoder_, &aacEncoderInfo));
  nlohmann::json writeConfig;
  writeConfig["name"] = name;
  writeConfig["type"] = "es";
  writeConfig["codec"] = "aac";
  writeConfig["profile"] = profile_;
  writeConfig["timescale"] = timescale_;
  return muxer_->write(writeConfig, aacEncoderInfo.confBuf, aacEncoderInfo.confSize);
}

MediaStatus FdkAacEncoder::encode(
  const nlohmann::json &sampleConfig, const uint8_t *data, size_t byteSize) {
  if (config_.count("name") == 0 || !config_["name"].is_string()) {
    return {"AAC track config must have a string 'name' field"};
  }
  const String &name = sampleConfig["name"].get<std::string>();

  // Set-up the input buffers. Logically const, despite const_cast.
  void *inBuffer[] = {const_cast<uint8_t *>(data)};
  INT inBufferSize[] = {static_cast<INT>(byteSize)};
  INT inBufferIds[] = {IN_AUDIO_DATA};
  INT inBufferElSize[] = {sizeof(INT_PCM)};

  // Set-up the output buffers
  void *outBuffer[] = {outputBuffer_.data()};
  INT outBufferIds[] = {OUT_BITSTREAM_DATA};
  INT outBufferSize[] = {static_cast<INT>(outputBuffer_.size())};
  INT outBufferElSize[] = {sizeof(UCHAR)};

  // Initialize input buffer descriptor
  AACENC_BufDesc inBufDesc;
  inBufDesc.numBufs = sizeof(inBuffer) / sizeof(void *);
  inBufDesc.bufs = (void **)&inBuffer;
  inBufDesc.bufferIdentifiers = inBufferIds;
  inBufDesc.bufSizes = inBufferSize;
  inBufDesc.bufElSizes = inBufferElSize;

  // Initialize output buffer descriptor
  AACENC_BufDesc outBufDesc;
  outBufDesc.numBufs = sizeof(outBuffer) / sizeof(void *);
  outBufDesc.bufs = (void **)&outBuffer;
  outBufDesc.bufferIdentifiers = outBufferIds;
  outBufDesc.bufSizes = outBufferSize;
  outBufDesc.bufElSizes = outBufferElSize;

  AACENC_InArgs inargs;
  inargs.numInSamples = byteSize / sizeof(INT_PCM);

  AACENC_OutArgs outargs = {};

  nlohmann::json writeConfig;
  writeConfig["name"] = name;
  writeConfig["type"] = "sample";
  writeConfig["codec"] = "aac";
  writeConfig["profile"] = profile_;
  writeConfig["timescale"] = timescale_;

  do {
    // Continue to process input that hasn't been consumed.
    inBuffer[0] = ((char *)inBuffer[0]) + outargs.numInSamples * sizeof(INT_PCM);
    inBufferSize[0] -= outargs.numInSamples * sizeof(INT_PCM);
    inargs.numInSamples -= outargs.numInSamples;
    CHECK_AACENC(aacEncEncode(encoder_, &inBufDesc, &outBufDesc, &inargs, &outargs));

    if (outargs.numOutBytes > 0) {
      // Skip the 7 byte header and write the payload.
      auto rv = muxer_->write(writeConfig, outputBuffer_.data() + 7, outargs.numOutBytes - 7);
      if (rv.code() != 0) {
        return rv;
      }
    }
  } while (outargs.numInSamples > 0);

  return {};
}

MediaStatus FdkAacEncoder::finish() {
  if (!encoder_) {
    return {"AAC encoder not initialized"};
  }

  if (config_.count("name") == 0 || !config_["name"].is_string()) {
    return {"AAC track config must have a string 'name' field"};
  }
  const String &name = config_["name"].get<std::string>();

  // Set-up the output buffers
  void *outBuffer[] = {outputBuffer_.data()};
  INT outBufferIds[] = {OUT_BITSTREAM_DATA};
  INT outBufferSize[] = {static_cast<INT>(outputBuffer_.size())};
  INT outBufferElSize[] = {sizeof(UCHAR)};

  // Initialize output buffer descriptor
  AACENC_BufDesc outBufDesc;
  outBufDesc.numBufs = sizeof(outBuffer) / sizeof(void *);
  outBufDesc.bufs = (void **)&outBuffer;
  outBufDesc.bufferIdentifiers = outBufferIds;
  outBufDesc.bufSizes = outBufferSize;
  outBufDesc.bufElSizes = outBufferElSize;

  // Use a -1 numInSamples to flush encoder.
  AACENC_InArgs inargs;
  inargs.numInSamples = -1;

  AACENC_OutArgs outargs = {};

  nlohmann::json writeConfig;
  writeConfig["name"] = name;
  writeConfig["type"] = "sample";
  writeConfig["codec"] = "aac";
  writeConfig["profile"] = profile_;
  writeConfig["timescale"] = timescale_;

  int result = AACENC_OK;
  while (1) {
    // Keep flushing the encoder until no more output comes out.
    result = aacEncEncode(encoder_, nullptr, &outBufDesc, &inargs, &outargs);
    if (outargs.numOutBytes > 0) {
      // Skip the 7 byte header and write the payload.
      auto rv = muxer_->write(writeConfig, outputBuffer_.data() + 7, outargs.numOutBytes - 7);
      if (rv.code() != 0) {
        return rv;
      }
    } else {
      break;
    }
  }

  if (result != AACENC_ENCODE_EOF) {
    return {"AAC encoding error during finish"};
  }

  // Close AAC encoder.
  aacEncClose(&encoder_);
  encoder_ = nullptr;

  muxer_ = nullptr;

  return {};
}

}  // namespace c8
