// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Paris Morgan (paris@8thwall.com)
//
// Splits a string on a delimiter.

#pragma once

#include <sstream>

#include "c8/string.h"
#include "c8/vector.h"

namespace c8 {

// Splits a string on a delimiter. Returns [""] if you pass an empty string. Returns [src] if you
// pass an empty del.
// TODO(paris): Move other internal implementations to this. They currently differ in several ways:
// - They do split("", ',') -> [], while this returns [""]
// - They do split("a a ", ' ') -> ["a", "a"], while this returns ["a", "a", ""]
// - https://github.com/8thwall/code8/blob/master/reality/quality/benchmark/benchmark-xr-6dof.cc#L81
// - https://github.com/8thwall/code8/blob/master/reality/quality/datasets/chesspose/chesspose-labeler.cc#L73
// - https://github.com/8thwall/code8/blob/master/reality/quality/datasets/chesspose/collate.cc#L90
Vector<String> split(const String &src, const String &del);

}  // namespace c8
