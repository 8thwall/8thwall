// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "image-detection-request-executor.h",
  };
  deps = {
    ":detection-image",
    "//c8:hmatrix",
    "//c8:c8-log",
    "//c8/geometry:device-pose",
    "//c8/geometry:egomotion",
    "//c8/pixels:pixel-transforms",
    "//c8/protolog:xr-requests",
    "//c8/stats:scope-timer",
    "//reality/engine/api:reality.capnp-cc",
    "//reality/engine/tracking:tracker",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xef5b3b30);

#define _USE_MATH_DEFINES
#include <cmath>

#include "c8/c8-log.h"
#include "c8/geometry/device-pose.h"
#include "c8/geometry/egomotion.h"
#include "c8/hmatrix.h"
#include "c8/pixels/pixel-transforms.h"
#include "c8/protolog/xr-requests.h"
#include "c8/stats/scope-timer.h"
#include "reality/engine/imagedetection/image-detection-request-executor.h"

using namespace c8;

namespace {
void fillDetectedImagesARCore(
  RequestSensor::Reader sensor,
  ResponsePose::Reader computedPose,
  ResponseDetection::Builder *response) {
  if (
    !sensor.getARCore().hasDetectedImages() || sensor.getARCore().getDetectedImages().size() == 0) {
    return;
  }

  int numDetectedImages = sensor.getARCore().getDetectedImages().size();
  response->initImages(numDetectedImages);

  for (int i = 0; i < numDetectedImages; ++i) {
    auto detectedImage = sensor.getARCore().getDetectedImages()[i];
    auto responseImage = response->getImages()[i];

    responseImage.setId(detectedImage.getHash());
    responseImage.setName(detectedImage.getName());
    responseImage.setWidthInMeters(detectedImage.getExtent().getX());
    responseImage.setHeightInMeters(detectedImage.getExtent().getZ());

    switch (detectedImage.getTrackingStatus()) {
      case ARCoreDetectedImage::TrackingStatus::FULL_TRACKING:
        responseImage.setTrackingStatus(DetectedImage::TrackingStatus::FULL_TRACKING);
        break;
      case ARCoreDetectedImage::TrackingStatus::LAST_KNOWN_POSE:
        responseImage.setTrackingStatus(DetectedImage::TrackingStatus::LAST_KNOWN_POSE);
        break;
      case ARCoreDetectedImage::TrackingStatus::NOT_TRACKING:
        responseImage.setTrackingStatus(DetectedImage::TrackingStatus::NOT_TRACKING);
        break;
      default:
        responseImage.setTrackingStatus(DetectedImage::TrackingStatus::UNSPECIFIED);
        break;
    }

    auto xrRot = xrRotationFromARCoreRotationWhileTracking(
      toQuaternion(detectedImage.getCenterPose().getRotation()));
    auto xrPos = xrPositionFromARCorePositionWhileTracking(
      toHPoint(detectedImage.getCenterPose().getTranslation()));

    auto initRot = toQuaternion(computedPose.getInitialTransform().getRotation());
    auto initPos = toHVector(computedPose.getInitialTransform().getPosition());

    auto xrPose = cameraMotion(xrPos, xrRot);
    auto initPose = cameraMotion(initPos, initRot);
    auto correctedPose = egomotion(initPose, xrPose);

    auto posBuilder = responseImage.getPlace().getPosition();
    auto rotBuilder = responseImage.getPlace().getRotation();
    setPosition(translation(correctedPose), &posBuilder);
    setQuaternion(rotation(correctedPose), &rotBuilder);
  }
}

void fillDetectedImagesARKit(
  RequestSensor::Reader sensor,
  ResponsePose::Reader computedPose,
  ResponseDetection::Builder *response) {
  if (sensor.getARKit().getImageAnchors().size() == 0) {
    return;
  }

  int numImageAnchors = sensor.getARKit().getImageAnchors().size();
  response->initImages(numImageAnchors);
  for (int i = 0; i < numImageAnchors; ++i) {
    auto sensorAnchor = sensor.getARKit().getImageAnchors()[i];
    auto responseAnchor = response->getImages()[i];

    // Copy fields unaltered.
    responseAnchor.setId(sensorAnchor.getUuidHash());
    responseAnchor.setName(sensorAnchor.getName());
    responseAnchor.setWidthInMeters(sensorAnchor.getPhysicalSizeWidth());
    responseAnchor.setHeightInMeters(sensorAnchor.getPhysicalSizeHeight());

    switch (sensorAnchor.getTrackingStatus()) {
      case ARKitImageAnchor::TrackingStatus::FULL_TRACKING:
        responseAnchor.setTrackingStatus(DetectedImage::TrackingStatus::FULL_TRACKING);
        break;
      case ARKitImageAnchor::TrackingStatus::LAST_KNOWN_POSE:
        responseAnchor.setTrackingStatus(DetectedImage::TrackingStatus::LAST_KNOWN_POSE);
        break;
      default:
        responseAnchor.setTrackingStatus(DetectedImage::TrackingStatus::UNSPECIFIED);
        break;
    }

    // For pose, convert to xr coordinates and then recenter.
    auto xrRot =
      xrRotationFromARKitRotation(toQuaternion(sensorAnchor.getAnchorPose().getRotation()));
    auto xrPos =
      xrPositionFromARKitPosition(toHPoint(sensorAnchor.getAnchorPose().getTranslation()));

    auto initRot = toQuaternion(computedPose.getInitialTransform().getRotation());
    auto initPos = toHVector(computedPose.getInitialTransform().getPosition());

    auto xrPose = cameraMotion(xrPos, xrRot);
    auto initPose = cameraMotion(initPos, initRot);
    auto correctedPose = egomotion(initPose, xrPose);

    // For some reason, ARKit still needs a 90 degree rotation to get an expected coordinate system.
    auto correctedPose2 = updateWorldPosition(correctedPose, HMatrixGen::zDegrees(-90.0f));

    auto posBuilder = responseAnchor.getPlace().getPosition();
    auto rotBuilder = responseAnchor.getPlace().getRotation();
    setPosition(translation(correctedPose2), &posBuilder);
    setQuaternion(rotation(correctedPose2), &rotBuilder);
  }
}
}  // namespace

