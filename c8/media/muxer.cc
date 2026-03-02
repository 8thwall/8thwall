// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "muxer.h",
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
cc_end(0x95292c0d);

#include "c8/media/muxer.h"

namespace c8 {

Muxer::Muxer(MuxerApi *muxer) : muxer_(muxer) {}

MediaStatus Muxer::open(const char *path) {
  if (!muxer_.get()) {
    return {"Invalid Muxer"};
  }

  return muxer_->open(path);
}

MediaStatus Muxer::addTrack(const nlohmann::json &trackConfig) {
  if (!muxer_.get()) {
    return {"Invalid Muxer"};
  }
  return muxer_->addTrack(trackConfig);
}

MediaStatus Muxer::write(const nlohmann::json &writeConfig, const uint8_t *data, size_t byteSize) {
  if (!muxer_.get()) {
    return {"Invalid Muxer"};
  }
  return muxer_->write(writeConfig, data, byteSize);
}

MediaStatus Muxer::close() {
  if (!muxer_.get()) {
    return {"Invalid Muxer"};
  }
  return muxer_->close();
}

bool Muxer::isValid() const { return muxer_.get() != nullptr; }

}  // namespace c8
