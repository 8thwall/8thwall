// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifndef C8_RELEASE_CONFIG_ENABLE_CONSOLE_LOGGING
#define C8_RELEASE_CONFIG_ENABLE_CONSOLE_LOGGING true
#endif  // C8_RELEASE_CONFIG_ENABLE_CONSOLE_LOGGING

#ifndef C8_RELEASE_CONFIG_ENABLE_REQUEST_DEBUG_DATA
#define C8_RELEASE_CONFIG_ENABLE_REQUEST_DEBUG_DATA false
#endif

#ifdef __cplusplus

#include "bzl/stamp/release-build-id.h"
#include "c8/string.h"

namespace c8 {

inline String releaseConfigBuildId() { return ReleaseBuildId(); }

inline bool releaseConfigIsConsoleLoggingEnabled() {
  return C8_RELEASE_CONFIG_ENABLE_CONSOLE_LOGGING;
}

inline bool releaseConfigIsRequestDebugDataEnabled() {
  return C8_RELEASE_CONFIG_ENABLE_REQUEST_DEBUG_DATA;
}

}  // namespace c8

#else  // __cplusplus

static inline int c8_releaseConfigIsConsoleLoggingEnabled() {
  return C8_RELEASE_CONFIG_ENABLE_CONSOLE_LOGGING;
}  // namespace c8

#endif  // __cplusplus
