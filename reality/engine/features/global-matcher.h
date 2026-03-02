// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include <algorithm>
#include <cfloat>

#include "c8/random-numbers.h"
#include "c8/stats/scope-timer.h"
#include "c8/vector.h"
#include "reality/engine/features/frame-point.h"
#include "reality/engine/features/image-descriptor.h"
#include "reality/engine/features/point-descriptor.h"
#include "third_party/cvlite/flann/lsh-index.h"

namespace c8 {

namespace {

constexpr int DIST_TH_GLOBAL = 64;

}  // namespace

template <typename Descriptor>
class GlobalMatcher {
public:
  GlobalMatcher() = default;

  GlobalMatcher(GlobalMatcher &&) = default;
  GlobalMatcher &operator=(GlobalMatcher &&) = default;

  GlobalMatcher(const GlobalMatcher &) = delete;
  GlobalMatcher &operator=(const GlobalMatcher &) = delete;

  void match(const FrameWithPoints &words, Vector<PointMatch> *matches) const {
    match(words, matches, distanceThreshold_);
  }

  void matchBruteForce(
    const FrameWithPoints &words, Vector<PointMatch> *matches, int distanceThreshold) const;

  static void matchMapBruteForce(
    const FrameWithPoints &words,
    const FeatureStore &dictionary,
    int distanceThreshold,
    RandomNumbers *random,
    Vector<PointMatch> *matches);

  void matchMapPopCount(
    const FrameWithPoints &words,
    Vector<PointMatch> *matches,
    int distanceThreshold,
    int popCountThreshold = -1) const;

  /** Returns up to top K matches for each word, such that the matches are below the absolute
   * threshold, and are within relativeThresholdRatio of the best match.
   * @param words The words to match.
   * @param dictionary The dictionary to match against.
   * @param topK The maximum number of matches to return for each word.
   * @param absoluteThreshold The maximum distance for a match to be considered.
   * @param relativeThresholdRatio The runner-up matches must be within this ratio of the best
   * match. The parameter must be in range [1, inf). If negative, no relative threshold is used.
   * @param matchesByWordIdx The output matches, indexed by word index.
   */
  static void matchMapBruteForceTopK(
    const FrameWithPoints &words,
    const FeatureStore &dictionary,
    int topK,
    int absoluteThreshold,
    float relativeThresholdRatio,
    Vector<Vector<PointMatch>> *matchesByWordIdx);

  void prepare(const FeatureStore &dictionary, int distanceThreshold = DIST_TH_GLOBAL);
  void prepare(const FrameWithPoints &dictionary, int distanceThreshold = DIST_TH_GLOBAL) {
    trainPts_ = &dictionary;
    prepare(dictionary.store(), distanceThreshold);
  }
  void preparePopCountLut(const FeatureStore &dictionary);

  void match(
    const FrameWithPoints &words, Vector<PointMatch> *matches, int distanceThreshold) const;

  void ensureQueryPointsPointer(const FeatureStore &pts);

  // Make sure the query points pointer is at the current location. These must be the points that
  // were originally set. This is a hacky way to recover from TargetWithPoints being moved.
  // TODO(nb): come up with a more robust solution here.
  void ensureQueryPointsPointer(const FrameWithPoints &pts) {
    trainPts_ = &pts;
    ensureQueryPointsPointer(pts.store());
  }

  static void match(
    const FrameWithPoints &words,
    const FrameWithPoints &dictionary,
    Vector<PointMatch> *matches,
    int distanceThreshold) {
    ScopeTimer t("global-matcher-match");
    GlobalMatcher<Descriptor> matcher;
    matcher.prepare(dictionary, distanceThreshold);
    matcher.match(words, matches, distanceThreshold);
  }

  static void match(
    const FrameWithPoints &words, const FrameWithPoints &dictionary, Vector<PointMatch> *matches) {
    ScopeTimer t("global-matcher-match");
    GlobalMatcher<Descriptor>::match(words, dictionary, matches, DIST_TH_GLOBAL);
  }

private:
  typedef ::c8flann::HammingPopcount HammingDistance;
  std::unique_ptr<c8flann::LshIndex<HammingDistance>> matcher_;
  c8flann::Matrix<uint8_t> dataset_;
  const FrameWithPoints *trainPts_ = nullptr;
  const FeatureStore *trainPtsArray_ = nullptr;
  int distanceThreshold_;

  Vector<Vector<size_t>> popCountToFeatureIdxLut_;
};  // namespace c8

extern template class GlobalMatcher<OrbFeature>;
extern template class GlobalMatcher<GorbFeature>;
extern template class GlobalMatcher<LearnedFeature>;

}  // namespace c8
