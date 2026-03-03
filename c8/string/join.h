// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// This file provides the method strJoin, which allows you to Join strings in an an array, vector,
// initializer_list, range, or container into a single string with separator and optional formatter.

#pragma once

#include <functional>
#include <initializer_list>
#include <iterator>
#include <type_traits>

#include "c8/string-view.h"
#include "c8/string.h"

namespace c8 {

template <class T>
using StrJoinFmtFunc = std::function<String(T)>;

template <class T>
struct StrJoinFmtDefault {
  // Default for typical string-like types.
  template <typename T_ = T, typename = std::enable_if_t<!std::is_arithmetic_v<T_>>>
  T &&operator()(T &&x) const noexcept {
    return std::forward<T &&>(x);
  }

  // Default for arithmetic types.
  template <typename T_ = T, typename = std::enable_if_t<std::is_arithmetic_v<T_>>>
  String operator()(T x) const noexcept {
    String s = std::to_string(x);
    s.erase(s.find_last_not_of('0') + 1, std::string::npos);
    s.erase(s.find_last_not_of('.') + 1, std::string::npos);
    return s;
  }
};

namespace internal {
template <class InputIt, class FormatFunc>
String strJoin(InputIt start, InputIt end, StringView separator, FormatFunc fmt);
}

// Join Strings from a class containing range iterators, such as a Vector.
//
// Example Usage:
//   Vector<String> items = { "one", "two", "three" };
//   String result = strJoin(items, ", ");
// OR
//   std::deque<int> items = { 1, 2, 3 };
//   String result = strJoin(items, ", ", [](int x) { return std::to_string(x); });
//
template <class InputRange, class FormatFunc = StrJoinFmtFunc<typename InputRange::value_type>>
String strJoin(
  const InputRange &range,
  StringView separator,
  FormatFunc fmt = StrJoinFmtDefault<typename InputRange::value_type>()) {
  return internal::strJoin<typename InputRange::const_iterator, FormatFunc>(
    range.cbegin(), range.cend(), separator, fmt);
}

// Join Strings using STL-compliant iterators.
//
// Example Usage:
//   constexpr const char* items[] = { "one", "two", "three" };
//   String result = strJoin(items, std::end(items), ", ");
// OR
//   std::deque<int> items = { 1, 2, 3 };
//   String result = strJoin(
//     items.cbegin(), items.cend(), ", ", [](int x) { return std::to_string(x); });
//
template <
  class InputIt,
  class FormatFunc = StrJoinFmtFunc<typename std::iterator_traits<InputIt>::value_type>>
String strJoin(
  InputIt start,
  InputIt end,
  StringView separator,
  FormatFunc fmt = StrJoinFmtDefault<typename std::iterator_traits<InputIt>::value_type>()) {
  return internal::strJoin<InputIt, FormatFunc>(start, end, separator, fmt);
}

// Join Strings from an initializer_list.
//
// Example Usage:
//   String result = strJoin({ "one", "two", "three" }, ", ");
// OR
//   String result = strJoin({ 1, 2, 3 }, ", ", [](int x) { return std::to_string(x); });
//
template <class T, class FormatFunc = StrJoinFmtFunc<T>>
String strJoin(
  std::initializer_list<T> input, StringView separator, FormatFunc fmt = StrJoinFmtDefault<T>()) {
  return internal::strJoin<typename decltype(input)::const_iterator, FormatFunc>(
    input.begin(), input.end(), separator, fmt);
}

// Internal implementation.
namespace internal {

template <class InputIterator, class FormatFunc>
String strJoin(InputIterator start, InputIterator end, StringView separator, FormatFunc fmt) {
  String output;
  StringView s = "";
  using ValueType = std::remove_cv_t<typename std::iterator_traits<InputIterator>::value_type>;
  std::for_each(start, end, [&output, &s, fmt, separator](const ValueType &element) {
    output += s;
    output += fmt(element);
    s = separator;
  });
  return output;
}

}  // namespace internal

}  // namespace c8
