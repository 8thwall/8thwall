#pragma once

#include "c8/vector.h"
#include "c8/xrapi/openxr/openxr.h"

namespace c8 {

class MetaFoveation {
public:
  MetaFoveation(XrInstance instance);

  static Vector<const char *> requiredExtensionNames();

  void createFoveationProfile(
    XrSession session,
    XrFoveationProfileCreateInfoFB *foveationProfileCreateInfo,
    XrFoveationProfileFB *foveationProfile);
  void destroyFoveationProfile(XrFoveationProfileFB &foveationProfile);
  void updateSwapchain(XrSwapchain swapchain, XrSwapchainStateBaseHeaderFB *swapchainState);
  void getSwapchainState(XrSwapchain swapchain, XrSwapchainStateBaseHeaderFB *swapchainState);

private:
  void loadMetaFoveationFunctions();

  XrInstance m_xrInstance;

  PFN_xrCreateFoveationProfileFB xrCreateFoveationProfileFB_ = nullptr;
  PFN_xrDestroyFoveationProfileFB xrDestroyFoveationProfileFB_ = nullptr;
  PFN_xrUpdateSwapchainFB xrUpdateSwapchainFB_ = nullptr;
  PFN_xrGetSwapchainStateFB xrGetSwapchainStateFB_ = nullptr;

  // Future use:
  // PFN_xrGetFoveationEyeTrackedStateMETA xrGetFoveationEyeTrackedStateMETA_ = nullptr;
};

}  // namespace c8