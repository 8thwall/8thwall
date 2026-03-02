// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)
//
// Gr8 features were originally forked from OpenCV's orb features.
//
// Software License Agreement (BSD License)
//
//  Copyright (c) 2009, Willow Garage, Inc.
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without modification, are permitted
//  provided that the following conditions are met:
//
//   * Redistributions of source code must retain the above copyright notice, this list of
//     conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright notice, this list of
//     conditions and the following disclaimer in the documentation and/or other materials provided
//     with the distribution.
//   * Neither the name of the Willow Garage nor the names of its contributors may be used to
//     endorse or promote products derived from this software without specific prior written
//     permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
//  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
//  FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
//  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
//  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.

#include "bzl/inliner/rules2.h"

cc_library {
  visibility = {
    "//visibility:public",
  };
  hdrs = {
    "gr8gl.h",
    "gr8gl-data.h",
  };
  deps = {
    ":frame-point",
    ":image-point",
    ":keypoint-queue",
    "//c8:c8-log",
    "//c8:exceptions",
    "//c8:random-numbers",
    "//c8:task-queue",
    "//c8:thread-pool",
    "//c8:vector",
    "//c8/string:format",
    "//c8/pixels:gr8-pyramid",
    "//c8/pixels:pixel-buffer",
    "//c8/pixels:pixel-transforms",
    "//c8/stats:scope-timer",
  };
}
cc_end(0x375d0b28);

#ifdef _WIN32
#define _USE_MATH_DEFINES
#endif
#include <math.h>

#include <array>
#include <cfloat>
#include <cmath>
#include <future>
#include <iterator>
#include <numeric>
#include <queue>
#include <vector>

#include "c8/c8-log.h"
#include "c8/exceptions.h"
#include "c8/pixels/pixel-transforms.h"
#include "c8/random-numbers.h"
#include "c8/stats/scope-timer.h"
#include "c8/string/format.h"
#include "reality/engine/features/gr8gl-data.h"
#include "reality/engine/features/gr8gl.h"

#define GET_VALUE(idx) (*(center + lut[idx][quantAngle]))

