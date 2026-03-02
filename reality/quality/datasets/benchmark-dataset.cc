// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules.h"

#include "c8/io/image-io.h"
#include "c8/map.h"
#include "c8/string.h"
#include "c8/vector.h"
#include "reality/quality/datasets/benchmark-dataset.h"

namespace c8 {
namespace {

TreeMap<BenchmarkName, Vector<String>> getDatasets() {
  return {
    {BenchmarkName::SIMPLE10_240x320,
     {
       "reality/quality/datasets/simple10/240x320/benchmark-000.jpg",
       "reality/quality/datasets/simple10/240x320/benchmark-001.jpg",
       "reality/quality/datasets/simple10/240x320/benchmark-002.jpg",
       "reality/quality/datasets/simple10/240x320/benchmark-003.jpg",
       "reality/quality/datasets/simple10/240x320/benchmark-004.jpg",
       "reality/quality/datasets/simple10/240x320/benchmark-005.jpg",
       "reality/quality/datasets/simple10/240x320/benchmark-006.jpg",
       "reality/quality/datasets/simple10/240x320/benchmark-007.jpg",
       "reality/quality/datasets/simple10/240x320/benchmark-008.jpg",
       "reality/quality/datasets/simple10/240x320/benchmark-009.jpg",
     }},
    {BenchmarkName::SIMPLE10_480x640,
     {
       "reality/quality/datasets/simple10/480x640/benchmark-000.jpg",
       "reality/quality/datasets/simple10/480x640/benchmark-001.jpg",
       "reality/quality/datasets/simple10/480x640/benchmark-002.jpg",
       "reality/quality/datasets/simple10/480x640/benchmark-003.jpg",
       "reality/quality/datasets/simple10/480x640/benchmark-004.jpg",
       "reality/quality/datasets/simple10/480x640/benchmark-005.jpg",
       "reality/quality/datasets/simple10/480x640/benchmark-006.jpg",
       "reality/quality/datasets/simple10/480x640/benchmark-007.jpg",
       "reality/quality/datasets/simple10/480x640/benchmark-008.jpg",
       "reality/quality/datasets/simple10/480x640/benchmark-009.jpg",
     }},
    {BenchmarkName::LOBBYTOUR10_480x640,
     {
       "reality/quality/datasets/lobbytour10/480x640/benchmark-000.jpg",
       "reality/quality/datasets/lobbytour10/480x640/benchmark-001.jpg",
       "reality/quality/datasets/lobbytour10/480x640/benchmark-002.jpg",
       "reality/quality/datasets/lobbytour10/480x640/benchmark-003.jpg",
       "reality/quality/datasets/lobbytour10/480x640/benchmark-004.jpg",
       "reality/quality/datasets/lobbytour10/480x640/benchmark-005.jpg",
       "reality/quality/datasets/lobbytour10/480x640/benchmark-006.jpg",
       "reality/quality/datasets/lobbytour10/480x640/benchmark-007.jpg",
       "reality/quality/datasets/lobbytour10/480x640/benchmark-008.jpg",
       "reality/quality/datasets/lobbytour10/480x640/benchmark-009.jpg",
     }},
  };
}

}  // namespace

int BenchmarkDataset::size(BenchmarkName name) {
  static const auto datasets = getDatasets();
  return datasets.at(name).size();
}

YPlanePixelBuffer BenchmarkDataset::loadY(BenchmarkName name, int index) {
  static const auto datasets = getDatasets();
  String filename = datasets.at(name).at(index);
  return readJpgToY(filename);
}

RGB888PlanePixelBuffer BenchmarkDataset::loadRGB(BenchmarkName name, int index) {
  static const auto datasets = getDatasets();
  String filename = datasets.at(name).at(index);
  return readJpgToRGB(filename);
}

BGR888PlanePixelBuffer BenchmarkDataset::loadBGR(BenchmarkName name, int index) {
  static const auto datasets = getDatasets();
  String filename = datasets.at(name).at(index);
  return readJpgToBGR(filename);
}

RGBA8888PlanePixelBuffer BenchmarkDataset::loadRGBA(BenchmarkName name, int index) {
  static const auto datasets = getDatasets();
  String filename = datasets.at(name).at(index);
  return readJpgToRGBA(filename);
}

}  // namespace c8
