// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Dat Chu (dat@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"histogram.h"};
  deps = {
    "//c8:vector",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x18093e28);

#include <cfloat>
#include <cmath>

#include "c8/stats/histogram.h"

namespace c8 {

// Find locations in ys where its value is larger than its neighbor within a radius
// @param radius 1 => immediate neighbors
Vector<int> findPeaks(Vector<float> ys, int radius, float minVal) {
  Vector<Vector<float>> leftDiff;
  leftDiff.reserve(radius);
  for (int r = 1; r <= radius; r++) {
    leftDiff.emplace_back(ys.size(), 0);
    for (size_t i = r; i < ys.size(); i++) {
      leftDiff.back()[i] = ys[i] - ys[i - r];
    }
  }

  Vector<int> out;
  for (size_t i = radius; i < ys.size() - radius; i++) {
    bool isPeak = true;
    // I have to be larger than every element to my left
    for (int j = 1; j <= radius; j++) {
      if (leftDiff[j - 1][i] < 0) {
        isPeak = false;
        break;
      }
    }

    if (!isPeak) {
      continue;
    }

    // I have to be larger than every element to my left
    for (int j = 1; j <= radius; j++) {
      if (leftDiff[j - 1][i + j] > 0) {
        isPeak = false;
        break;
      }
    }

    if (isPeak && ys[i] >= minVal) {
      out.push_back(i);
    }
  }
  return out;
}

// peaks within radius index from each other are combined
Vector<int> mergePeaksInRadius(const Vector<int> &peaks, int radius) {
  if (peaks.size() < 2) {
    return peaks;
  }
  Vector<int> supressedPeaks;
  supressedPeaks.reserve(peaks.size());
  int peakCombinedTotal = 0;
  int peakCombinedCount = 0;
  for (int i = 0; i < (peaks.size() - 1); i++) {
    if ((peaks[i + 1] - peaks[i]) <= radius) {
      // add this peak to be combined
      peakCombinedTotal += peaks[i];
      peakCombinedCount++;
    } else {
      if (peakCombinedCount > 0) {
        // NOTE(dat): int division auto round toward zero
        // We have to add this peak into our combination
        supressedPeaks.push_back((peakCombinedTotal + peaks[i]) / (peakCombinedCount + 1));
        peakCombinedTotal = 0;
        peakCombinedCount = 0;
      } else {
        supressedPeaks.push_back(peaks[i]);
      }
    }
  }
  if (peakCombinedCount > 0) {
    supressedPeaks.push_back(
      (peakCombinedTotal + peaks[peaks.size() - 1]) / (peakCombinedCount + 1));
  } else {
    supressedPeaks.push_back(peaks[peaks.size() - 1]);
  }
  return supressedPeaks;
}

void computeHistogramMetadata(const Vector<float> &data, size_t numBins, Histogram *histogram) {
  float minV = FLT_MAX;
  float maxV = FLT_MIN;

  // find min, max
  for (auto v : data) {
    if (minV > v) {
      minV = v;
    }
    if (maxV < v) {
      maxV = v;
    }
  }
  histogram->minVal = minV;
  histogram->maxVal = maxV;
  histogram->numBins = numBins;
}

void computeHistogram(const Vector<float> &data, Histogram *histogram) {
  histogram->binWidth = (histogram->maxVal - histogram->minVal) / histogram->numBins;
  histogram->binYs.resize(histogram->numBins);
  std::fill(histogram->binYs.begin(), histogram->binYs.end(), 0.f);
  for (auto v : data) {
    if (v < histogram->minVal) {
      continue;
    }
    if (v > histogram->maxVal) {
      continue;
    }
    int bin = static_cast<int>(std::floor(
      (v - histogram->minVal) / (histogram->maxVal - histogram->minVal) * histogram->numBins));
    // The max value is put into the top bin
    if (bin >= histogram->binYs.size()) {
      bin = histogram->binYs.size() - 1;
    }
    histogram->binYs[bin] += 1;
  }

  histogram->binXs.resize(histogram->numBins);
  for (size_t i = 0; i < histogram->numBins; i++) {
    histogram->binXs[i] = i * histogram->binWidth + histogram->minVal;
  }
}

}  // namespace c8
