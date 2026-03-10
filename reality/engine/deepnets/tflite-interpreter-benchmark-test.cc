// Copyright (c) 2023 Niantic, Inc.
// Original Author: Dat Chu (datchu@nianticlabs.com)

#include <benchmark/benchmark.h>

#include "reality/engine/deepnets/tflite-interpreter.h"
#include "reality/engine/deepnets/testdata/embedded-semantics.h"

namespace c8 {
class TFLiteInterpreterBenchmarkTest : public benchmark::Fixture {
};

BENCHMARK_F(TFLiteInterpreterBenchmarkTest, randomInput1Thread)(benchmark::State &state) {
  const Vector<uint8_t> tfliteFile(
      embeddedSemanticsFp32TfliteData,
      embeddedSemanticsFp32TfliteData + embeddedSemanticsFp32TfliteSize);
  constexpr int cacheSize = 8000000;
  TFLiteInterpreter interpreter(tfliteFile, cacheSize, 1);

  // running with random input
  for (auto _ : state) {
    // Run the tflite model on the input.
    interpreter->Invoke();
  }
}

BENCHMARK_F(TFLiteInterpreterBenchmarkTest, randomInput4Threads)(benchmark::State &state) {
  const Vector<uint8_t> tfliteFile(
      embeddedSemanticsFp32TfliteData,
      embeddedSemanticsFp32TfliteData + embeddedSemanticsFp32TfliteSize);
  constexpr int cacheSize = 8000000;
  TFLiteInterpreter interpreter(tfliteFile, cacheSize, 4);

  // running with random input
  for (auto _ : state) {
    // Run the tflite model on the input.
    interpreter->Invoke();
  }
}

}  // namespace c8
