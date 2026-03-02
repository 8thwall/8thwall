// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "descriptor-lsh-index.h",
  };
  deps = {
    "//c8:set",
    "//c8:map",
    "//c8:vector",
    "//reality/engine/features:image-descriptor",
  };
}
cc_end(0x8190e604);

#include "reality/engine/features/descriptor-lsh-index.h"

#include "c8/map.h"
#include "c8/set.h"

#include <bitset>
#include <random>

namespace c8 {

namespace {

// Round up to the next power of two.
constexpr int roundToPowerOfTwo(int v) {
  v--;
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  v++;
  return v;
}

// Quickly hash a uint64_t into a uint64_t that appears randomly distributed.
inline uint64_t genHash(uint64_t h) {
  // Smart choice for a xmx finalizer with only one multiplication for speed.
  h ^= h >> 30;
  h *= 0xc4ceb9fe1a85ec53ull;
  h ^= h >> 27;
  return h;
};

}  // namespace

template <int maxDescriptorCount, typename LocalDescriptor>
DescriptorLshIndex<maxDescriptorCount, LocalDescriptor>::DescriptorLshIndex(
  const Descriptor *descriptorArray,
  size_t descriptorArraySize,
  int bitsPerHash,
  int hashesPer64BitBlock,
  int numTriesPerHash,
  int hashBuckets)
    : hashesPer64BitBlock_(hashesPer64BitBlock) {
  descriptors_ = descriptorArray;
  descriptorsSize_ = descriptorArraySize;

  const int numHashes = hashesPer64BitBlock * numBlocks;

  Vector<uint64_t> potentialHashes;

  // Allocate the index at a power-of-two size.
  uint32_t numBuckets = roundToPowerOfTwo(hashBuckets);

  index_.resize(numBuckets);
  bucketMask_ = numBuckets - 1;

  // Create a random number generator with a fixed seed.
  std::mt19937 rnd(0);
  Vector<int> bitIndex;

  // Bit choice.
  for (int i = 0; i < 64; ++i) {
    bitIndex.push_back(i);
  }

  uint64_t bestHash;
  float maxEntropy = std::numeric_limits<float>::min();

  // Pick hash function candidates and compute the change to the bucket entropy
  // as a result. Keep the best hash after n tries.
  for (int t = 0; t < numHashes; ++t) {
    for (int i = 0; i < numTriesPerHash; ++i) {
      // Randomly choose a hash with n=bitsPerHash bits set to 1.
      std::shuffle(bitIndex.begin(), bitIndex.end(), rnd);
      std::bitset<64> bitset;
      for (int h = 0; h < bitsPerHash; ++h) {
        bitset[bitIndex[h]] = true;
      }
      uint64_t hf = bitset.to_ullong();

      // Add the hash function.
      potentialHashes.push_back(hf);

      // Compute the entropy.
      float e = computeEntropy(potentialHashes);

      if (maxEntropy < e) {
        maxEntropy = e;
        bestHash = hf;
      }
      // Pop off the candidate hash.
      potentialHashes.pop_back();
    }
    // Keep the hash that maximized the entropy.
    potentialHashes.push_back(bestHash);
  }

  // Sort the final hashes in block order, as we'll rely on this during search.
  for (int i = 0; i < potentialHashes.size(); ++i) {
    int iShift = i << blockShift;
    int idx = iShift % (numHashes) + iShift / numHashes;
    hashes_.push_back(potentialHashes[idx]);
  }

  // Hash all of the input data.
  for (int d = 0; d < descriptorsSize_; ++d) {
    const Descriptor &hd = descriptors_[d];
    for (int b = 0; b < numBlocks; ++b) {
      uint64_t block = reinterpret_cast<const uint64_t *>(hd.data())[b];
      const int shift = b * hashesPer64BitBlock_;
      for (int h = 0; h < hashesPer64BitBlock_; ++h) {
        // auto hsh = genHash(hf, hd);
        uint64_t hashFn = hashes_[shift + h];
        auto hsh = hashFn ^ genHash(block & hashFn);

        auto bucket = hsh & bucketMask_;

        // Mark that the bucket is filled with at least 1 item.
        index_[bucket].index[0] |= 0b1llu;

        if (d == 0) {
          // Encode index 0 along with index 1 in 0b10, since index 0 means no items in bucket.
          index_[bucket].index[0] |= 0b10llu;
        } else {
          // Set the d-th bit in the uint64_t array.
          index_[bucket].index[d >> 6] |= 0b1llu << (d & 63llu);
        }
      }
    }
  }
}

template <int maxDescriptorCount, typename LocalDescriptor>
float DescriptorLshIndex<maxDescriptorCount, LocalDescriptor>::computeEntropy(
  const Vector<uint64_t> &hashes) {
  constexpr int numBlocks = Descriptor::size() >> 3;
  TreeMap<uint64_t, int> counts;

  for (int d = 0; d < descriptorsSize_; ++d) {
    const Descriptor &hd = descriptors_[d];
    for (int i = 0; i < hashes.size(); ++i) {
      int blockIdx = i & (numBlocks - 1);  // i % numBlocks
      uint64_t block = reinterpret_cast<const uint64_t *>(hd.data())[blockIdx];
      uint64_t hashFn = hashes[i];
      auto hsh = hashFn ^ genHash(block & hashFn);
      auto bucket = hsh & bucketMask_;
      counts[bucket]++;
    }
  }
  float entropy = 0.0f;
  auto totalHashes = descriptorsSize_ * hashes.size();
  float total = 0.0;
  for (auto [hash, count] : counts) {
    float p = static_cast<float>(count) / totalHashes;
    total += p;
    entropy -= p * std::log(p);
  }
  return entropy;
}

template <int maxDescriptorCount, typename LocalDescriptor>
LshQueryResult<LocalDescriptor>
DescriptorLshIndex<maxDescriptorCount, LocalDescriptor>::findNearest(
  const Descriptor &query) const {

  uint64_t toQuery[blocksPerEntry] = {};
  const uint64_t *hashPtr = hashes_.data();
  const uint64_t *queryData = reinterpret_cast<const uint64_t *>(query.data());
  for (int b = 0; b < numBlocks; ++b) {
    uint64_t block = queryData[b];
    const uint64_t *hashBlockEnd = hashPtr + hashesPer64BitBlock_;

    for (; hashPtr != hashBlockEnd; ++hashPtr) {
      uint64_t hashFn = *hashPtr;
      auto hsh = hashFn ^ genHash(block & hashFn);

      const HashEntry &entry = index_[hsh & bucketMask_];
      for (int i = 0; i < blocksPerEntry; ++i) {
        toQuery[i] |= entry.index[i];
      }
    }
  }

  int closestDistance = std::numeric_limits<int>::max();
  int index = -1;

  // Bit 0b10 is a special case, since it encodes descriptor 0 and descriptor 1.
  if (toQuery[0] & static_cast<uint64_t>(0b10)) {
    closestDistance = query.hammingDistance(descriptors_[0]);
    index = 0;

    int d = query.hammingDistance(descriptors_[1]);
    if (d < closestDistance) {
      closestDistance = d;
      index = 1;
    }
  }

  // Work on the remaining bits in the first block.
  for (int i = 2; i < 64; ++i) {
    if (toQuery[0] & (static_cast<uint64_t>(0b1) << i)) {
      int d = query.hammingDistance(descriptors_[i]);
      if (d < closestDistance) {
        closestDistance = d;
        index = i;
      }
    }
  }

  // Work on the remaining bits in subsequent blocks.
  for (int b = 1; b < blocksPerEntry; ++b) {
    for (int i = 0; i < 64; ++i) {
      if (toQuery[b] & (static_cast<uint64_t>(0b1) << i)) {
        int d = query.hammingDistance(descriptors_[i + (b << 6)]);
        if (d < closestDistance) {
          closestDistance = d;
          index = i;
        }
      }
    }
  }

  return {index, closestDistance};
}

template <int maxDescriptorCount, typename LocalDescriptor>
LshQueryResult<LocalDescriptor>
DescriptorLshIndex<maxDescriptorCount, LocalDescriptor>::findNearestBruteForce(
  const Descriptor &query) const {
  LshQueryResult<Descriptor> result;
  result.distance = std::numeric_limits<int>::max();
  result.index = 0;
  for (int j = 0; j < descriptorsSize_; ++j) {
    int d = query.hammingDistance(descriptors_[j]);
    if (d < result.distance) {
      result.distance = d;
      result.index = j;
    }
  }
  return result;
}

// Instantiate these templates.
template class DescriptorLshIndex<64, ImageDescriptor32>;
template class DescriptorLshIndex<512, ImageDescriptor32>;
template class DescriptorLshIndex<4096, ImageDescriptor32>;

}  // namespace c8
