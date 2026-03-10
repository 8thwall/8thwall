// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Status return object for media and codecs.

#pragma once

namespace c8 {

class MediaStatus {
public:
  enum Code {
    SUCCESS = 0,
    NO_MORE_FRAMES = 100,
  };

  constexpr MediaStatus() : code_(0), message_("") {}
  constexpr MediaStatus(const char *message) : code_(1), message_(message) {}
  constexpr MediaStatus(int code, const char *message) : code_(code), message_(message) {}

  constexpr int code() const { return code_; }
  constexpr const char *message() const { return message_; }

private:
  int code_ = 0;
  const char *message_ = "";
};

}  // namespace c8
