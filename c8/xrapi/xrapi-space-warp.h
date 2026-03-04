#pragma once

#include <functional>
#include <memory>
#include <optional>

#include "c8/pixels/opengl/gl-framebuffer.h"
#include "c8/pixels/opengl/gl-texture.h"
#include "c8/vector.h"
#include "c8/xrapi/openxr/gl-graphics.h"
#include "c8/xrapi/openxr/gl-impl.h"
#include "c8/xrapi/openxr/openxr.h"

// TODO(lreyna): Make this a runtime option when spacewarp via shaders is available.
#define USE_QCOM_MOTION_ESTIMATION false

typedef std::function<bool()> IsSpaceWarpActiveCallback;

namespace c8 {

struct XrApiMotionVectorFrame {
  uint32_t motionVectorImageIndex;
  uint32_t motionVectorDepthImageIndex;
};

struct XrApiSpaceWarpLayer {
  uint32_t motionVectorWidth;
  uint32_t motionVectorHeight;
  uint32_t motionVectorSwapchainImageCount;
  uint32_t motionVectorDepthSwapchainImageCount;
  XrSwapchain motionVectorSwapchain;
  XrSwapchain motionVectorDepthSwapchain;
#if !USE_QCOM_MOTION_ESTIMATION
  std::unique_ptr<GlSwapchain> motionVectorGlSwapchain;
  std::unique_ptr<GlSwapchain> motionVectorDepthGlSwapchain;
#endif

  Vector<XrCompositionLayerSpaceWarpInfoFB> spaceWarpViews;

  // Render Resources
  int colorWidth = 0;
  int colorHeight = 0;
  Vector<XrSwapchainImageOpenGLESKHR> motionVectorSwapchainImages;
  Vector<XrSwapchainImageOpenGLESKHR> motionVectorDepthSwapchainImages;

  // TODO(lreyna): Add render resources for motion vectors
};

class XrApiSpaceWarp {
public:
  XrApiSpaceWarp();

  static Vector<const char *> requiredExtensionNames();

  void createMotionVectorSwapchain(
    GlBuffer *glBuffer, XrSession &session, int colorImageWidth, int colorImageHeight);

  void setSpaceWarpAvailableStatus(Vector<const char *> &activeExtensions);

  bool isSpaceWarpAvailable() { return spaceWarpAvailable_; }

  bool isSpaceWarpActive() {
    if (spaceWarpActive_.has_value()) {
      return spaceWarpActive_.value();
    }

    if (isSpaceWarpActiveCallback_) {
      spaceWarpActive_ = (*isSpaceWarpActiveCallback_)();
      return spaceWarpActive_.value();
    }

    return false;
  }

  void setIsSpaceWarpActiveCallback(IsSpaceWarpActiveCallback &&callback) {
    isSpaceWarpActiveCallback_ = std::make_unique<IsSpaceWarpActiveCallback>(std::move(callback));
  }

  void bindSwapchainImages();

  void updateSpaceWarpTextures(GLuint motionVectorTextureId, GLuint motionVectorDepthTextureId);

  XrCompositionLayerSpaceWarpInfoFB *mirrorToSwapchain(int numEyes);

  void acquireWaitMotionVectorSwapchainImage();

  void releaseMotionVectorSwapchainImage();

  void renderToMotionVector(int newColorWidth, int newColorHeight, int colorTextureId);

  XrSystemSpaceWarpPropertiesFB &getSpaceWarpProperties() { return spaceWarpProperties_; }

private:
  bool spaceWarpAvailable_ = false;
  std::unique_ptr<IsSpaceWarpActiveCallback> isSpaceWarpActiveCallback_ = nullptr;
  std::optional<bool> spaceWarpActive_ = std::nullopt;

  XrSystemSpaceWarpPropertiesFB spaceWarpProperties_ = {XR_TYPE_SYSTEM_SPACE_WARP_PROPERTIES_FB};

  XrApiSpaceWarpLayer spaceWarpLayer_;

  XrApiMotionVectorFrame motionVectorFrame_;
};

}  // namespace c8
