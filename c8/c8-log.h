// Copyright (c) 2016 8th Wall, Inc.
//
// Cross platform utilities for writing to stdout.

#pragma once

#ifdef ANDROID
#include <android/log.h>
#endif

#include "c8/release-config.h"

#ifdef __cplusplus

#include <sstream>

#include "c8/string.h"

namespace c8 {

#ifdef ANDROID
static constexpr char C8_LOG_TAG[] = "8thWall";
template <class... Args>
static void C8Log(const char *fmt, Args &&...args) {
  if (!releaseConfigIsConsoleLoggingEnabled()) {
    return;
  }
  __android_log_print(ANDROID_LOG_INFO, C8_LOG_TAG, fmt, std::forward<Args>(args)...);
}
#else
template <class... Args>
static void C8Log(const char *fmt, Args &&...args) {
  if (!releaseConfigIsConsoleLoggingEnabled()) {
    return;
  }
  printf(fmt, std::forward<Args>(args)...);
  printf("\n");
  fflush(stdout);
}
#endif  // ANDROID

template <class... Args>
static void C8Log(const char *fmt) {
  C8Log("%s", fmt);
}

// Invokes C8Log on each line of input. This does not support formatting.
void C8LogLines(const String &lines);

}  // namespace c8

#else  // __cplusplus
// Fallback for pure c.

#ifdef ANDROID
#include <android/log.h>
#include <stdarg.h>
static const char *C8_LOG_TAG = "8thWall";

static void C8Log(const char *fmt, ...) {
  if (!c8_releaseConfigIsConsoleLoggingEnabled()) {
    return;
  }
  va_list varargs;
  va_start(varargs, fmt);
  __android_log_vprint(ANDROID_LOG_INFO, C8_LOG_TAG, fmt, varargs);
  va_end(varargs);
}

#else  // ANDROID
// Non-Android
#include <stdarg.h>
#include <stdio.h>

static void C8Log(const char *fmt, ...) {
  if (!c8_releaseConfigIsConsoleLoggingEnabled()) {
    return;
  }
  va_list varargs;
  va_start(varargs, fmt);
  vprintf(fmt, varargs);
  printf("\n");
  va_end(varargs);
}

#endif  // ANDROID

#endif  //  __cpluslus
