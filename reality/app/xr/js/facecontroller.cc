// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#ifdef JAVASCRIPT

#include <emscripten.h>

#include <array>
#include <cstdint>
#include <deque>
#include <memory>

#include "c8/geometry/facemesh-data.h"
#include "c8/geometry/intrinsics.h"
#include "c8/stats/scope-timer.h"
#include "c8/symbol-visibility.h"
#include "reality/engine/api/base/camera-environments.capnp.h"
#include "reality/engine/api/face.capnp.h"
#include "reality/engine/ears/ear-roi-renderer.h"
#include "reality/engine/ears/ear-types.h"
#include "reality/engine/faces/face-detector-local.h"
#include "reality/engine/faces/face-geometry.h"
#include "reality/engine/faces/face-messages.h"
#include "reality/engine/faces/face-roi-renderer.h"
#include "reality/engine/faces/face-roi-shader.h"
#include "reality/engine/faces/face-tracker.h"
#include "reality/engine/geometry/orientation.h"

using namespace c8;

namespace {

struct FacePipelineData {
  uint32_t cameraTextureId = 0;
  int captureWidth = 0;
  int captureHeight = 0;
  int captureRotation = 0;
  ConstRootMessage<FaceResponse> response;
};

struct FaceControllerData {
  FaceTracker tracker;
  std::unique_ptr<FaceRoiRenderer> faceRenderer;
  std::unique_ptr<FaceRoiShader> faceShader;

  std::unique_ptr<EarRoiRenderer> earRenderer;

  ConstRootMessage<CameraEnvironment> env;
  ConstRootMessage<FaceOptions> opts;  // TODO(nb): don't clear options on detach(?)

  std::deque<FacePipelineData> pipeline;
  FacePipelineData active;
  c8_PixelPinholeCameraModel intrinsics;
  std::array<float, 16> projectionMatrix;

