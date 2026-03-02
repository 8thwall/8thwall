// Copyright (c) 2022 Niantic, Inc.
// Original Author: Yuyan Song (yuyansong@nianticlabs.com)

#ifdef JAVASCRIPT

#include <emscripten.h>

#include <cstdint>
#include <deque>

#include "c8/geometry/device-pose.h"
#include "c8/geometry/homography.h"
#include "c8/geometry/intrinsics.h"
#include "c8/pixels/pixel-transforms.h"
#include "c8/protolog/xr-requests.h"
#include "c8/stats/scope-timer.h"
#include "c8/symbol-visibility.h"
#include "reality/app/xr/js/xrweb.h"
#include "reality/engine/api/base/camera-environments.capnp.h"
#include "reality/engine/api/base/camera-intrinsics.capnp.h"
#include "reality/engine/api/semantics.capnp.h"
#include "reality/engine/geometry/orientation.h"
#include "reality/engine/semantics/semantics-classifier.h"
#include "reality/engine/semantics/semantics-cubemap-renderer.h"

using namespace c8;

namespace {

struct LayersPipelineData {
  uint32_t cameraTextureId = 0;
  Quaternion devicePoseInXr;
  ConstRootMessage<SemanticsResponse> response;
};

struct LayersControllerData {
  std::unique_ptr<SemanticsCubemapRenderer> renderer;
  c8_PixelPinholeCameraModel intrinsics;
  std::array<float, 16> projectionMatrix;

  RGBA8888PlanePixelBuffer inferenceResultPixels;

  ConstRootMessage<CameraEnvironment> env;
  ConstRootMessage<SemanticsOptions> opts;

  std::deque<LayersPipelineData> pipeline;
  LayersPipelineData active;

