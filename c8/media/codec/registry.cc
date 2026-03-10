// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "registry.h",
  };
  visibility = {
    ":codec-protected",
  };
  deps = {
    ":fdk-aac-decoder",
    ":fdk-aac-encoder",
    ":mkv-demuxer",
    ":mp4v2-demuxer",
    ":mp4v2-muxer",
    ":openh264-decoder",
    ":openh264-encoder",
    ":opus-decoder",
    ":vpx-decoder",
    "//c8/media:decoder",
    "//c8/media:demuxer",
    "//c8/media:encoder",
    "//c8/media:muxer",
    "//c8:map",
    "//c8:string",
    "//c8/string:strcat",
    "@json//:json",
  };
}
cc_end(0x03a99199);

#include "c8/media/codec/registry.h"

#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

#include "c8/string.h"
#include "c8/string/strcat.h"
#include "c8/media/codec/fdk-aac-decoder.h"
#include "c8/media/codec/fdk-aac-encoder.h"
#include "c8/media/codec/mkv-demuxer.h"
#include "c8/media/codec/mp4v2-demuxer.h"
#include "c8/media/codec/mp4v2-muxer.h"
#include "c8/media/codec/openh264-decoder.h"
#include "c8/media/codec/openh264-encoder.h"
#include "c8/media/codec/opus-decoder.h"
#include "c8/media/codec/vpx-decoder.h"

namespace c8 {

Decoder DecoderRegistry::create(const nlohmann::json &trackConfig) {
  if (trackConfig.count("codec") == 0 || !trackConfig["codec"].is_string()) {
    return {nullptr};
  }
  const String &codec = trackConfig["codec"].get<std::string>();
  if (codec == "h264") {
    return {new OpenH264Decoder(trackConfig)};
  } else if (codec == "aac") {
    return {new FdkAacDecoder(trackConfig)};
  } else if (codec == "vp8") {
    return {new VpxDecoder(trackConfig)};
  } else if (codec == "vp9") {
    return {new VpxDecoder(trackConfig)};
  } else if (codec == "opus") {
    return {new OpusDecoder(trackConfig)};
  }
  return {nullptr};
}

Encoder EncoderRegistry::create(const nlohmann::json &trackConfig) {
  if (trackConfig.count("codec") == 0 || !trackConfig["codec"].is_string()) {
    return {nullptr};
  }
  const String &codec = trackConfig["codec"].get<std::string>();
  if (codec == "h264") {
    return {new OpenH264Encoder(trackConfig)};
  } else if (codec == "aac") {
    return {new FdkAacEncoder(trackConfig)};
  }
  return {nullptr};
}

Muxer MuxerRegistry::create(const nlohmann::json &config) {
  // If the container is specified, use it as the source of truth.
  String filetype;
  if (config.count("container") > 0 && config["container"].is_string()) {
    filetype = strCat(".", config["container"].get<std::string>());
  } else if (config.count("path") > 0 && config["path"].is_string()) {
    // Otherwise parse the container from the file extension.
    filetype = std::filesystem::path(config["path"].get<std::string>().c_str()).extension();
  } else {
    return {nullptr};
  }

  std::transform(filetype.begin(), filetype.end(), filetype.begin(), [](auto c) {
    return std::tolower(c);
  });

  if (filetype == ".mp4") {
    return {new Mp4v2Muxer(config)};
  }
  return {nullptr};
}

Demuxer DemuxerRegistry::create(const nlohmann::json &config) {
  // If the container is specified, use it as the source of truth.
  String filetype;
  if (config.count("container") > 0 && config["container"].is_string()) {
    filetype = strCat(".", config["container"].get<std::string>());
  } else if (config.count("path") > 0 && config["path"].is_string()) {
    // Otherwise parse the container from the file extension.
    filetype = std::filesystem::path(config["path"].get<std::string>().c_str()).extension();
  } else {
    return {nullptr};
  }

  std::transform(filetype.begin(), filetype.end(), filetype.begin(), [](auto c) {
    return std::tolower(c);
  });

  if (filetype == ".mp4") {
    return {new Mp4v2Demuxer(config)};
  } else if (filetype == ".mkv" || filetype == ".webm") {
    return {new MkvDemuxer(config)};
  }
  return {nullptr};
}

}  // namespace c8
