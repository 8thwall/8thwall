// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "mp4v2-demuxer.h",
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
    "//c8:vector",
    "@json//:json",
    "@libmp4v2//:mp4v2",
  };
}
cc_end(0x84f068bc);

#include "c8/media/codec/mp4v2-demuxer.h"

#include <filesystem>

#include "c8/string-view.h"
#include "c8/string/strcat.h"

namespace c8 {

namespace {

using Json = ::nlohmann::json;

}  // namespace

Mp4v2Demuxer::Mp4v2Demuxer(const Json &config) : config_(config), handle_(nullptr) {}

Mp4v2Demuxer::~Mp4v2Demuxer() {}

MediaStatus Mp4v2Demuxer::open(const char *path) {
  if (handle_) {
    // Although a caller should close() a demuxer to finish use, we need to
    // have a clean state if the demuxer is reused. Here we close it ourselves on
    // a new call to open.
    close();
  }

  path_ = path;

  if (!std::filesystem::exists(path)) {
    return {"MP4 file does not exist"};
  }

  // Open the media file for reading.
  handle_ = MP4Read(path);

  if (!handle_) {
    return {"MP4 error opening file"};
  }

  auto numTracks = MP4GetNumberOfTracks(handle_);

  for (int i = 0; i < numTracks; ++i) {
    MP4TrackId trackId = MP4FindTrackId(handle_, i);

    TrackConfig track(trackId);

    Json trackConfig;

    const char *trackType = MP4GetTrackType(handle_, trackId);

    track.duration = MP4GetTrackDuration(handle_, trackId);
    track.timescale = MP4GetTrackTimeScale(handle_, trackId);

    track.samples = MP4GetTrackNumberOfSamples(handle_, trackId);

    const char *dataName = MP4GetTrackMediaDataName(handle_, trackId);

    track.codec = "unknown";
    if (!strcasecmp(dataName, "avc1") || !strcasecmp(dataName, "264b")) {
      track.codec = "h264";
      if (uint8_t profile, level; MP4GetTrackH264ProfileLevel(handle_, trackId, &profile, &level)) {
        track.profile = profile;
        track.level = level;
      }

      MP4GetTrackH264SeqPictHeaders(
        handle_,
        trackId,
        &track.spsHeaders,
        &track.spsHeaderSizes,
        &track.ppsHeaders,
        &track.ppsHeaderSizes);
    } else if (!strcasecmp(dataName, "mp4a")) {
      uint8_t audioType = MP4GetTrackEsdsObjectTypeId(handle_, trackId);
      if (MP4_IS_AAC_AUDIO_TYPE(audioType)) {
        track.codec = "aac";
        track.profile = audioType;
      }
    }

    String name;
    if (!strcmp(trackType, MP4_AUDIO_TRACK_TYPE)) {
      name = "audio";
    } else if (!strcmp(trackType, MP4_VIDEO_TRACK_TYPE)) {
      name = "video";
    } else {
      name = "data";
    }

    // Count any tracks that have this name as prefix.
    int sameNameCount = std::count_if(tracks_.cbegin(), tracks_.cend(), [&name](const auto &trk) {
      if (trk.first.rfind(name, 0) == 0) {
        return true;
      } else {
        return false;
      }
    });

    if (sameNameCount > 0) {
      name += std::to_string(sameNameCount);
    }

    if (!strcmp(trackType, MP4_VIDEO_TRACK_TYPE)) {
      // The real answer can be buried inside the ES configuration info
      track.width = MP4GetTrackVideoWidth(handle_, trackId);
      track.height = MP4GetTrackVideoHeight(handle_, trackId);
    }

    size_t bufferSize = MP4GetTrackMaxSampleSize(handle_, track.id);

    if (track.codec == "h264") {
      if (track.spsHeaders && track.spsHeaderSizes) {
        for (int i = 0; i < (track.spsHeaders[i] && track.spsHeaderSizes[i]); i++) {
          bufferSize += 4 + track.spsHeaderSizes[i];
        }
      }
      if (track.ppsHeaders && track.ppsHeaderSizes) {
        for (int i = 0; i < (track.ppsHeaders[i] && track.ppsHeaderSizes[i]); i++) {
          bufferSize += 4 + track.ppsHeaderSizes[i];
        }
      }
    }

    track.buffer.resize(bufferSize);

    tracks_.emplace(name, std::move(track));
  }

  return {};
}

MediaStatus Mp4v2Demuxer::forEachTrack(std::function<MediaStatus(const Json &)> callback) {
  if (!handle_) {
    return {"MP4 demuxer not initialized"};
  }

  for (const auto &[trackName, trackInfo] : tracks_) {
    Json trackConfig;
    trackConfig["name"] = trackName;
    trackConfig["codec"] = trackInfo.codec;
    trackConfig["timescale"] = trackInfo.timescale;
    trackConfig["samples"] = trackInfo.samples;

    if (trackInfo.duration) {
      trackConfig["duration"] = trackInfo.duration;
    }

    if (trackInfo.width || trackInfo.height) {
      trackConfig["width"] = trackInfo.width;
      trackConfig["height"] = trackInfo.height;
    }

    if (trackInfo.profile) {
      trackConfig["profile"] = trackInfo.profile;
    }

    if (trackInfo.level) {
      trackConfig["level"] = trackInfo.level;
    }

    auto status = callback(trackConfig);
    if (status.code() != 0) {
      return status;
    }
  }

  return {};
}

MediaStatus Mp4v2Demuxer::read(
  const Json &readConfig, const uint8_t **data, size_t *byteSize, Json *sampleMetadata) {
  if (!handle_) {
    return {"MP4 demuxer not initialized"};
  }

  if (readConfig.count("name") == 0 || !readConfig["name"].is_string()) {
    return {"MP4 sampleConfig must have a string 'name' field specifying the track"};
  }
  const String &name = readConfig["name"].get<std::string>();

  auto trackConfig = tracks_.find(name);
  if (trackConfig == tracks_.end()) {
    return {"MP4 readConfig 'name' field must match a track name"};
  }
  auto &track = trackConfig->second;

  if (track.currentSample >= track.samples) {
    return {MediaStatus::NO_MORE_FRAMES, "Track complete"};
  }

  Json &metadata = *sampleMetadata;
  metadata.clear();

  if (
    readConfig.count("type") > 0 && readConfig["type"].is_string()
    && readConfig["type"].get<std::string>() == "es") {
    // This is a request for an elementary stream, not a sample.
    uint8_t *esData = nullptr;
    uint32_t esDataSize = 0;
    MP4GetTrackESConfiguration(handle_, track.id, &esData, &esDataSize);
    *data = esData;
    *byteSize = esDataSize;
    return {};
  }

  constexpr uint8_t naluHeader[4] = {0u, 0u, 0u, 1u};
  uint32_t sampleOffset = 0;
  track.currentSample++;
  if (track.codec == "h264" && MP4GetSampleSync(handle_, track.id, track.currentSample)) {
    // This is a keyframe, add SPS and PPS.
    if (track.spsHeaders && track.spsHeaderSizes) {
      for (int i = 0; i < (track.spsHeaders[i] && track.spsHeaderSizes[i]); i++) {
        memcpy(track.buffer.data() + sampleOffset, naluHeader, sizeof(naluHeader));
        sampleOffset += sizeof(naluHeader);
        memcpy(track.buffer.data() + sampleOffset, track.spsHeaders[i], track.spsHeaderSizes[i]);
        sampleOffset += track.spsHeaderSizes[i];
      }
    }
    if (track.ppsHeaders && track.ppsHeaderSizes) {
      for (int i = 0; i < (track.ppsHeaders[i] && track.ppsHeaderSizes[i]); i++) {
        memcpy(track.buffer.data() + sampleOffset, naluHeader, sizeof(naluHeader));
        sampleOffset += sizeof(naluHeader);
        memcpy(track.buffer.data() + sampleOffset, track.ppsHeaders[i], track.ppsHeaderSizes[i]);
        sampleOffset += track.ppsHeaderSizes[i];
      }
    }
  }

  MP4Timestamp timestamp = 0;
  MP4Duration duration = 0;
  MP4Duration renderingOffset = 0;
  bool isSyncSample;
  uint8_t *sampleStart = track.buffer.data() + sampleOffset;
  uint32_t sampleSize = track.buffer.size() - sampleOffset;
  bool result = MP4ReadSample(
    handle_,
    track.id,
    track.currentSample,
    &sampleStart,
    &sampleSize,
    &timestamp,
    &duration,
    &renderingOffset,
    &isSyncSample);

  if (!result) {
    return {"MP4 demuxer failed to read sample"};
  }

  if (track.codec == "h264" && sampleSize >= sizeof(naluHeader)) {
    // Add nalu header at beginning of sample.
    memcpy(sampleStart, naluHeader, sizeof(naluHeader));
  }

  *data = track.buffer.data();
  *byteSize = sampleSize + sampleOffset;
  metadata["timestamp"] = timestamp;
  metadata["frameDuration"] = duration;
  metadata["renderingOffset"] = renderingOffset;
  metadata["syncFrame"] = isSyncSample;

  return {};
}

MediaStatus Mp4v2Demuxer::close() {
  if (!handle_) {
    return {"MP4 demuxer not initialized"};
  }

  MP4Close(handle_);

  config_.clear();
  path_.clear();
  handle_ = nullptr;
  tracks_.clear();

  return {};
}

}  // namespace c8
