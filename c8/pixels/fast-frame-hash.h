// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include <array>

#include "c8/pixels/pixels.h"

namespace c8 {

// Quickly computes a hash value from an image that can be used for statistically matching exact
// duplicates. This hash trades speed for reliability, e.g. very small changes in images may not be
// detected.
class FastFrameHash {
public:
  void fill(ConstRGBA8888PlanePixels p);

  bool operator==(const FastFrameHash &b) const { return pixelValues_ == b.pixelValues_; }

private:
  std::array<uint8_t, 100> pixelValues_{};
};

// A set of the last N unique frame hashes. Before hashes are added to the set, we check for
// collisions. On collision, the hash is not added, and the number of frames since the image was
// seen is returned.
template <size_t N>
class FastFrameHashSet {
public:
  // Computes the frame hash, adds it to the set, and returns 0 if it's a unique hash. If it's not
  // unique, return the least amount of frames since this frame was seen.
  int checkAndAdd(ConstRGBA8888PlanePixels p) {
    FastFrameHash h;
    h.fill(p);
    int startCheck = nextAddIdx_ + hashes_.size() - 1;
    int endCheck = startCheck - numInSet_;
    for (int i = startCheck; i > endCheck; --i) {
      if (h == hashes_[i % hashes_.size()]) {
        return startCheck - i + 1;
      }
    }

    if (numInSet_ < hashes_.size()) {
      hashes_[numInSet_++] = h;
    } else {
      hashes_[nextAddIdx_] = h;
    }
    nextAddIdx_ = (nextAddIdx_ + 1) % hashes_.size();
    return 0;
  }

private:
  std::array<FastFrameHash, N> hashes_{};
  int numInSet_ = 0;
  int nextAddIdx_ = 0;
};

}  // namespace c8
