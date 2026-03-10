// Copyright (c) 2019 8th Wall, Inc.
// Original Author: Scott Pollack (scott@8thwall.com)

#pragma once

#include "c8/hmatrix.h"
#include "c8/pixels/image-roi.h"

namespace c8 {

struct TrackedImage {
  enum class Status {
    UNKNOWN_STATUS = 0,
    NOT_FOUND = 1,
    LOCATED = 2,
    TRACKED = 3,
  };

  Status status = Status::UNKNOWN_STATUS;
  int index = -1;
  String name;
  HMatrix pose = HMatrixGen::i(); // pose relative to camera
  HMatrix worldPose = HMatrixGen::i(); // pose from tam
  float scale = 1.0f;
  int32_t lastSeen = -1;
  int32_t firstSeen = 0;
  c8_PixelPinholeCameraModel targetK;
  c8_PixelPinholeCameraModel camK;

  // Debug
  HVector3 unfilteredPosition_;
  Quaternion unfilteredRotation_;

  bool isUsable() const { return static_cast<int>(status) > 1; }
  bool isLocated() const { return status == Status::LOCATED; }
  bool isTracked() const { return status == Status::TRACKED; }
  bool everSeen() const { return lastSeen >= 0; }
};

struct LocatedImage {
  HMatrix pose = HMatrixGen::i();
  float width = 0.0f;
  float height = 0.0f;
  ImageRoi roi;
  // For debugging.
  TrackedImage targetSpace;
};

}  // namespace c8
