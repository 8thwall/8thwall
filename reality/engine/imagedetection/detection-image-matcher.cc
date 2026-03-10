// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"detection-image-matcher.h"};
  deps = {
    ":target-point",
    "//c8:parameter-data",
    "//c8:vector",
    "//c8/stats:scope-timer",
    "//reality/engine/features:frame-point",
    "//reality/engine/features:image-descriptor",
    "//third_party/cvlite/flann:lsh-index",
  };
}
cc_end(0xc12f9b11);

#include "c8/parameter-data.h"
#include "reality/engine/imagedetection/detection-image-matcher.h"

namespace c8 {

namespace {

struct Settings {
  int lshTables;
  int lshKeys;
  int lshMultiprobe;
};

const Settings &settings() {
  static const Settings settings_{
    globalParams().getOrSet("DetectionImageMatcher.lshTables", 8),
    globalParams().getOrSet("DetectionImageMatcher.lshKeys", 16),
    globalParams().getOrSet("DetectionImageMatcher.lshMultiprobe", 2),
  };
  return settings_;
}

}  // namespace

void DetectionImageMatcher::prepare(const TargetWithPoints &targetPoints, int distanceThreshold) {
  ensureQueryPointsPointer(targetPoints);

  distanceThreshold_ = distanceThreshold;
  matcher_.reset(nullptr);

  if (trainPts_->points().size() == 0) {
    return;
  }

  c8flann::LshIndexParams params(
    settings().lshTables, settings().lshKeys, settings().lshMultiprobe);
  matcher_.reset(new c8flann::LshIndex<HammingDistance>(dataset_, params));
  matcher_->buildIndex();
}

void DetectionImageMatcher::match(const FrameWithPoints &imagePoints, Vector<PointMatch> *matches) {
  match(imagePoints, distanceThreshold_, matches);
}

void DetectionImageMatcher::match(
  const FrameWithPoints &imagePoints, int distanceThreshold, Vector<PointMatch> *matches) {
  if (!matcher_) {
    return;
  }
  ScopeTimer t("global-match");
  matches->clear();
  int n = 0;
  if (featureType_ == DescriptorType::ORB) {
    n = imagePoints.store().keypointIndices<OrbFeature>().size();
  } else if (featureType_ == DescriptorType::GORB) {
    n = imagePoints.store().keypointIndices<GorbFeature>().size();
  } else {
    C8_THROW(
      "[detection-image-matcher@match] Unsupported feature type: %d",
      static_cast<int>(featureType_));
    return;
  }

  if (n == 0) {
    return;
  }

  // Allocated, unitialized arrays that clean themselves up.
  std::unique_ptr<int[]> indices(new int[n]);
  std::unique_ptr<int[]> distances(new int[n]);

  {
    c8flann::Matrix<int> outInds(indices.get(), n, 1);
    c8flann::Matrix<int> outDists(distances.get(), n, 1);
    // lsh-match
    if (featureType_ == DescriptorType::ORB) {
      c8flann::Matrix<uint8_t> query(
        const_cast<uint8_t *>(imagePoints.store().getFeatures<OrbFeature>()[0].data()),
        n,
        OrbFeature::size());

      matcher_->onnSearch(query, outInds, outDists);
    } else if (featureType_ == DescriptorType::GORB) {
      c8flann::Matrix<uint8_t> query(
        const_cast<uint8_t *>(imagePoints.store().getFeatures<GorbFeature>()[0].data()),
        n,
        GorbFeature::size());

      matcher_->onnSearch(query, outInds, outDists);
    } else {
      C8_THROW(
        "[detection-image-matcher@match] Unsupported feature type: %d",
        static_cast<int>(featureType_));
      return;
    }
  }

  {
    // output
    // Copy to 8th wall match struct.
    matches->reserve(n);
    for (size_t i = 0; i < n; ++i) {
      auto d = distances.get()[i];
      auto idx = indices.get()[i];
      if (idx < 0 || d > distanceThreshold) {
        continue;
      }
      size_t wordKptIdx = 0;
      if (featureType_ == DescriptorType::ORB) {
        wordKptIdx = imagePoints.store().keypointIndices<OrbFeature>()[i];
      } else if (featureType_ == DescriptorType::GORB) {
        wordKptIdx = imagePoints.store().keypointIndices<GorbFeature>()[i];
      } else {
        C8_THROW(
          "[detection-image-matcher@match] Unsupported feature type: %d",
          static_cast<int>(featureType_));
        return;
      }
      const auto &wordPt = imagePoints.points()[wordKptIdx];
      const auto &dictionaryPt = trainPts_->points()[idx];

      const auto &firstPoint = imagePoints.points()[i];
      // TODO(nb): check the sign on this; it's inconsistent with others. And normalize.
      // TODO(Riyaan): Maybe switch between angle() and gravity angle() based on the feature type
      float rotation;
      if (featureType_ == DescriptorType::ORB) {
        rotation = wordPt.angle() - dictionaryPt.angle();
      } else if (featureType_ == DescriptorType::GORB) {
        rotation = wordPt.gravityAngle() - dictionaryPt.gravityAngle();
      } else {
        C8_THROW(
          "[detection-image-matcher@match] Unsupported feature type: %d",
          static_cast<int>(featureType_));
        return;
      }
      matches->push_back(
        PointMatch{
          wordKptIdx,
          static_cast<size_t>(idx),
          rotation,
          static_cast<float>(d),
          firstPoint.scale()});
    }
  }
}

// Make sure the query points pointer is at the current location. These must be the points that
// were originally set. This is a hacky way to recover from TargetWithPoints being moved.
// TODO(nb): come up with a more robust solution here.
void DetectionImageMatcher::ensureQueryPointsPointer(const TargetWithPoints &pts) {
  trainPts_ = &pts;
  featureType_ = pts.featureType();
  if (featureType_ == DescriptorType::ORB) {
    auto n = trainPts_->store().numDescriptors<OrbFeature>();
    if (n == 0) {
      return;
    }
    dataset_ = c8flann::Matrix<uint8_t>(
      const_cast<uint8_t *>(trainPts_->store().getFeatures<OrbFeature>()[0].data()),
      n,
      OrbFeature::size());
  } else if (featureType_ == DescriptorType::GORB) {
    auto n = trainPts_->store().numDescriptors<GorbFeature>();
    if (n == 0) {
      return;
    }
    dataset_ = c8flann::Matrix<uint8_t>(
      const_cast<uint8_t *>(trainPts_->store().getFeatures<GorbFeature>()[0].data()),
      n,
      GorbFeature::size());
  } else {
    C8_THROW(
      "[detection-image-matcher@ensureQueryPointsPointer] Unsupported feature type: %d",
      static_cast<int>(featureType_));
    return;
  }
  if (matcher_ != nullptr) {
    matcher_->ensureDataset(dataset_);
  }
}

}  // namespace c8
