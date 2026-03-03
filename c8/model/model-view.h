// Copyright (c) 2023 Niantic, Inc.
// Original Author: Nicholas Butko (nbutko@nianticlabs.com)

#pragma once

#include <optional>
#include <queue>

#include "c8/geometry/bezier.h"
#include "c8/hpoint.h"
#include "c8/model/crop-box.h"
#include "c8/model/model-config.h"
#include "c8/pixels/render/gesture-detector.h"
#include "c8/pixels/render/object8.h"
#include "c8/pixels/render/renderer.h"
#include "c8/quaternion.h"
#include "c8/time/rolling-framerate.h"

namespace c8 {

struct ModelRenderResult {
  HVector3 cameraPos;
  Quaternion cameraRot;
  HVector3 modelPos;
  Quaternion modelRot;
  bool updatedCamera = false;
  bool rendered = false;
  int texId = -1;
  int width = 0;
  int height = 0;
  bool modelLoaded = false;
};

struct ViewRange {
  float min = 0.0f;
  float start = 0.0f;
  float max = 0.0f;

  bool empty() const { return min == 0.0f && start == 0.0f && max == 0.0f; }
};

struct ViewSpec {
  int viewportX = 0;
  int viewportY = 0;
  int viewportWidth = 0;
  int viewportHeight = 0;
  HPoint3 position;
  Quaternion orientation;
  c8_PixelPinholeCameraModel intrinsics;
  float near = 0.0f;
  float far = 0.0f;
};

struct MultiViewSpec {
  int framebufferWidth = 0;
  int framebufferHeight = 0;
  int framebufferId = 0;
  Vector<ViewSpec> views;
};

struct OrbitParams {
  ViewRange yawRadians;
  ViewRange pitchRadians;
  ViewRange distance;
  HPoint3 center;

  bool hasYawRadians() const { return !yawRadians.empty(); }

  bool hasPitchRadians() const { return !pitchRadians.empty(); }

  bool hasDistance() const { return !distance.empty(); }

  bool hasCenter() const { return center.x() != 0.0f || center.y() != 0.0f || center.z() != 0.0f; }

  bool empty() const {
    return !hasYawRadians() && !hasPitchRadians() && !hasDistance() && !hasCenter();
  }
};

struct ViewParams {
  OrbitParams orbit;
  ModelConfig::CoordinateSpace space = ModelConfig::CoordinateSpace::UNSPECIFIED;
  bool sortTexture = false;
  bool multiTexture = false;
  bool useOctree = false;

  bool hasOrbit() const { return !orbit.empty(); }

  bool hasSpace() const { return space != ModelConfig::CoordinateSpace::UNSPECIFIED; }
};

struct CropParams {
  enum Shape {
    BOX = 0,
    CYLINDER = 1,
  };
  std::optional<Box3> cropBox;
  std::optional<float> rotationDegrees;
  std::optional<Shape> shape;
};

enum class ModelKind {
  UNSPECIFIED = 0,
  POINT_CLOUD = 1,
  MESH = 2,
  SPLAT = 3,
};

// Responsible for handling camera movement along the axis and handling two finger getures to zoom
// and move camera.
class CropViewGestureHandler {
public:
  static std::unique_ptr<CropViewGestureHandler> create(
    GestureDetector &gestureDetector,
    Camera &camera,
    Renderable &model,
    Scene &scene,
    CropBox &cropBox);

  // Destructor removes this handler from the Gesture detector.
  ~CropViewGestureHandler();

  // Disallow copying.
  CropViewGestureHandler(const CropViewGestureHandler &) = delete;
  CropViewGestureHandler &operator=(const CropViewGestureHandler &) = delete;

  void updateViewpoint(CropBox::Side viewpoint);
  void reset();

  void tick(double frameTimeMillis);

private:
  CropViewGestureHandler(
    GestureDetector &gestureDetector,
    Camera &camera,
    Renderable &model,
    Scene &scene,
    CropBox &cropBox);

  // Updates the camera position based on the gestures.
  void updateCameraPosition();

  GestureDetector *detector_;
  Camera *camera_;
  Renderable *model_;
  Scene *scene_;
  CropBox *cropBox_;

