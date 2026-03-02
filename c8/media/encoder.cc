// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"encoder.h"};
  visibility = {
    "//c8/media:__subpackages__",
  };
  deps = {
    ":media-status",
    "//c8/media/codec:codec-api",
    "@json//:json",
  };
}
cc_end(0x3d9bae84);

#include "c8/media/encoder.h"

namespace c8 {

Encoder::Encoder(EncoderApi *encoder) : encoder_(encoder) {}

MediaStatus Encoder::start(Muxer *muxer) {
  if (!encoder_.get()) {
    return {"Invalid Encoder"};
  }

  return encoder_->start(muxer);
}

MediaStatus Encoder::encode(
  const nlohmann::json &sampleConfig, const uint8_t *data, size_t byteSize) {
  if (!encoder_.get()) {
    return {"Invalid Encoder"};
  }
  return encoder_->encode(sampleConfig, data, byteSize);
}

MediaStatus Encoder::finish() {
  if (!encoder_.get()) {
    return {"Invalid Encoder"};
  }
  return encoder_->finish();
}

bool Encoder::isValid() const { return encoder_.get() != nullptr; }

}  // namespace c8
