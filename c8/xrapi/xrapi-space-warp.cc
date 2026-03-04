#include "c8/xrapi/xrapi-space-warp.h"

#include <string>
#include <utility>

#include "c8/c8-log.h"
#include "c8/pixels/opengl/gl-error.h"
#include "c8/pixels/opengl/gl.h"
#include "c8/xrapi/openxr/gl-motion-estimation.h"
#include "c8/xrapi/openxr/helper.h"

namespace {

void checkXr(XrResult result, const char *message) {
  if (!XR_SUCCEEDED(result)) {
    C8Log("[xrapi-space-warp] OPENXR_FAIL: %d (%s)\n", int(result), message);
    DEBUG_BREAK;
  }
}

}  // namespace

namespace c8 {

XrApiSpaceWarp::XrApiSpaceWarp() {}

Vector<const char *> XrApiSpaceWarp::requiredExtensionNames() {
  return {XR_FB_SPACE_WARP_EXTENSION_NAME};
}

void XrApiSpaceWarp::setSpaceWarpAvailableStatus(Vector<const char *> &activeExtensions) {
  const auto requiredExtensions = requiredExtensionNames();
  int numberOfMatches = 0;
  spaceWarpAvailable_ = false;

  for (const auto &requiredExtension : requiredExtensions) {
    for (const auto &activeExtension : activeExtensions) {
      if (std::strcmp(requiredExtension, activeExtension) == 0) {
        numberOfMatches++;
      }
    }
  }

  if (numberOfMatches == requiredExtensions.size()) {
    spaceWarpAvailable_ = true;
  }
}

void XrApiSpaceWarp::createMotionVectorSwapchain(
  GlBuffer *glBuffer, XrSession &session, int colorImageWidth, int colorImageHeight) {
  // Reference: https://registry.khronos.org/OpenXR/specs/1.0/html/xrspec.html#XR_FB_space_warp
  // Deeper Info: https://registry.khronos.org/OpenGL/extensions/QCOM/QCOM_motion_estimation.txt

  if (!spaceWarpAvailable_) {
    C8Log("[xrapi-space-warp] Space Warp is not available");
    return;
  }

  if (
    !spaceWarpProperties_.recommendedMotionVectorImageRectWidth
    || !spaceWarpProperties_.recommendedMotionVectorImageRectHeight) {
    C8Log("[xrapi-space-warp] Space Warp properties not set");
    return;
  }

#if USE_QCOM_MOTION_ESTIMATION
  auto motionVectorDims = texEstimateMotionOutputDimensions(colorImageWidth, colorImageHeight);
  spaceWarpLayer_.motionVectorWidth = motionVectorDims.first;
  spaceWarpLayer_.motionVectorHeight = motionVectorDims.second;

  C8Log(
    "[xrapi-space-warp] QCOM Motion Estimation - "
    "Motion Vector Swapchain Dimensions: %d x %d",
    motionVectorDims.first,
    motionVectorDims.second);
#else
  spaceWarpLayer_.motionVectorWidth =
    spaceWarpProperties_.recommendedMotionVectorImageRectWidth * 2;
  spaceWarpLayer_.motionVectorHeight = spaceWarpProperties_.recommendedMotionVectorImageRectHeight;
#endif

  // Create the motion vector swapchain
  XrSwapchainCreateInfo swapchainCreateInfo = {
    .type = XR_TYPE_SWAPCHAIN_CREATE_INFO,
    .usageFlags = XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT,
    .format = GL_RGBA16F,
    .sampleCount = 1,
    .width = spaceWarpLayer_.motionVectorWidth,
    .height = spaceWarpLayer_.motionVectorHeight,
    .faceCount = 1,
    .arraySize = 1,
    .mipCount = 1,
  };

  checkXr(
    glBuffer->runSyncCommand(
      xrCreateSwapchain, session, &swapchainCreateInfo, &spaceWarpLayer_.motionVectorSwapchain),
    "[xrapi-space-warp] Failed to create motion vector swapchain");

  checkXr(
    glBuffer->runSyncCommand(
      xrEnumerateSwapchainImages,
      spaceWarpLayer_.motionVectorSwapchain,
      0,
      &spaceWarpLayer_.motionVectorSwapchainImageCount,
      nullptr),
    "[xrapi-space-warp] Failed to enumerate motion vector swapchain images");

  Vector<XrSwapchainImageOpenGLESKHR> motionVectorSwapchainImageVector(
    spaceWarpLayer_.motionVectorSwapchainImageCount, {XR_TYPE_SWAPCHAIN_IMAGE_OPENGL_ES_KHR});

  checkXr(
    glBuffer->runSyncCommand(
      xrEnumerateSwapchainImages,
      spaceWarpLayer_.motionVectorSwapchain,
      spaceWarpLayer_.motionVectorSwapchainImageCount,
      &spaceWarpLayer_.motionVectorSwapchainImageCount,
      reinterpret_cast<XrSwapchainImageBaseHeader *>(motionVectorSwapchainImageVector.data())),
    "[xrapi-space-warp] Failed to enumerate motion vector swapchain images");

  // Create the motion vector depth swapchain
  swapchainCreateInfo.format = GL_DEPTH_COMPONENT24;
  swapchainCreateInfo.usageFlags =
    XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

  checkXr(
    glBuffer->runSyncCommand(
      xrCreateSwapchain,
      session,
      &swapchainCreateInfo,
      &spaceWarpLayer_.motionVectorDepthSwapchain),
    "[xrapi-space-warp] Failed to create motion vector swapchain");

  checkXr(
    glBuffer->runSyncCommand(
      xrEnumerateSwapchainImages,
      spaceWarpLayer_.motionVectorDepthSwapchain,
      0,
      &spaceWarpLayer_.motionVectorDepthSwapchainImageCount,
      nullptr),
    "[xrapi-space-warp] Failed to enumerate motion vector swapchain images");

  spaceWarpLayer_.motionVectorSwapchainImages = std::move(motionVectorSwapchainImageVector);

  Vector<XrSwapchainImageOpenGLESKHR> motionVectorDepthSwapchainImageVector(
    spaceWarpLayer_.motionVectorDepthSwapchainImageCount, {XR_TYPE_SWAPCHAIN_IMAGE_OPENGL_ES_KHR});

  checkXr(
    glBuffer->runSyncCommand(
      xrEnumerateSwapchainImages,
      spaceWarpLayer_.motionVectorDepthSwapchain,
      spaceWarpLayer_.motionVectorDepthSwapchainImageCount,
      &spaceWarpLayer_.motionVectorDepthSwapchainImageCount,
      reinterpret_cast<XrSwapchainImageBaseHeader *>(motionVectorDepthSwapchainImageVector.data())),
    "[xrapi-space-warp] Failed to enumerate motion vector depth swapchain images");

  spaceWarpLayer_.motionVectorDepthSwapchainImages =
    std::move(motionVectorDepthSwapchainImageVector);

#if !USE_QCOM_MOTION_ESTIMATION
  spaceWarpLayer_.motionVectorGlSwapchain = std::make_unique<GlSwapchain>(
    glBuffer,
    0,
    spaceWarpLayer_.motionVectorWidth,
    spaceWarpLayer_.motionVectorHeight,
    spaceWarpLayer_.motionVectorSwapchainImageCount,
    GL_RGBA16F,
    spaceWarpLayer_.motionVectorSwapchainImages.data());

  spaceWarpLayer_.motionVectorDepthGlSwapchain = std::make_unique<GlSwapchain>(
    glBuffer,
    0,
    spaceWarpLayer_.motionVectorWidth,
    spaceWarpLayer_.motionVectorHeight,
    spaceWarpLayer_.motionVectorDepthSwapchainImageCount,
    GL_DEPTH_COMPONENT24,
    spaceWarpLayer_.motionVectorDepthSwapchainImages.data());
#endif

  glBuffer->runSyncCommand(+[]() {});  // DEBUG
}

void XrApiSpaceWarp::bindSwapchainImages() {
#if !USE_QCOM_MOTION_ESTIMATION
  spaceWarpLayer_.motionVectorGlSwapchain->bind(motionVectorFrame_.motionVectorImageIndex);
  spaceWarpLayer_.motionVectorDepthGlSwapchain->bind(
    motionVectorFrame_.motionVectorDepthImageIndex);
#endif
}

void XrApiSpaceWarp::updateSpaceWarpTextures(
  GLuint motionVectorTextureId, GLuint motionVectorDepthTextureId) {
#if !USE_QCOM_MOTION_ESTIMATION
  spaceWarpLayer_.motionVectorGlSwapchain->updateTextureId(motionVectorTextureId);
  spaceWarpLayer_.motionVectorDepthGlSwapchain->updateTextureId(motionVectorDepthTextureId);
#endif
}

/*
 * Associate Swapchain images with the composition layer
 * https://registry.khronos.org/OpenXR/specs/1.0/html/xrspec.html#XrCompositionLayerSpaceWarpInfoFB
 */
XrCompositionLayerSpaceWarpInfoFB *XrApiSpaceWarp::mirrorToSwapchain(int numEyes) {
  spaceWarpLayer_.spaceWarpViews.resize(numEyes, {XR_TYPE_COMPOSITION_LAYER_SPACE_WARP_INFO_FB});

  // Assuming double wide texture
  const uint32_t viewWidth = spaceWarpLayer_.motionVectorWidth / numEyes;
  for (int eyeIndex = 0; eyeIndex < numEyes; eyeIndex++) {
    spaceWarpLayer_.spaceWarpViews[eyeIndex].next = nullptr;
    spaceWarpLayer_.spaceWarpViews[eyeIndex].layerFlags = 0;

    spaceWarpLayer_.spaceWarpViews[eyeIndex].motionVectorSubImage.swapchain =
      spaceWarpLayer_.motionVectorSwapchain;

    spaceWarpLayer_.spaceWarpViews[eyeIndex].motionVectorSubImage.imageRect.offset.x =
      eyeIndex * viewWidth;
    spaceWarpLayer_.spaceWarpViews[eyeIndex].motionVectorSubImage.imageRect.offset.y = 0;
    spaceWarpLayer_.spaceWarpViews[eyeIndex].motionVectorSubImage.imageRect.extent.width =
      viewWidth;
    spaceWarpLayer_.spaceWarpViews[eyeIndex].motionVectorSubImage.imageRect.extent.height =
      spaceWarpLayer_.motionVectorHeight;
    spaceWarpLayer_.spaceWarpViews[eyeIndex].motionVectorSubImage.imageArrayIndex = 0;

    spaceWarpLayer_.spaceWarpViews[eyeIndex].depthSubImage.swapchain =
      spaceWarpLayer_.motionVectorDepthSwapchain;
    spaceWarpLayer_.spaceWarpViews[eyeIndex].depthSubImage.imageRect.offset.x =
      eyeIndex * viewWidth;
    spaceWarpLayer_.spaceWarpViews[eyeIndex].depthSubImage.imageRect.offset.y = 0;
    spaceWarpLayer_.spaceWarpViews[eyeIndex].depthSubImage.imageRect.extent.width = viewWidth;
    spaceWarpLayer_.spaceWarpViews[eyeIndex].depthSubImage.imageRect.extent.height =
      spaceWarpLayer_.motionVectorHeight;
    spaceWarpLayer_.spaceWarpViews[eyeIndex].depthSubImage.imageArrayIndex = 0;

    spaceWarpLayer_.spaceWarpViews[eyeIndex].nearZ = 0.1f;
    spaceWarpLayer_.spaceWarpViews[eyeIndex].farZ = 1000.0f;
    spaceWarpLayer_.spaceWarpViews[eyeIndex].minDepth = 0.0f;
    spaceWarpLayer_.spaceWarpViews[eyeIndex].maxDepth = 1.0f;

    // Default to identity pose, should be updated by the app
    spaceWarpLayer_.spaceWarpViews[eyeIndex].appSpaceDeltaPose = {
      {0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}};
  }

  return spaceWarpLayer_.spaceWarpViews.data();
}

void XrApiSpaceWarp::acquireWaitMotionVectorSwapchainImage() {
  XrSwapchainImageAcquireInfo acquireInfo{XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO};
  checkXr(
    xrAcquireSwapchainImage(
      spaceWarpLayer_.motionVectorSwapchain,
      &acquireInfo,
      &motionVectorFrame_.motionVectorImageIndex),
    "[xrapi-space-warp] Failed to acquire Image from the motion vector Swapchain");

  XrSwapchainImageWaitInfo waitInfo{XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO};
  waitInfo.timeout = XR_INFINITE_DURATION;
  checkXr(
    xrWaitSwapchainImage(spaceWarpLayer_.motionVectorSwapchain, &waitInfo),
    "[xrapi-space-warp] Failed to wait for Image from the motion vector Swapchain");

  checkXr(
    xrAcquireSwapchainImage(
      spaceWarpLayer_.motionVectorDepthSwapchain,
      &acquireInfo,
      &motionVectorFrame_.motionVectorDepthImageIndex),
    "[xrapi-space-warp] Failed to acquire Image from the motion vector depth Swapchain");

  checkXr(
    xrWaitSwapchainImage(spaceWarpLayer_.motionVectorDepthSwapchain, &waitInfo),
    "[xrapi-space-warp] Failed to wait for Image from the motion vector depth Swapchain");
}

void XrApiSpaceWarp::releaseMotionVectorSwapchainImage() {
  XrSwapchainImageReleaseInfo releaseInfo = {XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO};

  checkXr(
    xrReleaseSwapchainImage(spaceWarpLayer_.motionVectorSwapchain, &releaseInfo),
    "[xrapi-space-warp] Failed to release motion vector image");

  checkXr(
    xrReleaseSwapchainImage(spaceWarpLayer_.motionVectorDepthSwapchain, &releaseInfo),
    "[xrapi-space-warp] Failed to release motion vector depth image");

  spaceWarpActive_ = std::nullopt;
}

void XrApiSpaceWarp::renderToMotionVector(
  int newColorWidth, int newColorHeight, int colorTextureId) {
  // TODO(lreyna): Fill in the motion vector swapchain images with the motion vectors from QCOM
  if (
    newColorWidth != spaceWarpLayer_.colorWidth || newColorHeight != spaceWarpLayer_.colorHeight) {
    spaceWarpLayer_.colorWidth = newColorWidth;
    spaceWarpLayer_.colorHeight = newColorHeight;
  }

  checkGLError("[xrapi-space-warp] renderToMotionVector");
}

}  // namespace c8