  bool hasSetYawOffset = false;
  Quaternion yawOffset;
};

std::unique_ptr<LayersControllerData> &data() {
  static std::unique_ptr<LayersControllerData> d(nullptr);
  if (d == nullptr) {
    d.reset(new LayersControllerData());
  }
  return d;
}

void initSemanticsRenderer() {
  if (data()->renderer == nullptr) {
    data()->renderer.reset(new SemanticsCubemapRenderer());
  }
}

void updateIntrinsics(CameraEnvironment::Reader newEnv, SemanticsOptions::Reader newOpts) {
  int cameraWidth = newEnv.getCameraWidth();
  int cameraHeight = newEnv.getCameraHeight();
  if (
    cameraWidth == 0 || cameraHeight == 0 || data()->renderer == nullptr
    || !newEnv.hasDeviceEstimate()) {
    return;
  }

  data()->renderer->setDeviceOrientation(newEnv.getOrientation());

  data()->intrinsics = Intrinsics::rotateCropAndScaleIntrinsics(
    Intrinsics::getCameraIntrinsics(DeviceInfos::getDeviceModel(newEnv.getDeviceEstimate())),
    cameraWidth,
    cameraHeight);

  // Get projection matrix for rendering engines.
  auto kd = Intrinsics::cropAndScaleIntrinsics(
    data()->intrinsics, newEnv.getDisplayWidth(), newEnv.getDisplayHeight());
  Intrinsics::toClipSpace(
    kd, newOpts.getNearClip(), newOpts.getFarClip(), data()->projectionMatrix.data());

  // Use lower resolution for semantics output rendering
  // TODO(yuyan): set to use the video dimensions when switching to rendering to semantics texture
  const float ratio = 0.3;
  auto w = static_cast<int>(ratio * cameraWidth);
  auto h = static_cast<int>(ratio * cameraHeight);
  data()->renderer->initOutputSemanticsScene(data()->intrinsics, w, h);

  // TODO(yuyan): remove hard coded 144x256 aspect ratio
  int cropW = 0;
  int cropH = 0;
  cropDimensionByAspectRatio(cameraWidth, cameraHeight, 144.0 / 256.0, &cropW, &cropH);
  auto projIntrinsics = Intrinsics::rotateCropAndScaleIntrinsics(data()->intrinsics, cropW, cropH);
  data()->renderer->setProjectionIntrinsics(projIntrinsics);
}

void updateSemanticsCameraEnvironment(CameraEnvironment::Reader newEnv) {
  updateIntrinsics(newEnv, data()->opts.reader());  // update intrinsics if needed.
  data()->env = ConstRootMessage<CameraEnvironment>(newEnv);
}

void updateSemanticsOptions(SemanticsOptions::Reader newOpts) {
  int outWidth = newOpts.getOutputWidth();
  int outHeight = newOpts.getOutputHeight();
  // No need to initialize GL related resources if camera environment or output information is not
  // set properly.
  if (outWidth == 0 || outHeight == 0 || !data()->renderer) {
    return;
  }

  updateIntrinsics(data()->env.reader(), newOpts);  // update intrinsics if needed.
  data()->renderer->initOutputSemanticsScene(data()->intrinsics, outWidth, outHeight);

  data()->renderer->setSkyPostProcessParameters(
    newOpts.getSkyOpts().getEdgeSmoothness(),
    newOpts.getSkyOpts().getHaveSeenSky(),
    newOpts.getSkyOpts().getInvertAlphaMask());

  data()->opts = ConstRootMessage<SemanticsOptions>(newOpts);
}

void semanticsControllerCleanup() {
  data()->renderer.reset(nullptr);
  data()->pipeline.clear();
  data()->active = {};
}

void renderToCubemap(SemanticsInferenceResult::Reader infRes) {
  if (!data()->renderer) {
    return;
  }

  Quaternion32f::Reader qr = infRes.getCamera().getDevicePose();
  Quaternion q(qr.getW(), qr.getX(), qr.getY(), qr.getZ());
  HMatrix mat = xrRotationFromDeviceRotation(q).toRotationMat();

  const int rows = infRes.getImage().getRows();
  const int cols = infRes.getImage().getCols();
  int bytesPerRow = infRes.getImage().getBytesPerRow();
  if (
    data()->inferenceResultPixels.pixels().rows() != rows
    || data()->inferenceResultPixels.pixels().cols() != cols) {
    data()->inferenceResultPixels = RGBA8888PlanePixelBuffer(rows, cols);
  }
  std::memcpy(
    data()->inferenceResultPixels.pixels().pixels(),
    infRes.getImage().getUInt8PixelData().begin(),
    rows * bytesPerRow);

  // RGB channels have the same value per pixel while Alpha channel is 255
  data()->renderer->updateSrcTexture(data()->inferenceResultPixels.pixels());
  data()->renderer->renderToCubemap(mat);
}

void setYawOffset(const Quaternion &rotationInXr) {
  data()->yawOffset = groundPlaneRotation(rotationInXr).inverse();
  data()->hasSetYawOffset = true;
}

void recenter() {
  // Here we clear the yaw offset so in processStagedLayersControllerFrame() we'll set it again.
  data()->hasSetYawOffset = false;
  data()->yawOffset = {};
}

void stageLayersControllerFrame(SemanticsCameraTransformMsg::Reader msg) {
  data()->pipeline.emplace_back();
  auto &p = data()->pipeline.back();
  Quaternion32f::Reader qr = msg.getDevicePose();
  Quaternion q(qr.getW(), qr.getX(), qr.getY(), qr.getZ());
  p.devicePoseInXr = xrRotationFromDeviceRotation(q);
  p.cameraTextureId = msg.getCameraTexture();
}

void processStagedLayersControllerFrame(
  int outputSemanticsTexId, int outputSemanticsTextW, int outputSemanticsTexH) {
  if (!data()->renderer) {
    return;
  }

  while (data()->pipeline.size() > 2) {
    C8Log("[layers-controller] WARNING: Processing queue is too full, draining old frames.");
    data()->pipeline.pop_front();
  }

  if (data()->pipeline.size() == 2) {
    data()->renderer->drawOutputSemantics(
      data()->pipeline[0].devicePoseInXr.toRotationMat(),
      outputSemanticsTexId,
      outputSemanticsTextW,
      outputSemanticsTexH);

    // Construct response.
    MutableRootMessage<SemanticsResponse> responseMsg;
    auto response = responseMsg.builder();

    // Set the camera field for SemanticsResponse
    auto gc = response.getCamera().getIntrinsic().initMatrix44f(16);
    for (int i = 0; i < 16; ++i) {
      gc.set(i, data()->projectionMatrix[i]);
    }

    // TODO(paris): When we support landscape mode check if we should add
    // portraitToDeviceOrientationPostprocess()

    if (!data()->hasSetYawOffset) {
      // Correct the yaw of the extrinsic so that it is facing forward. Let’s say on frame 1 that
      // the rotation has a yaw of +10°. In that case, our yawOffset would offset that by -10° to
      // put our responsiveExtrinsic to 0°. We want the extrinsic to face forward in z so that it
      // can be easily manipulated afterwards by orientation.cc’s recenterAndScale so that it will
      // face the direction specified by the user.
      setYawOffset(data()->pipeline[0].devicePoseInXr);
    }

    auto responsiveExtrinsic = data()->yawOffset.times(data()->pipeline[0].devicePoseInXr);
    setQuaternion(responsiveExtrinsic, response.getCamera().getExtrinsic().getRotation());

    // transform the response based on the user's configuration such as coordinate system, scale
    // mirrored display, etc
    rewriteCoordinateSystemPostprocess(data()->opts.reader().getCoordinates(), response);

    // Update pipeline.
    data()->active = std::move(data()->pipeline[0]);
    data()->active.response = ConstRootMessage<SemanticsResponse>(response);
    data()->pipeline.pop_front();
  }

  EM_ASM_(
    {
      self._c8.response = $0;
      self._c8.responseSize = $1;
      self._c8.responseTexture = GL.textures[$2];
    },
    data()->active.response.bytes().begin(),
    data()->active.response.bytes().size(),
    data()->active.cameraTextureId);
}

}  // namespace

