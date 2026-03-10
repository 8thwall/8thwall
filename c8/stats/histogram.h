// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Dat Chu (dat@8thwall.com)

#pragma once

#include "c8/vector.h"

namespace c8 {

struct Histogram {
  // parameters
  float minVal;
  float maxVal;
  size_t numBins;

  // computed values based on set parameters when computeHistogram is called
  float binWidth;
  Vector<float> binYs;
  Vector<float> binXs;
};

// Compute histogram minVal, maxVal and set numBins
void computeHistogramMetadata(const Vector<float> &data, size_t numBins, Histogram *histogram);
// Call after the histogram metadata is set to find binYs, binXs and binWidth
void computeHistogram(const Vector<float> &data, Histogram *histogram);

Vector<int> findPeaks(Vector<float> ys, int radius = 2, float minVal = 0);
Vector<int> mergePeaksInRadius(const Vector<int> &peaks, int radius = 2);

}  // namespace c8
