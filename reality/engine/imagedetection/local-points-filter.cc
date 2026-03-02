// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Dat Chu (dat@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"local-points-filter.h"};
  deps = {
    "//c8:hpoint",
    "//c8:map",
    "//c8:set",
    "//c8:vector",
    "//c8/geometry:homography",
    "//reality/engine/features:frame-point",
    "//reality/engine/tracking:ray-point-filter",
    "//reality/engine/imagedetection:detection-image",
  };
}
cc_end(0x979f9d29);

#include "c8/geometry/homography.h"
#include "reality/engine/imagedetection/local-points-filter.h"

namespace c8 {
/**
 * @param camRays corresponding camera ray for matches. This is already extracted via wordsIdx
 */
void LocalPointsFilter::apply(const Vector<PointMatch> &matches, Vector<HPoint2> *camRays) {
  unseenIds_.clear();
  if (ptIdToFilters_.empty()) {
    for (int i = 0; i < matches.size(); i++) {
      const auto &dictionaryIdx = matches[i].dictionaryIdx;
      const auto &pt = (*camRays)[i];
      ptIdToFilters_.emplace(std::make_pair(dictionaryIdx, pt));
    }
    return;
  }

  // We need to keep track of the points we haven't seen in this update
  for (auto &ptIdFilterPair : ptIdToFilters_) {
    unseenIds_.insert(ptIdFilterPair.first);
  }

  for (int i = 0; i < matches.size(); i++) {
    const auto dictionaryIdx = matches[i].dictionaryIdx;
    const auto pt = (*camRays)[i];
    unseenIds_.erase(dictionaryIdx);

    auto filter = ptIdToFilters_.find(dictionaryIdx);
    if (filter != ptIdToFilters_.end()) {
      (*camRays)[i] = filter->second.filter(pt);
    } else {
      ptIdToFilters_.emplace(std::make_pair(dictionaryIdx, pt));
    }
  }
}

void LocalPointsFilter::reset() {
  ptIdToFilters_.clear();
  unseenIds_.clear();
}

void LocalPointsFilter::updateUnseenPlanar(const DetectionImage &im, const HMatrix &foundPose) {
  auto H = homographyForPlane(
    foundPose,
    // This is the plane parallel to the xy plane at z = 1
    getScaledPlaneNormal({0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 1.0f}));
  // map unseen target points in target ray space to search ray space
  // this is the inverse of `projectToTargetSpace` in location.cc
  auto &targetPoints = im.framePoints().points();
  for (auto &id : unseenIds_) {
    // unseen target points in target ray space
    auto targetPoint = targetPoints[id].position();
    auto ray = (H * HPoint3{targetPoint.x(), targetPoint.y(), 1.0f}).flatten();
    ptIdToFilters_.at(id).filter(ray);
  }
}

void LocalPointsFilter::updateUnseenObject(
  const DetectionImage &im, const FrameWithPoints &searchFrame, const HMatrix &foundPose) {
  if (unseenIds_.size() == 0) {
    return;
  }

  auto &geom = im.getGeometry();
  auto pixels = im.framePoints().pixels();
  Vector<HPoint2> imPts;
  imPts.reserve(unseenIds_.size());
  Vector<size_t> ids;
  ids.reserve(unseenIds_.size());
  for (auto &id : unseenIds_) {
    imPts.push_back(pixels[id]);
    ids.push_back(id);
  }
  Vector<HPoint3> worldPts;
  worldPts.reserve(unseenIds_.size());
  mapToGeometryPoints(geom, imPts, &worldPts);
  auto predictedPts3 = foundPose.inv() * worldPts;
  for (int i = 0; i < ids.size(); i++) {
    auto &predictedPt3 = predictedPts3[i];
    auto id = ids[i];
    // update the filter with predicted data in this frame so it is ready for the next frame
    ptIdToFilters_.at(id).filter(predictedPt3.flatten());
  }
}

}  // namespace c8
