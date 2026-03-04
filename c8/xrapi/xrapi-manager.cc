#include "c8/xrapi/xrapi-manager.h"

#include <dlfcn.h>
#include <uv.h>

#include <vector>

#include "c8/hmatrix.h"
#include "c8/io/capnp-messages.h"
#include "c8/quaternion.h"
#include "c8/xrapi/openxr/gl-contexts.h"
#include "c8/xrapi/openxr/gl-impl.h"
#include "c8/xrapi/openxr/gl-texture-auto-restore.h"
#include "c8/xrapi/openxr/helper.h"
#include "c8/xrapi/xrapi-space-warp.h"

namespace {

std::optional<XrReferenceSpaceType> spaceTypeStringToEnum(const std::string &spaceType) {
  if (spaceType == "viewer") {
    return XR_REFERENCE_SPACE_TYPE_VIEW;
  } else if (spaceType == "local") {
    return XR_REFERENCE_SPACE_TYPE_LOCAL;
  } else if (spaceType == "stage") {
    return XR_REFERENCE_SPACE_TYPE_STAGE;
  } else if (spaceType == "local-floor") {
    return XR_REFERENCE_SPACE_TYPE_LOCAL_FLOOR;
  } else {
    return std::nullopt;
  }
}

}  // namespace

using namespace c8;

using cmd::TransferWrap;

const int BEFORE_SESSION_POLL_INTERNAL_MS = 10;

XrApiManager::XrApiManager() {}

void asyncFrameLoop(XrApiWaitFrameState *state) {
  uv_loop_t uv_loop;
  uv_loop_init(&uv_loop);

  state->frameDoneHandle = uv_async_t();
  state->frameDoneHandle.data = state;

  state->endFrameHandle = uv_async_t();
  state->endFrameHandle.data = state;

  uv_async_init(&uv_loop, &state->frameDoneHandle, [](uv_async_t *handle) {
    XrFrameWaitInfo frameWaitInfo{XR_TYPE_FRAME_WAIT_INFO};
    XrFrameState localFrameState{XR_TYPE_FRAME_STATE};
    XrResult waitResult{};

    auto state = static_cast<XrApiWaitFrameState *>(handle->data);
    waitResult = xrWaitFrame(state->session, &frameWaitInfo, &localFrameState);
    if (waitResult == XR_ERROR_SESSION_NOT_RUNNING) {
      // Thread exits when xrWaitFrame returns a XR_ERROR_SESSION_NOT_RUNNING.
      state->frameThreadActive = false;
      return;
    }
    if (!XR_SUCCEEDED(waitResult)) {
      C8Log("[xrapi-manager] Error caught from xrWaitFrame: %d", waitResult);
      // Process the previous frame to allow the app to continue.
      uv_async_send(&state->processFrameHandle);
      return;
    }

    // Thread-safe store of the FrameState.
    state->frameState.store(localFrameState, std::memory_order_release);

    // Notify the UV loop of a new frame to process.
    uv_async_send(&state->processFrameHandle);
  });

  // End the frame loop (to be called at stopping the session).
  uv_async_init(&uv_loop, &state->endFrameHandle, [](uv_async_t *handle) {
    auto state = static_cast<XrApiWaitFrameState *>(handle->data);
    state->frameThreadActive = false;
  });

  uv_async_send(&state->frameDoneHandle);
  state->frameThreadActive = true;

  while (state->frameThreadActive && uv_run(&uv_loop, UV_RUN_ONCE)) {
  }

  uv_close((uv_handle_t *)&state->frameDoneHandle, NULL);
  uv_close((uv_handle_t *)&state->endFrameHandle, NULL);
}

void asyncFrameCallback(uv_async_t *handle) {
  auto state = static_cast<XrApiWaitFrameState *>(handle->data);
  XrFrameState localFrameState = state->frameState.load(std::memory_order_acquire);
  state->manager->processFrame(localFrameState);
}

void pollTimerCallback(uv_timer_t *handle) {
  auto manager = static_cast<XrApiManager *>(handle->data);
  manager->pollEvents();
}

void XrApiManager::performSetup(struct android_app *android_app, uv_loop_t *loop) {
  if (didSetup_) {
    C8Log("[xrapi-manager] Unexpected performSetup, already setup");
    return;
  }
  didSetup_ = true;

  loop_ = loop;

  PFN_xrInitializeLoaderKHR xrInitializeLoaderKHR = nullptr;
  xrGetInstanceProcAddr(
    XR_NULL_HANDLE, "xrInitializeLoaderKHR", (PFN_xrVoidFunction *)&xrInitializeLoaderKHR);
  if (xrInitializeLoaderKHR) {
    // Fill out an XrLoaderInitInfoAndroidKHR structure and initialize the loader for Android.
    XrLoaderInitInfoAndroidKHR loaderInitializeInfoAndroid{XR_TYPE_LOADER_INIT_INFO_ANDROID_KHR};
    loaderInitializeInfoAndroid.applicationVM = android_app->activity->vm;
    loaderInitializeInfoAndroid.applicationContext = android_app->activity->clazz;
    checkXr(
      xrInitializeLoaderKHR((XrLoaderInitInfoBaseHeaderKHR *)&loaderInitializeInfoAndroid),
      "[xrapi-manager] Failed to initialize Loader for Android.");
  } else {
    C8Log("[xrapi-manager] xrInitializeLoaderKHR not found.");
    raise(SIGTRAP);
  }
}

bool XrApiManager::canRequestSession() { return !session_ && !ended_; }

void XrApiManager::updateDeltaPose(const XrPosef &pose) { appSpaceDeltaPose_ = pose; }

void XrApiManager::updateFixedFoveation(float fixedFoveation) {
  if (!renderLayer_) {
    C8Log("[xrapi-manager] Unexpected updateFixedFoveation, no renderLayer");
    raise(SIGTRAP);
  }

  renderLayer_->fixedFoveation = fixedFoveation;

  // Foveation should be updated when the texture is available
  if (renderLayer_->textureId) {
    updateTextureFoveation(glBuffer_, renderLayer_->textureId, renderLayer_->fixedFoveation);
  }
}

void XrApiManager::updateRenderState(float newDepthNear, float newDepthFar) {
  depthNear_ = newDepthNear;
  depthFar_ = newDepthFar;
}

void XrApiManager::updateRenderTexture(GLuint textureId) {
  if (!renderLayer_) {
    C8Log("[xrapi-manager] Unexpected updateRenderTexture, no renderLayer");
    raise(SIGTRAP);
  }

  renderLayer_->textureId = textureId;

  renderLayer_->colorSwapchain.updateTextureId(textureId);
  updateTextureFoveation(glBuffer_, renderLayer_->textureId, renderLayer_->fixedFoveation);
}

void XrApiManager::updateSpaceWarpTextures(
  GLuint motionVectorTextureId, GLuint motionVectorDepthTextureId) {
  if (!renderLayer_) {
    C8Log("[xrapi-manager] Unexpected updateSpaceWarpTextures, no renderLayer");
    raise(SIGTRAP);
  }

  if (spaceWarpHandler_) {
    glBuffer_->runSyncCommand(
      +[](
         XrApiSpaceWarp *spaceWarpHandler,
         GLuint motionVectorTextureId,
         GLuint motionVectorDepthTextureId) {
        spaceWarpHandler->updateSpaceWarpTextures(
          motionVectorTextureId, motionVectorDepthTextureId);
      },
      &*spaceWarpHandler_,
      motionVectorTextureId,
      motionVectorDepthTextureId);
  }
}

void fillPerspectiveMatrix(XrFrameView::Builder &view, const XrFovf &fov, float near, float far) {
  float top = tan(fov.angleUp) * near;
  float bottom = tan(fov.angleDown) * near;
  float left = tan(fov.angleLeft) * near;
  float right = tan(fov.angleRight) * near;

  auto projection = view.initProjection(16);
  projection.set(0, (2 * near) / (right - left));
  projection.set(1, 0);
  projection.set(2, 0);
  projection.set(3, 0);
  projection.set(4, 0);
  projection.set(5, (2 * near) / (top - bottom));
  projection.set(6, 0);
  projection.set(7, 0);
  projection.set(8, (right + left) / (right - left));
  projection.set(9, (top + bottom) / (top - bottom));
  projection.set(10, (far + near) / (near - far));
  projection.set(11, -1);
  projection.set(12, 0);
  projection.set(13, 0);
  projection.set(14, (2 * far * near) / (near - far));
  projection.set(15, 0);
}

void fillTransform(XrFrameView::Builder &view, const XrPosef &pose) {
  auto mat =
    Quaternion(pose.orientation.w, pose.orientation.x, pose.orientation.y, pose.orientation.z)
      .toRotationMat()
      .translate(pose.position.x, pose.position.y, pose.position.z);

  auto transform = view.initTransform(16);

  for (int i = 0; i < 16; i++) {
    transform.set(i, mat.data()[i]);
  }
}

void XrApiManager::requestSession(XrSessionRequest request) {
  if (session_) {
    C8Log("[xrapi-manager] Unexpected requestSession, session already exists");
    return;
  }
  if (!didSetup_) {
    C8Log("[xrapi-manager] XrApiManager not setup, calling performSetup");
    return;
  }
  if (startSession(request.mode)) {
    C8Log("[xrapi-manager] Failed for some reason");
  };

  onFrameData_ = request.frameCallback;

  request.readyCallback();
}

