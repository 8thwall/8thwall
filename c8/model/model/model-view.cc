// Copyright (c) 2023 Niantic, Inc.
// Original Author: Nicholas Butko (nbutko@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "model-view.h",
  };
  deps = {
    ":crop-box",
    ":model-config",
    ":model-data",
    ":splat-skybox",
    "//c8:c8-log",
    "//c8:hpoint",
    "//c8:quaternion",
    "//c8/camera:device-infos",
    "//c8/geometry:bezier",
    "//c8/geometry:egomotion",
    "//c8/geometry:intrinsics",
    "//c8/geometry:splat",
    "//c8/geometry:vectors",
    "//c8/pixels/render:hit-test",
    "//c8/pixels/render:gesture-detector",
    "//c8/pixels/render:object8",
    "//c8/pixels/render:renderer",
    "//c8/time:now",
    "//c8/time:rolling-framerate",
  };
}
cc_end(0x8323bd2e);

#include "c8/c8-log.h"
#include "c8/camera/device-infos.h"
#include "c8/geometry/egomotion.h"
#include "c8/geometry/intrinsics.h"
#include "c8/geometry/splat.h"
#include "c8/geometry/vectors.h"
#include "c8/model/model-data.h"
#include "c8/model/model-view.h"
#include "c8/model/splat-skybox.h"
#include "c8/pixels/render/hit-test.h"
#include "c8/time/now.h"

