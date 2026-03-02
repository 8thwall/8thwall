// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "mkv-demuxer.h",
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
    "@libwebm//:libwebm",
  };
}
cc_end(0x83099f3a);

#include "c8/media/codec/mkv-demuxer.h"

#include <filesystem>

#include "c8/string-view.h"
#include "c8/string/strcat.h"

namespace c8 {

namespace {

using Json = ::nlohmann::json;

int parseJsonInt(const nlohmann::json &config, const char *field, int defaultValue) {
  if (config.count(field) > 0 && config[field].is_number_integer()) {
    return config[field].get<int>();
  }
  return defaultValue;
}

}  // namespace

MkvDemuxer::MkvDemuxer(const Json &config) : config_(config), reader_() {}

MkvDemuxer::~MkvDemuxer() {}

MediaStatus MkvDemuxer::open(const char *path) {
  if (reader_.get()) {
    // Although a caller should close() a demuxer to finish use, we need to
    // have a clean state if the demuxer is reused. Here we close it ourselves on
    // a new call to open.
    close();
  }

  reader_.reset(new mkvparser::MkvReader());

  path_ = path;

  if (!std::filesystem::exists(path)) {
    return {"MP4 file does not exist"};
  }

  if (auto result = reader_->Open(path); result != 0) {
    return {result, "MKV/WEBM file is invalid"};
  }

  long long pos = 0;
  mkvparser::EBMLHeader ebml_header;
  int ret = ebml_header.Parse(reader_.get(), pos);
  if (ret) {
    return {ret, "MKV demuxer failed to parse EBML header"};
  }

  mkvparser::Segment *segmentPtr;
  ret = mkvparser::Segment::CreateInstance(reader_.get(), pos, segmentPtr);
  if (ret) {
    return {ret, "MKV demuxer failed to create Segment instance"};
  }

  segment_.reset(segmentPtr);
  ret = segment_->Load();
  if (ret < 0) {
    return {ret, "MKV demuxer failed to load Segment"};
  }

  const mkvparser::SegmentInfo *const segmentInfo = segment_->GetInfo();
  if (segmentInfo == nullptr) {
    return {ret, "MKV demuxer failed to get SegmentInfo"};
  }

  const mkvparser::Tracks *const tracks = segment_->GetTracks();
  if (tracks == nullptr) {
    return {ret, "MKV demuxer failed to get tracks"};
  }

  const unsigned long clusterCount = segment_->GetCount();
  if (clusterCount == 0) {
    return {ret, "MKV Segment has no Clusters"};
  }

  auto numTracks = tracks->GetTracksCount();

  for (int i = 0; i < numTracks; ++i) {
    const mkvparser::Track *track = tracks->GetTrackByIndex(i);

    TrackConfig trackConfig(track->GetNumber());

    // Start with a 1k initial track buffer.
    trackConfig.buffer.resize(1024);

    if (0 == strcmp(track->GetCodecId(), "V_VP8")) {
      trackConfig.codec = "vp8";
    } else if (0 == strcmp(track->GetCodecId(), "V_VP9")) {
      trackConfig.codec = "vp9";
    } else if (0 == strcmp(track->GetCodecId(), "A_VORBIS")) {
      trackConfig.codec = "vorbis";
    } else if (0 == strcmp(track->GetCodecId(), "A_OPUS")) {
      trackConfig.codec = "opus";
    } else if (0 == strcmp(track->GetCodecId(), "V_MPEG4/ISO/AVC")) {
      trackConfig.codec = "h264";
    } else if (0 == strncmp(track->GetCodecId(), "A_AAC", 5)) {
      trackConfig.codec = "aac";
    }

    trackConfig.defaultFrameDuration = track->GetDefaultDuration();

    // Count the number of frames in the track and the duration of the track.
    MediaStatus more = advanceTrack(trackConfig);
    int64_t totalFrames = 0;
    int64_t duration = 0;
    int64_t lastFrameTimestamp = 0;
    int64_t lastFrameDuration = 0;
    while (more.code() == 0) {
      ++totalFrames;
      int64_t timestamp = trackConfig.block->GetTime(trackConfig.cluster);

      // First check if there is a custom duration on this block.
      int64_t blockDuration =
        (trackConfig.blockEntry->GetKind() == mkvparser::BlockEntry::kBlockGroup)
        ? static_cast<const mkvparser::BlockGroup *>(trackConfig.blockEntry)->GetDurationTimeCode()
        : 0;

      // If that isn't the case, use the default duration.
      if (blockDuration <= 0) {
        blockDuration = trackConfig.defaultFrameDuration;
      }

      // Advance the track.
      more = advanceTrack(trackConfig);

      if (blockDuration == 0) {
        // If the blockDuration is still zero, this means there is default
        // duration in the track, so let's compute duration from the timestamp
        // interval.
        if (more.code() == MediaStatus::SUCCESS) {
          // Compute the timestamp difference.
          blockDuration = trackConfig.block->GetTime(trackConfig.cluster) - timestamp;
        } else if (more.code() == MediaStatus::NO_MORE_FRAMES) {
          // This was the last frame, so let's estimate that the last frame
          // duration matches the penultimate frame.
          blockDuration = lastFrameDuration;
        }
      }

      // Advance the track duration.
      duration += blockDuration;

      // Set the last frame timestamp and duration.
      lastFrameTimestamp = timestamp;
      lastFrameDuration = blockDuration;
    }
    // Restart the track.
    trackConfig.state = TrackConfig::UNSTARTED;

    // Container timescale in seconds.
    int64_t containerTimescale = 1000 * segmentInfo->GetTimeCodeScale();

    int64_t trackTimescale = containerTimescale;

    String name;
    switch (track->GetType()) {
      case mkvparser::Track::kVideo: {
        name = "video";
        const mkvparser::VideoTrack *const videoTrack =
          static_cast<const mkvparser::VideoTrack *>(track);
        trackConfig.width = videoTrack->GetWidth();
        trackConfig.height = videoTrack->GetHeight();
        trackConfig.fps = videoTrack->GetFrameRate();
        trackConfig.timescale = 1000 * parseJsonInt(config_, "timescale", 1000000);
        if (trackConfig.fps == 0.0 && trackConfig.defaultFrameDuration) {
          trackConfig.fps =
            static_cast<double>(trackConfig.timescale) / trackConfig.defaultFrameDuration;
        }
        break;
      }
      case mkvparser::Track::kAudio: {
        name = "audio";
        const mkvparser::AudioTrack *const audioTrack =
          static_cast<const mkvparser::AudioTrack *>(track);

        if (trackConfig.codec == "opus") {
          // Opus recommends always decoding 48000 regardless of the reported container metadata
          trackConfig.samplingRate = 48000;
        } else {
          trackConfig.samplingRate = static_cast<int>(audioTrack->GetSamplingRate());
        }

        // Use the samplingRate as the timescale for audio tracks.
        trackTimescale = trackConfig.samplingRate;
        trackConfig.channels = audioTrack->GetChannels();
        trackConfig.bitDepth = audioTrack->GetBitDepth();
        break;
      }
      case mkvparser::Track::kSubtitle:
        name = "subtitle";
        break;
      case mkvparser::Track::kMetadata:
      default:
        name = "data";
        break;
    }

    trackConfig.samples = totalFrames;
    trackConfig.duration =
      std::llround(static_cast<double>(trackTimescale) * duration / containerTimescale);

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

    if (trackConfig.codec == "h264") {
      // If this is an h264 track, we need to parse the SPS and PPS headers from
      // an AVCC format buffer into an Annex-B preample for IDR-frames.
      constexpr uint8_t naluHeader[4] = {0u, 0u, 0u, 1u};
      const mkvparser::Track *const mkvTrack =
        segment_->GetTracks()->GetTrackByNumber(trackConfig.id);

      // The AVCC format consists of the following:
      //   8 bits = 0x01
      //   8 bits of avc profile (from sps[0][1])
      //   8 bits of avc compatibility (from sps[0][2])
      //   8 bits of avc level (from sps[0][3])
      //   6 bits = 0b111111
      //   2 bits of NALULengthSizeMinusOne (typically 3)
      //   3 bits of 0b111
      //   5 bits of number of SPS NALU (usually 1)
      //   Repeated per SPS
      //   16 bits of SPS size
      //   Variable SPS NALU data
      //   8 bits of number of PPS NALU (usually 1)
      //   Repeated once per PPS
      //   16 bits of PPS size
      //   Variable PPS NALU data

      size_t spsPpsSize = 0;
      const uint8_t *spsPps = mkvTrack->GetCodecPrivate(spsPpsSize);

      if (spsPpsSize < 6) {
        return {"MKV invalid CodecPrivate data size for h264"};
      }

      trackConfig.profile = spsPps[1];
      trackConfig.level = spsPps[3];
      const uint8_t numSps = spsPps[5] & 0b11111;

      const uint8_t *cursor = spsPps + 6;
      int remaining = spsPpsSize - 6;
      for (int i = 0; i < numSps; ++i) {
        if (remaining < 2) {
          return {"MKV invalid CodecPrivate section h264"};
        }
        const uint16_t naluSize = (cursor[0] << 8) + cursor[1];
        cursor += 2;
        remaining -= 2;

        if (remaining < naluSize) {
          return {"MKV invalid CodecPrivate section h264"};
        }

        while (trackConfig.buffer.size()
               < trackConfig.sampleOffset + sizeof(naluHeader) + naluSize) {
          trackConfig.buffer.resize(trackConfig.buffer.size() << 1);
        }

        memcpy(
          trackConfig.buffer.data() + trackConfig.sampleOffset, naluHeader, sizeof(naluHeader));
        trackConfig.sampleOffset += sizeof(naluHeader);

        memcpy(trackConfig.buffer.data() + trackConfig.sampleOffset, cursor, naluSize);
        trackConfig.sampleOffset += naluSize;

        cursor += naluSize;
        remaining -= naluSize;
      }
      if (remaining < 1) {
        return {"MKV invalid CodecPrivate section h264"};
      }
      const uint8_t numPps = cursor[0];
      cursor += 1;
      remaining -= 1;
      for (int i = 0; i < numPps; ++i) {
        if (remaining < 2) {
          return {"MKV invalid CodecPrivate section h264"};
        }
        const uint16_t naluSize = (cursor[0] << 8) + cursor[1];
        cursor += 2;
        remaining -= 2;

        if (remaining < naluSize) {
          return {"MKV invalid CodecPrivate section h264"};
        }

        while (trackConfig.buffer.size()
               < trackConfig.sampleOffset + sizeof(naluHeader) + naluSize) {
          trackConfig.buffer.resize(trackConfig.buffer.size() << 1);
        }

        memcpy(
          trackConfig.buffer.data() + trackConfig.sampleOffset, naluHeader, sizeof(naluHeader));
        trackConfig.sampleOffset += sizeof(naluHeader);

        memcpy(trackConfig.buffer.data() + trackConfig.sampleOffset, cursor, naluSize);
        trackConfig.sampleOffset += naluSize;

        cursor += naluSize;
        remaining -= naluSize;
      }
    }

    tracks_.emplace(name, std::move(trackConfig));
  }

  return {};
}

MediaStatus MkvDemuxer::forEachTrack(std::function<MediaStatus(const Json &)> callback) {
  if (!reader_.get()) {
    return {"MKV demuxer not initialized"};
  }

  for (const auto &[trackName, trackInfo] : tracks_) {
    Json trackConfig;
    trackConfig["name"] = trackName;
    trackConfig["codec"] = trackInfo.codec;

    trackConfig["samples"] = trackInfo.samples;

    if (trackInfo.width || trackInfo.height) {
      trackConfig["width"] = trackInfo.width;
      trackConfig["height"] = trackInfo.height;
    }

    if (trackInfo.samplingRate) {
      trackConfig["sampleRate"] = trackInfo.samplingRate;
      trackConfig["duration"] = trackInfo.duration;
    } else {
      trackConfig["fps"] = trackInfo.fps;
      trackConfig["duration"] = trackInfo.duration / 1000;
    }

    if (trackInfo.timescale) {
      trackConfig["timescale"] = trackInfo.timescale / 1000;
    } else {
      if (trackInfo.samplingRate) {
        trackConfig["timescale"] = trackInfo.samplingRate;
      }
    }

    if (trackInfo.channels) {
      trackConfig["channels"] = trackInfo.channels;
    }

    auto status = callback(trackConfig);
    if (status.code() != 0) {
      return status;
    }
  }

  return {};
}

MediaStatus MkvDemuxer::advanceTrack(TrackConfig &track) {
  // To advance the track we have to start with the blockFrame and work our way
  // outwards.

  switch (track.state) {
    case TrackConfig::UNSTARTED:
      track.state = TrackConfig::ADVANCING;
      track.cluster = segment_->GetFirst();
      track.blockEntry = nullptr;
      track.block = nullptr;
      break;
    case TrackConfig::ADVANCING:
      goto resume;
    case TrackConfig::FINISHED:
      return {MediaStatus::NO_MORE_FRAMES, "Track complete"};
  }

  while (track.cluster != NULL && !track.cluster->EOS()) {
    if (0 > track.cluster->GetFirst(track.blockEntry)) {
      return {"MKV error parsing first Block of Cluster"};
    }

    while (track.blockEntry != NULL && !track.blockEntry->EOS()) {
      track.block = track.blockEntry->GetBlock();
      bool blockMatchesTrack;

      {
        const long long trackNum = track.block->GetTrackNumber();
        const unsigned long tn = static_cast<unsigned long>(trackNum);
        const mkvparser::Track *const mkvTrack = segment_->GetTracks()->GetTrackByNumber(tn);

        if (mkvTrack == NULL) {
          return {"MKV unknown track\n"};
        }

        blockMatchesTrack = (track.id == mkvTrack->GetNumber());
      }

      if (blockMatchesTrack) {
        for (track.blockFrame = 0; track.blockFrame < track.block->GetFrameCount();
             ++track.blockFrame) {
          return {};
        resume:;
        }
      }

      if (0 > track.cluster->GetNext(track.blockEntry, track.blockEntry)) {
        return {"MKV error parsing next Block of Cluster"};
      }
    }

    track.cluster = segment_->GetNext(track.cluster);
  }
  track.state = TrackConfig::FINISHED;
  return {MediaStatus::NO_MORE_FRAMES, "Track complete"};
}

MediaStatus MkvDemuxer::read(
  const Json &readConfig, const uint8_t **data, size_t *byteSize, Json *sampleMetadata) {

  if (!reader_.get()) {
    return {"MKV demuxer not initialized"};
  }

  if (readConfig.count("name") == 0 || !readConfig["name"].is_string()) {
    return {"MKV sampleConfig must have a string 'name' field specifying the track"};
  }
  const String &name = readConfig["name"].get<std::string>();

  auto trackConfig = tracks_.find(name);
  if (trackConfig == tracks_.end()) {
    return {"MP4 readConfig 'name' field must match a track name"};
  }
  auto &track = trackConfig->second;

  if (
    readConfig.count("type") > 0 && readConfig["type"].is_string()
    && readConfig["type"].get<std::string>() == "es") {
    // This is a request for an elementary stream, not a sample.
    const mkvparser::Track *const mkvTrack = segment_->GetTracks()->GetTrackByNumber(track.id);
    *data = mkvTrack->GetCodecPrivate(*byteSize);
    return {};
  }

  if (auto result = advanceTrack(track); result.code() != 0) {
    return result;
  }

  Json &metadata = *sampleMetadata;
  metadata.clear();

  const mkvparser::Block::Frame &frame = track.block->GetFrame(track.blockFrame);

  while (track.buffer.size() < (frame.len + track.sampleOffset)) {
    track.buffer.resize((frame.len << 1) + track.sampleOffset);
  }

  // Read the data out of the MKV file.
  frame.Read(reader_.get(), track.buffer.data() + track.sampleOffset);

  if (track.codec == "h264") {
    // Write the nalu header over the sample.
    constexpr uint8_t naluHeader[4] = {0u, 0u, 0u, 1u};
    memcpy(track.buffer.data() + track.sampleOffset, naluHeader, sizeof(naluHeader));
  }

  if (track.codec == "h264" && track.block->IsKey()) {
    // If this is an H.264 keyframe, start the frame with SPS and PPS nalus.
    *data = track.buffer.data();
    *byteSize = frame.len + track.sampleOffset;
  } else {
    *data = track.buffer.data() + track.sampleOffset;
    *byteSize = frame.len;
  }

  metadata["timestamp"] = track.block->GetTime(track.cluster) / 1000;
  metadata["syncFrame"] = track.block->IsKey() || /* isAudio */ (track.samplingRate > 0);
  metadata["codec"] = track.codec;

  if (track.channels > 0) {
    metadata["channels"] = track.channels;
  }
  if (track.samplingRate > 0) {
    metadata["sampleRate"] = track.samplingRate;
  }

  // NOTE: There are likely ways to get the rendering offset or time-adjustment
  // for B-frames when certain codecs are put in an MKV. I know not this wisdom.
  metadata["renderingOffset"] = 0;

  if (track.blockEntry->GetKind() == mkvparser::BlockEntry::kBlockGroup) {
    metadata["frameDuration"] =
      static_cast<const mkvparser::BlockGroup *>(track.blockEntry)->GetDurationTimeCode() / 1000;
  } else {
    metadata["frameDuration"] = track.defaultFrameDuration / 1000;
  }

  return {};
}

MediaStatus MkvDemuxer::close() {
  if (!reader_.get()) {
    return {"MKV demuxer not initialized"};
  }

  path_ = "";
  segment_.reset();
  tracks_.clear();
  config_.clear();
  reader_.reset();

  return {};
}

}  // namespace c8
