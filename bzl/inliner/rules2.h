// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// New include header for inliner rules. Only use this in .cc files, not headers.
//
// The end_cc macro must exist and it is a compile-time assert if the hash value within it doesn't
// match the rules. If set to 0, the assert will be skipped.
//
// Example Usage:
//
//  cc_library {
//    hdrs = {
//      "my-library.h",
//    };
//    srcs = {
//      "my-library.cc",
//    };
//    deps = {
//      "//c8:string",
//      "//c8:vector",
//    };
//    testonly = 1;
//    visibility = {
//      "//visibility:public"
//    };
//  }
//  end_cc(0);

#include <stdint.h>
#include <string.h>

#include <initializer_list>

#include "third_party/murmurhash/constexpr_murmur3.h"

namespace {

class InlinerConstString {
public:
  template <std::size_t N>
  constexpr InlinerConstString(const char (&input)[N]) : str_(input), size_(N - 1) {}

  constexpr InlinerConstString(const char *input, int size) : str_(input), size_(size) {}

  constexpr const char *data() const noexcept { return str_; }

  constexpr std::size_t size() const noexcept { return size_; }

  template <std::size_t N>
  constexpr bool operator==(const char (&input)[N]) const noexcept {
    if (N - 1 != size_) {
      return false;
    }
    for (int i = 0; i < size_; ++i) {
      if (str_[i] != input[i]) {
        return false;
      }
    }
    return true;
  }

private:
  const char *str_;
  const std::size_t size_;
};

template <std::size_t N>
static constexpr uint32_t inlinerStringHash(const char (&str)[N], uint32_t seed = 0) {
  return ce_mm3::mm3_x86_32(str, seed);
}

template <std::size_t N>
static constexpr uint32_t isHeaderFile(const char (&str)[N]) {
  return N > 2 && str[N - 3] == '.' && str[N - 2] == 'h';
}

static constexpr uint32_t inlinerStringHash(const InlinerConstString &str, uint32_t seed = 0) {
  return ce_mm3::mm3_x86_32(str.data(), str.size(), seed);
}

struct RuleHasher {
public:
  uint32_t hash = 0;

  constexpr RuleHasher() = default;

  constexpr uint32_t operator=(const InlinerConstString &text) {
    hash = inlinerStringHash(text, hash);
    return hash;
  }

  constexpr uint32_t operator=(std::initializer_list<InlinerConstString> stringArray) {
    for (auto str : stringArray) {
      if (str == "//bzl/inliner:rules" || str == "//bzl/inliner:rules2") {
        // Skip inliner dependencies when computing the hash.
        continue;
      }
      hash = inlinerStringHash(str, hash);
    }
    return hash;
  }

