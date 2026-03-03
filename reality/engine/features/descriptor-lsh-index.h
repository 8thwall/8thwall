// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Efficient LSH Index and nearest neighbor search for ImageDescriptors in
// smallish corpora (10s-1000s).

#pragma once

#include "c8/vector.h"
#include "reality/engine/features/image-descriptor.h"

namespace c8 {

template <typename LocalDescriptor>
struct LshQueryResult {
  int index;
  int distance;
};

// Instantiate a DescriptorLshIndex with a maxDescriptorCount that should be a multiple of 64.
template <int maxDescriptorCount, typename LocalDescriptor>
class DescriptorLshIndex {
public:
  using Descriptor = LocalDescriptor;
  static constexpr int maxDescriptors = maxDescriptorCount;

  static_assert(maxDescriptors % 64 == 0, "maxDescriptorCount must be divisible by 64");

  // Constructor. Class will maintain pointers to the descriptors supplied,
  // which must remain allocated and in memory throughout the lifetime of the
  // DescriptorLshIndex.
  //
  // Parameters:
  //   descriptorArray - pointer to array of ImageDescriptors, not owned by class
  //   descriptorArraySize - number of ImageDescriptors in array.
  //   bitsPerHash - number of bit-hashes in each LSH hash family.
  //   hashesPer64BitBlock - number of unique hashes to use per 64-bit block in the ImageDescriptor
  //   numTriesPerHash - try to maximize bucket entropy by retrying each new hash this many times.
  //                     A large value will slow down the constructor.
  //   hashBuckets - number of buckets in the hashtable, will be rounded up to power of two.
  DescriptorLshIndex(
    const Descriptor *descriptorArray,
    size_t descriptorArraySize,
    int bitsPerHash,
    int hashesPer64BitBlock,
    int numTriesPerHash,
    int hashBuckets);

  // Default move constructors.
  DescriptorLshIndex(DescriptorLshIndex &&) = default;
  DescriptorLshIndex &operator=(DescriptorLshIndex &&) = default;

  // Disallow copying.
  DescriptorLshIndex(const DescriptorLshIndex &) = delete;
  DescriptorLshIndex &operator=(const DescriptorLshIndex &) = delete;

  // LSH-index search for the nearest neighbor. Returns the nearest descriptor,
  // its index, and its hammingDistance to the query, using the LSH index. May
  // return a near-match and not an exact match. Returns index = -1 if all hash
  // collisions fail.
  LshQueryResult<Descriptor> findNearest(const Descriptor &query) const;

  // Brute force search for the nearest neigbor.Returns the nearest descriptor,
  // its index, and its hammingDistance to the query using a brute force search.
  LshQueryResult<Descriptor> findNearestBruteForce(const Descriptor &query) const;

  // Update location of descriptorArray. If the underlying location of the
  // descriptor array moves, you can update the pointer with this method. The
  // requirement is that the descriptor contents and size are unchanged.
  // The 'optnone' annotation prevents a clang optimziation that empirically
  // hurts overall performance.
  [[clang::optnone]] void updateDescriptorArray(const Descriptor *descriptorArray) {
    descriptors_ = descriptorArray;
  }

private:
  static constexpr int blocksPerEntry = maxDescriptorCount >> 6;
  static constexpr int numBlocks = Descriptor::size() >> 3;
  static constexpr int blockShift = __builtin_ctz(numBlocks);  // log2(numBlocks)

  struct HashEntry {
    uint64_t index[blocksPerEntry] = {};
  };

  // Compute the entropy of the hash buckets when hashing all of the descriptors
  // for a given set of hash functions.
  float computeEntropy(const Vector<uint64_t> &hashes);

  const Descriptor *descriptors_;
  int descriptorsSize_;

  int hashesPer64BitBlock_;
  Vector<HashEntry> index_;
  uint64_t bucketMask_;
  Vector<uint64_t> hashes_;
};

extern template class DescriptorLshIndex<64, ImageDescriptor32>;
extern template class DescriptorLshIndex<512, ImageDescriptor32>;
extern template class DescriptorLshIndex<4096, ImageDescriptor32>;

}  // namespace c8
