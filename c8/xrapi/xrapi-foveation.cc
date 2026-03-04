#include "c8/xrapi/xrapi-foveation.h"

#include "c8/c8-log.h"

namespace c8 {

XrApiFoveation::XrApiFoveation(XrInstance instance, XrSession session) {
  instance_ = instance;
  session_ = session;

  // For now we only support the Meta extension, but we could abstract this to support multiple
  metaFoveation_ = std::make_unique<MetaFoveation>(instance_);
}

Vector<const char *> XrApiFoveation::requiredExtensionNames() {
  // TODO: check which extensions are needed for hardware / graphics api
  return MetaFoveation::requiredExtensionNames();
}

/*
 * Check if the active extensions contain all the required extensions for foveation
 */
void XrApiFoveation::setFoveationActiveStatus(Vector<const char *> &activeExtensions) {
  const auto requiredExtensions = requiredExtensionNames();
  int numberOfMatches = 0;
  foveationActive_ = false;

  for (const auto &requiredExtension : requiredExtensions) {
    for (const auto &activeExtension : activeExtensions) {
      if (std::strcmp(requiredExtension, activeExtension) == 0) {
        numberOfMatches++;
      }
    }
  }

  if (numberOfMatches == requiredExtensions.size()) {
    foveationActive_ = true;
  }
}

bool XrApiFoveation::isFoveationActive() { return foveationActive_; }

void XrApiFoveation::setFoveationLevel(XrFoveationLevelFB level) { foveationLevel_ = level; }

void *XrApiFoveation::getFoveationImage() {
#ifdef XR_USE_GRAPHICS_API_VULKAN
  static XrSwapchainImageFoveationVulkanFB vulkanFoveationImage = {
    .type = XR_TYPE_SWAPCHAIN_IMAGE_FOVEATION_VULKAN_FB,
    .next = XR_NULL_HANDLE,
  };

  return &vulkanFoveationImage;

#else
  return nullptr;
#endif
}

XrFoveationProfileFB XrApiFoveation::createFoveationProfileHandle() {
  XrFoveationProfileFB foveationProfile;

  XrFoveationDynamicFB dynamic = XR_FOVEATION_DYNAMIC_DISABLED_FB;

  XrFoveationLevelProfileCreateInfoFB foveationLevelProfileCreateInfo{
    .type = XR_TYPE_FOVEATION_LEVEL_PROFILE_CREATE_INFO_FB,
    .next = nullptr,
    .level = foveationLevel_,
    .verticalOffset = 0.0f,
    .dynamic = dynamic,
  };

  XrFoveationProfileCreateInfoFB foveationProfileCreateInfo{
    .type = XR_TYPE_FOVEATION_PROFILE_CREATE_INFO_FB, .next = &foveationLevelProfileCreateInfo};

  metaFoveation_->createFoveationProfile(session_, &foveationProfileCreateInfo, &foveationProfile);

  return foveationProfile;
}

XrSwapchainCreateInfoFoveationFB XrApiFoveation::getSwapchainCreateInfoFoveation() {
  XrSwapchainCreateInfoFoveationFB foveationSwapchainCreateInfo{
    .type = XR_TYPE_SWAPCHAIN_CREATE_INFO_FOVEATION_FB,
    .next = nullptr,
    .flags = XR_SWAPCHAIN_CREATE_FOVEATION_FRAGMENT_DENSITY_MAP_BIT_FB,
  };

  return foveationSwapchainCreateInfo;
}

void XrApiFoveation::tryUpdateSwapchainFoveationState(XrSwapchain swapchain) {
  auto foveationProfile = createFoveationProfileHandle();

  if (foveationProfile == XR_NULL_HANDLE) {
    C8Log("[xrapi-foveation] No foveation profile set, not updating swapchain foveation");
    return;
  }

  XrSwapchainStateFoveationFB swapchainStateFoveation{
    .type = XR_TYPE_SWAPCHAIN_STATE_FOVEATION_FB,
    .next = nullptr,
    .flags = XR_SWAPCHAIN_CREATE_FOVEATION_FRAGMENT_DENSITY_MAP_BIT_FB,
    .profile = foveationProfile,
  };

  metaFoveation_->updateSwapchain(
    swapchain, (XrSwapchainStateBaseHeaderFB *)&swapchainStateFoveation);

  metaFoveation_->destroyFoveationProfile(foveationProfile);
}

}  // namespace c8
