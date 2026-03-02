// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"
cc_library {
  hdrs = {
    "face-detector-global.h",
  };
  deps = {
    "//c8:c8-log",
    "//c8:vector",
    "//c8/geometry:box2",
    "//c8/geometry:face-types",
    "//c8/stats:scope-timer",
    "//c8/string:join",
    "//c8/pixels:pixel-transforms",
    "//reality/engine/deepnets:detection-anchor-nms",
    "//reality/engine/deepnets:tflite-interpreter",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xcf318290);

#include <cmath>

#include "c8/c8-log.h"
#include "c8/geometry/box2.h"
#include "c8/pixels/pixel-transforms.h"
#include "c8/stats/scope-timer.h"
#include "c8/string/join.h"
#include "reality/engine/deepnets/detection-anchor-nms.h"
#include "reality/engine/faces/face-detector-global.h"

namespace c8 {

namespace {

constexpr float IOU_THRESHOLD = 0.3;

////////////////////////////////////////// PROCESS /////////////////////////////////////////

void removeLetterBoxScale(Vector<Detection> &detections, int width, int height) {
  float xScale = 1;
  float yScale = 1;

  if (height > width) {
    xScale = static_cast<float>(width) / height;
  } else {
    yScale = static_cast<float>(height) / width;
  }

  for (auto &detection : detections) {
    auto bb = detection.locationData.relativeBoundingBox;

    // update the bounding box
    detection.locationData.relativeBoundingBox = {
      ((bb.x - 0.5f) / xScale) + 0.5f,
      ((bb.y - 0.5f) / yScale) + 0.5f,
      bb.w / xScale,
      bb.h / yScale,
    };

    // update the key landmark coordinates
    for (auto &pt : detection.locationData.relativeKeypoints) {
      pt = {((pt.x() - 0.5f) / xScale) + 0.5f, ((pt.y() - 0.5f) / yScale) + 0.5f};
    }
  }
}

}  // namespace

void FaceDetectorGlobal::initializeDetection() {
  AnchorOptions options;
  anchors_ = generateAnchors(options);
}

Vector<DetectedPoints> FaceDetectorGlobal::detectFaces(
  RenderedSubImage src, c8_PixelPinholeCameraModel k) {
  ScopeTimer t("global-detect-faces");
  toLetterboxRGBFloatN1to1(src.image, 128, 128, interpreter_->typed_input_tensor<float>(0));

  {
    ScopeTimer t1("global-model-invoke");
    interpreter_->Invoke();
  }

  auto fullDetections = processDetections(
    interpreter_->typed_output_tensor<float>(0),
    interpreter_->typed_output_tensor<float>(1),
    anchors_,
    decodeOptions_);
  auto detections = weightedNonMaxSuppression(fullDetections, IOU_THRESHOLD);

  removeLetterBoxScale(detections, src.image.cols(), src.image.rows());

  Vector<DetectedPoints> output;
  output.reserve(detections.size());
  for (const auto &detection : detections) {
    auto bb = detection.locationData.relativeBoundingBox;
    // eyes are 0 (image left) and 1 (image right).
    const auto &pts = detection.locationData.relativeKeypoints;

    Vector<HPoint3> bbPts = {
      // boundingBox
      {bb.x, bb.y, 1.0f},                              // upper left
      {bb.x + bb.w, bb.y, 1.0f},                       // upper right
      {bb.x, bb.y + bb.h, 1.0f},                       // lower left
      {bb.x + bb.w, bb.y + bb.h, 1.0f},                // lower right
      {bb.x + bb.w * 0.5f, bb.y + bb.h * 0.5f, 1.0f},  // center
    };

    // compensate for aspect ratio distortion.
    auto stretch = HMatrixGen::scale(src.image.cols() - 1, src.image.rows() - 1, 1);
    Vector<HPoint3> eyePts = {
      {pts[0].x(), pts[0].y(), 1.0f},
      {pts[1].x(), pts[1].y(), 1.0f},
    };

    auto stretchEyePts = stretch * eyePts;
    auto stretchBbpts = stretch * bbPts;

    // atan2 takes arguments: y, x
    auto zRotRad = std::atan2(
      stretchEyePts[1].y() - stretchEyePts[0].y(), stretchEyePts[1].x() - stretchEyePts[0].x());

    auto center = HMatrixGen::translation(stretchBbpts[4].x(), stretchBbpts[4].y(), 0.0f);
    auto transform =
      stretch.inv() * center * HMatrixGen::zRadians(zRotRad) * center.inv() * stretch;
    auto rotBbPts = flatten<2>(transform * bbPts);

    output.push_back(
      {detection.score,
       0,
       src.viewport,
       src.roi,
       {
         // boundingBox
         rotBbPts[0],  // upper left
         rotBbPts[1],  // upper right
         rotBbPts[2],  // lower left
         rotBbPts[3],  // lower right
       },
       {},
       k});
    auto &d = output.back();
    for (auto pt : detection.locationData.relativeKeypoints) {
      d.points.push_back({pt.x(), pt.y(), 0.0f});
    }
  }

  return output;
}  // namespace c8

}  // namespace c8
