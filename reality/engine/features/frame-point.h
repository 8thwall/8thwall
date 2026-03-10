// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)
//
// FrameWithPoints holds a collection FramePoint's.

#pragma once

#include <cfloat>

#include "c8/geometry/egomotion.h"
#include "c8/geometry/pixel-pinhole-camera-model.h"
#include "c8/hmatrix.h"
#include "c8/hpoint.h"
#include "c8/map.h"
#include "c8/pixels/image-roi.h"
#include "c8/quaternion.h"
#include "c8/set.h"
#include "c8/string.h"
#include "c8/string/format.h"
#include "c8/vector.h"
#include "reality/engine/features/feature-manager.h"
#include "reality/engine/features/image-descriptor.h"
#include "reality/engine/geometry/epipolar.h"
#include "reality/engine/xr-id.h"

namespace c8 {

constexpr uint8_t SCALE_UNKNOWN = 255;

// The mental model for a point match is "looking up words in a dictionary." The dictionary is
// often a stable, larger reference set that may be queried multiple times with new words. Depending
// on the matching implementation, multiple words may be matched (synonyms) to the same dictionary
// entry.
struct PointMatch {
  size_t wordsIdx = -1;
  size_t dictionaryIdx = -1;
  float rotation = FLT_MAX;
  float descriptorDist = FLT_MAX;
  uint8_t dictionaryScale = SCALE_UNKNOWN;

  String toString() const noexcept {
    return format(
      "(wordsIdx: %3zu, dictionaryIdx: %3zu, rotation: %.4f, descriptorDist: %.4f, "
      "dictionaryScale: %u)",
      wordsIdx,
      dictionaryIdx,
      rotation,
      descriptorDist,
      dictionaryScale);
  }
};

// A class which helps efficiently maintain the best k point matches i.e. the k point matches with
// the lowest descriptor distances.
class BestPointMatches {
public:
  BestPointMatches(size_t k) : k_(k) { matches_.reserve(k); }

  // Push a new match into the list of best matches. If the list is not full, the match is always
  // added.
  void push(const PointMatch &match) {
    if (matches_.size() < k_) {
      matches_.push_back(match);
      std::push_heap(matches_.begin(), matches_.end(), comparator_);
    } else if (match.descriptorDist < matches_.front().descriptorDist) {
      std::pop_heap(matches_.begin(), matches_.end(), comparator_);
      matches_.pop_back();
      matches_.push_back(match);
      std::push_heap(matches_.begin(), matches_.end(), comparator_);
    }
  }

  // Returns a copy of the best matches, sorted ascendingly by descriptor distance.
  Vector<PointMatch> sortedMatches() {
    // Copy the matches and sort them.
    Vector<PointMatch> sortedMatches = matches_;
    std::sort_heap(sortedMatches.begin(), sortedMatches.end(), comparator_);
    return sortedMatches;
  }

  void clear() { matches_.clear(); }

private:
  size_t k_;
  Vector<PointMatch> matches_;
  static constexpr struct {
    bool operator()(const PointMatch &lhs, const PointMatch &rhs) const {
      return lhs.descriptorDist < rhs.descriptorDist;
    }
  } comparator_;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// FramePoint
//
// FramePoints are points as viewed by in an image (frame). FramePoints are always represented in
// ray-space, not pixel-space. This removes the effect of any inter-frame variability in camera
// intrinsics, and generally simplifies math.
class FramePoint {
public:
  FramePoint() = default;

  // Initialize a frame point with purely two-d information.
  FramePoint(HPoint2 position, uint8_t scaleIn, float angleIn, float gravityAngleIn) noexcept
      : scale_(scaleIn),
        angle_(angleIn),
        gravityAngle_(gravityAngleIn),
        x_(position.x()),
        y_(position.y()) {}

  // Initialize a frame point with two-d and three-d information.
  FramePoint(
    HPoint2 position,
    uint8_t scaleIn,
    float angleIn,
    float gravityAngleIn,
    WorldMapPointId id,
    float depth) noexcept
      : scale_(scaleIn),
        angle_(angleIn),
        gravityAngle_(gravityAngleIn),
        id_(id),
        depth_(depth),
        x_(position.x()),
        y_(position.y()) {}

  ~FramePoint() = default;

  // Default move constructors.
  FramePoint(FramePoint &&) = default;
  FramePoint &operator=(FramePoint &&) = default;

  // Allow copying.
  FramePoint(const FramePoint &) = default;
  FramePoint &operator=(const FramePoint &) = default;

  // 2D/3D position of the point.
  HPoint2 position() const { return {x_, y_}; }
  void setPosition(const HPoint2 &position) {
    x_ = position.x();
    y_ = position.y();
  }
  HPoint3 position3() const { return {x_ * depth_, y_ * depth_, depth_}; }
  HPoint3 initialPosition3() const {
    return {
      static_cast<float>(x_ * initialDepth_),
      static_cast<float>(y_ * initialDepth_),
      static_cast<float>(initialDepth_)};
  }

