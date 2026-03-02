// Copyright (c) 2016 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Easy string concatenation with StrCat using C++17 parameter pack folding.

#pragma once

#include <sstream>
#include "c8/string.h"

namespace c8 {

template <typename ...Stringish>
String strCat(Stringish&&... args) noexcept {
  std::stringstream out;
  (out << ... << args);
  return out.str();
}

}  // namespace c8
