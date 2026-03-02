// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "xr-requests.h",
  };
  deps = {
    "//c8:hpoint",
    "//c8:hvector",
    "//c8:quaternion",
    "//c8/geometry:pixel-pinhole-camera-model",
    "//c8/pixels:gr8-pyramid",
    "//c8/pixels:image-roi",
    "//c8/pixels:pixels",
    "//c8/protolog:xr-extern",
    "//c8/stats:scope-timer",
    "//reality/engine/api:reality.capnp-cc",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x2c92d8be);

#include <capnp/message.h>

#include "c8/protolog/xr-requests.h"
#include "c8/stats/scope-timer.h"

namespace c8 {

namespace {

bool hasDepthMapData(RequestARKit::Reader arkit) {
  return arkit.getDepthMap().getRows() > 0 && arkit.getDepthMap().getCols() > 0;
}

DepthFloatPixelBuffer depthMapPixelBuffer(RequestARKit::Reader arkit) {
  auto depthMap = arkit.getDepthMap();
  int rows = depthMap.getRows();
  int cols = depthMap.getCols();

  DepthFloatPixelBuffer output{rows, cols};
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      output.pixels().pixels()[i * cols + j] = *(depthMap.getPixelData().begin() + i * cols + j);
    }
  }

  return output;
}

bool hasDepthMapData(RequestARCore::Reader arcore) {
  return arcore.getDepthMap().getRows() > 0 && arcore.getDepthMap().getCols() > 0;
}

// Rotate the landscape dapth buffer and translate from uint16_t millimeters to float32 meters.
DepthFloatPixelBuffer depthMapPixelBuffer(RequestARCore::Reader arcore) {
  auto depthMap = arcore.getDepthMap();
  int rows = depthMap.getRows();
  int cols = depthMap.getCols();

  DepthFloatPixelBuffer output{cols, rows};

  // Rotate the 16-bit landscape image buffer into a portrait float image, converting millimeters to
  // meters.
  // TODO(nb): Implement loop unrolling if performance is a concern here.
  // TODO(nb): handle landscape mode capture, e.g. possibly no need to rotate.
  const int srcHeight = rows;
  const int srcWidth = cols;
  const int srcStride = cols;
  const uint16_t *srcBuf =
    reinterpret_cast<const uint16_t *>(depthMap.getUInt16PixelData().begin());
  const int destStride = output.pixels().rowElements();
  float *destBuf = output.pixels().pixels();
  int srcRowStart = 0;
  int destCol = srcHeight - 1;

  for (int sourceRow = 0; sourceRow < srcHeight; ++sourceRow) {
    const uint16_t *srcPix = srcBuf + srcRowStart;
    float *destPix = destBuf + destCol;

    for (int sourceCol = 0; sourceCol < srcWidth; ++sourceCol) {
      *destPix = *srcPix * 0.001f;  // Translate uint16_t millimeters to float32 meters.
      srcPix++;
      destPix += destStride;
    }

    destCol--;
    srcRowStart += srcStride;
  }

  return output;
}

}  // namespace

void setXRConfigurationLegacy(
  const c8_XRConfigurationLegacy &xrConfig, XRConfiguration::Builder *requestConfig) {
  requestConfig->setMask(capnp::defaultValue<XRRequestMask>());
  requestConfig->getMask().setLighting(xrConfig.maskLighting);
  requestConfig->getMask().setCamera(xrConfig.maskCamera);
  requestConfig->getMask().setSurfaces(xrConfig.maskSurfaces);
  requestConfig->getMask().setVerticalSurfaces(xrConfig.maskVerticalSurfaces);

  requestConfig->setGraphicsIntrinsics(capnp::defaultValue<GraphicsPinholeCameraModel>());
  auto graphicsIntrinsics = requestConfig->getGraphicsIntrinsics();
  graphicsIntrinsics.setTextureWidth(xrConfig.graphicsIntrinsicsTextureWidth);
  graphicsIntrinsics.setTextureHeight(xrConfig.graphicsIntrinsicsTextureHeight);
  graphicsIntrinsics.setNearClip(xrConfig.graphicsIntrinsicsNearClip);
  graphicsIntrinsics.setFarClip(xrConfig.graphicsIntrinsicsFarClip);
  graphicsIntrinsics.setDigitalZoomHorizontal(xrConfig.graphicsIntrinsicsDigitalZoomHorizontal);
  graphicsIntrinsics.setDigitalZoomVertical(xrConfig.graphicsIntrinsicsDigitalZoomVertical);

  if (xrConfig.mobileAppKey) {
    requestConfig->setMobileAppKey(xrConfig.mobileAppKey);
  }
  requestConfig->getCameraConfiguration().setAutofocus(xrConfig.autofocus);

  requestConfig->getCameraConfiguration().setDepthMapping(xrConfig.depthMapping);
}

