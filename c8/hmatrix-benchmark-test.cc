// Copyright (c) 2022 Niantic, Inc.
// Original Author: Dat Chu (datchu@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":hmatrix-nosimd",
    "@com_google_benchmark//:benchmark",
  };
}
cc_end(0x9dc2cb82);

#include <benchmark/benchmark.h>

#include "c8/hmatrix.h"

namespace c8 {

class HMatrixBenchmarkTest : public benchmark::Fixture {
protected:
  void SetUp(const ::benchmark::State &state) override {}
};

BENCHMARK_F(HMatrixBenchmarkTest, MatMulMat)(benchmark::State &state) {
  HMatrix a{
    {1.0f, 2.0f, 3.0f, 4.0f},
    {1.0f, 2.0f, 3.0f, 4.0f},
    {1.0f, 2.0f, 3.0f, 4.0f},
    {1.0f, 2.0f, 3.0f, 4.0f}};
  HMatrix b{
    {1.0f, 5.0f, 9.0f, 13.0f},
    {2.0f, 6.0f, 10.0f, 14.0f},
    {3.0f, 7.0f, 11.0f, 15.0f},
    {4.0f, 8.0f, 12.0f, 16.0f}};
  for (auto _ : state) {
    b = a * b;
  }
}

BENCHMARK_F(HMatrixBenchmarkTest, MatMulVecHPoint3)(benchmark::State &state) {
  HMatrix a{
    {1.0f, 2.0f, 3.0f, 4.0f},
    {1.0f, 2.0f, 3.0f, 4.0f},
    {1.0f, 2.0f, 3.0f, 4.0f},
    {1.0f, 2.0f, 3.0f, 4.0f}};
  Vector<HPoint3> b{{1.0f, 2.0f, 3.0f}};

  for (auto _ : state) {
    b = a * b;
    b.push_back({1.1f, 1.2f, 1.3f});
  }
}
}  // namespace c8