std::vector<uint8_t> XrApiManager::initializeSwapchain(GLuint textureId, bool spaceWarpRequested) {
  if (!session_) {
    C8Log("[xrapi-manager] Unexpected initializeSwapchain, no session");
    raise(SIGTRAP);
  }
  if (renderLayer_) {
    C8Log("[xrapi-manager] Unexpected initializeSwapchain, swapchain already exists");
    raise(SIGTRAP);
  }

  createSwapchain(textureId);

  int motionVectorWidth = 0, motionVectorHeight = 0;
  int depthStencilWidth = 0, depthStencilHeight = 0;
  if (spaceWarpHandler_ && spaceWarpRequested) {
    spaceWarpHandler_->createMotionVectorSwapchain(
      glBuffer_,
      session_->session,
      // NOTE: For QCOM_motion_estimation Expects double wide texture dimensions from color buffer
      session_->views[0].recommendedImageRectWidth * 2,
      session_->views[0].recommendedImageRectHeight);

    motionVectorWidth =
      spaceWarpHandler_->getSpaceWarpProperties().recommendedMotionVectorImageRectWidth;
    motionVectorHeight =
      spaceWarpHandler_->getSpaceWarpProperties().recommendedMotionVectorImageRectHeight;

    // NOTE: Oculus Browser sets the depthStencil texture
    // to the same size as the motion vector texture when space-warp is enabled
    depthStencilWidth = motionVectorWidth;
    depthStencilHeight = motionVectorHeight;
  } else {
    // Case where space warp is supported by OpenXR, but not requested by App
    // TODO(lreyna): Clean up XR Feature Requested / Enabled logic
    spaceWarpHandler_ = std::nullopt;
  }

  MutableRootMessage<XrSwapchainSetup> res;

  res.builder().setWidth(session_->views[0].recommendedImageRectWidth);
  res.builder().setHeight(session_->views[0].recommendedImageRectHeight);
  res.builder().setMotionVectorWidth(motionVectorWidth);
  res.builder().setMotionVectorHeight(motionVectorHeight);
  res.builder().setDepthStencilWidth(depthStencilWidth);
  res.builder().setDepthStencilHeight(depthStencilHeight);

  ConstRootMessage<XrSwapchainSetup> resData(res.reader());
  return std::vector<uint8_t>(resData.bytes().begin(), resData.bytes().end());
}

void XrApiManager::setIsSpaceWarpActiveCallback(
  IsSpaceWarpActiveCallback &&isSpaceWarpActiveCallback) {
  if (spaceWarpHandler_) {
    spaceWarpHandler_->setIsSpaceWarpActiveCallback(std::move(isSpaceWarpActiveCallback));
    return;
  }

  C8Log("[xrapi-manager] Unexpected setIsSpaceWarpActiveCallback, no Space Warp is not Active");
}

// TODO(christoph): Implement properly
void XrApiManager::endSession() {
  ended_ = true;
  uv_close((uv_handle_t *)&waitFrameState_.processFrameHandle, NULL);
  uv_timer_stop(&pollTimer_);
  uv_close((uv_handle_t *)&pollTimer_, NULL);
}

int XrApiManager::pollEvents() {
  // Poll OpenXR for a new event.
  XrEventDataBuffer eventData{XR_TYPE_EVENT_DATA_BUFFER};
  auto poll = [&]() -> bool {
    eventData = {XR_TYPE_EVENT_DATA_BUFFER};
    return xrPollEvent(session_->instance, &eventData) == XR_SUCCESS;
  };

  int i = 32;

  while (poll() && (i-- > 0)) {
    int result = handleEvent(&eventData);
    if (result != 0) {
      C8Log("[xrapi-manager] Failed to handle event");
      return result;
    }
  }

  return 0;
}

void XrApiManager::pollActions(XrTime predictedTime) {
  auto &controllers_ = session_->controllers;

  // Update our action set with up-to-date input data.
  // First, we specify the actionSet we are polling.
  XrActiveActionSet activeActionSet{};
  activeActionSet.actionSet = actionSet_;
  activeActionSet.subactionPath = XR_NULL_PATH;
  // Now we sync the Actions to make sure they have current data.
  XrActionsSyncInfo actionsSyncInfo{XR_TYPE_ACTIONS_SYNC_INFO};
  actionsSyncInfo.countActiveActionSets = 1;
  actionsSyncInfo.activeActionSets = &activeActionSet;
  checkXr(
    xrSyncActions(session_->session, &actionsSyncInfo), "[xrapi-manager] Failed to sync Actions.");
  XrActionStateGetInfo actionStateGetInfo{XR_TYPE_ACTION_STATE_GET_INFO};

  // For each hand, get the states of all actions
  for (int i = 0; i < controllers_.size(); i++) {
    if (handTrackingEnabled_) {
      updateHand(i);
    }

    actionStateGetInfo.subactionPath = controllers_[i].handPath;

    // Controller hand pose action
    actionStateGetInfo.action = palmPoseAction_;
    checkXr(
      xrGetActionStatePose(session_->session, &actionStateGetInfo, &controllers_[i].handPoseState),
      "[xrapi-manager] Failed to get Pose State.");

    // Controller aim pose action
    actionStateGetInfo.action = aimPoseAction_;
    checkXr(
      xrGetActionStatePose(session_->session, &actionStateGetInfo, &controllers_[i].aimPoseState),
      "[xrapi-manager] Failed to get Aim Pose State.");

    // Controller thumbstick position actions
    actionStateGetInfo.action = thumbstickXAction_;
    checkXr(
      xrGetActionStateFloat(
        session_->session, &actionStateGetInfo, &controllers_[i].thumbstickXState),
      "[xrapi-manager] Failed to get Float State of Thumbstick X action.");

    actionStateGetInfo.action = thumbstickYAction_;
    checkXr(
      xrGetActionStateFloat(
        session_->session, &actionStateGetInfo, &controllers_[i].thumbstickYState),
      "[xrapi-manager] Failed to get Float State of Thumbstick Y action.");

    // Controller thumbstick click action
    actionStateGetInfo.action = thumbstickClickAction_;
    checkXr(
      xrGetActionStateBoolean(
        session_->session, &actionStateGetInfo, &controllers_[i].thumbstickClickState),
      "[xrapi-manager] Failed to get Boolean State of Thumbstick Click action.");

    // Controller select action
    actionStateGetInfo.action = selectAction_;
    checkXr(
      xrGetActionStateFloat(session_->session, &actionStateGetInfo, &controllers_[i].selectState),
      "[xrapi-manager] Failed to get Float State of Select action.");

    // Controller squeeze action
    actionStateGetInfo.action = squeezeAction_;
    checkXr(
      xrGetActionStateFloat(session_->session, &actionStateGetInfo, &controllers_[i].squeezeState),
      "[xrapi-manager] Failed to get Float State of Squeeze action.");

    // If possible store aim and hand poses in the controller
    if (controllers_[i].handPoseState.isActive) {
      XrSpaceLocation spaceLocation{XR_TYPE_SPACE_LOCATION};
      XrResult res = xrLocateSpace(
        controllers_[i].handPoseSpace, session_->referenceSpace, predictedTime, &spaceLocation);
      if (
        XR_UNQUALIFIED_SUCCESS(res)
        && (spaceLocation.locationFlags & XR_SPACE_LOCATION_POSITION_VALID_BIT) != 0
        && (spaceLocation.locationFlags & XR_SPACE_LOCATION_ORIENTATION_VALID_BIT) != 0) {
        controllers_[i].handPose = spaceLocation.pose;
      } else {
        controllers_[i].handPoseState.isActive = false;
      }
    }
    if (controllers_[i].aimPoseState.isActive) {
      XrSpaceLocation spaceLocation{XR_TYPE_SPACE_LOCATION};
      XrResult res = xrLocateSpace(
        controllers_[i].aimPoseSpace, session_->referenceSpace, predictedTime, &spaceLocation);
      if (
        XR_UNQUALIFIED_SUCCESS(res)
        && (spaceLocation.locationFlags & XR_SPACE_LOCATION_POSITION_VALID_BIT) != 0
        && (spaceLocation.locationFlags & XR_SPACE_LOCATION_ORIENTATION_VALID_BIT) != 0) {
        controllers_[i].aimPose = spaceLocation.pose;
      } else {
        controllers_[i].aimPoseState.isActive = false;
      }
    }
  }

  // Controller button actions
  actionStateGetInfo.action = xButtonAction_;
  actionStateGetInfo.subactionPath = controllers_[0].handPath;
  checkXr(
    xrGetActionStateBoolean(
      session_->session, &actionStateGetInfo, &controllers_[0].buttonStates[0].clicked),
    "[xrapi-manager] Failed to get Boolean State of X Button action.");
  actionStateGetInfo.action = yButtonAction_;
  checkXr(
    xrGetActionStateBoolean(
      session_->session, &actionStateGetInfo, &controllers_[0].buttonStates[1].clicked),
    "[xrapi-manager] Failed to get Boolean State of Y Button action.");
  actionStateGetInfo.action = menuButtonAction_;
  checkXr(
    xrGetActionStateBoolean(
      session_->session, &actionStateGetInfo, &controllers_[0].buttonStates[2].clicked),
    "[xrapi-manager] Failed to get Boolean State of Menu Button action.");
  actionStateGetInfo.action = xTouchedAction_;
  checkXr(
    xrGetActionStateBoolean(
      session_->session, &actionStateGetInfo, &controllers_[0].buttonStates[0].touched),
    "[xrapi-manager] Failed to get Boolean State of X Touched action.");
  actionStateGetInfo.action = yTouchedAction_;
  checkXr(
    xrGetActionStateBoolean(
      session_->session, &actionStateGetInfo, &controllers_[0].buttonStates[1].touched),
    "[xrapi-manager] Failed to get Boolean State of Y Touched action.");

  actionStateGetInfo.action = aButtonAction_;
  actionStateGetInfo.subactionPath = controllers_[1].handPath;
  checkXr(
    xrGetActionStateBoolean(
      session_->session, &actionStateGetInfo, &controllers_[1].buttonStates[0].clicked),
    "[xrapi-manager] Failed to get Boolean State of A Button action.");
  actionStateGetInfo.action = bButtonAction_;
  checkXr(
    xrGetActionStateBoolean(
      session_->session, &actionStateGetInfo, &controllers_[1].buttonStates[1].clicked),
    "[xrapi-manager] Failed to get Boolean State of B Button action.");
  actionStateGetInfo.action = aTouchedAction_;
  checkXr(
    xrGetActionStateBoolean(
      session_->session, &actionStateGetInfo, &controllers_[1].buttonStates[0].touched),
    "[xrapi-manager] Failed to get Boolean State of A Touched action.");
  actionStateGetInfo.action = bTouchedAction_;
  checkXr(
    xrGetActionStateBoolean(
      session_->session, &actionStateGetInfo, &controllers_[1].buttonStates[1].touched),
    "[xrapi-manager] Failed to get Boolean State of B Touched action.");
}