////////////////////////////////////////////////////////////////////////////////////////////////////
namespace c8 {

using gr8gldata::FEATURE_LUT;

namespace {

#ifdef JAVASCRIPT
constexpr bool USE_MULTITHREADED_FEATURES = false;
#else
constexpr bool USE_MULTITHREADED_FEATURES = true;
#endif

std::array<std::array<int32_t, 36>, 512> INDEX_FEATURE_LUT{};
int lutStep = 0;
void initFeatureLut(int step) {
  if (step == lutStep) {
    return;
  }
  for (int i = 0; i < 512; i++) {
    for (int j = 0; j < 36; j++) {
      INDEX_FEATURE_LUT[i][j] = FEATURE_LUT[i][j].second * step + FEATURE_LUT[i][j].first * 4;
    }
  }
  lutStep = step;
}

// TODO(mc): Switch to std::hardware_destructive_interference_size when we get to C++17.
constexpr std::size_t destructiveInterferenceSize = 64;

template <size_t N>
struct cacheSafeUnsignedCharArray {
  alignas(destructiveInterferenceSize) unsigned char value[N];
};

// TODO(nb): revert to more uniform bins when tracking is stabilized.
constexpr int bin(int x, int y, int wbp, int hbp, int numBinCols) {
  return numBinCols * (y / hbp) + (x / wbp);
}
/*
constexpr int RETAIN_BEST_BIN_PIXELS = 5;
constexpr int bin(int x, int y, int numBinCols) {
  return numBinCols * (y >> RETAIN_BEST_BIN_PIXELS) + (x >> RETAIN_BEST_BIN_PIXELS);
}
*/

}  // namespace

void Gr8Gl::retainBest(
  Vector<ImagePointLocation> &keypoints,
  int nPoints,
  int cols,
  int rows,
  int edgeThreshold,
  bool quantizeTo256) {
  // If we already have enough points, then we're done.
  if (keypoints.size() <= nPoints) {
    return;
  }

  // If we have no budget for points, clear all of them.
  if (nPoints == 0) {
    keypoints.clear();
    return;
  }
  std::array<int, 12000> binCounts{};

  Vector<std::pair<int, ImagePointLocation>> collisionToScore;
  collisionToScore.reserve(keypoints.size());
  if (quantizeTo256) {
    std::array<int, 256> count{};
    for (auto &keypoint : keypoints) {
      const uint8_t quantizedResponse = 255 - static_cast<uint8_t>(keypoint.response);
      ++count[quantizedResponse];
    }

    int total = 0;
    for (int i = 0; i < count.size(); ++i) {
      int oldCount = count[i];
      count[i] = total;
      total += oldCount;
    }

    Vector<ImagePointLocation> sortedKeypoints;
    sortedKeypoints.resize(keypoints.size());
    for (int i = 0; i < keypoints.size(); ++i) {
      const uint8_t quantizedResponse = 255 - static_cast<uint8_t>(keypoints[i].response);
      sortedKeypoints[count[quantizedResponse]] = keypoints[i];
      count[quantizedResponse] += 1;
    }

    swap(sortedKeypoints, keypoints);
  } else {
    std::stable_sort(keypoints.begin(), keypoints.end(), KeypointResponseMore());
  }

  constexpr float minpixperbin = 60.0f;
  int xw = cols - 2 * edgeThreshold;
  int yh = rows - 2 * edgeThreshold;
  float xbins = std::max(std::floor(xw / minpixperbin), 1.0f);
  float ybins = std::max(std::floor(yh / minpixperbin), 1.0f);
  int wbp = static_cast<int>(std::ceil(xw / xbins));
  int hbp = static_cast<int>(std::ceil(yh / ybins));

  /*
  int bincols = std::ceil(static_cast<float>(cols) / (1 << RETAIN_BEST_BIN_PIXELS));
  */
  for (const auto &keypoint : keypoints) {
    /*
    const int bini = bin(keypoint.pt.x, keypoint.pt.y, bincols);
    */
    // TODO(nb): revert to more uniform bins when tracking is stabilized.
    const int bini =
      bin(keypoint.pt.x - edgeThreshold, keypoint.pt.y - edgeThreshold, wbp, hbp, xbins);
    collisionToScore.push_back(std::make_pair(binCounts[bini]++, keypoint));
  }
  // We are only interested in nPoints from this list
  std::nth_element(
    collisionToScore.begin(),
    collisionToScore.begin() + nPoints,
    collisionToScore.end(),
    KeypointResponsePairMore());

  keypoints.resize(nPoints);
  for (size_t i = 0; i < nPoints; i++) {
    keypoints[i] = collisionToScore[i].second;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

static float angleFromCenterOfMass(
  const uint8_t *center, int step, const ImagePointLocation &pt, const Vector<int> &circularMask) {
  // circularMask[16] = [ 15, 15, 15, 15, 14, 14, 14, 13, 13, 12, 11, 10, 9, 8, 6, 3];

  // Create an accumulation buffer to remove extra multiplies.
  constexpr int HALF_K = Gr8Gl::ANGLES_PATCH_SIZE / 2;  // HALF_K=15
  std::array<int, HALF_K * 2 + 1> sumOfV{};             // sumOfV.size() = 31

  int vWeightedSumOfH = 0;
  int hWeightedSumOfV = 0;

  // Go line by line in the circular patch. Each iteration covers two lines from the middle out.
  // We skip over v=0 at this point because its weight contribution is 0.
  int widthStep = 0;
  for (int v = 1; v <= HALF_K; ++v) {  // for v = 1; v <= 15; ++v
    widthStep += step;                 // one row, two rows, three rows, etc. starting at 1.
    // Proceed over the two lines.
    int twoLinesSum = 0;
    int d = circularMask[v];
    for (int u = -d, u4 = -(d << 2); u <= d; ++u, u4 += 4) {
      int pixelLower = center[u4 + widthStep];
      int pixelUpper = center[u4 - widthStep];

      // For the center of mass, we need v * pixelLower + (-v) * pixelUpper, but we factor v out of
      // the multiply until later.
      twoLinesSum += pixelLower - pixelUpper;

      // Add both of these pixels to the sum of this vertical line.
      sumOfV[u + HALF_K] += pixelLower + pixelUpper;
    }
    // Add these two lines to the total weighted sum, factoring out the weight to a single multiply
    // for the whole two lines.
    vWeightedSumOfH += v * twoLinesSum;
  }

  // Now compute the horizontal weighted sum of vertical pixels.
  for (int u = -HALF_K, u4 = -(HALF_K << 2); u <= HALF_K; ++u, u4 += 4) {
    // Since we skipped over the center line above, we need to add it to the vertical line sum here
    // before multiplying by the weight.
    hWeightedSumOfV += u * (center[u4] + sumOfV[u + HALF_K]);
  }

  // Return the angle of the vector that points from the center of the patch to the circular-roi
  // patch center of mass.
  return fastAtan2(static_cast<float>(vWeightedSumOfH), static_cast<float>(hWeightedSumOfV));
}

static void computeCenterOfMassAngles(
  const Gr8Pyramid &pyramid,
  const Vector<size_t> &keypointIndices,
  Vector<ImagePointLocation> &pts,
  const Vector<int> &circularMask) {
  int step = pyramid.data.rowBytes();
  for (auto &pt : pts) {

    auto layer = pt.roi < 0 ? pyramid.levels[pt.scale] : pyramid.rois[pt.roi].layout;
    int cr = static_cast<int>(std::round(pt.pt.y)) + layer.r;
    int cc = static_cast<int>(std::round(pt.pt.x)) + layer.c;

    // Compute center of mass from gaussian blurred image, effectively increasing ROI size.
    // TODO(nb): revert to computing angle on gaussian image when tracking is stabilized.
    const uint8_t *center = pyramid.data.pixels() + cr * pyramid.data.rowBytes() + (cc << 2) + 1;
    // const uint8_t *center = pyramid.data.pixels() + cr * pyramid.data.rowBytes() + (cc << 2) + 0;

    pt.angle = angleFromCenterOfMass(center, step, pt, circularMask);
  }
}

inline float angleFromGravity(
  const HMatrix &camToWorld,
  const HPoint3 &rayInCam,
  const HPoint3 &rayInWorld,
  float gravityAngleOffset) {
  // Add small epsilon in the up direction to get up direction at the ray's position
  // Note: Any number between (0, 1) exclusive should work here, but numbers close to 0 may have
  // precision issues and numbers close to 1 may have large projected distortion; 0.1 should be a
  // good compromise between these extremes
  static constexpr float upEpsilon = 0.1f;
  const auto upPt = HPoint3{rayInWorld.x(), rayInWorld.y() + upEpsilon, rayInWorld.z()};
  // Compute up vector at the ray's position in camera space
  const auto upPtInCam = (camToWorld.inv() * upPt).flatten();
  const auto yDelta = upPtInCam.y() - rayInCam.y();
  const auto xDelta = upPtInCam.x() - rayInCam.x();
  // Return the angle of that vector
  const auto angle = -fastAtan2(yDelta, xDelta) + gravityAngleOffset;
  return angle < 0.f ? angle + 360.f : angle;
}

static void computeGravityAngles(
  const Gr8Pyramid &pyramid,
  const Vector<size_t> &keypointIndices,
  const FrameWithPoints &frame,
  const Vector<HPoint3> &raysInCam,
  const Vector<HPoint3> &raysInWorld,
  Vector<ImagePointLocation> *pts,
  float gravityAngleOffset) {

  // For rotated pyramid levels, rotate camera pose by -90 degrees around the z-axis.
  HMatrix camToWorld = frame.xrDevicePose().toRotationMat();
  HMatrix rotatedCamToWorld = camToWorld * HMatrixGen::zRadians(-M_PI / 2);

  for (auto gorbKptIdx : keypointIndices) {
    // Compute gravity angle
    if (pyramid.levels[pts->at(gorbKptIdx).scale].rotated) {
      pts->at(gorbKptIdx).gravityAngle = angleFromGravity(
        rotatedCamToWorld, raysInCam[gorbKptIdx], raysInWorld[gorbKptIdx], gravityAngleOffset);
    } else {
      pts->at(gorbKptIdx).gravityAngle = angleFromGravity(
        camToWorld, raysInCam[gorbKptIdx], raysInWorld[gorbKptIdx], gravityAngleOffset);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

namespace {
void computeGr8Descriptors(
  const Gr8Pyramid &pyramid,
  const Vector<size_t> &keypointIndices,
  bool useGravityAngle,
  ImagePoints *pointsWithDescriptors,
  int wta_k,
  TaskQueue *taskQueue,
  ThreadPool *threadPool) {
  int nkeypoints = (int)keypointIndices.size();

  constexpr int DSIZE = Gr8Gl::descriptorSize();

  Vector<cacheSafeUnsignedCharArray<DSIZE>> threadDesc(nkeypoints);

#if CV_NEON
  constexpr uint8x8_t bitMask = {
    0x80,
    0x40,
    0x20,
    0x10,
    0x8,
    0x4,
    0x2,
    0x1,
  };
#endif

  auto computeDescriptor = [&](int keypointIdx, int j) {
    ImagePointLocation kpt = pointsWithDescriptors->at(keypointIdx).location();
    auto level = kpt.roi < 0 ? pyramid.levels[kpt.scale] : pyramid.rois[kpt.roi].layout;
    auto angle = useGravityAngle ? kpt.gravityAngle : kpt.angle;
    int quantAngle = ((static_cast<int>(angle + 5.0f) / 10)) % 36;

    int r = static_cast<int>(std::round(kpt.pt.y)) + level.r;
    int c = static_cast<int>(std::round(kpt.pt.x)) + level.c;
    const uint8_t *center = pyramid.data.pixels() + r * pyramid.data.rowBytes() + (c << 2) + 1;
    const auto *lut = &INDEX_FEATURE_LUT[0];
    uint8_t *desc = threadDesc[j].value;

    if (wta_k == 2) {
      for (int i = 0; i < DSIZE; ++i, lut += 16) {
#if CV_NEON
        uint8x8_t tV0 = {
          GET_VALUE(14),
          GET_VALUE(12),
          GET_VALUE(10),
          GET_VALUE(8),
          GET_VALUE(6),
          GET_VALUE(4),
          GET_VALUE(2),
          GET_VALUE(0),
        };
        uint8x8_t tV1 = {
          GET_VALUE(15),
          GET_VALUE(13),
          GET_VALUE(11),
          GET_VALUE(9),
          GET_VALUE(7),
          GET_VALUE(5),
          GET_VALUE(3),
          GET_VALUE(1),
        };
        int8x8_t lt = vclt_u8(tV0, tV1);
        int8x8_t masked = vand_u8(lt, bitMask);

        int8_t output[8];
        vst1_s8(output, masked);
        desc[i] = (uint8_t)(output[0] | output[1] | output[2] | output[3] | output[4] | output[5]
                            | output[6] | output[7]);
#else
        int t0, t1, val;
        t0 = GET_VALUE(0);
        t1 = GET_VALUE(1);
        val = t0 < t1;
        t0 = GET_VALUE(2);
        t1 = GET_VALUE(3);
        val |= (t0 < t1) << 1;
        t0 = GET_VALUE(4);
        t1 = GET_VALUE(5);
        val |= (t0 < t1) << 2;
        t0 = GET_VALUE(6);
        t1 = GET_VALUE(7);
        val |= (t0 < t1) << 3;
        t0 = GET_VALUE(8);
        t1 = GET_VALUE(9);
        val |= (t0 < t1) << 4;
        t0 = GET_VALUE(10);
        t1 = GET_VALUE(11);
        val |= (t0 < t1) << 5;
        t0 = GET_VALUE(12);
        t1 = GET_VALUE(13);
        val |= (t0 < t1) << 6;
        t0 = GET_VALUE(14);
        t1 = GET_VALUE(15);
        val |= (t0 < t1) << 7;

        desc[i] = (uint8_t)val;
#endif
      }
    } else if (wta_k == 3) {
      for (int i = 0; i < DSIZE; ++i, lut += 12) {
        int t0, t1, t2, val;
        t0 = GET_VALUE(0);
        t1 = GET_VALUE(1);
        t2 = GET_VALUE(2);
        val = t2 > t1 ? (t2 > t0 ? 2 : 0) : (t1 > t0);

        t0 = GET_VALUE(3);
        t1 = GET_VALUE(4);
        t2 = GET_VALUE(5);
        val |= (t2 > t1 ? (t2 > t0 ? 2 : 0) : (t1 > t0)) << 2;

        t0 = GET_VALUE(6);
        t1 = GET_VALUE(7);
        t2 = GET_VALUE(8);
        val |= (t2 > t1 ? (t2 > t0 ? 2 : 0) : (t1 > t0)) << 4;

        t0 = GET_VALUE(9);
        t1 = GET_VALUE(10);
        t2 = GET_VALUE(11);
        val |= (t2 > t1 ? (t2 > t0 ? 2 : 0) : (t1 > t0)) << 6;

        desc[i] = (uint8_t)val;
      }
    } else if (wta_k == 4) {
      for (int i = 0; i < DSIZE; ++i, lut += 16) {
        int t0, t1, t2, t3, u, v, k, val;
        t0 = GET_VALUE(0);
        t1 = GET_VALUE(1);
        t2 = GET_VALUE(2);
        t3 = GET_VALUE(3);
        u = 0, v = 2;
        if (t1 > t0)
          t0 = t1, u = 1;
        if (t3 > t2)
          t2 = t3, v = 3;
        k = t0 > t2 ? u : v;
        val = k;

        t0 = GET_VALUE(4);
        t1 = GET_VALUE(5);
        t2 = GET_VALUE(6);
        t3 = GET_VALUE(7);
        u = 0, v = 2;
        if (t1 > t0)
          t0 = t1, u = 1;
        if (t3 > t2)
          t2 = t3, v = 3;
        k = t0 > t2 ? u : v;
        val |= k << 2;

        t0 = GET_VALUE(8);
        t1 = GET_VALUE(9);
        t2 = GET_VALUE(10);
        t3 = GET_VALUE(11);
        u = 0, v = 2;
        if (t1 > t0)
          t0 = t1, u = 1;
        if (t3 > t2)
          t2 = t3, v = 3;
        k = t0 > t2 ? u : v;
        val |= k << 4;

        t0 = GET_VALUE(12);
        t1 = GET_VALUE(13);
        t2 = GET_VALUE(14);
        t3 = GET_VALUE(15);
        u = 0, v = 2;
        if (t1 > t0)
          t0 = t1, u = 1;
        if (t3 > t2)
          t2 = t3, v = 3;
        k = t0 > t2 ? u : v;
        val |= k << 6;

        desc[i] = (uint8_t)val;
      }
    } else {
      C8_THROW("Wrong wta_k. It can be only 2, 3 or 4.");
    }
  };

  auto computeDescriptorRange =
    [&computeDescriptor](const Vector<size_t> &kptIndices, const int start, const int end) {
      for (int j = start; j < end; ++j) {
        computeDescriptor(kptIndices[j], j);
      }
    };

  // Split work up into blocks.
  const int blockSize = 30;
  for (int j = 0; j < nkeypoints; j += blockSize) {
    taskQueue->addTask(
      std::bind(
        std::cref(computeDescriptorRange),
        keypointIndices,
        j,
        std::min(nkeypoints, j + blockSize)));
  }

  taskQueue->executeWithThreadPool(threadPool);

  for (int j = 0; j < nkeypoints; j++) {
    uint8_t *src = threadDesc[j].value;
    auto kptIdx = keypointIndices[j];
    if (useGravityAngle) {
      pointsWithDescriptors->at(kptIdx).mutableFeatures().gorbFeature = GorbFeature();
      std::copy(
        src,
        src + DSIZE,
        pointsWithDescriptors->at(kptIdx).mutableFeatures().gorbFeature->mutableData());
    } else {
      pointsWithDescriptors->at(kptIdx).mutableFeatures().orbFeature = OrbFeature();
      std::copy(
        src,
        src + DSIZE,
        pointsWithDescriptors->at(kptIdx).mutableFeatures().orbFeature->mutableData());
    }
  }
}

}  // namespace

void keypointsForCrop(
  ConstRGBA8888PlanePixels s,
  uint8_t scale,
  int8_t roi,
  int edgeThreshold,
  Vector<ImagePointLocation> &keypoints) {
  int r = s.rows() - 2 * edgeThreshold;
  int c = s.cols() - 2 * edgeThreshold;

  const uint8_t *rowStart = s.pixels() + edgeThreshold * s.rowBytes() + 4 * edgeThreshold + 2;
  for (int i = 0; i < r; ++i) {
    const uint8_t *e = rowStart + 4 * c;
    for (const uint8_t *p = rowStart; p < e; p += 4) {
      if (*p) {
        // Get the integer pixel location from the byte position.
        float y = static_cast<float>(i + edgeThreshold);
        float x = static_cast<float>(((p - rowStart) >> 2) + edgeThreshold);

        // Decode the subpixel offsets from the alpha channel.
        constexpr float offsetScale = 1.0 / 14.0;
        float xoffset = offsetScale * static_cast<float>(p[1] % 16) - 0.5f;
        float yoffset = offsetScale * static_cast<float>(p[1] >> 4) - 0.5f;

        // x and y are scaled later.
        keypoints.emplace_back(
          x + xoffset,
          y + yoffset,
          static_cast<float>(Gr8Gl::PATCH_SIZE),
          scale,
          -1 /* angle */,
          -1, /* gravityAngle */
          *p /* response */,
          roi);
      }
    }
    rowStart += s.rowBytes();
  }
}

// NOTE(nb): This method is faster in WebAssembly than keypointsForCrop. When we run benchmark that
// uses this method (default engine behavior) vs keypointsForCrop, the engine runs faster.
//
// clang-format off
// For sequence 20180706/log.1530230399-600-carpet-zoom, when we use keypointsForCropJs
//   /xr-engine/features/gr8gl/compute-key-points:  600 events  (Latency: 365.345 micros -- 50%/90%/99%: 378.898/485.38/509.338)
// vs keypointsForCrop
//   /xr-engine/features/gr8gl/compute-key-points:  600 events  (Latency: 559.85 micros -- 50%/90%/99%: 513.712/924.682/1017.15)
// clang-format on
void keypointsForCropJs(
  ConstRGBA8888PlanePixels s,
  uint8_t scale,
  int8_t roi,
  int edgeThreshold,
  Vector<ImagePointLocation> &keypoints) {
  std::array<const uint8_t *, 480 * 64> ptidxs;
  int r = s.rows() - 2 * edgeThreshold;
  int c = s.cols() - 2 * edgeThreshold;

  int nptidxs = 0;
  const uint8_t *rowStart = s.pixels() + edgeThreshold * s.rowBytes() + 4 * edgeThreshold + 2;
  for (int i = 0; i < r; ++i) {
    const uint8_t *e = rowStart + 4 * c;
    for (const uint8_t *p = rowStart; p < e; p += 4) {
      if (*p) {
        ptidxs[nptidxs++] = p;
      }
    }
    if (c > (ptidxs.size() - nptidxs) || i == r - 1) {
      keypoints.reserve(keypoints.size() + nptidxs);
      for (int idx = 0; idx < nptidxs; ++idx) {
        // Get the integer pixel location from the byte position.
        const uint8_t *pt = ptidxs[idx];
        int off = pt - s.pixels();
        float y = static_cast<float>(off / s.rowBytes());
        float x = static_cast<float>(((off % s.rowBytes()) - 2) >> 2);

        // Decode the subpixel offsets from the alpha channel.
        constexpr float offsetScale = 1.0 / 14.0;
        float xoffset = offsetScale * static_cast<float>(pt[1] % 16) - 0.5f;
        float yoffset = offsetScale * static_cast<float>(pt[1] >> 4) - 0.5f;

        // x and y are scaled later.
        keypoints.emplace_back(
          x + xoffset,
          y + yoffset,
          static_cast<float>(Gr8Gl::PATCH_SIZE),
          scale,
          -1 /* angle */,
          -1, /* gravityAngle */
          *pt /* response */,
          roi);
      }
      nptidxs = 0;
    }
    rowStart += s.rowBytes();
  }
}

static void bestKeypointsForCrop(
  ConstRGBA8888PlanePixels s,
  uint8_t scale,
  int8_t roi,
  int edgeThreshold,
  int nKeypoints,
  Vector<ImagePointLocation> &keypoints,
  Vector<int> &countsBeforeBest) {
  keypointsForCrop(s, scale, roi, edgeThreshold, keypoints);

  size_t idx = roi < 0 ? scale : roi;
  countsBeforeBest[idx] = keypoints.size();

  Gr8Gl::retainBest(keypoints, nKeypoints, s.cols(), s.rows(), edgeThreshold, true);
}

static void bestKeypointsForCropJs(
  ConstRGBA8888PlanePixels s,
  uint8_t scale,
  int8_t roi,
  int edgeThreshold,
  int nKeypoints,
  Vector<ImagePointLocation> &keypoints,
  Vector<int> &countsBeforeBest) {
  keypointsForCropJs(s, scale, roi, edgeThreshold, keypoints);

  size_t idx = roi < 0 ? scale : roi;
  countsBeforeBest[idx] = keypoints.size();

  // Keep more points than necessary as FAST does not give amazing corners
  Gr8Gl::retainBest(keypoints, nKeypoints, s.cols(), s.rows(), edgeThreshold, true);
}

static void keypointsByLevelMultiThread(
  const Gr8Pyramid &pyramid,
  const Vector<int> &nfeaturesPerLevel,
  int edgeThreshold,
  TaskQueue *taskQueue,
  ThreadPool *threadPool,
  Vector<Vector<ImagePointLocation>> *keypointLevels) {
  ScopeTimer t("keypoints-by-level-multithread");

  Vector<int> counts(pyramid.levels.size());

  auto detectFeaturesForLevel = [&](int level) {
    bestKeypointsForCrop(
      pyramid.level(level),
      static_cast<uint8_t>(level),
      -1 /* roi */,
      edgeThreshold,
      nfeaturesPerLevel[level],
      keypointLevels->at(level),
      counts);
  };

  // Add the level tasks to the thread queue in reverse, since level 0 is fastest.
  for (int level = 0; level < pyramid.levels.size(); ++level) {
    taskQueue->addTask(std::bind(std::cref(detectFeaturesForLevel), level));
  }

  {
    // Detect features in parallel.
    ScopeTimer tLevel("n-levels-features");
    taskQueue->executeWithThreadPool(threadPool);
  }

  for (int level = 0; level < pyramid.levels.size(); ++level) {
    t.addCounter(format("extract-keypoints-level-%02d-num-keypoints", level), counts[level]);
  }
}

static void keypointsByLevel(
  const Gr8Pyramid &pyramid,
  const Vector<int> &nfeaturesPerLevel,
  int edgeThreshold,
  Vector<Vector<ImagePointLocation>> *keypointLevels) {
  // Create the keypoint levels.
  Vector<int> counts(pyramid.levels.size());
  for (int level = 0; level < pyramid.levels.size(); ++level) {
    bestKeypointsForCropJs(
      pyramid.level(level),
      static_cast<uint8_t>(level),
      -1 /* roi */,
      edgeThreshold,
      nfeaturesPerLevel[level],
      keypointLevels->at(level),
      counts);
  }
  for (int level = 0; level < pyramid.levels.size(); ++level) {
    ScopeTimer::current()->addCounter(
      format("extract-keypoints-level-%02d-num-keypoints", level), counts[level]);
  }
}

static Vector<int> featureTargetsForLevels(
  const Gr8Pyramid &pyramid, double scaleFactor, int nfeatures) {
  int nlevels = pyramid.levels.size();
  Vector<int> nfeaturesPerLevel(nlevels);

  // fill the extractors and descriptors for the corresponding scales
  // TODO(nb): scaleFactor is implicit in the pyramid; we shouldn't be computing it here.
  float factor = (float)(1.0 / scaleFactor);
  float ndesiredFeaturesPerScale =
    nfeatures * (1 - factor) / (1 - (float)std::pow((double)factor, (double)nlevels));

  int sumFeatures = 0;
  for (int level = 0; level < nlevels - 1; level++) {
    nfeaturesPerLevel[level] = std::round(ndesiredFeaturesPerScale);
    sumFeatures += nfeaturesPerLevel[level];
    ndesiredFeaturesPerScale *= factor;
  }
  nfeaturesPerLevel[nlevels - 1] = std::max(nfeatures - sumFeatures, 0);
  return nfeaturesPerLevel;
}

static void flattenKeypointScales(
  const Vector<Vector<ImagePointLocation>> &keypointLevels,
  Vector<ImagePointLocation> *allKeypoints,
  Vector<int> *counters) {
  counters->resize(keypointLevels.size());
  for (int level = 0; level < keypointLevels.size(); ++level) {
    auto &keypoints = keypointLevels[level];
    (*counters)[level] = keypoints.size();
    std::copy(keypoints.begin(), keypoints.end(), std::back_inserter(*allKeypoints));
  }
}

/** Compute the Gr8 keypoints on an image
 * @param image_pyramid the image pyramid to compute the features and descriptors on
 * @param keypoints the resulting keypoints, clustered per level
 */
static Vector<ImagePointLocation> computeKeyPoints(
  const Gr8Pyramid &pyramid,
  int nfeatures,
  double scaleFactor,
  int edgeThreshold,
  TaskQueue *taskQueue,
  ThreadPool *threadPool,
  Vector<Vector<ImagePointLocation>> *keypointLevels) {
  ScopeTimer t("compute-key-points");
  Vector<int> nfeaturesPerLevel = featureTargetsForLevels(pyramid, scaleFactor, nfeatures);

  Vector<ImagePointLocation> allKeypoints;
  allKeypoints.reserve(nfeaturesPerLevel[0] * nfeaturesPerLevel.size());

  keypointLevels->resize(pyramid.levels.size());
  for (auto &l : *keypointLevels) {
    l.clear();
  }

  // Create the keypoint levels.
  if (USE_MULTITHREADED_FEATURES) {
    keypointsByLevelMultiThread(
      pyramid, nfeaturesPerLevel, edgeThreshold, taskQueue, threadPool, keypointLevels);
  } else {
    keypointsByLevel(pyramid, nfeaturesPerLevel, edgeThreshold, keypointLevels);
  }

  Vector<int> counters;
  flattenKeypointScales(*keypointLevels, &allKeypoints, &counters);

  return allKeypoints;
}

/** Compute the Gr8 keypoints on an image
 * @param image_pyramid the image pyramid to compute the features and descriptors on
 * @param keypoints the resulting keypoints, clustered per level
 */
static Vector<ImagePointLocation> computeKeyPointsRoi(
  const Gr8Pyramid &pyramid, int roi, int edgeThreshold) {
  ScopeTimer t("compute-key-points");

  Vector<ImagePointLocation> pts;
  Vector<int> counts(pyramid.rois.size());

  auto roiim = pyramid.roi(roi);
  auto proi = pyramid.rois[roi].roi;
  // TODO(nb): Should nKeypoints be a parameter?
  int nKeypoints = proi.source == ImageRoi::Source::HIRES_SCAN ? 125 : 1000;
  int scale = 2;  // 2 is the closest level to the roi size.

  bestKeypointsForCropJs(roiim, scale, roi, edgeThreshold, nKeypoints, pts, counts);

  return pts;
}

Vector<ImagePointLocation> Gr8Gl::computeKeyPoints(const Gr8Pyramid &pyramid, int nFeatures) {
  return c8::computeKeyPoints(
    pyramid,
    nFeatures,
    scaleFactor_,  // TODO(nb): remove scaleFactor.
    edgeThreshold_,
    &taskQueue_,
    threadPool_.get(),
    &keypointLevels_);
}

Vector<ImagePointLocation> Gr8Gl::computeKeyPointsRoi(const Gr8Pyramid &pyramid, int roi) {
  return c8::computeKeyPointsRoi(pyramid, roi, edgeThreshold_);
}

Vector<size_t> Gr8Gl::computeOrbKeypoints(
  const Gr8Pyramid &pyramid, const DetectionConfig &config, Vector<ImagePointLocation> *pts) const {
  ScopeTimer t("compute-orb-keypoints");

  Vector<size_t> orbIndices;
  orbIndices.reserve(pts->size());
  if (config.allOrb) {
    for (size_t i = 0; i < pts->size(); i++) {
      orbIndices.push_back(i);
    }
  } else {
    for (size_t i = 0; i < pts->size(); i++) {
      if ((pts->at(i).gravityPortion & config.gravityPortions[DescriptorType::ORB]) == 0) {
        continue;
      }
      orbIndices.push_back(i);
    }
  }

  c8::computeCenterOfMassAngles(pyramid, orbIndices, *pts, circularMask_);
  return orbIndices;
}

Vector<size_t> Gr8Gl::computeGorbKeypoints(
  const Gr8Pyramid &pyramid,
  const DetectionConfig &config,
  const FrameWithPoints &frame,
  const Vector<HPoint3> &raysInCam,
  const Vector<HPoint3> &raysInWorld,
  Vector<ImagePointLocation> *pts,
  float gravityAngleOffset) const {
  ScopeTimer t("compute-gorb-keypoints");

  Vector<size_t> gorbIndices;
  gorbIndices.reserve(pts->size());
  if (config.allGorb) {
    for (size_t i = 0; i < pts->size(); i++) {
      gorbIndices.push_back(i);
    }
  } else {
    for (size_t i = 0; i < pts->size(); i++) {
      if ((pts->at(i).gravityPortion & config.gravityPortions[DescriptorType::GORB]) == 0) {
        continue;
      }
      gorbIndices.push_back(i);
    }
  }

  computeGravityAngles(
    pyramid, gorbIndices, frame, raysInCam, raysInWorld, pts, gravityAngleOffset);
  return gorbIndices;
}

ImagePoints Gr8Gl::toPointsWithEmptyDescriptors(const Vector<ImagePointLocation> &pts) {
  // ImagePoint32 pointsWithDescriptors is a custom data structure with a Vector which automatically
  // reserves memory for the given number of keypoints.
  ImagePoints pointsWithDescriptors(pts.size());
  for (int i = 0; i < pts.size(); i++) {
    pointsWithDescriptors.push_back(pts[i]);
  }
  return pointsWithDescriptors;
}

void Gr8Gl::computeGr8Descriptors(
  const Gr8Pyramid &pyramid,
  const Vector<size_t> &kptIndices,
  bool useGravityAngle,
  ImagePoints *pointsWithDescriptors) {
  ScopeTimer t("do-descriptors");
  initFeatureLut(pyramid.data.rowBytes());
  c8::computeGr8Descriptors(
    pyramid,
    kptIndices,
    useGravityAngle,
    pointsWithDescriptors,
    wtaK_,
    &taskQueue_,
    threadPool_.get());
}

void Gr8Gl::computeRayVectors(
  const Gr8Pyramid &pyramid,
  const FrameWithPoints &frame,
  const Vector<ImagePointLocation> &ptImageLocations,
  Vector<HPoint3> *raysInCam,
  Vector<HPoint3> *raysInWorld) {
  raysInWorld->reserve(ptImageLocations.size());
  raysInCam->reserve(ptImageLocations.size());
  auto camToWorld = frame.xrDevicePose().toRotationMat();
  auto rotatedCamToWorld = camToWorld * HMatrixGen::zRadians(-M_PI / 2);
  auto layerScale = pyramid.mapLevelsToBase();

  for (const auto &ptImgLocation : ptImageLocations) {
    float baseX =
      ptImgLocation.pt.x * layerScale[ptImgLocation.scale][0] + layerScale[ptImgLocation.scale][1];
    float baseY =
      ptImgLocation.pt.y * layerScale[ptImgLocation.scale][2] + layerScale[ptImgLocation.scale][3];

    auto ptRayInCam = frame.undistort(HPoint2(baseX, baseY)).extrude();
    raysInCam->emplace_back(ptRayInCam);
    if (pyramid.levels[ptImgLocation.scale].rotated) {
      raysInWorld->emplace_back(rotatedCamToWorld * ptRayInCam);
    } else {
      raysInWorld->emplace_back(camToWorld * ptRayInCam);
    }
  }
}

void Gr8Gl::computeGravityPortions(
  const Vector<HPoint3> &raysInWorld, Vector<ImagePointLocation> *pts) {
  for (int i = 0; i < raysInWorld.size(); i++) {
    const auto &ray = raysInWorld[i];
    float angleWithUpDeg =
      fastAcos(ray.y() / std::sqrt(ray.x() * ray.x() + ray.y() * ray.y() + ray.z() * ray.z()));
    pts->at(i).gravityPortion = angleWithUpToPortion(angleWithUpDeg);
  }
}

void Gr8Gl::scaleToBaseImage(const Gr8Pyramid &pyramid, ImagePoints *pointsWithDescriptors) {
  auto layerScale = pyramid.mapLevelsToBase();
  auto b = pyramid.levels[0];

  Vector<HMatrix> roiInvs;
  Vector<float> roiScales;
  Vector<MapGeometryData> curvyRoiMapToGeometryData;
  Vector<CurvyImageGeometry> curvyRoiGeometry;
  for (const auto &roi : pyramid.rois) {
    auto l = roi.layout;

    // Divide by roi width/height, then subtract .5, then multiply by 2.
    auto preMul = HMatrixGen::scale(2.0f, 2.0f, 1.0f) * HMatrixGen::translation(-0.5f, -0.5f, 0.0f)
      * HMatrixGen::scale(1.0f / (l.w - 1), 1.0f / (l.h - 1), 1.0);

    // Divide by 2, add 0.5, and then multiply by base width / height.
    auto postMul = HMatrixGen::scale(b.w - 1.0f, b.h - 1.0f, 1.0f)
      * HMatrixGen::translation(0.5f, 0.5f, 0.0f) * HMatrixGen::scale(0.5f, 0.5f, 1.0f);

    roiInvs.push_back(postMul * roi.roi.warp.inv() * preMul);
    // If level is rotated, compare h to w instead of h to h
    if (l.rotated != b.rotated) {
      roiScales.push_back(
        0.5f
        * (static_cast<float>(b.h) / static_cast<float>(l.w) + static_cast<float>(b.w) / static_cast<float>(l.h)));
    } else {
      roiScales.push_back(
        0.5f
        * (static_cast<float>(b.h) / static_cast<float>(l.h) + static_cast<float>(b.w) / static_cast<float>(l.w)));
    }

    if (roi.roi.source == ImageRoi::Source::CURVY_IMAGE_TARGET) {
      CurvyImageGeometry liftedGeom = roi.roi.geom;
      // Adjust dimensions for landscape image targets (including isRotated=true).
      float w = roi.roi.geom.srcCols > roi.roi.geom.srcRows ? l.h : l.w;
      float h = roi.roi.geom.srcCols > roi.roi.geom.srcRows ? l.w : l.h;
      if (roi.roi.geom.srcCols > roi.roi.geom.srcRows) {
        liftedGeom.srcRows = roi.roi.geom.srcCols;
        liftedGeom.srcCols = roi.roi.geom.srcRows;
      }

      // For non 3x4 image targets, the lifted image target won't take up the full viewport of the
      // quad. Compute the size in pixels that the lifted image takes up in the quad.
      float aspectRatio = 1.f * l.w / l.h;
      float liftedCols = (static_cast<float>(liftedGeom.srcCols) / liftedGeom.srcRows > .75f)
        ? w
        : w * liftedGeom.srcCols / (aspectRatio * liftedGeom.srcRows);
      float liftedRows = (static_cast<float>(liftedGeom.srcCols) / liftedGeom.srcRows > .75f)
        ? h * liftedGeom.srcRows / (liftedGeom.srcCols / aspectRatio)
        : h;

      liftedGeom.srcRows = liftedRows;
      liftedGeom.srcCols = liftedCols;
      curvyRoiMapToGeometryData.push_back(prepareMapToGeometryData(liftedGeom));
      curvyRoiGeometry.push_back(liftedGeom);
    } else {
      curvyRoiMapToGeometryData.push_back({});
      curvyRoiGeometry.push_back({});
    }
  }

  // Scale points at each level to be the full size of the pyramid, and rotate if needed.
  auto levels = pyramid.levels;
  for (auto &p : *pointsWithDescriptors) {
    auto pt = p.location();
    auto isRoi = pt.roi >= 0;
    auto l = isRoi ? pyramid.rois[pt.roi].layout : pyramid.levels[pt.scale];
    if (isRoi) {
      if (pyramid.rois[pt.roi].roi.source == ImageRoi::Source::CURVY_IMAGE_TARGET) {
        // Transform feature points from lifted target space to search pixel space.
        auto roi = pyramid.rois[pt.roi].roi;

        // Check if we need to rotate point 90 degrees counterclockwise, as a rotated image will be
        // rotated in the ROI.
        float x = (roi.geom.srcCols > roi.geom.srcRows) ? pt.pt.y : pt.pt.x;
        float y = (roi.geom.srcCols > roi.geom.srcRows) ? l.w - pt.pt.x : pt.pt.y;
        HPoint3 point = mapToGeometryPoint(
          curvyRoiMapToGeometryData[pt.roi], curvyRoiGeometry[pt.roi], HPoint2(x, y));
        // globalPose.inv() gets us into world space, then intrinsics get us into search pixel
        // space.
        HPoint2 ptInSearchPixel = ((roi.intrinsics * roi.globalPose.inv()) * point).flatten();
        pt.pt.x = ptInSearchPixel.x();
        pt.pt.y = ptInSearchPixel.y();
      } else {
        auto p = (roiInvs[pt.roi] * HPoint2{pt.pt.x, pt.pt.y}.extrude()).truncate();
        pt.pt.x = p.x();
        pt.pt.y = p.y();
      }
    } else {
      pt.pt.x = pt.pt.x * layerScale[pt.scale][0] + layerScale[pt.scale][1];
      pt.pt.y = pt.pt.y * layerScale[pt.scale][2] + layerScale[pt.scale][3];
    }
    pt.size *= isRoi ? roiScales[pt.roi] : layerScale[pt.scale][4];
    if (l.rotated) {
      auto tempx = pt.pt.x;
      pt.pt.x = b.w - 1 - pt.pt.y;
      pt.pt.y = tempx;
      pt.angle += 90.0f;
      pt.gravityAngle += 90.0f;
    }
    p.setLocation(pt);
  }
}

ImagePoints Gr8Gl::detectAndCompute(
  const Gr8Pyramid &pyramid,
  const FrameWithPoints &frame,
  const DetectionConfig &config,
  float gravityAngleOffset) {
  ScopeTimer t("gr8gl");

  auto pts = computeKeyPoints(pyramid, config.nKeypoints);

  Vector<HPoint3> raysInWorld;
  Vector<HPoint3> raysInCam;
  // Prework for gravity angles and determining which keypoints get GORB
  computeRayVectors(pyramid, frame, pts, &raysInCam, &raysInWorld);

  // If the config has gravityPortion-based selection for descriptors, compute the gravity portions
  if (
    config.gravityPortions[DescriptorType::ORB] != GravityPortion::NONE
    || config.gravityPortions[DescriptorType::GORB] != GravityPortion::NONE
    || config.gravityPortions[DescriptorType::LEARNED] != GravityPortion::NONE) {
    computeGravityPortions(raysInWorld, &pts);
  }

  // Compute which indices will get ORB and GORB descriptors, and compute relevant keypoint angles
  const auto orbIndices = computeOrbKeypoints(pyramid, config, &pts);
  const auto gorbIndices =
    computeGorbKeypoints(pyramid, config, frame, raysInCam, raysInWorld, &pts, gravityAngleOffset);

  // Compute the right descriptors for the keypoints
  auto ptsWithDescriptors = toPointsWithEmptyDescriptors(pts);
  computeGr8Descriptors(pyramid, orbIndices, false, &ptsWithDescriptors);
  computeGr8Descriptors(pyramid, gorbIndices, true, &ptsWithDescriptors);
  scaleToBaseImage(pyramid, &ptsWithDescriptors);

  return ptsWithDescriptors;
}

ImagePoints Gr8Gl::detectAndCompute(const Gr8Pyramid &pyramid, int numFeatures) {
  ScopeTimer t("gr8gl");

  if (numFeatures < 0) {
    numFeatures = FEATURES_PER_FRAME_GL;
  }

  auto pts = computeKeyPoints(pyramid, numFeatures);
  auto orbIndices = computeOrbKeypoints(pyramid, DETECTION_CONFIG_ALL_ORB, &pts);
  auto ptsWithDescriptors = toPointsWithEmptyDescriptors(pts);
  computeGr8Descriptors(pyramid, orbIndices, false, &ptsWithDescriptors);
  scaleToBaseImage(pyramid, &ptsWithDescriptors);

  return ptsWithDescriptors;
}

Vector<ImagePoints> Gr8Gl::detectAndComputeRois(
  const Gr8Pyramid &pyramid,
  const FrameWithPoints &frame,
  const DetectionConfig &config,
  float gravityAngleOffset) {
  ScopeTimer t("gr8gl-rois");
  Vector<ImagePoints> ptsWithDescriptors;
  ptsWithDescriptors.reserve(pyramid.rois.size());

  for (int i = 0; i < pyramid.rois.size(); ++i) {
    auto pts = computeKeyPointsRoi(pyramid, i);

    Vector<HPoint3> raysInWorld;
    Vector<HPoint3> raysInCam;

    // Prework for gravity angles and determining which keypoints get GORB
    computeRayVectors(pyramid, frame, pts, &raysInCam, &raysInWorld);

    // If the config has gravityPortion-based selection for descriptors, compute the gravity
    // portions
    if (
      config.gravityPortions[DescriptorType::ORB] != GravityPortion::NONE
      || config.gravityPortions[DescriptorType::GORB] != GravityPortion::NONE
      || config.gravityPortions[DescriptorType::LEARNED] != GravityPortion::NONE) {
      computeGravityPortions(raysInWorld, &pts);
    }

    // Compute which indices will get ORB and GORB descriptors, and compute relevant keypoint angles
    const auto orbIndices = computeOrbKeypoints(pyramid, config, &pts);
    const auto gorbIndices = computeGorbKeypoints(
      pyramid, config, frame, raysInCam, raysInWorld, &pts, gravityAngleOffset);

    // Compute the right descriptors for the keypoints
    ptsWithDescriptors.push_back(toPointsWithEmptyDescriptors(pts));

    computeGr8Descriptors(pyramid, orbIndices, false, &ptsWithDescriptors.back());
    computeGr8Descriptors(pyramid, gorbIndices, true, &ptsWithDescriptors.back());
    scaleToBaseImage(pyramid, &ptsWithDescriptors.back());
  }
  return ptsWithDescriptors;
}

/** Compute the Gr8 features and descriptors on an image
 * @param img the image to compute the features and descriptors on
 * @param keypoints the resulting keypoints
 * @param descriptors the resulting descriptors
 */
Vector<ImagePoints> Gr8Gl::detectAndComputeRois(const Gr8Pyramid &pyramid) {
  ScopeTimer t("gr8gl-rois");
  Vector<ImagePoints> ptsWithDescriptors;
  ptsWithDescriptors.reserve(pyramid.rois.size());

  for (int i = 0; i < pyramid.rois.size(); ++i) {
    auto pts = computeKeyPointsRoi(pyramid, i);
    auto orbIndices = computeOrbKeypoints(pyramid, DETECTION_CONFIG_ALL_ORB, &pts);
    ptsWithDescriptors.push_back(toPointsWithEmptyDescriptors(pts));
    computeGr8Descriptors(pyramid, orbIndices, false, &ptsWithDescriptors.back());
    scaleToBaseImage(pyramid, &ptsWithDescriptors.back());
  }

  return ptsWithDescriptors;
}

}  // namespace c8
