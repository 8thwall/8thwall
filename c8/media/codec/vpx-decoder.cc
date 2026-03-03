// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "vpx-decoder.h",
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
    "@libvpx//:libvpx-decoder",
    "@json//:json",
  };
}
cc_end(0x1ea63e63);

#include "c8/string.h"
#include "c8/vector.h"
#include "external/libvpx/tools_common.h"
#include "c8/media/codec/vpx-decoder.h"
#include "c8/media/demuxer.h"

// Part of the API for libvpx/tools_common.h requires usage_exit to be defined in the file that
// imports the header.
extern "C" {
void usage_exit() { exit(EXIT_FAILURE); }
}

namespace c8 {

VpxDecoder::VpxDecoder(const nlohmann::json &config)
    : config_(config), demuxer_(nullptr), decoder_(nullptr) {}

VpxDecoder::~VpxDecoder() {}

MediaStatus VpxDecoder::start(Demuxer *demuxer) {
  demuxer_ = demuxer;
  if (config_.count("name") == 0 || !config_["name"].is_string()) {
    return {"VPX decoder must have a string 'name' field"};
  }
  trackName_ = config_["name"].get<std::string>();

  return {};
}

MediaStatus VpxDecoder::decode(
  const nlohmann::json &sampleConfig,
  const uint8_t **data,
  size_t *byteSize,
  nlohmann::json *sampleMetadata) {
  const uint8_t *muxerData = nullptr;
  size_t muxerDataSize = 0;

  nlohmann::json &metadata = *sampleMetadata;
  metadata.clear();

  nlohmann::json readConfig = sampleConfig;
  readConfig["name"] = trackName_;

  if (auto readResult = demuxer_->read(readConfig, &muxerData, &muxerDataSize, &metadata);
      readResult.code() != 0) {
    return readResult;
  }

  if (!decoder_) {
    if (metadata.count("codec") == 0 || !metadata["codec"].is_string()) {
      return {"VPX decoder not provided 'codec' field from muxer"};
    }
    String codecName = metadata["codec"].get<std::string>();

    if (codecName == "vp8") {
      decoder_ = get_vpx_decoder_by_fourcc(VP8_FOURCC);
    } else if (codecName == "vp9") {
      decoder_ = get_vpx_decoder_by_fourcc(VP9_FOURCC);
    }
    if (!decoder_) {
      return {"VPX decoder unknown codec."};
    }

    if (vpx_codec_dec_init(&codec_, decoder_->codec_interface(), NULL, 0)) {
      return {"VPX decoder failed to initialize"};
    }
  }

  if (vpx_codec_decode(&codec_, muxerData, muxerDataSize, NULL, 0)) {
    return {"VPX failed to decode frame"};
  }

  vpx_codec_iter_t iter = nullptr;
  vpx_image_t *img = img = vpx_codec_get_frame(&codec_, &iter);
  if (!img) {
    return {"VPX failed to get frame"};
  }

  metadata["format"] = "yuv420p";

  metadata["width"] = img->d_w;
  metadata["height"] = img->d_h;

  metadata["data0"] = reinterpret_cast<uintptr_t>(img->planes[0]);
  metadata["data1"] = reinterpret_cast<uintptr_t>(img->planes[1]);
  metadata["data2"] = reinterpret_cast<uintptr_t>(img->planes[2]);

  metadata["rowBytes0"] = img->stride[0];
  metadata["rowBytes1"] = img->stride[1];
  metadata["rowBytes2"] = img->stride[1];

  *data = img->planes[0];
  *byteSize = img->planes[2] - img->planes[0] + (img->d_h >> 1) * img->stride[2];

  return {};
}

MediaStatus VpxDecoder::finish() {
  demuxer_ = nullptr;
  if (decoder_) {
    decoder_ = nullptr;
    if (vpx_codec_destroy(&codec_)) {
      return {"VPX failed to destroy codec"};
    }
    codec_ = vpx_codec_ctx_t();
  }

  return {};
}

}  // namespace c8