void ImageDetectionRequestExecutor::execute(
  Tracker *tracker,
  DetectionImageMap *targets,
  RandomNumbers *random,
  const ResponseDetection::Reader &lastResponse,
  RequestSensor::Reader sensor,
  ResponsePose::Reader computedPose,
  ResponseDetection::Builder *response,
  EngineExport::Builder *engineExport) const {

  if (sensor.hasARCore()) {
    ScopeTimer t("arcore-image-detection");
    fillDetectedImagesARCore(sensor, computedPose, response);
  } else if (sensor.hasARKit()) {
    ScopeTimer t("arkit-image-detection");
    fillDetectedImagesARKit(sensor, computedPose, response);
  } else {
    fillDetectedImagesC8(
      tracker, targets, random, lastResponse, sensor, computedPose, response, engineExport);
  }
}

void ImageDetectionRequestExecutor::configure(XRConfiguration::Reader config) {
  configureC8(config);
}

void ImageDetectionRequestExecutor::configureC8(XRConfiguration::Reader config) {}

void ImageDetectionRequestExecutor::fillDetectedImagesC8(
  Tracker *tracker,
  DetectionImageMap *targetsPtr,
  RandomNumbers *random,
  const ResponseDetection::Reader &lastResponse,
  RequestSensor::Reader sensor,
  ResponsePose::Reader computedPose,
  ResponseDetection::Builder *response,
  EngineExport::Builder *engineExport) const {
  if (
    tracker == nullptr || targetsPtr == nullptr || targetsPtr->empty()
    || tracker->getLastKeypoints() == nullptr) {
    return;
  }

  auto initRot = toQuaternion(computedPose.getInitialTransform().getRotation());
  auto initPos = toHVector(computedPose.getInitialTransform().getPosition());
  auto initPose = cameraMotion(initPos, initRot);

  const DetectionImageMap &detectionImages = *targetsPtr;
  Vector<ImageRoi> rois;
  TreeMap<String, MutableRootMessage<DetectedImage>> responseTargets;
  for (const auto &located : tracker->locatedImageTargets()) {
    // Try to recover with an ROI for 20 frames after tracking loss.
    if (located.targetSpace.lastSeen < 20) {
      rois.push_back(located.roi);
    }
    // Make the target disappear quickly after tracking loss, with only a short window for recovery.
    if (located.targetSpace.lastSeen >= 3) {
      continue;
    }
    // Wait for the target to be tracked for three contiguous frames before showing.
    if (located.targetSpace.firstSeen <= 3) {
      continue;
    }
    const DetectionImage &detectionImage = detectionImages.at(located.targetSpace.name);

    auto r = responseTargets[located.roi.name].builder();

    r.setId(located.roi.faceId);
    r.setName(detectionImage.getName());

    r.setWidthInMeters(located.width);
    r.setHeightInMeters(located.height);
    auto pose = egomotion(initPose, located.pose);
    switch (detectionImage.getType()) {
      case DetectionImageType::PLANAR: {
        r.setType(ImageTargetTypeMsg::PLANAR);
        break;
      }
      case DetectionImageType::CURVY: {
        r.setType(ImageTargetTypeMsg::CURVY);
        detectionImage.toCurvyGeometry(r.getCurvyGeometry());
        break;
      }
      case DetectionImageType::UNSPECIFIED: {
        r.setType(ImageTargetTypeMsg::UNSPECIFIED);
        break;
      }
    }

    setPosition(translation(pose), r.getPlace().getPosition());
    setQuaternion(rotation(pose), r.getPlace().getRotation());
  }
  setRois(rois, *engineExport);

  response->initImages(responseTargets.size());
  int i = 0;
  for (const auto &l : responseTargets) {
    auto r = response->getImages()[i++];
    auto lr = l.second.reader();
    r.setPlace(lr.getPlace());
    r.setName(lr.getName());
    r.setId(lr.getId());
    r.setWidthInMeters(lr.getWidthInMeters());
    r.setHeightInMeters(lr.getHeightInMeters());
    r.setType(lr.getType());
    if (lr.getType() == ImageTargetTypeMsg::CURVY) {
      r.getCurvyGeometry().setCurvyCircumferenceTop(
        lr.getCurvyGeometry().getCurvyCircumferenceTop());
      r.getCurvyGeometry().setCurvyCircumferenceBottom(
        lr.getCurvyGeometry().getCurvyCircumferenceBottom());
      r.getCurvyGeometry().setCurvySideLength(lr.getCurvyGeometry().getCurvySideLength());
      r.getCurvyGeometry().setHeight(lr.getCurvyGeometry().getHeight());
      r.getCurvyGeometry().setTargetCircumferenceTop(
        lr.getCurvyGeometry().getTargetCircumferenceTop());
      r.getCurvyGeometry().setTopRadius(lr.getCurvyGeometry().getTopRadius());
      r.getCurvyGeometry().setBottomRadius(lr.getCurvyGeometry().getBottomRadius());
      r.getCurvyGeometry().setArcLengthRadians(lr.getCurvyGeometry().getArcLengthRadians());
      r.getCurvyGeometry().setArcStartRadians(lr.getCurvyGeometry().getArcStartRadians());
    }
  }
}
