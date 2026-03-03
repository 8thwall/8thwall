// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "media-decoder.h",
  };
  visibility = {
    "//visibility:public",
  };
  deps = {
    ":decoder",
    ":media-status",
    ":demuxer",
    "//c8/media/codec:registry",
    "//c8/pixels:pixels",
    "//c8:map",
    "//c8:string",
    "//c8:string-view",
    "@json//:json",
  };
}
cc_end(0xb1100439);

#include "c8/media/media-decoder.h"

#include <nlohmann/json.hpp>

#include "c8/media/codec/registry.h"

namespace c8 {

namespace {
using Json = nlohmann::json;

int parseJsonInt(const Json &config, const char *field, int defaultValue) {
  if (config.count(field) > 0 && config[field].is_number_integer()) {
    return config[field].get<int>();
  }
  return defaultValue;
}

String parseJsonString(const nlohmann::json &config, const char *field, const char *defaultValue) {
  if (config.count(field) > 0 && config[field].is_string()) {
    return config[field].get<std::string>();
  }
  return defaultValue;
}

uintptr_t parseJsonPtr(const nlohmann::json &config, const char *field, uintptr_t defaultValue) {
  if (config.count(field) > 0 && config[field].is_number_integer()) {
    return config[field].get<uintptr_t>();
  }
  return defaultValue;
}

}  // namespace

MediaDecoder::~MediaDecoder(){};

MediaStatus MediaDecoder::open(StringView config) {
  // Remove any prior demuxers or decoders.
  demuxer_ = {};
  decoders_.clear();

  // Parse the json config.
  auto jsonConfig = Json::parse(config);

  if (jsonConfig.is_discarded()) {
    return {"JSON syntax error in config"};
  }

  const String path = parseJsonString(jsonConfig, "path", "");
  if (path.empty()) {
    return {"MediaDecoder config must have string 'path' field"};
  }

  // Create the demuxer.
  demuxer_ = DemuxerRegistry::create(jsonConfig);
  if (!demuxer_.isValid()) {
    return {
      "MediaDecoder config has unknown 'path' extension and no 'container' field to specify type"};
  }

  // Open the media file at 'path' for demuxing.
  if (auto status = demuxer_.open(path.c_str()); status.code() != 0) {
    return status;
  }

  // Initialize tracks.
  auto status = demuxer_.forEachTrack([this](const Json &track) -> MediaStatus {
    String name = parseJsonString(track, "name", "");
    if (name.empty()) {
      return {"MediaDecoder track missing string 'name' field"};
    }

    auto [decoder, inserted] = decoders_.emplace(std::move(name), DecoderRegistry::create(track));
    if (!inserted) {
      return {"MediaDecoder track names must be unique"};
    }

    if (!decoder->second.isValid()) {
      return {"MediaDecoder track config has unsupported codec"};
    }
    return {};
  });

  if (status.code() != 0) {
    return status;
  }

  // Start the decoders.
  for (auto &[name, decoder] : decoders_) {
    auto status = decoder.start(&demuxer_);
    if (status.code() != 0) {
      return status;
    }
  }

  return {};
}

MediaStatus MediaDecoder::forEachTrack(std::function<MediaStatus(const char *)> callback) {
  return demuxer_.forEachTrack(
    [callback](const Json &track) -> MediaStatus { return callback(track.dump().c_str()); });
}

MediaStatus MediaDecoder::decode(
  StringView params, const uint8_t **data, size_t *byteSize, String *metadata) {
  auto sampleConfig = Json::parse(params);
  if (sampleConfig.count("name") == 0 && !sampleConfig["name"].is_string()) {
    return {"MediaDecoder decode params must have string 'name' field"};
  }

  auto decoder = decoders_.find(sampleConfig["name"].get<std::string>());
  if (decoder == decoders_.end()) {
    return {"MediaDecoder can't decode unknown track name"};
  }

  Json jsonMetadata;
  auto result = decoder->second.decode(sampleConfig, data, byteSize, &jsonMetadata);
  *metadata = jsonMetadata.dump();

  return result;
}

MediaStatus MediaDecoder::decodePlanarYUV(
  StringView params,
  ConstYPlanePixels *y,
  ConstUPlanePixels *u,
  ConstVPlanePixels *v,
  String *metadata) {
  auto sampleConfig = Json::parse(params);
  if (sampleConfig.count("name") == 0 && !sampleConfig["name"].is_string()) {
    return {"MediaDecoder decode params must have string 'name' field"};
  }

  auto decoder = decoders_.find(sampleConfig["name"].get<std::string>());
  if (decoder == decoders_.end()) {
    return {"MediaDecoder can't decode unknown track name"};
  }

  const uint8_t *data;
  size_t byteSize;
  Json jsonMetadata;

  auto decodeResult = decoder->second.decode(sampleConfig, &data, &byteSize, &jsonMetadata);
  *metadata = jsonMetadata.dump();

  if (decodeResult.code() == MediaStatus::SUCCESS) {
    const String format = parseJsonString(jsonMetadata, "format", "");
    if (format != "yuv420p") {
      return {"MediaDecoder unsupported video format"};
    }
    const int rows = parseJsonInt(jsonMetadata, "height", 0);
    const int cols = parseJsonInt(jsonMetadata, "width", 0);
    const uint8_t *dataY = reinterpret_cast<uint8_t *>(parseJsonPtr(jsonMetadata, "data0", 0));
    const uint8_t *dataU = reinterpret_cast<uint8_t *>(parseJsonPtr(jsonMetadata, "data1", 0));
    const uint8_t *dataV = reinterpret_cast<uint8_t *>(parseJsonPtr(jsonMetadata, "data2", 0));
    const int strideY = parseJsonInt(jsonMetadata, "rowBytes0", 0);
    const int strideU = parseJsonInt(jsonMetadata, "rowBytes1", 0);
    const int strideV = parseJsonInt(jsonMetadata, "rowBytes2", 0);
    *y = ConstYPlanePixels(rows, cols, strideY, dataY);
    *u = ConstUPlanePixels(rows >> 1, cols >> 1, strideU, dataU);
    *v = ConstVPlanePixels(rows >> 1, cols >> 1, strideV, dataV);
  }
  return decodeResult;
}

MediaStatus MediaDecoder::close() {
  if (!demuxer_.isValid()) {
    return {"MediaDecoder muxer is invalid"};
  }

  for (auto &[name, decoder] : decoders_) {
    if (!decoder.isValid()) {
      return {"MediaDecoder decoder is invalid"};
    }
    // Finish the decoder.
    MediaStatus status = decoder.finish();
    if (status.code() != 0) {
      return status;
    }
  }

  // Close the muxer.
  MediaStatus status = demuxer_.close();
  if (status.code() != 0) {
    return status;
  }

  // Remove the demuxer and decoders.
  demuxer_ = {};
  decoders_.clear();

  return {};
}

}  // namespace c8
