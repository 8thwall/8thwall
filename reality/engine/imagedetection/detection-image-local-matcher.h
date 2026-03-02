// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include <algorithm>
#include <cfloat>

#include "c8/map.h"
#include "c8/stats/scope-timer.h"
#include "c8/vector.h"
#include "reality/engine/binning/linear-bin.h"
#include "reality/engine/features/frame-point.h"
#include "reality/engine/features/image-descriptor.h"
#include "reality/engine/imagedetection/target-point.h"

namespace c8 {

// A TargetBin divides the bounds of an image (in ray space) into nx column bins and ny row bins.
// For example, a 640x480 image with focal length 625 has bounds [[-.512, .512), [-.384, .384)].
// If nx = 20, then each column bin has width .0384. If ny = 30, then each row bin has height .0341.
// Bins are numbered in row-major order. In the example above,
// * The point [-.384, -.512,] would have bin 0.
// * The point [.384, -.512] would have bin number 19.
// * The point [-.384, .512] would have bin number 580.
// * The point [.384, .512] would have bin number 599.
class TargetBin {
public:
  TargetBin() = default;
  TargetBin(HPoint2 mn, HPoint2 mx, size_t nx, size_t ny) {
    nx_ = nx;
    x_ = LinearBin{nx, mn.x(), mx.x()};
    y_ = LinearBin{ny, mn.y(), mx.y()};
  }

  // Compute the bin number of a ray in ray-space.
  size_t binNum(HPoint2 p) { return binNum(xBin(p.x()), yBin(p.y())); }

  size_t binNumPt(float x, float y) { return binNum(xBin(x), yBin(y)); }

  // Compute the bin number given a known row bin number (y) and a known col
  // bin number (x).
  size_t binNum(size_t x, size_t y) { return y * nx_ + x; }

  // Compute the x-dmension bin number of a point.
  size_t xBin(float v) { return x_.binNum(v); }

  // Compute the y-dimension bin number of a point.
  size_t yBin(float v) { return y_.binNum(v); }

private:
  LinearBin x_;
  LinearBin y_;
  size_t nx_ = 1;
};

// A bin map is a data structure for querying frame points near a specified point. Calling
// TargetBinMap::reset on a frame builds a queryable map of points. Subsequently, calling
// getPointsInRadiusEager returns pointers to points near the query point. This method is
// eager in the sense that all points within the radius are returned, but some points outside the
// radius are returned as well. A further test is needed to check that the points are actually
// within the desired radius.
struct TargetBinMap {
public:
  TargetBinMap(int nx, int ny) : map_(nx * ny), map2_(nx * ny), nx_(nx), ny_(ny) {};

  void reset(
    HPoint2 mn, HPoint2 mx, const Vector<TargetPoint> &points, const Vector<size_t> *indices) {
    bin_ = TargetBin(mn, mx, nx_, ny_);
    for (auto &v : map_) {
      v.clear();
    }
    for (auto &v : map2_) {
      v.clear();
    }
    if (indices) {
      for (auto idx : *indices) {
        auto pt = points[idx];
        size_t b = bin_.binNumPt(pt.x(), pt.y());
        map_[b].push_back(idx);
        // Scale 2 is a special scale for image targets because it is closest to the ROI size.
        if (pt.scale() == 2) {
          map2_[b].push_back(idx);
        }
      }
    } else {
      for (size_t i = 0; i < points.size(); i++) {
        auto pt = points[i];
        size_t b = bin_.binNumPt(pt.x(), pt.y());
        map_[b].push_back(i);
        // Scale 2 is a special scale for image targets because it is closest to the ROI size.
        if (pt.scale() == 2) {
          map2_[b].push_back(i);
        }
      }
    }
  }

  // public for inlining.
  Vector<Vector<size_t>> map_;
  Vector<Vector<size_t>> map2_;
  TargetBin bin_;

private:
  int nx_ = 1;
  int ny_ = 1;
};

// A local matcher builds a datastructure for efficiently performing local search for features. The
// algorithm can be summarized as:
//
// * Build a query map to find points from a first image within a radius of a specified point.
// * For each point in a second image
// *  - Find the set of all points in image 1 within the radius of that point in image 2.
// *  - Find the most similar two points in that set by image descriptor hamming distance.
// *  - If the most similar point is a significantly better match than the second most similar
//      point, return a match between the first and second image.
struct DetectionImageLocalMatcher {
  static constexpr float RATIO_TEST_TH = 0.78;

  DetectionImageLocalMatcher(int nx, int ny, float radius)
      : map_(nx, ny), radius_(radius), descriptorDistance_(std::numeric_limits<float>::max()) {}

  DetectionImageLocalMatcher(int nx, int ny, float radius, float descriptorDistance)
      : map_(nx, ny), radius_(radius), descriptorDistance_(descriptorDistance) {}

  void useRatioTest(bool useRatioTest) { useRatioTest_ = useRatioTest; }

  void useScaleFilter(bool useScaleFilter) { useScaleFilter_ = useScaleFilter; }
  void setDescriptorThreshold(float dist) { descriptorDistance_ = dist; }
  void setRoiScale(bool roiScale) { roiScale_ = roiScale; }
  void setRadius(float radius) { radius_ = radius; }