void XrApiManager::updateHapticState(String handedness, float value, float duration) {
  auto &controllers_ = session_->controllers;

  for (int i = 0; i < controllers_.size(); i++) {
    if (pathToHandedness(controllers_[i].handPath) == handedness) {
      XrHapticVibration vibration{
        .type = XR_TYPE_HAPTIC_VIBRATION,
        .duration = static_cast<XrDuration>(duration),
        .frequency = XR_FREQUENCY_UNSPECIFIED,
        .amplitude = value,
      };

      XrHapticActionInfo hapticActionInfo{
        .type = XR_TYPE_HAPTIC_ACTION_INFO,
        .action = hapticAction_,
        .subactionPath = controllers_[i].handPath,
      };

      checkXr(
        xrApplyHapticFeedback(
          session_->session, &hapticActionInfo, (XrHapticBaseHeader *)&vibration),
        "[xrapi-manager] Failed to apply haptic feedback.");
      break;
    }
  }
}

int XrApiManager::handleEvent(XrEventDataBuffer *event) {
  switch (event->type) {
    case XR_TYPE_EVENT_DATA_EVENTS_LOST: {
      return handleEventsLost(reinterpret_cast<XrEventDataEventsLost *>(event));
    }
    case XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING: {
      return handleInstanceLossPending(reinterpret_cast<XrEventDataInstanceLossPending *>(event));
    }
    case XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED: {
      return handleInteractionProfileChanged(
        reinterpret_cast<XrEventDataInteractionProfileChanged *>(event));
    }
    case XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING: {
      return handleReferenceStateChange(
        reinterpret_cast<XrEventDataReferenceSpaceChangePending *>(event));
    }
    case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED: {
      return handleStateChange(reinterpret_cast<XrEventDataSessionStateChanged *>(event));
    }
    default: {
      return 0;
    }
  }
}

bool XrApiManager::isSessionSupported() { return true; }

void XrApiManager::processFrame(XrFrameState frameState) {
  if (!session_) {
    C8Log("[xrapi-manager] Unexpected tick, no session");
    return;
  }

  pollEvents();

  if (!renderLayer_) {
    C8Log("[xrapi-manager] Unexpected tick, no renderLayer");
    return;
  }

  if (!session_) {
    C8Log("[xrapi-manager] Session might have ended");
    return;
  }

  bool canRenderFrame =
    (session_->state == XR_SESSION_STATE_READY || session_->state == XR_SESSION_STATE_VISIBLE
     || session_->state == XR_SESSION_STATE_FOCUSED
     || session_->state == XR_SESSION_STATE_SYNCHRONIZED);

  if (!canRenderFrame) {
    C8Log("[xrapi-manager] Not ready: %d", session_->state);
    return;
  }

  {
    TextureAutoRestore textureAutoRestore(glBuffer_);

    // Tell the OpenXR compositor that the application is beginning the frame.
    XrFrameBeginInfo frameBeginInfo{XR_TYPE_FRAME_BEGIN_INFO};
    checkXr(
      glBuffer_->runSyncCommand(xrBeginFrame, session_->session, &frameBeginInfo),
      "[xrapi-manager] Failed to begin the XR Frame.");
  }

  if (!frameState.shouldRender) {
    C8Log("[xrapi-manager] Should not render, but we will render anyways");
    // return;
  }

  RenderLayerInfo renderLayerInfo{};
  beforeRender(frameState);
  glBuffer_->runSyncCommand(
    +[](XrApiManager *self, XrTime time) { self->pollActions(time); },
    this,
    frameState.predictedDisplayTime);
  renderFrame(renderLayerInfo);
  afterRender();

  glBuffer_->queueCommand(
    +[](
       XrApiManager *self,
       const XrCompositionLayerProjectionView *layerProjectionViewsPtr,
       std::size_t layerProjectionViewsSize,
       XrTime predictedDisplayTime) {
      XrCompositionLayerProjection projectionLayer = {
        .type = XR_TYPE_COMPOSITION_LAYER_PROJECTION,
        .layerFlags = XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT
          | XR_COMPOSITION_LAYER_CORRECT_CHROMATIC_ABERRATION_BIT,
        .space = self->session_->referenceSpace,
        .viewCount = static_cast<uint32_t>(layerProjectionViewsSize),
        .views = layerProjectionViewsPtr,
      };

      XrCompositionLayerBaseHeader *projectionLayerPtr =
        reinterpret_cast<XrCompositionLayerBaseHeader *>(&projectionLayer);

      XrFrameEndInfo frameEndInfo{
        .type = XR_TYPE_FRAME_END_INFO,
        .displayTime = predictedDisplayTime,
        .environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE,
        .layerCount = 1,
        .layers = &projectionLayerPtr};

      self->checkXr(
        xrEndFrame(self->session_->session, &frameEndInfo),
        "[xrapi-manager] Failed to end the XR Frame.");
    },
    this,
    TransferWrap(
      renderLayerInfo.layerProjectionViews.data(), renderLayerInfo.layerProjectionViews.size()),
    renderLayerInfo.layerProjectionViews.size(),
    frameState.predictedDisplayTime);

  glBuffer_->queueCommand(uv_async_send, &waitFrameState_.frameDoneHandle);
}

int XrApiManager::beforeRender(XrFrameState &frameState) {
  std::vector<XrView> xrViews(renderLayer_->viewCount, {XR_TYPE_VIEW});

  XrViewState viewState{XR_TYPE_VIEW_STATE};
  XrViewLocateInfo viewLocateInfo{
    .type = XR_TYPE_VIEW_LOCATE_INFO,
    .viewConfigurationType = session_->viewMode,
    .displayTime = frameState.predictedDisplayTime,
    .space = session_->referenceSpace};
  uint32_t viewCount = 0;
  XrResult result = xrLocateViews(
    session_->session,
    &viewLocateInfo,
    &viewState,
    static_cast<uint32_t>(xrViews.size()),
    &viewCount,
    xrViews.data());

  if (result != XR_SUCCESS) {
    C8Log("[xrapi-manager] Failed to locate Views.");
    return -1;
  }

  if (viewCount != renderLayer_->viewCount) {
    C8Log("[xrapi-manager] Unexpected xr view count compared to layer view count: %d", viewCount);
    return -1;
  }

  currentFrame_.emplace(CurrentFrame{.state = frameState});

  for (uint32_t i = 0; i < viewCount; i++) {
    currentFrame_->views.push_back({
      .xrView = xrViews[i],
    });
  }

  auto &colorSwapchainInfo = renderLayer_->colorInfo;

  // Acquire and wait for an image from the swapchains.
  // Get the image index of an image in the swapchains.
  // The timeout is infinite.
  XrSwapchainImageAcquireInfo acquireInfo{XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO};
  checkXr(
    glBuffer_->runSyncCommand(
      xrAcquireSwapchainImage,
      colorSwapchainInfo.swapchain,
      &acquireInfo,
      &currentFrame_->colorIndex),
    "[xrapi-manager] Failed to acquire Image from the Color Swapchain");

  XrSwapchainImageWaitInfo waitInfo = {XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO};
  waitInfo.timeout = XR_INFINITE_DURATION;
  checkXr(
    glBuffer_->runSyncCommand(xrWaitSwapchainImage, colorSwapchainInfo.swapchain, &waitInfo),
    "[xrapi-manager] Failed to wait for Image from the Color Swapchain");

  if (spaceWarpHandler_) {
    glBuffer_->runSyncCommand(
      +[](XrApiSpaceWarp *spaceWarpHandler) {
        spaceWarpHandler->acquireWaitMotionVectorSwapchainImage();
      },
      &*spaceWarpHandler_);
  }

  // This already uses the command buffer through GlSwapchain::bind().
  renderLayer_->colorSwapchain.bind(currentFrame_->colorIndex);

  if (spaceWarpHandler_) {
    // This already uses the command buffer through GlSwapchain::bind().
    spaceWarpHandler_->bindSwapchainImages();
  }

  return 0;
}

int XrApiManager::afterRender() {
  glBuffer_->queueCommand(
    +[](XrApiManager *self) {
      XrSwapchainImageReleaseInfo releaseInfo{XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO};
      xrReleaseSwapchainImage(self->renderLayer_->colorInfo.swapchain, &releaseInfo);
    },
    this);

  if (spaceWarpHandler_) {
    glBuffer_->queueCommand(
      +[](XrApiSpaceWarp *spaceWarpHandler) {
        spaceWarpHandler->releaseMotionVectorSwapchainImage();
      },
      &*spaceWarpHandler_);
  }

  currentFrame_ = std::nullopt;
  return 0;
}

void spaceToProtoPose(XrPose::Builder poseBuilder, XrPosef currPose) {
  auto pos = poseBuilder.initPosition(3);
  auto rot = poseBuilder.initOrientation(4);

  pos.set(0, currPose.position.x);
  pos.set(1, currPose.position.y);
  pos.set(2, currPose.position.z);

  rot.set(0, currPose.orientation.x);
  rot.set(1, currPose.orientation.y);
  rot.set(2, currPose.orientation.z);
  rot.set(3, currPose.orientation.w);
}

