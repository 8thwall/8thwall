// Copyright (c) 2024 Niantic, Inc.
// Original Author: Riyaan Bakhda (riyaanbakhda@nianticlabs.com)
//
// The goal of TreeMatcher is to create a lookup tree for the dictionary descriptors
// using the "pop count patterns" of the descriptors. The pop count pattern is the
// pop count of substrings ("chunks") of the descriptor. At query time, by looking up
// the pop count pattern of a query descriptor, we can find its matches in the tree.
// In practice, while building the tree is a bit expensive, matching is extremely fast.

#pragma once

#include "c8/stats/scope-timer.h"
#include "c8/vector.h"
#include "reality/engine/features/feature-manager.h"
#include "reality/engine/features/frame-point.h"

namespace c8 {

// The lookup tree is an n-ary tree where each node has (8*bytesPerChunk_)+1 children.
// Eg. If we create a tree for 8-byte descriptors with 4 chunks, we would
// 1) Split each dictionary descriptor into 4 chunks of 2 bytes each.
// 2) Create a tree of height 4, where each node has 17 children
//      Note: 17 because that is the number of distinct pop counts that 2 bytes can have
// 3) Each leaf node will hold a list of dictionary indices.
// While searching, we will take advantage of the "pop count pattern" of the query descriptor
// to traverse the tree efficiently and find the best match.

// TODO(Riyaan): Since such a tree has a high branching factor, we will later integrate the
// following strategies:
// - Don't give a node children if it has fewer than splitThreshold_ descriptors under it.
// - "Sparsify" the tree by skipping over empty nodes during search
// - Add quantization to the descriptor chunks to reduce the branching factor.

template <typename Descriptor>
class TreeMatcher {
public:
  // Consider the below 8-byte descriptor A, which is used in a unit test,
  // clang-format off
  // With numChunks = 4, bytesPerChunk = 2, the descriptor A has the following pop count pattern:
  // A = {0b00001000, 0b011000000, 0b10101010, 0b11111111, 0b00001100, 0b011001000, 0b11101010, 0b00000100}
  //      \_________3___________/  \_________12_________/  \__________5__________/  \__________6_________/
  // i.e. getPopCountPattern(A) = {3, 12, 5, 6}
  // clang-format on
  // So, the descriptor goes in the tree as the 4th child at level 0, 12th child at level 1, 5th
  // child at level 2, and 6th child at level 3.
  // @param descriptor The descriptor to get the pop count pattern for.
  // @param popCountPattern The pop count pattern of the descriptor.
  void getPopCountPattern(const Descriptor &descriptor, Vector<int> *popCountPattern) const;

  // @param nestingLevel Nesting Level controls the height of the lookup tree.
  // @param splitThreshold The number of descriptors a node must have before giving it children.
  // @param distanceThreshold The distance threshold for matching.
  TreeMatcher(size_t nestingLevel = 0, size_t splitThreshold = 1, int distanceThreshold = 0);

  TreeMatcher(TreeMatcher &&) = default;
  TreeMatcher &operator=(TreeMatcher &&) = default;

  TreeMatcher(const TreeMatcher &) = delete;
  TreeMatcher &operator=(const TreeMatcher &) = delete;

  void prepare(const FeatureStore &dictionary);

  void match(
    const FrameWithPoints &words, Vector<PointMatch> *matches, int distanceThreshold) const;

private:
  // Node in the lookup tree.
  struct Node {
    // The dictIndices vector is only populated for leaf nodes.
    Vector<size_t> dictIndices;
    // The children vector is only populated for non-leaf nodes.
    Vector<Node> children;

    // Total number of indices in this node and its children.
    size_t size = 0;

    // For a node at level i
    // - minPopCount: over the patterns of descriptors under this node, the min pop count at level i
    // - maxPopCount: over the patterns of descriptors under this node, the max pop count at level i
    int16_t minPopCount = std::numeric_limits<int16_t>::max();
    int16_t maxPopCount = std::numeric_limits<int16_t>::min();
  };

  // Insert a dictionary descriptor into the lookup tree.
  // @param currentChunk The current chunk of the descriptor's pop count pattern.
  // @param dictIdx The index of the descriptor in the dictionary.
  // @param node The current node in the lookup tree.
  void prepareRecursive(int currentChunk, size_t dictIdx, Node *node);

  // Match a query descriptor against the lookup tree.
  // @param queryPattern The pop count pattern of the query descriptor.
  // @param currentChunk The current chunk of the query pop count pattern.
  // @param node The current node in the lookup tree.
  // @param accumulatedPopCountDiff The accumulated pop count difference so far.
  // @param minD The hamming distance for the best match so far
  // @param idx The index of the best match so far
  void matchRecursive(
    const Descriptor &d1,
    const Vector<uint16_t> &queryPattern,
    const int16_t currentChunk,
    const Node &node,
    int accumulatedPopCountDiff,
    int *minD,
    int *idx) const;

  // bytesPerChunk_ * numChunks_ must equal Descriptor::size()
  size_t bytesPerChunk_;
  size_t numChunks_;
  // The number of descriptors a node should have before splitting into children
  size_t splitThreshold_;

  // The root of the lookup tree
  Node root_;

  // The dictionary to match against
  const FeatureStore *trainPtsArray_ = nullptr;
  int distanceThreshold_;
};

extern template class TreeMatcher<ImageDescriptor<8>>;

}  // namespace c8
