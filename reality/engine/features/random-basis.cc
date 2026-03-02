// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "random-basis.h",
  };
  deps = {
    "//c8:exceptions",
    "//c8:float-vector",
    "//c8:c8-log",
    "//reality/engine/features:image-descriptor",
    "//reality/engine/features/api:descriptors.capnp-cc",
    "@eigen3",
  };
}
cc_end(0xf97d99a3);

#include "reality/engine/features/random-basis.h"

#include "c8/c8-log.h"
#include "c8/exceptions.h"
#include "reality/engine/features/image-descriptor.h"

#include <Eigen/Dense>
#include <numeric>

namespace c8 {

template <typename Descriptor>
RandomBasis<Descriptor>::RandomBasis(int basisVectorSize) : basisVectorSize_(basisVectorSize) {
  // Seed with 0 to ensure basis is deterministic.
  std::mt19937 rnd(0);
  std::normal_distribution<float> distribution(0.0f, 1.0f);

  basis_.resize(basisVectorSize * projectionSize);
  for (float &item : basis_) {
    item = distribution(rnd);
  }
}

template <typename Descriptor>
Descriptor RandomBasis<Descriptor>::projectAndBinarize(const FloatVector &input) const {
  FloatVector projection(projectionSize);

  // Compute the matrix multiplication Y = AX. This could be done with BLAS
  // instead of Eigen, and a future benchmark could focus on improving the speed
  // of this.
  Eigen::Map<Eigen::MatrixXf>(projection.data(), projectionSize, 1) =
    Eigen::Map<const Eigen::MatrixXf>(basis_.data(), projectionSize, input.size())
    * Eigen::Map<const Eigen::MatrixXf>(input.data(), input.size(), 1);

  Descriptor result;

  for (int p = 0; p < Descriptor::size(); ++p) {
    const int bitStart = p << 3;
    uint8_t &byte = result.mutableData()[p];
    byte = static_cast<uint8_t>(0);

    for (int b = 7; b >= 0; --b) {
      byte = (byte << 1) | (projection[bitStart + b] > 0.0f);
    }
  }

  return result;
}

// Instantiate the following template classes.
template class RandomBasis<OrbFeature>;
template class RandomBasis<ImageDescriptor32>;
template class RandomBasis<ImageDescriptor<64>>;
template class RandomBasis<ImageDescriptor<128>>;
template class RandomBasis<ImageDescriptor<256>>;
template class RandomBasis<ImageDescriptor<512>>;

}  // namespace c8