void setCameraPixelPointers(
  ConstYPlanePixels srcY, ConstUVPlanePixels srcUV, RequestCamera::Builder *cameraBuilder) {

  // Create a reference to the frame pixels that can be used by the reality engine.
  auto image = cameraBuilder->getCurrentFrame().getImage();
  image.getOneOf().getGrayImagePointer().setRows(srcY.rows());
  image.getOneOf().getGrayImagePointer().setCols(srcY.cols());
  image.getOneOf().getGrayImagePointer().setBytesPerRow(srcY.rowBytes());
  image.getOneOf().getGrayImagePointer().setUInt8PixelDataPointer(
    reinterpret_cast<size_t>(srcY.pixels()));

  auto uvImage = cameraBuilder->getCurrentFrame().getUvImage();
  uvImage.getOneOf().getGrayImagePointer().setRows(srcUV.rows());
  uvImage.getOneOf().getGrayImagePointer().setCols(srcUV.cols());
  uvImage.getOneOf().getGrayImagePointer().setBytesPerRow(srcUV.rowBytes());
  uvImage.getOneOf().getGrayImagePointer().setUInt8PixelDataPointer(
    reinterpret_cast<size_t>(srcUV.pixels()));
}

void setRawYImageData(ConstYPlanePixels yPixels, GrayImageData::Builder imageBuilder) {
  imageBuilder.setRows(yPixels.rows());
  imageBuilder.setCols(yPixels.cols());
  imageBuilder.setBytesPerRow(yPixels.rowBytes());
  imageBuilder.initUInt8PixelData(yPixels.rows() * yPixels.rowBytes());
  std::memcpy(
    imageBuilder.getUInt8PixelData().begin(),
    yPixels.pixels(),
    yPixels.rows() * yPixels.rowBytes());
}

void setRawUvImageData(ConstUVPlanePixels uvPixels, GrayImageData::Builder imageBuilder) {
  imageBuilder.setRows(uvPixels.rows());
  imageBuilder.setCols(uvPixels.cols());
  imageBuilder.setBytesPerRow(uvPixels.rowBytes());
  imageBuilder.initUInt8PixelData(uvPixels.rows() * uvPixels.rowBytes());
  std::memcpy(
    imageBuilder.getUInt8PixelData().begin(),
    uvPixels.pixels(),
    uvPixels.rows() * uvPixels.rowBytes());
}

void setDepthMap(ConstDepthFloatPixels srcDepth, RequestARKit::Builder *arkitBuilder) {
  FloatImageData::Builder depthMapBuilder = arkitBuilder->initDepthMap();
  depthMapBuilder.setRows(srcDepth.rows());
  depthMapBuilder.setCols(srcDepth.cols());
  depthMapBuilder.setElementsPerRow(srcDepth.rowElements());
  depthMapBuilder.setPixelData(
    kj::ArrayPtr<const float>(srcDepth.pixels(), srcDepth.rows() * srcDepth.rowElements()));
}

