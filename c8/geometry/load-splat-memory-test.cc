// Copyright (c) 2025 Niantic, Inc.
// Original Author: Diego Mazala (diegomazala@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {":load-splat", "@com_google_googletest//:gtest_main", "//c8:c8-log"};
}
cc_end(0x65a56d9d);

#include <chrono>
#include <fstream>
#include <sstream>
#include <thread>

#include "c8/geometry/load-splat.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#ifdef __APPLE__
#include <mach/mach.h>
#include <mach/task_info.h>
#endif

namespace c8 {

// Helper to get memory usage in MB
size_t getMemoryUsageMB() {
  // Linux implementation
#ifdef __linux__
  FILE *statm = fopen("/proc/self/statm", "r");
  if (statm) {
    long size, resident, share, text, lib, data, dt;
    fscanf(statm, "%ld %ld %ld %ld %ld %ld %ld", &size, &resident, &share, &text, &lib, &data, &dt);
    fclose(statm);
    return (resident * getpagesize()) / (1024 * 1024);  // Convert to MB
  }
  return 0;

  // macOS implementation
#elif defined(__APPLE__)
  // Use task_info API to get memory usage
  struct task_basic_info info;
  mach_msg_type_number_t size = sizeof(info);

  kern_return_t kerr = task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t)&info, &size);
  if (kerr == KERN_SUCCESS) {
    return info.resident_size / (1024 * 1024);  // Convert to MB
  }
  return 0;

#else
  return 0;  // Other platforms not implemented
#endif
}

class SplatMemoryTest : public ::testing::Test {
protected:
  size_t startMemoryMB_ = 0;

  void SetUp() override {
    // Record starting memory usage
    startMemoryMB_ = getMemoryUsageMB();
    C8Log("[load-splat-memory-test] Memory usage [start test]    : %ld MB", startMemoryMB_);
  }

  void TearDown() override {
    // Current memory reporting
    const size_t endMemoryMB = getMemoryUsageMB();
    C8Log("[load-splat-memory-test] Memory usage [end test]      : %ld MB", endMemoryMB);
    C8Log(
      "[load-splat-memory-test] Memory usage [delta test]    : %ld MB",
      (endMemoryMB - startMemoryMB_));
  }

  void LogMemory(const std::string &checkpoint) const {
    const size_t currentMemory = getMemoryUsageMB();
    C8Log(
      "[load-splat-memory-test] Memory usage [%s] : %ld MB (%ld MB)",
      checkpoint.c_str(),
      currentMemory,
      (currentMemory - startMemoryMB_));
  }

  static size_t CalculateSplatRawDataSize(const SplatRawData &result) {
    return sizeof(SplatMetadata) + result.positions.size() * sizeof(float)
      + result.scales.size() * sizeof(float) + result.rotations.size() * sizeof(float)
      + result.alphas.size() * sizeof(float) + result.colors.size() * sizeof(float)
      + result.sh.size() * sizeof(float);
  }

  // Helper function to create a synthetic PLY file in memory with the specified header structure
  static Vector<uint8_t> CreateSyntheticPly(size_t numPoints) {
    // Construct PLY header
    std::stringstream headerStream;
    headerStream << "ply\n"
                 << "format binary_little_endian 1.0\n"
                 << "element vertex " << numPoints << "\n"
                 << "property float x\n"
                 << "property float y\n"
                 << "property float z\n"
                 << "property float nx\n"
                 << "property float ny\n"
                 << "property float nz\n"
                 << "property float f_dc_0\n"
                 << "property float f_dc_1\n"
                 << "property float f_dc_2\n";

    // Add f_rest_0 through f_rest_44
    for (int i = 0; i <= 44; i++) {
      headerStream << "property float f_rest_" << i << "\n";
    }

    headerStream << "property float opacity\n"
                 << "property float scale_0\n"
                 << "property float scale_1\n"
                 << "property float scale_2\n"
                 << "property float rot_0\n"
                 << "property float rot_1\n"
                 << "property float rot_2\n"
                 << "property float rot_3\n"
                 << "end_header\n";

    std::string header = headerStream.str();

    // Calculate vertex size - 62 float properties per vertex
    constexpr size_t FLOATS_PER_VERTEX =
      62;  // 3 pos + 3 normal + 3 dc + 45 rest + 1 opacity + 3 scale + 4 rot
    constexpr size_t VERTEX_SIZE = FLOATS_PER_VERTEX * sizeof(float);

    // Calculate total size and allocate buffer
    const size_t totalSize = header.size() + (numPoints * VERTEX_SIZE);
    Vector<uint8_t> plyData(totalSize);

    // Copy header
    std::copy(header.begin(), header.end(), plyData.begin());

    // Create sample vertex data (simple pattern for testing)
    Vector<float> sampleVertex(FLOATS_PER_VERTEX, 0.0f);
    for (int i = 0; i < FLOATS_PER_VERTEX; i++) {
      sampleVertex[i] = static_cast<float>(i % 10) * 0.1f;  // Simple repeating pattern
    }

    // Fill data section with repeating vertices
    const size_t headerSize = header.size();
    for (size_t i = 0; i < numPoints; i++) {
      std::memcpy(
        plyData.data() + headerSize + (i * VERTEX_SIZE), sampleVertex.data(), VERTEX_SIZE);
    }

    return plyData;
  }
};

TEST_F(SplatMemoryTest, TestMemoryUsage) {
  constexpr size_t NUM_POINTS = 1000000;
  const auto syntheticPly = CreateSyntheticPly(NUM_POINTS);

  LogMemory("Data Creation");

  const size_t beforeMemoryMB = getMemoryUsageMB();
  const auto startTime = std::chrono::high_resolution_clock::now();
  const auto splatData = loadSplatPlyData(syntheticPly.data(), syntheticPly.size());
  const auto endTime = std::chrono::high_resolution_clock::now();
  const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
  const size_t afterMemoryMB = getMemoryUsageMB();
  const size_t expectedMemoryMB = CalculateSplatRawDataSize(splatData) / (1024 * 1024);
  const auto memoryDiffMB = afterMemoryMB - beforeMemoryMB;

  C8Log("[load-splat-memory-test] Time taken (ms)              : %ld", duration.count());
  C8Log(
    "[load-splat-memory-test] Points loaded                : %ld", splatData.positions.size() / 3);
  C8Log("[load-splat-memory-test] Memory used  [splat data]    : %ld MB", expectedMemoryMB);
  C8Log(
    "[load-splat-memory-test] Memory used  [after - before]: %ld MB", memoryDiffMB);

  ASSERT_EQ(splatData.positions.size() / 3, NUM_POINTS);

  // Only check memory usage on platforms where memory reporting works
  if (beforeMemoryMB > 0 || afterMemoryMB > 0) {
    ASSERT_LE(memoryDiffMB, expectedMemoryMB * 1.2);  // Allow 20% overhead, --config=asan adds more
  } else {
    C8Log("[load-splat-memory-test] WARNING: Memory reporting not working, skipping memory check");
  }

}

}  // namespace c8
