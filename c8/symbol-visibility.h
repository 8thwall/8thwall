// Copyright (c) 2019 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// General purpose macros and type aliases.

#pragma once

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define C8_EMSCRIPTEN_KEEPALIVE EMSCRIPTEN_KEEPALIVE
#else
#define C8_EMSCRIPTEN_KEEPALIVE
#endif

#if defined(_WIN32) || defined(__CYGWIN__)
#define C8_PUBLIC __declspec(dllexport)
#define C8_LOCAL
#else
#if __GNUC__ >= 4
#define C8_PUBLIC C8_EMSCRIPTEN_KEEPALIVE __attribute__((visibility("default")))
#define C8_LOCAL __attribute__((visibility("hidden")))
#else
#define C8_PUBLIC C8_EMSCRIPTEN_KEEPALIVE
#define C8_LOCAL
#endif
#endif