  // Fast accessors for 2D position.
  constexpr float x() const { return x_; }
  constexpr float y() const { return y_; }

  // Scale of the point.
  uint8_t scale() const { return scale_; };

  // Angle of the point.
  float angle() const { return angle_; };
  // Gravity angle of the point.
  float gravityAngle() const { return gravityAngle_; }

  // Return the id of this point in a world map.
  WorldMapPointId id() const { return id_; }
  void setId(WorldMapPointId id);

  // Get the depth of this point from the camera.
  void setDepth(float depth) { depth_ = depth; }
  void clearDepth() { depth_ = 1.0f; }
  float depth() const { return depth_; }
  void updateInitialDepthDuringDeserialization(double initialDepth);

  void clearDepth(WorldMapKeyframeId id);

  void updatePointDepth(
    const EpipolarDepthImagePrework &iw, const WorldMapKeyframeId &kfId, const FramePoint &kfPt);
  // Update the initial depth estimate of the point given the scaled ground plane normal.
  void updateInitialDepth(const HVector3 &scaledGroundPlaneNormal);
  // Set known depth if associated with a DEPTH point.
  void setKnownDepth(float knownDepth);

  void clearDepths();

  Vector<WorldMapKeyframeId> keyframeViews() const;

  const EpipolarDepthResult &depthWithCertainty() const { return depthWithCertainty_; }
  double initialDepth() const { return initialDepth_; }

  const auto &depthsByKeyframe() const { return depthsByKeyframe_; }

  String toString() const noexcept;

private:
  void recomputeDepth();

  // The scale (octave) for the keypoint.
  uint8_t scale_ = 0;

  // The oriented angle for the keypoint.
  float angle_ = 0.0f;
  // The angle of the 2d reprojection of the up-vector in world space
  float gravityAngle_ = 0.0f;

  // The id of a point in a world map, if known.
  WorldMapPointId id_ = WorldMapPointId::nullId();

  // The depth of a point from the camera, if known.
  float depth_ = 1.0f;

  // The x/y position of the point in ray space, corresponding to the eccentricity at distance
  // 1.
  float x_ = 0.0f;
  float y_ = 0.0f;

  TreeMap<WorldMapKeyframeId, EpipolarDepthResult> depthsByKeyframe_;
  EpipolarDepthResult depthWithCertainty_;
  // The initial estimate of depth, either the depth of a ray intersecting with the ground plane,
  // or a far off depth in the sky, or provided given a DEPTH point position.
  double initialDepth_ = 1.0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// FrameWithPoints
//
// A frame with points is the collection of points that are visible from a frame. The frame can be
// captured (in pixels) or synthesized (e.g. as a view on a map). The points in a FrameWithPoints
// are always stored in ray-space, not pixel-space. This removes the effect of any inter-frame
// variability in camera intrinsics, and generally simplifies math. The frame itself stores the
// camera model needed to recover the pixel locations of points if needed.  This is generally not
// needed, except in debugging, to overlay detected points on a frame.
class FrameWithPoints {
public:
  FrameWithPoints(c8_PixelPinholeCameraModel intrinsic)
      : intrinsic_(intrinsic),
        K_(HMatrixGen::intrinsic(intrinsic)),
        extrinsic_(HMatrixGen::i()),
        xrDevicePose_(1.0f, 0.0f, 0.0f, 0.0f) {}

  FrameWithPoints(c8_PixelPinholeCameraModel intrinsic, const HMatrix &extrinsic)
      : intrinsic_(intrinsic),
        K_(HMatrixGen::intrinsic(intrinsic)),
        extrinsic_(extrinsic),
        xrDevicePose_(1.0f, 0.0f, 0.0f, 0.0f) {}

  FrameWithPoints(
    c8_PixelPinholeCameraModel intrinsic, const HMatrix &extrinsic, Quaternion xrDevicePose)
      : intrinsic_(intrinsic),
        K_(HMatrixGen::intrinsic(intrinsic)),
        extrinsic_(extrinsic),
        xrDevicePose_(xrDevicePose) {}

  ~FrameWithPoints() = default;

  // Default move constructors.
  FrameWithPoints(FrameWithPoints &&) = default;
  FrameWithPoints &operator=(FrameWithPoints &&) = default;

  // Disallow copying.
  FrameWithPoints(const FrameWithPoints &) = delete;
  FrameWithPoints &operator=(const FrameWithPoints &) = delete;