int XrApiManager::renderFrame(RenderLayerInfo &renderLayerInfo) {
  MutableRootMessage<XrFrameData> frameData;

  auto &controllers_ = session_->controllers;

  auto writer = frameData.builder();

  float time = currentFrame_->state.predictedDisplayTime / 1e6;

  writer.setTime(time);

  auto views = writer.initViews(currentFrame_->views.size());
  for (int i = 0; i < currentFrame_->views.size(); i++) {
    auto view = views[i];
    fillPerspectiveMatrix(view, currentFrame_->views[i].xrView.fov, depthNear_, depthFar_);
    fillTransform(view, currentFrame_->views[i].xrView.pose);
  }

  auto controllers = writer.initControllers(controllers_.size());
  for (int i = 0; i < controllers_.size(); i++) {
    auto controller = controllers[i];

    if (controllers_[i].jointLocations.isActive) {
      controller.setIsHand(true);
      auto joints = controller.initJointPoses(XR_HAND_JOINT_COUNT_EXT);
      for (int j = 0; j < XR_HAND_JOINT_COUNT_EXT; j++) {
        auto joint = joints[j];
        auto jointPose = joint.initPose();
        spaceToProtoPose(jointPose, controllers_[i].jointLocations.jointLocations[j].pose);
        joint.setRadius(controllers_[i].jointLocations.jointLocations[j].radius);
      }
    }

    auto pose = controller.initPose();
    spaceToProtoPose(pose, controllers_[i].handPose);

    auto aimPose = controller.initAimPose();
    spaceToProtoPose(aimPose, controllers_[i].aimPose);

    auto thumbstick = controller.initThumbstickState();
    if (controllers_[i].thumbstickXState.isActive) {
      thumbstick.setX(controllers_[i].thumbstickXState.currentState);
    }
    if (controllers_[i].thumbstickYState.isActive) {
      thumbstick.setY(controllers_[i].thumbstickYState.currentState);
    }
    if (controllers_[i].thumbstickClickState.isActive) {
      auto clickState = thumbstick.initClick();
      clickState.setValue(controllers_[i].thumbstickClickState.currentState);
    }

    if (controllers_[i].selectState.isActive) {
      controller.setSelectState(controllers_[i].selectState.currentState);
    }
    if (controllers_[i].squeezeState.isActive) {
      controller.setSqueezeState(controllers_[i].squeezeState.currentState);
    }

    auto states = controller.initButtonStates(controllers_[i].buttonStates.size());
    for (size_t j = 0; j < controllers_[i].buttonStates.size(); ++j) {
      auto state = states[j];
      if (controllers_[i].buttonStates[j].clicked.isActive) {
        state.setValue(controllers_[i].buttonStates[j].clicked.currentState);
      }
      if (controllers_[i].buttonStates[j].touched.isActive) {
        state.setTouched(controllers_[i].buttonStates[j].touched.currentState);
      }
    }
  }

  auto events = writer.initConnectEvents(connectEvents_.size());
  for (int i = 0; i < connectEvents_.size(); i++) {
    auto event = events[i];
    event.setConnectionState(connectEvents_[i].connectionState);
    event.setHandedness(connectEvents_[i].handedness);
    event.setInteractionProfile(connectEvents_[i].interactionProfile);
    event.setIsHand(connectEvents_[i].isHand);
  }
  connectEvents_.clear();

  ConstRootMessage<XrFrameData> frameDataConst(frameData.reader());

  FrameCallbackData data(frameDataConst.bytes().begin(), frameDataConst.bytes().end());

  if (onFrameData_) {
    (*onFrameData_)(data);
  }

#if USE_QCOM_MOTION_ESTIMATION
  if (spaceWarpHandler_ && spaceWarpHandler_->isSpaceWarpActive()) {
    spaceWarpHandler_->renderToMotionVector(
      renderLayer_->fullWidth, renderLayer_->height, renderLayer_->textureId);
  }
#endif

  const bool removeDepthStore = !(spaceWarpHandler_ && spaceWarpHandler_->isSpaceWarpActive());
  glFinalize(glBuffer_, removeDepthStore);

  mirrorToSwapchain(renderLayerInfo);

  return 0;
}

int XrApiManager::mirrorToSwapchain(RenderLayerInfo &renderLayerInfo) {
  if (!renderLayer_) {
    C8Log("[xrapi-manager] Unexpected mirrorToSwapchain, no renderLayer");
    return 0;
  }

  auto &colorSwapchainInfo = renderLayer_->colorInfo;

  // Resize the layer projection views to match the view count. The layer projection views are
  // used in the layer projection.
  renderLayerInfo.layerProjectionViews.resize(
    renderLayer_->viewCount, {XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW});

  // Per view in the view configuration:
  for (uint32_t i = 0; i < currentFrame_->views.size(); i++) {
    XrView &xrView = currentFrame_->views[i].xrView;

    // Get the width and height and construct the viewport and scissors.
    const uint32_t &width = renderLayer_->viewWidth;
    const uint32_t &height = renderLayer_->height;

    // Fill out the XrCompositionLayerProjectionView structure specifying the pose and fov from
    // the view. This also associates the swapchain image with this layer projection view.
    renderLayerInfo.layerProjectionViews[i] = {XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW};
    renderLayerInfo.layerProjectionViews[i].pose = xrView.pose;
    renderLayerInfo.layerProjectionViews[i].fov = xrView.fov;
    renderLayerInfo.layerProjectionViews[i].subImage.swapchain = colorSwapchainInfo.swapchain;
    renderLayerInfo.layerProjectionViews[i].subImage.imageRect.offset.x = i * width;
    renderLayerInfo.layerProjectionViews[i].subImage.imageRect.offset.y = 0;
    renderLayerInfo.layerProjectionViews[i].subImage.imageRect.extent.width =
      static_cast<int32_t>(width);
    renderLayerInfo.layerProjectionViews[i].subImage.imageRect.extent.height =
      static_cast<int32_t>(height);
    renderLayerInfo.layerProjectionViews[i].subImage.imageArrayIndex =
      0;  // Useful for multiview rendering.
  }

  if (spaceWarpHandler_ && spaceWarpHandler_->isSpaceWarpActive()) {
    auto spaceWarpViews = spaceWarpHandler_->mirrorToSwapchain(currentFrame_->views.size());
    for (uint32_t i = 0; i < currentFrame_->views.size(); i++) {
      spaceWarpViews[i].appSpaceDeltaPose = appSpaceDeltaPose_;
      renderLayerInfo.layerProjectionViews[i].next = &spaceWarpViews[i];
    }
  }

  return 0;
}

