// Copyright (c) 2024 Niantic, Inc.
// Original Author: Riyaan Bakhda (riyaanbakhda@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"tree-matcher.h"};
  deps = {
    ":frame-point",
    ":image-descriptor",
    "//c8:parameter-data",
    "//c8:vector",
    "//c8/stats:scope-timer",
  };
}
cc_end(0xf43293ac);

#include "reality/engine/features/tree-matcher.h"

namespace c8 {

template <typename Descriptor>
TreeMatcher<Descriptor>::TreeMatcher(
  size_t nestingLevel, size_t splitThreshold, int distanceThreshold) {
  // For example, if Descriptor::size() is 32 bytes and nestingLevel is 3, then:
  //  - numChunks     = 2^nestingLevel                 = 8 chunks
  //  - bytesPerChunk = Descriptor::size() / numChunks = 4 bytes per chunk
  numChunks_ = 1 << nestingLevel;
  bytesPerChunk_ = Descriptor::size() / numChunks_;

  // Note that we want bytesPerChunk >= 1, so we will set it to 1 if it is less than 1.
  if (bytesPerChunk_ < 1) {
    numChunks_ = Descriptor::size();
    bytesPerChunk_ = 1;
  }

  // The number of descriptors a node should have before splitting into children, minimum 1.
  splitThreshold_ = splitThreshold;
  if (splitThreshold_ < 1) {
    splitThreshold_ = 1;
  }

  // The distance threshold for matching.
  distanceThreshold_ = distanceThreshold;
  if (distanceThreshold_ <= 0) {
    distanceThreshold_ = Descriptor::size() * 8;
  }
}

template <typename Descriptor>
void TreeMatcher<Descriptor>::getPopCountPattern(
  const Descriptor &descriptor, Vector<int> *popCountPattern) const {
  popCountPattern->clear();
  popCountPattern->resize(numChunks_, 0);
  // Note: Endianness doesn't matter if we only use this function to get the pop count pattern.
  const uint8_t *chunkStartPtr = descriptor.data();
  for (int i = 0; i < numChunks_; ++i) {
    const uint8_t *chunkEndPtr = chunkStartPtr + bytesPerChunk_;
    while (chunkStartPtr < chunkEndPtr) {
      (*popCountPattern)[i] += __builtin_popcount(*(chunkStartPtr++));
    }
  }
}

template class TreeMatcher<ImageDescriptor<8>>;

}  // namespace c8