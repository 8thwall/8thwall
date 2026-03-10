// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "c8/c8-log.h"

namespace c8 {

void C8LogLines(const String &lines) {
  std::istringstream stream(lines);
  for (std::string line; std::getline(stream, line);) {
    C8Log("%s", line.c_str());
  }
}

}  // namespace c8