int XrApiManager::startSession(String mode) {
  glBuffer_ = getXrCommandBuffer();
  if (glBuffer_ == nullptr) {
    C8Log("[xrapi-manager] Failed to get GL Command Buffer");
    return -1;
  }

  XrInstance xrInstance = {};
  std::vector<const char *> activeApiLayers = {};
  std::vector<const char *> activeInstanceExtensions = {};
  std::vector<XrHandController> controllers = {};
  XrSystemId systemId = {};
  std::vector<XrViewConfigurationType> viewConfigurations;
  XrViewConfigurationType viewConfiguration = XR_VIEW_CONFIGURATION_TYPE_MAX_ENUM;
  std::vector<XrViewConfigurationView> viewConfigurationViews;
  std::vector<XrViewConfigurationType> applicationViewConfigurations = {
    XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_MONO};
  std::vector<XrEnvironmentBlendMode> applicationEnvironmentBlendModes = {
    XR_ENVIRONMENT_BLEND_MODE_OPAQUE, XR_ENVIRONMENT_BLEND_MODE_ADDITIVE};
  std::vector<XrEnvironmentBlendMode> environmentBlendModes = {};

  XrSystemProperties systemProperties = {XR_TYPE_SYSTEM_PROPERTIES};

  XrApplicationInfo AI;
  strncpy(AI.applicationName, "OpenXR Tutorial Chapter 2", XR_MAX_APPLICATION_NAME_SIZE);
  AI.applicationVersion = 1;
  strncpy(AI.engineName, "OpenXR Engine", XR_MAX_ENGINE_NAME_SIZE);
  AI.engineVersion = 1;
  AI.apiVersion = XR_API_VERSION_1_0;

  C8Log(
    "[xrapi-manager] Application info: %s %d %s %d %d",
    AI.applicationName,
    AI.applicationVersion,
    AI.engineName,
    AI.engineVersion,
    AI.apiVersion);

  std::vector<std::string> instanceExtensions = {
    XR_EXT_DEBUG_UTILS_EXTENSION_NAME,
    "XR_KHR_opengl_es_enable",
    "XR_FB_composition_layer_image_layout",
    "XR_EXT_hand_tracking"};

  const auto displayRefreshRateExtensions = DisplayRefreshRateFB::requiredExtensionNames();
  instanceExtensions.insert(
    instanceExtensions.end(),
    displayRefreshRateExtensions.begin(),
    displayRefreshRateExtensions.end());

  const auto foveationExtensions = XrApiFoveation::requiredExtensionNames();
  instanceExtensions.insert(
    instanceExtensions.end(), foveationExtensions.begin(), foveationExtensions.end());

  const auto spaceWarpExtensions = XrApiSpaceWarp::requiredExtensionNames();
  instanceExtensions.insert(
    instanceExtensions.end(), spaceWarpExtensions.begin(), spaceWarpExtensions.end());

  const auto performanceSettingsExtensions = ExtPerformanceSettings::requiredExtensionNames();
  instanceExtensions.insert(
    instanceExtensions.end(),
    performanceSettingsExtensions.begin(),
    performanceSettingsExtensions.end());

  uint32_t apiLayerCount = 0;
  std::vector<XrApiLayerProperties> apiLayerProperties;
  checkXr(
    xrEnumerateApiLayerProperties(0, &apiLayerCount, nullptr),
    "[xrapi-manager] Failed to enumerate ApiLayerProperties.");
  apiLayerProperties.resize(apiLayerCount, {XR_TYPE_API_LAYER_PROPERTIES});
  checkXr(
    xrEnumerateApiLayerProperties(apiLayerCount, &apiLayerCount, apiLayerProperties.data()),
    "[xrapi-manager] Failed to enumerate ApiLayerProperties.");

  uint32_t extensionCount = 0;
  std::vector<XrExtensionProperties> extensionProperties;
  checkXr(
    xrEnumerateInstanceExtensionProperties(nullptr, 0, &extensionCount, nullptr),
    "[xrapi-manager] Failed to enumerate InstanceExtensionProperties.");
  extensionProperties.resize(extensionCount, {XR_TYPE_EXTENSION_PROPERTIES});
  checkXr(
    xrEnumerateInstanceExtensionProperties(
      nullptr, extensionCount, &extensionCount, extensionProperties.data()),
    "[xrapi-manager] Failed to enumerate InstanceExtensionProperties.");

  // Check the requested Instance Extensions against the ones from the OpenXR runtime.
  // If an extension is found add it to Active Instance Extensions.
  // Log error if the Instance Extension is not found.
  for (auto &requestedInstanceExtension : instanceExtensions) {
    bool found = false;
    for (auto &extensionProperty : extensionProperties) {
      // strcmp returns 0 if the strings match.
      if (strcmp(requestedInstanceExtension.c_str(), extensionProperty.extensionName) != 0) {
        continue;
      } else {
        activeInstanceExtensions.push_back(requestedInstanceExtension.c_str());
        found = true;
        break;
      }
    }
    if (!found) {
      C8Log(
        "[xrapi-manager] Failed to find OpenXR instance extension: %s",
        requestedInstanceExtension.c_str());
    }
  }

  XrInstanceCreateInfo instanceCI{
    .type = XR_TYPE_INSTANCE_CREATE_INFO,
    .createFlags = 0,
    .applicationInfo = AI,
    .enabledApiLayerCount = static_cast<uint32_t>(activeApiLayers.size()),
    .enabledApiLayerNames = activeApiLayers.data(),
    .enabledExtensionCount = static_cast<uint32_t>(activeInstanceExtensions.size()),
    .enabledExtensionNames = activeInstanceExtensions.data()};
  checkXr(
    glBuffer_->runSyncCommand(xrCreateInstance, &instanceCI, &xrInstance),
    "[xrapi-manager] Failed to create Instance.");

  XrInstanceProperties instanceProperties{XR_TYPE_INSTANCE_PROPERTIES};
  checkXr(
    xrGetInstanceProperties(xrInstance, &instanceProperties),
    "[xrapi-manager] Failed to get InstanceProperties.");

  C8Log(
    "[xrapi-manager] OpenXR Runtime: %s - %d.%d.%d\n",
    instanceProperties.runtimeName,
    XR_VERSION_MAJOR(instanceProperties.runtimeVersion),
    XR_VERSION_MINOR(instanceProperties.runtimeVersion),
    XR_VERSION_PATCH(instanceProperties.runtimeVersion));

  // TODO(lreyna): Get this bool from a feature request from typescript.
  spaceWarpHandler_.emplace();
  spaceWarpHandler_.value().setSpaceWarpAvailableStatus(activeInstanceExtensions);

  if (spaceWarpHandler_->isSpaceWarpAvailable()) {
    systemProperties.next = reinterpret_cast<void *>(&spaceWarpHandler_->getSpaceWarpProperties());
  } else {
    spaceWarpHandler_ = std::nullopt;
  }

  // Get the XrSystemId from the instance and the supplied XrFormFactor.
  XrSystemGetInfo systemGI{XR_TYPE_SYSTEM_GET_INFO};
  systemGI.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
  checkXr(xrGetSystem(xrInstance, &systemGI, &systemId), "[xrapi-manager] Failed to get SystemID.");

  // Get the System's properties for some general information about the hardware and the vendor.
  checkXr(
    xrGetSystemProperties(xrInstance, systemId, &systemProperties),
    "[xrapi-manager] Failed to get SystemProperties.");

  if (spaceWarpHandler_) {
    C8Log(
      "[xrapi-manager] SpaceWarp Properties: recommendedMotionVectorImageRectWidth=%d "
      "recommendedMotionVectorImageRectHeight=%d",
      spaceWarpHandler_->getSpaceWarpProperties().recommendedMotionVectorImageRectWidth,
      spaceWarpHandler_->getSpaceWarpProperties().recommendedMotionVectorImageRectHeight);
  }

  // Get the System's properties for some general information about the hardware and the vendor.
  checkXr(
    xrGetSystemProperties(xrInstance, systemId, &systemProperties),
    "[xrapi-manager] Failed to get SystemProperties a second time.");

  // Gets the View Configuration Types. The first call gets the count of the array that will be
  // returned. The next call fills out the array.
  uint32_t viewConfigurationCount = 0;
  checkXr(
    xrEnumerateViewConfigurations(xrInstance, systemId, 0, &viewConfigurationCount, nullptr),
    "[xrapi-manager] Failed to enumerate View Configurations.");
  viewConfigurations.resize(viewConfigurationCount);
  checkXr(
    xrEnumerateViewConfigurations(
      xrInstance,
      systemId,
      viewConfigurationCount,
      &viewConfigurationCount,
      viewConfigurations.data()),
    "[xrapi-manager] Failed to enumerate View Configurations.");

  // Pick the first application supported View Configuration Type con supported by the hardware.
  for (const XrViewConfigurationType &viewConfigurationOption : applicationViewConfigurations) {
    if (
      std::find(viewConfigurations.begin(), viewConfigurations.end(), viewConfigurationOption)
      != viewConfigurations.end()) {
      viewConfiguration = viewConfigurationOption;
      break;
    }
  }
  if (viewConfiguration == XR_VIEW_CONFIGURATION_TYPE_MAX_ENUM) {
    std::cerr << "[xrapi-manager] Failed to find a view configuration type. Defaulting to "
                 "XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO."
              << std::endl;
    viewConfiguration = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
  }

  // Gets the View Configuration Views. The first call gets the count of the array that will be
  // returned. The next call fills out the array.
  uint32_t viewConfigurationViewCount = 0;
  checkXr(
    xrEnumerateViewConfigurationViews(
      xrInstance, systemId, viewConfiguration, 0, &viewConfigurationViewCount, nullptr),
    "[xrapi-manager] Failed to enumerate ViewConfiguration Views.");
  viewConfigurationViews.resize(viewConfigurationViewCount, {XR_TYPE_VIEW_CONFIGURATION_VIEW});
  checkXr(
    xrEnumerateViewConfigurationViews(
      xrInstance,
      systemId,
      viewConfiguration,
      viewConfigurationViewCount,
      &viewConfigurationViewCount,
      viewConfigurationViews.data()),
    "[xrapi-manager] Failed to enumerate ViewConfiguration Views.");

  // Retrieves the available blend modes. The first call gets the count of the array that will be
  // returned. The next call fills out the array.
  uint32_t environmentBlendModeCount = 0;
  checkXr(
    xrEnumerateEnvironmentBlendModes(
      xrInstance, systemId, viewConfiguration, 0, &environmentBlendModeCount, nullptr),
    "[xrapi-manager] Failed to enumerate EnvironmentBlend Modes.");
  environmentBlendModes.resize(environmentBlendModeCount);
  checkXr(
    xrEnumerateEnvironmentBlendModes(
      xrInstance,
      systemId,
      viewConfiguration,
      environmentBlendModeCount,
      &environmentBlendModeCount,
      environmentBlendModes.data()),
    "[xrapi-manager] Failed to enumerate EnvironmentBlend Modes.");

  checkXr(
    xrGetSystemProperties(xrInstance, systemId, &systemProperties),
    "[xrapi-manager] Failed to get SystemProperties.");

  PFN_xrGetOpenGLESGraphicsRequirementsKHR xrGetOpenGLESGraphicsRequirementsKHR;

  checkXr(
    xrGetInstanceProcAddr(
      xrInstance,
      "xrGetOpenGLESGraphicsRequirementsKHR",
      reinterpret_cast<PFN_xrVoidFunction *>(&xrGetOpenGLESGraphicsRequirementsKHR)),
    "[xrapi-manager] Failed to get proc for xrGetOpenGLESGraphicsRequirementsKHR.");

  XrGraphicsRequirementsOpenGLESKHR glGraphicsRequirements{
    XR_TYPE_GRAPHICS_REQUIREMENTS_OPENGL_ES_KHR};

  xrGetOpenGLESGraphicsRequirementsKHR(xrInstance, systemId, &glGraphicsRequirements);
  displayRefreshRateFB_.emplace(xrInstance);

  session_.emplace(XrApiSession{
    .mode = mode,
    .instance = xrInstance,
    .session = XR_NULL_HANDLE,
    .views = viewConfigurationViews,
    .viewMode = viewConfiguration,
    .state = XR_SESSION_STATE_UNKNOWN,
    .referenceSpace = XR_NULL_HANDLE,
    .controllers = controllers,
  });

  XrSessionCreateInfo sessionCI{
    .type = XR_TYPE_SESSION_CREATE_INFO,
    .next = getGlGraphicsBinding(),
    .createFlags = 0,
    .systemId = systemId};

  checkXr(
    glBuffer_->runSyncCommand(xrCreateSession, xrInstance, &sessionCI, &session_->session),
    "[xrapi-manager] Failed to create Session.");

  // foveationHandler_.emplace(xrInstance, session_->session);
  // foveationHandler_.value().setFoveationActiveStatus(activeInstanceExtensions);
  // if (!foveationHandler_->isFoveationActive()) {
  //   foveationHandler_ = std::nullopt;
  // }

  performanceSettings_.emplace(xrInstance);

  // Default setting is sustained high based on example projects.
  if (performanceSettings_.has_value()) {
    performanceSettings_->setCPUPerformanceLevel(
      session_->session, XR_PERF_SETTINGS_LEVEL_SUSTAINED_HIGH_EXT);
    performanceSettings_->setGPUPerformanceLevel(
      session_->session, XR_PERF_SETTINGS_LEVEL_SUSTAINED_HIGH_EXT);
  }

  registerReferenceSpaces();

  setupInputs();

  pollTimer_.data = this;
  uv_timer_init(loop_, &pollTimer_);
  uv_timer_start(&pollTimer_, &pollTimerCallback, 0, BEFORE_SESSION_POLL_INTERNAL_MS);
  return 0;
}

