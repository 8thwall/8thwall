// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "media-recorder.h",
  };
  visibility = {
    "//visibility:public",
  };
  deps = {
    ":encoder",
    ":media-status",
    ":muxer",
    "//c8/media/codec:registry",
    "//c8/pixels:pixels",
    "//c8:map",
    "//c8:string",
    "//c8:string-view",
    "@json//:json",
  };
}
cc_end(0x486b83e8);

#include "c8/media/media-recorder.h"

#include <nlohmann/json.hpp>

#include "c8/media/codec/registry.h"

namespace c8 {

MediaRecorder::~MediaRecorder() {};

MediaStatus MediaRecorder::start(StringView config) {
  // Remove any prior muxers or encoders.
  muxer_ = {};
  encoders_.clear();

  // Parse the json config.
  // auto json = nlohmann::json::parse(config, nullptr, false);
  auto json = nlohmann::json::parse(config);

  if (json.is_discarded()) {
    return {"JSON syntax error in config"};
  }

  if (json.count("path") == 0 && !json["path"].is_string()) {
    return {"MediaRecorder config must have string 'path' field"};
  }
  const String &path = json["path"].get<std::string>();

  if (json.count("tracks") == 0 && !json["tracks"].is_array()) {
    return {"MediaRecorder config must have at least 1 track"};
  }

  // Create the muxer.
  muxer_ = MuxerRegistry::create(json);

  if (!muxer_.isValid()) {
    return {"MediaRecorder config must have valid string 'container' field"};
  }

  // Create the encoders.
  for (auto track : json["tracks"]) {
    if (track.count("name") == 0 && !track["name"].is_string()) {
      return {"MediaRecorder track config must have string 'name' field"};
    }
    auto [encoder, inserted] =
      encoders_.emplace(track["name"].get<std::string>(), EncoderRegistry::create(track));
    if (!inserted) {
      return {"MediaRecorder track names must be unique"};
    }
    if (!encoder->second.isValid()) {
      return {"MediaRecorder track config must have valid string 'codec' field"};
    }
  }

  // Open the muxer for writing.
  if (auto status = muxer_.open(path.c_str()); status.code() != 0) {
    return status;
  }

  // Start the encoders.
  for (auto &[name, encoder] : encoders_) {
    auto status = encoder.start(&muxer_);
    if (status.code() != 0) {
      return status;
    }
  }

  return {};
}

MediaStatus MediaRecorder::encode(StringView params, const uint8_t *data, size_t byteSize) {
  auto sampleConfig = nlohmann::json::parse(params);
  if (sampleConfig.count("name") == 0 && !sampleConfig["name"].is_string()) {
    return {"MediaRecorder encode params must have string 'name' field"};
  }

  auto encoder = encoders_.find(sampleConfig["name"].get<std::string>());
  if (encoder == encoders_.end()) {
    return {"MediaRecorder can't encode unknown track name"};
  }

  return encoder->second.encode(sampleConfig, data, byteSize);
}

MediaStatus MediaRecorder::encodePlanarYUV(
  StringView params, ConstYPlanePixels y, ConstUPlanePixels u, ConstVPlanePixels v) {
  auto sampleConfig = nlohmann::json::parse(params);
  if (sampleConfig.count("name") == 0 && !sampleConfig["name"].is_string()) {
    return {"MediaRecorder encode params must have string 'name' field"};
  }

  sampleConfig["width"] = y.cols();
  sampleConfig["height"] = y.rows();

  sampleConfig["data0"] = reinterpret_cast<uintptr_t>(y.pixels());
  sampleConfig["data1"] = reinterpret_cast<uintptr_t>(u.pixels());
  sampleConfig["data2"] = reinterpret_cast<uintptr_t>(v.pixels());

  sampleConfig["rowBytes0"] = y.rowBytes();
  sampleConfig["rowBytes1"] = u.rowBytes();
  sampleConfig["rowBytes2"] = v.rowBytes();

  auto encoder = encoders_.find(sampleConfig["name"].get<std::string>());
  if (encoder == encoders_.end()) {
    return {"MediaRecorder can't encode unknown track name"};
  }

  return encoder->second.encode(sampleConfig, y.pixels(), -1);
}

MediaStatus MediaRecorder::finish() {
  if (!muxer_.isValid()) {
    return {"MediaRecorder muxer is invalid"};
  }

  for (auto &[name, encoder] : encoders_) {
    if (!encoder.isValid()) {
      return {"MediaRecorder encoder is invalid"};
    }
    // Finish all of the encoding.
    MediaStatus status = encoder.finish();
    if (status.code() != 0) {
      return status;
    }
  }

  // Close the muxer.
  MediaStatus status = muxer_.close();
  if (status.code() != 0) {
    return status;
  }

  // Remove the muxer and encoders.
  muxer_ = {};
  encoders_.clear();

  return {};
}

}  // namespace c8
