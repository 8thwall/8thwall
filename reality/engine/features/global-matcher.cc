// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"global-matcher.h"};
  deps = {
    ":frame-point",
    ":image-descriptor",
    ":point-descriptor",
    "//c8:parameter-data",
    "//c8:random-numbers",
    "//c8:vector",
    "//c8/stats:scope-timer",
    "//third_party/cvlite/flann:lsh-index",
  };
}
cc_end(0x64256abf);

#include "c8/parameter-data.h"
#include "reality/engine/features/global-matcher.h"

namespace c8 {

namespace {

struct Settings {
  int lshTables;
  int lshKeys;
  int lshMultiprobe;
  int maxNumPointsToConsiderMapMatch;
};

const Settings &settings() {
  static int paramsVersion_ = -1;
  static Settings settings_;
  if (globalParams().version() == paramsVersion_) {
    return settings_;
  }
  settings_ = {
    globalParams().getOrSet("GlobalMatcher.lshTables", 8),
    globalParams().getOrSet("GlobalMatcher.lshKeys", 16),
    globalParams().getOrSet("GlobalMatcher.lshMultiprobe", 2),
    globalParams().getOrSet("GlobalMatcher.maxNumPointsToConsiderMapMatch", 3000),
  };
  paramsVersion_ = globalParams().version();
  return settings_;
}

}  // namespace

// TODO(hzhu): may later need to pass dead point indices to skip over those points
template <typename Descriptor>
void GlobalMatcher<Descriptor>::prepare(const FeatureStore &dictionary, int distanceThreshold) {
  distanceThreshold_ = distanceThreshold;
  ensureQueryPointsPointer(dictionary);
  matcher_.reset(nullptr);

  if (trainPtsArray_->empty<Descriptor>()) {
    return;
  }
  c8flann::LshIndexParams params(
    settings().lshTables, settings().lshKeys, settings().lshMultiprobe);
  matcher_.reset(new c8flann::LshIndex<HammingDistance>(dataset_, params));
  matcher_->buildIndex();
}

template <typename Descriptor>
void GlobalMatcher<Descriptor>::preparePopCountLut(const FeatureStore &dictionary) {
  if (trainPtsArray_ == &dictionary) {
    return;
  }
  trainPtsArray_ = &dictionary;

  popCountToFeatureIdxLut_.resize(Descriptor::size() * 8 + 1);
  for (size_t i = 0; i < popCountToFeatureIdxLut_.size(); ++i) {
    popCountToFeatureIdxLut_[i].clear();
  }

  const auto &features = dictionary.getFeatures<Descriptor>();
  for (size_t i = 0; i < features.size(); ++i) {
    const auto &d = features[i];
    popCountToFeatureIdxLut_[d.totalPopCount()].push_back(i);
  }
}

template <typename Descriptor>
void GlobalMatcher<Descriptor>::match(
  const FrameWithPoints &words, Vector<PointMatch> *matches, int distanceThreshold) const {
  if (!matcher_) {
    return;
  }

  ScopeTimer t("global-match");
  matches->clear();

  const auto &wordsStore = words.store();

  auto nDesc = wordsStore.numDescriptors<Descriptor>();
  if (nDesc == 0) {
    return;
  }
  size_t dsize = Descriptor::size();  // AKA ImagePoints32

  // Allocated, unitialized arrays that clean themselves up.
  std::unique_ptr<int[]> indices(new int[nDesc]);
  std::unique_ptr<int[]> distances(new int[nDesc]);

  {
    // lsh-match
    c8flann::Matrix<uint8_t> query(
      const_cast<uint8_t *>(wordsStore.getFeatures<Descriptor>()[0].data()), nDesc, dsize);
    c8flann::Matrix<int> outInds(indices.get(), nDesc, 1);
    c8flann::Matrix<int> outDists(distances.get(), nDesc, 1);

    matcher_->onnSearch(query, outInds, outDists);
  }

  {
    // Copy to 8th wall match struct.
    matches->reserve(nDesc);
    for (size_t i = 0; i < nDesc; ++i) {
      auto d = distances.get()[i];
      auto idx = indices.get()[i];
      if (idx < 0 || d > distanceThreshold) {
        continue;
      }

      const auto wordKptIdx = words.store().keypointIndices<Descriptor>()[i];
      const auto dictKptIdx = trainPtsArray_->keypointIndices<Descriptor>()[idx];
      const auto &wordPt = words.points()[wordKptIdx];
      const auto &dictionaryPt = trainPts_->points()[dictKptIdx];

      // TODO(nb): check the sign on this; it's inconsistent with others. And normalize.
      // TODO(Riyaan): Maybe switch between angle() and gravity angle() based on the feature type
      auto rotation = wordPt.angle() - dictionaryPt.angle();
      matches->push_back(PointMatch{
        wordKptIdx,
        static_cast<size_t>(dictKptIdx),
        rotation,
        static_cast<float>(d),
        dictionaryPt.scale()});
    }
  }
}

template <typename Descriptor>
void GlobalMatcher<Descriptor>::ensureQueryPointsPointer(const FeatureStore &store) {
  trainPtsArray_ = &store;
  if (trainPtsArray_->empty<Descriptor>()) {
    return;
  }
  size_t dsize = Descriptor::size();  // AKA ImagePoints
  int nDesc = trainPtsArray_->numDescriptors<Descriptor>();
  dataset_ = c8flann::Matrix<uint8_t>(
    const_cast<uint8_t *>(trainPtsArray_->getFeatures<Descriptor>()[0].data()), nDesc, dsize);
  if (matcher_ != nullptr) {
    matcher_->ensureDataset(dataset_);
  }
}

template <typename Descriptor>
void GlobalMatcher<Descriptor>::matchBruteForce(
  const FrameWithPoints &words, Vector<PointMatch> *matches, int distanceThreshold) const {
  ScopeTimer t("global-match-brute-force");
  matches->clear();
  matches->reserve(words.size());

  if (distanceThreshold == 0) {
    distanceThreshold = Descriptor::size() * 8;
  }

  const FeatureStore &wordsStore = words.store();
  const auto &trainFeatures = trainPtsArray_->getFeatures<Descriptor>();

  for (auto i : wordsStore.keypointIndices<Descriptor>()) {
    const auto &d1 = wordsStore.get<Descriptor>(i);
    int idx = -1;
    int minD = distanceThreshold + 1;
    for (size_t j = 0; j < trainFeatures.size(); ++j) {
      const auto &d2 = trainFeatures[j];
      int d = d1.hammingDistance(d2);
      if (minD > d) {
        minD = d;
        idx = j;
      }
    }
    if (idx < 0) {
      continue;
    }

    auto dictKptIdx = trainPtsArray_->keypointIndices<Descriptor>()[idx];

    const auto &wordPt = words.points()[i];
    const auto &dictionaryPt = trainPts_->points()[dictKptIdx];

    // TODO(nb): check the sign on this; it's inconsistent with others. And normalize.
    auto rotation = wordPt.angle() - dictionaryPt.angle();
    matches->push_back(PointMatch{
      i,
      dictKptIdx,
      rotation,
      static_cast<float>(minD),
      dictionaryPt.scale()});
  }
}

template <typename Descriptor>
void GlobalMatcher<Descriptor>::matchMapBruteForce(
  const FrameWithPoints &words,
  const FeatureStore &dictionary,
  int distanceThreshold,
  RandomNumbers *random,
  Vector<PointMatch> *matches) {
  if (dictionary.empty<Descriptor>()) {
    return;
  }
  ScopeTimer t("global-match-map-brute-force");
  matches->clear();
  matches->reserve(words.size());

  if (distanceThreshold <= 0) {
    distanceThreshold = Descriptor::size() * 8;
  }

  const FeatureStore &wordsStore = words.store();
  const auto &trainFeatures = dictionary.getFeatures<Descriptor>();

  static Vector<size_t> dictIndices;
  dictIndices.resize(trainFeatures.size());
  std::iota(dictIndices.begin(), dictIndices.end(), 0);
  random->shuffle(dictIndices.begin(), dictIndices.end());
  dictIndices.resize(
    std::min(dictIndices.size(), static_cast<size_t>(settings().maxNumPointsToConsiderMapMatch)));

  for (size_t i : wordsStore.keypointIndices<Descriptor>()) {
    const auto &d1 = wordsStore.get<Descriptor>(i);
    int idx = -1;
    int minD = distanceThreshold + 1;
    for (const auto &j : dictIndices) {
      const auto &d2 = trainFeatures[j];
      int d = d1.hammingDistance(d2);
      if (minD > d) {
        minD = d;
        idx = j;
      }
    }
    if (idx < 0) {
      continue;
    }

    // TODO(haomin): add rotation and scale once we have meanViewAngle and meanScale in the map
    // point
    matches->push_back(PointMatch{
      .wordsIdx = i,
      .dictionaryIdx = dictionary.keypointIndices<Descriptor>()[idx],
      .descriptorDist = static_cast<float>(minD)});
  }
}

template <typename Descriptor>
void GlobalMatcher<Descriptor>::matchMapPopCount(
  const FrameWithPoints &words,
  Vector<PointMatch> *matches,
  int distanceThreshold,
  int popCountThreshold) const {

  // TODO(Riyaan): Since 1s are more common in Q than 0s, ideally our intervals should skew
  // a bit to the left by a small proportion, which can be dynamically set based on |Q|

  if (popCountToFeatureIdxLut_.empty() || trainPtsArray_ == nullptr || trainPtsArray_->empty<Descriptor>()) {
    return;
  }

  ScopeTimer t("global-match-map-popcount");

  matches->clear();

  if (distanceThreshold <= 0) {
    distanceThreshold = Descriptor::size() * 8;
  }

  if (popCountThreshold <= 0 || popCountThreshold > distanceThreshold) {
    popCountThreshold = distanceThreshold;
  }

  Vector<int> lutIndices;
  lutIndices.reserve(popCountToFeatureIdxLut_.size());

  const auto &wordsStore = words.store();

  for (auto i : wordsStore.keypointIndices<Descriptor>()) {

    const auto &d1 = wordsStore.get<Descriptor>(i);
    int idx = -1;
    int minD = distanceThreshold + 1;

    auto d1PopCount = d1.totalPopCount();
    lutIndices.clear();
    lutIndices.push_back(d1PopCount);

    size_t numCandidatesInPopCountRange = popCountToFeatureIdxLut_[d1PopCount].size();
    // Add lutIndices in an order moving away from d1PopCount (alternating moving left and right)
    for (int popCountDiff = 1; popCountDiff <= popCountThreshold; ++popCountDiff) {
      if (d1PopCount >= popCountDiff) {
        lutIndices.push_back(d1PopCount - popCountDiff);
        numCandidatesInPopCountRange += popCountToFeatureIdxLut_[d1PopCount - popCountDiff].size();
      }
      if (numCandidatesInPopCountRange > settings().maxNumPointsToConsiderMapMatch) {
        break;
      }
      if (d1PopCount + popCountDiff < popCountToFeatureIdxLut_.size()) {
        lutIndices.push_back(d1PopCount + popCountDiff);
        numCandidatesInPopCountRange += popCountToFeatureIdxLut_[d1PopCount + popCountDiff].size();
      }
      if (numCandidatesInPopCountRange > settings().maxNumPointsToConsiderMapMatch) {
        break;
      }
    }

    const auto &features = trainPtsArray_->getFeatures<Descriptor>();
    for (const auto lutIndex : lutIndices) {
      if (std::abs(d1PopCount - lutIndex) > minD) {
        continue;
      }
      // j is the feature index, not the keypoint index
      for (const auto j : popCountToFeatureIdxLut_[lutIndex]) {
        const auto &d2 = features[j];
        int d = d1.hammingDistance(d2);
        if (minD > d) {
          minD = d;
          idx = j;
        }
      }
    }

    if (idx < 0) {
      continue;
    }

    matches->push_back(PointMatch{
      .wordsIdx = i,
      // Get the keypoint index from the feature index
      .dictionaryIdx = trainPtsArray_->keypointIndices<Descriptor>()[idx],
      .descriptorDist = static_cast<float>(minD)});
  }
}

template <typename Descriptor>
void GlobalMatcher<Descriptor>::matchMapBruteForceTopK(
  const FrameWithPoints &words,
  const FeatureStore &dictionary,
  int topK,
  int absoluteThreshold,
  float relativeThresholdRatio,
  Vector<Vector<PointMatch>> *matchesByWordIdx) {
  if (dictionary.empty<Descriptor>()) {
    return;
  }
  BestPointMatches bestMatches(topK);
  matchesByWordIdx->clear();
  matchesByWordIdx->resize(words.size());

  const auto &wordsStore = words.store();

  for (auto i : wordsStore.keypointIndices<Descriptor>()) {
    const auto &d1 = wordsStore.get<Descriptor>(i);
    bestMatches.clear();
    auto &matchesForWord = matchesByWordIdx->at(i);
    for (auto j : dictionary.keypointIndices<Descriptor>()) {
      const auto &d2 = dictionary.get<Descriptor>(j);
      int d = d1.hammingDistance(d2);
      if (d <= absoluteThreshold) {
        bestMatches.push(
          PointMatch{.wordsIdx = i, .dictionaryIdx = j, .descriptorDist = static_cast<float>(d)});
      }
    }

    matchesForWord = bestMatches.sortedMatches();
    if (relativeThresholdRatio < 0.f || matchesForWord.size() < 2) {
      // No relative threshold or not enough matches to apply it
      continue;
    }

    // If there is a relative threshold, remove matches that are not close to the best match
    auto relativeThreshold = matchesForWord[0].descriptorDist * relativeThresholdRatio;
    std::erase_if(matchesForWord, [relativeThreshold](const PointMatch &match) {
      return match.descriptorDist > relativeThreshold;
    });
  }
}

template class GlobalMatcher<OrbFeature>;
template class GlobalMatcher<GorbFeature>;
template class GlobalMatcher<LearnedFeature>;

}  // namespace c8
