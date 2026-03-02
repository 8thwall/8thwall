// Copyright (c) 2022 Niantic Inc.
// Original Author: Riyaan Bakhda (riyaanbakhda@nianticlabs.com)

#pragma once
#include <concepts>
#include <type_traits>

#include "c8/string-view.h"
#include "c8/string.h"

namespace c8 {

template <typename T>
concept StringViewConvertible =
  std::is_convertible_v<T, StringView> || std::is_same_v<T, const char *>;

// Test that `str` starts with with `start`, e.g. `FooBar ` starts with `Foo`
template <StringViewConvertible Str, StringViewConvertible Prefix>
constexpr bool startsWith(const Str &str, const Prefix &start) {
  if constexpr (std::is_same_v<Str, StringView> && std::is_same_v<Prefix, StringView>) {
    return str.starts_with(start);
  } else {
    return StringView(str).starts_with(StringView(start));
  }
}

// Test that `str` ends with `end`, e.g. `FooBar ` ends with `Bar `
template <StringViewConvertible Str, StringViewConvertible Suffix>
constexpr bool endsWith(const Str &str, const Suffix &end) {
  if constexpr (std::is_same_v<Str, StringView> && std::is_same_v<Suffix, StringView>) {
    return str.ends_with(end);
  } else {
    return StringView(str).ends_with(StringView(end));
  }
}
}  // namespace c8