extern "C" {

C8_PUBLIC
void c8EmAsm_initSemanticsRenderer() { initSemanticsRenderer(); }

C8_PUBLIC
void c8EmAsm_updateSemanticsCameraEnvironment(uint8_t *bytes, int numBytes) {
  ConstRootMessage<CameraEnvironment> env(bytes, numBytes);
  updateSemanticsCameraEnvironment(env.reader());
}

C8_PUBLIC
void c8EmAsm_updateSemanticsOptions(uint8_t *bytes, int numBytes) {
  ConstRootMessage<SemanticsOptions> env(bytes, numBytes);
  updateSemanticsOptions(env.reader());
}

C8_PUBLIC
void c8EmAsm_semanticsControllerCleanup() { semanticsControllerCleanup(); }

C8_PUBLIC
void c8EmAsm_semanticsControllerRecenter() {
  C8Log("[layers-controller] recenter");
  recenter();
}

// take semantics inference results and render to cubemap
C8_PUBLIC
void c8EmAsm_semanticsControllerRenderToCubemap(uint8_t *bytes, int numBytes) {
  ScopeTimer t("c8EmAsm_semanticsControllerRenderToCubemap");
  ConstRootMessage<SemanticsInferenceResult> infR(bytes, numBytes);
  renderToCubemap(infR.reader());
}

// Tick for gpu processing: stage camera extrinsic.
C8_PUBLIC
void c8EmAsm_stageLayersControllerFrame(uint8_t *bytes, int numBytes) {
  ScopeTimer t("c8EmAsm_stageLayersControllerFrame");

  ConstRootMessage<SemanticsCameraTransformMsg> camMsg(bytes, numBytes);
  stageLayersControllerFrame(camMsg.reader());
}

// Tock for GPU processing: take a previously staged input, render semantics mask, and do
// processing.
C8_PUBLIC
void c8EmAsm_processStagedLayersControllerFrame(
  int outputSemanticsTexId, int outputSemanticsTextW, int outputSemanticsTexH) {
  ScopeTimer t("c8EmAsm_processStagedLayersControllerFrame");
  processStagedLayersControllerFrame(
    outputSemanticsTexId, outputSemanticsTextW, outputSemanticsTexH);
}

}  // EXTERN "C"
#else
#warning "layers-controller.cc requires --cpu=js"
#endif