YPlanePixels frameYPixels(CameraFrame::Builder frame) {
  int rows = 0;
  int cols = 0;
  int rowBytes = 0;
  uint8_t *data = nullptr;

  // Create a reference to the frame pixels that can be used by the reality engine.
  auto image = frame.getImage();
  if (image.getOneOf().isGrayImagePointer()) {
    auto imptr = image.getOneOf().getGrayImagePointer();
    rows = imptr.getRows();
    cols = imptr.getCols();
    rowBytes = imptr.getBytesPerRow();
    data = reinterpret_cast<uint8_t *>(imptr.getUInt8PixelDataPointer());
  } else if (image.getOneOf().isGrayImageData()) {
    auto imdata = image.getOneOf().getGrayImageData();
    rows = imdata.getRows();
    cols = imdata.getCols();
    rowBytes = imdata.getBytesPerRow();
    data = static_cast<uint8_t *>(imdata.getUInt8PixelData().begin());
  }

  return YPlanePixels(rows, cols, rowBytes, data);
}

ConstYPlanePixels constFrameYPixels(CameraFrame::Reader frame) {
  int rows = 0;
  int cols = 0;
  int rowBytes = 0;
  const uint8_t *data = nullptr;

  // Create a reference to the frame pixels that can be used by the reality engine.
  auto image = frame.getImage();
  if (image.getOneOf().isGrayImagePointer()) {
    auto imptr = image.getOneOf().getGrayImagePointer();
    rows = imptr.getRows();
    cols = imptr.getCols();
    rowBytes = imptr.getBytesPerRow();
    data = reinterpret_cast<const uint8_t *>(imptr.getUInt8PixelDataPointer());
  } else if (image.getOneOf().isGrayImageData()) {
    auto imdata = image.getOneOf().getGrayImageData();
    rows = imdata.getRows();
    cols = imdata.getCols();
    rowBytes = imdata.getBytesPerRow();
    data = static_cast<const uint8_t *>(imdata.getUInt8PixelData().begin());
  }

  return ConstYPlanePixels(rows, cols, rowBytes, data);
}

UVPlanePixels frameUVPixels(CameraFrame::Builder frame) {
  int rows = 0;
  int cols = 0;
  int rowBytes = 0;
  uint8_t *data = nullptr;

  auto image = frame.getUvImage();
  if (image.getOneOf().isGrayImagePointer()) {
    auto imptr = image.getOneOf().getGrayImagePointer();
    rows = imptr.getRows();
    cols = imptr.getCols();
    rowBytes = imptr.getBytesPerRow();
    data = reinterpret_cast<uint8_t *>(imptr.getUInt8PixelDataPointer());
  } else if (image.getOneOf().isGrayImageData()) {
    auto imdata = image.getOneOf().getGrayImageData();
    rows = imdata.getRows();
    cols = imdata.getCols();
    rowBytes = imdata.getBytesPerRow();
    data = static_cast<uint8_t *>(imdata.getUInt8PixelData().begin());
  }

  return UVPlanePixels(rows, cols, rowBytes, data);
}

ConstUVPlanePixels constFrameUVPixels(CameraFrame::Reader frame) {
  int rows = 0;
  int cols = 0;
  int rowBytes = 0;
  const uint8_t *data = nullptr;

  auto image = frame.getUvImage();
  if (image.getOneOf().isGrayImagePointer()) {
    auto imptr = image.getOneOf().getGrayImagePointer();
    rows = imptr.getRows();
    cols = imptr.getCols();
    rowBytes = imptr.getBytesPerRow();
    data = reinterpret_cast<const uint8_t *>(imptr.getUInt8PixelDataPointer());
  } else if (image.getOneOf().isGrayImageData()) {
    auto imdata = image.getOneOf().getGrayImageData();
    rows = imdata.getRows();
    cols = imdata.getCols();
    rowBytes = imdata.getBytesPerRow();
    data = static_cast<const uint8_t *>(imdata.getUInt8PixelData().begin());
  }

  return ConstUVPlanePixels(rows, cols, rowBytes, data);
}

DepthFloatPixelBuffer depthMapPixelBuffer(RequestSensor::Reader sensor) {
  ScopeTimer t("depth-map-pixel-buffer");
  if (sensor.getARKit().hasDepthMap()) {
    return depthMapPixelBuffer(sensor.getARKit());
  }
  if (sensor.getARCore().hasDepthMap()) {
    return depthMapPixelBuffer(sensor.getARCore());
  }
  return {};
}

bool hasDepthMapData(RequestSensor::Reader sensor) {
  return hasDepthMapData(sensor.getARKit()) || hasDepthMapData(sensor.getARCore());
}

