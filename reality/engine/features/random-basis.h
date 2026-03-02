// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Class generate a random projection basis and project FloatVectors into this
// space followed by sign-based binarization into an ImageDescriptor.

#pragma once

#include <random>

#include "c8/float-vector.h"
#include "reality/engine/features/image-descriptor.h"

namespace c8 {

template <typename Descriptor>
class RandomBasis {
public:
  static constexpr int projectionSize = 8 * Descriptor::size();

  // Construct a RandomBasis with a C++11 random distribution class, such as
  // std::normal_distribution<float> or std::cauchy_distribution<float>.
  RandomBasis(int basisVectorSize);

  // Default move constructors.
  RandomBasis(RandomBasis &&) = default;
  RandomBasis &operator=(RandomBasis &&) = default;

  // Disallow copying.
  RandomBasis(const RandomBasis &) = delete;
  RandomBasis &operator=(const RandomBasis &) = delete;

  // Project a data sample into the random projection space then binarize it
  // by returning one bit based on the sign of the projection vector component.
  // Hamming distance on the result will estimate the cosine similarity of the
  // original vector, or the L2 distance between L2 normalized input.
  Descriptor projectAndBinarize(const FloatVector &input) const;

private:
  // Size of each basis vector.
  int basisVectorSize_;

  // Projection basis.
  FloatVector basis_;
};

extern template class RandomBasis<OrbFeature>;
extern template class RandomBasis<ImageDescriptor32>;
extern template class RandomBasis<ImageDescriptor<64>>;
extern template class RandomBasis<ImageDescriptor<128>>;
extern template class RandomBasis<ImageDescriptor<256>>;
extern template class RandomBasis<ImageDescriptor<512>>;

// Aliases for the specific Agate Extractor(s) we use.
using RandomBasisOrb = RandomBasis<OrbFeature>;
using RandomBasis32 = RandomBasis<ImageDescriptor32>;
using RandomBasis64 = RandomBasis<ImageDescriptor<64>>;
using RandomBasis128 = RandomBasis<ImageDescriptor<128>>;
using RandomBasis256 = RandomBasis<ImageDescriptor<256>>;
using RandomBasis512 = RandomBasis<ImageDescriptor<512>>;

}  // namespace c8
