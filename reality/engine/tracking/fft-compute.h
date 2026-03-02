// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Dat Chu (dat@8thwall.com)

#pragma once

#include <memory>

#include "c8/vector.h"
#include "kiss_fft.h"

namespace c8 {

class FftCompute {
public:
  // @param numFft the data length you are going to provide to the compute method
  explicit FftCompute(int numFft);
  virtual ~FftCompute();

  // @param data a series of sampled data points at equal interval. size >= numFft
  // @param samplingRate rate of your data sample in hertz
  void compute(const Vector<float> &data, float samplingRate);
  // Frequency bins. No DC.
  const Vector<float> &frequencies() const { return outputFrequencies_; }
  // Amplitude at corresponding frequency bins. No DC.
  const Vector<float> &amplitudes() const { return outputAmplitudes_; }

private:
  int numFft_;
  kiss_fft_cfg kissFftState_;
  kiss_fft_scalar zero_;
  std::unique_ptr<kiss_fft_cpx[]> cin_;
  std::unique_ptr<kiss_fft_cpx[]> cout_;
  Vector<float> outputFrequencies_;
  Vector<float> outputAmplitudes_;
};

}  // namespace c8