void setPosition32f(float x, float y, float z, Position32f::Builder *position) {
  position->setX(x);
  position->setY(y);
  position->setZ(z);
}

void setQuaternion32f(float w, float x, float y, float z, Quaternion32f::Builder *rotation) {
  rotation->setW(w);
  rotation->setX(x);
  rotation->setY(y);
  rotation->setZ(z);
}

void setDevicePose(
  float ax,
  float ay,
  float az,
  float qw,
  float qx,
  float qy,
  float qz,
  RequestPose::Builder *devicePose) {
  auto devicePoseAcceleration = devicePose->getDeviceAcceleration();
  auto devicePoseRotation = devicePose->getDevicePose();
  auto deviceSensorRotationToPortrait = devicePose->getSensorRotationToPortrait();
  setPosition32f(ax, ay, az, &devicePoseAcceleration);
  setQuaternion32f(qw, qx, qy, qz, &devicePoseRotation);
  setQuaternion32f(1.0f, 0.0f, 0.0f, 0.0f, &deviceSensorRotationToPortrait);
}

void setGps(float latitude, float longitude, float horizontalAccuracy, RequestGPS::Builder *gps) {
  gps->setLatitude(latitude);
  gps->setLongitude(longitude);
  gps->setHorizontalAccuracy(horizontalAccuracy);
}

void setInvalidRequest(const char *message, ResponseError::Builder *error) {
  error->setFailed(true);
  error->setCode(ResponseError::ErrorCode::INVALID_REQUEST);
  error->setMessage(message);
}

void setXRResponseLegacy(
  const RealityResponse::Reader &response, c8_XRResponseLegacy *newXRReality) {
  newXRReality->eventIdTimeMicros = response.getEventId().getEventTimeMicros();

  // Set lighting values.
  auto xrResponse = response.getXRResponse();
  newXRReality->lightingGlobalExposure = xrResponse.getLighting().getGlobal().getExposure();

  // Set camera values.
  auto cameraExtrinsic = xrResponse.getCamera().getExtrinsic();
  newXRReality->cameraExtrinsicPositionX = cameraExtrinsic.getPosition().getX();
  newXRReality->cameraExtrinsicPositionY = cameraExtrinsic.getPosition().getY();
  newXRReality->cameraExtrinsicPositionZ = cameraExtrinsic.getPosition().getZ();
  newXRReality->cameraExtrinsicRotationW = cameraExtrinsic.getRotation().getW();
  newXRReality->cameraExtrinsicRotationX = cameraExtrinsic.getRotation().getX();
  newXRReality->cameraExtrinsicRotationY = cameraExtrinsic.getRotation().getY();
  newXRReality->cameraExtrinsicRotationZ = cameraExtrinsic.getRotation().getZ();

  int i;
  // Copy camera matrix.  TODO(nb): is there a more efficient way to do this by directly copying
  // the float array?
  i = 0;
  for (float val : xrResponse.getCamera().getIntrinsic().getMatrix44f()) {
    newXRReality->cameraIntrinsicMatrix44f[i] = val;
    if (++i == C8_API_LIMITS_MATRIX44) {
      break;
    }
  }

  // Set surfaces values.
  i = 0;
  for (auto surface : xrResponse.getSurfaces().getSet().getSurfaces()) {
    // TODO(nb): Handle face and vertex indices that are outside bounds.
    newXRReality->surfacesSetSurfacesIdTimeMicros[i] = surface.getId().getEventTimeMicros();
    newXRReality->surfacesSetSurfacesFacesBeginIndex[i] = surface.getFacesBeginIndex();
    newXRReality->surfacesSetSurfacesFacesEndIndex[i] = surface.getFacesEndIndex();
    newXRReality->surfacesSetSurfacesVerticesBeginIndex[i] = surface.getVerticesBeginIndex();
    newXRReality->surfacesSetSurfacesVerticesEndIndex[i] = surface.getVerticesEndIndex();
    if (++i == C8_API_LIMITS_MAX_SURFACES) {
      break;
    }
  }
  newXRReality->surfacesSetSurfacesCount = i;

  i = 0;
  for (auto face : xrResponse.getSurfaces().getSet().getFaces()) {
    // TODO(nb): Handle vertex indices that are outside bounds.
    newXRReality->surfacesSetFaces[i] = face.getV0();
    newXRReality->surfacesSetFaces[i + 1] = face.getV1();
    newXRReality->surfacesSetFaces[i + 2] = face.getV2();
    i += 3;
    if (i == (C8_API_LIMITS_MAX_SURFACE_FACES * 3)) {
      break;
    }
  }
  newXRReality->surfacesSetFacesCount = i / 3;

  i = 0;
  for (auto vertex : xrResponse.getSurfaces().getSet().getVertices()) {
    newXRReality->surfacesSetVertices[i] = vertex.getX();
    newXRReality->surfacesSetVertices[i + 1] = vertex.getY();
    newXRReality->surfacesSetVertices[i + 2] = vertex.getZ();
    i += 3;
    if (i == (C8_API_LIMITS_MAX_SURFACE_VERTICES * 3)) {
      break;
    }
  }
  newXRReality->surfacesSetVerticesCount = i / 3;

  const auto &activeSurface = xrResponse.getSurfaces().getActiveSurface();
  newXRReality->surfacesActiveSurfaceIdTimeMicros = activeSurface.getId().getEventTimeMicros();
  newXRReality->surfacesActiveSurfaceActivePointX = activeSurface.getActivePoint().getX();
  newXRReality->surfacesActiveSurfaceActivePointY = activeSurface.getActivePoint().getY();
  newXRReality->surfacesActiveSurfaceActivePointZ = activeSurface.getActivePoint().getZ();
}