  constexpr uint32_t operator=(int32_t value) {
    hash ^= value + 0x9e3779b9 + (hash << 6) + (value >> 2);
    return hash;
  }
};

// Get the default rule name from the built-in __FILE__ macro, from basename minus extension.
template <std::size_t N>
constexpr InlinerConstString inlinerDefaultName(const char (&str)[N]) {
  if (N <= 2) {
    return InlinerConstString(str);
  }

  const char *start = str + N - 2;
  const char *last = str + N - 1;

  for (; start > str + 1; --start) {
#ifdef _WIN32
    if (*start == '\\') {
#else
    if (*start == '/') {
#endif
      start++;
      break;
    }
  }

  for (; last > start; --last) {
    if (*last == '.') {
      break;
    }
  }

  const char *end = (last == start) ? str + N - 1 : last;

  return InlinerConstString(start, end - start);
}

}  // namespace

#define _INL_UNUSED __attribute__((unused))

#define cc_library                            \
  static constexpr uint32_t inlinerRule() {   \
    RuleHasher rule;                          \
    auto &name = rule;                        \
    name = inlinerDefaultName(__FILE__);      \
    auto &deps _INL_UNUSED = rule;            \
    auto &srcs _INL_UNUSED = rule;            \
    auto &data _INL_UNUSED = rule;            \
    auto &hdrs _INL_UNUSED = rule;            \
    auto &alwayslink _INL_UNUSED = rule;      \
    auto &compatible_with _INL_UNUSED = rule; \
    auto &copts _INL_UNUSED = rule;           \
    auto &defines _INL_UNUSED = rule;         \
    auto &deprecation _INL_UNUSED = rule;     \
    auto &distribs _INL_UNUSED = rule;        \
    auto &features _INL_UNUSED = rule;        \
    auto &includes _INL_UNUSED = rule;        \
    auto &licenses _INL_UNUSED = rule;        \
    auto &linkopts _INL_UNUSED = rule;        \
    auto &linkstatic _INL_UNUSED = rule;      \
    auto &nocopts _INL_UNUSED = rule;         \
    auto &restricted_to _INL_UNUSED = rule;   \
    auto &tags _INL_UNUSED = rule;            \
    auto &testonly _INL_UNUSED = rule;        \
    auto &textual_hdrs _INL_UNUSED = rule;    \
    auto &visibility _INL_UNUSED = rule;      \
    auto &target_compatible_with _INL_UNUSED = rule;

#define cc_binary                             \
  static constexpr uint32_t inlinerRule() {   \
    RuleHasher rule;                          \
    auto &name = rule;                        \
    name = inlinerDefaultName(__FILE__);      \
    auto &deps _INL_UNUSED = rule;            \
    auto &srcs _INL_UNUSED = rule;            \
    auto &data _INL_UNUSED = rule;            \
    auto &args _INL_UNUSED = rule;            \
    auto &compatible_with _INL_UNUSED = rule; \
    auto &copts _INL_UNUSED = rule;           \
    auto &defines _INL_UNUSED = rule;         \
    auto &deprecation _INL_UNUSED = rule;     \
    auto &distribs _INL_UNUSED = rule;        \
    auto &features _INL_UNUSED = rule;        \
    auto &includes _INL_UNUSED = rule;        \
    auto &licenses _INL_UNUSED = rule;        \
    auto &linkopts _INL_UNUSED = rule;        \
    auto &linkshared _INL_UNUSED = rule;      \
    auto &linkstatic _INL_UNUSED = rule;      \
    auto &malloc _INL_UNUSED = rule;          \
    auto &nocopts _INL_UNUSED = rule;         \
    auto &output_licenses _INL_UNUSED = rule; \
    auto &restricted_to _INL_UNUSED = rule;   \
    auto &stamp _INL_UNUSED = rule;           \
    auto &tags _INL_UNUSED = rule;            \
    auto &testonly _INL_UNUSED = rule;        \
    auto &visibility _INL_UNUSED = rule;      \
    auto &target_compatible_with _INL_UNUSED = rule;

#define cc_test                               \
  static constexpr uint32_t inlinerRule() {   \
    RuleHasher rule;                          \
    auto &name = rule;                        \
    name = inlinerDefaultName(__FILE__);      \
    auto &deps _INL_UNUSED = rule;            \
    auto &srcs _INL_UNUSED = rule;            \
    auto &data _INL_UNUSED = rule;            \
    auto &args _INL_UNUSED = rule;            \
    auto &compatible_with _INL_UNUSED = rule; \
    auto &copts _INL_UNUSED = rule;           \
    auto &defines _INL_UNUSED = rule;         \
    auto &deprecation _INL_UNUSED = rule;     \
    auto &distribs _INL_UNUSED = rule;        \
    auto &features _INL_UNUSED = rule;        \
    auto &flaky _INL_UNUSED = rule;           \
    auto &includes _INL_UNUSED = rule;        \
    auto &licenses _INL_UNUSED = rule;        \
    auto &linkopts _INL_UNUSED = rule;        \
    auto &linkstatic _INL_UNUSED = rule;      \
    auto &local _INL_UNUSED = rule;           \
    auto &malloc _INL_UNUSED = rule;          \
    auto &nocopts _INL_UNUSED = rule;         \
    auto &restricted_to _INL_UNUSED = rule;   \
    auto &shard_count _INL_UNUSED = rule;     \
    auto &size _INL_UNUSED = rule;            \
    auto &stamp _INL_UNUSED = rule;           \
    auto &tags _INL_UNUSED = rule;            \
    auto &testonly _INL_UNUSED = rule;        \
    auto &timeout _INL_UNUSED = rule;         \
    auto &visibility _INL_UNUSED = rule;      \
    auto &target_compatible_with _INL_UNUSED = rule;

#define cc_end(expected)                                                            \
  ;                                                                                 \
  return rule.hash;                                                                 \
  }                                                                                 \
  static constexpr void validateInlinerRule() _INL_UNUSED;                          \
  static constexpr void validateInlinerRule() {                                     \
    static_assert(!isHeaderFile(__FILE__), "Inliner rules cannnot be in .h files"); \
    constexpr bool inlinerHashCheck = (inlinerRule() == expected) || expected == 0; \
    static_assert(inlinerHashCheck, "Invalid hash, re-run inliner");                \
  }

