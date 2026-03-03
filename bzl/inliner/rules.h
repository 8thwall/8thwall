#pragma once

#include <initializer_list>

#define _C8_BUILD_CREATE_NAME_INNER(rule, token) BUILD_##rule##_##token
#define _C8_BUILD_CREATE_NAME(rule, token) \
  _C8_BUILD_CREATE_NAME_INNER(rule, token)

#define _C8_BUILD_CC_LIBRARY                                                  \
  static void _C8_BUILD_CREATE_NAME(cc_library, __LINE__)(                    \
    const char* name __attribute__((unused)),                                 \
    std::initializer_list<const char*> deps __attribute__((unused)),          \
    std::initializer_list<const char*> srcs __attribute__((unused)),          \
    std::initializer_list<const char*> data __attribute__((unused)),          \
    std::initializer_list<const char*> hdrs __attribute__((unused)),          \
    int alwayslink __attribute__((unused)),                                   \
    std::initializer_list<const char*> compatible_with                        \
    __attribute__((unused)),                                                  \
    std::initializer_list<const char*> copts __attribute__((unused)),         \
    std::initializer_list<const char*> defines __attribute__((unused)),       \
    const char* deprecation __attribute__((unused)),                          \
    std::initializer_list<const char*> distribs __attribute__((unused)),      \
    std::initializer_list<const char*> features __attribute__((unused)),      \
    std::initializer_list<const char*> includes __attribute__((unused)),      \
    std::initializer_list<const char*> licenses __attribute__((unused)),      \
    std::initializer_list<const char*> linkopts __attribute__((unused)),      \
    int linkstatic __attribute__((unused)),                                   \
    const char* nocopts __attribute__((unused)),                              \
    std::initializer_list<const char*> restricted_to __attribute__((unused)), \
    std::initializer_list<const char*> tags __attribute__((unused)),          \
    int testonly __attribute__((unused)),                                     \
    std::initializer_list<const char*> textual_hdrs __attribute__((unused)),  \
    std::initializer_list<const char*> visibility __attribute__((unused)),    \
    std::initializer_list<const char*> target_compatible_with                 \
    __attribute__((unused)))

#define cc_library                              \
  _C8_BUILD_CC_LIBRARY __attribute__((unused)); \
  _C8_BUILD_CC_LIBRARY

#define _C8_BUILD_CC_BINARY                               \
  static void _C8_BUILD_CREATE_NAME(cc_binary, __LINE__)( \
    const char* name,                                     \
    std::initializer_list<const char*> deps,              \
    std::initializer_list<const char*> srcs,              \
    std::initializer_list<const char*> data,              \
    std::initializer_list<const char*> args,              \
    std::initializer_list<const char*> compatible_with,   \
    std::initializer_list<const char*> copts,             \
    std::initializer_list<const char*> defines,           \
    const char* deprecation,                              \
    std::initializer_list<const char*> distribs,          \
    std::initializer_list<const char*> features,          \
    std::initializer_list<const char*> includes,          \
    std::initializer_list<const char*> licenses,          \
    std::initializer_list<const char*> linkopts,          \
    int linkshared,                                       \
    int linkstatic,                                       \
    const char* malloc,                                   \
    const char* nocopts,                                  \
    std::initializer_list<const char*> output_licenses,   \
    std::initializer_list<const char*> restricted_to,     \
    int stamp,                                            \
    std::initializer_list<const char*> tags,              \
    int testonly,                                         \
    std::initializer_list<const char*> visibility,       \
    std::initializer_list<const char*> target_compatible_with)  \


#define cc_binary                              \
  _C8_BUILD_CC_BINARY __attribute__((unused)); \
  _C8_BUILD_CC_BINARY

#define _C8_BUILD_CC_TEST                               \
  static void _C8_BUILD_CREATE_NAME(cc_test, __LINE__)( \
    const char* name,                                   \
    std::initializer_list<const char*> deps,            \
    std::initializer_list<const char*> srcs,            \
    std::initializer_list<const char*> data,            \
    std::initializer_list<const char*> args,            \
    std::initializer_list<const char*> compatible_with, \
    std::initializer_list<const char*> copts,           \
    std::initializer_list<const char*> defines,         \
    const char* deprecation,                            \
    std::initializer_list<const char*> distribs,        \
    std::initializer_list<const char*> features,        \
    int flaky,                                          \
    std::initializer_list<const char*> includes,        \
    std::initializer_list<const char*> licenses,        \
    std::initializer_list<const char*> linkopts,        \
    int linkstatic,                                     \
    int local,                                          \
    const char* malloc,                                 \
    const char* nocopts,                                \
    std::initializer_list<const char*> restricted_to,   \
    unsigned int shard_count,                           \
    const char* size,                                   \
    int stamp,                                          \
    std::initializer_list<const char*> tags,            \
    int testonly,                                       \
    const char* timeout,                                \
    std::initializer_list<const char*> visibility,      \
    std::initializer_list<const char*> target_compatible_with) \

#define cc_test                              \
  _C8_BUILD_CC_TEST __attribute__((unused)); \
  _C8_BUILD_CC_TEST
