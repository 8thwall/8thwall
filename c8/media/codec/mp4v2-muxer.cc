// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "mp4v2-muxer.h",
  };
  visibility = {
    "//visibility:private",
  };
  deps = {
    ":codec-api",
    "//c8:map",
    "//c8:string",
    "//c8:string-view",
    "//c8/string:strcat",
    "@json//:json",
    "@libmp4v2//:mp4v2",
  };
}
cc_end(0x61adc65c);

#include "c8/media/codec/mp4v2-muxer.h"

#include "c8/string-view.h"
#include "c8/string/strcat.h"

namespace c8 {

namespace {
enum class SampleType {
  UNSPECIFIED = 0,
  SAMPLE = 1,
  ELEMENTARY_STREAM = 2,
  H264_SEQUENCE_PARAMETER_SET = 3,
  H264_PICTURE_PARAMETER_SET = 4,
};

SampleType getSampleType(const char *type) {
  static TreeMap<String, SampleType> map = {
    {"sample", SampleType::SAMPLE},
    {"es", SampleType::ELEMENTARY_STREAM},
    {"sps", SampleType::H264_SEQUENCE_PARAMETER_SET},
    {"pps", SampleType::H264_PICTURE_PARAMETER_SET},
  };

  if (auto t = map.find(type); t != map.end()) {
    return t->second;
  }

  return SampleType::UNSPECIFIED;
}

bool parseJsonBool(const nlohmann::json &config, const char *field, bool defaultValue) {
  if (config.count(field) > 0 && config[field].is_boolean()) {
    return config[field].get<bool>();
  }
  return defaultValue;
}

int parseJsonInt(const nlohmann::json &config, const char *field, int defaultValue) {
  if (config.count(field) > 0 && config[field].is_number_integer()) {
    return config[field].get<int>();
  }
  return defaultValue;
}

}  // namespace

Mp4v2Muxer::Mp4v2Muxer(const nlohmann::json &config) : config_(config), handle_(nullptr) {}

// Virtual destructor.
Mp4v2Muxer::~Mp4v2Muxer() {}

// Open new media file for muxing.
MediaStatus Mp4v2Muxer::open(const char *path) {
  if (handle_) {
    // Although a caller should close() a muxer to finish recording, we need to
    // have a clean state if the muxer is reused. Here we close it ourselves on
    // a new call to open.
    close();
  }

  path_ = path;
  // Get the MP4 file timescale from the config. Default is 90000 if
  // unspecified.
  uint32_t timescale = parseJsonInt(config_, "timescale", 90000);

  handle_ = MP4Create(path, 0);

  if (!handle_) {
    return {"MP4 unable to create file"};
  }

  MP4SetTimeScale(handle_, timescale);

  return {};
}

MediaStatus Mp4v2Muxer::addTrack(const nlohmann::json &trackConfig) {
  if (!handle_) {
    return {"MP4 muxer not initialized"};
  }

  if (trackConfig.count("name") == 0 || !trackConfig["name"].is_string()) {
    return {"MP4 track must have a string 'name' field"};
  }
  if (trackConfig.count("codec") == 0 || !trackConfig["codec"].is_string()) {
    return {"MP4 track must have a string 'codec' field"};
  }
  const String &codec = trackConfig["codec"].get<std::string>();
  if (codec != "h264" && codec != "aac" && codec != "ac3" && codec != "mp3" && codec != "eac3") {
    return {"MP4 muxer expecting one of the following codecs: [h264, aac, ac3, mp3, eac3]"};
  }

  uint32_t trackTimescale = parseJsonInt(trackConfig, "timescale", 90000);

  MP4Duration frameDuration = MP4_INVALID_DURATION;
  if (trackConfig.count("fps") > 0) {
    if (trackConfig.count("frameDuration") > 0) {
      return {"MP4 config cannot specify both 'fps' and 'frameDuration'"};
    }
    if (!trackConfig["fps"].is_number()) {
      return {"MP4 'fps' field must be a number"};
    }
    frameDuration = static_cast<MP4Duration>(trackTimescale / trackConfig["fps"].get<double>());
  }
  if (trackConfig.count("frameDuration") > 0) {
    if (trackConfig.count("fps") > 0) {
      return {"MP4 config cannot specify both 'frameDuration' and 'fps'"};
    }
    if (!trackConfig["frameDuration"].is_number()) {
      return {"MP4 'frameDuration' field must be a number"};
    }
    frameDuration = static_cast<MP4Duration>(trackConfig["frameDuration"].get<int>());
  }

  auto [unused, inserted] = tracks_.emplace(
    std::piecewise_construct,
    std::forward_as_tuple(trackConfig["name"].get<std::string>()),
    std::forward_as_tuple(MP4_INVALID_TRACK_ID, codec, trackTimescale, frameDuration));
  if (!inserted) {
    return {"MP4 config has duplicate track names"};
  }

  trackConfig["name"].get<std::string>();

  return {};
}

MediaStatus Mp4v2Muxer::write(
  const nlohmann::json &writeConfig, const uint8_t *data, size_t byteSize) {
  if (!handle_) {
    return {"MP4 muxer not initialized"};
  }

  if (writeConfig.count("name") == 0 || !writeConfig["name"].is_string()) {
    return {"MP4 writeConfig must have a string 'name' field"};
  }
  const String &name = writeConfig["name"].get<std::string>();

  if (writeConfig.count("codec") == 0 || !writeConfig["codec"].is_string()) {
    return {"MP4 writeConfig must have a string 'codec' field"};
  }
  const String &codec = writeConfig["codec"].get<std::string>();

  if (writeConfig.count("type") == 0 || !writeConfig["type"].is_string()) {
    return {"MP4 writeConfig must have a string 'type' field"};
  }
  SampleType type = getSampleType(writeConfig["type"].get<std::string>().c_str());

  auto trackConfig = tracks_.find(name);

  if (trackConfig == tracks_.end()) {
    return {"MP4 writeConfig 'name' field must match a track name"};
  }
  auto &track = trackConfig->second;

  MP4Duration writeConfigFrameDuration = track.frameDuration;
  if (writeConfig.count("fps") != 0) {
    return {"MP4 writeConfig should not contain an 'fps' field"};
  }
  if (writeConfig.count("frameDuration") > 0) {
    if (!writeConfig["frameDuration"].is_number()) {
      return {"MP4 'frameDuration' field must be a number"};
    }
    // Use a per-frame frameDuration.
    writeConfigFrameDuration = static_cast<MP4Duration>(writeConfig["frameDuration"].get<int>());
  }

  bool isSyncFrame = parseJsonBool(writeConfig, "syncFrame", true);
  MP4Duration renderingOffset = parseJsonInt(writeConfig, "renderingOffset", 0);

  uint16_t width = 0;
  uint16_t height = 0;
  if (codec == "h264") {
    // This is a video track.
    if (writeConfig.count("width") == 0 || !writeConfig["width"].is_number_integer()) {
      return {"MP4 writeConfig for video tracks must have integer 'width' field"};
    }
    width = writeConfig["width"].get<int>();
    if (writeConfig.count("height") == 0 || !writeConfig["height"].is_number_integer()) {
      return {"MP4 writeConfig for video tracks must have integer 'height' field"};
    }
    height = writeConfig["height"].get<int>();
  }

  if (track.id == MP4_INVALID_TRACK_ID) {
    if (writeConfig.count("profile") == 0 || !writeConfig["profile"].is_number_integer()) {
      return {"MP4 writeConfig must have integer 'profile' field on first sample"};
    }
    uint8_t profile = writeConfig["profile"].get<int>();

    if (
      type == SampleType::SAMPLE && track.frameDuration == MP4_INVALID_DURATION
      && writeConfigFrameDuration == MP4_INVALID_DURATION) {
      return {"MP4 writeConfig 'frameDuration' missing for variable framerate track"};
    }

    if (codec == "h264") {
      // Initialize the video track.
      if (type == SampleType::H264_SEQUENCE_PARAMETER_SET) {
        track.width = width;
        track.height = height;

        // H.264 initializes based on the first SPS unit.
        track.id = MP4AddH264VideoTrack(
          handle_,
          track.timescale,
          track.frameDuration,
          width,
          height,
          data[1],  // sps[1] AVCProfileIndication
          data[2],  // sps[2] profile_compat
          data[3],  // sps[3] AVCLevelIndication
          3         // 4 bytes length before each NAL unit
        );
        MP4SetVideoProfileLevel(handle_, profile);
      }
    } else {
      // Initialize the audio track.
      track.id =
        MP4AddAudioTrack(handle_, track.timescale, track.frameDuration, MP4_MPEG4_AUDIO_TYPE);
      MP4SetAudioProfileLevel(handle_, profile);
    }
  }

  if (track.id != MP4_INVALID_TRACK_ID && data && byteSize > 0) {
    if (track.width != width || track.height != height) {
      return {"MP4 writeConfig 'width' or 'height' does not match video track dimensions"};
    }

    switch (type) {
      case SampleType::ELEMENTARY_STREAM:
        MP4SetTrackESConfiguration(handle_, track.id, data, byteSize);
        break;
      case SampleType::H264_PICTURE_PARAMETER_SET:
        MP4AddH264PictureParameterSet(handle_, track.id, data, byteSize);
        break;
      case SampleType::H264_SEQUENCE_PARAMETER_SET:
        MP4AddH264SequenceParameterSet(handle_, track.id, data, byteSize);
        break;
      case SampleType::SAMPLE:
        MP4WriteSample(
          handle_,
          track.id,
          data,
          byteSize,
          writeConfigFrameDuration,
          renderingOffset,
          isSyncFrame);
        break;
      case SampleType::UNSPECIFIED:
        return {"MP4 writeConfig 'type' field must be one of [sample, es, sps, pps]"};
        break;
    }
  }

  return {};
}  // namespace c8

// Close and finish writing media file.
MediaStatus Mp4v2Muxer::close() {
  if (!handle_) {
    return {"MP4 muxer not initialized"};
  }

  MP4Close(handle_);

  bool optimize = parseJsonBool(config_, "optimize", false);

  if (optimize) {
    // Re-write the file with the frames and data reorganized for playback
    // without seeking.
    MP4Optimize(path_.c_str());
  }

  path_.clear();
  handle_ = nullptr;
  tracks_.clear();
  config_.clear();

  return {};
}

}  // namespace c8