  // Add a point with location specified in pixels, typically from a feature detector. Internally
  // this is converted into a ray-based representation.
  void addImagePixelPoint(
    HPoint2 position, uint8_t scale, float angle, float gravityAngle, FeatureSet &&features) {
    store_.append(std::move(features));
    pts_.emplace_back(undistort(position), scale, angle, gravityAngle);
    pixels_.push_back(position);
  }

  // Add a point with location specified in rays.
  void addImageRayPoint(
    HPoint2 position, uint8_t scale, float angle, float gravityAngle, FeatureSet &&features) {
    store_.append(std::move(features));
    pts_.emplace_back(position, scale, angle, gravityAngle);
    pixels_.push_back(distort(position));
  }

  // Updates the intrinsics and recomputes points in ray space.
  void updateIntrinsics(const c8_PixelPinholeCameraModel &intrinsic);

  // Add a point with location specified in world coordinate.
  // also storing the point ID, keyframe ID, and its initial depth with respect to this frame
  // Internally this is converted into a ray-based representation.
  // Note that points IDs, keyframe IDs, and descriptors are indexed together.
  void addWorldMapPoint(
    const WorldMapPointId &id,
    const HPoint3 &worldPosition,
    uint8_t scale,
    float angle,
    float gravityAngle,
    FeatureSet &&descriptors) {
    auto proj = extrinsic_.inv() * worldPosition;
    auto projFlatten = proj.flatten();
    addWorldMapPoint(
      {projFlatten, scale, angle, gravityAngle, id, proj.z()},
      distort(projFlatten),
      std::move(descriptors));
  }

  // Adds a world point from a precomputed FramePoint and pixel location.  NOTE(akul): Should
  // always prefer the above `addWorldMapPoint()` API unless this is absolutely needed.
  // @param pt a precomputed FramePoint from another frame. Note that this frame's `extrisnsic_` has
  // to equal that of the of the frame that the passed in pt was created from otherwise the added pt
  // will be wrong.
  // @param pixel the pixel location of the pt. Note this frame's `extrisnsic_` and `K_` has to
  // equal that of the of the frame that the passed in pt was created from, otherwise the added
  // pixel location will be wrong.
  void addWorldMapPoint(const FramePoint &pt, const HPoint2 &pixel, FeatureSet &&features) {
    pts_.emplace_back(pt);
    worldPtIndices_.insert(pts_.size() - 1);
    pixels_.push_back(pixel);
    store_.append(std::move(features));
  }

  // Access to the intrinsic model.
  c8_PixelPinholeCameraModel intrinsic() const { return intrinsic_; };

  void rewriteUndistortMatrix(const c8_PixelPinholeCameraModel &intrinsic) {
    intrinsic_ = intrinsic;
    K_ = HMatrixGen::intrinsic(intrinsic_);
  }

  // Get and set the extrinsic model.
  const HMatrix &extrinsic() const { return extrinsic_; };

  // Set the extrinsic model on the frame.
  void setExtrinsic(const HMatrix &extrinsic) {
    extrinsic_ = extrinsic;
    auto scaledGroundPlaneNormal = groundPointTriangulationPrework(extrinsic);
    for (auto &pt : pts_) {
      pt.updateInitialDepth(scaledGroundPlaneNormal);
    }
  };

  // Called if the frame point is associated with a depth point, where we'll know its depth.
  void setKnownDepth(size_t selfIdx, float knownDepth);

  // Get and set the device pose.
  const Quaternion &xrDevicePose() const { return xrDevicePose_; };
  void setXRDevicePose(Quaternion q) { xrDevicePose_ = q; }

  // Access to the Point vector.
  const Vector<FramePoint> &points() const { return pts_; }
  Vector<FramePoint> *mutablePoints() { return &pts_; }

  // Access to the Point pixels.
  const Vector<HPoint2> &pixels() const { return pixels_; }

  const FeatureStore &store() const { return store_; }
  FeatureStore *mutableStore() { return &store_; }

  void frameBounds(HPoint2 *lowerLeft, HPoint2 *upperRight) const {
    ::c8::frameBounds(intrinsic_, lowerLeft, upperRight);
  }
  void frameBoundsExcludingEdge(HPoint2 *lowerLeft, HPoint2 *upperRight) const {
    ::c8::frameBounds(intrinsic_, excludedEdgePixels_, lowerLeft, upperRight);
  }

  // Get points in ray coordinates.
  Vector<HPoint2> hpoints() const;
  // Get points in world coordinates. Note that some points will not be world points so will not
  // have depth.
  Vector<HPoint3> worldPoints() const;
  // Get ids for points
  const Vector<WorldMapPointId> pointIds() const;

  // Indices in points which are world points.
  const TreeSet<size_t> &worldPointIndices() const { return worldPtIndices_; }

