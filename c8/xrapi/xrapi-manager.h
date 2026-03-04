#include <android_native_app_glue.h>
#include <uv.h>

#include <functional>
#include <map>
#include <optional>
#include <thread>
#include <vector>

#include "c8/c8-log.h"
#include "c8/string.h"
#include "c8/xrapi/openxr/display-refresh-rate-fb.h"
#include "c8/xrapi/openxr/ext-performance-settings.h"
#include "c8/xrapi/openxr/gl-graphics.h"
#include "c8/xrapi/openxr/gl-impl.h"
#include "c8/xrapi/openxr/openxr.h"
#include "c8/xrapi/xrapi-foveation.h"
#include "c8/xrapi/xrapi-proto.capnp.h"
#include "c8/xrapi/xrapi-space-warp.h"

XR_DEFINE_HANDLE(XrHandTrackerEXT);

namespace c8 {

struct RenderLayerInfo {
  std::vector<XrCompositionLayerProjectionView> layerProjectionViews;
};

struct ButtonState {
  XrActionStateBoolean clicked = {XR_TYPE_ACTION_STATE_BOOLEAN};
  XrActionStateBoolean touched = {XR_TYPE_ACTION_STATE_BOOLEAN};
};

struct XrHandController {
  XrPath handPath = 0;
  XrSpace handPoseSpace;
  XrSpace aimPoseSpace;
  XrActionStatePose handPoseState = {XR_TYPE_ACTION_STATE_POSE};
  XrActionStatePose aimPoseState = {XR_TYPE_ACTION_STATE_POSE};
  XrActionStateFloat selectState = {XR_TYPE_ACTION_STATE_FLOAT};
  XrActionStateFloat squeezeState = {XR_TYPE_ACTION_STATE_FLOAT};
  XrActionStateFloat thumbstickXState = {XR_TYPE_ACTION_STATE_FLOAT};
  XrActionStateFloat thumbstickYState = {XR_TYPE_ACTION_STATE_FLOAT};
  XrActionStateBoolean thumbstickClickState = {XR_TYPE_ACTION_STATE_BOOLEAN};
  std::vector<ButtonState> buttonStates;
  XrPosef handPose = {{1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.5f}};
  XrPosef aimPose = {{1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.5f}};
  bool connectedLastFrame = false;
  XrHandTrackerEXT handTracker;
  XrHandJointLocationsEXT jointLocations{
    .type = XR_TYPE_HAND_JOINT_LOCATIONS_EXT,
    .next = nullptr,
    .jointCount = XR_HAND_JOINT_COUNT_EXT,
    .jointLocations = new XrHandJointLocationEXT[XR_HAND_JOINT_COUNT_EXT]};
};

struct XrApiSession {
  String mode;
  XrInstance instance = XR_NULL_HANDLE;
  XrSession session = XR_NULL_HANDLE;
  std::vector<XrViewConfigurationView> views = {};
  XrViewConfigurationType viewMode = XR_VIEW_CONFIGURATION_TYPE_MAX_ENUM;
  XrSessionState state;
  XrSpace referenceSpace;
  std::vector<XrHandController> controllers;
};

struct XrApiSwapchain {
  XrSwapchain swapchain = XR_NULL_HANDLE;
};
struct XrApiRenderLayer {
  GLuint textureId;
  size_t viewCount;
  uint32_t fullWidth;
  uint32_t height;
  uint32_t viewWidth;
  float fixedFoveation;
  XrApiSwapchain colorInfo;
  GlSwapchain colorSwapchain;
};

using FrameCallbackData = std::vector<uint8_t>;

using FrameCallback = std::function<void(const FrameCallbackData &)>;

struct XrSessionRequest {
  String mode;
  std::function<void()> readyCallback;
  std::function<void(const FrameCallbackData &)> frameCallback;
};

struct CurrentFrameView {
  XrView xrView;
};

struct CurrentFrame {
  XrFrameState state;
  uint32_t colorIndex;
  std::vector<CurrentFrameView> views;
};

struct XrConnectionEvent {
  XrConnectionState connectionState = XrConnectionState::NOCHANGE;
  String handedness = "";
  String interactionProfile = "";
  bool isHand = false;
};

class XrApiManager;

struct XrApiWaitFrameState {
  XrSession session;
  XrApiManager *manager;
  uv_async_t processFrameHandle;
  std::atomic<XrFrameState> frameState;
  std::thread thread;