  GestureDetector::GestureHandler handleOneFingerMove_;
  GestureDetector::GestureHandler handleOneFingerStart_;
  GestureDetector::GestureHandler handleOneFingerEnd_;
  GestureDetector::GestureHandler handleTwoFingerMove_;

  float normalisedDistance() const { return distance_ / (cropFocalLength_ / sensorHeight_); }

  HPoint3 focalPoint_ = HPoint3{0.0f, 0.0f, 0.0f};
  float yaw_ = 0.0f;
  float pitch_ = 0.0f;
  float distance_ = 0.0f;
  bool dragging_ = false;
  bool cornerHit_ = false;
  int32_t cornerIndex_ = -1;
  float cropFocalLength_ = 16000.0f;
  float sensorHeight_ = 36.0f;
  float modelExtent_ = 0.0f;
  float resolutionWidth_ = 0.0f;
  float resolutionHeight_ = 0.0f;
  c8_PixelPinholeCameraModel intrinsics_;
};

// Moves the model based on single finger gestures
class XrViewGestureHandler {
public:
  static std::unique_ptr<XrViewGestureHandler> create(
    GestureDetector &gestureDetector, Renderable &model, Camera &camera);

  // Destructor removes this handler from the Gesture detector.
  ~XrViewGestureHandler();

  // Disallow copying.
  XrViewGestureHandler(const XrViewGestureHandler &) = delete;
  XrViewGestureHandler &operator=(const XrViewGestureHandler &) = delete;

private:
  XrViewGestureHandler(GestureDetector &gestureDetector, Renderable &model, Camera &camera);

  std::unique_ptr<TwoFingerRotateHandler> twoFingerRotateHandler_;
  std::unique_ptr<PinchScaleHandler> pinchScaleHandler_;
  std::unique_ptr<ModelMoveHandler> modelMoveHandler_;
};

class ModelViewGestureHandler {
public:
  static std::unique_ptr<ModelViewGestureHandler> create(
    GestureDetector &gestureDetector, Camera &camera, Renderable &model);

  // Destructor removes this handler from the Gesture detector. The gesture detector should outlive
  // the handler.
  ~ModelViewGestureHandler();

  void configure(const ViewParams &viewParams);
  void tick(double frameTimeMillis);
  // Default move constructors.
  ModelViewGestureHandler(ModelViewGestureHandler &&) = default;
  ModelViewGestureHandler &operator=(ModelViewGestureHandler &&) = default;

  // Disallow copying.
  ModelViewGestureHandler(const ModelViewGestureHandler &) = delete;
  ModelViewGestureHandler &operator=(const ModelViewGestureHandler &) = delete;

  // Updates orbit params. Removes/does not consider role component.
  void updateOrbitParams(const HMatrix& cameraPose);

private:
  ModelViewGestureHandler(GestureDetector &gestureDetector, Camera &camera, Renderable &model);
  void updateCameraPosition();

  float updatePitch(float start, float delta, bool overshoot = true);
  float updateYaw(float start, float delta, bool overshoot = true);

  GestureDetector *detector_;
  Camera *camera_;
  Renderable *model_;

  GestureDetector::GestureHandler handleOneFingerMove_;
  GestureDetector::GestureHandler handleOneFingerStart_;
  GestureDetector::GestureHandler handleOneFingerEnd_;
  GestureDetector::GestureHandler handleTwoFingerStart_;
  GestureDetector::GestureHandler handleTwoFingerMove_;
  GestureDetector::GestureHandler handleTwoFingerEnd_;
  GestureDetector::GestureHandler handleOneFingerDoubleTap_;
  GestureDetector::GestureHandler handleTwoFingerDoubleTap_;

  AnimationState<float, 3> pyd_;         // pitch, yaw, distance
  AnimationState<float, 3> focalPoint_;  // x, y, z

  std::array<float, 3> pydStart_ = {};
  std::array<float, 3> focalPointStart_ = {};

  bool boundedYaw_ = false;
  float minYawDegrees_ = 0.0f;
  float maxYawDegrees_ = 360.0f;
  float minPitchDegrees_ = -5.0f;
  float maxPitchDegrees_ = 89.9f;
  float minDistance_ = 0.25f;
  float maxDistance_ = 1.0f;

  int rotationFrameCount_ = 0;
  float dragStartYaw_ = 0.0f;
  float dragStartPitch_ = 0.0f;
  float pinchStartDistance_ = 0.0f;

