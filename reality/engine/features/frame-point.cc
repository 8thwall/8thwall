// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"frame-point.h"};
  deps = {
    "//c8:hmatrix",
    "//c8:c8-log",
    "//c8:parameter-data",
    "//c8:hpoint",
    "//c8:quaternion",
    "//c8:map",
    "//c8:set",
    "//c8:string",
    "//c8:vector",
    "//c8/geometry:egomotion",
    "//c8/geometry:pixel-pinhole-camera-model",
    "//c8/pixels:image-roi",
    "//c8/string:format",
    "//reality/engine/features:image-descriptor",
    "//reality/engine/features:feature-manager",
    "//reality/engine/geometry:epipolar",
    "//reality/engine:xr-id",
  };
}
cc_end(0xf58b6f94);

#include "c8/parameter-data.h"
#include "reality/engine/features/frame-point.h"

namespace c8 {

namespace {
struct Settings {
  // Whether to use useInitialDepth, which holds an initial depth based on whether it's a ground or
  // sky point.
  bool useInitialDepth;
  // If a point has less than this certainty threshold, use the initial depth instead.
  float initialDepthCertaintyThreshold;
  // Distance of the skypoints from the camera in world space.
  float skypointsDistance;
  // How much a depth point's depth estimate can be trusted.
  float knownDepthCertainty;
};

const Settings &settings() {
  static const Settings settings_{
    globalParams().getOrSet("FramePoint.useInitialDepth", true),
    globalParams().getOrSet("FramePoint.initialDepthCertaintyThreshold", 4e-10f),
    globalParams().getOrSet("FramePoint.skypointsDistance", 100.0f),
    globalParams().getOrSet("FramePoint.knownDepthCertainty", 0.95f),
  };
  return settings_;
}
}  // namespace
////////////////////////////////////////////////////////////////////////////////////////////////////
// FramePoint
////////////////////////////////////////////////////////////////////////////////////////////////////
void FramePoint::setId(WorldMapPointId id) {
  id_ = id;
  // If we're clearing the world point, clear other world-point associated data, keeping primary
  // data (like position, angle and scale) intact.  TODO(nb): either clear depth_ here too, or
  // remove depth_ from the API once depthWithCertainty is preferred.
  if (id == WorldMapPointId::nullId()) {
    clearDepths();
  }
}

void FramePoint::recomputeDepth() {
  depthWithCertainty_ = combineDepths<WorldMapKeyframeId>(depthsByKeyframe_);
  if (settings().useInitialDepth) {
    const auto depth = depthWithCertainty_.depthInWords * depthWithCertainty_.certainty
      + initialDepth_ * settings().initialDepthCertaintyThreshold;
    depthWithCertainty_.certainty += settings().initialDepthCertaintyThreshold;
    depthWithCertainty_.depthInWords = depth / depthWithCertainty_.certainty;
  }
}

void FramePoint::clearDepth(WorldMapKeyframeId id) {
  auto found = depthsByKeyframe_.find(id);
  if (found == depthsByKeyframe_.end()) {
    return;
  }
  depthsByKeyframe_.erase(found);

  recomputeDepth();
}

void FramePoint::updatePointDepth(
  const EpipolarDepthImagePrework &iw, const WorldMapKeyframeId &kfId, const FramePoint &kfPt) {
  auto pw = epipolarDepthPointPrework(iw, {x_, y_});
  depthsByKeyframe_[kfId] = depthOfEpipolarPoint(iw, pw, {kfPt.x(), kfPt.y()}, kfPt.scale());

  recomputeDepth();
}

void FramePoint::updateInitialDepth(const HVector3 &scaledGroundPlaneNormal) {
  // Solve for the z component of the ray intersection with the ground plane.
  initialDepth_ = 1.0 / scaledGroundPlaneNormal.dot(HVector3(x_, y_, 1.f));
  // If depth < 0 means we were looking up so intersected with the ground behind us. If very large
  // then we're looking at the horizon. In either case set depth to a large value, like a sky point.
  if (initialDepth_ < 0.0 || initialDepth_ > settings().skypointsDistance) {
    initialDepth_ = settings().skypointsDistance;
  }

  recomputeDepth();
}
void FramePoint::updateInitialDepthDuringDeserialization(double initialDepth) {
  initialDepth_ = initialDepth;
  recomputeDepth();
};

void FramePoint::setKnownDepth(float knownDepth) {
  initialDepth_ = knownDepth;
  depth_ = knownDepth;
  depthWithCertainty_ = {
    knownDepth,
    settings().knownDepthCertainty,
    EpipolarDepthResult::Status::OK,
  };
}

void FramePoint::clearDepths() {
  depthsByKeyframe_.clear();
  // clearDepths() is called when a point is disassociated with a map but it's extrinsic is still
  // the same, so the initialDepth_ should stay the same here.

  recomputeDepth();
}

Vector<WorldMapKeyframeId> FramePoint::keyframeViews() const {
  Vector<WorldMapKeyframeId> kfIds;
  kfIds.reserve(depthsByKeyframe_.size());
  for (const auto &[kfId, depth] : depthsByKeyframe_) {
    kfIds.push_back(kfId);
  }
  return kfIds;
}

String FramePoint::toString() const noexcept {
  return format(
    "(x_: %.4f, y_: %.4f, (depth: %.4f, certainty: %f, samples: %d, initialDepth: %.4f), scale_: "
    "%2u, angle_: %.4f, id_.id(): %11lu, id_.index(): %3zu, [deprecated] depth_: %.2f)",
    x_,
    y_,
    depthWithCertainty_.depthInWords,
    depthWithCertainty_.certainty,
    depthsByKeyframe_.size(),
    initialDepth_,
    scale_,
    angle_,
    id_.id(),
    id_.index(),
    depth_);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// FrameWithPoints
////////////////////////////////////////////////////////////////////////////////////////////////////
void FrameWithPoints::updateIntrinsics(const c8_PixelPinholeCameraModel &intrinsic) {
  rewriteUndistortMatrix(intrinsic);
  for (int i = 0; i < pts_.size(); i++) {
    pts_[i].setPosition(undistort(pixels_[i]));
  }
}

Vector<HPoint2> FrameWithPoints::hpoints() const {
  Vector<HPoint2> retval;
  std::transform(
    pts_.begin(), pts_.end(), std::back_inserter(retval), [](const FramePoint &p) -> HPoint2 {
      return p.position();
    });
  return retval;
}

Vector<HPoint3> FrameWithPoints::worldPoints() const {
  Vector<HPoint3> retval;
  std::transform(
    pts_.begin(), pts_.end(), std::back_inserter(retval), [this](const FramePoint &p) -> HPoint3 {
      return extrinsic_ * p.position3();
    });
  return retval;
}

const Vector<WorldMapPointId> FrameWithPoints::pointIds() const {
  Vector<WorldMapPointId> worldMapPtIds;
  worldMapPtIds.reserve(pts_.size());
  for (const FramePoint &p : pts_) {
    worldMapPtIds.push_back(p.id());
  }
  return worldMapPtIds;
}

void FrameWithPoints::setWorldPoint(size_t selfIdx, WorldMapPointId id) {
  pts_[selfIdx].setId(id);
  if (id != WorldMapPointId::nullId()) {
    worldPtIndices_.insert(selfIdx);
  } else {
    auto keyframeViews = pts_[selfIdx].keyframeViews();
    for (const auto &kfId : keyframeViews) {
      removePointView(kfId, selfIdx);
    }
    worldPtIndices_.erase(selfIdx);
    pts_[selfIdx].clearDepth();
    pts_[selfIdx].clearDepths();
  }
}

void FrameWithPoints::setKnownDepth(size_t selfIdx, float knownDepth) {
  pts_[selfIdx].setKnownDepth(knownDepth);
}

void FrameWithPoints::removePointView(WorldMapKeyframeId kfId, size_t selfIdx) {
  pts_[selfIdx].clearDepth(kfId);
  pointsViewedByKeyframe_[kfId].erase(selfIdx);
  if (pointsViewedByKeyframe_[kfId].empty()) {
    pointsViewedByKeyframe_.erase(kfId);
    depthPrework_.erase(kfId);
  }
}

void FrameWithPoints::addPointView(
  WorldMapKeyframeId kfId, const FrameWithPoints &kf, size_t kfIdx, size_t selfIdx) {
  auto depthPreworkLookup = depthPrework_.find(kfId);
  if (depthPreworkLookup == depthPrework_.end()) {
    depthPreworkLookup =
      depthPrework_.insert({kfId, epipolarDepthImagePrework(extrinsic_, kf.extrinsic())}).first;
  }
  pts_[selfIdx].updatePointDepth(depthPreworkLookup->second, kfId, kf.points()[kfIdx]);
  pointsViewedByKeyframe_[kfId][selfIdx] = kfIdx;
}

void FrameWithPoints::updateCurrentPointViews(
  WorldMapKeyframeId kfId, const FrameWithPoints &kf, bool skipPrework) {
  auto depthPreworkLookup = depthPrework_.find(kfId);
  if (skipPrework && depthPreworkLookup == depthPrework_.end()) {
    C8_THROW(
      "[frame-point@updateCurrentPointViews] Passed in skipPrework=true but lookup of prework "
      "failed.");
  }

  if (!skipPrework) {
    depthPrework_[kfId] = epipolarDepthImagePrework(extrinsic_, kf.extrinsic());
    depthPreworkLookup = depthPrework_.find(kfId);
  }

  for (const auto &[selfIdx, kfIdx] : pointsViewedByKeyframe_[kfId]) {
    pts_[selfIdx].updatePointDepth(depthPreworkLookup->second, kfId, kf.points()[kfIdx]);
  }
}

Vector<WorldMapKeyframeId> FrameWithPoints::keyframesWithViews() const {
  Vector<WorldMapKeyframeId> kfs;
  kfs.reserve(pointsViewedByKeyframe_.size());
  for (const auto &[kfId, pointMatch] : pointsViewedByKeyframe_) {
    kfs.push_back(kfId);
  }
  return kfs;
}

String FrameWithPoints::toString() const noexcept {
  return format(
    "(extrinsic: %s, intrinsic: %s, pts: %d, pixels: %d, worldPtIndices: %d, "
    "xrDevicePose: %s"
    "roi source: %d)",
    extrinsicToString(extrinsic()).c_str(),
    intrinsic().toString().c_str(),
    pts_.size(),
    pixels_.size(),
    worldPtIndices_.size(),
    xrDevicePose_.toString().c_str(),
    roi_.source);
}

void FrameWithPoints::clear() {
  pts_.clear();
  pixels_.clear();
  store_.clear();
  worldPtIndices_.clear();
  depthPrework_.clear();
  pointsViewedByKeyframe_.clear();
}

FrameWithPoints FrameWithPoints::clone() const {
  FrameWithPoints newFrame{{}};
  newFrame.clearAndClone(*this);
  return newFrame;
}

void FrameWithPoints::clearAndClone(const FrameWithPoints &toCopy) {
  intrinsic_ = toCopy.intrinsic_;
  K_ = HMatrixGen::intrinsic(intrinsic_);
  extrinsic_ = toCopy.extrinsic_;
  xrDevicePose_ = toCopy.xrDevicePose_;

  pts_ = toCopy.pts_;
  pixels_ = toCopy.pixels_;

  store_ = toCopy.store_.clone();

  worldPtIndices_ = toCopy.worldPtIndices_;
  depthPrework_ = toCopy.depthPrework_;
  pointsViewedByKeyframe_ = toCopy.pointsViewedByKeyframe_;

  roi_ = toCopy.roi_;
  excludedEdgePixels_ = toCopy.excludedEdgePixels_;
}

FrameWithPoints FrameWithPoints::cloneForNewMap() const {
  FrameWithPoints newFrame{{}};
  newFrame.clearAndClone(*this);
  newFrame.clearTiesToMap();
  return newFrame;
}

void FrameWithPoints::clearTiesToMap() {
  worldPtIndices_ = {};
  depthPrework_ = {};
  pointsViewedByKeyframe_ = {};

  // Unset each FramePoint
  for (auto &pt : pts_) {
    pt.setId(WorldMapPointId::nullId());
    pt.clearDepth();
    pt.clearDepths();
  }
}

void FrameWithPoints::addAll(const FrameWithPoints &toCopy) {
  pts_.reserve(pts_.size() + toCopy.pts_.size());
  for (const auto &pt : toCopy.pts_) {
    pts_.push_back(pt);
  }
  pixels_.reserve(pixels_.size() + toCopy.pixels_.size());
  for (const auto &pixel : toCopy.pixels_) {
    pixels_.push_back(pixel);
  }

  auto beforeNumKeypoints = store_.numKeypoints();
  store_.addKeypoints(toCopy.store().numKeypoints());
  // Copy Orb features
  for (auto it = toCopy.store_.begin<OrbFeature>();
       it != toCopy.store_.end<OrbFeature>();
       ++it) {
    store_.add<OrbFeature>(
      toCopy.store_.get<OrbFeature>(*it).clone(), *it + beforeNumKeypoints);
  }
  // Copy Gorb features
  for (auto it = toCopy.store_.begin<GorbFeature>();
       it != toCopy.store_.end<GorbFeature>();
       ++it) {
    store_.add<GorbFeature>(
      toCopy.store_.get<GorbFeature>(*it).clone(), *it + beforeNumKeypoints);
  }
  // Copy Learned features
  for (auto it = toCopy.store_.begin<LearnedFeature>();
       it != toCopy.store_.end<LearnedFeature>();
       ++it) {
    store_.add<LearnedFeature>(
      toCopy.store_.get<LearnedFeature>(*it).clone(), *it + beforeNumKeypoints);
  }

  for (const auto &i : toCopy.worldPtIndices_) {
    worldPtIndices_.insert(i);
  }
}

}  // namespace c8