  // Initialized and used only within thread
  bool frameThreadActive = true;
  uv_async_t frameDoneHandle;
  uv_async_t endFrameHandle;
};

class XrApiManager {
public:
  XrApiManager();

  bool canRequestSession();

  void requestSession(XrSessionRequest request);

  void endSession();

  void performSetup(struct android_app *android_app, uv_loop_t *loop);

  bool isSessionSupported();

  std::vector<uint8_t> initializeSwapchain(GLuint textureId, bool spaceWarpRequested);

  void setIsSpaceWarpActiveCallback(IsSpaceWarpActiveCallback &&isSpaceWarpActiveCallback);

  void updateHapticState(String handedness, float value, float duration);

  void processFrame(XrFrameState frameState);

  int pollEvents();

  void updateDeltaPose(const XrPosef &pose);

  void updateFixedFoveation(float fixedFoveation);

  void updateRenderState(float depthNear, float depthFar);

  void updateRenderTexture(GLuint textureId);

  void updateSpaceWarpTextures(GLuint motionVectorTextureId, GLuint motionVectorDepthTextureId);

  std::vector<float> getAvailableRefreshRates();

  void trySetRefreshRate(float refreshRate);

  std::vector<uint8_t> tryGetReferenceSpacePose(std::string referenceSpaceType);

private:
  int startSession(String mode);
  int createSwapchain(GLuint textureId);
  int renderFrame(RenderLayerInfo &renderLayerInfo);
  void pollActions(XrTime predictedTime);
  int handleEvent(XrEventDataBuffer *event);
  int handleStateChange(XrEventDataSessionStateChanged *event);
  int handleReferenceStateChange(XrEventDataReferenceSpaceChangePending *event);
  int handleInstanceLossPending(XrEventDataInstanceLossPending *event);
  int handleInteractionProfileChanged(XrEventDataInteractionProfileChanged *event);
  int handleEventsLost(XrEventDataEventsLost *event);
  int beforeRender(XrFrameState &frameState);
  int afterRender();
  void setupInputs();
  void updateHand(int idx);
  void registerReferenceSpaces();
  XrPath stringToPath(const char *pathString);
  String pathToString(XrPath path);
  std::string pathToHandedness(XrPath handPath);
  void recordCurrentBindings();
  int mirrorToSwapchain(RenderLayerInfo &renderLayerInfo);
  void checkXr(XrResult result, const char *errorString);

  std::optional<XrApiSession> session_ = std::nullopt;
  std::optional<XrApiRenderLayer> renderLayer_ = std::nullopt;
  std::optional<CurrentFrame> currentFrame_ = std::nullopt;
  std::vector<XrConnectionEvent> connectEvents_;
  std::map<XrReferenceSpaceType, XrSpace> supportedReferenceSpaces_;

  bool ended_ = false;
  bool didSetup_ = false;
  bool handTrackingEnabled_ = false;
  float depthNear_ = 0.1f;
  float depthFar_ = 1000.0f;
  XrPosef appSpaceDeltaPose_ = {{0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}};

  std::optional<FrameCallback> onFrameData_ = std::nullopt;
  std::optional<DisplayRefreshRateFB> displayRefreshRateFB_ = std::nullopt;
  std::optional<XrApiFoveation> foveationHandler_ = std::nullopt;
  std::optional<XrApiSpaceWarp> spaceWarpHandler_ = std::nullopt;
  std::optional<ExtPerformanceSettings> performanceSettings_ = std::nullopt;

  XrActionSet actionSet_;
  // An action for the controller trigger.
  XrAction selectAction_;
  // The action for getting the hand or controller position and orientation.
  XrAction palmPoseAction_;
  // An action for the controller aim pose.
  XrAction aimPoseAction_;
  // An action for the controller squeeze button
  XrAction squeezeAction_;
  // Actions for the individual buttons on both controllers
  XrAction xButtonAction_, yButtonAction_, aButtonAction_, bButtonAction_;
  XrAction xTouchedAction_, yTouchedAction_, aTouchedAction_, bTouchedAction_;
  XrAction menuButtonAction_;
  // Actions for the thumbstick axes on both controllers
  XrAction thumbstickXAction_, thumbstickYAction_, thumbstickClickAction_;
  // Action for haptic output
  XrAction hapticAction_;

  PFN_xrLocateHandJointsEXT pfnLocateHandJointsEXT;

  uv_loop_t *loop_;
  XrApiWaitFrameState waitFrameState_;
  uv_timer_t pollTimer_;

  GlBuffer *glBuffer_ = nullptr;
};

}  // namespace c8
