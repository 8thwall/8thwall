// Copyright 2023, The Khronos Group Inc.
//
// SPDX-License-Identifier: Apache-2.0

// OpenXR Tutorial for Khronos Group

#pragma once

#include "c8/c8-log.h"
#include "c8/xrapi/openxr/openxr.h"

// C/C++ Headers
#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <vector>

// Debugbreak
#if defined(_MSC_VER)
#define DEBUG_BREAK __debugbreak()
#else
#include <signal.h>
#define DEBUG_BREAK raise(SIGTRAP)
#endif

using namespace c8;

template <typename T>
inline bool BitwiseCheck(const T &value, const T &checkValue) {
  return ((value & checkValue) == checkValue);
}

inline void OpenXRDebugBreak() {
  DEBUG_BREAK;
  raise(SIGTRAP);
}

inline const char *GetXRErrorString(XrInstance xrInstance, XrResult result) {
  static char string[XR_MAX_RESULT_STRING_SIZE];
  xrResultToString(xrInstance, result, string);
  return string;
}

#define OPENXR_CHECK(x, y)                                            \
  {                                                                   \
    XrResult result = (x);                                            \
    if (!XR_SUCCEEDED(result)) {                                      \
      C8Log(                                                          \
        "ERROR: OPENXR_FAIL: %d (%s) %s\n",                           \
        int(result),                                                  \
        (m_xrInstance ? GetXRErrorString(m_xrInstance, result) : ""), \
        y);                                                           \
      OpenXRDebugBreak();                                             \
    }                                                                 \
  }

template <typename T>
inline void loadXrProcOrDie(XrInstance m_xrInstance, const char *methodName, T *handle) {
  XrResult result = xrGetInstanceProcAddr(m_xrInstance, methodName, (PFN_xrVoidFunction *)(handle));

  OPENXR_CHECK(result, methodName);
}
