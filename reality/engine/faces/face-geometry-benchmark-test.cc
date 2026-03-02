// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nathan Waters (nathan@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":face-geometry",
    "//c8/camera:device-infos",
    "//c8/geometry:facemesh-data",
    "//c8/geometry:intrinsics",
    "@com_google_benchmark//:benchmark",
  };
}
cc_end(0x09ed61fd);

#include <benchmark/benchmark.h>

#include <algorithm>

#include "c8/camera/device-infos.h"
#include "c8/geometry/facemesh-data.h"
#include "c8/geometry/intrinsics.h"
#include "c8/stats/scope-timer.h"
#include "reality/engine/faces/face-geometry.h"

namespace c8 {

static void BenchmarkDetectionToFaceNoFilter(benchmark::State &state) {
  DetectedPoints a{
    1.0f,                                              // confidence
    0,                                                 // detectedClass
    {0, 0, 128, 128},                                  // viewport
    {ImageRoi::Source::FACE, 0, "", HMatrixGen::i()},  // roi
    {
      // boundingBox
      {.25f, 0.5f},  // upper left
      {.75f, 0.5f},  // upper right
      {.25f, 1.0f},  // lower left
      {1.0f, 1.0f},  // lower right
    },
    FACEMESH_SAMPLE_OUTPUT,                                        // points
    Intrinsics::getCameraIntrinsics(DeviceInfos::APPLE_IPHONE_6),  // intrinsics
  };

  ScopeTimer rt("test-detection-to-mesh");

  for (auto _ : state) {
    const auto faceData = detectionToMeshNoFilter(a);
  }
}

// Register the function as a benchmark
BENCHMARK(BenchmarkDetectionToFaceNoFilter);

BENCHMARK_MAIN();

}  // namespace c8
