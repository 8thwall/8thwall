// Copyright (c) 2024 Niantic, Inc.
// Original Author: Riyaan Bakhda (riyaanbakhda@nianticlabs.com)
//
// Feature Type utilities for the engine

#pragma once

#include <array>
#include <limits>
#include <optional>
#include <tuple>

#include "c8/exceptions.h"
#include "c8/vector.h"
#include "reality/engine/features/image-descriptor.h"

namespace c8 {

namespace {

constexpr const size_t NO_INDEX = std::numeric_limits<size_t>::max();

}  // namespace

// Utility struct for storage and moving of feature sets for single keypoints (e.g. for projection)
struct FeatureSet {
  std::optional<OrbFeature> orbFeature = std::nullopt;
  std::optional<GorbFeature> gorbFeature = std::nullopt;
  std::optional<LearnedFeature> learnedFeature = std::nullopt;

  FeatureSet() = default;
  FeatureSet(OrbFeature &&orbFeature) : orbFeature(std::move(orbFeature)) {}
  FeatureSet(GorbFeature &&gorbFeature) : gorbFeature(std::move(gorbFeature)) {}
  FeatureSet(LearnedFeature &&learnedFeature) : learnedFeature(std::move(learnedFeature)) {}

  // Move constructors.
  FeatureSet(FeatureSet &&other) = default;
  FeatureSet &operator=(FeatureSet &&other) = default;
  // Disallow copying.
  FeatureSet(const FeatureSet &) = delete;
  FeatureSet &operator=(const FeatureSet &) = delete;

  template <typename Feature>
  constexpr const Feature &get() const {
    static_assert(Feature::type() != DescriptorType::UNSPECIFIED, "Invalid feature type");

    if constexpr (std::is_same_v<Feature, OrbFeature>) {
      return orbFeature.value();
    } else if constexpr (std::is_same_v<Feature, GorbFeature>) {
      return gorbFeature.value();
    } else if constexpr (std::is_same_v<Feature, LearnedFeature>) {
      return learnedFeature.value();
    }
  }

  FeatureSet clone() const {
    FeatureSet set;
    if (orbFeature.has_value()) {
      set.orbFeature = std::make_optional(orbFeature.value().clone());
    }
    if (gorbFeature.has_value()) {
      set.gorbFeature = std::make_optional(gorbFeature.value().clone());
    }
    if (learnedFeature.has_value()) {
      set.learnedFeature = std::make_optional(learnedFeature.value().clone());
    }
    return set;
  }
};

class FeatureStore {
public:
  FeatureStore() = default;

  FeatureStore(FeatureStore &&) = default;
  FeatureStore &operator=(FeatureStore &&) noexcept = default;

  FeatureStore(const FeatureStore &) = delete;
  FeatureStore &operator=(const FeatureStore &) = delete;

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Add and reserve keypoints
  //////////////////////////////////////////////////////////////////////////////////////////////////
  void addKeypoints(size_t count) {
    auto beforeSize = descriptorIndicesByKeypointIndex_.size();
    descriptorIndicesByKeypointIndex_.resize(beforeSize + count);
    for (size_t i = 0; i < count; ++i) {
      descriptorIndicesByKeypointIndex_.at(beforeSize + i).fill(NO_INDEX);
    }
  }
  inline void reserveKeypoints(size_t count) { descriptorIndicesByKeypointIndex_.reserve(count); }
  inline size_t numKeypoints() const { return descriptorIndicesByKeypointIndex_.size(); }

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Add and reserve features
  //////////////////////////////////////////////////////////////////////////////////////////////////
  template <typename Feature>
  inline void add(ImageDescriptor<Feature::size()> &&feature, size_t keypointIndex) {
    Vector<size_t> &indices = keypointIndicesByDescriptorType_[Feature::type()];
    if (descriptorIndexOfKeypoint<Feature>(keypointIndex) != NO_INDEX) {
      // TODO(Riyaan): Should overwriting a feature be allowed?
      C8_THROW("[feature-manager] Feature already exists for keypoint");
    }
    indices.push_back(keypointIndex);

    Vector<Feature> &features = getMutableFeatures<Feature>();
    features.emplace_back(std::move(feature));
    descriptorIndicesByKeypointIndex_[keypointIndex][Feature::type()] = features.size() - 1;
  }

  template <typename Feature>
  inline void append(ImageDescriptor<Feature::size()> &&feature) {
    addKeypoints(1);
    add<Feature>(std::move(feature), numKeypoints() - 1);
  }

  inline void add(FeatureSet &&set, size_t keypointIndex) {
    if (set.orbFeature.has_value()) {
      add<OrbFeature>(std::move(*set.orbFeature), keypointIndex);
      set.orbFeature.reset();
    }
    if (set.gorbFeature.has_value()) {
      add<GorbFeature>(std::move(*set.gorbFeature), keypointIndex);
      set.gorbFeature.reset();
    }
    if (set.learnedFeature.has_value()) {
      add<LearnedFeature>(std::move(*set.learnedFeature), keypointIndex);
      set.learnedFeature.reset();
    }
  }

  inline void append(FeatureSet &&set) {
    addKeypoints(1);
    add(std::move(set), numKeypoints() - 1);
  }

  template <typename Feature>
  inline void reserveFeatures(size_t count) {
    getMutableFeatures<Feature>().reserve(count);
  }

