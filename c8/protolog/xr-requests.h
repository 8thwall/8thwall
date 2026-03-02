// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)
//
// Helpers for getting data into and out of xr api.

#pragma once

#include "c8/geometry/pixel-pinhole-camera-model.h"
#include "c8/hpoint.h"
#include "c8/hvector.h"
#include "c8/pixels/gr8-pyramid.h"
#include "c8/pixels/image-roi.h"
#include "c8/pixels/pixels.h"
#include "c8/protolog/xr-extern.h"
#include "c8/quaternion.h"
#include "reality/engine/api/reality.capnp.h"

namespace c8 {

void setXRConfigurationLegacy(
  const c8_XRConfigurationLegacy &config, XRConfiguration::Builder *requestConfig);

YPlanePixels frameYPixels(CameraFrame::Builder frame);
UVPlanePixels frameUVPixels(CameraFrame::Builder frame);

ConstYPlanePixels constFrameYPixels(CameraFrame::Reader frame);
ConstUVPlanePixels constFrameUVPixels(CameraFrame::Reader frame);

bool hasDepthMapData(RequestSensor::Reader sensor);

// Return estimated distance in meters.
DepthFloatPixelBuffer depthMapPixelBuffer(RequestSensor::Reader sensor);

void setCameraPixelPointers(
  ConstYPlanePixels srcY, ConstUVPlanePixels srcUV, RequestCamera::Builder *camera);

void setRawUvImageData(ConstUVPlanePixels uvPixels, GrayImageData::Builder imageBuilder);
void setRawYImageData(ConstYPlanePixels yPixels, GrayImageData::Builder imageBuilder);

void setDepthMap(ConstDepthFloatPixels srcDepth, RequestARKit::Builder *arkitBuilder);

void setPosition32f(float x, float y, float z, Position32f::Builder *position);

inline void setPosition32f(float x, float y, float z, Position32f::Builder position) {
  setPosition32f(x, y, z, &position);
}

inline HPoint3 toHPoint(const Position32f::Reader p) {
  return HPoint3(p.getX(), p.getY(), p.getZ());
}

inline HVector3 toHVector(const Position32f::Reader p) {
  return HVector3(p.getX(), p.getY(), p.getZ());
}

void setQuaternion32f(float w, float x, float y, float z, Quaternion32f::Builder *rotation);

inline void setQuaternion32f(float w, float x, float y, float z, Quaternion32f::Builder rotation) {
  setQuaternion32f(w, x, y, z, &rotation);
}

inline void setQuaternion(Quaternion q, Quaternion32f::Builder *b) {
  setQuaternion32f(q.w(), q.x(), q.y(), q.z(), b);
}

inline void setQuaternion(Quaternion q, Quaternion32f::Builder b) { setQuaternion(q, &b); }

inline void setPosition(HPoint3 p, Position32f::Builder *b) {
  setPosition32f(p.x(), p.y(), p.z(), b);
}

inline void setPosition(HPoint3 p, Position32f::Builder b) { setPosition(p, &b); }

inline void setPosition(HVector3 p, Position32f::Builder *b) {
  setPosition32f(p.x(), p.y(), p.z(), b);
}

inline void setPosition(HVector3 p, Position32f::Builder b) { setPosition(p, &b); }

void setQuaternion32f(float w, float x, float y, float z, Quaternion32f::Builder *rotation);

inline Quaternion toQuaternion(const Quaternion32f::Reader q) {
  return Quaternion(q.getW(), q.getX(), q.getY(), q.getZ());
}

void setDevicePose(
  float ax,
  float ay,
  float az,
  float qw,
  float qx,
  float qy,
  float qz,
  RequestPose::Builder *devicePose);

void setGps(float latitude, float longitude, float horizontalAccuracy, RequestGPS::Builder *gps);

void setInvalidRequest(const char *message, ResponseError::Builder *error);

void setXRResponseLegacy(
  const RealityResponse::Reader &response, c8_XRResponseLegacy *newXRReality);

void setPixelPinholeCameraModelRotateToPortrait(
  const c8_PixelPinholeCameraModel &model, PixelPinholeCameraModel::Builder *builder);

inline void setPixelPinholeCameraModelRotateToPortrait(
  const c8_PixelPinholeCameraModel &model, PixelPinholeCameraModel::Builder builder) {
  setPixelPinholeCameraModelRotateToPortrait(model, &builder);
}

void setPixelPinholeCameraModelNoRotate(
  const c8_PixelPinholeCameraModel &model, PixelPinholeCameraModel::Builder *builder);

inline void setPixelPinholeCameraModelNoRotate(
  const c8_PixelPinholeCameraModel &model, PixelPinholeCameraModel::Builder builder) {
  setPixelPinholeCameraModelNoRotate(model, &builder);
}

// Sets `imageRoi`s on the builder.
void setRois(const Vector<ImageRoi> &rois, EngineExport::Builder builder);

void setImageRoi(const ImageRoi &imageRoi, Gr8ImageRoi::Builder builder);
ImageRoi getImageRoi(Gr8ImageRoi::Reader imageRoi);
// Gets `imageRoi`s.
Vector<ImageRoi> getRois(EngineExport::Reader reader);

void setLevelLayout(const LevelLayout &level, Gr8LevelLayout::Builder builder);
LevelLayout getLevelLayout(Gr8LevelLayout::Reader level);

/** Copies data from a Gr8Pyramid into a RealityRequest's pyramid.
 * @param pyramid the source image pyramid
 * @param requestBuilder the builder to copy the pixels and level sizes to
 */
void buildRealityRequestPyramid(const Gr8Pyramid &pyramid, RealityRequest::Builder requestBuilder);

// Extract quaternion from ARKit or ARCore sensor data if available.
Quaternion poseQuaternion(RequestSensor::Reader s);

// Extra translation from ARKit or ARCore sensor data if available.
HPoint3 poseTranslation(RequestSensor::Reader s);

}  // namespace c8
