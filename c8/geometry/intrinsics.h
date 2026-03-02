// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "c8/camera/device-infos.h"
#include "c8/geometry/pixel-pinhole-camera-model.h"
#include "c8/hmatrix.h"
#include "reality/engine/api/reality.capnp.h"

namespace c8 {

// Static utilities for getting intrinsic matrices for different cameras.
class Intrinsics {
public:
  // The intrinsic matrix of a camera suitable for unit tests.
  static HMatrix unitTestCamera();

  // The intrinsic matrix for a Logitech QuickCam Pro 9000 webcam.
  static HMatrix logitechQuickCamPro9000();

  // The intrinsic matrix for an iPhone 7's rear camera in portrait orientation.
  static HMatrix iPhone7RearPortrait();

  // The intrinsic matrix for a Galaxy S6's rear camera in landscape orientation.
  static HMatrix galaxyS6RearLandscape();
  static HMatrix galaxyS6RearPortrait();

  // The intrinsic matrix for a Galaxy S6's rear camera in landscape orientation.
  static HMatrix galaxyS7RearLandscape();

  static void toClipSpace(
    const c8_PixelPinholeCameraModel intrinsics,
    float nearClip,
    float farClip,
    float *colMajorProjMat,
    float *invColMajorProjMat = nullptr);

  static HMatrix toClipSpaceMat(
    const c8_PixelPinholeCameraModel intrinsics, float nearClip, float farClip);

  static void toClipSpaceLeftHanded(
    const c8_PixelPinholeCameraModel intrinsics,
    float nearClip,
    float farClip,
    float *colMajorProjMat,
    float *invColMajorProjMat = nullptr);

  static HMatrix toClipSpaceMatLeftHanded(
    const c8_PixelPinholeCameraModel intrinsics, float nearClip, float farClip);

  // Return OpenGL glOrtho projection matrix
  // The coordinate system used in OpenGL is right hand coordinate.
  // The Y axis is up and the positive Z axis points towards the viewer.
  // @param xLeft Distance from viewer to left clipping plane
  // @param xRight Distance from viewer to right clipping plane
  // @param yUp Distance from viewer to upper clipping plane
  // @param yDown Distance from viewer to down clipping plane
  // @param zNear Distance from viewer to near clipping plane
  // @param zFar Distance from viewer to far clipping plane
  static HMatrix orthographicProjectionRightHanded(
    float xLeft, float xRight, float yUp, float yDown, float zNear, float zFar) noexcept;

  // Return OpenGL gluPerspective projection matrix.
  // The coordinate system used in OpenGL is right hand coordinate.
  // The Y axis is up and the positive Z axis points towards the viewer.
  // @param fovRadius the field of view angle, in radius, in the y direction.
  // @param aspectRatio The aspect ratio is the ratio of x (width) to y (height)
  // @param nearClip the distance from the viewer to the near clipping plane (always positive).
  // @param farClip the distance from the viewer to the far clipping plane (always positive).
  static HMatrix perspectiveProjectionRightHanded(
    float fovRadius, float aspectRatio, float nearClip, float farClip) noexcept;

  static c8_PixelPinholeCameraModel rotateCropAndScaleIntrinsics(
    const c8_PixelPinholeCameraModel intrinsics, int width, int height);

  static c8_PixelPinholeCameraModel cropAndScaleIntrinsics(
    const c8_PixelPinholeCameraModel intrinsics, int width, int height);

  static c8_PixelPinholeCameraModel getCaptureIntrinsics(
    const c8_PixelPinholeCameraModel fullFrameIntrinsics,
    const CameraCaptureGeometry::Reader captureGeometry);

  static c8_PixelPinholeCameraModel getProcessingIntrinsics(
    const c8_PixelPinholeCameraModel captureIntrinsics, const CameraFrame::Reader processingFrame);

  static c8_PixelPinholeCameraModel getProcessingIntrinsics(
    DeviceInfos::DeviceModel model, int width, int height);

  static c8_PixelPinholeCameraModel getDisplayIntrinsics(
    const c8_PixelPinholeCameraModel captureIntrinsics,
    const GraphicsPinholeCameraModel::Reader displayModel);

  static c8_PixelPinholeCameraModel getFullFrameIntrinsics(
    const RequestCamera::Reader cameraReader, const DeviceInfo::Reader &model);

  static c8_PixelPinholeCameraModel getCaptureIntrinsics(
    const RequestCamera::Reader cameraReader,
    const DeviceInfo::Reader &model,
    const CameraCaptureGeometry::Reader captureGeometry);

  static c8_PixelPinholeCameraModel getProcessingIntrinsics(
    const RequestCamera::Reader cameraReader,
    const DeviceInfo::Reader &model,
    const CameraCaptureGeometry::Reader captureGeometry,
    const CameraFrame::Reader processingFrame);

  static c8_PixelPinholeCameraModel getDisplayIntrinsics(
    const RequestCamera::Reader cameraReader,
    const DeviceInfo::Reader &model,
    const CameraCaptureGeometry::Reader captureGeometry,
    const GraphicsPinholeCameraModel::Reader displayModel);

  static c8_PixelPinholeCameraModel getFullFrameIntrinsics(const RealityRequest::Reader &request);
  static c8_PixelPinholeCameraModel getCaptureIntrinsics(const RealityRequest::Reader &request);
  static c8_PixelPinholeCameraModel getProcessingIntrinsics(const RealityRequest::Reader &request);
  static c8_PixelPinholeCameraModel getDisplayIntrinsics(const RealityRequest::Reader &request);
  static c8_PixelPinholeCameraModel getCameraIntrinsics(const DeviceInfos::DeviceModel &model);

  // Gets an intrinsics matrix for a camera with an extended full frame sensor and a focal length
  // in mm. Full frame sensors have a 24mm height and 36mm width.
  static c8_PixelPinholeCameraModel fullFrameFocalLengthMm(float focalLength);

  // This is a static utility class, it can't be constructred, copied or moved.
  Intrinsics() = delete;
  Intrinsics(Intrinsics &&) = delete;
  Intrinsics &operator=(Intrinsics &&) = delete;
  Intrinsics(const Intrinsics &) = delete;
  Intrinsics &operator=(const Intrinsics &) = delete;

private:
  static c8_PixelPinholeCameraModel getFallbackCameraIntrinsics(
    const DeviceInfo::Reader &deviceInfoReader);
};

}  // namespace c8