void setPixelPinholeCameraModelNoRotate(
  const c8_PixelPinholeCameraModel &model, PixelPinholeCameraModel::Builder *builder) {
  builder->setPixelsWidth(model.pixelsWidth);
  builder->setPixelsHeight(model.pixelsHeight);
  builder->setCenterPointX(model.centerPointX);
  builder->setCenterPointY(model.centerPointY);
  builder->setFocalLengthHorizontal(model.focalLengthHorizontal);
  builder->setFocalLengthVertical(model.focalLengthVertical);
}

void setPixelPinholeCameraModelRotateToPortrait(
  const c8_PixelPinholeCameraModel &model, PixelPinholeCameraModel::Builder *builder) {
  bool rotate90 = model.pixelsWidth > model.pixelsHeight;
  if (!rotate90) {
    setPixelPinholeCameraModelNoRotate(model, builder);
    return;
  }

  // Rotated 90 clockwise.  The input image is pre-rotated before being sent to the request, so
  // we need to pre-rotate the intrinsic matrix too.
  // Rotating the center point:
  // +x -> +y
  // +y -> -x
  // E.g. in 640x480 -> 480x640, center 322,242 goes to 238,322

  // /----------------\
  // |                |
  // |                |
  // |        \       |
  // |                |
  // \----------------/
  // /---------\
  // |         |
  // |         |
  // |         |
  // |   /     |
  // |         |
  // |         |
  // \---------/
  int naturalCenterX = model.pixelsWidth / 2;
  int naturalCenterY = model.pixelsHeight / 2;
  float relCenterX = model.centerPointX - naturalCenterX;
  float relCenterY = model.centerPointY - naturalCenterY;
  float newCenterX = naturalCenterY - relCenterY;
  float newCenterY = naturalCenterX + relCenterX;

  builder->setPixelsWidth(model.pixelsHeight);
  builder->setPixelsHeight(model.pixelsWidth);
  builder->setCenterPointX(newCenterX);
  builder->setCenterPointY(newCenterY);
  builder->setFocalLengthHorizontal(model.focalLengthVertical);
  builder->setFocalLengthVertical(model.focalLengthHorizontal);
}

