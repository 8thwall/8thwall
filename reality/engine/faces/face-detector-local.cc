// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"
cc_library {
  hdrs = {
    "face-detector-local.h",
  };
  deps = {
    ":face-geometry",
    "//c8:vector",
    "//c8/geometry:face-types",
    "//c8/geometry:facemesh-data",
    "//c8/pixels:pixel-transforms",
    "//c8/stats:scope-timer",
    "//reality/engine/deepnets:tflite-interpreter",
    "//reality/engine/ears:ear-types",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x7b9a15c2);

#include <cmath>

#include "c8/c8-log.h"
#include "c8/geometry/facemesh-data.h"
#include "c8/pixels/pixel-transforms.h"
#include "c8/stats/scope-timer.h"
#include "reality/engine/faces/face-detector-local.h"
#include "reality/engine/faces/face-geometry.h"

namespace c8 {

Vector<DetectedPoints> FaceDetectorLocal::analyzeFace(
  const RenderedSubImage &src, c8_PixelPinholeCameraModel k) {
  ScopeTimer t("local-detect-faces");
  toLetterboxRGBFloat0To1(
    src.image, 192, src.image.rows(), interpreter_->typed_input_tensor<float>(0));

  {
    ScopeTimer t1("local-model-invoke");
    interpreter_->Invoke();
  }

  // Num outputs: 7
  // Name | Bytes (4 (float) * 2 (x + y) * num_points)
  // Output tensor[0] name: output_mesh_identity | 5616 | 468 points. Has xyz.
  // Output tensor[1] name: output_lips | 640 | 80 points. Only xy.
  // Output tensor[2] name: output_left_eye | 568 | 71 points. Only xy.
  // Output tensor[3] name: output_right_eye | 568 | 71 points. Only xy.
  // Output tensor[4] name: output_left_iris | 40 | 5 points. Only xy.
  // Output tensor[5] name: output_right_iris | 40 | 5 points. Only xy.
  // Output tensor[6] name: conv_faceflag | 4 | confidence that the character is in the scene.
  const auto *classTensor = interpreter_->typed_output_tensor<float>(6);
  auto confidence = classTensor[0];
  if (confidence <= 0.5f) {
    return {};
  }

  // results to be returned
  // element 0 is always the full face
  // extra elements are other head features, e.g. ears.
  // extra elements should have DetectedPoints.roi.source set for data source flags.
  Vector<DetectedPoints> results;

  const auto *meshData = interpreter_->typed_output_tensor<float>(0);
  const auto *lipsData = interpreter_->typed_output_tensor<float>(1);
  const auto *leftEyeData = interpreter_->typed_output_tensor<float>(2);
  const auto *rightEyeData = interpreter_->typed_output_tensor<float>(3);
  const auto *leftIrisData = interpreter_->typed_output_tensor<float>(4);
  const auto *rightIrisData = interpreter_->typed_output_tensor<float>(5);

  DetectedPoints d{
    confidence,
    0,
    src.viewport,
    src.roi,
    {},  // bounding box -- will fill later.
    {},  // points -- will fill later.
    k,
  };

  d.points.reserve(NUM_FACEMESH_POINTS);
  auto scale = 1.0f / 191.0f;
  for (int i = 0; i < ((NUM_FACEMESH_POINTS - NUM_IRIS_POINTS) * 3); i += 3) {
    d.points.push_back({
      meshData[i] * scale,
      meshData[i + 1] * scale,
      meshData[i + 2] * scale,
    });
  }

  {
    // Refinement via attention layers.
    for (int i = 0; i < FACEMESH_ATTENTION_LIPS_INDICES.size(); ++i) {
      auto x = lipsData[i * 2] * scale;
      auto y = lipsData[i * 2 + 1] * scale;
      // Refinement only updates xy, not z.
      auto z = d.points[FACEMESH_ATTENTION_LIPS_INDICES[i]].z();
      d.points[FACEMESH_ATTENTION_LIPS_INDICES[i]] = {x, y, z};
    }
    for (int i = 0; i < FACEMESH_ATTENTION_LEFT_EYE_INDICES.size(); ++i) {
      auto x = leftEyeData[i * 2] * scale;
      auto y = leftEyeData[i * 2 + 1] * scale;
      // Refinement only updates xy, not z.
      auto z = d.points[FACEMESH_ATTENTION_LEFT_EYE_INDICES[i]].z();
      d.points[FACEMESH_ATTENTION_LEFT_EYE_INDICES[i]] = {x, y, z};
    }
    for (int i = 0; i < FACEMESH_ATTENTION_RIGHT_EYE_INDICES.size(); ++i) {
      auto x = rightEyeData[i * 2] * scale;
      auto y = rightEyeData[i * 2 + 1] * scale;
      // Refinement only updates xy, not z.
      auto z = d.points[FACEMESH_ATTENTION_RIGHT_EYE_INDICES[i]].z();
      d.points[FACEMESH_ATTENTION_RIGHT_EYE_INDICES[i]] = {x, y, z};
    }
  }

  {
    // The iris z is determined by averaging the nearby eye zs.
    Vector<float> leftIrisZs;
    leftIrisZs.reserve(FACEMESH_ATTENTION_LEFT_IRIS_Z_AVERAGE_INDICES.size());
    for (int i = 0; i < FACEMESH_ATTENTION_LEFT_IRIS_Z_AVERAGE_INDICES.size(); ++i) {
      leftIrisZs.push_back(d.points[FACEMESH_ATTENTION_LEFT_IRIS_Z_AVERAGE_INDICES[i]].z());
    }
    float leftIrisZ = accumulate(leftIrisZs.begin(), leftIrisZs.end(), 0.0f) / leftIrisZs.size();

    for (int i = 0; i < FACEMESH_ATTENTION_LEFT_IRIS_INDICES.size(); ++i) {
      auto x = leftIrisData[i * 2] * scale;
      auto y = leftIrisData[i * 2 + 1] * scale;
      d.points.push_back({x, y, leftIrisZ});
    }

    Vector<float> rightIrisZs;
    rightIrisZs.reserve(FACEMESH_ATTENTION_RIGHT_IRIS_Z_AVERAGE_INDICES.size());
    for (int i = 0; i < FACEMESH_ATTENTION_RIGHT_IRIS_Z_AVERAGE_INDICES.size(); ++i) {
      rightIrisZs.push_back(d.points[FACEMESH_ATTENTION_RIGHT_IRIS_Z_AVERAGE_INDICES[i]].z());
    }
    float rightIrisZ =
      accumulate(rightIrisZs.begin(), rightIrisZs.end(), 0.0f) / rightIrisZs.size();

    for (int i = 0; i < FACEMESH_ATTENTION_RIGHT_IRIS_INDICES.size(); ++i) {
      auto x = rightIrisData[i * 2] * scale;
      auto y = rightIrisData[i * 2 + 1] * scale;
      d.points.push_back({x, y, rightIrisZ});
    }
  }

  auto texToRay = renderTexToRaySpace(src.roi, k);

  Vector<HPoint3> referencePts = {
    {d.points[FACEMESH_R_EYE_OUTER_CORNER].x(),
     d.points[FACEMESH_R_EYE_OUTER_CORNER].y(),
     1.0f},  // left eye in image
    {d.points[FACEMESH_L_EYE_OUTER_CORNER].x(),
     d.points[FACEMESH_L_EYE_OUTER_CORNER].y(),
     1.0f},                                                                    // right eye in image
    {d.points[FACEMESH_NOSE_TOP].x(), d.points[FACEMESH_NOSE_TOP].y(), 1.0f},  // center
  };
  auto rayRefPts = texToRay * referencePts;

  // atan2 takes arguments: y, x
  auto zRotRad =
    std::atan2(rayRefPts[1].y() - rayRefPts[0].y(), rayRefPts[1].x() - rayRefPts[0].x());

  auto transformForBox = HMatrixGen::zRadians(-zRotRad)
    * HMatrixGen::translation(-rayRefPts[2].x(), -rayRefPts[2].y(), 1.0f) * texToRay;
  auto pixPts = transformForBox * d.points;

  HPoint3 minVal;
  HPoint3 maxVal;
  calculateBoundingCube(pixPts, &minVal, &maxVal);

  Vector<HPoint3> boxRays = {
    {minVal.x(), maxVal.y(), 1.0f},  // lower left
    {maxVal.x(), maxVal.y(), 1.0f},  // lower right
    {minVal.x(), minVal.y(), 1.0f},  // upper left
    {maxVal.x(), minVal.y(), 1.0f},  // upper right
  };

  auto boxCorners = transformForBox.inv() * boxRays;

  // Return the rectangular bounding box from facemesh. Note that this will get drawn to a
  // square region for processing, with distortion, but that is what facemesh wants.
  d.boundingBox = {
    {boxCorners[0].x(), boxCorners[0].y()},  // upper left
    {boxCorners[1].x(), boxCorners[1].y()},  // upper right
    {boxCorners[2].x(), boxCorners[2].y()},  // lower left
    {boxCorners[3].x(), boxCorners[3].y()},  // lower right
  };

  // at this point, the full face info should be filled out
  results.push_back(d);

  // Get ear landmarks based on detected face mesh
  if (earConfig_.isEnabled) {
    Vector<DetectedPoints> earDetections = earRoisByFaceMesh(d);
    results.insert(results.end(), earDetections.begin(), earDetections.end());
  }

  return results;
}

}  // namespace c8
