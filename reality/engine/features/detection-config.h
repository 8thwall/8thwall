// Copyright (c) 2025 Niantic, Inc.
// Original Author: Riyaan Bakhda (riyaanbakhda@nianticlabs.com)

#pragma once

#include "reality/engine/features/feature-manager.h"

namespace c8 {

static constexpr int FEATURES_PER_FRAME_GL = 500;

enum GravityPortion : uint8_t {
  // clang-format off
  NONE    = 0b00000000,
  FIRST   = 0b00000001,
  SECOND  = 0b00000010,
  THIRD   = 0b00000100,
  FOURTH  = 0b00001000,
  FIFTH   = 0b00010000,
  SIXTH   = 0b00100000,
  SEVENTH = 0b01000000,
  EIGHTH  = 0b10000000,
  ALL     = 0b11111111,
  // clang-format on
};

// Struct for configuring the detection of keypoints and descriptors
// @param nKeypoints: Number of keypoints to detect
// @param gravityPortions: Bitmask for which of the 8 22.5degree portions to use for gravity
// For example, if gravityPortions[0] = FIRST | SECOND, then the first and second 22.5degree
// portions will get ORB descriptors
// @param allOrb: If true, all keypoints get an ORB descriptor
// @param allGorb: If true, all keypoints get a GORB descriptor
// @param allLearned: If true, all keypoints get a Learned descriptor
struct DetectionConfig {
  int nKeypoints = FEATURES_PER_FRAME_GL;
  GravityPortion gravityPortions[NUM_DESCRIPTOR_TYPES] = {
    NONE,  // ORB
    NONE,  // GORB
    NONE,  // LEARNED
  };
  bool allOrb = false;
  bool allGorb = false;
  bool allLearned = false;

  void clear() {
    allOrb = false;
    allGorb = false;
    allLearned = false;
  }
};

static DetectionConfig DETECTION_CONFIG_ALL_ORB = {
  .allOrb = true,
  .allGorb = false,
  .allLearned = false,
};
static DetectionConfig DETECTION_CONFIG_ALL_GORB = {
  .allOrb = false,
  .allGorb = true,
  .allLearned = false,
};
static DetectionConfig DETECTION_CONFIG_ALL_ORB_GORB = {
  .allOrb = true,
  .allGorb = true,
  .allLearned = false,
};
static DetectionConfig DETECTION_CONFIG_ALL_LEARNED = {
  .allOrb = false,
  .allGorb = false,
  .allLearned = true,
};

}  // namespace c8