XrPath XrApiManager::stringToPath(const char *pathString) {
  XrPath xrPath;
  checkXr(
    xrStringToPath(session_->instance, pathString, &xrPath),
    "[xrapi-manager] Failed to create XrPath from string.");
  return xrPath;
}

std::string XrApiManager::pathToString(XrPath path) {
  uint32_t strl;
  char text[XR_MAX_PATH_LENGTH];
  XrResult res;
  res = xrPathToString(session_->instance, path, XR_MAX_PATH_LENGTH, &strl, text);
  std::string str;
  if (res == XR_SUCCESS) {
    str = text;
  } else {
    checkXr(res, "[xrapi-manager] Failed to retrieve path.");
  }
  return str;
}

std::vector<float> XrApiManager::getAvailableRefreshRates() {
  if (!displayRefreshRateFB_.has_value()) {
    C8Log("[xrapi-manager] Unexpected getAvailableRefreshRates, procs not loaded");
    return std::vector<float>();
  }

  if (!session_) {
    C8Log("[xrapi-manager] Unexpected getAvailableRefreshRates, no session");
    return std::vector<float>();
  }

  uint32_t refreshRateValuesCount;
  displayRefreshRateFB_->enumerateDisplayRefreshRatesFB(
    session_->session, 0, &refreshRateValuesCount, NULL);

  std::vector<float> refreshRateValues(refreshRateValuesCount);
  displayRefreshRateFB_->enumerateDisplayRefreshRatesFB(
    session_->session, refreshRateValuesCount, &refreshRateValuesCount, refreshRateValues.data());

  return refreshRateValues;
}

void XrApiManager::trySetRefreshRate(float requestedRefreshRate) {
  if (!displayRefreshRateFB_.has_value()) {
    C8Log("[xrapi-manager] Unexpected getAvailableRefreshRates, procs not loaded");
    return;
  }

  if (!session_) {
    C8Log("[xrapi-manager] Unexpected getAvailableRefreshRates, no session");
    return;
  }

  const auto result =
    displayRefreshRateFB_->requestDisplayRefreshRateFB(session_->session, requestedRefreshRate);
  if (result == XR_ERROR_DISPLAY_REFRESH_RATE_UNSUPPORTED_FB) {
    C8Log("[xrapi-manager] Invalid refresh rate requested: %f", requestedRefreshRate);
  } else if (result != XR_SUCCESS) {
    C8Log("[xrapi-manager] Failed to set refresh rate: %f", requestedRefreshRate);
  }
}

