// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Simple implementation for image descriptors.

#pragma once

#include <algorithm>
#include <array>
#include <cstdint>

namespace c8 {

enum DescriptorType : size_t {
  UNSPECIFIED = 0,
  ORB = 1,
  GORB = 2,
  LEARNED = 3,
};
constexpr const size_t NUM_DESCRIPTOR_TYPES = 4;

template <size_t N>
class ImageDescriptor {
public:
  static_assert(N % sizeof(8) == 0, "ImageDescriptor N must be divisible by 8");

  // Default constructor.
  ImageDescriptor() = default;
  ImageDescriptor(const std::array<uint8_t, N> &d) { memcpy(data_, d.data(), N); };

  ImageDescriptor(ImageDescriptor &&) = default;
  ImageDescriptor &operator=(ImageDescriptor &&) noexcept = default;

  ImageDescriptor(const ImageDescriptor &) = delete;
  ImageDescriptor &operator=(const ImageDescriptor &) = delete;

  // Destructor.
  ~ImageDescriptor() {}

  static constexpr DescriptorType type() { return DescriptorType::UNSPECIFIED; }

  // Access the underlying data as an array of bytes.
  const uint8_t *data() const { return reinterpret_cast<const uint8_t *>(data_); }
  uint8_t *mutableData() { return reinterpret_cast<uint8_t *>(data_); }

  // Access the underlying data blocks.
  const uint64_t *blockData() const { return data_; };

  // Number of uint64_t blocks in the descriptor
  static constexpr int blockCount() { return N >> 3; }

  std::array<uint8_t, N> copy() const {
    std::array<uint8_t, N> a;
    memcpy(a.data(), data_, N);
    return a;
  }

  ImageDescriptor<N> clone() const {
    ImageDescriptor<N> b;
    std::copy(std::begin(data_), std::end(data_), std::begin(b.data_));
    return b;
  }

  // Size of the descriptor in bytes.
  static constexpr size_t size() { return N; }

  int hammingDistance(const ImageDescriptor<N> &other) const {
    int dist = 0;
    const UintT *dptr = data_;
    const UintT *dend = data_ + NN;
    const UintT *optr = other.data_;
    while (dptr != dend) {
      dist += __builtin_popcountll(*dptr ^ *optr);
      ++dptr;
      ++optr;
    }
    return dist;
  }

  int totalPopCount() const {
    int pop = 0;
    const UintT *dptr = data_;
    const UintT *dend = data_ + NN;
    while (dptr != dend) {
      pop += __builtin_popcountll(*dptr);
      ++dptr;
    }
    return pop;
  }

private:
  using UintT = unsigned long long;
  static constexpr size_t NN = N / sizeof(UintT);

  UintT data_[NN];
};

using ImageDescriptor16 = ImageDescriptor<16>;
using ImageDescriptor32 = ImageDescriptor<32>;
using ImageDescriptor64 = ImageDescriptor<64>;

template <size_t N, DescriptorType T>
struct Feature : public ImageDescriptor<N> {
  static constexpr DescriptorType type() { return T; }

  Feature() = default;
  Feature(ImageDescriptor<N> &&descriptor) : ImageDescriptor<N>(std::move(descriptor)) {}
  Feature(const std::array<uint8_t, N> &d) : ImageDescriptor<N>(d) {}

  Feature clone() const { return Feature{ImageDescriptor<N>::clone()}; }
};

using OrbFeature = Feature<32, DescriptorType::ORB>;
using GorbFeature = Feature<32, DescriptorType::GORB>;
using LearnedFeature = Feature<16, DescriptorType::LEARNED>;

}  // namespace c8
