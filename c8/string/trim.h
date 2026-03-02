// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Alvin Portillo (alvin@8thwall.com)
//
// This file provides a utility for trimming leading and trailing characters in a string.

#pragma once

#include "c8/string-view.h"
#include "c8/string.h"

namespace c8 {

String strTrim(StringView str);

}  // namespace c8