void XrApiManager::setupInputs() {
  auto &controllers_ = session_->controllers;
  const int CONTROLLER_COUNT = 2;

  for (int i = 0; i < CONTROLLER_COUNT; i++) {
    controllers_.emplace_back();

    // NOTE(akashmahesh): index 0 always corresponds to the left hand
    int buttonCount = i == 0 ? 3 : 2;
    controllers_[i].buttonStates.resize(buttonCount);

    if (handTrackingEnabled_) {
      XrHandEXT hand = i == 0 ? XR_HAND_LEFT_EXT : XR_HAND_RIGHT_EXT;

      XrHandTrackerCreateInfoEXT handTrackerCreateInfo{
        .type = XR_TYPE_HAND_TRACKER_CREATE_INFO_EXT,
        .next = nullptr,
        .hand = hand,
        .handJointSet = XR_HAND_JOINT_SET_DEFAULT_EXT};

      PFN_xrCreateHandTrackerEXT pfnCreateHandTrackerEXT;
      checkXr(
        xrGetInstanceProcAddr(
          session_->instance,
          "xrCreateHandTrackerEXT",
          reinterpret_cast<PFN_xrVoidFunction *>(&pfnCreateHandTrackerEXT)),
        "[xrapi-manager] Failed to get proc for xrCreateHandTrackerEXT.");

      checkXr(
        pfnCreateHandTrackerEXT(
          session_->session, &handTrackerCreateInfo, &controllers_[i].handTracker),
        "[xrapi-manager] Failed to Create Hand Tracker.");
    }
  }

  checkXr(
    xrGetInstanceProcAddr(
      session_->instance,
      "xrLocateHandJointsEXT",
      reinterpret_cast<PFN_xrVoidFunction *>(&pfnLocateHandJointsEXT)),
    "[xrapi-manager] Failed to get proc for xrLocateHandJointsEXT.");

  XrActionSetCreateInfo actionSetCI{XR_TYPE_ACTION_SET_CREATE_INFO};
  // The internal name the runtime uses for this Action Set.
  strncpy(actionSetCI.actionSetName, "native-browse-action-set", XR_MAX_ACTION_SET_NAME_SIZE);
  // Localized names are required so there is a human-readable action name to show the user if they
  // are rebinding Actions in an options screen.
  strncpy(
    actionSetCI.localizedActionSetName,
    "Native Browse ActionSet",
    XR_MAX_LOCALIZED_ACTION_SET_NAME_SIZE);

  checkXr(
    xrCreateActionSet(session_->instance, &actionSetCI, &actionSet_),
    "[xrapi-manager] Failed to create ActionSet.");
  // Set a priority: this comes into play when we have multiple Action Sets, and determines which
  // Action takes priority in binding to a specific input.
  actionSetCI.priority = 0;

  auto createAction = [this](
                        XrAction &xrAction,
                        const char *name,
                        XrActionType xrActionType,
                        std::vector<const char *> subactionPaths = {}) -> void {
    // Subaction paths, e.g. left and right hand. To distinguish the same action performed on
    // different devices.
    std::vector<XrPath> subactionXrPaths;
    for (auto p : subactionPaths) {
      subactionXrPaths.push_back(stringToPath(p));
    }
    XrActionCreateInfo actionCI{
      .type = XR_TYPE_ACTION_CREATE_INFO,
      .next = nullptr,
      .actionType = xrActionType,
      .countSubactionPaths = static_cast<uint32_t>(subactionXrPaths.size()),
      .subactionPaths = subactionXrPaths.data(),
    };
    // The internal name the runtime uses for this Action.
    strncpy(actionCI.actionName, name, XR_MAX_ACTION_NAME_SIZE);
    // Localized names are required so there is a human-readable action name to show the user if
    // they are rebinding the Action in an options screen.
    strncpy(actionCI.localizedActionName, name, XR_MAX_LOCALIZED_ACTION_NAME_SIZE);
    checkXr(
      xrCreateAction(actionSet_, &actionCI, &xrAction), "[xrapi-manager] Failed to create Action.");
  };

  createAction(
    selectAction_,
    "controller-select",
    XR_ACTION_TYPE_FLOAT_INPUT,
    {"/user/hand/left", "/user/hand/right"});

  createAction(
    squeezeAction_,
    "controller-squeeze",
    XR_ACTION_TYPE_FLOAT_INPUT,
    {"/user/hand/left", "/user/hand/right"});

  createAction(
    hapticAction_,
    "haptics",
    XR_ACTION_TYPE_VIBRATION_OUTPUT,
    {"/user/hand/left", "/user/hand/right"});

  createAction(xButtonAction_, "controller-x", XR_ACTION_TYPE_BOOLEAN_INPUT, {"/user/hand/left"});
  createAction(yButtonAction_, "controller-y", XR_ACTION_TYPE_BOOLEAN_INPUT, {"/user/hand/left"});
  createAction(
    menuButtonAction_, "controller-menu", XR_ACTION_TYPE_BOOLEAN_INPUT, {"/user/hand/left"});
  createAction(aButtonAction_, "controller-a", XR_ACTION_TYPE_BOOLEAN_INPUT, {"/user/hand/right"});
  createAction(bButtonAction_, "controller-b", XR_ACTION_TYPE_BOOLEAN_INPUT, {"/user/hand/right"});

  createAction(
    xTouchedAction_, "controller-x-touched", XR_ACTION_TYPE_BOOLEAN_INPUT, {"/user/hand/left"});
  createAction(
    yTouchedAction_, "controller-y-touched", XR_ACTION_TYPE_BOOLEAN_INPUT, {"/user/hand/left"});
  createAction(
    aTouchedAction_, "controller-a-touched", XR_ACTION_TYPE_BOOLEAN_INPUT, {"/user/hand/right"});
  createAction(
    bTouchedAction_, "controller-b-touched", XR_ACTION_TYPE_BOOLEAN_INPUT, {"/user/hand/right"});

  createAction(
    thumbstickXAction_,
    "thumbstick-x",
    XR_ACTION_TYPE_FLOAT_INPUT,
    {"/user/hand/left", "/user/hand/right"});

  createAction(
    thumbstickYAction_,
    "thumbstick-y",
    XR_ACTION_TYPE_FLOAT_INPUT,
    {"/user/hand/left", "/user/hand/right"});

  createAction(
    thumbstickClickAction_,
    "thumbstick-click",
    XR_ACTION_TYPE_BOOLEAN_INPUT,
    {"/user/hand/left", "/user/hand/right"});

  // An Action for the position of the palm of the user's hand - appropriate for the location of a
  // grabbing Actions.
  createAction(
    palmPoseAction_,
    "palm-pose",
    XR_ACTION_TYPE_POSE_INPUT,
    {"/user/hand/left", "/user/hand/right"});

  createAction(
    aimPoseAction_, "aim-pose", XR_ACTION_TYPE_POSE_INPUT, {"/user/hand/left", "/user/hand/right"});

  // For later convenience we create the XrPaths for the subaction path names.
  controllers_[0].handPath = stringToPath("/user/hand/left");
  controllers_[1].handPath = stringToPath("/user/hand/right");

  auto suggestBindings =
    [this](const char *profilePath, std::vector<XrActionSuggestedBinding> bindings) -> bool {
    // The application can call xrSuggestInteractionProfileBindings once per interaction profile
    // that it supports.
    XrInteractionProfileSuggestedBinding interactionProfileSuggestedBinding{
      .type = XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING,
      .next = nullptr,
      .interactionProfile = stringToPath(profilePath),
      .countSuggestedBindings = (uint32_t)bindings.size(),
      .suggestedBindings = bindings.data(),
    };
    XrResult res =
      xrSuggestInteractionProfileBindings(session_->instance, &interactionProfileSuggestedBinding);
    if (res == XrResult::XR_SUCCESS)
      return true;
    return false;
  };

  bool any_ok = false;
  // Each Action here has two paths, one for each SubAction path.
  // NOTE(akashmahesh): KHR simple controller is a required fallback binding.
  any_ok |= suggestBindings(
    "/interaction_profiles/khr/simple_controller",
    {{palmPoseAction_, stringToPath("/user/hand/left/input/grip/pose")},
     {palmPoseAction_, stringToPath("/user/hand/right/input/grip/pose")},
     {selectAction_, stringToPath("/user/hand/left/input/select/click")}});
  // Each Action here has two paths, one for each SubAction path.
  any_ok |= suggestBindings(
    "/interaction_profiles/oculus/touch_controller",
    {{palmPoseAction_, stringToPath("/user/hand/left/input/grip/pose")},
     {palmPoseAction_, stringToPath("/user/hand/right/input/grip/pose")},
     {aimPoseAction_, stringToPath("/user/hand/left/input/aim/pose")},
     {aimPoseAction_, stringToPath("/user/hand/right/input/aim/pose")},
     {selectAction_, stringToPath("/user/hand/left/input/trigger/value")},
     {selectAction_, stringToPath("/user/hand/right/input/trigger/value")},
     {squeezeAction_, stringToPath("/user/hand/left/input/squeeze/value")},
     {squeezeAction_, stringToPath("/user/hand/right/input/squeeze/value")},
     {xButtonAction_, stringToPath("/user/hand/left/input/x/click")},
     {yButtonAction_, stringToPath("/user/hand/left/input/y/click")},
     {aButtonAction_, stringToPath("/user/hand/right/input/a/click")},
     {bButtonAction_, stringToPath("/user/hand/right/input/b/click")},
     {xTouchedAction_, stringToPath("/user/hand/left/input/x/touch")},
     {yTouchedAction_, stringToPath("/user/hand/left/input/y/touch")},
     {aTouchedAction_, stringToPath("/user/hand/right/input/a/touch")},
     {bTouchedAction_, stringToPath("/user/hand/right/input/b/touch")},
     {thumbstickXAction_, stringToPath("/user/hand/left/input/thumbstick/x")},
     {thumbstickXAction_, stringToPath("/user/hand/right/input/thumbstick/x")},
     {thumbstickYAction_, stringToPath("/user/hand/left/input/thumbstick/y")},
     {thumbstickYAction_, stringToPath("/user/hand/right/input/thumbstick/y")},
     {thumbstickClickAction_, stringToPath("/user/hand/left/input/thumbstick/click")},
     {thumbstickClickAction_, stringToPath("/user/hand/right/input/thumbstick/click")},
     {hapticAction_, stringToPath("/user/hand/left/output/haptic")},
     {hapticAction_, stringToPath("/user/hand/right/output/haptic")},
     {menuButtonAction_, stringToPath("/user/hand/left/input/menu/click")}});

  if (!any_ok) {
    C8Log("[xrapi-manager] Failed to suggest bindings.");
    DEBUG_BREAK;
  }

  // Create an xrSpace for a pose action.
  auto createActionPoseSpace =
    [this](XrSession session, XrAction xrAction, const char *subactionPath = nullptr) -> XrSpace {
    XrSpace xrSpace;
    const XrPosef xrPoseIdentity = {{0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}};
    // Create frame of reference for a pose action
    XrActionSpaceCreateInfo actionSpaceCI{
      .type = XR_TYPE_ACTION_SPACE_CREATE_INFO,
      .next = nullptr,
      .action = xrAction,
      .subactionPath = subactionPath ? stringToPath(subactionPath) : XR_NULL_PATH,
      .poseInActionSpace = xrPoseIdentity,
    };
    checkXr(
      xrCreateActionSpace(session, &actionSpaceCI, &xrSpace),
      "[xrapi-manager] Failed to create ActionSpace.");
    return xrSpace;
  };
  controllers_[0].handPoseSpace =
    createActionPoseSpace(session_->session, palmPoseAction_, "/user/hand/left");
  controllers_[1].handPoseSpace =
    createActionPoseSpace(session_->session, palmPoseAction_, "/user/hand/right");

  controllers_[0].aimPoseSpace =
    createActionPoseSpace(session_->session, aimPoseAction_, "/user/hand/left");
  controllers_[1].aimPoseSpace =
    createActionPoseSpace(session_->session, aimPoseAction_, "/user/hand/right");

  // Attach the action set we just made to the session. We could attach multiple action sets!
  XrSessionActionSetsAttachInfo actionSetAttachInfo{
    .type = XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO,
    .next = nullptr,
    .countActionSets = 1,
    .actionSets = &actionSet_,
  };
  checkXr(
    xrAttachSessionActionSets(session_->session, &actionSetAttachInfo),
    "[xrapi-manager] Failed to attach ActionSet to Session.");
}

void XrApiManager::recordCurrentBindings() {
  auto &controllers_ = session_->controllers;

  if (session_->session) {
    for (int i = 0; i < controllers_.size(); i++) {
      XrInteractionProfileState interactionProfile = {XR_TYPE_INTERACTION_PROFILE_STATE};

      checkXr(
        xrGetCurrentInteractionProfile(
          session_->session, controllers_[i].handPath, &interactionProfile),
        "[xrapi-manager] Failed to get profile.");

      bool isConnected = interactionProfile.interactionProfile != XR_NULL_PATH;
      std::string currProfile =
        isConnected ? pathToString(interactionProfile.interactionProfile) : "";
      bool controllerReady = currProfile.find("simple_controller") == std::string::npos;
      if (!controllerReady) {
        continue;
      }

      if (isConnected != controllers_[i].connectedLastFrame) {
        XrConnectionEvent connectEvent;
        if (isConnected) {
          connectEvent.connectionState = XrConnectionState::CONNECTED;
        } else {
          connectEvent.connectionState = XrConnectionState::DISCONNECTED;
        }
        connectEvent.handedness = pathToHandedness(controllers_[i].handPath).c_str();
        connectEvent.interactionProfile = currProfile.c_str();
        controllers_[i].connectedLastFrame = isConnected;
        connectEvents_.push_back(connectEvent);
      }
    }
  }
}

void XrApiManager::updateHand(int idx) {
  auto &controllers_ = session_->controllers;

  XrHandJointsLocateInfoEXT handJointsLocateInfo{
    .type = XR_TYPE_HAND_JOINTS_LOCATE_INFO_EXT,
    .next = nullptr,
    .baseSpace = session_->referenceSpace,
    .time = currentFrame_->state.predictedDisplayTime};

  bool isHandTracking = controllers_[idx].jointLocations.isActive;

  checkXr(
    pfnLocateHandJointsEXT(
      controllers_[idx].handTracker, &handJointsLocateInfo, &controllers_[idx].jointLocations),
    "[xrapi-manager] Failed to locate hand joints.");

  if (controllers_[idx].jointLocations.isActive != isHandTracking) {
    XrConnectionEvent connectEvent;
    connectEvent.handedness = pathToHandedness(controllers_[idx].handPath).c_str();
    if (controllers_[idx].jointLocations.isActive) {
      // new hand connection
      connectEvent.connectionState = XrConnectionState::DISCONNECTED;
      connectEvents_.push_back(connectEvent);
      connectEvent.connectionState = XrConnectionState::CONNECTED;
      connectEvent.interactionProfile = "/interaction_profiles/hands";
      connectEvent.isHand = true;
      connectEvents_.push_back(connectEvent);
    } else {
      // new hand disconnection
      connectEvent.connectionState = XrConnectionState::DISCONNECTED;
      connectEvent.interactionProfile = "";
      connectEvents_.push_back(connectEvent);
    }
  }
}

