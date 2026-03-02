// Copyright (c) 2024 Niantic, Inc.
// Original Author: Riyaan Bakhda (riyaanbakhda@nianticlabs.com)

#pragma once

#include <array>

#include "c8/exceptions.h"

namespace c8 {

// Increments every oneCounts entry by 1 for every 1 seen at the corresponding bit of data.
// NOTE: The length of data must be N, i.e., total of N * 8 bits.
template <size_t N>
void addOneCounts(const uint8_t *data, std::array<size_t, N * 8> *oneCounts) {
  for (size_t i = 0; i < N; ++i) {
    (*oneCounts)[i * 8 + 0] += ((data[i] & (1 << 7)) ? 1 : 0);
    (*oneCounts)[i * 8 + 1] += ((data[i] & (1 << 6)) ? 1 : 0);
    (*oneCounts)[i * 8 + 2] += ((data[i] & (1 << 5)) ? 1 : 0);
    (*oneCounts)[i * 8 + 3] += ((data[i] & (1 << 4)) ? 1 : 0);
    (*oneCounts)[i * 8 + 4] += ((data[i] & (1 << 3)) ? 1 : 0);
    (*oneCounts)[i * 8 + 5] += ((data[i] & (1 << 2)) ? 1 : 0);
    (*oneCounts)[i * 8 + 6] += ((data[i] & (1 << 1)) ? 1 : 0);
    (*oneCounts)[i * 8 + 7] += ((data[i] & (1 << 0)) ? 1 : 0);
  }
}

// Decrements every oneCounts entry by 1 for every 1 seen at the corresponding bit of data.
// NOTE: The length of data must be N, i.e., total of N * 8 bits.
template <size_t N>
void subtractOneCounts(const uint8_t *data, std::array<size_t, N * 8> *oneCounts) {
  const auto subtractOneCountForBit = [](size_t &oneCount, bool bitIsOne) {
    if (!bitIsOne) {
      return;
    }
    if (oneCount == 0) {
      C8_THROW("Subtracting from a zeroed unsigned counter.");
    }
    --oneCount;
  };
  for (size_t i = 0; i < N; ++i) {
    subtractOneCountForBit((*oneCounts)[i * 8 + 0], data[i] & (1 << 7));
    subtractOneCountForBit((*oneCounts)[i * 8 + 1], data[i] & (1 << 6));
    subtractOneCountForBit((*oneCounts)[i * 8 + 2], data[i] & (1 << 5));
    subtractOneCountForBit((*oneCounts)[i * 8 + 3], data[i] & (1 << 4));
    subtractOneCountForBit((*oneCounts)[i * 8 + 4], data[i] & (1 << 3));
    subtractOneCountForBit((*oneCounts)[i * 8 + 5], data[i] & (1 << 2));
    subtractOneCountForBit((*oneCounts)[i * 8 + 6], data[i] & (1 << 1));
    subtractOneCountForBit((*oneCounts)[i * 8 + 7], data[i] & (1 << 0));
  }
}

// Returns a majority voting result of oneCounts. For each entry of oneCounts, set the corresponding
// bit of majorityData to 1 if that entry has a value greater than majorityThreshold; otherwise, set
// to 0.
// NOTE: The length of majorityData must be N, i.e., total of N * 8 bits.
template <size_t N>
void getMajorityFromOneCounts(
  const std::array<size_t, N * 8> &oneCounts,
  const size_t majorityThreshold,
  uint8_t *majorityData) {
  for (size_t i = 0; i < N; ++i) {
    // clang-format off
    majorityData[i] = ((oneCounts[i * 8 + 0] > majorityThreshold) ? (1 << 7) : 0)
                    + ((oneCounts[i * 8 + 1] > majorityThreshold) ? (1 << 6) : 0)
                    + ((oneCounts[i * 8 + 2] > majorityThreshold) ? (1 << 5) : 0)
                    + ((oneCounts[i * 8 + 3] > majorityThreshold) ? (1 << 4) : 0)
                    + ((oneCounts[i * 8 + 4] > majorityThreshold) ? (1 << 3) : 0)
                    + ((oneCounts[i * 8 + 5] > majorityThreshold) ? (1 << 2) : 0)
                    + ((oneCounts[i * 8 + 6] > majorityThreshold) ? (1 << 1) : 0)
                    + ((oneCounts[i * 8 + 7] > majorityThreshold) ? (1 << 0) : 0);
    // clang-format on
  }
}

}  // namespace c8