  ConstRootMessage<FrameworkResponse> frameworkData;
};

std::unique_ptr<FaceControllerData> &data() {
  static std::unique_ptr<FaceControllerData> d(nullptr);
  if (d == nullptr) {
    d.reset(new FaceControllerData());
  }
  return d;
}

void updateIntrinsics(CameraEnvironment::Reader newEnv, FaceOptions::Reader newOpts) {
  data()->intrinsics = Intrinsics::rotateCropAndScaleIntrinsics(
    Intrinsics::getCameraIntrinsics(DeviceInfos::getDeviceModel(newEnv.getDeviceEstimate())),
    newEnv.getCameraWidth(),
    newEnv.getCameraHeight());
  auto kd = Intrinsics::cropAndScaleIntrinsics(
    data()->intrinsics, newEnv.getDisplayWidth(), newEnv.getDisplayHeight());
  Intrinsics::toClipSpace(
    kd, newOpts.getNearClip(), newOpts.getFarClip(), data()->projectionMatrix.data());
}

void updateVideoGeometry(CameraEnvironment::Reader newEnv) {
  if (
    newEnv.getCameraWidth() == data()->env.reader().getCameraWidth()
    && newEnv.getCameraHeight() == data()->env.reader().getCameraHeight()
    && data()->faceRenderer != nullptr) {
    return;
  }
  data()->faceRenderer.reset(nullptr);
  data()->earRenderer.reset(nullptr);
  if (newEnv.getCameraWidth() == 0 || newEnv.getCameraHeight() == 0) {
    return;
  }

  if (data()->faceShader == nullptr) {
    data()->faceShader.reset(new FaceRoiShader());
    data()->faceShader->initialize();
  }

  data()->faceRenderer.reset(new FaceRoiRenderer());
  data()->faceRenderer->initialize(
    data()->faceShader.get(), newEnv.getCameraWidth(), newEnv.getCameraHeight());
  data()->earRenderer.reset(new EarRoiRenderer());
  data()->earRenderer->initialize(
    data()->faceShader.get(), newEnv.getCameraWidth(), newEnv.getCameraHeight());
}

void loadFaceDetectModel(uint8_t *model, int modelSize) {
  data()->tracker.setFaceDetectModel(model, modelSize);
}

void loadFaceMeshModel(uint8_t *model, int modelSize) {
  data()->tracker.setFaceMeshModel(model, modelSize);
}

void loadEarModel(uint8_t *model, int modelSize) { data()->tracker.setEarModel(model, modelSize); }

void updateCameraEnvironment(CameraEnvironment::Reader newEnv) {
  updateVideoGeometry(newEnv);                      // update video geometry if needed.
  updateIntrinsics(newEnv, data()->opts.reader());  // update intrinsics if needed.
  data()->env = ConstRootMessage<CameraEnvironment>(newEnv);
}

void updateFaceOptions(FaceOptions::Reader newOpts) {
  updateIntrinsics(data()->env.reader(), newOpts);  // update intrinsics if needed.
  data()->opts = ConstRootMessage<FaceOptions>(newOpts);
  data()->tracker.setIsTrackingEars(data()->opts.reader().getEnableEars());
}

void faceControllerCleanup() {
  data()->tracker.reset();
  data()->faceRenderer.reset(nullptr);
  data()->faceShader.reset(nullptr);
  data()->earRenderer.reset(nullptr);
  data()->pipeline.clear();
  data()->active = {};
  data()->frameworkData = {};
}

void stageFaceControllerFrame(int cameraTexture) {
  data()->pipeline.emplace_back();
  auto &p = data()->pipeline.back();
  p.cameraTextureId = cameraTexture;
  p.captureWidth = data()->env.reader().getCameraWidth();
  p.captureHeight = data()->env.reader().getCameraHeight();
  p.captureRotation = data()->env.reader().getOrientation();

  if (data()->faceRenderer == nullptr) {
    updateVideoGeometry(data()->env.reader());
  }

  data()->faceRenderer->draw(p.cameraTextureId, GpuReadPixelsOptions::DEFER_READ);
  data()->earRenderer->draw(p.cameraTextureId, GpuReadPixelsOptions::DEFER_READ);
}

void processStagedFaceControllerFrame() {
  while (data()->pipeline.size() > 2) {
    C8Log("[facecontroller] WARNING: Processing queue is too full, draining old frames.");
    data()->pipeline.pop_front();
  }

  if (data()->pipeline.size() == 2) {
    auto opts = data()->opts.reader();

    // Do computations
    auto faceRenderResult = data()->faceRenderer->result();
    auto trackResult = data()->tracker.track(faceRenderResult, data()->intrinsics, opts.getDebug());
    data()->faceRenderer->setDetectedFaces(*trackResult.globalFaces, *trackResult.localFaces);

    // Construct response.
    MutableRootMessage<FaceResponse> responseMsg;
    auto response = responseMsg.builder();

    if (opts.hasDebug()) {
      auto debug = opts.getDebug();
      if (debug.getExtraOutput().getRenderedImg()) {
        fillRenderedImage(faceRenderResult, response.getDebug());
      }
      if (debug.getExtraOutput().getDetections()) {
        fillDetections(
          *trackResult.globalFaces, *trackResult.localFaces, response.getDebug().getDetections());
      }
    }

    fillFaces(response, *trackResult.faceData);

    if (opts.getEnableEars()) {
      // Get ear results from renderer
      auto earRenderResult = data()->earRenderer->result();
      auto earTrackResult = data()->tracker.trackEars(earRenderResult, data()->intrinsics);

      if (opts.hasDebug()) {
        auto debug = opts.getDebug();
        if (debug.getExtraOutput().getEarRenderedImg()) {
          fillEarRenderedImage(earRenderResult, response.getDebug());
        }
        if (debug.getExtraOutput().getEarDetections()) {
          // Getting visualizations for ear landmarks on the face roi and the camera feed.
          Vector<HPoint3> earInFaceRoi;
          Vector<HPoint3> earInCameraFeed;

          if ((trackResult.localFaces->size() > 0) && (earTrackResult.localEars->size() > 0)) {
            // Get points for detected left ear.
            const auto localEar0 = (*earTrackResult.localEars)[0];

            // Transform matrix from ear to face mesh.
            const HMatrix mEarToFace0 =
              renderTexToRenderTex(localEar0.roi, (*trackResult.localFaces)[0].roi);
            // Transform matrix from ear to image clip space.
            const HMatrix mToImage0 = renderTexToImageTex(localEar0.roi);
            for (size_t i = 2; i < localEar0.points.size(); i++) {
              earInFaceRoi.push_back(mEarToFace0 * localEar0.points[i]);
              earInCameraFeed.push_back(mToImage0 * localEar0.points[i]);
            }

            // Get points for detected right ear.
            const auto localEar1 = (*earTrackResult.localEars)[1];
            // Transform matrix from ear to face mesh.
            const HMatrix mEarToFace1 =
              renderTexToRenderTex(localEar1.roi, (*trackResult.localFaces)[0].roi);
            // Transform matrix from ear to image clip space.
            const HMatrix mToImage1 = renderTexToImageTex(localEar1.roi);
            for (size_t i = 2; i < localEar1.points.size(); i++) {
              earInFaceRoi.push_back(mEarToFace1 * localEar1.points[i]);
              earInCameraFeed.push_back(mToImage1 * localEar1.points[i]);
            }
          }

          fillEarDetections(
            *earTrackResult.localEars,
            earInFaceRoi,
            earInCameraFeed,
            response.getDebug().getDetections());
        }
      }

      // Set the next ROIs from local ears if there are local faces.
      if (!trackResult.localFaces->empty() && !earTrackResult.localEars->empty()) {
        data()->earRenderer->setDetectedEars(*earTrackResult.localEars);
      }

      // Fill results for surfacing to users
      fillEars(response, *earTrackResult.earData);
    }

    // Set the camera field for FaceResponse
    auto gc = response.getCamera().getIntrinsic().initMatrix44f(16);
    auto intrinsics = data()->projectionMatrix;
    for (int i = 0; i < 16; ++i) {
      gc.set(i, intrinsics[i]);
    }

    // by default the capnp Quaternion32f is created with values 0.0w, 0.0x, 0.0y, 0.0z
    response.getCamera().getExtrinsic().getRotation().setW(1.0f);

    // transform the response based on the user's configuration such as coordinate system, scale
    // mirrored display, etc
    rewriteCoordinateSystemPostprocess(opts.getCoordinates(), response);

    // Update pipeline
    data()->active = std::move(data()->pipeline[0]);
    data()->active.response = ConstRootMessage<FaceResponse>(response);
    data()->pipeline.pop_front();
  }

  EM_ASM_(
    {
      window._c8.response = $0;
      window._c8.responseSize = $1;
      window._c8.responseTexture = GL.textures[$2];
    },
    data()->active.response.bytes().begin(),
    data()->active.response.bytes().size(),
    data()->active.cameraTextureId);
}

void getFrameworkData() {
  if (data()->frameworkData.reader().getModelGeometry().getMaxDetections() == 0) {
    MutableRootMessage<FrameworkResponse> m;
    auto b = m.builder().getModelGeometry();
    b.setPointsPerDetection(NUM_FACEMESH_POINTS);

    auto opts = data()->opts.reader();

    b.setMaxDetections(opts.getMaxDetections());
    data()->tracker.setMaxTrackedFaces(opts.getMaxDetections());
    fillIndices(opts, b);

    auto ms = b.initUvs(NUM_FACEMESH_POINTS);

    auto uvs = opts.getUseStandardUvs() ? FACEMESH_UVS : FACEMESH_FLAT_UVS;
    for (int i = 0; i < NUM_FACEMESH_POINTS; ++i) {
      ms[i].setU(uvs[i].u);
      ms[i].setV(uvs[i].v);
    }

    // Apply the user configuration options.  For example, if the user specifies that they are
    // in a right-handed coordinate system, then we should make the indices counter clockwise
    rewriteCoordinateSystemFrameworkPostprocess(opts.getCoordinates(), m.builder());

    // Create a durable flat array
    data()->frameworkData = ConstRootMessage<FrameworkResponse>(m.reader());
  }

  EM_ASM_(
    {
      window._c8.frameworkResponse = $0;
      window._c8.frameworkResponseSize = $1;
    },
    data()->frameworkData.bytes().begin(),
    data()->frameworkData.bytes().size());
}

}  // namespace

