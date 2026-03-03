// Copyright (c) 2021 8th Wall, Inc.;
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "media-transcoder.h",
  };
  visibility = {
    "//visibility:public",
  };
  deps = {
    ":media-decoder",
    ":media-recorder",
    "//c8:string",
    "//c8:vector",
    "@json//:json",
  };
}
cc_end(0x595e42ff);

#include "c8/media/media-transcoder.h"

#include <algorithm>
#include <nlohmann/json.hpp>

namespace c8 {

using Json = nlohmann::json;

namespace {

double parseJsonDouble(const Json &config, const char *field, double defaultValue) {
  if (config.count(field) > 0 && config[field].is_number()) {
    return config[field].get<double>();
  }
  return defaultValue;
}

int parseJsonInt(const Json &config, const char *field, int defaultValue) {
  if (config.count(field) > 0 && config[field].is_number_integer()) {
    return config[field].get<int>();
  }
  return defaultValue;
}

uint64_t parseJsonUint64(const Json &config, const char *field, uint64_t defaultValue) {
  if (config.count(field) > 0 && config[field].is_number_integer()) {
    return config[field].get<uint64_t>();
  }
  return defaultValue;
}

String parseJsonString(const nlohmann::json &config, const char *field, const char *defaultValue) {
  if (config.count(field) > 0 && config[field].is_string()) {
    return config[field].get<std::string>();
  }
  return defaultValue;
}

}  // namespace

MediaTranscoder::~MediaTranscoder(){};

MediaStatus MediaTranscoder::open(StringView inputConfig, StringView outputConfig) {
  if (auto result = decoder_.open(inputConfig); result.code() != 0) {
    return result;
  }

  nlohmann::json encoderConfig = nlohmann::json::parse(outputConfig);

  // Keep a copy of the user specified track information to use as an override
  // of the config from the demuxed tracks.
  nlohmann::json userTracks = encoderConfig["tracks"];

  // Most of the track information will be found from the demuxed track.
  encoderConfig["tracks"] = nlohmann::json::array();

  nlohmann::json inputInfo = nlohmann::json::parse(inputConfig);
  inputInfo["tracks"] = nlohmann::json::array();

  // Get the tracks from the source and add them to the destination.
  auto res = decoder_.forEachTrack(
    [this, &inputInfo, &encoderConfig, &userTracks](const char *trackInfo) -> MediaStatus {
      TrackData data;
      data.trackInfo = trackInfo;

      Json trackJson = Json::parse(trackInfo);
      inputInfo["tracks"].push_back(trackJson);

      nlohmann::json encoderTrack;

      data.inputTimescale = parseJsonInt(trackJson, "timescale", 0);
      encoderTrack["name"] = parseJsonString(trackJson, "name", "");

      if (encoderTrack["name"].get<std::string>().rfind("video", 0) == 0) {
        encoderTrack["width"] = parseJsonInt(trackJson, "width", 0);
        encoderTrack["height"] = parseJsonInt(trackJson, "height", 0);
      }

      data.outputTimescale = 90000;
      if (auto sampleRate = parseJsonInt(trackJson, "sampleRate", 0)) {
        data.outputTimescale = sampleRate;
        encoderTrack["sampleRate"] = sampleRate;
      } else {
        double fps = parseJsonDouble(trackJson, "fps", 0.0);
        if (fps > 0.0) {
          encoderTrack["fps"] = fps;
          // Default output timescale is based on the fps.
          constexpr int frameScale = 512;
          data.outputTimescale = frameScale * fps;
          encoderTrack["timescale"] = data.outputTimescale;
          data.defaultFrameDuration = frameScale;
        }
      }

      if (auto channels = parseJsonInt(trackJson, "channels", 0)) {
        encoderTrack["channels"] = channels;
        // For 7 channels and below, the enum value of the mode matches the channel count.
        if (channels <= 7 && encoderTrack.count("channelMode") == 0) {
          encoderTrack["channelMode"] = channels;
        }
      }

      // Add in any user-specified track options.
      for (const auto &userTrack : userTracks) {
        if (userTrack["name"] == encoderTrack["name"]) {
          encoderTrack.update(userTrack);
          data.outputTimescale = parseJsonInt(userTrack, "timescale", data.outputTimescale);
          break;
        }
      }

      data.trackName = parseJsonString(trackJson, "name", "");

      tracks_.emplace_back(0ULL, std::move(data));

      encoderConfig["tracks"].push_back(encoderTrack);

      return {};
    });

  if (res.code() != 0) {
    return res;
  }

  decoderInfo_ = inputInfo.dump();
  encoderInfo_ = encoderConfig.dump();

  // Start the encoder.
  return recorder_.start(encoderInfo_);
}

// Returns information about the input and output media files in JSON format.
MediaStatus MediaTranscoder::getInfo(String *inputInfo, String *outputInfo) {
  *inputInfo = decoderInfo_;
  *outputInfo = encoderInfo_;
  return {};
}

MediaStatus MediaTranscoder::transcode(String *metadata) {
  if (tracks_.empty()) {
    return {MediaStatus::NO_MORE_FRAMES, ""};
  }
  auto track = std::min_element(tracks_.begin(), tracks_.end());
  auto &[nextTimestamp, data] = *track;

  const uint8_t *frameData = nullptr;
  size_t byteSize = 0;

  // Decode the frame.
  auto result = decoder_.decode(data.trackInfo, &frameData, &byteSize, metadata);

  if (result.code() == MediaStatus::NO_MORE_FRAMES) {
    // Remove the track since we are finished with it.
    tracks_.erase(track);
    // Try again for the remaining tracks.
    return transcode(metadata);
  }

  else if (result.code() != MediaStatus::SUCCESS) {
    return result;
  }

  Json sampleInfo = Json::parse(*metadata);
  uint64_t currentTimestamp = parseJsonUint64(sampleInfo, "timestamp", 0);
  int frameDuration = parseJsonInt(sampleInfo, "frameDuration", 0);

  nextTimestamp = currentTimestamp + frameDuration;

  sampleInfo["name"] = data.trackName;

  int64_t ts = currentTimestamp * data.outputTimescale / data.inputTimescale;
  int64_t dur = frameDuration * data.outputTimescale / data.inputTimescale;

  int sampleRate = parseJsonInt(sampleInfo, "sampleRate", 0);
  if (sampleRate != 0) {
    sampleInfo.erase("timestamp");
    sampleInfo.erase("frameDuration");
  } else {
    sampleInfo["timestamp"] = ts;
    // Encode the duration only if it isn't the default duration.
    if (dur == data.defaultFrameDuration) {
      sampleInfo.erase("frameDuration");
    } else {
      sampleInfo["frameDuration"] = dur;
    }
  }

  // Encode the frame.
  data.timestamp = ts;
  return recorder_.encode(sampleInfo.dump(), frameData, byteSize);
}

MediaStatus MediaTranscoder::close() {
  auto decResult = decoder_.close();
  auto encResult = recorder_.finish();

  encoderInfo_.clear();
  decoderInfo_.clear();

  if (decResult.code() != 0) {
    return decResult;
  }
  if (encResult.code() != 0) {
    return encResult;
  }
  return {};
}

}  // namespace c8
