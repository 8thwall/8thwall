// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "c8/string.h"

namespace c8 {

struct XrwebEnvironment {
  int displayWidth;
  int displayHeight;

  bool isWebGl2;
};

XrwebEnvironment &xrwebEnvironment();

void pushTrace(const String &tag);
void popTrace();

}  // namespace c8
