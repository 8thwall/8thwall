// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  visibility = {
    "//reality/engine/executor:__subpackages__",
    "//reality/engine/pose:__subpackages__",
  };
  hdrs = {
    "pose-request-executor.h",
  };
  deps = {
    "//c8:c8-log",
    "//c8:hmatrix",
    "//c8:hpoint",
    "//c8:hvector",
    "//c8:quaternion",
    "//c8:random-numbers",
    "//c8/camera:device-infos",
    "//c8/geometry:device-pose",
    "//c8/geometry:egomotion",
    "//c8/geometry:homography",
    "//c8/protolog:xr-requests",
    "//reality/engine/api/request:app.capnp-cc",
    "//reality/engine/api/request:flags.capnp-cc",
    "//reality/engine/api/request:sensor.capnp-cc",
    "//reality/engine/api/response:pose.capnp-cc",
    "//reality/engine/api/response:features.capnp-cc",
    "//reality/engine/features:frame-point",
    "//reality/engine/tracking:tracker",
  };
}
cc_end(0x804daeca);

#include "c8/c8-log.h"
#include "c8/geometry/device-pose.h"
#include "c8/geometry/egomotion.h"
#include "c8/geometry/homography.h"
#include "c8/hmatrix.h"
#include "c8/hpoint.h"
#include "c8/hvector.h"
#include "c8/protolog/xr-requests.h"
#include "c8/quaternion.h"
#include "reality/engine/features/frame-point.h"
#include "reality/engine/pose/pose-request-executor.h"

