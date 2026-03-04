#include "c8/xrapi/openxr/display-refresh-rate-fb.h"

#include "c8/xrapi/openxr/helper.h"

namespace c8 {

DisplayRefreshRateFB::DisplayRefreshRateFB(XrInstance instance) : m_xrInstance(instance) {
  loadDisplayRefreshRateFBFunctions();
}

Vector<const char *> DisplayRefreshRateFB::requiredExtensionNames() {
  return {XR_FB_DISPLAY_REFRESH_RATE_EXTENSION_NAME};
}

void DisplayRefreshRateFB::loadDisplayRefreshRateFBFunctions() {
  loadXrProcOrDie(
    m_xrInstance, "xrEnumerateDisplayRefreshRatesFB", &xrEnumerateDisplayRefreshRatesFB_);
  loadXrProcOrDie(m_xrInstance, "xrGetDisplayRefreshRateFB", &xrGetDisplayRefreshRateFB_);
  loadXrProcOrDie(m_xrInstance, "xrRequestDisplayRefreshRateFB", &xrRequestDisplayRefreshRateFB_);
}

void DisplayRefreshRateFB::enumerateDisplayRefreshRatesFB(
  XrSession session,
  uint32_t displayRefreshRateCount,
  uint32_t *displayRefreshRateCountOutput,
  float *displayRefreshRates) {
  XrResult result = xrEnumerateDisplayRefreshRatesFB_(
    session, displayRefreshRateCount, displayRefreshRateCountOutput, displayRefreshRates);

  OPENXR_CHECK(result, "Call to xrEnumerateDisplayRefreshRatesFB failed")
}

void DisplayRefreshRateFB::getDisplayRefreshRateFB(XrSession session, float *displayRefreshRate){
  OPENXR_CHECK(
    xrGetDisplayRefreshRateFB_(session, displayRefreshRate),
    "Call to xrGetDisplayRefreshRateFB failed")}

XrResult
  DisplayRefreshRateFB::requestDisplayRefreshRateFB(XrSession session, float displayRefreshRate) {

  return xrRequestDisplayRefreshRateFB_(session, displayRefreshRate);
}

}  // namespace c8
