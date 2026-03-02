// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// MediaTranscoder is an extensible video transcoding library, intended to be lightweight and
// client-side. At its inception, MediaTranscoder had support for encoding and
// decoding .mp4 files with H.264 video and AAC audio.

#include <tuple>

#include "c8/string-view.h"
#include "c8/vector.h"

#include "c8/media/media-decoder.h"
#include "c8/media/media-recorder.h"
#include "c8/media/media-status.h"

namespace c8 {

class MediaTranscoder {
public:
  MediaTranscoder() = default;

  ~MediaTranscoder();

  // Start the MediaTranscoder with a JSON configuration.
  // The 'params' field is a JSON string, containing parameters to pass to the decoder.
  // Here are some common options:
  //   "path": (required string) path to the file.
  MediaStatus open(StringView inputConfig, StringView outputConfig);

  // Returns information about the input and output media files in JSON format.
  MediaStatus getInfo(String* inputInfo, String* outputInfo);

  // Transcode one frame/sample.
  MediaStatus transcode(String *metadata);

  // Flush any remaining transcoding and close the media.
  MediaStatus close();

  // Default move constructors.
  MediaTranscoder(MediaTranscoder &&) = default;
  MediaTranscoder &operator=(MediaTranscoder &&) = default;

  // Disallow copying.
  MediaTranscoder(const MediaTranscoder &) = delete;
  MediaTranscoder &operator=(const MediaTranscoder &) = delete;

private:
  MediaDecoder decoder_;
  MediaRecorder recorder_;

  struct TrackData {
    String trackInfo;
    String trackName;
    int64_t inputTimescale = 0;
    int64_t outputTimescale = 0;
    int64_t timestamp = 0;
    int64_t defaultFrameDuration = 0;
    bool operator<(const TrackData &rhs) const { return false; }
  };

  String decoderInfo_;
  String encoderInfo_;

  // Pair of timestamp and track-specific data.
  Vector<std::tuple<uint64_t, TrackData>> tracks_;
};

}  // namespace c8
