// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "demuxer.h",
  };
  visibility = {
    "//c8/media:__subpackages__",
  };
  deps = {
    ":media-status",
    "//c8/media/codec:codec-api",
    "@json//:json",
  };
}
cc_end(0xc123c15b);

#include "c8/media/demuxer.h"

namespace c8 {

Demuxer::Demuxer(DemuxerApi *demuxer) : demuxer_(demuxer) {}

MediaStatus Demuxer::open(const char *path) {
  if (!demuxer_.get()) {
    return {"Invalid Demuxer"};
  }

  return demuxer_->open(path);
}

MediaStatus Demuxer::forEachTrack(std::function<MediaStatus(const nlohmann::json &)> callback) {
  if (!demuxer_.get()) {
    return {"Invalid Demuxer"};
  }

  return demuxer_->forEachTrack(callback);
}

MediaStatus Demuxer::read(
  const nlohmann::json &readConfig,
  const uint8_t **data,
  size_t *byteSize,
  nlohmann::json *sampleMetadata) {
  if (!demuxer_.get()) {
    return {"Invalid Demuxer"};
  }

  return demuxer_->read(readConfig, data, byteSize, sampleMetadata);
}

MediaStatus Demuxer::close() {
  if (!demuxer_.get()) {
    return {"Invalid Demuxer"};
  }
  return demuxer_->close();
}

bool Demuxer::isValid() const { return demuxer_.get() != nullptr; }

}  // namespace c8
