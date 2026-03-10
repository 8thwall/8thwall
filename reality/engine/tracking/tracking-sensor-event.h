// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "c8/camera/device-infos.h"
#include "c8/geometry/pixel-pinhole-camera-model.h"
#include "c8/io/capnp-messages.h"
#include "c8/pixels/gr8-pyramid.h"
#include "c8/pixels/pixels.h"
#include "c8/quaternion.h"
#include "c8/vector.h"
#include "reality/engine/api/reality.capnp.h"

namespace c8 {

struct TrackingSensorEvent {
  enum TrackingSensorEventKind {
    TRACKING_SENSOR_EVENT_KIND_UNSPECIFIED = 0,
    ACCELEROMETER = 1,
    MAGNETOMETER = 2,
    GYROSCOPE = 3,
    LINEAR_ACCELERATION = 4,
  };

  TrackingSensorEventKind kind = TrackingSensorEventKind::TRACKING_SENSOR_EVENT_KIND_UNSPECIFIED;
  // The timestamp. On web this is the time the event was received by the event handler.
  int64_t eventTimeNanos = 0;
  // The interval at which data is obtained from the underlying hardware. On web it is this or the
  // equivalent property per sensor kind:
  // https://developer.mozilla.org/en-US/docs/Web/API/DeviceMotionEvent/interval
  int64_t intervalNanos = 0;
  // If GYROSCOPE: degrees/second.
  // If ACCELEROMETER / LINEAR_ACCELERATION: meters/second^2.
  float x = 0.0f;
  float y = 0.0f;
  float z = 0.0f;

  HPoint3 hpoint() const { return {x, y, z}; }
  HVector3 hvector() const { return {x, y, z}; }
  float l2Norm() const { return std::sqrt(x * x + y * y + z * z); }

  bool operator<(TrackingSensorEvent b) const {
    if (eventTimeNanos != b.eventTimeNanos) {
      return eventTimeNanos < b.eventTimeNanos;
    }
    if (kind != b.kind) {
      return kind < b.kind;
    }
    if (x != b.x) {
      return x < b.x;
    }
    if (y != b.y) {
      return y < b.y;
    }
    return z < b.z;
  }

  String toString() const noexcept;
};

struct TrackingSensorFrame {
  DeviceInfos::DeviceModel deviceModel;
  bool isIos = false;
  int64_t timeNanos = 0;
  c8_PixelPinholeCameraModel intrinsic;
  YPlanePixels srcY;
  Gr8Pyramid pyramid;
  Quaternion devicePose;  // Uncorrected -> To consume this, need xrRotationFromDeviceRotation.
  double latitude = 0.0;
  double longitude = 0.0;
  double horizontalAccuracy = 0.0;
  Vector<TrackingSensorEvent> sensorEvents;
  Vector<ConstRootMessage<DebugData>> debugData;

  TrackingSensorFrame clone() const {
    TrackingSensorFrame res{};
    res.deviceModel = deviceModel;
    res.isIos = isIos;
    res.timeNanos = timeNanos;
    res.intrinsic = intrinsic;
    res.srcY = srcY;
    res.pyramid = pyramid;
    res.devicePose = devicePose;
    res.latitude = latitude;
    res.longitude = longitude;
    res.horizontalAccuracy = horizontalAccuracy;
    res.sensorEvents = sensorEvents;
    for (const auto &e : debugData) {
      res.debugData.push_back(e.clone());
    }
    return res;
  }
};

// Sets the grayscale image, GlPyramid (if available in the request), device model, intrinsic,
// gyro pose, and sensor events (accelerometer, gravity, etc) on the TrackingSensorFrame.
// NOTE(akul): Maybe see the comment in doIosWorkaround() about this function before major refactor.
void prepareTrackingSensorFrame(
  DeviceInfos::DeviceModel deviceModel,
  const String &deviceManufacturer,
  c8_PixelPinholeCameraModel intrinsic,
  RequestSensor::Reader requestSensor,
  TrackingSensorFrame *frame);

}  // namespace c8
