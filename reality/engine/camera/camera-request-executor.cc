// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  visibility = {
    "//reality/engine/executor:__subpackages__", "//reality/engine/camera:__subpackages__",
  };
  hdrs = {
    "camera-request-executor.h",
  };
  deps = {
    "//c8:c8-log",
    "//c8:c8-log-proto",
    "//c8/camera:device-infos",
    "//c8/geometry:intrinsics",
    "//c8/io:capnp-messages",
    "//c8/protolog:api-limits",
    "//c8/protolog:xr-requests",
    "//reality/engine/api:reality.capnp-cc",
    "//reality/engine/api/request:sensor.capnp-cc",
    "//reality/engine/api/device:info.capnp-cc",
    "//reality/engine/api/base:camera-intrinsics.capnp-cc",
  };
}
cc_end(0x9e465c3b);

#include <capnp/list.h>
#include "c8/c8-log.h"
#include "c8/c8-log-proto.h"
#include "c8/camera/device-infos.h"
#include "c8/geometry/intrinsics.h"
#include "c8/io/capnp-messages.h"
#include "c8/protolog/api-limits.h"
#include "c8/protolog/xr-requests.h"
#include "reality/engine/api/base/camera-intrinsics.capnp.h"
#include "reality/engine/camera/camera-request-executor.h"

using MutablePixelPinholeCameraModel = c8::MutableRootMessage<c8::PixelPinholeCameraModel>;