  /**
   * Match using the built map. Make sure to call setQueryPointsPointer() before you call this
   * method.
   */
  void findMatches(const FrameWithPoints &points, Vector<PointMatch> *matches) {
    matches->clear();
    if (featureType_ == DescriptorType::ORB) {
      for (auto i : points.store().keypointIndices<OrbFeature>()) {
        PointMatch m;
        if (findBestMatch<OrbFeature>(points.points()[i], points.store().get<OrbFeature>(i), &m)) {
          m.wordsIdx = i;
          matches->push_back(m);
        }
      }
    } else if (featureType_ == DescriptorType::GORB) {
      for (auto i : points.store().keypointIndices<GorbFeature>()) {
        PointMatch m;
        if (findBestMatch<GorbFeature>(
              points.points()[i], points.store().get<GorbFeature>(i), &m)) {
          m.wordsIdx = i;
          matches->push_back(m);
        }
      }
    } else {
      C8_THROW(
        "[detection-image-local-matcher@findMatches] Unsupported feature type: %d",
        static_cast<int>(featureType_));
      return;
    }
  }

  // Build the query data structure for find best match.
  void setQueryPointsPointer(const TargetWithPoints &pts) {
    pts_ = &pts;
    HPoint2 mn;
    HPoint2 mx;
    pts_->frameBoundsExcludingEdge(&mn, &mx);
    featureType_ = pts.featureType();
    if (featureType_ == DescriptorType::ORB) {
      map_.reset(mn, mx, pts_->points(), &pts_->store().keypointIndices<OrbFeature>());
    } else if (featureType_ == DescriptorType::GORB) {
      map_.reset(mn, mx, pts_->points(), &pts_->store().keypointIndices<GorbFeature>());
    } else {
      C8_THROW(
        "[detection-image-local-matcher@setQueryPointsPointer] Unsupported feature type: %d",
        static_cast<int>(featureType_));
      return;
    }
  }

  // Make sure the query points pointer is at the current location. These must be the points that
  // were originally set. This is a hacky way to recover from TargetWithPoints being moved.
  // TODO(nb): come up with a more robust solution here.
  void ensureQueryPointsPointer(const TargetWithPoints &pts) { pts_ = &pts; }

  // Find the closest point in the query set to this point. Returns false if no best match is found.
  template <typename Descriptor>
  bool findBestMatch(const FramePoint &pt, const Descriptor &desc, PointMatch *match) {
    PointMatch second;

    findNearestTwoNeighbors(pt, desc, match, &second);

    if (useRatioTest_ && match->descriptorDist >= RATIO_TEST_TH * second.descriptorDist) {
      return false;
    }

    // Post-reject matches at a scale distance that is too far. If we're at this point, it means
    // that the best descriptor matched across a 2x (1.44^2) scaling factor, which means it's
    // probably a bad match.
    //
    // NOTE(nb): pre-filtering with useScaleFilter_ is currently disabled (useScaleFilter_ is forced
    // to always be false) because post-filtering appears to give reliably better results. This
    // might be because the descriptor was ambiguous to start with, and this provides a mechanism
    // to filter it from future consideration.
    auto scaleDistance = std::abs(pt.scale() - match->dictionaryScale);
    if (scaleDistance > 1) {
      return false;
    }

    if (match->dictionaryIdx == -1) {
      return false;
    }

    return true;
  }

private:
  DescriptorType featureType_ = DescriptorType::ORB;
  bool useRatioTest_ = true;
  bool useScaleFilter_ = false;
  bool roiScale_ = false;
  TargetBinMap map_;
  const TargetWithPoints *pts_ = nullptr;
  float radius_ = 0.0f;
  float descriptorDistance_ = std::numeric_limits<float>::max();

  template <typename Descriptor>
  void findNearestTwoNeighbors(
    const FramePoint &pt, const Descriptor &desc, PointMatch *first, PointMatch *second) {
    auto ptx = pt.x();
    auto pty = pt.y();

    const auto &pts = pts_->points();
    const auto &dictStore = pts_->store();

    *first = PointMatch();
    *second = PointMatch();
    float r2 = radius_ * radius_;
    const auto &grid = roiScale_ ? map_.map2_ : map_.map_;

    // inline map_.getPointsInRadiusEager
    size_t xmin = map_.bin_.xBin(ptx - radius_);
    size_t xmax = map_.bin_.xBin(ptx + radius_);
    size_t ymin = map_.bin_.yBin(pty - radius_);
    size_t ymax = map_.bin_.yBin(pty + radius_);
    for (size_t y = ymin; y <= ymax; y++) {
      for (size_t x = xmin; x <= xmax; x++) {
        auto bin = map_.bin_.binNum(x, y);
        for (auto i : grid[bin]) {
          const auto &pt2 = pts[i];

          if (useScaleFilter_ && pt2.scale() != pt.scale()) {
            continue;
          }

          // Consider only the points within the radius.
          auto xd = ptx - pt2.x();
          auto yd = pty - pt2.y();
          auto d2 = (xd * xd) + (yd * yd);
          if (r2 < d2) {
            continue;
          }

          // Compute the descriptor distance.
          auto hamDist = desc.hammingDistance(dictStore.get<Descriptor>(i));

          // reject if hamming distance is too large
          if (hamDist > descriptorDistance_) {
            continue;
          }

          // Set the descriptor to second best, or to first best (replacing second best).
          if (hamDist < second->descriptorDist) {
            second->dictionaryIdx = i;
            second->dictionaryScale = pt2.scale();
            second->descriptorDist = hamDist;
            second->rotation = 0.0f;
          }

          if (second->descriptorDist < first->descriptorDist) {
            std::swap(*first, *second);
          }
        }
      }
    }
  }
};

}  // namespace c8
