#pragma once

#include <memory>

#include "c8/vector.h"
#include "c8/xrapi/openxr/meta-foveation.h"
#include "c8/xrapi/openxr/openxr.h"

namespace c8 {

class XrApiFoveation {
public:
  XrApiFoveation(XrInstance instance, XrSession session);

  static Vector<const char *> requiredExtensionNames();

  void setFoveationActiveStatus(Vector<const char *> &activeExtensions);

  bool isFoveationActive();

  void setFoveationLevel(XrFoveationLevelFB level);

  void *getFoveationImage();

  XrSwapchainCreateInfoFoveationFB getSwapchainCreateInfoFoveation();

  void tryUpdateSwapchainFoveationState(XrSwapchain swapchain);

private:
  bool foveationActive_;
  XrInstance instance_;
  XrSession session_;

  XrFoveationLevelFB foveationLevel_ = XrFoveationLevelFB::XR_FOVEATION_LEVEL_HIGH_FB;

  std::unique_ptr<MetaFoveation> metaFoveation_ = nullptr;

  XrFoveationProfileFB createFoveationProfileHandle();
};

}  // namespace c8