namespace c8 {

c8_PixelPinholeCameraModel toPinholeModelStruct(const PixelPinholeCameraModel::Reader &p) {
  c8_PixelPinholeCameraModel m;
  m.pixelsWidth = p.getPixelsWidth();
  m.pixelsHeight = p.getPixelsHeight();
  m.centerPointX = p.getCenterPointX();
  m.centerPointY = p.getCenterPointY();
  m.focalLengthHorizontal = p.getFocalLengthHorizontal();
  m.focalLengthVertical = p.getFocalLengthVertical();
  return m;
}

c8_PixelPinholeCameraModel toPinholeModelStruct(const capnp::List<float>::Reader &p, int w, int h) {
  auto m00 = p[0];
  auto m11 = p[5];
  auto m02 = p[8];
  auto m12 = p[9];

  c8_PixelPinholeCameraModel m;
  m.pixelsWidth = w;
  m.pixelsHeight = h;
  m.centerPointX = w * (1.0f - m02) / 2.0f - 0.5f;
  m.centerPointY = h * (m12 + 1.0f) / 2.0f - 0.5f;
  m.focalLengthHorizontal = m00 * w / 2.0f;
  m.focalLengthVertical = m11 * h / 2.0f;

  return m;
}

void setIntrinsicForProjectionMatrix(
  capnp::List<float>::Reader &projectionMatrix, GraphicsCalibration::Builder *c) {
  for (int i = 0; i < 16; ++i) {
    c->getMatrix44f().set(i, projectionMatrix[i]);
  }
}

void setIntrinsicsMatrix(
  c8_PixelPinholeCameraModel intrinsics,
  const GraphicsPinholeCameraModel::Reader &g,
  GraphicsCalibration::Builder *c) {
  std::array<float, 16> projMat;
  Intrinsics::toClipSpace(intrinsics, g.getNearClip(), g.getFarClip(), projMat.data());
  for (int i = 0; i < 16; ++i) {
    c->getMatrix44f().set(i, projMat[i]);
  }
}

// definitions
void CameraRequestExecutor::execute(
  const RequestSensor::Reader &sensor,
  const XRConfiguration::Reader &config,
  const ResponsePose::Reader &computedPose,
  const DeviceInfo::Reader &deviceInfo,
  ResponseCamera::Builder *response) const {
  // Copy the pose that was already computed.
  response->setExtrinsic(computedPose.getTransform());
  response->setTrackingState(computedPose.getTrackingState());

  // Initialize the matrix to zeros.
  response->getIntrinsic().initMatrix44f(16);
  for (int i = 0; i < 16; ++i) {
    // Initialize to 0. TODO(nb): is this needed?
    response->getIntrinsic().getMatrix44f().set(i, 0.0f);
  }

  auto c = response->getIntrinsic();
  if (sensor.getARCore().hasProjectionMatrix()) {
    auto projectionMatrix = sensor.getARCore().getProjectionMatrix();
    if (config.getGraphicsIntrinsics().getFarClip() > 0) {
      float w = sensor.getCamera().getPixelIntrinsics().getPixelsWidth();
      float h = sensor.getCamera().getPixelIntrinsics().getPixelsHeight();
      setIntrinsicsMatrix(
        Intrinsics::getDisplayIntrinsics(
          toPinholeModelStruct(projectionMatrix, w, h),
          config.getGraphicsIntrinsics()
        ),
        config.getGraphicsIntrinsics(),
        &c
      );
    } else {
      setIntrinsicForProjectionMatrix(projectionMatrix, &c);
    }
    return;
  }

  if (sensor.getCamera().hasPixelIntrinsics() && config.getGraphicsIntrinsics().getFarClip() > 0) {
    setIntrinsicsMatrix(
      Intrinsics::getDisplayIntrinsics(
        toPinholeModelStruct(sensor.getCamera().getPixelIntrinsics()),
        config.getGraphicsIntrinsics()
      ),
      config.getGraphicsIntrinsics(),
      &c
    );
    return;
  }

  // In case the underlying framework is not giving the camera intrinsics. We get the intrinsics
  // using the make and model.
  if (config.getGraphicsIntrinsics().getFarClip() > 0) {
    MutablePixelPinholeCameraModel cameraModelMessage;
    auto cameraModelBuilder = cameraModelMessage.builder();
    setPixelPinholeCameraModelNoRotate(
      Intrinsics::getDisplayIntrinsics(
        sensor.getCamera(),
        deviceInfo,
        config.getCameraConfiguration().getCaptureGeometry(),
        config.getGraphicsIntrinsics()
      ),
      &cameraModelBuilder);
    setIntrinsicsMatrix(
      toPinholeModelStruct(cameraModelMessage.reader()),
      config.getGraphicsIntrinsics(),
      &c);
    return;
  }
  // TODO(nb): Add a fallback when the pixels intrinsics are known but not the graphics
  // intrinsics. This should use default clipping planes (.3, 1) on Android (OGL) and (.3, 1000)
  // on iOS (Metal) and assume that the display mathces the geometry of the image.

  // TODO(nb): Add a fallback when the graphics intrinsics are known, but not the pixel
  // intrinsics, using a reasonable K.

  /*
  TODO(nb): Do this in a more principled and general way. Currently this is hard coded to make
  egg toss look good on iPhone7 with ARKit. Here's some notes:

  iPhone7 intrinsics for a 1280x720 image:
    {{ 1052.73, 0.00000, 638.297}
     { 0.00000, 1052.73, 357.192}
     { 0.00000, 0.00000, 1.00000}}

  Projection mat when the image fills the screen:
    {{ 2.92424, 0.0000, 0.007799980, 0.000000}
     { 0.00000, 1.6453, -0.00266171, 0.000000}
     { 0.00000, 0.0000, -1.00060000, -0.60018}
     { 0.00000, 0.0000, -1.00000000, 0.000000}}

  Dubiously useful, but still noted:
  IOS projection mat with no clipping planes or window size params:
  {{ 1.64488, 0.00000, 0.00266093, 0.00000}
   { 0.00000, 2.92424, 0.00779998, 0.0000}
   { 0.00000, 0.00000, -1.0000000, -0.002}
   { 0.00000, 0.00000, -1.0000000, 0.0000}}

   // NOTE(nb): I no longer think this is the right math (we don't need to zoom; if wee did the
  math
   // would be right), but left here for notes for now.
   Focal length factor induced by 640x480 aspect: 1.3333333333 (4 / 3)
    {{ 3.89899, 0.0000, 0.010399973, 0.000000}
     { 0.00000, 2.1937, -0.00354895, 0.000000}
     { 0.00000, 0.0000, -1.00060000, -0.60018}
     { 0.00000, 0.0000, -1.00000000, 0.000000}}
  */
  // Fall back to a hard coded matrix that looks good for egg toss on iPhone7 with ARKit.

  c.getMatrix44f().set(0, 2.92424f);
  c.getMatrix44f().set(5, 1.64488f);
  c.getMatrix44f().set(8, 0.0f);
  c.getMatrix44f().set(9, 0.0015625f);  // 2 / h: TODO(nb): double check this with mc@.
  c.getMatrix44f().set(10, -1.0006f);
  c.getMatrix44f().set(11, -1.0f);
  c.getMatrix44f().set(14, -0.60018f);
}

}  // namespace c8
