// Copyright (c) 2016 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// StringView alias for std::string_view, allowing 8th Wall
// to use more consistent class naming.

#pragma once

#include <string_view>

namespace c8 {

using StringView = std::string_view;

}  // namespace c8