void setImageRoi(const ImageRoi &imageRoi, Gr8ImageRoi::Builder builder) {
  switch (imageRoi.source) {
    case ImageRoi::Source::GRAVITY:
      builder.setSource(Gr8ImageRoi::Source::GRAVITY);
      break;
    case ImageRoi::Source::IMAGE_TARGET:
      builder.setSource(Gr8ImageRoi::Source::IMAGE_TARGET);
      break;
    case ImageRoi::Source::HIRES_SCAN:
      builder.setSource(Gr8ImageRoi::Source::HIRES_SCAN);
      break;
    case ImageRoi::Source::CURVY_IMAGE_TARGET:
      builder.setSource(Gr8ImageRoi::Source::CURVY_IMAGE_TARGET);
      builder.setGlobalPose(kj::ArrayPtr<const float>(imageRoi.globalPose.data().data(), 16));
      builder.setInverseGlobalPose(
        kj::ArrayPtr<const float>(imageRoi.globalPose.inverseData().data(), 16));
      builder.setCameraIntrinsics(kj::ArrayPtr<const float>(imageRoi.intrinsics.data().data(), 16));
      builder.setInverseCameraIntrinsics(
        kj::ArrayPtr<const float>(imageRoi.intrinsics.inverseData().data(), 16));
      builder.getGeom().setRadius(imageRoi.geom.radius);
      builder.getGeom().setRadiusBottom(imageRoi.geom.radiusBottom);
      builder.getGeom().setHeight(imageRoi.geom.height);
      builder.getGeom().setSrcRows(imageRoi.geom.srcRows);
      builder.getGeom().setSrcCols(imageRoi.geom.srcCols);
      builder.getGeom().getActivationRegion().setLeft(imageRoi.geom.activationRegion.left);
      builder.getGeom().getActivationRegion().setRight(imageRoi.geom.activationRegion.right);
      builder.getGeom().getActivationRegion().setTop(imageRoi.geom.activationRegion.top);
      builder.getGeom().getActivationRegion().setBottom(imageRoi.geom.activationRegion.bottom);
      break;
    default:
      break;
  }
  builder.setId(imageRoi.faceId);
  builder.setName(imageRoi.name);
  builder.setWarp(kj::ArrayPtr<const float>(imageRoi.warp.data().data(), 16));
  builder.setInverseWarp(kj::ArrayPtr<const float>(imageRoi.warp.inverseData().data(), 16));
}

void setRois(const Vector<ImageRoi> &rois, EngineExport::Builder builder) {
  builder.initRois(rois.size());
  for (int i = 0; i < rois.size(); ++i) {
    const auto &r = rois[i];
    auto b = builder.getRois()[i];
    setImageRoi(r, b);
  }
}

ImageRoi getImageRoi(Gr8ImageRoi::Reader imageRoi) {
  std::array<float, 16> warpData;
  std::array<float, 16> invWarpData;
  std::array<float, 16> intrinsicsData;
  std::array<float, 16> invIntrinsicsData;
  std::array<float, 16> globalPoseData;
  std::array<float, 16> invGlobalPoseData;
  ImageRoi ret;
  switch (imageRoi.getSource()) {
    case Gr8ImageRoi::Source::GRAVITY:
      ret.source = ImageRoi::Source::GRAVITY;
      break;
    case Gr8ImageRoi::Source::IMAGE_TARGET:
      ret.source = ImageRoi::Source::IMAGE_TARGET;
      break;
    case Gr8ImageRoi::Source::HIRES_SCAN:
      ret.source = ImageRoi::Source::HIRES_SCAN;
      break;
    case Gr8ImageRoi::Source::CURVY_IMAGE_TARGET:
      ret.source = ImageRoi::Source::CURVY_IMAGE_TARGET;

      ret.geom = {
        imageRoi.getGeom().getRadius(),
        imageRoi.getGeom().getHeight(),
        {imageRoi.getGeom().getActivationRegion().getLeft(),
         imageRoi.getGeom().getActivationRegion().getRight(),
         imageRoi.getGeom().getActivationRegion().getTop(),
         imageRoi.getGeom().getActivationRegion().getBottom()},
        imageRoi.getGeom().getSrcRows(),
        imageRoi.getGeom().getSrcCols(),
        std::abs(imageRoi.getGeom().getRadiusBottom() - imageRoi.getGeom().getRadius()) > 1e-3,
        imageRoi.getGeom().getRadiusBottom()};

      std::copy(
        imageRoi.getCameraIntrinsics().begin(),
        imageRoi.getCameraIntrinsics().end(),
        intrinsicsData.begin());
      std::copy(
        imageRoi.getInverseCameraIntrinsics().begin(),
        imageRoi.getInverseCameraIntrinsics().end(),
        invIntrinsicsData.begin());
      ret.intrinsics = HMatrix{intrinsicsData.data(), invIntrinsicsData.data()};

      std::copy(
        imageRoi.getGlobalPose().begin(), imageRoi.getGlobalPose().end(), globalPoseData.begin());
      std::copy(
        imageRoi.getInverseGlobalPose().begin(),
        imageRoi.getInverseGlobalPose().end(),
        invGlobalPoseData.begin());
      ret.globalPose = HMatrix{globalPoseData.data(), invGlobalPoseData.data()};
      break;
    default:
      break;
  }
  ret.faceId = imageRoi.getId();
  ret.name = imageRoi.getName();
  std::copy(imageRoi.getWarp().begin(), imageRoi.getWarp().end(), warpData.begin());
  std::copy(
    imageRoi.getInverseWarp().begin(), imageRoi.getInverseWarp().end(), invWarpData.begin());
  ret.warp = HMatrix{warpData.data(), invWarpData.data()};
  return ret;
}