  bool hasDelayedUpdate_ = false;
  std::array<float, 3> delayedPyd_ = {};
  std::array<float, 3> delayedFocalPoint_ = {};
};

class ModelView {
public:
  ModelView();

  // Default constructors.
  ModelView(ModelView &&) = default;
  ModelView &operator=(ModelView &&) = default;

  // Disallow copying.
  ModelView(const ModelView &) = delete;
  ModelView &operator=(const ModelView &) = delete;

  void configure(const ViewParams &viewParams);

  void setModel(const uint8_t *serializedModel);

  void updateModel(const uint8_t *serializedModel);

  void setOrUpdateModel(const uint8_t *serializedModel);

  HMatrix cameraPose();

  // Updates camera location. Removes roll component from the supplied matrix so the camera
  // is facing up. Resets the orbit params for gesture handling to maintain the current
  // focal distance, but moves the focal point to be in front of the supplied camera pose.
  void updateCameraPose(const HMatrix &cameraPose);

  // Crop mode.
  void startCropMode(const CropParams &params);
  void updateCropMode(const CropParams &params);
  void updateCropViewpoint(CropBox::Side viewpoint);  // Set model viewpoint while in Crop Mode.
  CropParams finishCropMode();

  // XR Mode. Switches to exogenous camera movement and gestures that move the model. Use
  // setBackgroundImage while in this mode to draw the camera feed, and use `setBackgroundImage({})`
  // when finished to clear it again.
  void startXrMode();
  void setCameraPosition(const HMatrix &extrinsics, const c8_PixelPinholeCameraModel intrinsics);
  void setMultiCameraPosition(const MultiViewSpec &multiViewSpec);
  void finishXrMode();

  // Sets a background image to render behind the model. If width or height == 0, clears the image.
  // This is typically used in VR mode, but can be used in other modes as well.
  void setBackgroundImage(const int &textureId, const int &width, const int &height);
  void setBackgroundImagePixels(ConstRGBA8888PlanePixels texturePixels);
  // Sets a background color to render behind the model. This is mostly useful for meshes.
  // For splats the recommended background color is black.
  void setBackgroundColor(const uint8_t &r, const uint8_t &g, const uint8_t &b);

  void setRenderResolution(int width, int height);

  ModelRenderResult render(double frameTimeMillis);

  void gotTouches(const TouchEvent &event);

  void logStats(bool doLog) { logStats_ = doLog; }

private:
  void pauseModelMode();
  void resumeModelMode();

  // Scene
  Renderer renderer_;
  std::unique_ptr<Scene> scene_;
  CropBox *cropBox_ = nullptr;
  Renderable *model_ = nullptr;
  Camera *camera_ = nullptr;
  ModelKind modelKind_ = ModelKind::UNSPECIFIED;

  // XR
  bool xrMode_ = false;
  int textureWidth_ = 0;
  int textureHeight_ = 0;
  bool hasXrModeOrigin_ = false;
  HMatrix xrModeOrigin_ = HMatrixGen::i();
  MultiViewSpec xrModeMultiViewSpec_;
  HMatrix previousModelExtrinsics_ = HMatrixGen::i();
  HMatrix previousCameraExtrinsics_ = HMatrixGen::i();
  HMatrix previousCameraIntrinsics_ = HMatrixGen::i();
  bool setBackQuadSize(const int &textureWidth, const int &textureHeight);

  // Configuration
  ViewParams viewParams_;

  // Gestures
  GestureDetector gestures_;
  std::unique_ptr<ModelViewGestureHandler> modelViewGestureHandler_;
  std::unique_ptr<CropViewGestureHandler> cropViewGestureHandler_;
  std::unique_ptr<XrViewGestureHandler> xrViewGestureHandler_;

  // Render state
  bool needsRender_ = false;
  HVector3 lastCameraPos_;
  Quaternion lastCameraRot_;

  // Stats
  bool logStats_ = false;
  RollingFramerate renderFramerate_{static_cast<int64_t>(2e6)};
  RollingFramerate touchFramerate_{static_cast<int64_t>(2e6)};

  std::optional<CropParams> pendingCrop_;
  int currentRotation_ = 0;

  std::queue<TouchEvent> pendingTouches_;
};

}  // namespace c8
