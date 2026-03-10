// Copyright (c) 2022 8th Wall, LLC
// Original Author: Yuyan Song (yuyansong@nianticlabs.com)

#include "bzl/inliner/rules2.h"
cc_library {
  hdrs = {
    "detection-anchor-nms.h",
  };
  deps = {
    "//c8:c8-log",
    "//c8:hvector",
    "//c8:vector",
    "//c8/geometry:box2",
    "//c8/stats:scope-timer",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xa394e1fa);

#include <cmath>

#include "c8/c8-log.h"
#include "c8/geometry/box2.h"
#include "c8/stats/scope-timer.h"
#include "reality/engine/deepnets/detection-anchor-nms.h"

namespace c8 {

float calculateScale(float minScale, float maxScale, float strideIndex, float numStrides) {
  return minScale + (maxScale - minScale) * 1.0f * strideIndex / (numStrides - 1.0f);
}

// Generate anchors for SSD object detection model.
Vector<Anchor> generateAnchors(const AnchorOptions &options) {
  Vector<Anchor> anchors;
  int layerId = 0;
  while (layerId < options.strides.size()) {
    Vector<float> anchorHeight;
    Vector<float> anchorWidth;
    Vector<float> aspectRatios;
    Vector<float> scales;

    // For same strides, we merge the anchors in the same order.
    int lastSameStrideLayer = layerId;
    while (lastSameStrideLayer < options.strides.size()
           && options.strides[lastSameStrideLayer] == options.strides[layerId]) {
      auto scale = calculateScale(
        options.minScale, options.maxScale, lastSameStrideLayer, options.strides.size());
      if (lastSameStrideLayer == 0 && options.reduceBoxesInLowestLayer) {
        // For first layer, it can be specified to use predefined anchors.
        aspectRatios.push_back(1.0f);
        aspectRatios.push_back(2.0f);
        aspectRatios.push_back(0.5f);
        scales.push_back(0.1f);
        scales.push_back(scale);
        scales.push_back(scale);
      } else {
        for (int aspectRatioId = 0; aspectRatioId < options.aspectRatios.size(); ++aspectRatioId) {
          aspectRatios.push_back(options.aspectRatios[aspectRatioId]);
          scales.push_back(scale);
        }
        if (options.interpolatedScaleAspectRatio > 0) {
          float scaleNext = lastSameStrideLayer == options.strides.size() - 1
            ? 1.0f
            : calculateScale(
              options.minScale, options.maxScale, lastSameStrideLayer + 1, options.strides.size());
          scales.push_back(std::sqrt(scale * scaleNext));
          aspectRatios.push_back(options.interpolatedScaleAspectRatio);
        }
      }
      lastSameStrideLayer++;
    }

    for (int i = 0; i < aspectRatios.size(); ++i) {
      float ratioSqrts = std::sqrt(aspectRatios[i]);
      anchorHeight.push_back(scales[i] / ratioSqrts);
      anchorWidth.push_back(scales[i] * ratioSqrts);
    }

    int featureMapHeight = 0;
    int featureMapWidth = 0;
    if (options.featureMapHeight.size()) {
      featureMapHeight = options.featureMapHeight[layerId];
      featureMapWidth = options.featureMapWidth[layerId];
    } else {
      int stride = options.strides[layerId];
      featureMapHeight = std::ceil(1.0f * options.inputSizeHeight / stride);
      featureMapWidth = std::ceil(1.0f * options.inputSizeWidth / stride);
    }

    for (int y = 0; y < featureMapHeight; ++y) {
      for (int x = 0; x < featureMapWidth; ++x) {
        for (int anchorId = 0; anchorId < anchorHeight.size(); ++anchorId) {
          anchors.push_back({
            (x + options.anchorOffsetX) * 1.0f / featureMapWidth,
            (y + options.anchorOffsetY) * 1.0f / featureMapHeight,
            options.fixedAnchorSize ? 1.0f : anchorWidth[anchorId],
            options.fixedAnchorSize ? 1.0f : anchorHeight[anchorId],
          });
        }
      }
    }
    layerId = lastSameStrideLayer;
  }

  return anchors;
}

Vector<float> decodeBoxes(
  const float *rawBoxes, const Vector<Anchor> &anchors, const ProcessOptions &options) {
  if (!rawBoxes) {
    return {};
  }

  const int numCoords = options.numCoords;
  Vector<float> boxes(numCoords * options.numBoxes);
  for (int i = 0; i < options.numBoxes; ++i) {
    auto boxOffset = i * numCoords + options.boxCoordOffset;

    auto yCenter = rawBoxes[boxOffset];
    auto xCenter = rawBoxes[boxOffset + 1];

    auto h = rawBoxes[boxOffset + 2];
    auto w = rawBoxes[boxOffset + 3];

    if (options.reverseOutputOrder) {
      xCenter = rawBoxes[boxOffset];
      yCenter = rawBoxes[boxOffset + 1];
      w = rawBoxes[boxOffset + 2];
      h = rawBoxes[boxOffset + 3];
    }

    xCenter = xCenter / options.xScale * anchors[i].w + anchors[i].xCenter;
    yCenter = yCenter / options.yScale * anchors[i].h + anchors[i].yCenter;

    if (options.applyExponentialOnBoxSize) {
      h = std::exp(h / options.hScale) * anchors[i].h;
      w = std::exp(w / options.wScale) * anchors[i].w;
    } else {
      h = h / options.hScale * anchors[i].h;
      w = w / options.wScale * anchors[i].w;
    }

    auto ymin = yCenter - h / 2.0f;
    auto xmin = xCenter - w / 2.0f;
    auto ymax = yCenter + h / 2.0f;
    auto xmax = xCenter + w / 2.0f;

    boxes[i * numCoords + 0] = ymin;
    boxes[i * numCoords + 1] = xmin;
    boxes[i * numCoords + 2] = ymax;
    boxes[i * numCoords + 3] = xmax;

    for (int k = 0; k < options.numKeypoints; ++k) {
      auto offset = i * numCoords + options.keypointCoordOffset + k * options.numValuesPerKeypoint;

      auto keypointY = rawBoxes[offset];
      auto keypointX = rawBoxes[offset + 1];
      if (options.reverseOutputOrder) {
        keypointX = rawBoxes[offset];
        keypointY = rawBoxes[offset + 1];
      }

      boxes[offset] = keypointX / options.xScale * anchors[i].w + anchors[i].xCenter;
      boxes[offset + 1] = keypointY / options.yScale * anchors[i].h + anchors[i].yCenter;
    }
  }
  return boxes;
}

Detection convertToDetection(
  float boxYmin,
  float boxXmin,
  float boxYmax,
  float boxXmax,
  float score,
  int classId,
  bool flipVertically) {
  return {
    score,
    classId,
    {
      "relativeBoundingBox",
      {
        boxXmin,
        flipVertically ? 1.0f - boxYmax : boxYmin,
        boxXmax - boxXmin,
        boxYmax - boxYmin,
      },
      {},
    }};
}

Vector<Detection> convertToDetections(
  const Vector<float> &detectionBoxes,
  const Vector<float> &detectionScores,
  const Vector<int> &detectionClasses,
  const ProcessOptions &options) {
  const int numBoxes = options.numBoxes;
  const int numCoords = options.numCoords;
  Vector<Detection> outputDetections;
  for (int i = 0; i < numBoxes; ++i) {
    if (options.minScoreThresh && detectionScores[i] < options.minScoreThresh) {
      continue;
    }
    auto boxOffset = i * numCoords;
    auto detection = convertToDetection(
      detectionBoxes[boxOffset + 0],
      detectionBoxes[boxOffset + 1],
      detectionBoxes[boxOffset + 2],
      detectionBoxes[boxOffset + 3],
      detectionScores[i],
      detectionClasses[i],
      options.flipVertically);
    // Add keypoints.
    auto maxPts = options.numKeypoints * options.numValuesPerKeypoint;
    for (int kpId = 0; kpId < maxPts; kpId += options.numValuesPerKeypoint) {
      auto keypointIndex = boxOffset + options.keypointCoordOffset + kpId;
      detection.locationData.relativeKeypoints.push_back({
        detectionBoxes[keypointIndex + 0],
        detectionBoxes[keypointIndex + 1],
      });
    }
    outputDetections.push_back(detection);
  }
  return outputDetections;
}

// Convert result TFLite tensors from object detection models into MediaPipe Detections.
Vector<Detection> processDetections(
  const float *rawBoxes,
  const float *rawScores,
  const Vector<Anchor> &anchors,
  const ProcessOptions &options) {
  auto boxes = decodeBoxes(rawBoxes, anchors, options);

  const int numClasses = options.numClasses;
  const int numBoxes = options.numBoxes;
  Vector<float> detectionScores(numBoxes);
  Vector<int> detectionClasses(numBoxes);

  // Filter classes by scores.
  for (int i = 0; i < numBoxes; ++i) {
    int classId = -1;
    float maxScore = std::numeric_limits<float>::lowest();
    // Find the top score for box i.
    for (int scoreIdx = 0; scoreIdx < numClasses; ++scoreIdx) {
      float score = rawScores[i * numClasses + scoreIdx];
      if (options.sigmoidScore) {
        if (options.scoreClippingThresh) {
          score = score < -options.scoreClippingThresh ? -options.scoreClippingThresh : score;
          score = score > options.scoreClippingThresh ? options.scoreClippingThresh : score;
        }
        // TODO(nb): use simple threshold, only transform score if passes.
        score = 1.0f / (1.0f + std::exp(-score));
      }
      if (maxScore < score) {
        maxScore = score;
        classId = scoreIdx;
      }
    }
    detectionScores[i] = maxScore;
    detectionClasses[i] = classId;
  }

  return convertToDetections(boxes, detectionScores, detectionClasses, options);
}

// Get the index of the detection that has the max confidence
int getMaxDetection(const Vector<Detection> &detections) {
  // calculate the detection with the highest confidence
  auto maxConfidenceScore = detections[0].score;
  auto maxConfidenceIndex = 0;
  auto idx = 0;
  for (const auto &d : detections) {
    if (d.score > maxConfidenceScore) {
      maxConfidenceScore = d.score;
      maxConfidenceIndex = idx;
    }
    ++idx;
  }

  return maxConfidenceIndex;
}

Vector<Detection> weightedNonMaxSuppression(
  const Vector<Detection> &inDetections, float iouThreshold) {
  auto detections = inDetections;
  Vector<Detection> prunedDetections;

  Vector<Detection> candidates;
  candidates.reserve(detections.size());
  Vector<Detection> remaining;
  remaining.reserve(detections.size());

  while (!detections.empty()) {
    auto maxConfidenceIndex = getMaxDetection(detections);
    auto maxConfidenceDetection = detections[maxConfidenceIndex];

    // remove the detection with the highest confidence
    const auto &maxBoundingBox = maxConfidenceDetection.locationData.relativeBoundingBox;

    // Go through each detection.  If the IOU of the detection and the detection with the
    // highest confidence is over the 'iouThreshold', then remove that item
    candidates.clear();
    remaining.clear();

    for (const auto &detection : detections) {
      const auto &bboxA = detection.locationData.relativeBoundingBox;
      if (intersectionOverUnion(bboxA, maxBoundingBox) > iouThreshold) {
        candidates.push_back(detection);
      } else {
        remaining.push_back(detection);
      }
    }

    // if there are candidates, create a weighted detection that is made up of the values
    // of the many candidates multiplied by their confidence
    // NOTE(nb): candidates should never be empty.
    if (!candidates.empty()) {
      float wXmin = 0.0f;
      float wYmin = 0.0f;
      float wXmax = 0.0;
      float wYmax = 0.0;
      float totalScore = 0.0;

      Vector<RelativeKeypoint> keypoints(
        maxConfidenceDetection.locationData.relativeKeypoints.size());

      for (const auto &candidate : candidates) {
        auto score = candidate.score;
        totalScore += score;
        auto bbox = candidate.locationData.relativeBoundingBox;
        wXmin += bbox.x * score;
        wYmin += bbox.y * score;
        wXmax += (bbox.x + bbox.w) * score;
        wYmax += (bbox.y + bbox.h) * score;

        int idx = 0;
        for (const auto &keypt : candidate.locationData.relativeKeypoints) {
          auto &wpt = keypoints[idx];
          wpt = {wpt.x() + keypt.x() * score, wpt.y() + keypt.y() * score};
          idx++;
        }
      }

      Detection weightedDetection = maxConfidenceDetection;  // initialize non-geometric fields.
      auto &weightedLocation = weightedDetection.locationData;

      auto wt = 1.0f / totalScore;
      // Now that we have summed up bounding box and keypoint data, we need to divide them
      // by the total score
      weightedLocation.relativeBoundingBox = {
        wXmin * wt, wYmin * wt, (wXmax - wXmin) * wt, (wYmax - wYmin) * wt};

      int idx = 0;
      for (auto &kpt : weightedDetection.locationData.relativeKeypoints) {
        kpt = {keypoints[idx].x() * wt, keypoints[idx].y() * wt};
        ++idx;
      }

      prunedDetections.push_back(weightedDetection);
    }

    detections = remaining;
  }

  return prunedDetections;
}

}  // namespace c8
