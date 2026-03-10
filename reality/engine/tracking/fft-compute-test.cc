// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Dat Chu (dat@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {":fft-compute", "//c8:c8-log", "//c8:vector","@com_google_googletest//:gtest_main"};
}
cc_end(0x3488f85b);

#include <cmath>
#include <gtest/gtest.h>

#include "c8/c8-log.h"
#include "reality/engine/tracking/fft-compute.h"

namespace c8 {
class FftComputeTest : public ::testing::Test {};

TEST_F(FftComputeTest, ComputeOnSineWave) {
  Vector<float> sineData;
  for (int i = 0; i < 120; i++) {
    sineData.push_back(std::sin(i * 3 * M_PI / 180));
  }

  FftCompute fft(120);
  fft.compute(sineData, 60);
  EXPECT_EQ(60, fft.frequencies().size());
  EXPECT_EQ(60, fft.amplitudes().size());

  // DC component is the first component
  EXPECT_LT(fft.amplitudes()[0], 1e-6) << "DC component should be zero for centered sine wave";
  EXPECT_FLOAT_EQ(0.f, fft.frequencies()[0]);

  // our sine wave makes one full loop every 120 points,
  // since we are sampling at 60Hz, the sine wave frequency 0.5Hz
  EXPECT_FLOAT_EQ(0.5f, fft.amplitudes()[1]);
  EXPECT_FLOAT_EQ(0.5f, fft.frequencies()[1]);

  // There should be no other frequency data
  for (int i = 2; i < 60; i++) {
    EXPECT_LT(fft.amplitudes()[i], 1e-6) << i;
  }
}

TEST_F(FftComputeTest, ComputeOnOffsetSineWave) {
  Vector<float> sineData;
  for (int i = 0; i < 120; i++) {
    sineData.push_back(std::sin(i * 3 * M_PI / 180) + 4.0f);
  }

  FftCompute fft(120);
  fft.compute(sineData, 60);
  EXPECT_EQ(60, fft.frequencies().size());
  EXPECT_EQ(60, fft.amplitudes().size());

  // DC component is the first component
  EXPECT_FLOAT_EQ(fft.amplitudes()[0], 4.f) << "DC component should be 4 for offset sine wave";
  EXPECT_FLOAT_EQ(0.f, fft.frequencies()[0]);

  // our sine wave makes one full loop every 120 points,
  // since we are sampling at 60Hz, the sine wave frequency 0.5Hz
  EXPECT_FLOAT_EQ(0.5f, fft.amplitudes()[1]);
  EXPECT_FLOAT_EQ(0.5f, fft.frequencies()[1]);

  // There should be no other frequency data
  for (int i = 2; i < 60; i++) {
    EXPECT_LT(fft.amplitudes()[i], 1e-6) << i;
  }
}

TEST_F(FftComputeTest, ComputeOnMultipleSineWave) {
  Vector<float> sineData;
  float freq1 = 20.f;
  float amplitude1 = 1.f;
  float freq2 = 5.f;
  float amplitude2 = 2.f;
  float freq3 = 12.f;
  float amplitude3 = 3.f;
  for (int i = 0; i < 360; i++) {
    // A sine wave x(t) = A * sin(2pi * f * t + psi)
    // where A is the amplitude (half the height from the top of the sine wave to the bottom)
    //       f is the frequency
    //       t is the time variable
    //       psi is the phase (we don't compute return the phase right now so we don't test it either)
    auto sineWave1 = amplitude1 * 2 * std::sin(2 * M_PI * freq1 * i / 360);
    auto sineWave2 = amplitude2 * 2 * std::sin(2 * M_PI * freq2 * i / 360);
    auto sineWave3 = amplitude3 * 2 * std::sin(2 * M_PI * freq3 * i / 360);
    sineData.push_back(sineWave1 + sineWave2 + sineWave3);
  }

  FftCompute fft(360);
  fft.compute(sineData, 360);
  EXPECT_EQ(180, fft.frequencies().size());
  EXPECT_EQ(180, fft.amplitudes().size());

  // DC component is the first component
  EXPECT_LT(fft.amplitudes()[0], 1e-6) << "DC component should be zero for centered sine wave";
  EXPECT_FLOAT_EQ(0.f, fft.frequencies()[0]);

  for (int i = 1; i < 60; i++) {
    // There should be no other frequency data
    if (fft.frequencies()[i] == freq1) {
      EXPECT_FLOAT_EQ(amplitude1, fft.amplitudes()[i]);
      continue;
    }
    if (fft.frequencies()[i] == freq2) {
      EXPECT_FLOAT_EQ(amplitude2, fft.amplitudes()[i]);
      continue;
    }
    if (fft.frequencies()[i] == freq3) {
      EXPECT_FLOAT_EQ(amplitude3, fft.amplitudes()[i]);
      continue;
    }
    EXPECT_LT(fft.amplitudes()[i], 1e-6) << i;
  }
}

TEST_F(FftComputeTest, MultiplySignalMeansMultiplyAmplitude) {
  Vector<float> sineData;
  float freq1 = 10.f;
  float amplitude1 = 1.f;
  float freq2 = 21.f;
  float amplitude2 = 2.f;
  float freq3 = 5.f;
  float amplitude3 = 3.f;
  float signalMultiple = 1.37f; // just some number
  for (int i = 0; i < 360; i++) {
    auto sineWave1 = amplitude1 * 2 * std::sin(2 * M_PI * freq1 * i / 360);
    auto sineWave2 = amplitude2 * 2 * std::sin(2 * M_PI * freq2 * i / 360);
    auto sineWave3 = amplitude3 * 2 * std::sin(2 * M_PI * freq3 * i / 360);
    sineData.push_back(signalMultiple * (sineWave1 + sineWave2 + sineWave3));
  }

  FftCompute fft(360);
  fft.compute(sineData, 360);
  EXPECT_EQ(180, fft.frequencies().size());
  EXPECT_EQ(180, fft.amplitudes().size());

  // DC component is the first component
  EXPECT_LT(fft.amplitudes()[0], 1e-6) << "DC component should be zero for centered sine wave";
  EXPECT_FLOAT_EQ(0.f, fft.frequencies()[0]);

  for (int i = 1; i < 60; i++) {
    // A multiplied signal linearly multiply amplitude of each frequency component.
    if (fft.frequencies()[i] == freq1) {
      EXPECT_FLOAT_EQ(signalMultiple * amplitude1, fft.amplitudes()[i]);
      continue;
    }
    if (fft.frequencies()[i] == freq2) {
      EXPECT_FLOAT_EQ(signalMultiple * amplitude2, fft.amplitudes()[i]);
      continue;
    }
    if (fft.frequencies()[i] == freq3) {
      EXPECT_FLOAT_EQ(signalMultiple * amplitude3, fft.amplitudes()[i]);
      continue;
    }

    // There should be no other frequency data
    EXPECT_LT(fft.amplitudes()[i], 1e-6) << i;
  }
}

}  // namespace c8