  template <typename Feature>
  inline size_t numDescriptors() const {
    return getFeatures<Feature>().size();
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Index Iterators
  //////////////////////////////////////////////////////////////////////////////////////////////////
  template <typename Feature>
  inline Vector<size_t>::const_iterator begin() const {
    return keypointIndicesByDescriptorType_[Feature::type()].begin();
  }
  template <typename Feature>
  inline Vector<size_t>::const_iterator end() const {
    return keypointIndicesByDescriptorType_[Feature::type()].end();
  }
  template <typename Feature>
  inline const Vector<size_t> &keypointIndices() const {
    return keypointIndicesByDescriptorType_[Feature::type()];
  }
  template <typename Feature>
  inline const size_t descriptorIndexOfKeypoint(size_t keypointIndex) const {
    return descriptorIndicesByKeypointIndex_[keypointIndex][Feature::type()];
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Accessors and check methods
  //////////////////////////////////////////////////////////////////////////////////////////////////
  template <typename Feature>
  inline const Feature &get(size_t keypointIndex) const {
    return getFeatures<Feature>()[descriptorIndexOfKeypoint<Feature>(keypointIndex)];
  }

  template <typename Feature>
  inline Feature &getMutable(size_t keypointIndex) {
    return getMutableFeatures<Feature>()[descriptorIndexOfKeypoint<Feature>(keypointIndex)];
  }

  // Useful for transferring a feature set to a different keypoint index
  FeatureSet getClone(size_t keypointIndex) const {
    FeatureSet set;
    if (has<OrbFeature>(keypointIndex)) {
      set.orbFeature = std::make_optional(get<OrbFeature>(keypointIndex).clone());
    }
    if (has<GorbFeature>(keypointIndex)) {
      set.gorbFeature = std::make_optional(get<GorbFeature>(keypointIndex).clone());
    }
    if (has<LearnedFeature>(keypointIndex)) {
      set.learnedFeature = std::make_optional(get<LearnedFeature>(keypointIndex).clone());
    }
    return set;
  }

  template <typename Feature>
  inline bool has(size_t keypointIndex) const {
    return descriptorIndexOfKeypoint<Feature>(keypointIndex) != NO_INDEX;
  }

  template <typename Feature>
  inline bool has() const {
    return !getFeatures<Feature>().empty();
  }

  template <typename Feature>
  inline bool empty() const {
    return getFeatures<Feature>().empty();
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Clear and clone
  //////////////////////////////////////////////////////////////////////////////////////////////////
  void clear() {
    descriptorIndicesByKeypointIndex_.clear();

    getMutableFeatures<OrbFeature>().clear();
    getMutableFeatures<GorbFeature>().clear();
    getMutableFeatures<LearnedFeature>().clear();

    for (size_t i = 0; i < NUM_DESCRIPTOR_TYPES; ++i) {
      keypointIndicesByDescriptorType_[i].clear();
    }
  }

  FeatureStore clone() const {
    FeatureStore store;
    store.descriptorIndicesByKeypointIndex_ = descriptorIndicesByKeypointIndex_;
    store.keypointIndicesByDescriptorType_ = keypointIndicesByDescriptorType_;

    auto &storeOrbFeatures = store.getMutableFeatures<OrbFeature>();
    auto &storeGorbFeatures = store.getMutableFeatures<GorbFeature>();
    auto &storeLearnedFeatures = store.getMutableFeatures<LearnedFeature>();

    storeOrbFeatures.reserve(numDescriptors<OrbFeature>());
    for (const auto &feature : getFeatures<OrbFeature>()) {
      storeOrbFeatures.emplace_back(feature.clone());
    }
    storeGorbFeatures.reserve(numDescriptors<GorbFeature>());
    for (const auto &feature : getFeatures<GorbFeature>()) {
      storeGorbFeatures.emplace_back(feature.clone());
    }
    storeLearnedFeatures.reserve(numDescriptors<LearnedFeature>());
    for (const auto &feature : getFeatures<LearnedFeature>()) {
      storeLearnedFeatures.emplace_back(feature.clone());
    }
    return store;
  }

  template <typename Feature>
  constexpr const Vector<Feature> &getFeatures() const {
    static_assert(Feature::type() != DescriptorType::UNSPECIFIED, "Invalid feature type");
    return std::get<Vector<Feature>>(features_);
  }

private:
  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Private helper methods
  //////////////////////////////////////////////////////////////////////////////////////////////////
  template <typename Feature>
  constexpr Vector<Feature> &getMutableFeatures() {
    static_assert(Feature::type() != DescriptorType::UNSPECIFIED, "Invalid feature type");
    return std::get<Vector<Feature>>(features_);
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////
  // Private member variables
  //////////////////////////////////////////////////////////////////////////////////////////////////

  // Features:
  std::tuple<Vector<OrbFeature>, Vector<GorbFeature>, Vector<LearnedFeature>> features_;

  // Indexing:
  // descriptorIndicesByKeypointIndex_[i][j] = k --> the jth desc-type of keypoint i is at index k
  // descriptorIndicesByKeypointIndex_[i][j] = NO_INDEX --> the jth desc-type of keypoint i does not
  // exist
  Vector<std::array<size_t, NUM_DESCRIPTOR_TYPES>> descriptorIndicesByKeypointIndex_;
  // keypointIndicesByDescriptorType_[j][k] = i --> for the jth desc-type, index k holds descriptor
  // for keypoint i
  std::array<Vector<size_t>, NUM_DESCRIPTOR_TYPES> keypointIndicesByDescriptorType_;
};

}  // namespace c8