namespace c8 {

namespace {

void setRotationDelta(
  const Quaternion32f::Reader &r1, const Quaternion32f::Reader &r2, Quaternion32f::Builder *rd) {
  setQuaternion(rotationDelta(toQuaternion(r1), toQuaternion(r2)), rd);
}

void updateCameraPoseARKit(
  const RequestARKit::Reader &arkit,
  Quaternion32f::Builder *newRotation,
  Position32f::Builder *newPosition,
  XrTrackingState::Builder *newStatus) {
  setQuaternion(
    xrRotationFromARKitRotation(toQuaternion(arkit.getPose().getRotation())), newRotation);
  setPosition(xrPositionFromARKitPosition(toHPoint(arkit.getPose().getTranslation())), newPosition);

  switch (arkit.getTrackingState().getStatus()) {
    case ARKitTrackingState::ARKitTrackingStatus::NOT_AVAILABLE:
      newStatus->setStatus(XrTrackingState::XrTrackingStatus::NOT_AVAILABLE);
      break;
    case ARKitTrackingState::ARKitTrackingStatus::NORMAL:
      newStatus->setStatus(XrTrackingState::XrTrackingStatus::NORMAL);
      break;
    case ARKitTrackingState::ARKitTrackingStatus::LIMITED:
      newStatus->setStatus(XrTrackingState::XrTrackingStatus::LIMITED);
      break;
    default:
      // Don't set.
      break;
  }

  switch (arkit.getTrackingState().getReason()) {
    case ARKitTrackingState::ARKitTrackingStatusReason::INITIALIZING:
      newStatus->setReason(XrTrackingState::XrTrackingStatusReason::INITIALIZING);
      break;
    case ARKitTrackingState::ARKitTrackingStatusReason::RELOCALIZING:
      newStatus->setReason(XrTrackingState::XrTrackingStatusReason::RELOCALIZING);
      break;
    case ARKitTrackingState::ARKitTrackingStatusReason::EXCESSIVE_MOTION:
      newStatus->setReason(XrTrackingState::XrTrackingStatusReason::TOO_MUCH_MOTION);
      break;
    case ARKitTrackingState::ARKitTrackingStatusReason::INSUFFICIENT_FEATURES:
      newStatus->setReason(XrTrackingState::XrTrackingStatusReason::NOT_ENOUGH_TEXTURE);
      break;
    default:
      // Don't set.
      break;
  }
}

void updateCameraPoseTango(
  const RequestTango::Reader &tango,
  Quaternion32f::Builder *newRotation,
  Position32f::Builder *newPosition,
  XrTrackingState::Builder *newStatus) {
  setQuaternion(
    xrRotationFromTangoRotation(toQuaternion(tango.getPose().getRotation())), newRotation);
  setPosition(xrPositionFromTangoPosition(toHPoint(tango.getPose().getTranslation())), newPosition);
}

void updateCameraPoseARCore(
  const RequestARCore::Reader &arcore,
  const Quaternion32f::Reader &imuRotation,
  const Quaternion32f::Reader &lastImuRotation,
  const ResponsePose::Reader &lastResponse,
  Quaternion32f::Builder *newRotation,
  Position32f::Builder *newPosition,
  XrTrackingState::Builder *newStatus) {

  switch (arcore.getPoseTrackingState()) {
    case RequestARCore::ARCoreTrackingState::TRACKING:
      newStatus->setStatus(XrTrackingState::XrTrackingStatus::NORMAL);
      break;
    case RequestARCore::ARCoreTrackingState::STOPPED:
      newStatus->setStatus(XrTrackingState::XrTrackingStatus::NOT_AVAILABLE);
      break;
    case RequestARCore::ARCoreTrackingState::PAUSED:
      newStatus->setStatus(XrTrackingState::XrTrackingStatus::LIMITED);
      break;
    default:
      // Don't set.
      break;
  }

  if (arcore.hasPose()) {
    setQuaternion(
      xrRotationFromARCoreRotationWhileTracking(toQuaternion(arcore.getPose().getRotation())),
      newRotation);
    setPosition(
      xrPositionFromARCorePositionWhileTracking(toHPoint(arcore.getPose().getTranslation())),
      newPosition);
    return;
  }

  if (!lastResponse.hasInitialTransform()) {
    // Can't correct for the current rotation if the initial hasn't been set yet.
    // Instead, just use the IMU to determin rotation.
    setQuaternion(deviceRotationFromXrRotation(toQuaternion(imuRotation)), newRotation);
    return;
  }

  // ARCore has lost tracking; fall back to 3DoF IMU with a fixed position.
  setQuaternion(
    xrRotationFromARCoreRotationLostTracking(
      toQuaternion(imuRotation),
      toQuaternion(lastImuRotation),
      toQuaternion(lastResponse.getTransform().getRotation()),
      toQuaternion(lastResponse.getInitialTransform().getRotation())),
    newRotation);

  setPosition(
    xrPositionFromARCorePositionLostTracking(
      toQuaternion(lastResponse.getInitialTransform().getRotation()),
      toQuaternion(lastResponse.getTransform().getRotation()),
      // The initial position offset is already accounted for in this position so we don't need to
      // re-correct it.
      toHVector(lastResponse.getTransform().getPosition())),
    newPosition);
}

void updateCameraPoseImu(
  DeviceInfos::DeviceModel deviceModel,
  const String &deviceManufacturer,
  c8_PixelPinholeCameraModel intrinsic,
  RequestSensor::Reader requestSensor,
  capnp::List<DebugData>::Reader debugData,
  Quaternion32f::Builder *newRotation,
  Position32f::Builder *newPosition,
  Quaternion32f::Builder *newTamRotation,
  Position32f::Builder *newTamPosition,
  XrTrackingState::Builder *newStatus,
  Tracker *tracker,
  DetectionImageMap *targets,
  RandomNumbers *random) {

  // Construct a TrackingSensorFrame, which consists of the grayscale camera frame, GlPyramid,
  // intrinsics, gyro pose, and the sensor events (accelerometer, gravity, etc) leading up to the
  // frame.
  TrackingSensorFrame frame;
  prepareTrackingSensorFrame(deviceModel, deviceManufacturer, intrinsic, requestSensor, &frame);

  for (auto dd : debugData) {
    frame.debugData.emplace_back(dd);
  }

  DetectionImagePtrMap its;
  if (targets != nullptr) {
    for (auto it = targets->begin(); it != targets->end(); it++) {
      its.insert(std::make_pair(it->first, &it->second));
    }
  }

  // Use the tracker to update all state needed to estimate a new camera position.
  {
    auto cam = tracker->track(frame, its, random, *newStatus);
    auto r = rotation(cam);
    auto t = translation(cam);
    setQuaternion(r, newRotation);
    setPosition(t, newPosition);
  }

  // Update tamExtrinsic
  {
    auto tamCam = tracker->tamExtrinsic();
    auto r = rotation(tamCam);
    auto t = translation(tamCam);
    setQuaternion(r, newTamRotation);
    setPosition(t, newTamPosition);
  }
}

void computeRawPose(
  DeviceInfos::DeviceModel deviceModel,
  const String &deviceManufacturer,
  c8_PixelPinholeCameraModel intrinsic,
  const RequestSensor::Reader &lastRequest,
  const ResponsePose::Reader &lastResponse,
  const RequestSensor::Reader &requestSensor,
  const capnp::List<DebugData>::Reader &debugData,
  ResponsePose::Builder *response,
  Tracker *tracker,
  DetectionImageMap *targets,
  RandomNumbers *random) {
  auto newTransform = response->getTransform();

  // Get position / rotation by platform.
  auto newRotation = newTransform.getRotation();
  auto newPosition = newTransform.getPosition();
  auto newStatus = response->getTrackingState();
  if (requestSensor.hasARKit()) {
    updateCameraPoseARKit(requestSensor.getARKit(), &newRotation, &newPosition, &newStatus);
  } else if (requestSensor.hasTango()) {
    updateCameraPoseTango(requestSensor.getTango(), &newRotation, &newPosition, &newStatus);
  } else if (requestSensor.hasARCore()) {
    updateCameraPoseARCore(
      requestSensor.getARCore(),
      requestSensor.getPose().getDevicePose(),
      lastRequest.getPose().getDevicePose(),
      lastResponse,
      &newRotation,
      &newPosition,
      &newStatus);
  } else {
    // Note(anvith) Tracking against 8w cloud maps for persistence use cases will need the origin of
    // the tracker to match the origin of the VPS node/8w cloud map/mesh. By using the TAM
    // extrinsic, we ensure that the tracker origin matches the map origin. The regular extrinsic
    // does not work for this use-case as it undergoes yaw correction and recenter.
    auto newTamTransform = response->getExperimental().getVioTrackerTransform();
    auto newTamRotation = newTamTransform.getRotation();
    auto newTamPosition = newTamTransform.getPosition();

    updateCameraPoseImu(
      deviceModel,
      deviceManufacturer,
      intrinsic,
      requestSensor,
      debugData,
      &newRotation,
      &newPosition,
      &newTamRotation,
      &newTamPosition,
      &newStatus,
      tracker,
      targets,
      random);
  }
}

void recenterPose(
  const RequestSensor::Reader &requestSensor,
  const RealityResponse::Reader &lastResponse,
  const AppContext::Reader &appContext,
  ResponsePose::Builder *response) {
  auto lastPoseResponse = lastResponse.getPose();
  // Re-center the axes of the raw pose so that the way the camera was facing at application start
  // is considered forward.
  auto rotationBuilder = response->getTransform().getRotation();
  auto positionBuilder = response->getTransform().getPosition();

  auto lastOrientation = lastResponse.getAppContext().getDeviceOrientation();
  auto orientation = appContext.getDeviceOrientation();
  if (
    lastOrientation != AppContext::DeviceOrientation::UNSPECIFIED
    && orientation != AppContext::DeviceOrientation::UNSPECIFIED
    && lastOrientation != orientation) {
    // TODO (alvin): Device orientation has changed. We should update the initial rotation,
    //   otherwise we'd be trying to recenter from a quaternion with a different axis.
    C8Log("[pose-request-executor] %s", "Device orientation updated");
  }

  auto initialOrientation = toQuaternion(lastPoseResponse.getInitialTransform().getRotation());
  auto initialPosition = toHVector(lastPoseResponse.getInitialTransform().getPosition());
  auto currentOrientation = toQuaternion(rotationBuilder);
  auto currentPosition = toHVector(positionBuilder);
  if (requestSensor.getARCore().hasPose()) {
    // Once ARCore starts tracking, its results are under the assumption that the direction
    // the camera was facing (along x/z axis) and positioned at are zero. Reset these values
    // so we don't try to correct against a different pose.
    initialOrientation = Quaternion();
    initialPosition = HVector3();
  } else if (!lastPoseResponse.hasInitialTransform()) {
    // TODO (alvin): Investigate if this is ever called. If there is no initial transform,
    // we should have crashed at the time `initialOrientation` was first assigned a value.
    initialOrientation = groundPlaneRotation(currentOrientation);
    initialPosition = currentPosition;
  }

  auto initialOrientationBuilder = response->getInitialTransform().getRotation();
  auto initialPositionBuilder = response->getInitialTransform().getPosition();
  setQuaternion(initialOrientation, &initialOrientationBuilder);
  setPosition(initialPosition, &initialPositionBuilder);

  auto currentMatrix = cameraMotion(currentPosition, currentOrientation);
  auto originalMatrix = cameraMotion(initialPosition, initialOrientation);
  auto correctedMatrix = egomotion(originalMatrix, currentMatrix);
  setPosition(translation(correctedMatrix), &positionBuilder);
  setQuaternion(rotation(correctedMatrix), &rotationBuilder);
}

void updateDeltasAndVelocity(
  float dt,
  const ResponsePose::Reader &lastResponse,
  const RequestSensor::Reader &requestSensor,
  ResponsePose::Builder *response) {
  auto rotationBuilder = response->getTransform().getRotation();
  auto positionBuilder = response->getTransform().getPosition();

  // Compute rotation delta, position delta, and velocity after correcting for pose.
  auto lastTransformRotation = lastResponse.getTransform().getRotation();
  auto newTransformRotationDelta = response->getTransformDelta().getRotation();
  setRotationDelta(lastTransformRotation, rotationBuilder, &newTransformRotationDelta);

  auto lastPosition = lastResponse.getTransform().getPosition();
  auto translationDelta = response->getTransformDelta().getPosition();
  setPosition(
    positionDelta(toHVector(positionBuilder), toHVector(lastPosition)), &translationDelta);

  auto velocity = response->getVelocity().getPosition();
  setPosition(
    updateVelocity(
      toHPoint(requestSensor.getPose().getDeviceAcceleration()),
      toQuaternion(rotationBuilder),
      toHPoint(lastResponse.getVelocity().getPosition()),
      dt),
    &velocity);
}

void updateKeyframeDelta(
  const ResponsePose::Reader &lastResponse,
  const DeprecatedResponseFeatures::Reader &responseFeatures,
  ResponsePose::Builder *response) {
  auto newRotation = response->getTransform().getRotation();
  auto newPosition = response->getTransform().getPosition();
  auto keyframePose = response->getExperimental().getKeyframe().getKeyframeTransform();
  auto keyframeDelta = response->getExperimental().getKeyframe().getKeyframeTransformDelta();
  if (responseFeatures.getExperimental().getKeyframe().getFramesSinceKeyframe() <= 0) {
    // This is a keyframe, set the position and rotation, and zero delta.
    keyframePose.setRotation(newRotation);
    keyframePose.setPosition(newPosition);
    keyframeDelta.getRotation().setW(1.0);  // Other rotation dimensions get initialized 0.
    keyframeDelta.getPosition();            // Initialize position delta to 0.
  } else {
    auto lastKeyframePose = lastResponse.getExperimental().getKeyframe().getKeyframeTransform();
    response->getExperimental().getKeyframe().setKeyframeTransform(lastKeyframePose);
    auto responseKeyframeRotationDelta = keyframeDelta.getRotation();
    setRotationDelta(lastKeyframePose.getRotation(), newRotation, &responseKeyframeRotationDelta);

    // TODO(nb): Set position delta in the reference frame of lastKeyframePose.
    auto lastKeyframePos = lastKeyframePose.getPosition();
    auto keyframePos = keyframeDelta.getPosition();
    setPosition32f(
      newPosition.getX() - lastKeyframePos.getX(),
      newPosition.getY() - lastKeyframePos.getY(),
      newPosition.getZ() - lastKeyframePos.getZ(),
      &keyframePos);
  }
}

}  // namespace

void PoseRequestExecutor::execute(
  const long lastEventTimeMicros,
  const long eventTimeMicros,
  DeviceInfos::DeviceModel deviceModel,
  const String &deviceManufacturer,
  c8_PixelPinholeCameraModel intrinsic,
  const RequestFlags::Reader &requestFlags,
  const RequestSensor::Reader &lastRequest,
  const RealityResponse::Reader &lastResponse,
  const AppContext::Reader &appContext,
  const RequestSensor::Reader &requestSensor,
  const capnp::List<DebugData>::Reader &debugData,
  const DeprecatedResponseFeatures::Reader &responseFeatures,
  ResponsePose::Builder *response,
  Tracker *tracker,
  DetectionImageMap *targets,
  RandomNumbers *random) const {
  float dt = lastEventTimeMicros == 0 ? 0.0f : (eventTimeMicros - lastEventTimeMicros) * 1.0e-6;
  auto lastPoseResponse = lastResponse.getPose();
  // Compute the raw pose from the IMU system.
  computeRawPose(
    deviceModel,
    deviceManufacturer,
    intrinsic,
    lastRequest,
    lastPoseResponse,
    requestSensor,
    debugData,
    response,
    tracker,
    targets,
    random);

  // Re-center the axes of the raw pose so that the way the camera was facing at application start
  // is considered forward.
  recenterPose(requestSensor, lastResponse, appContext, response);

  // Compute rotation delta, position delta, and velocity after correcting for pose.
  updateDeltasAndVelocity(dt, lastPoseResponse, requestSensor, response);

  // If experimental mode is enabled, set the pose delta from the keyframe.
  if (requestFlags.getExperimental()) {
    updateKeyframeDelta(lastPoseResponse, responseFeatures, response);
  }

  response->getStatus();
}

}  // namespace c8