namespace c8 {

namespace {
const Color SCANIVERSE_GRAY = Color(63, 63, 63);
constexpr float NORMAL_FOCAL_LENGTH = 28.0f;

float clamp(float x, float min, float max) { return std::max(min, std::min(max, x)); }

float softMin(float x, float min, float margin) {
  if (x < min) {
    auto t = (min - x) / margin;
    return min - t / (t + 1.0f) * margin;
  }
  return x;
}

float softMax(float x, float max, float margin) {
  if (x > max) {
    auto t = (x - max) / margin;
    return max + t / (t + 1.0f) * margin;
  }
  return x;
}

float softClamp(float x, float min, float max, float margin) {
  return softMin(softMax(x, max, margin), min, margin);
}

float clampWithWarning(float val, float min, float max, const String &tag) {
  if (val < min) {
    C8Log("[model-view] WARNING: %s (%f) is less than %f, clamping.", tag.c_str(), val, min);
    return min;
  }
  if (val > max) {
    C8Log("[model-view] WARNING: %s (%f) is greater than %f, clamping.", tag.c_str(), val, max);
    return max;
  }
  return val;
}

bool areAllPointsInFrustum(const Vector<HPoint3> points, const HMatrix &viewProj) {
  auto projected = viewProj * points;
  for (const auto &point : projected) {
    if (point.z() < 0.01f) {
      return false;
    }
    auto x = point.x();
    auto y = point.y();
    if (x < -1.0f || y < -1.0f || x > 1.0f || y > 1.0f) {
      return false;
    }
  }
  return true;
}

HMatrix cameraForOrbitParams(std::array<float, 3> pyd, std::array<float, 3> focalPoint) {
  auto [pitch, yaw, distance] = pyd;
  auto [fx, fy, fz] = focalPoint;
  auto fp = HPoint3{fx, fy, fz};
  HMatrix updatedPosition = updateWorldPosition(
    cameraMotion(fp, {}), Quaternion::fromPitchYawRollDegrees(pitch, yaw, 0.0f).toRotationMat());

  return updateWorldPosition(updatedPosition, HMatrixGen::translation(0.0f, 0.0f, -distance));
}

// Computes orbit parameters from the camera pose and focal point. Removes/does not consider role
// component.
OrbitParams orbitParamsFromCameraPose(const HMatrix &cameraPose, float focalDistance) {
  auto pyrRadians = trsRotation(cameraPose).toPitchYawRollRadians();
  auto pitch = pyrRadians.x();
  auto yaw = pyrRadians.y();
  auto focalPoint =
    trsTranslation(updateWorldPosition(cameraPose, HMatrixGen::translateZ(focalDistance)));

  OrbitParams orbit;
  orbit.pitchRadians = {pitch, pitch, pitch};
  orbit.yawRadians = {yaw, yaw, yaw};
  orbit.distance = {focalDistance, focalDistance, focalDistance};
  orbit.center = HPoint3(focalPoint.x(), focalPoint.y(), focalPoint.z());
  return orbit;
}

std::array<std::array<float, 3>, 2> placeCameraToFitCropBound(
  const Box3 &cropBound, const Camera &camera, float zoom = 1.2f, float pitch = 30.0f) {

  auto lo = cropBound.min;
  auto hi = cropBound.max;
  auto center = cropBound.center();

  Vector<HPoint3> corners = {
    HPoint3(lo.x(), lo.y(), lo.z()),
    HPoint3(lo.x(), lo.y(), hi.z()),
    HPoint3(lo.x(), hi.y(), lo.z()),
    HPoint3(lo.x(), hi.y(), hi.z()),
    HPoint3(hi.x(), lo.y(), lo.z()),
    HPoint3(hi.x(), lo.y(), hi.z()),
    HPoint3(hi.x(), hi.y(), lo.z()),
    HPoint3(hi.x(), hi.y(), hi.z()),
  };

  auto p = camera.projection();

  std::array<float, 3> pyd = {pitch, 0.0f, 0.3f};
  std::array<float, 3> focalPoint = {center.x(), center.y(), center.z()};

  // Step back until the entire scene fits in the frustum.
  // TODO: It would be faster and more accurate to do this analytically.
  while (pyd[2] < 1000.0f) {
    auto cameraMatrix = cameraForOrbitParams(pyd, focalPoint);
    auto viewProj = p * cameraMatrix.inv();

    if (areAllPointsInFrustum(corners, viewProj)) {
      break;
    }
    pyd[2] = std::max(pyd[2] + 0.1f, pyd[2] * 1.05f);
  }
  pyd[2] /= zoom;
  return {pyd, focalPoint};
};

c8_PixelPinholeCameraModel makeIntrinsics(
  int32_t width, int32_t height, float fullFrameFocalLengthMm) {
  // A full frame sensor is 36x24 mm (24mm along the shorter edge).
  const float f = fullFrameFocalLengthMm / 24.0f * std::min(width, height);
  return {width, height, 0.5f * width, 0.5f * height, f, f};
}

}  // namespace

ModelView::ModelView() {
  scene_ = ObGen::scene(300, 150);  // Default, this should be set externally later.
  camera_ =
    &scene_->add(ObGen::perspectiveCamera(makeIntrinsics(300, 150, NORMAL_FOCAL_LENGTH), 300, 150));
  scene_->renderSpec().blitToScreen = true;
  scene_->renderSpec().clearColor = SCANIVERSE_GRAY;
  needsRender_ = true;
};

void ModelView::configure(const ViewParams &config) {
  viewParams_ = config;
  if (modelViewGestureHandler_ != nullptr) {
    modelViewGestureHandler_->configure(viewParams_);
  }
}

std::unique_ptr<Renderable> setSortedTextureData(
  const SplatMetadata &header, Vector<uint8_t> &&interleavedBuffer) {
  std::unique_ptr<Renderable> model = ObGen::splatSortedTexture(header);

  const int stride = 16 * sizeof(uint32_t);  // total channels in bytes
  Vector<AttributeLayout> instanceAttributes = {
    {"positionColor", 4, GpuDataType::UINT, false, stride, 0},
    {"scaleRotation", 4, GpuDataType::UINT, false, stride, 16},
    {"shRG", 4, GpuDataType::UINT, false, stride, 32},
    {"shB", 4, GpuDataType::UINT, false, stride, 48},
  };

  InterleavedBufferData interleavedData = {std::move(interleavedBuffer), instanceAttributes};

  model->geometry().setInterleavedInstanceData(interleavedData);

  return model;
}

void ModelView::setModel(const uint8_t *serializedModel) {
  if (model_ != nullptr) {
    model_->remove();
    model_ = nullptr;
    scene_->renderSpec().clearColor = SCANIVERSE_GRAY;
  }

  modelKind_ = ModelKind::UNSPECIFIED;

  std::unique_ptr<Renderable> model;
  if (hasSplatAttributes(serializedModel)) {
    modelKind_ = ModelKind::SPLAT;
    auto splat = splatAttributesView(serializedModel);

    model = ObGen::splatAttributes(
      splat.positions.size,
      splat.positions.data,
      splat.rotations.data,
      splat.scales.data,
      splat.colors.data);
    scene_->renderSpec().clearColor = Color::TRUE_BLACK;
  } else if (hasSplatTexture(serializedModel)) {
    modelKind_ = ModelKind::SPLAT;
    scene_->renderSpec().clearColor = Color::TRUE_BLACK;

    auto splat = splatTexture(serializedModel);

    if (viewParams_.sortTexture) {
      const auto interleavedBuffer = splat.interleavedAttributeData;
      auto interleavedBufferVector =
        Vector<uint8_t>(interleavedBuffer.data, interleavedBuffer.data + interleavedBuffer.size);
      model = setSortedTextureData(splat.header, std::move(interleavedBufferVector));
    } else {
      RGBA32PlanePixelBuffer texIm(splat.texture.rows(), splat.texture.cols());
      // Row elements is the number of integers per row; In an RGBA32PlanePixels image, it's
      // 4x the number of columns.
      std::memcpy(
        texIm.pixels().pixels(),
        splat.texture.pixels(),
        splat.texture.rows() * splat.texture.rowElements() * sizeof(uint32_t));
      // Splat data is packed into the texIm in a few pixels per splat. Each pixel is 4 integers
      // (16 bytes). See PackedSplat for more information about the packing.

      model = ObGen::splatTextureStacked(splat.header, std::move(texIm), 128);
      ObGen::updateSplatTextureStacked(model.get(), splat.sortedIds.data, splat.sortedIds.size);
    }
    if (splat.header.hasSkybox) {
      model
        ->add(ObGen::positioned(ObGen::cubeMesh(), trsMat({}, Quaternion::xDegrees(180), 700.0f)))
        .setMaterial(MatGen::image())
        .material()
        .setRenderSide(RenderSide::BACK)
        .colorTexture()
        ->setRgbaPixelBuffer(mergeSkybox(splat.skybox));
    }
  } else if (hasSplatMultiTexture(serializedModel)) {
    modelKind_ = ModelKind::SPLAT;
    scene_->renderSpec().clearColor = Color::TRUE_BLACK;

    auto splat = splatMultiTexture(serializedModel);

    model = ObGen::splatMultiTex(splat);
    ObGen::updateSplatMultiTex(model.get(), splat.sortedIds.pixels(), splat.header.numPoints);

    if (splat.header.hasSkybox) {
      model
        ->add(ObGen::positioned(ObGen::cubeMesh(), trsMat({}, Quaternion::xDegrees(180), 700.0f)))
        .setMaterial(MatGen::image())
        .material()
        .setRenderSide(RenderSide::BACK)
        .colorTexture()
        ->setRgbaPixelBuffer(mergeSkybox(splat.skybox));
    }
  } else if (hasMesh(serializedModel)) {
    modelKind_ = ModelKind::MESH;
    auto mesh = meshView(serializedModel);

    model = ObGen::meshGeometry(
      mesh.geometry.points.data,
      mesh.geometry.points.size,
      mesh.geometry.colors.data,
      mesh.geometry.colors.size,
      mesh.geometry.triangles.data,
      mesh.geometry.triangles.size,
      mesh.geometry.normals.data,
      mesh.geometry.normals.size,
      mesh.geometry.uvs.data,
      mesh.geometry.uvs.size);

    model->setMaterial(MatGen::image());
    model->material().setColorTexture(TexGen::rgbaPixelBuffer(mesh.texture));
  } else if (hasPointCloud(serializedModel)) {
    modelKind_ = ModelKind::POINT_CLOUD;
    auto pc = pointCloudView(serializedModel);

    model = ObGen::pointCloud(pc.points.data, pc.points.size, pc.colors.data, pc.colors.size);
    // Default point cloud material is physical, but we only want color only.
    model->setMaterial(MatGen::pointCloudColorOnly());
    model->material().set(Shaders::POINT_CLOUD_POINT_SIZE, 0.005f);  // TODO: Tune
  } else {
    C8Log("WARNING: Model had no data");
    return;
  }

  model_ = &scene_->add(std::move(model));

  needsRender_ = true;

  // Regardless of which mode we start in, initialize Model Mode first so it can be restored
  // properly later.
  modelViewGestureHandler_ = ModelViewGestureHandler::create(gestures_, *camera_, *model_);
  modelViewGestureHandler_->configure(viewParams_);

  if (xrMode_) {
    startXrMode();  // Adjust the camera to be placed for the new model.
    return;
  }

  if (pendingCrop_) {
    startCropMode(*pendingCrop_);
    return;
  }
}

void ModelView::updateModel(const uint8_t *serializedModel) {
  if (hasSplatAttributes(serializedModel)) {
    auto splat = splatAttributesView(serializedModel);
    ObGen::updateSplatAttributes(
      model_,
      splat.positions.size,
      splat.positions.data,
      splat.rotations.data,
      splat.scales.data,
      splat.colors.data);
  } else if (hasSplatTexture(serializedModel) && viewParams_.sortTexture) {
    auto splat = splatTexture(serializedModel);
    const auto interleavedBuffer = splat.interleavedAttributeData;
    auto attributeLayout = model_->geometry().interleavedInstanceData().attributes;
    auto interleavedBufferVector =
      std::vector<uint8_t>(interleavedBuffer.data, interleavedBuffer.data + interleavedBuffer.size);
    model_->geometry().setInterleavedInstanceData(
      {std::move(interleavedBufferVector), attributeLayout});
  } else if (hasSplatTexture(serializedModel)) {
    auto splat = splatTexture(serializedModel);
    ObGen::updateSplatTextureStacked(model_, splat.sortedIds.data, splat.sortedIds.size);
  } else if (hasSplatMultiTexture(serializedModel)) {
    auto splat = splatMultiTexture(serializedModel);
    ObGen::updateSplatMultiTex(model_, splat.sortedIds.pixels(), splat.header.numPoints);
  } else if (hasMesh(serializedModel)) {
    C8Log("Update not implemented for mesh");
  } else {
    C8Log("WARNING: Model had no data");
    return;
  }

  needsRender_ = true;
}

void ModelView::setOrUpdateModel(const uint8_t *serializedModel) {
  if (model_ == nullptr) {
    setModel(serializedModel);
  } else {
    updateModel(serializedModel);
  }
}

HMatrix ModelView::cameraPose() { return camera_->local(); }

void ModelView::updateCameraPose(const HMatrix &cameraPose) {
  if (xrMode_) {
    C8Log("[model-view] WARNING: updateCameraPose called while in XR mode.");
    return;
  }
  camera_->setLocal(cameraPose);
  needsRender_ = true;
  modelViewGestureHandler_->updateOrbitParams(cameraPose);
}

void ModelView::updateCropViewpoint(CropBox::Side viewpoint) {
  if (cropBox_ == nullptr) {
    if (pendingCrop_) {
      // TODO: Set viewpoint after model gets loaded and stop printing this warning.
      C8Log("[model-view] WARNING: updateCropViewpoint was called before setModel completed.");
    } else {
      C8Log("[model-view] WARNING: updateCropViewpoint was called without startCrop.");
    }
    return;
  }
  cropViewGestureHandler_->updateViewpoint(viewpoint);
  cropBox_->viewFrom(viewpoint);
}

void ModelView::updateCropMode(const CropParams &params) {
  if (cropBox_ == nullptr) {
    if (pendingCrop_) {  // startCrop was called before setModel, merge pending params.
      if (params.cropBox) {
        pendingCrop_->cropBox = *params.cropBox;
      }
      if (params.shape) {
        pendingCrop_->shape = *params.shape;
      }
      if (params.rotationDegrees) {
        pendingCrop_->rotationDegrees = *params.rotationDegrees;
      }
    } else {  // updateCrop was called but startCrop wasn't.
      C8Log("[model-view] WARNING: updateCrop called without startCrop.");
    }
    // Either way, we are done.
    return;
  }

  needsRender_ = true;

  if (params.rotationDegrees) {
    auto center = model_->geometry().boundingBox().center();
    auto centerTransform = HMatrixGen::translation(center.x(), center.y(), center.z());
    auto rotatedCentered =
      updateWorldPosition(centerTransform, HMatrixGen::yDegrees(*params.rotationDegrees));
    auto rotatedLocal = updateWorldPosition(rotatedCentered, centerTransform.inv());
    model_->setLocal(rotatedLocal);
    currentRotation_ = *params.rotationDegrees;
  }

  if (params.cropBox) {
    auto cropBounds = *params.cropBox;
    if (
      cropBounds.dimensions().x() == 0 || cropBounds.dimensions().y() == 0
      || cropBounds.dimensions().z() == 0) {
      cropBounds = model_->geometry().boundingBox();
    }
    cropBox_->update(cropBounds);
  }

  if (params.shape) {
    cropBox_->setElliptical(*params.shape == CropParams::Shape::CYLINDER);
  }
}

void ModelView::startCropMode(const CropParams &params) {
  if (xrMode_) {
    C8Log("[model-view] WARNING: startCrop called while in XR mode.");
    return;
  }

  if (model_ == nullptr) {
    if (pendingCrop_) {
      C8Log("[model-view] WARNING: startCrop called before multiple times before model loaded.");
    }

    // Wipe out any previous calls to start/update with the newest params.
    pendingCrop_ = params;
    return;
  }

  pendingCrop_ = {};
  pauseModelMode();

  Box3 cropBounds = model_->geometry().boundingBox();
  if (params.cropBox) {
    cropBounds = *params.cropBox;
  }
  cropBox_ = &scene_->add(std::make_unique<CropBox>(cropBounds, scene_.get(), &renderer_));
  cropViewGestureHandler_ =
    CropViewGestureHandler::create(gestures_, *camera_, *model_, *scene_, *cropBox_);

  updateCropMode(params);
}

CropParams ModelView::finishCropMode() {
  if (cropBox_ == nullptr) {
    if (pendingCrop_) {
      C8Log("[model-view] WARNING: finishCrop was called before model loaded.");
    } else {
      C8Log("[model-view] WARNING: finishCrop was called without startCrop.");
    }
    pendingCrop_ = {};
    return {};
  }

  auto finalRotation = currentRotation_;
  currentRotation_ = 0;
  auto cropBounds = cropBox_->bounds();
  auto shape = cropBox_->elliptical() ? CropParams::Shape::CYLINDER : CropParams::Shape::BOX;
  cropBox_->remove();
  cropBox_ = nullptr;
  cropViewGestureHandler_ = nullptr;
  pendingCrop_ = {};

  resumeModelMode();
  return {cropBounds, finalRotation, shape};
}

void ModelView::pauseModelMode() {
  previousCameraExtrinsics_ = camera_->local();
  previousCameraIntrinsics_ = camera_->projection();
  if (model_ != nullptr) {
    previousModelExtrinsics_ = model_->local();
  }
  modelViewGestureHandler_ = nullptr;
}

void ModelView::startXrMode() {
  if (cropBox_ != nullptr) {
    C8Log("[model-view] WARNING: startXrMode called while in crop mode.");
    return;
  }

  if (modelViewGestureHandler_ != nullptr) {
    pauseModelMode();
  }

  xrMode_ = true;

  // If there's no model yet, wait until there is one before adjusting the model and camera.
  if (model_ == nullptr) {
    return;
  }

  // Hook up gestures to control the model motion (instead of the camera motion).
  xrViewGestureHandler_ = XrViewGestureHandler::create(gestures_, *model_, *camera_);

  // Adjust the model to be at a good viewing spot for the camera, but preserve gravity alignment.
  HMatrix modelLocal = HMatrixGen::i();
  if (modelKind_ != ModelKind::SPLAT) {
    auto [pyd, focalPoint] = placeCameraToFitCropBound(model_->geometry().boundingBox(), *camera_);
    modelLocal = gravityAlignedTr(cameraForOrbitParams(pyd, focalPoint)).inv();
  }
  model_->setLocal(modelLocal);

  camera_->setLocal(HMatrixGen::i());  // Camera starts at the origin.

  // Initialize state to make sure the camera will point at the model once tracking starts.
  hasXrModeOrigin_ = false;
  xrModeOrigin_ = HMatrixGen::i();

  needsRender_ = true;  // Render with the updated camera and model positions.
}

void ModelView::resumeModelMode() {
  camera_->setLocal(previousCameraExtrinsics_);
  camera_->setProjection(previousCameraIntrinsics_);
  previousCameraExtrinsics_ = HMatrixGen::i();
  previousCameraIntrinsics_ = HMatrixGen::i();
  if (model_ != nullptr) {
    model_->setLocal(previousModelExtrinsics_);

    modelViewGestureHandler_ = ModelViewGestureHandler::create(gestures_, *camera_, *model_);
    modelViewGestureHandler_->configure(viewParams_);
  }
  previousModelExtrinsics_ = HMatrixGen::i();
}

void ModelView::finishXrMode() {
  if (!xrMode_) {
    C8Log("[model-view] WARNING: finishXrMode called while not in XR mode.");
    return;
  }

  xrViewGestureHandler_ = nullptr;
  xrMode_ = false;
  xrModeMultiViewSpec_ = {};
  resumeModelMode();
  needsRender_ = true;
}

void ModelView::setCameraPosition(
  const HMatrix &extrinsics, const c8_PixelPinholeCameraModel intrinsics) {
  if (!xrMode_) {
    C8Log("[model-view] WARNING: setCameraPosition called while not in XR mode.");
    return;
  }

  // Tracking systems often start facing a direction other than forward. In order to guarantee that
  // the camera starts off looking at the model, we need to capture the initial orientation of the
  // camera and correct for it (preserving gravity).
  if (!hasXrModeOrigin_ && !isIdentity(extrinsics)) {
    xrModeOrigin_ = gravityAlignedTr(extrinsics);
    hasXrModeOrigin_ = true;
  }

  float sw = scene_->renderSpec().resolutionWidth;
  float sh = scene_->renderSpec().resolutionHeight;
  auto intrinsicsForViewport = Intrinsics::rotateCropAndScaleIntrinsics(intrinsics, sw, sh);

  camera_->setLocal(egomotion(xrModeOrigin_, extrinsics));  // Correct for initial pose.
  camera_->setProjection(Intrinsics::toClipSpaceMatLeftHanded(intrinsicsForViewport, 0.1f, 10.0f));
  needsRender_ = true;
}

void ModelView::setMultiCameraPosition(const MultiViewSpec &spec) {
  xrModeMultiViewSpec_ = spec;
  needsRender_ = true;
}

bool ModelView::setBackQuadSize(const int &textureWidth, const int &textureHeight) {
  // Remove backquad if there's no more image.
  if (scene_->has<Renderable>("backquad") && (textureWidth == 0 || textureHeight == 0)) {
    scene_->find<Renderable>("backquad").remove();
    textureWidth_ = textureWidth;
    textureHeight_ = textureHeight;
    needsRender_ = true;
    return false;
  }

  // Add a backquad if needed.
  if (!scene_->has<Renderable>("backquad")) {
    scene_->add(ObGen::named(ObGen::backQuad(), "backquad")).setMaterial(MatGen::image());
  }

  // If there's nothing to change with scale, return.
  if (textureWidth_ == textureWidth && textureHeight_ == textureHeight) {
    return true;
  }

  textureWidth_ = textureWidth;
  textureHeight_ = textureHeight;

  float sw = scene_->renderSpec().resolutionWidth;
  float sh = scene_->renderSpec().resolutionHeight;

  float sceneRatio = sw / sh;

  float textureRatio = static_cast<float>(textureWidth_) / static_cast<float>(textureHeight);

  float rr = sceneRatio / textureRatio;

  if (rr > 1.0f) {
    // The display is wider than the camera texture, we need stretch the camera vertically.
    scene_->find<Renderable>("backquad").setLocal(HMatrixGen::scale(1.0f, -rr, 1.0f));
  } else {
    // The display is narrower than the camera texture, we need stretch the camera
    // horizontally.
    scene_->find<Renderable>("backquad").setLocal(HMatrixGen::scale(1.0f / rr, -1.0f, 1.0f));
  }
  return true;
}

void ModelView::setBackgroundImagePixels(ConstRGBA8888PlanePixels texturePixels) {
  if (!setBackQuadSize(texturePixels.cols(), texturePixels.rows())) {
    return;
  }

  scene_->find<Renderable>("backquad").material().colorTexture()->setRgbaPixels(texturePixels);
  needsRender_ = true;
}

void ModelView::setBackgroundImage(const int &textureId, const int &width, const int &height) {
  if (!setBackQuadSize(width, height)) {
    return;
  }

  scene_->find<Renderable>("backquad").material().colorTexture()->setNativeId(textureId);
  needsRender_ = true;
}

void ModelView::setBackgroundColor(const uint8_t &r, const uint8_t &g, const uint8_t &b) {
  scene_->renderSpec().clearColor = Color(r, g, b);
  needsRender_ = true;
}

void ModelView::setRenderResolution(int width, int height) {
  if (
    scene_->renderSpec().resolutionWidth == width
    && scene_->renderSpec().resolutionHeight == height) {
    return;
  }
  scene_->renderSpec().resolutionWidth = width;
  scene_->renderSpec().resolutionHeight = height;
  ObGen::updatePerspectiveCamera(
    camera_, makeIntrinsics(width, height, NORMAL_FOCAL_LENGTH), width, height);
  needsRender_ = true;
}

ModelRenderResult ModelView::render(double frameTimeMillis) {
  if (logStats_) {
    renderFramerate_.push();
    static int64_t lastFrameratePrint = nowMicros();
    if (lastFrameratePrint + 1e6 < nowMicros()) {
      C8Log("[model-view] Render Framerate: %0.1f hz", renderFramerate_.fps());
      lastFrameratePrint = nowMicros();
    }
  }

  while (!pendingTouches_.empty()) {
    auto event = pendingTouches_.front();
    pendingTouches_.pop();
    gestures_.observe(event);

    needsRender_ = true;  // TODO: Remove this, probably CropViewGestureHandler needs this?
  }

  if (modelViewGestureHandler_ != nullptr) {
    modelViewGestureHandler_->tick(frameTimeMillis);
  }

  if (cropViewGestureHandler_ != nullptr) {
    cropViewGestureHandler_->tick(frameTimeMillis);
  }

  auto cameraPos = trsTranslation(camera_->world());
  auto cameraRot = trsRotation(camera_->world());

  // TODO: This should be modelView, not camera.
  auto updatedCamera = cameraPos.x() != lastCameraPos_.x() || cameraPos.y() != lastCameraPos_.y()
    || cameraPos.z() != lastCameraPos_.z() || cameraRot.w() != lastCameraRot_.w()
    || cameraRot.x() != lastCameraRot_.x() || cameraRot.y() != lastCameraRot_.y()
    || cameraRot.z() != lastCameraRot_.z();

  if (updatedCamera) {
    lastCameraPos_ = cameraPos;
    lastCameraRot_ = cameraRot;
  }

  needsRender_ |= updatedCamera;

  HVector3 modelPos;
  Quaternion modelRot;
  if (model_ != nullptr) {
    modelPos = trsTranslation(model_->world());
    modelRot = trsRotation(model_->world());
  }

  ModelRenderResult renderState = {
    cameraPos,
    cameraRot,
    modelPos,
    modelRot,
    updatedCamera,
    needsRender_,
  };

  if (!needsRender_) {
    return renderState;
  }

  c8::RenderResult result;
  bool first = true;
  if (!xrModeMultiViewSpec_.views.empty()) {
    auto prevSpec = scene_->renderSpec();
    // Create a new render spec like the old render spec but drawing to the framebuffer and clearing
    // the background, TODO: changing clear color should be optional.
    auto newSpec = prevSpec;
    newSpec.outputFramebuffer = xrModeMultiViewSpec_.framebufferId;
    newSpec.blitToScreen = false;

    newSpec.clearColor = Color::CLEAR;

    // Update resolution to match the full framebuffer.
    newSpec.resolutionWidth = xrModeMultiViewSpec_.framebufferWidth;
    newSpec.resolutionHeight = xrModeMultiViewSpec_.framebufferHeight;

    // Render to each view in the multi-view spec.
    for (const auto &view : xrModeMultiViewSpec_.views) {
      if (first) {
        newSpec.clearBuffers = BufferClear::ALL;
      } else {
        newSpec.clearBuffers = BufferClear::NONE;
      }

      // Update the viewport to match the view.
      auto sw = view.viewportWidth;
      auto sh = view.viewportHeight;
      newSpec.viewport = {
        .x = view.viewportX,
        .y = view.viewportY,
        .w = sw,
        .h = sh,
      };

      scene_->setRenderSpecs({newSpec});

      // Update the camera to match the view.
      auto intrinsics = Intrinsics::rotateCropAndScaleIntrinsics(view.intrinsics, sw, sh);

      // Correct for initial pose
      camera_->setLocal(egomotion(xrModeOrigin_, cameraMotion(view.position, view.orientation)));
      camera_->setProjection(Intrinsics::toClipSpaceMatLeftHanded(intrinsics, view.near, view.far));

      // Render the scene.
      ObGen::updateSplatMultiTexCamera(model_, *camera_);  // TODO: Update once for both eyes.
      result = renderer_.render(*scene_);
      first = false;
    }

    // Reset the previous render specs.
    scene_->setRenderSpecs({prevSpec});
  } else {
    ObGen::updateSplatMultiTexCamera(model_, *camera_);
    result = renderer_.render(*scene_);
  }

  renderState.texId = result.texId;
  renderState.width = result.width;
  renderState.height = result.height;
  if (model_ != nullptr) {
    renderState.modelLoaded = true;
  }
  needsRender_ = false;
  return renderState;
}

void ModelView::gotTouches(const TouchEvent &event) {
  if (logStats_) {
    touchFramerate_.push();
    static int64_t lastFrameratePrint = nowMicros();
    if (lastFrameratePrint + 1e6 < nowMicros()) {
      C8Log("[model-view] Touch Framerate: %0.1f hz", touchFramerate_.fps());
      lastFrameratePrint = nowMicros();
    }
  }

  pendingTouches_.push(event);
}

std::unique_ptr<ModelViewGestureHandler> ModelViewGestureHandler::create(
  GestureDetector &gestureDetector, Camera &camera, Renderable &model) {
  return std::unique_ptr<ModelViewGestureHandler>{
    new ModelViewGestureHandler(gestureDetector, camera, model)};
}

void ModelViewGestureHandler::updateOrbitParams(const HMatrix &cameraPose) {
  HVector3 camPos = translation(cameraPose);
  float distance =
    (HVector3(focalPoint_.get()[0], focalPoint_.get()[1], focalPoint_.get()[2]) - camPos).l2Norm();
  auto newOrbit = orbitParamsFromCameraPose(cameraPose, distance);
  pyd_.set(
    {static_cast<float>(newOrbit.pitchRadians.start * 180.0f / M_PI),
     static_cast<float>(newOrbit.yawRadians.start * 180.0f / M_PI),
     newOrbit.distance.start});
  pydStart_ = pyd_.get();
  focalPointStart_ = focalPoint_.get();
}

float ModelViewGestureHandler::updatePitch(float start, float delta, bool overshoot) {
  if (overshoot) {
    return softClamp(start + delta, minPitchDegrees_, maxPitchDegrees_, 30);
  } else {
    return clamp(start + delta, minPitchDegrees_, maxPitchDegrees_);
  }
}

float ModelViewGestureHandler::updateYaw(float start, float delta, bool overshoot) {
  if (maxYawDegrees_ - minYawDegrees_ > 359.0f) {
    return start + delta;  // No constraints on yaw.
  }
  // Yaw wraps around a circle. Adjust min/max so the midpoint is close to the gesture start yaw.
  auto mid = (minYawDegrees_ + maxYawDegrees_) / 2.0f;
  auto offset = std::round((start - mid) / 360.0f) * 360.0f;
  auto newYaw = start + delta;
  if (overshoot) {
    return softClamp(newYaw, minYawDegrees_ + offset, maxYawDegrees_ + offset, 30.0);
  } else {
    return clamp(start + delta, minYawDegrees_ + offset, maxYawDegrees_ + offset);
  }
}

// Destructor removes this handler from the Gesture detector. The gesture detector should outlive
// the handler.
ModelViewGestureHandler::~ModelViewGestureHandler() {
  detector_->removeListener("onefingermove", &(this->handleOneFingerMove_));
  detector_->removeListener("onefingerstart", &(this->handleOneFingerStart_));
  detector_->removeListener("onefingerend", &(this->handleOneFingerEnd_));
  detector_->removeListener("twofingerstart", &(this->handleTwoFingerStart_));
  detector_->removeListener("twofingermove", &(this->handleTwoFingerMove_));
  detector_->removeListener("twofingerend", &(this->handleTwoFingerEnd_));
  detector_->removeListener("onefingerdoubletap", &(this->handleOneFingerDoubleTap_));
  detector_->removeListener("twofingerdoubletap", &(this->handleTwoFingerDoubleTap_));
}

ModelViewGestureHandler::ModelViewGestureHandler(
  GestureDetector &gestureDetector, Camera &camera, Renderable &model) {
  camera_ = &camera;
  model_ = &model;
  detector_ = &gestureDetector;

  auto globalPosition = translation(model_->world());
  auto modelBox = model_->geometry().boundingBox();

  auto [pyd, focalPoint] = placeCameraToFitCropBound(modelBox, *camera_);

  pyd_.set(pyd);
  focalPoint_.set(
    {globalPosition.x() + focalPoint[0],
     globalPosition.y() + focalPoint[1],
     globalPosition.z() + focalPoint[2]});

  pydStart_ = pyd_.get();
  focalPointStart_ = focalPoint_.get();

  minDistance_ = 0.01f;
  maxDistance_ = pydStart_[2] * 1.2f;
  // Set a better default max for splats. Splat geometry comes from a quad that doesn't contain
  // their points, and gives a normal max distance of about 3. We also don't want to use the whole
  // bounding box of the splat. This gives a reasonable in-between.
  if (!model.geometry().instanceIds().empty()) {
    maxDistance_ *= 10.0f;
  }

  handleOneFingerMove_ = [this](const String &e, const TouchState &t) {
    auto [oldPitch, oldYaw, oldDistance] = pyd_.get();

    auto touchDelta = t.position - t.startPosition;
    float scale = 3.0f * 180.0f / (M_PI * t.normalizedViewSize.x());

    float pitch = updatePitch(dragStartPitch_, touchDelta.y() * scale);
    float yaw = updateYaw(dragStartYaw_, touchDelta.x() * scale);
    ++rotationFrameCount_;

    pyd_.set({pitch, yaw, oldDistance});
  };

  handleOneFingerStart_ = [this](const String &e, const TouchState &t) {
    auto [pitch, yaw, distance] = pyd_.get();
    dragStartPitch_ = pitch;
    dragStartYaw_ = yaw;
    rotationFrameCount_ = 0;
  };

  handleOneFingerEnd_ = [this](const String &e, const TouchState &t) {
    if (rotationFrameCount_ <= 3) {
      return;
    }
    auto [pitch, yaw, distance] = pyd_.get();
    auto touchDelta = t.position - t.startPosition;
    auto v = t.velocity;
    auto vScale = 0.2 * 800.0;
    auto delta = touchDelta + v * vScale;
    float scale = 3.0f * 180.0f / (M_PI * t.normalizedViewSize.x());
    auto newYaw = updateYaw(dragStartYaw_, delta.x() * scale, false);
    auto newPitch = updatePitch(dragStartPitch_, delta.y() * scale, false);
    float newDistance = clamp(distance, minDistance_, maxDistance_);
    pyd_.animate({newPitch, newYaw, newDistance}, BezierAnimation::strongEase(), 800.0);
  };

  handleTwoFingerStart_ = [this](const String &e, const TouchState &t) {
    auto [pitch, yaw, distance] = pyd_.get();
    pinchStartDistance_ = distance;
  };

  handleTwoFingerMove_ = [this](const String &e, const TouchState &t) {
    auto [pitch, yaw, distance] = pyd_.get();
    auto [fx, fy, fz] = focalPoint_.get();
    HPoint3 oldFocalPoint = {fx, fy, fz};

    float dx = t.positionChange.x();
    float dy = -t.positionChange.y();
    float ndx = 2.0f * dx / t.normalizedViewSize.x();
    float ndy = 2.0f * dy / t.normalizedViewSize.y();
    HPoint2 cameraRay = (camera_->projection().inv() * HPoint3{ndx, ndy, 0.0f}).flatten();

    // Update focal position
    auto focalPoint = asPoint(translation(updateWorldPosition(
      updateWorldPosition(
        cameraMotion(oldFocalPoint, {}),
        Quaternion::fromPitchYawRollDegrees(pitch, yaw, 0.0f).toRotationMat()),
      HMatrixGen::translation(-cameraRay.x() * distance, -cameraRay.y() * distance, 0.0f))));

    // Update zoom delta
    auto spreadChange = t.spread / t.startSpread;

    float newDistance =
      softMax(pinchStartDistance_ / spreadChange, maxDistance_, maxDistance_ * 0.2f);

    pyd_.set({pitch, yaw, newDistance});
    focalPoint_.set({focalPoint.x(), focalPoint.y(), focalPoint.z()});
  };

  handleTwoFingerEnd_ = [this](const String &e, const TouchState &t) {
    auto [pitch, yaw, distance] = pyd_.get();
    if (distance < minDistance_ || distance > maxDistance_) {
      pyd_.animate(
        {pitch, yaw, clamp(distance, minDistance_, maxDistance_)},
        BezierAnimation::strongEase(),
        400.0f);
    }
  };

  handleOneFingerDoubleTap_ = [this](const String &e, const TouchState &t) {
    // Check for hits against the model.
    auto hits = hitTestRenderable(*model_, *camera_, t.positionClip);
    if (hits.empty()) {
      return;
    }

    // Sort hits by distance from camera.
    std::sort(hits.begin(), hits.end(), [](const auto &a, const auto &b) {
      return a.clipSpaceZ < b.clipSpaceZ;
    });

    // Find the position of the hit in world space.
    auto clipSpacePosition = HPoint3{t.positionClip.x(), t.positionClip.y(), hits[0].clipSpaceZ};
    auto cameraSpacePosition = camera_->projection().inv() * clipSpacePosition;
    auto hitPosition = camera_->world() * cameraSpacePosition;

    // Determine the vector from the camera to the tap
    auto cameraWorld = translation(camera_->world());
    HVector3 v = {
      hitPosition.x() - cameraWorld.x(),
      hitPosition.y() - cameraWorld.y(),
      hitPosition.z() - cameraWorld.z()};

    // Compute updated pitch and yaw, and move 50% of the distance to the tap
    auto [oldPitch, oldYaw, oldDistance] = pyd_.get();
    float pitch = -std::atan2(v.y(), HVector2{v.x(), -v.z()}.l2Norm()) * 180.0f / M_PI;
    float yaw = pitch < 75.0f ? -std::atan2(-v.x(), v.z()) * 180.0f / M_PI : oldYaw;
    auto zoomedDistance = v.l2Norm() * 0.5f;
    float newDistance = clamp(zoomedDistance, minDistance_, maxDistance_);

    // Save pitch, yaw, and distance for delayed update. This will be applied after the next
    // animation tick. This is needed because `hitTestRenderable` is a slow call, if we start
    // animating now, it will use the time point from the current tick as the start of the
    // animation, and the next tick will be more than a hundred milliseconds later, leading to a
    // large, visually perceivable jump at the start of the next animation.
    hasDelayedUpdate_ = true;
    delayedPyd_ = {pitch, yaw, newDistance};
    delayedFocalPoint_ = {hitPosition.x(), hitPosition.y(), hitPosition.z()};
  };

  handleTwoFingerDoubleTap_ = [this](const String &e, const TouchState &t) {
    pyd_.animate(pydStart_, BezierAnimation::easeInEaseOut(), 800.0);
    focalPoint_.animate(focalPointStart_, BezierAnimation::easeInEaseOut(), 800.0);
  };

  detector_->addListener("onefingermove", &(this->handleOneFingerMove_));
  detector_->addListener("onefingerstart", &(this->handleOneFingerStart_));
  detector_->addListener("onefingerend", &(this->handleOneFingerEnd_));
  detector_->addListener("twofingerstart", &(this->handleTwoFingerStart_));
  detector_->addListener("twofingermove", &(this->handleTwoFingerMove_));
  detector_->addListener("twofingerend", &(this->handleTwoFingerEnd_));
  detector_->addListener("onefingerdoubletap", &(this->handleOneFingerDoubleTap_));
  detector_->addListener("twofingerdoubletap", &(this->handleTwoFingerDoubleTap_));
}

void ModelViewGestureHandler::updateCameraPosition() {
  camera_->setLocal(cameraForOrbitParams(pyd_.get(), focalPoint_.get()));
}

void ModelViewGestureHandler::configure(const ViewParams &viewParams) {
  if (!viewParams.hasOrbit()) {
    return;
  }
  C8Log(
    "[model-view] Orbit Params: yaw (%f, %f, %f), pitch (%f, %f, %f), dist (%f, %f, %f), center "
    "(%f, %f, %f)",
    viewParams.orbit.yawRadians.min,
    viewParams.orbit.yawRadians.start,
    viewParams.orbit.yawRadians.max,
    viewParams.orbit.pitchRadians.min,
    viewParams.orbit.pitchRadians.start,
    viewParams.orbit.pitchRadians.max,
    viewParams.orbit.distance.min,
    viewParams.orbit.distance.start,
    viewParams.orbit.distance.max,
    viewParams.orbit.center.x(),
    viewParams.orbit.center.y(),
    viewParams.orbit.center.z());

  C8Log("Coordinate space: %i", static_cast<int>(viewParams.space));
  float rubMul = viewParams.space == ModelConfig::CoordinateSpace::RUB ? -1.0f : 1.0f;
  focalPoint_.set(
    {viewParams.orbit.center.x(),
     viewParams.orbit.center.y(),
     rubMul * viewParams.orbit.center.z()});

  float minYawDegrees = rubMul * viewParams.orbit.yawRadians.min * 180.0f / M_PI;
  float maxYawDegrees = rubMul * viewParams.orbit.yawRadians.max * 180.0f / M_PI;
  if (rubMul < 0) {
    std::swap(minYawDegrees, maxYawDegrees);
    if (maxYawDegrees < 0) {
      maxYawDegrees += 360;
      minYawDegrees += 360;
    }
  }

  float minPitchDegrees = rubMul * viewParams.orbit.pitchRadians.min * 180.0f / M_PI;
  float maxPitchDegrees = rubMul * viewParams.orbit.pitchRadians.max * 180.0f / M_PI;
  if (rubMul < 0) {
    std::swap(minPitchDegrees, maxPitchDegrees);
  }

  float yawDegrees = rubMul * viewParams.orbit.yawRadians.start * 180.0f / M_PI;
  float pitchDegrees = rubMul * viewParams.orbit.pitchRadians.start * 180.0f / M_PI;

  maxYawDegrees_ = clampWithWarning(maxYawDegrees, 0.0f, 360.0f, "Max yaw degrees");
  minYawDegrees_ = maxYawDegrees_
    - clampWithWarning(maxYawDegrees_ - minYawDegrees, 0.0f, 360.0f, "Yaw range degrees");
  maxPitchDegrees_ = clampWithWarning(maxPitchDegrees, -90.0f, 90.0f, "Max pitch degrees");
  minPitchDegrees_ = clampWithWarning(minPitchDegrees, -90.0f, 90.0f, "Min pitch degrees");
  maxDistance_ = clampWithWarning(viewParams.orbit.distance.max, 0.01f, 1000.0f, "Max distance");
  minDistance_ =
    clampWithWarning(viewParams.orbit.distance.min, 0.01f, maxDistance_, "Min distance");
  auto distance =
    clampWithWarning(viewParams.orbit.distance.start, minDistance_, maxDistance_, "Start distance");

  if (yawDegrees > maxYawDegrees_) {
    yawDegrees -= 360;
  }
  if (yawDegrees < minYawDegrees_) {
    yawDegrees += 360;
  }
  auto yaw = clampWithWarning(yawDegrees, minYawDegrees_, maxYawDegrees_, "Start yaw degrees");
  auto pitch =
    clampWithWarning(pitchDegrees, minPitchDegrees_, maxPitchDegrees_, "Start pitch degrees");

  boundedYaw_ = maxYawDegrees_ - minYawDegrees_ < 359.0f;

  pyd_.set({pitch, yaw, distance});

  pydStart_ = pyd_.get();
  focalPointStart_ = focalPoint_.get();
}

// Handles the stateful animations, should be called every frame
// Velocities are exponentially dampened and boundaries are modelled as springs
void ModelViewGestureHandler::tick(double frameTimeMillis) {
  auto updatedPyd = pyd_.tick(frameTimeMillis);
  auto updatedFocal = focalPoint_.tick(frameTimeMillis);
  if (updatedPyd || updatedFocal) {
    this->updateCameraPosition();
  }

  // Process delayed update after the call to tick(). This will be set after long calls to
  // GestureDetector::observe. If we don't have this delayed update, there will be a visual jump at
  // the start of the animation while we try to catch up from the time of a slow frame.
  if (hasDelayedUpdate_) {
    auto [oldPitch, oldYaw, oldDistance] = pyd_.get();
    auto [pitch, yaw, distance] = delayedPyd_;

    // Adjust current yaw so we will rotate at most 180 degrees to get to the target yaw.
    while (yaw - oldYaw > 180.0f) {
      oldYaw += 360.0f;
    }
    while (yaw - oldYaw < -180.0f) {
      oldYaw -= 360.0f;
    }

    // Keep camera in the same place but adjust yaw by 360 degrees to get to the target yaw.
    pyd_.set({oldPitch, oldYaw, oldDistance});

    pyd_.animateDelayed(delayedPyd_, BezierAnimation::linear(), 800.0f);
    focalPoint_.animateDelayed(delayedFocalPoint_, BezierAnimation::linear(), 800.0f);
    hasDelayedUpdate_ = false;
  }
}

std::unique_ptr<CropViewGestureHandler> CropViewGestureHandler::create(
  GestureDetector &gestureDetector,
  Camera &camera,
  Renderable &model,
  Scene &scene,
  CropBox &cropBox) {
  return std::unique_ptr<CropViewGestureHandler>{
    new CropViewGestureHandler(gestureDetector, camera, model, scene, cropBox)};
}

// Destructor removes this handler from the Gesture detector. The gesture detector should outlive
// the handler.
CropViewGestureHandler::~CropViewGestureHandler() {
  detector_->removeListener("twofingermove", &(this->handleTwoFingerMove_));
  detector_->removeListener("onefingerstart", &(this->handleOneFingerStart_));
  detector_->removeListener("onefingermove", &(this->handleOneFingerMove_));
  detector_->removeListener("onefingerend", &(this->handleOneFingerEnd_));
}

/**
 * @brief Constructs a CropViewGestureHandler object.
 *
 * This class is responsible for handling pinch, scale, and move gestures.
 *
 * @param gestureDetector The gesture detector object.
 * @param camera The camera object.
 * @param model The renderable model object.
 */
CropViewGestureHandler::CropViewGestureHandler(
  GestureDetector &gestureDetector,
  Camera &camera,
  Renderable &model,
  Scene &scene,
  CropBox &cropBox) {
  yaw_ = 0.0f;
  pitch_ = 90.0f;
  distance_ = 1.0f;
  camera_ = &camera;
  model_ = &model;
  scene_ = &scene;
  detector_ = &gestureDetector;
  cropBox_ = &cropBox;

  auto globalPosition = translation(model_->world());
  auto modelBox = model_->geometry().boundingBox();
  auto localCenter = modelBox.center();

  auto maxMax = std::max({modelBox.max.x(), modelBox.max.y(), modelBox.max.z()});
  auto minMin = std::min({modelBox.min.x(), modelBox.min.y(), modelBox.min.z()});
  modelExtent_ = std::max(std::abs(maxMax), std::abs(minMin));

  resolutionWidth_ = scene_->renderSpec().resolutionWidth;
  resolutionHeight_ = scene_->renderSpec().resolutionHeight;
  intrinsics_ = makeIntrinsics(resolutionWidth_, resolutionHeight_, cropFocalLength_);

  // Start the focal point at the center of the model.
  focalPoint_ = {
    globalPosition.x() + localCenter.x(),
    globalPosition.y() + localCenter.y(),
    globalPosition.z() + localCenter.z()};

  // Place the camera at a distance so that the front plane of the model bounding box covers
  // one of the screen dimensions.

  handleOneFingerStart_ = [this](const String &e, const TouchState &t) {
    if (!dragging_) {
      dragging_ = true;
    } else {
      return;
    }

    auto hit = cropBox_->hitTestClipSpace(t.positionClip);
    if (hit >= 0) {
      cornerHit_ = true;
      cornerIndex_ = hit;
    }
  };

  handleOneFingerMove_ = [this](const String &e, const TouchState &t) {
    if (!dragging_ || !cornerHit_) {
      return;
    }

    cropBox_->update(t.positionClip.x(), t.positionClip.y(), cornerIndex_);
  };

  handleOneFingerEnd_ = [this](const String &e, const TouchState &t) {
    dragging_ = false;
    cornerHit_ = false;
    cornerIndex_ = -1;
  };

  handleTwoFingerMove_ = [this](const String &e, const TouchState &t) {
    // Update zoom
    distance_ += t.spreadChange * -fmax(6.0 * abs(distance_), 1.5);

    // Update focal position
    focalPoint_ = asPoint(translation(updateWorldPosition(
      updateWorldPosition(
        cameraMotion(this->focalPoint_, {}),
        Quaternion::fromPitchYawRollDegrees(pitch_, yaw_, 0.0f).toRotationMat()),
      HMatrixGen::translation(
        -t.positionChange.x() * normalisedDistance(),
        t.positionChange.y() * normalisedDistance(),
        0.0f))));

    this->updateCameraPosition();
  };

  detector_->addListener("twofingermove", &(this->handleTwoFingerMove_));
  detector_->addListener("onefingerstart", &(this->handleOneFingerStart_));
  detector_->addListener("onefingermove", &(this->handleOneFingerMove_));
  detector_->addListener("onefingerend", &(this->handleOneFingerEnd_));

  reset();
}

void CropViewGestureHandler::updateCameraPosition() {
  HMatrix updatedPosition = updateWorldPosition(
    cameraMotion(focalPoint_, {}),
    Quaternion::fromPitchYawRollDegrees(pitch_, yaw_, 0.0f).toRotationMat());

  camera_->setLocal(
    updateWorldPosition(updatedPosition, HMatrixGen::translation(0.0f, 0.0f, -distance_)));

  ObGen::updatePerspectiveCamera(
    camera_,
    intrinsics_,
    resolutionWidth_,
    resolutionHeight_,
    std::max(0.1f, distance_ - modelExtent_ * 3.0f),
    std::max(10.0f, distance_ + modelExtent_ * 3.0f));
}

void CropViewGestureHandler::updateViewpoint(CropBox::Side viewpoint) {
  if (viewpoint == CropBox::Side::TOP) {
    yaw_ = 0.0f;
    pitch_ = 90.0f;
  } else if (viewpoint == CropBox::Side::LEFT) {
    yaw_ = 90.0f;
    pitch_ = 0.0f;
  } else if (viewpoint == CropBox::Side::RIGHT) {
    yaw_ = 270.0f;
    pitch_ = 0.0f;
  } else {  // Default to CropBox::Side::FRONT
    yaw_ = 0.0f;
    pitch_ = 0.0f;
  }

  this->updateCameraPosition();
}

void CropViewGestureHandler::reset() {
  auto modelBox = model_->geometry().boundingBox();
  auto maxXY = std::fmax(modelBox.dimensions().x(), modelBox.dimensions().y());
  auto maxEdge = std::fmax(maxXY, modelBox.dimensions().z());
  auto scale = maxEdge * 1.7;

  distance_ = scale * cropFocalLength_ / sensorHeight_;

  this->updateCameraPosition();
}

void CropViewGestureHandler::tick(double frameTimeMillis) { cropBox_->tick(frameTimeMillis); }

std::unique_ptr<XrViewGestureHandler> XrViewGestureHandler::create(
  GestureDetector &gestureDetector, Renderable &model, Camera &camera) {
  return std::unique_ptr<XrViewGestureHandler>{
    new XrViewGestureHandler(gestureDetector, model, camera)};
}

XrViewGestureHandler::XrViewGestureHandler(
  GestureDetector &gestureDetector, Renderable &model, Camera &camera) {
  modelMoveHandler_ = ModelMoveHandler::create(gestureDetector, camera, model);
  twoFingerRotateHandler_ = TwoFingerRotateHandler::create(gestureDetector, model);
  pinchScaleHandler_ = PinchScaleHandler::create(gestureDetector, model);
}

XrViewGestureHandler::~XrViewGestureHandler() {
  modelMoveHandler_ = nullptr;
  twoFingerRotateHandler_ = nullptr;
  pinchScaleHandler_ = nullptr;
}

}  // namespace c8
