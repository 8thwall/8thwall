// Copyright (c) 2019 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)
//
// format function for strings.

#pragma once

#include <cstdio>
#include <memory>
#include <sstream>
#include <string>

#include "c8/string.h"
#include "c8/vector.h"

namespace c8 {
namespace {
// SFINAE test
template <typename T>
class HasToString {
private:
  typedef char YesType[1];
  typedef char NoType[2];

  template <typename C>
  static YesType &test(decltype(&C::toString));
  template <typename C>
  static NoType &test(...);

public:
  enum { value = sizeof(test<T>(0)) == sizeof(YesType) };
};

}  // namespace

template <typename... Args>
String format(const String &in, Args... args) {
  size_t formattedSize = snprintf(nullptr, 0, in.c_str(), args...) + 1;
  std::unique_ptr<char[]> out(new char[formattedSize]);
  std::snprintf(out.get(), formattedSize, in.c_str(), args...);
  return String(out.get(), out.get() + formattedSize - 1);
}

String toUpperCase(String s);
String boolToString(bool b);

template <typename T>
typename std::enable_if<HasToString<T>::value, String>::type static toString(const Vector<T> &vec) {
  auto sep = "";
  std::stringstream ss;
  ss << "[";
  for (const auto &e : vec) {
    ss << sep;
    ss << e.toString();
    sep = ", ";
  }
  ss << "]";
  return ss.str();
}

template <typename T>
typename std::enable_if<(!HasToString<T>::value), String>::type static toString(
  const Vector<T> &vec) {
  auto sep = "";
  std::stringstream ss;
  ss << "[";
  for (const auto &e : vec) {
    ss << sep;
    ss << e;
    sep = ", ";
  }
  ss << "]";
  return ss.str();
}

template <typename T>
String toString(const T &e) {
  std::stringstream ss;
  ss << e;
  return ss.str();
}

}  // namespace c8