  // Reserves space for points, pixels, and descriptors.
  void reserve(size_t size) {
    pts_.reserve(size);
    store_.reserveKeypoints(size);
    // TODO(Riyaan): Remove hardcoded feature type.
    store_.reserveFeatures<OrbFeature>(size);
    pixels_.reserve(size);
  }
  size_t size() const { return pts_.size(); }
  size_t empty() const { return pts_.empty(); }

  // Convert from pixel space to ray space.
  HPoint2 undistort(HPoint2 pt) const { return (K_.inv() * pt.extrude()).flatten(); }
  // Convert from ray space to pixel space.
  HPoint2 distort(HPoint2 ray) const { return (K_ * ray.extrude()).flatten(); }

  // Given a ray with depth in the frame, get position in world space.
  //
  // @param ray A ray in this frame.
  // @param depth The depth of the point along this ray.
  HPoint3 positionInWorld(HPoint2 ray, float depth) const {
    return extrinsic_ * HPoint3(ray.x() * depth, ray.y() * depth, depth);
  }
  HPoint3 positionInWorld(HPoint2 ray, double depth) const {
    return positionInWorld(ray, static_cast<float>(depth));
  }

  // Associates a world map point with a point in this frame.
  //
  // @param selfIdx The index of the point in this frame that is associated with the world point.
  // @param id The id of the world point.
  void setWorldPoint(size_t selfIdx, WorldMapPointId id);

  void setRoi(const ImageRoi &roi) { roi_ = roi; }
  const ImageRoi &roi() const { return roi_; }

  void setExcludedEdgePixels(int pix) { excludedEdgePixels_ = pix; }

  // Remove an external keyframe's view of a point in this keyframe and update the point's depth
  // information accordingly.
  //
  // @param kfId The id of a different keyframe that previously viewed a point in this frame.
  // @param selfIdx The point index in this frame that was previously viewed by `kfId`.
  void removePointView(WorldMapKeyframeId kfId, size_t selfIdx);

  // Add an external keyframe's view of a point in this keyframe and update the point's depth
  // information accordingly.
  //
  // @param kfId The id of a different keyframe that views a point in this frame.
  // @param kf The keyframe that corresponds to `kfId`.
  // @param kfIdx The point index in `kf` that corresponds to the point at `selfIdx` in this frame.
  // @param selfIdx The point index in this frame that was previously viewed by `kfId`.
  void addPointView(
    WorldMapKeyframeId kfId, const FrameWithPoints &kf, size_t kfIdx, size_t selfIdx);

  // Updates all of the keyframe's associated point views with a new triangulation value.
  // When this keyframe or any keyframe with associated views changes its position, we need to
  // update our estimate of point depth from all associated views.
  //
  // @param kfId The id of the keyframe whose views need updating.
  // @param kf The keyframe whose views need updating.
  // @param skipPrework set to true when there is no change to the kf extrinsic to avoid
  // recomputing the same preWork again.
  void updateCurrentPointViews(
    WorldMapKeyframeId kfId, const FrameWithPoints &kf, bool skipPrework = false);

  // Get a list of keyframe ids that have views of points in this keyframe.
  Vector<WorldMapKeyframeId> keyframesWithViews() const;

  const auto &depthPrework() const { return depthPrework_; }
  const auto &pointsViewedByKeyframe() const { return pointsViewedByKeyframe_; }

  void clear();

  FrameWithPoints clone() const;
  void clearAndClone(const FrameWithPoints &toCopy);

  // Copy the FrameWithPoints but doesn't contain data that associates it to a previous map. This
  // means it can be associated with a new map.
  FrameWithPoints cloneForNewMap() const;

  void addAll(const FrameWithPoints &toCopy);

  String toString() const noexcept;

private:
  // Removes any ties to the map.
  void clearTiesToMap();

  // Feature points within the keyframe, stored in ray space.
  Vector<FramePoint> pts_;
  // Feature points within the keyframe, stored in pixel space.
  Vector<HPoint2> pixels_;
  // Descriptors for the feature points.
  FeatureStore store_;

  // Helpers for computing depths of points through triangulation. For these helpers, the current
  // frame is considered "words" and the other frames are considered "dictionary".
  TreeMap<WorldMapKeyframeId, EpipolarDepthImagePrework> depthPrework_;
  // For each keyframe, keep a map where the key is the index of the point in this keyframe and
  // the value is the index of the point in the other keyframe.
  TreeMap<WorldMapKeyframeId, TreeMap<size_t, size_t>> pointsViewedByKeyframe_;

  // Only a subset of pts_ are world points.
  // Indices of points within pts_ that are world points.
  TreeSet<size_t> worldPtIndices_;

  // Calibration parameters
  c8_PixelPinholeCameraModel intrinsic_;
  HMatrix K_;

  // 3d position of the camera, if known.
  HMatrix extrinsic_;

  Quaternion xrDevicePose_;

  ImageRoi roi_;

  int excludedEdgePixels_ = 0;
};

}  // namespace c8
