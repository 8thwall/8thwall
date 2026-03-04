#pragma once

#include "c8/vector.h"
#include "c8/xrapi/openxr/openxr.h"

namespace c8 {

class DisplayRefreshRateFB {
public:
  DisplayRefreshRateFB(XrInstance instance);

  static Vector<const char *> requiredExtensionNames();

  void enumerateDisplayRefreshRatesFB(
    XrSession session,
    uint32_t displayRefreshRateCount,
    uint32_t *displayRefreshRateCountOutput,
    float *displayRefreshRates);

  void getDisplayRefreshRateFB(XrSession session, float *displayRefreshRate);

  XrResult requestDisplayRefreshRateFB(XrSession session, float displayRefreshRate);

private:
  void loadDisplayRefreshRateFBFunctions();

  XrInstance m_xrInstance;

  PFN_xrEnumerateDisplayRefreshRatesFB xrEnumerateDisplayRefreshRatesFB_ = nullptr;
  PFN_xrGetDisplayRefreshRateFB xrGetDisplayRefreshRateFB_ = nullptr;
  PFN_xrRequestDisplayRefreshRateFB xrRequestDisplayRefreshRateFB_ = nullptr;
};

}  // namespace c8
