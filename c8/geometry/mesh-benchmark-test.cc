// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nathan Waters (nathan@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":mesh",
    "//c8/geometry:facemesh-data",
    "@com_google_benchmark//:benchmark",
  };
}
cc_end(0xdb67fc11);

#include <benchmark/benchmark.h>

#include <algorithm>

#include "c8/geometry/facemesh-data.h"
#include "c8/geometry/mesh.h"
#include "c8/stats/scope-timer.h"

namespace c8 {

static void BenchmarkComputeVertexNormalsWithBox(benchmark::State &state) {
  ScopeTimer t("benchmark-compute-vertex-normals-with-box");

  // clang-format off
  Vector<HPoint3> boxVertices = {
    {0.5f, 0.5f, 0.5f},
    {0.5f, 0.5f, -0.5f},
    {0.5f, -0.5f, 0.5f},
    {0.5f, -0.5f, -0.5f},
    {-0.5f, 0.5f, -0.5f},
    {-0.5f, 0.5f, 0.5f},
    {-0.5f, -0.5f, -0.5f},
    {-0.5f, -0.5f, 0.5f},
  };

  Vector<MeshIndices> boxIndices {
    {0, 2, 1},
    {2, 3, 1},
    {4, 6, 5},
    {6, 7, 5},
    {4, 5, 1},
    {5, 0, 1},
    {7, 6, 2},
    {6, 3, 2},
    {5, 7, 0},
    {7, 2, 0},
    {1, 3, 4},
    {3, 6, 4},
  };
  // clang-format on

  Vector<UV> boxUVs;
  for (auto _ : state) {
    Vector<HVector3> boxVertexNormals;
    computeVertexNormals(boxVertices, boxIndices, &boxVertexNormals);
  }
}

// Register the function as a benchmark
BENCHMARK(BenchmarkComputeVertexNormalsWithBox);

static void BenchmarkComputeVertexNormalsWithFacemeshData(benchmark::State &state) {
  ScopeTimer t("benchmark-compute-vertex-normals-with-facemesh-data");

  Vector<UV> boxUVs;
  for (auto _ : state) {
    Vector<HVector3> boxVertexNormals;
    computeVertexNormals(
      FACEMESH_SAMPLE_VERTICES, FACEMESH_INDICES, &boxVertexNormals);
  }
}

// Register the function as a benchmark
BENCHMARK(BenchmarkComputeVertexNormalsWithFacemeshData);

BENCHMARK_MAIN();

}  // namespace c8
