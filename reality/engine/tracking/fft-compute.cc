// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Dat Chu (dat@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"fft-compute.h"};
  deps = {
    "//c8:c8-log",
    "//c8:vector",
    "@kissfftlib//:kissfft",
  };
}
cc_end(0x632f7415);

#include <cmath>
#include "reality/engine/tracking/fft-compute.h"

namespace c8 {

FftCompute::FftCompute(int numFft) {
  memset(
    &zero_, 0, sizeof(zero_));  // ugly way of setting short,int,float,double, or __m128 to zero

  cin_.reset(new kiss_fft_cpx[numFft]);
  cout_.reset(new kiss_fft_cpx[numFft]);
  kissFftState_ = kiss_fft_alloc(numFft, 0, 0, 0);
  outputFrequencies_.resize(numFft / 2);
  outputAmplitudes_.resize(numFft / 2);
  numFft_ = numFft;
}

FftCompute::~FftCompute() { free(kissFftState_); }

void FftCompute::compute(const Vector<float> &data, float samplingRate) {
  // copy data into our own structure
  for (int i = 0; i < numFft_; ++i) {
    cin_[i].r = data[i];
    cin_[i].i = zero_;
  }

  // compute Fourier Transform
  kiss_fft(kissFftState_, cin_.get(), cout_.get());

  // We only need half of the output. The other half is a mirror
  for (int i = 0; i < (numFft_ / 2); ++i) {
    outputAmplitudes_[i] =
      std::sqrt(cout_[i].r * cout_[i].r + cout_[i].i * cout_[i].i) * 1.f / numFft_;
  }

  for (int i = 0; i < (numFft_ / 2); i++) {
    outputFrequencies_[i] = i * samplingRate / numFft_;
  }
}

}  // namespace c8
