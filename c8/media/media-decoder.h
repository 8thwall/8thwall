// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// MediaDecoder is an extensible video decoding library, intended to be lightweight and
// client-side. At its inception, MediaDecoder had support for decoding .mp4 files with H.264 video
// and AAC audio.
//
// Example Usage:
// TODO(mc): Add example usage.

#include "c8/map.h"
#include "c8/pixels/pixels.h"
#include "c8/string-view.h"
#include "c8/string.h"

#include "c8/media/decoder.h"
#include "c8/media/demuxer.h"
#include "c8/media/media-status.h"

namespace c8 {

class MediaDecoder {
public:
  MediaDecoder() = default;

  ~MediaDecoder();

  // Start the MediaDecoder with a JSON configuration.
  // The 'params' field is a JSON string, containing parameters to pass to the decoder.
  // Here are some common options:
  //   "path": (required string) path to the file.
  MediaStatus open(StringView config);

  // Call the provided callback once per track, providing the caller with
  // track-based metadata.
  MediaStatus forEachTrack(std::function<MediaStatus(const char *)> callback);

  // Decode a sample/frame of input data. The decoder owns the data, which becomes invalid after
  // another call to decode.
  // The 'params' field is a JSON string, containing parameters to pass to the decoder.
  // It supports the following fields:
  //   "name" - (required string) matching the track name to decode.
  MediaStatus decode(StringView params, const uint8_t **data, size_t *byteSize, String *metadata);

  // Decode a planar YUV format. Helpful for video formats like 'yuv420p'.
  // The 'params' field is a JSON string, containing parameters to pass to the decoder.
  // It supports the following fields:
  //   "name": (required string) matching the track name in which to decode.
  MediaStatus decodePlanarYUV(
    StringView params,
    ConstYPlanePixels *y,
    ConstUPlanePixels *u,
    ConstVPlanePixels *v,
    String *metadata);

  // Flush any remaining decoding and close the media.
  MediaStatus close();

  // Default move constructors.
  MediaDecoder(MediaDecoder &&) = default;
  MediaDecoder &operator=(MediaDecoder &&) = default;

  // Disallow copying.
  MediaDecoder(const MediaDecoder &) = delete;
  MediaDecoder &operator=(const MediaDecoder &) = delete;

private:
  Demuxer demuxer_;
  TreeMap<String, Decoder> decoders_;
};

}  // namespace c8