Vector<ImageRoi> getRois(EngineExport::Reader reader) {
  Vector<ImageRoi> builder(reader.getRois().size());
  for (int i = 0; i < reader.getRois().size(); ++i) {
    auto r = reader.getRois()[i];
    builder[i] = getImageRoi(r);
  }
  return builder;
}

LevelLayout getLevelLayout(Gr8LevelLayout::Reader level) {
  LevelLayout ret;
  ret.c = level.getC();
  ret.r = level.getR();
  ret.w = level.getW();
  ret.h = level.getH();
  ret.rotated = level.getRotated();
  return ret;
}

void setLevelLayout(const LevelLayout &level, Gr8LevelLayout::Builder builder) {
  builder.setC(level.c);
  builder.setR(level.r);
  builder.setW(level.w);
  builder.setH(level.h);
  builder.setRotated(level.rotated);
}

void buildRealityRequestPyramid(const Gr8Pyramid &pyramid, RealityRequest::Builder requestBuilder) {
  auto pyramidBuilder = requestBuilder.getSensors().getCamera().getCurrentFrame().getPyramid();
  pyramidBuilder.getImage().setCols(pyramid.data.cols());
  pyramidBuilder.getImage().setRows(pyramid.data.rows());
  pyramidBuilder.getImage().setBytesPerRow(pyramid.data.rowBytes());
  pyramidBuilder.getImage().setUInt8PixelDataPointer(
    reinterpret_cast<size_t>(pyramid.data.pixels()));

  auto levels = pyramidBuilder.initLevels(pyramid.levels.size());
  for (int i = 0; i < pyramid.levels.size(); i++) {
    setLevelLayout(pyramid.levels[i], levels[i]);
  }

  auto rois = pyramidBuilder.initRois(pyramid.rois.size());
  for (int i = 0; i < pyramid.rois.size(); i++) {
    setLevelLayout(pyramid.rois[i].layout, rois[i].getLayout());
    setImageRoi(pyramid.rois[i].roi, rois[i].getRoi());
  }
}

// Extract quaternion from ARKit or ARCore sensor data if available.
Quaternion poseQuaternion(RequestSensor::Reader s) {
  if (s.hasARKit()) {
    return toQuaternion(s.getARKit().getPose().getRotation());
  }
  if (s.hasARCore()) {
    return toQuaternion(s.getARCore().getPose().getRotation());
  }
  return {};
}

// Extra translation from ARKit or ARCore sensor data if available.
HPoint3 poseTranslation(RequestSensor::Reader s) {
  if (s.hasARKit()) {
    return toHPoint(s.getARKit().getPose().getTranslation());
  }
  if (s.hasARCore()) {
    return toHPoint(s.getARCore().getPose().getTranslation());
  }
  return {};
}

}  // namespace c8