extern "C" {

C8_PUBLIC
void c8EmAsm_updateCameraEnvironment(uint8_t *bytes, int numBytes) {
  ConstRootMessage<CameraEnvironment> env(bytes, numBytes);
  updateCameraEnvironment(env.reader());
}

C8_PUBLIC
void c8EmAsm_updateFaceOptions(uint8_t *bytes, int numBytes) {
  ConstRootMessage<FaceOptions> env(bytes, numBytes);
  updateFaceOptions(env.reader());
}

C8_PUBLIC
void c8EmAsm_loadFaceDetectModel(uint8_t *model, int modelSize) {
  loadFaceDetectModel(model, modelSize);
}

C8_PUBLIC
void c8EmAsm_loadFaceMeshModel(uint8_t *model, int modelSize) {
  loadFaceMeshModel(model, modelSize);
}

C8_PUBLIC
void c8EmAsm_loadEarModel(uint8_t *model, int modelSize) { loadEarModel(model, modelSize); }

C8_PUBLIC
void c8EmAsm_faceControllerCleanup() { faceControllerCleanup(); }

// Tick for gpu processing, take a texture and start processing it.
C8_PUBLIC
void c8EmAsm_stageFaceControllerFrame(GLuint cameraTexture) {
  ScopeTimer t("c8EmAsm_stageFaceControllerFrame");
  stageFaceControllerFrame(cameraTexture);
}

// Tock for gpu processing, take a previously staged input, read its
// result and do processing.
C8_PUBLIC
void c8EmAsm_processStagedFaceControllerFrame() {
  ScopeTimer t("c8EmAsm_processStagedFaceControllerFrame");
  processStagedFaceControllerFrame();
}

C8_PUBLIC
void c8EmAsm_getFrameworkData() { getFrameworkData(); }

}  // EXTERN "C"
#else
#warning "xr-js-cc requires --cpu=js"
#endif
