// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"decoder.h"};
  visibility = {
    "//c8/media:__subpackages__",
  };
  deps = {
    ":media-status",
    "//c8/media/codec:codec-api",
    "@json//:json",
  };
}
cc_end(0xddccd0e2);

#include "c8/media/decoder.h"

namespace c8 {

Decoder::Decoder(DecoderApi *decoder) : decoder_(decoder) {}

MediaStatus Decoder::start(Demuxer *demuxer) {
  if (!decoder_.get()) {
    return {"Invalid Decoder"};
  }

  return decoder_->start(demuxer);
}

MediaStatus Decoder::decode(
  const nlohmann::json &sampleConfig,
  const uint8_t **data,
  size_t* byteSize,
  nlohmann::json *sampleMetadata) {
  if (!decoder_.get()) {
    return {"Invalid Decoder"};
  }
  return decoder_->decode(sampleConfig, data, byteSize, sampleMetadata);
}

MediaStatus Decoder::finish() {
  if (!decoder_.get()) {
    return {"Invalid Decoder"};
  }
  return decoder_->finish();
}

bool Decoder::isValid() const { return decoder_.get() != nullptr; }

}  // namespace c8
