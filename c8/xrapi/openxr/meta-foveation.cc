#include "c8/xrapi/openxr/meta-foveation.h"

#include "c8/xrapi/openxr/helper.h"

namespace c8 {

MetaFoveation::MetaFoveation(XrInstance instance) : m_xrInstance(instance) {
  loadMetaFoveationFunctions();
}

Vector<const char *> MetaFoveation::requiredExtensionNames() {
  return {
  // TODO(christoph): Make this work again
#ifdef XR_USE_GRAPHICS_API_VULKAN
    XR_FB_SWAPCHAIN_UPDATE_STATE_EXTENSION_NAME,
    XR_FB_SWAPCHAIN_UPDATE_STATE_VULKAN_EXTENSION_NAME,
    XR_FB_FOVEATION_EXTENSION_NAME,
    XR_FB_FOVEATION_VULKAN_EXTENSION_NAME,
    XR_FB_FOVEATION_CONFIGURATION_EXTENSION_NAME,
#endif
  };
}

void MetaFoveation::loadMetaFoveationFunctions() {
  loadXrProcOrDie(m_xrInstance, "xrCreateFoveationProfileFB", &xrCreateFoveationProfileFB_);
  loadXrProcOrDie(m_xrInstance, "xrDestroyFoveationProfileFB", &xrDestroyFoveationProfileFB_);
  loadXrProcOrDie(m_xrInstance, "xrUpdateSwapchainFB", &xrUpdateSwapchainFB_);
  loadXrProcOrDie(m_xrInstance, "xrGetSwapchainStateFB", &xrGetSwapchainStateFB_);
}

void MetaFoveation::createFoveationProfile(
  XrSession session,
  XrFoveationProfileCreateInfoFB *foveationProfileCreateInfo,
  XrFoveationProfileFB *foveationProfile) {
  XrResult result =
    xrCreateFoveationProfileFB_(session, foveationProfileCreateInfo, foveationProfile);

  OPENXR_CHECK(result, "xrCreateFoveationProfileFB")
}

void MetaFoveation::destroyFoveationProfile(XrFoveationProfileFB &foveationProfile) {
  OPENXR_CHECK(xrDestroyFoveationProfileFB_(foveationProfile), "xrDestroyFoveationProfileFB")
}

void MetaFoveation::updateSwapchain(
  XrSwapchain swapchain, XrSwapchainStateBaseHeaderFB *swapchainState) {
  OPENXR_CHECK(xrUpdateSwapchainFB_(swapchain, swapchainState), "xrUpdateSwapchainFB")
}

void MetaFoveation::getSwapchainState(
  XrSwapchain swapchain, XrSwapchainStateBaseHeaderFB *swapchainState) {
  OPENXR_CHECK(xrGetSwapchainStateFB_(swapchain, swapchainState), "xrGetSwapchainStateFB")
}

}  // namespace c8
