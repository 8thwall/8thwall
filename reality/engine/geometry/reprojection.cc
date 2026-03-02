// Copyright (c) 2024 Niantic, Inc.
// Original Author: Riyaan Bakhda (riyaanbakhda@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "reprojection.h",
  };
  deps = {
    "//c8:hmatrix",
    "//c8:hpoint",
    "//c8:vector",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x25cd46d0);

#include "reprojection.h"

namespace c8 {

namespace {

constexpr int HPOINT2_INCREMENT = 3;
constexpr int HPOINT3_INCREMENT = 4;

static inline void fillZInvBuffer(
  const Vector<HPoint3> &worldPts, const HMatrix &worldToCam, Vector<float> *zInvBuffer) {
  zInvBuffer->resize(worldPts.size());
  const float *worldPtsPtr = reinterpret_cast<const float *>(worldPts.data());
  float *zInvBufferPtr = zInvBuffer->data();
  const float *zInvBufferEnd = zInvBufferPtr + zInvBuffer->size();
  while (zInvBufferPtr < zInvBufferEnd) {
    *zInvBufferPtr = 1.f
      / (worldToCam(2, 0) * worldPtsPtr[0] + worldToCam(2, 1) * worldPtsPtr[1]
         + worldToCam(2, 2) * worldPtsPtr[2] + worldToCam(2, 3));
    ++zInvBufferPtr;
    worldPtsPtr += HPOINT3_INCREMENT;
  }
}

inline float square(float x) { return x * x; }

}  // namespace

void reprojectionInliers(
  const Vector<HPoint3> &worldPts,
  const HMatrix &worldToCam,
  const Vector<HPoint2> &rays,
  const float sqResidualThresh,
  Vector<size_t> *inliers) {
  inliers->clear();
  static Vector<float> zInvBuffer;
  fillZInvBuffer(worldPts, worldToCam, &zInvBuffer);
  const float *raysPtr = reinterpret_cast<const float *>(rays.data());
  const float *worldPtsPtr = reinterpret_cast<const float *>(worldPts.data());
  const float *zInvBufferPtr = zInvBuffer.data();
  for (size_t i = 0; i < rays.size(); ++i) {
    const float zInv = *zInvBufferPtr;
    ++zInvBufferPtr;
    if (std::signbit(zInv)) {
      raysPtr += HPOINT2_INCREMENT;
      worldPtsPtr += HPOINT3_INCREMENT;
      continue;
    }
    const float rayHomogInv = 1.f / raysPtr[2];
    // Note: Technically, the HPoint3s are homogeneous so we should be dividing by the Dth element,
    // but we can get away without that because the Dth element is assumed to always be 1.
    const float dxdx = square(
      raysPtr[0] * rayHomogInv
      - (worldToCam(0, 0) * worldPtsPtr[0] + worldToCam(0, 1) * worldPtsPtr[1]
         + worldToCam(0, 2) * worldPtsPtr[2] + worldToCam(0, 3))
        * zInv);
    const float dydy = square(
      raysPtr[1] * rayHomogInv
      - (worldToCam(1, 0) * worldPtsPtr[0] + worldToCam(1, 1) * worldPtsPtr[1]
         + worldToCam(1, 2) * worldPtsPtr[2] + worldToCam(1, 3))
        * zInv);
    if (dxdx + dydy < sqResidualThresh) {
      inliers->push_back(i);
    }
    raysPtr += HPOINT2_INCREMENT;
    worldPtsPtr += HPOINT3_INCREMENT;
  }
}

}  // namespace c8
