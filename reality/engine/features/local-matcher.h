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

namespace c8 {

// A local matcher builds a datastructure for efficiently performing local search for features. The
// algorithm can be summarized as:
//
// * Build a query map to find points from a first image within a radius of a specified poiont.
// * For each point in a second image
// *  - Find the set of all points in image 1 within the radius of that point in image 2.
// *  - Find the most similar two points in that set by image descriptor hamming distance.
// *  - If the most similar point is a significantly better match than the second most similar
//      point, return a match between the first and second image.

// Stores data about the matches.
struct MatchResultFrequency {
  // Word specific sums.  So for trackMapFrame(), this records how a single projected map point was
  // able to match against all the features of the current frame.  If it's able to match at all,
  // we'll +1 to "match".  If the current frame's point was later found to be a better match
  // against a separate projected map point, then we'll -1 to "match" and +1 to "foundBetterMatch".
  int match = 0;
  int unmatched = 0;
  int failedScaleTest = 0;
  int failedRatioTest = 0;
  int foundBetterMatch = 0;
  int suppressed = 0;
  // Word-Dictionary Pair-Wise Sums for "unmatched". So if a word was not able to match against a
  // single feature in the dictionary, then we'll sum all the pair-wise reasons it failed for each
  // attempt.  If a word matched with any point, then none of the failures will be added here.
  int unmatchedPairWiseBadScale = 0;
  int unmatchedPairWiseBadRadius = 0;
  int unmatchedPairWiseBadDist = 0;

  bool operator==(const MatchResultFrequency &r) const {
    return match == r.match && unmatched == r.unmatched && failedScaleTest == r.failedScaleTest
      && failedRatioTest == r.failedRatioTest && foundBetterMatch == r.foundBetterMatch
      && suppressed == r.suppressed && unmatchedPairWiseBadScale == r.unmatchedPairWiseBadScale
      && unmatchedPairWiseBadRadius == r.unmatchedPairWiseBadRadius
      && unmatchedPairWiseBadDist == r.unmatchedPairWiseBadDist;
  }

  MatchResultFrequency operator+(const MatchResultFrequency &r) const {
    MatchResultFrequency result;
    result.match = match + r.match;
    result.unmatched = unmatched + r.unmatched;
    result.failedScaleTest = failedScaleTest + r.failedScaleTest;
    result.failedRatioTest = failedRatioTest + r.failedRatioTest;
    result.foundBetterMatch = foundBetterMatch + r.foundBetterMatch;
    result.suppressed = suppressed + r.suppressed;
    result.unmatchedPairWiseBadScale = unmatchedPairWiseBadScale + r.unmatchedPairWiseBadScale;
    result.unmatchedPairWiseBadRadius = unmatchedPairWiseBadRadius + r.unmatchedPairWiseBadRadius;
    result.unmatchedPairWiseBadDist = unmatchedPairWiseBadDist + r.unmatchedPairWiseBadDist;
    return result;
  }
};

template <typename Descriptor>
class LocalMatcher {
public:
  static constexpr float RATIO_TEST_TH = 0.78;

  // Reasonably tuned performance thresholds.
  LocalMatcher() : map_(15, 20), radius_(0.1f), descriptorDistance_(64) {}
  LocalMatcher(int nx, int ny) : map_(nx, ny), radius_(0.1f), descriptorDistance_(64) {}

  LocalMatcher(int nx, int ny, float radius)
      : map_(nx, ny), radius_(radius), descriptorDistance_(std::numeric_limits<float>::max()) {}

  LocalMatcher(int nx, int ny, float radius, float descriptorDistance)
      : map_(nx, ny), radius_(radius), descriptorDistance_(descriptorDistance) {}

  void useRatioTest(bool useRatioTest) { useRatioTest_ = useRatioTest; }

  void useScaleFilter(bool useScaleFilter) { useScaleFilter_ = useScaleFilter; }
  void setDescriptorThreshold(float dist) { descriptorDistance_ = dist; }
  void setSearchRadius(float radius) { radius_ = radius; }

  // Finds all local matches between the first and second frame.
  void match(
    const FrameWithPoints &words, const FrameWithPoints &dictionary, Vector<PointMatch> *matches) {
    ScopeTimer t("local-match");
    // build query map.
    setQueryPointsPointer(dictionary);

    // find matches
    findMatches(words, matches);
  }

  /**
   * Match using the built map. Make sure to call setQueryPointsPointer() before you call this
   * method.
   */
  void findMatches(const FrameWithPoints &words, Vector<PointMatch> *matches) const {
    MatchResultFrequency matchResultFrequency;
    return findMatches(words, matches, &matchResultFrequency);
  }

  void findMatches(
    const FrameWithPoints &words,
    Vector<PointMatch> *matches,
    MatchResultFrequency *matchResultFrequency) const {
    return findMatches(words.points(), words.store(), matches, matchResultFrequency);
  }

  void findMatches(
    const Vector<FramePoint> &points,
    const FeatureStore &wordsStore,
    Vector<PointMatch> *matches) const {
    MatchResultFrequency matchResultFrequency;
    return findMatches(points, wordsStore, matches, &matchResultFrequency);
  }

  void findMatches(
    const Vector<FramePoint> &points,
    const FeatureStore &wordsStore,
    Vector<PointMatch> *matches,
    MatchResultFrequency *matchResultFrequency) const;

  // Build the query data structure for find best match.
  void setQueryPointsPointer(const FrameWithPoints &pts) {
    pts_ = &pts;
    HPoint2 mn;
    HPoint2 mx;
    pts_->frameBoundsExcludingEdge(&mn, &mx);
    map_.reset<FramePoint>(
      mn.x(), mn.y(), mx.x(), mx.y(), pts.points(), &pts.store().keypointIndices<Descriptor>());
  }

  // Make sure the query points pointer is at the current location. These must be the points that
  // were originally set. This is a hacky way to recover from FrameWithPoints being moved.
  // TODO(nb): come up with a more robust solution here.
  void ensureQueryPointsPointer(const FrameWithPoints &pts) { pts_ = &pts; }

  // Find the closest point in the query set to this point. Returns false if no best match is found.
  bool findBestMatch(
    const FramePoint &pt,
    const Descriptor &desc,
    PointMatch *match,
    MatchResultFrequency *matchResultFrequency) const;

  const FrameWithPoints *dictionary() const { return pts_; }

private:
  bool useRatioTest_ = true;
  bool useScaleFilter_ = false;
  BinMap map_;
  const FrameWithPoints *pts_ = nullptr;
  float radius_ = 0.0f;
  float descriptorDistance_ = std::numeric_limits<float>::max();

  void findNearestTwoNeighbors(
    const FramePoint &pt,
    const Descriptor &desc,
    PointMatch *first,
    PointMatch *second,
    MatchResultFrequency *matchResultFrequency) const;
};

extern template class LocalMatcher<OrbFeature>;
extern template class LocalMatcher<GorbFeature>;
extern template class LocalMatcher<LearnedFeature>;

}  // namespace c8
