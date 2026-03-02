// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"local-matcher.h"};
  deps = {
    ":frame-point",
    ":image-descriptor",
    "//c8:hpoint",
    "//c8:map",
    "//c8:vector",
    "//c8/stats:scope-timer",
    "//reality/engine/binning:linear-bin",
  };
}
cc_end(0x253de0d9);

#include "reality/engine/features/local-matcher.h"

namespace c8 {

template <typename Descriptor>
void LocalMatcher<Descriptor>::findMatches(
  const Vector<FramePoint> &points,
  const FeatureStore &wordsStore,
  Vector<PointMatch> *matches,
  MatchResultFrequency *matchResultFrequency) const {
  TreeMap<size_t, PointMatch> dedupMatches;
  TreeSet<size_t> suppressMatches;
  const auto &keypointIndices = wordsStore.keypointIndices<Descriptor>();
  const auto &descriptors = wordsStore.getFeatures<Descriptor>();
  int nDesc = wordsStore.numDescriptors<Descriptor>();
  matches->clear();
  *matchResultFrequency = {};

  for (int descIdx = 0; descIdx < nDesc; ++descIdx) {
    PointMatch m;
    if (findBestMatch(
          points[keypointIndices[descIdx]], descriptors[descIdx], &m, matchResultFrequency)) {
      m.wordsIdx = keypointIndices[descIdx];
      auto found = dedupMatches.find(m.dictionaryIdx);
      if (found == dedupMatches.end()) {
        // If we haven't seen this match before, add it to the list.
        dedupMatches[m.dictionaryIdx] = m;
      } else {
        // If there is an existing version of this match, suppress it if something else has
        // exactly the same score. Otherwise, if it's better than the one we already had, keep the
        // new version and make sure it's not suppressed.
        if (found->second.descriptorDist == m.descriptorDist) {
          suppressMatches.insert(m.dictionaryIdx);
          ++(matchResultFrequency->suppressed);
          --(matchResultFrequency->match);
        } else if (found->second.descriptorDist > m.descriptorDist) {
          dedupMatches[m.dictionaryIdx] = m;
          ++(matchResultFrequency->foundBetterMatch);
          --(matchResultFrequency->match);
        } else {
          // We found a lesser match.
          --(matchResultFrequency->match);
        }
      }
    }
  }

  matches->clear();
  matches->reserve(dedupMatches.size());
  for (const auto &m : dedupMatches) {
    // Only add points that aren't marked as suppressed.
    if (suppressMatches.find(m.first) == suppressMatches.end()) {
      matches->push_back(m.second);
    }
  }
}

template <typename Descriptor>
bool LocalMatcher<Descriptor>::findBestMatch(
  const FramePoint &pt,
  const Descriptor &desc,
  PointMatch *match,
  MatchResultFrequency *matchResultFrequency) const {
  PointMatch second;

  findNearestTwoNeighbors(pt, desc, match, &second, matchResultFrequency);

  if (match->dictionaryIdx == -1) {
    ++(matchResultFrequency->unmatched);
    return false;
  }

  // If Ratio test is requested, reject if the second highest descriptor is too close to the
  // second one. Even if ratio test is not requested, reject if there are multiple matches with
  // the exact same score.
  auto rejectThreshold =
    useRatioTest_ ? RATIO_TEST_TH * second.descriptorDist : second.descriptorDist;
  if (match->descriptorDist >= rejectThreshold) {
    ++(matchResultFrequency->failedRatioTest);
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
  auto scaleDistance =
    std::abs(static_cast<int>(pt.scale()) - static_cast<int>(match->dictionaryScale));
  if (scaleDistance > 1 && match->dictionaryScale != SCALE_UNKNOWN && pt.scale() != SCALE_UNKNOWN) {
    ++(matchResultFrequency->failedScaleTest);
    return false;
  }

  ++(matchResultFrequency->match);
  return true;
}

template <typename Descriptor>
void LocalMatcher<Descriptor>::findNearestTwoNeighbors(
  const FramePoint &pt,
  const Descriptor &desc,
  PointMatch *first,
  PointMatch *second,
  MatchResultFrequency *matchResultFrequency) const {
  auto ptx = pt.x();
  auto pty = pt.y();

  const auto &pts = pts_->points();
  const auto &dictStore = pts_->store();

  *first = PointMatch();
  *second = PointMatch();
  float r2 = radius_ * radius_;
  const auto &grid = map_.map_;

  // inline map_.getPointsInRadiusEager
  size_t xmin = map_.bin_.xBin(ptx - radius_);
  size_t xmax = map_.bin_.xBin(ptx + radius_);
  size_t ymin = map_.bin_.yBin(pty - radius_);
  size_t ymax = map_.bin_.yBin(pty + radius_);

  size_t badScale = 0;
  size_t badRadius = 0;
  size_t badDist = 0;
  bool foundMatch = false;
  for (size_t y = ymin; y <= ymax; y++) {
    for (size_t x = xmin; x <= xmax; x++) {
      auto bin = map_.bin_.binNum(x, y);
      for (auto i : grid[bin]) {
        const auto &pt2 = pts[i];

        if (useScaleFilter_ && pt2.scale() != pt.scale()) {
          ++badScale;
          continue;
        }

        // Consider only the points within the radius.
        auto xd = ptx - pt2.x();
        auto yd = pty - pt2.y();
        auto d2 = (xd * xd) + (yd * yd);
        if (r2 < d2) {
          ++badRadius;
          continue;
        }

        // Compute the descriptor distance.
        auto hamDist = desc.hammingDistance(dictStore.get<Descriptor>(i));

        // reject if hamming distance is too large
        if (hamDist > descriptorDistance_) {
          ++badDist;
          continue;
        }

        foundMatch = true;
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

  if (!foundMatch) {
    matchResultFrequency->unmatchedPairWiseBadScale += badScale;
    matchResultFrequency->unmatchedPairWiseBadRadius += badRadius;
    matchResultFrequency->unmatchedPairWiseBadDist += badDist;
  }
}

template class LocalMatcher<OrbFeature>;
template class LocalMatcher<GorbFeature>;
template class LocalMatcher<LearnedFeature>;

}  // namespace c8
