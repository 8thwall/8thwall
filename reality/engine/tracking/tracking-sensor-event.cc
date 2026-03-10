// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"tracking-sensor-event.h"};
  deps = {
    "//c8:c8-log",
    "//c8:quaternion",
    "//c8:vector",
    "//c8/camera:device-infos",
    "//c8/geometry:pixel-pinhole-camera-model",
    "//c8/io:capnp-messages",
    "//c8/pixels:gr8-pyramid",
    "//c8/pixels:pixels",
    "//c8/protolog:xr-requests",
    "//reality/engine/api:reality.capnp-cc",
  };
}
cc_end(0x34121d01);

#include <cctype>

#include "c8/protolog/xr-requests.h"
#include "reality/engine/tracking/tracking-sensor-event.h"

namespace c8 {

namespace {
String kindString(TrackingSensorEvent::TrackingSensorEventKind k) {
  switch (k) {
    case TrackingSensorEvent::ACCELEROMETER:
      return "ACCELEROMETER";
    case TrackingSensorEvent::GYROSCOPE:
      return "GYROSCOPE";
    case TrackingSensorEvent::MAGNETOMETER:
      return "MAGNETOMETER";
    case TrackingSensorEvent::LINEAR_ACCELERATION:
      return "LINEAR_ACCELERATION";
    default:
      return format("%d", k);
  }
}

// Returns the grayscale image of the camera frame.
YPlanePixels requestYPixels(RequestCamera::Reader requestCamera) {
  auto requestY = requestCamera.getCurrentFrame().getImage().getOneOf().getGrayImagePointer();
  int rows = requestY.getRows();
  int cols = requestY.getCols();
  int bytesPerRow = requestY.getBytesPerRow();

  // Cast a long to a size_t, possibly downcasting, before reinterpreting as a pointer.
  size_t imageAddr = static_cast<size_t>(requestY.getUInt8PixelDataPointer());
  uint8_t *rowStart = reinterpret_cast<uint8_t *>(imageAddr);
  return YPlanePixels(rows, cols, bytesPerRow, rowStart);
}

}  // namespace

String TrackingSensorEvent::toString() const noexcept {
  return c8::format(
    "(time: %lld, kind: %s, x: %04.2f, y: %04.2f, z: %04.2f)",
    eventTimeNanos,
    kindString(kind).c_str(),
    x,
    y,
    z);
}

void prepareTrackingSensorFrame(
  DeviceInfos::DeviceModel deviceModel,
  const String &deviceManufacturer,
  c8_PixelPinholeCameraModel intrinsic,
  RequestSensor::Reader requestSensor,
  TrackingSensorFrame *frame) {
  auto isIos = std::strcmp(toUpperCase(deviceManufacturer).c_str(), "APPLE") == 0;
  auto sensorRotationToPortrait = requestSensor.getPose().hasSensorRotationToPortrait()
    ? toQuaternion(requestSensor.getPose().getSensorRotationToPortrait())
    : Quaternion();
  auto sensorRotationToPortraitMat = sensorRotationToPortrait.toRotationMat();

  frame->deviceModel = deviceModel;
  frame->isIos = isIos;
  frame->intrinsic = intrinsic;
  frame->devicePose =
    toQuaternion(requestSensor.getPose().getDevicePose()).times(sensorRotationToPortrait);
  frame->latitude = requestSensor.getGps().getLatitude();
  frame->longitude = requestSensor.getGps().getLongitude();
  frame->horizontalAccuracy = requestSensor.getGps().getHorizontalAccuracy();

  // Use frameTimeNanos if we can, as it's when we read the frame, versus timeNanos which is when
  // the frame is staged. Note that videoTimeNanos is when the frame is created so may be an
  // improvement, but it is on a differnt timescale so would need converting.
  auto frameTimeNanos = requestSensor.getCamera().getCurrentFrame().getFrameTimestampNanos();
  frame->timeNanos = frameTimeNanos != 0
    ? frameTimeNanos
    : requestSensor.getCamera().getCurrentFrame().getTimestampNanos();
  frame->sensorEvents.reserve(requestSensor.getPose().getEventQueue().size());
  for (auto re : requestSensor.getPose().getEventQueue()) {
    TrackingSensorEvent se;
    auto rotated = sensorRotationToPortraitMat * toHPoint(re.getValue());
    se.x = rotated.x();
    se.y = rotated.y();
    se.z = rotated.z();
    se.eventTimeNanos = re.getTimestampNanos();
    if (isIos) {
      // On iOS the interval returned is in seconds instead of milliseconds as the spec says.
      se.intervalNanos = re.getIntervalNanos() * 1000;
    } else {
      // On Android we get 16000000 nanos. This looks like they are removing precision from a 60hz
      // clock, so correct to 60hz.
      se.intervalNanos = re.getIntervalNanos() == 16000000 ? 16666667 : re.getIntervalNanos();
    }

    // Fixing sensor event's sign
    switch (re.getKind()) {
      case RawPositionalSensorValue::PositionalSensorKind::ACCELEROMETER:
      case RawPositionalSensorValue::PositionalSensorKind::LINEAR_ACCELERATION:
        // On all devices we go from right-handed to left-handed by setting z = -z.
        // https://www.w3.org/TR/accelerometer/#model
        if (isIos) {
          // On iOS there is a bug where the sign of acceleration is flipped. You can test this by
          // setting a phone down on a table with the screen up. Gravity pulls us down so we are
          // accelerating up (away from the table). In a right-handed coordinate system with +z up,
          // we should get 9.8, and do on Android. But on iOS we get -9.8. So multiply accel by -1.
          // This has been tested with both acceleration and linear acceleration.
          se.x = -se.x;
          se.y = -se.y;
        } else {
          se.z = -se.z;
        }
        break;
      case RawPositionalSensorValue::PositionalSensorKind::GYROSCOPE:
        // Align the signs of angular velocity between VIO and IMU. See charts on
        // https://github.com/8thwall/code8/pull/22251
        // On the web the raw data from the `devicemotion` event is:
        // x: alpha - the motion of the device around the z axis in degrees. From [0, 360).
        // y: beta - the motion of the device around the x axis in degrees. From [-180, 180).
        //    This represents a front to back motion of the device.
        // z: gamma - the motion of the device around the y axis in degrees. From [-90, 90).
        //    This represents a left to right motion of the device.
        // https://developer.mozilla.org/en-US/docs/Web/Events/Detecting_device_orientation#orientation_values_explained
        se.x = -se.x;
        se.y = -se.y;
        break;
      default:
        break;
    }

    // Setting sensor event's kind
    switch (re.getKind()) {
      case RawPositionalSensorValue::PositionalSensorKind::ACCELEROMETER:
        se.kind = TrackingSensorEvent::ACCELEROMETER;
        break;
      case RawPositionalSensorValue::PositionalSensorKind::GYROSCOPE:
        se.kind = TrackingSensorEvent::GYROSCOPE;
        break;
      case RawPositionalSensorValue::PositionalSensorKind::MAGNETOMETER:
        se.kind = TrackingSensorEvent::MAGNETOMETER;
        break;
      case RawPositionalSensorValue::PositionalSensorKind::LINEAR_ACCELERATION:
        se.kind = TrackingSensorEvent::LINEAR_ACCELERATION;
        break;
      default:
        se.kind = TrackingSensorEvent::TRACKING_SENSOR_EVENT_KIND_UNSPECIFIED;
        break;
    }

    frame->sensorEvents.push_back(se);
  }

  std::sort(frame->sensorEvents.begin(), frame->sensorEvents.end());

  // Add grayscale image of the camera frame.
  (*frame).srcY = requestYPixels(requestSensor.getCamera());

  // Add image pyramid.
  if (requestSensor.getCamera().getCurrentFrame().hasPyramid()) {
    auto pyramid = requestSensor.getCamera().getCurrentFrame().getPyramid();
    uint8_t *dataAddress =
      reinterpret_cast<uint8_t *>(pyramid.getImage().getUInt8PixelDataPointer());
    RGBA8888PlanePixels pyramidPixels(
      pyramid.getImage().getRows(),
      pyramid.getImage().getCols(),
      pyramid.getImage().getBytesPerRow(),
      dataAddress);

    Vector<LevelLayout> levels(pyramid.getLevels().size());
    for (int i = 0; i < pyramid.getLevels().size(); i++) {
      levels[i] = getLevelLayout(pyramid.getLevels()[i]);
    }

    auto messageRois = pyramid.getRois();
    Vector<ROI> rois(messageRois.size());
    for (int i = 0; i < messageRois.size(); i++) {
      rois[i] = {getImageRoi(messageRois[i].getRoi()), getLevelLayout(messageRois[i].getLayout())};
    }
    (*frame).pyramid = Gr8Pyramid{pyramidPixels, levels, rois};
  }
}

}  // namespace c8