void XrApiManager::registerReferenceSpaces() {
  uint32_t spaceCount;
  xrEnumerateReferenceSpaces(session_->session, 0, &spaceCount, NULL);

  std::vector<XrReferenceSpaceType> availableSpaces(spaceCount);
  xrEnumerateReferenceSpaces(session_->session, spaceCount, &spaceCount, availableSpaces.data());

  // HACK NOTE: for some reason LOCAL_FLOOR is not being enumerated, but it works on Quest.
  availableSpaces.push_back(XR_REFERENCE_SPACE_TYPE_LOCAL_FLOOR);

  // Fill out an XrReferenceSpaceCreateInfo structure for each possible referenceSpace
  for (auto &spaceType : availableSpaces) {
    XrSpace referenceSpace;

    XrReferenceSpaceCreateInfo referenceSpaceCI{
      .type = XR_TYPE_REFERENCE_SPACE_CREATE_INFO,
      .referenceSpaceType = spaceType,
      .poseInReferenceSpace = {
        .orientation = {0.0f, 0.0f, 0.0f, 1.0f}, .position = {0.0f, 0.0f, 0.0f}}};

    checkXr(
      xrCreateReferenceSpace(session_->session, &referenceSpaceCI, &referenceSpace),
      "[xrapi-manager] Failed to create ReferenceSpace.");

    supportedReferenceSpaces_[spaceType] = referenceSpace;
  }

  // Default to local space with an identity pose as the origin.
  session_->referenceSpace = supportedReferenceSpaces_[XR_REFERENCE_SPACE_TYPE_LOCAL];
}

std::string XrApiManager::pathToHandedness(XrPath handPath) {
  if (handPath == session_->controllers[0].handPath) {
    return "left";
  } else if (handPath == session_->controllers[1].handPath) {
    return "right";
  }
  return "unknown";
}

int XrApiManager::createSwapchain(GLuint textureId) {
  TextureAutoRestore textureAutoRestore(glBuffer_);

  // Get the supported swapchain formats as an array of int64_t and ordered by runtime preference.
  uint32_t formatCount = 0;
  checkXr(
    glBuffer_->runSyncCommand(
      xrEnumerateSwapchainFormats, session_->session, 0, &formatCount, nullptr),
    "[xrapi-manager] Failed to enumerate Swapchain Formats");

  std::vector<int64_t> formats(formatCount);
  checkXr(
    glBuffer_->runSyncCommand(
      xrEnumerateSwapchainFormats, session_->session, formatCount, &formatCount, formats.data()),
    "[xrapi-manager] Failed to enumerate Swapchain Formats");

  XrApiSwapchain colorSwapchainInfo;

  if (session_->views.empty()) {
    C8Log("[xrapi-manager] Missing configuration views");
    DEBUG_BREAK;
  }

  uint32_t viewCount = session_->views.size();
  uint32_t viewWidth = session_->views[0].recommendedImageRectWidth;
  uint32_t fullWidth = viewWidth * viewCount;
  uint32_t height = session_->views[0].recommendedImageRectHeight;
  uint32_t sampleCount = session_->views[0].recommendedSwapchainSampleCount;
  auto colorFormat = selectGlColorFormat(formats);

  // Fill out an XrSwapchainCreateInfo structure and create an XrSwapchain.
  // Color.
  XrSwapchainCreateInfo swapchainCI{
    .type = XR_TYPE_SWAPCHAIN_CREATE_INFO,
    .createFlags = true ? 0 : XR_SWAPCHAIN_CREATE_PROTECTED_CONTENT_BIT,
    .usageFlags = XR_SWAPCHAIN_USAGE_TRANSFER_DST_BIT | XR_SWAPCHAIN_USAGE_SAMPLED_BIT
      | XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT,
    .format = colorFormat,
    .sampleCount = sampleCount,
    .width = fullWidth,
    .height = height,
    .faceCount = 1,
    .arraySize = 1,
    .mipCount = 1};

  if (foveationHandler_.has_value()) {
    auto swapchainFovCI = foveationHandler_->getSwapchainCreateInfoFoveation();
    swapchainCI.next = &swapchainFovCI;
  }

  checkXr(
    glBuffer_->runSyncCommand(
      xrCreateSwapchain, session_->session, &swapchainCI, &colorSwapchainInfo.swapchain),
    "[xrapi-manager] Failed to create Color Swapchain");

  // Get the number of images in the color swapchain and allocate Swapchain image data via
  // GraphicsAPI to store the returned array.
  uint32_t colorSwapchainImageCount = 0;
  checkXr(
    glBuffer_->runSyncCommand(
      xrEnumerateSwapchainImages,
      colorSwapchainInfo.swapchain,
      0,
      &colorSwapchainImageCount,
      nullptr),
    "[xrapi-manager] Failed to enumerate Color Swapchain Images.");

  Vector<XrSwapchainImageOpenGLESKHR> colorSwapchainImages(
    colorSwapchainImageCount, {XR_TYPE_SWAPCHAIN_IMAGE_OPENGL_ES_KHR});

  checkXr(
    glBuffer_->runSyncCommand(
      xrEnumerateSwapchainImages,
      colorSwapchainInfo.swapchain,
      colorSwapchainImageCount,
      &colorSwapchainImageCount,
      reinterpret_cast<XrSwapchainImageBaseHeader *>(colorSwapchainImages.data())),
    "[xrapi-manager] Failed to enumerate Color Swapchain Images.");

  if (foveationHandler_.has_value()) {
    foveationHandler_->tryUpdateSwapchainFoveationState(colorSwapchainInfo.swapchain);
  }

  renderLayer_ = XrApiRenderLayer{
    .textureId = textureId,
    .viewCount = viewCount,
    .fullWidth = fullWidth,
    .height = height,
    .viewWidth = viewWidth,
    .fixedFoveation = 0.0f,
    .colorInfo = colorSwapchainInfo,
    .colorSwapchain =
      GlSwapchain{
        glBuffer_,
        textureId,
        fullWidth,
        height,
        colorSwapchainImageCount,
        colorFormat,
        colorSwapchainImages.data()},
  };

  return 0;
}

int XrApiManager::handleStateChange(XrEventDataSessionStateChanged *event) {
  if (!session_) {
    C8Log("[xrapi-manager] Unexpected session state change, no session");
    return -1;
  }

  if (event->session != session_->session) {
    C8Log("[xrapi-manager] XrEventDataSessionStateChanged for unknown session");
    return -1;
  }

  session_->state = event->state;
  if (event->state == XR_SESSION_STATE_READY) {
    // SessionState is ready. Begin the XrSession using the XrViewConfigurationType.
    XrSessionBeginInfo sessionBeginInfo{XR_TYPE_SESSION_BEGIN_INFO};
    sessionBeginInfo.primaryViewConfigurationType = session_->viewMode;
    checkXr(
      xrBeginSession(session_->session, &sessionBeginInfo),
      "[xrapi-manager] Failed to begin Session.");

    // For now, try to default to 72Hz.
    const float desiredRefreshRate = 72.0f;

    trySetRefreshRate(desiredRefreshRate);

    auto waitFrameStatePtr = &waitFrameState_;
    // If session is still the same, there's no need to set it again and reinitalize the async loop.
    if (waitFrameState_.session != session_->session) {
      waitFrameState_.session = session_->session;
      waitFrameState_.manager = this;
      waitFrameState_.processFrameHandle.data = waitFrameStatePtr;

      uv_async_init(loop_, &waitFrameState_.processFrameHandle, asyncFrameCallback);
    }

    waitFrameState_.thread =
      std::thread([waitFrameStatePtr]() { asyncFrameLoop(waitFrameStatePtr); });
  }
  if (event->state == XR_SESSION_STATE_STOPPING) {
    // SessionState is stopping. End the XrSession.
    checkXr(xrEndSession(session_->session), "[xrapi-manager] Failed to end Session.");
    uv_async_send(&waitFrameState_.endFrameHandle);
    waitFrameState_.thread.join();
  }

  return 0;
}

int XrApiManager::handleReferenceStateChange(XrEventDataReferenceSpaceChangePending *event) {
  return 0;
}

std::vector<uint8_t> XrApiManager::tryGetReferenceSpacePose(std::string referenceSpaceType) {
  const auto predictedTime =
    waitFrameState_.frameState.load(std::memory_order_acquire).predictedDisplayTime;

  MutableRootMessage<XrPose> poseMsg;
  const auto writer = poseMsg.builder();

  const auto referenceTypeEnum = spaceTypeStringToEnum(referenceSpaceType);
  XrSpaceLocation spaceLocation{XR_TYPE_SPACE_LOCATION};
  XrResult result;
  if (referenceTypeEnum) {
    result = xrLocateSpace(
      supportedReferenceSpaces_[referenceTypeEnum.value()],
      session_->referenceSpace,
      predictedTime,
      &spaceLocation);
  } else {
    C8Log("[xrapi-manager] Unknown reference space type: %s", referenceSpaceType.c_str());
    return std::vector<uint8_t>();
  }

  if (result != XR_SUCCESS) {
    C8Log("[xrapi-manager] Failed to locate space: %d", result);
    return std::vector<uint8_t>();
  }

  spaceToProtoPose(writer, spaceLocation.pose);

  ConstRootMessage<XrPose> responseData(poseMsg.reader());
  return std::vector<uint8_t>(responseData.bytes().begin(), responseData.bytes().end());
}

int XrApiManager::handleInstanceLossPending(XrEventDataInstanceLossPending *event) {
  C8Log("[xrapi-manager] Instance Loss Pending at: %llu\n", event->lossTime);
  session_->state = XR_SESSION_STATE_STOPPING;
  return 0;
}
int XrApiManager::handleInteractionProfileChanged(XrEventDataInteractionProfileChanged *event) {
  recordCurrentBindings();
  return 0;
}

int XrApiManager::handleEventsLost(XrEventDataEventsLost *event) {
  C8Log("[xrapi-manager] Events Lost: %d\n", event->lostEventCount);
  return 0;
}

void XrApiManager::checkXr(XrResult result, const char *message) {
  if (!XR_SUCCEEDED(result)) {
    C8Log(
      "[xrapi-manager] OPENXR_FAIL: %d (%s) %s\n",
      int(result),
      (session_ ? GetXRErrorString(session_->instance, result) : "<no session>"),
      message);
    DEBUG_BREAK;
  }
}
