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
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//   * Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above
//     copyright notice, this list of conditions and the following
//     disclaimer in the documentation and/or other materials provided
//     with the distribution.
//   * Neither the name of the Willow Garage nor the names of its
//     contributors may be used to endorse or promote products derived
//     from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
//  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
//  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
//  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
//  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
//  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
//  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
//  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.

#include "bzl/inliner/rules2.h"

cc_library {
  visibility = {
    "//reality/engine/features:__subpackages__",
  };
  hdrs = {
    "gr8.h",
  };
  deps = {
    ":gr8cpu-interface",
    ":image-point",
    ":opencv-image-descriptor",
    "//c8:c8-log",
    "//c8:task-queue",
    "//c8:thread-pool",
    "//c8/pixels:pixels",
    "//third_party/cvlite/core",
    "//third_party/cvlite/features2d:fast",
    "//third_party/cvlite/features2d:keypoint",
    "//third_party/cvlite/imgproc:imgwarp",
    "//third_party/cvlite/imgproc:smooth",
    "//third_party/cvlite/imgproc:thresh",
  };
}
cc_end(0x2e47b550);

#include <array>
#include <future>
#include <iterator>
#include <map>
#include <numeric>
#include <queue>
#include <vector>

#include "c8/c8-log.h"
#include "reality/engine/features/gr8.h"
#include "reality/engine/features/opencv-image-descriptor.h"
#include "third_party/cvlite/core/core.hpp"
#include "third_party/cvlite/features2d/fast.h"
#include "third_party/cvlite/features2d/keypoint.h"
#include "third_party/cvlite/imgproc/imgwarp.h"
#include "third_party/cvlite/imgproc/smooth.h"
#include "third_party/cvlite/imgproc/thresh.h"

#ifndef CV_IMPL_ADD
#define CV_IMPL_ADD(x)
#endif

#define GET_VALUE(idx) (xy = lut[idx][quantAngle], *(center + xy.second * step + xy.first))

////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace c8cv;

namespace {
struct KeypointResponseMore {
  inline bool operator()(const c8cv::KeyPoint &kp1, const c8cv::KeyPoint &kp2) const {
    return kp1.response > kp2.response;
  }
};
struct KeypointResponsePairMore {
  inline bool operator()(
    const std::pair<int, c8cv::KeyPoint> &kp1, const std::pair<int, c8cv::KeyPoint> &kp2) const {
    return kp1.first != kp2.first ? kp1.first < kp2.first
                                  : kp1.second.response > kp2.second.response;
  }
};
}  // namespace

namespace c8 {

namespace {

// Forward declaration of a Look-up table from binary pattern to indices.
extern const std::pair<int8_t, int8_t> FEATURE_LUT[512][36];

// TODO(mc): Switch to std::hardware_destructive_interference_size when we get to C++17.
constexpr std::size_t destructiveInterferenceSize = 64;

template <size_t N>
struct cacheSafeUnsignedCharArray {
  alignas(destructiveInterferenceSize) unsigned char value[N];
};

}  // namespace

constexpr int bin(int x, int y, int numBinCols) { return numBinCols * (y >> 5) + (x >> 5); }

void Gr8::retainBest(
  std::vector<c8cv::KeyPoint> &keypoints, int nPoints, int cols, bool quantizeTo256) {
  // If we already have enough points, then we're done.
  if (keypoints.size() <= nPoints) {
    return;
  }

  // If we have no budget for points, clear all of them.
  if (nPoints == 0) {
    keypoints.clear();
    return;
  }
  int bincols = std::ceil(cols / 32.0f);
  std::array<int, 12000> binCounts{};

  std::vector<std::pair<int, c8cv::KeyPoint>> collisionToScore;
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

    std::vector<c8cv::KeyPoint> sortedKeypoints;
    sortedKeypoints.resize(keypoints.size());
    for (int i = 0; i < keypoints.size(); ++i) {
      const uint8_t quantizedResponse = 255 - static_cast<uint8_t>(keypoints[i].response);
      sortedKeypoints[count[quantizedResponse]] = keypoints[i];
      count[quantizedResponse] += 1;
    }

    swap(sortedKeypoints, keypoints);
  } else {
    std::sort(keypoints.begin(), keypoints.end(), KeypointResponseMore());
  }
  for (const auto &keypoint : keypoints) {
    const int bini = bin(keypoint.pt.x, keypoint.pt.y, bincols);
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

const float HARRIS_K = 0.04f;

/**
 * Function that computes the Harris responses in a
 * blockSize x blockSize patch at given points in the image
 */
static void HarrisResponses(
  const c8cv::Mat &img,
  const std::vector<c8cv::Rect> &layerinfo,
  std::vector<c8cv::KeyPoint> &pts,
  int blockSize,
  float harris_k) {
  C8CV_Assert(img.type() == CV_8UC1 && blockSize * blockSize <= 2048);

  size_t ptidx, ptsize = pts.size();

  const uchar *ptr00 = img.ptr<uchar>();
  int step = (int)(img.step / img.elemSize1());
  int r = blockSize / 2;

  float scale = 1.f / ((1 << 2) * blockSize * 255.f);
  float scale_sq_sq = scale * scale * scale * scale;

  c8cv::AutoBuffer<int> ofsbuf(blockSize * blockSize);
  int *ofs = ofsbuf;
  for (int i = 0; i < blockSize; i++)
    for (int j = 0; j < blockSize; j++) ofs[i * blockSize + j] = (int)(i * step + j);

  for (ptidx = 0; ptidx < ptsize; ptidx++) {
    int x0 = cvRound(pts[ptidx].pt.x);
    int y0 = cvRound(pts[ptidx].pt.y);
    int z = pts[ptidx].octave;

    const uchar *ptr0 = ptr00 + (y0 - r + layerinfo[z].y) * step + x0 - r + layerinfo[z].x;
    int a = 0, b = 0, c = 0;

    for (int k = 0; k < blockSize * blockSize; k++) {
      const uchar *ptr = ptr0 + ofs[k];
      int Ix = (ptr[1] - ptr[-1]) * 2 + (ptr[-step + 1] - ptr[-step - 1])
        + (ptr[step + 1] - ptr[step - 1]);
      int Iy = (ptr[step] - ptr[-step]) * 2 + (ptr[step - 1] - ptr[-step - 1])
        + (ptr[step + 1] - ptr[-step + 1]);
      a += Ix * Ix;
      b += Iy * Iy;
      c += Ix * Iy;
    }
    pts[ptidx].response =
      ((float)a * b - (float)c * c - harris_k * ((float)a + b) * ((float)a + b)) * scale_sq_sq;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

static void ICAngles(
  const c8cv::Mat &img,
  const std::vector<c8cv::Rect> &layerinfo,
  std::vector<c8cv::KeyPoint> &pts,
  const std::vector<int> &u_max) {
  int step = (int)img.step1();
  size_t ptidx, ptsize = pts.size();

  // Create an accumulation buffer to remove extra multiplies.
  constexpr int HALF_K = Gr8::PATCH_SIZE / 2;
  std::array<int, HALF_K * 2 + 1> accum10;

  for (ptidx = 0; ptidx < ptsize; ptidx++) {
    const c8cv::Rect &layer = layerinfo[pts[ptidx].octave];
    const uchar *center =
      &img.at<uchar>(cvRound(pts[ptidx].pt.y) + layer.y, cvRound(pts[ptidx].pt.x) + layer.x);

    int m_01 = 0, m_10 = 0;

    // Zero-out the accumulation buffer.
    memset(accum10.data(), 0, accum10.size() * sizeof(decltype(accum10)::value_type));

    // Go line by line in the circular patch
    int widthStep = 0;
    for (int v = 1; v <= HALF_K; ++v) {
      widthStep += step;
      // Proceed over the two lines
      int v_sum = 0;
      int d = u_max[v];
      for (int u = -d; u <= d; ++u) {
        int val_plus = center[u + widthStep];
        int val_minus = center[u - widthStep];
        v_sum += val_plus - val_minus;
        accum10[u + HALF_K] += val_plus + val_minus;
      }
      m_01 += v * v_sum;
    }

    // Treat the center line differently, v=0 and accumulate the rest.
    for (int u = -HALF_K; u <= HALF_K; ++u) {
      m_10 += u * (center[u] + accum10[u + HALF_K]);
    }

    pts[ptidx].angle = c8cv::fastAtan2((float)m_01, (float)m_10);
  }
}  // namespace c8

////////////////////////////////////////////////////////////////////////////////////////////////////

void computeGr8Descriptors(
  const c8cv::Mat &imagePyramid,
  const std::vector<c8cv::Rect> &layerInfo,
  const std::vector<float> &layerScale,
  std::vector<c8cv::KeyPoint> &keypoints,
  c8cv::Mat &descriptors,
  const std::vector<c8cv::Point> &_pattern,
  int wta_k,
  TaskQueue *taskQueue,
  ThreadPool *threadPool) {
  int step = (int)imagePyramid.step;
  int nkeypoints = (int)keypoints.size();

  constexpr int DSIZE = Gr8::descriptorSize();

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

  auto computeDescriptor = [&](int j) {
    const c8cv::KeyPoint &kpt = keypoints[j];
    const c8cv::Rect &layer = layerInfo[kpt.octave];
    float scale = 1.f / layerScale[kpt.octave];
    int quantAngle = ((static_cast<int>(kpt.angle + 5.0f) / 10)) % 36;

    const uchar *center = &imagePyramid.at<uchar>(
      cvRound(kpt.pt.y * scale) + layer.y, cvRound(kpt.pt.x * scale) + layer.x);
    std::pair<int8_t, int8_t> xy;
    const auto *lut = &FEATURE_LUT[0];
    uchar *desc = threadDesc[j].value;

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
        desc[i] = (uchar)(output[0] | output[1] | output[2] | output[3] | output[4] | output[5]
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

        desc[i] = (uchar)val;
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

        desc[i] = (uchar)val;
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

        desc[i] = (uchar)val;
      }
    } else {
      C8CV_Error(c8cv::Error::StsBadSize, "Wrong wta_k. It can be only 2, 3 or 4.");
    }
  };

  auto computeDescriptorRange = [&computeDescriptor](int start, int end) {
    for (int i = start; i < end; ++i) {
      computeDescriptor(i);
    }
  };

  // Split work up into blocks.
  const int blockSize = 30;
  for (int j = 0; j < nkeypoints; j += blockSize) {
    taskQueue->addTask(
      std::bind(std::cref(computeDescriptorRange), j, std::min(nkeypoints, j + blockSize)));
  }

  taskQueue->executeWithThreadPool(threadPool);

  for (int j = 0; j < nkeypoints; j++) {
    uchar *src = threadDesc[j].value;
    uchar *dest = descriptors.ptr<uchar>(j);
    std::copy(src, src + DSIZE, dest);
  }
}

static void initializeGr8Pattern(
  const c8cv::Point *pattern0,
  std::vector<c8cv::Point> &pattern,
  int ntuples,
  int tupleSize,
  int poolSize) {
  c8cv::RNG rng(0x12345678);
  int i, k, k1;
  pattern.resize(ntuples * tupleSize);

  for (i = 0; i < ntuples; i++) {
    for (k = 0; k < tupleSize; k++) {
      for (;;) {
        int idx = rng.uniform(0, poolSize);
        c8cv::Point pt = pattern0[idx];
        for (k1 = 0; k1 < k; k1++)
          if (pattern[tupleSize * i + k1] == pt)
            break;
        if (k1 == k) {
          pattern[tupleSize * i + k] = pt;
          break;
        }
      }
    }
  }
}

static int bit_pattern_31_[256 * 4] = {
  8,   -3,  9,   5,   4,   2,   7,   -12, -11, 9,   -8,  2,   7,   -12, 12,  -13, 2,   -13, 2,
  12,  1,   -7,  1,   6,   -2,  -10, -2,  -4,  -13, -13, -11, -8,  -13, -3,  -12, -9,  10,  4,
  11,  9,   -13, -8,  -8,  -9,  -11, 7,   -9,  12,  7,   7,   12,  6,   -4,  -5,  -3,  0,   -13,
  2,   -12, -3,  -9,  0,   -7,  5,   12,  -6,  12,  -1,  -3,  6,   -2,  12,  -6,  -13, -4,  -8,
  11,  -13, 12,  -8,  4,   7,   5,   1,   5,   -3,  10,  -3,  3,   -7,  6,   12,  -8,  -7,  -6,
  -2,  -2,  11,  -1,  -10, -13, 12,  -8,  10,  -7,  3,   -5,  -3,  -4,  2,   -3,  7,   -10, -12,
  -6,  11,  5,   -12, 6,   -7,  5,   -6,  7,   -1,  1,   0,   4,   -5,  9,   11,  11,  -13, 4,
  7,   4,   12,  2,   -1,  4,   4,   -4,  -12, -2,  7,   -8,  -5,  -7,  -10, 4,   11,  9,   12,
  0,   -8,  1,   -13, -13, -2,  -8,  2,   -3,  -2,  -2,  3,   -6,  9,   -4,  -9,  8,   12,  10,
  7,   0,   9,   1,   3,   7,   -5,  11,  -10, -13, -6,  -11, 0,   10,  7,   12,  1,   -6,  -3,
  -6,  12,  10,  -9,  12,  -4,  -13, 8,   -8,  -12, -13, 0,   -8,  -4,  3,   3,   7,   8,   5,
  7,   10,  -7,  -1,  7,   1,   -12, 3,   -10, 5,   6,   2,   -4,  3,   -10, -13, 0,   -13, 5,
  -13, -7,  -12, 12,  -13, 3,   -11, 8,   -7,  12,  -4,  7,   6,   -10, 12,  8,   -9,  -1,  -7,
  -6,  -2,  -5,  0,   12,  -12, 5,   -7,  5,   3,   -10, 8,   -13, -7,  -7,  -4,  5,   -3,  -2,
  -1,  -7,  2,   9,   5,   -11, -11, -13, -5,  -13, -1,  6,   0,   -1,  5,   -3,  5,   2,   -4,
  -13, -4,  12,  -9,  -6,  -9,  6,   -12, -10, -8,  -4,  10,  2,   12,  -3,  7,   12,  12,  12,
  -7,  -13, -6,  5,   -4,  9,   -3,  4,   7,   -1,  12,  2,   -7,  6,   -5,  1,   -13, 11,  -12,
  5,   -3,  7,   -2,  -6,  7,   -8,  12,  -7,  -13, -7,  -11, -12, 1,   -3,  12,  12,  2,   -6,
  3,   0,   -4,  3,   -2,  -13, -1,  -13, 1,   9,   7,   1,   8,   -6,  1,   -1,  3,   12,  9,
  1,   12,  6,   -1,  -9,  -1,  3,   -13, -13, -10, 5,   7,   7,   10,  12,  12,  -5,  12,  9,
  6,   3,   7,   11,  5,   -13, 6,   10,  2,   -12, 2,   3,   3,   8,   4,   -6,  2,   6,   12,
  -13, 9,   -12, 10,  3,   -8,  4,   -7,  9,   -11, 12,  -4,  -6,  1,   12,  2,   -8,  6,   -9,
  7,   -4,  2,   3,   3,   -2,  6,   3,   11,  0,   3,   -3,  8,   -8,  7,   8,   9,   3,   -11,
  -5,  -6,  -4,  -10, 11,  -5,  10,  -5,  -8,  -3,  12,  -10, 5,   -9,  0,   8,   -1,  12,  -6,
  4,   -6,  6,   -11, -10, 12,  -8,  7,   4,   -2,  6,   7,   -2,  0,   -2,  12,  -5,  -8,  -5,
  2,   7,   -6,  10,  12,  -9,  -13, -8,  -8,  -5,  -13, -5,  -2,  8,   -8,  9,   -13, -9,  -11,
  -9,  0,   1,   -8,  1,   -2,  7,   -4,  9,   1,   -2,  1,   -1,  -4,  11,  -6,  12,  -11, -12,
  -9,  -6,  4,   3,   7,   7,   12,  5,   5,   10,  8,   0,   -4,  2,   8,   -9,  12,  -5,  -13,
  0,   7,   2,   12,  -1,  2,   1,   7,   5,   11,  7,   -9,  3,   5,   6,   -8,  -13, -4,  -8,
  9,   -5,  9,   -3,  -3,  -4,  -7,  -3,  -12, 6,   5,   8,   0,   -7,  6,   -6,  12,  -13, 6,
  -5,  -2,  1,   -10, 3,   10,  4,   1,   8,   -4,  -2,  -2,  2,   -13, 2,   -12, 12,  12,  -2,
  -13, 0,   -6,  4,   1,   9,   3,   -6,  -10, -3,  -5,  -3,  -13, -1,  1,   7,   5,   12,  -11,
  4,   -2,  5,   -7,  -13, 9,   -9,  -5,  7,   1,   8,   6,   7,   -8,  7,   6,   -7,  -4,  -7,
  1,   -8,  11,  -7,  -8,  -13, 6,   -12, -8,  2,   4,   3,   9,   10,  -5,  12,  3,   -6,  -5,
  -6,  7,   8,   -3,  9,   -8,  2,   -12, 2,   8,   -11, -2,  -10, 3,   -12, -13, -7,  -9,  -11,
  0,   -10, -5,  5,   -3,  11,  8,   -2,  -13, -1,  12,  -1,  -8,  0,   9,   -13, -11, -12, -5,
  -10, -2,  -10, 11,  -3,  9,   -2,  -13, 2,   -3,  3,   2,   -9,  -13, -4,  0,   -4,  6,   -3,
  -10, -4,  12,  -2,  -7,  -6,  -11, -4,  9,   6,   -3,  6,   11,  -13, 11,  -5,  5,   11,  11,
  12,  6,   7,   -5,  12,  -2,  -1,  12,  0,   7,   -4,  -8,  -3,  -2,  -7,  1,   -6,  7,   -13,
  -12, -8,  -13, -7,  -2,  -6,  -8,  -8,  5,   -6,  -9,  -5,  -1,  -4,  5,   -13, 7,   -8,  10,
  1,   5,   5,   -13, 1,   0,   10,  -13, 9,   12,  10,  -1,  5,   -8,  10,  -9,  -1,  11,  1,
  -13, -9,  -3,  -6,  2,   -1,  -10, 1,   12,  -13, 1,   -8,  -10, 8,   -11, 10,  -6,  2,   -13,
  3,   -6,  7,   -13, 12,  -9,  -10, -10, -5,  -7,  -10, -8,  -8,  -13, 4,   -6,  8,   5,   3,
  12,  8,   -13, -4,  2,   -3,  -3,  5,   -13, 10,  -12, 4,   -13, 5,   -1,  -9,  9,   -4,  3,
  0,   3,   3,   -9,  -12, 1,   -6,  1,   3,   2,   4,   -8,  -10, -10, -10, 9,   8,   -13, 12,
  12,  -8,  -12, -6,  -5,  2,   2,   3,   7,   10,  6,   11,  -8,  6,   8,   8,   -12, -7,  10,
  -6,  5,   -3,  -9,  -3,  9,   -1,  -13, -1,  5,   -3,  -7,  -3,  4,   -8,  -2,  -8,  3,   4,
  2,   12,  12,  2,   -5,  3,   11,  6,   -9,  11,  -13, 3,   -1,  7,   12,  11,  -1,  12,  4,
  -3,  0,   -3,  6,   4,   -11, 4,   12,  2,   -4,  2,   1,   -10, -6,  -8,  1,   -13, 7,   -11,
  1,   -13, 12,  -11, -13, 6,   0,   11,  -13, 0,   -1,  1,   4,   -13, 3,   -9,  -2,  -9,  8,
  -6,  -3,  -13, -6,  -8,  -2,  5,   -9,  8,   10,  2,   7,   3,   -9,  -1,  -6,  -1,  -1,  9,
  5,   11,  -2,  11,  -3,  12,  -8,  3,   0,   3,   5,   -1,  4,   0,   10,  3,   -6,  4,   5,
  -13, 0,   -10, 5,   5,   8,   12,  11,  8,   9,   9,   -6,  7,   -4,  8,   -12, -10, 4,   -10,
  9,   7,   3,   12,  4,   9,   -7,  10,  -2,  7,   0,   12,  -2,  -1,  -6,  0,   -11,
};

static void makeRandomPattern(c8cv::Point *pattern, int npoints) {
  c8cv::RNG rng(0x34985739);  // we always start with a fixed seed,
                              // to make patterns the same on each run
  for (int i = 0; i < npoints; i++) {
    pattern[i].x = rng.uniform(-Gr8::PATCH_SIZE / 2, Gr8::PATCH_SIZE / 2 + 1);
    pattern[i].y = rng.uniform(-Gr8::PATCH_SIZE / 2, Gr8::PATCH_SIZE / 2 + 1);
  }
}

static inline float getScale(int level, int firstLevel, double scaleFactor) {
  return (float)std::pow(scaleFactor, (double)(level - firstLevel));
}

int Gr8::descriptorType() const { return CV_8U; }

int Gr8::defaultNorm() const { return c8cv::NORM_HAMMING; }

/** Compute the Gr8 keypoints on an image
 * @param image_pyramid the image pyramid to compute the features and descriptors on
 * @param keypoints the resulting keypoints, clustered per level
 */
static void computeKeyPoints(
  const c8cv::Mat &imagePyramid,
  const std::vector<c8cv::Rect> &layerInfo,
  const std::vector<float> &layerScale,
  std::vector<c8cv::KeyPoint> &allKeypoints,
  int nfeatures,
  double scaleFactor,
  int edgeThreshold,
  int scoreType,
  int fastThreshold,
  const std::vector<int> &umax,
  TaskQueue *taskQueue,
  ThreadPool *threadPool) {

  ScopeTimer t("compute-key-points");

  int i, nkeypoints, level, nlevels = (int)layerInfo.size();
  std::vector<int> nfeaturesPerLevel(nlevels);

  // fill the extractors and descriptors for the corresponding scales
  float factor = (float)(1.0 / scaleFactor);
  float ndesiredFeaturesPerScale =
    nfeatures * (1 - factor) / (1 - (float)std::pow((double)factor, (double)nlevels));

  int sumFeatures = 0;
  for (level = 0; level < nlevels - 1; level++) {
    nfeaturesPerLevel[level] = cvRound(ndesiredFeaturesPerScale);
    sumFeatures += nfeaturesPerLevel[level];
    ndesiredFeaturesPerScale *= factor;
  }
  nfeaturesPerLevel[nlevels - 1] = std::max(nfeatures - sumFeatures, 0);

  // Make sure we forget about what is too close to the boundary
  // edge_threshold_ = std::max(edge_threshold_, patch_size_/2 + kKernelWidth / 2 + 2);

  allKeypoints.clear();
  std::vector<int> counters(nlevels);

  // Create the keypoint levels.
  std::vector<std::vector<c8cv::KeyPoint>> keypointLevels(nlevels);

  auto detectFeaturesForLevel = [&](int level) {
    ScopeTimer t(format("detect-features-for-level-%02d", level));

    int featuresNum = nfeaturesPerLevel[level];

    c8cv::Mat img = imagePyramid(layerInfo[level]);

    std::vector<c8cv::KeyPoint> &keypoints = keypointLevels[level];
    keypoints.reserve(featuresNum * 2);

    // Detect FAST features, 20 is a good threshold
    {
      c8cv::Ptr<c8cv::FastFeatureDetector> fd =
        c8cv::FastFeatureDetector::create(fastThreshold, true);
      fd->detect(img, keypoints);
    }

    {
      // Remove keypoints very close to the border
      c8cv::KeyPointsFilter::runByImageBorder(keypoints, img.size(), edgeThreshold);
      // Keep more points than necessary as FAST does not give amazing corners
      Gr8::retainBest(
        keypoints,
        scoreType == Gr8CpuInterface::HARRIS_SCORE ? 2 * featuresNum : featuresNum,
        img.cols,
        true);

      float sf = layerScale[level];
      for (int i = 0; i < keypoints.size(); i++) {
        keypoints[i].octave = level;
        keypoints[i].size = Gr8::PATCH_SIZE * sf;
      }
    }
  };

  // Add the level tasks to the thread queue in reverse, since level 0 is fastest.
  for (int level = 0; level < nlevels; ++level) {
    taskQueue->addTask(std::bind(std::cref(detectFeaturesForLevel), level));
  }

  {
    ScopeTimer tLevel("n-levels-features");

    // Detect features in parallel.
    taskQueue->executeWithThreadPool(threadPool);

    for (int level = 0; level < nlevels; ++level) {
      auto &keypoints = keypointLevels[level];
      std::copy(keypoints.begin(), keypoints.end(), std::back_inserter(allKeypoints));
    }
  }

  // Select best features using the Harris cornerness (better scoring than FAST)
  if (scoreType == Gr8CpuInterface::HARRIS_SCORE) {
    ScopeTimer t3("select-best-features");

    {
      ScopeTimer t4("harris-responses");
      HarrisResponses(imagePyramid, layerInfo, allKeypoints, 7, HARRIS_K);
    }

    Vector<int> offsets;
    offsets.reserve(nlevels);

    // Compute offsets.
    int offset = 0;
    for (const auto counter : counters) {
      offsets.push_back(offset);
      offset += counter;
    }

    std::vector<c8cv::KeyPoint> newAllKeypoints;
    newAllKeypoints.reserve(nfeaturesPerLevel[0] * nlevels);

    {
      ScopeTimer t4("retain-best");

      auto retainBest = [&](int level) {
        int featuresNum = nfeaturesPerLevel[level];
        int offset = offsets[level];
        int nkeypoints = counters[level];

        // Assign the allKeypoints with Harris information back to the keypoints array. This relies
        // on the fact that keypointLevels still has the correct number of elements.
        auto &keypoints = keypointLevels[level];

        // keypoints.resize(nkeypoints);
        auto start = allKeypoints.cbegin() + offset;
        std::copy(start, start + nkeypoints, keypoints.begin());

        // cull to the final desired level, using the new Harris scores.
        c8cv::Mat img = imagePyramid(layerInfo[level]);
        Gr8::retainBest(keypoints, featuresNum, img.cols, false);
      };

      for (int level = 0; level < nlevels; ++level) {
        taskQueue->addTask(std::bind(std::cref(retainBest), level));
      }

      taskQueue->executeWithThreadPool(threadPool);

      for (int level = 0; level < nlevels; level++) {
        auto &keypoints = keypointLevels[level];
        std::copy(keypoints.cbegin(), keypoints.cend(), std::back_inserter(newAllKeypoints));
      }
    }
    std::swap(allKeypoints, newAllKeypoints);
  }

  nkeypoints = (int)allKeypoints.size();

  {
    ScopeTimer t4("ic-angles");
    ICAngles(imagePyramid, layerInfo, allKeypoints, umax);
  }

  for (i = 0; i < nkeypoints; i++) {
    float scale = layerScale[allKeypoints[i].octave];
    allKeypoints[i].pt *= scale;
  }
}

// Compute the Gr8 features and descriptors in an image.
ImagePoints Gr8::detectAndCompute(YPlanePixels frame) {
  std::vector<c8cv::KeyPoint> keyPoints;
  c8cv::Mat descriptors;
  {
    // Detect features and compute descriptors.
    c8cv::Mat frameWrap(frame.rows(), frame.cols(), CV_8UC1, frame.pixels(), frame.rowBytes());
    detectAndCompute(frameWrap, keyPoints, descriptors);
  }

  ImagePoints pts;
  // copy output.
  int i = 0;
  for (const auto &k : keyPoints) {
    pts.push_back(
      {k.pt.x,
       k.pt.y,
       k.size,
       static_cast<uint8_t>(k.octave),
       k.angle,
       -1,  // Gravity angle unknown in CV implementation
       k.response,
       -1});
    pts.back().mutableFeatures().orbFeature = toImageDescriptor<32>(descriptors.rowRange(i, i + 1));
    i++;
  }
  return pts;
}

/** Compute the Gr8 features and descriptors on an image
 * @param img the image to compute the features and descriptors on
 * @param keypoints the resulting keypoints
 * @param descriptors the resulting descriptors
 */
void Gr8::detectAndCompute(
  c8cv::InputArray _image, std::vector<c8cv::KeyPoint> &keypoints, c8cv::OutputArray _descriptors) {

  ScopeTimer t("gr8");

  if (_image.empty()) {
    return;
  }

  // ROI handling
  const int HARRIS_BLOCK_SIZE = 9;

  // sqrt(2.0) is for handling patch rotation of halfPatchSize
  int descPatchSize = cvCeil(Gr8::PATCH_SIZE * std::sqrt(0.5f));
  int border = std::max(edgeThreshold_, std::max(descPatchSize, HARRIS_BLOCK_SIZE / 2)) + 1;

  c8cv::Mat image = _image.getMat();
  C8CV_Assert(image.type() == CV_8UC1);

  int level, nLevels = this->nlevels_, nkeypoints = (int)keypoints.size();

  std::vector<c8cv::Rect> layerInfo(nLevels);
  std::vector<int> layerOfs(nLevels);
  std::vector<float> layerScale(nLevels);

  int level_dy = image.rows + border * 2;
  c8cv::Point level_ofs(0, 0);
  c8cv::Size bufSize((image.cols + border * 2 + 15) & -16, 0);

  for (level = 0; level < nLevels; level++) {
    float scale = getScale(level, firstLevel_, scaleFactor_);
    layerScale[level] = scale;
    // force the level to be a factor of 2 in height.
    c8cv::Size sz(cvRound(image.cols / scale), 2 * cvRound(image.rows / scale / 2));
    c8cv::Size wholeSize(sz.width + border * 2, sz.height + border * 2);
    if (level_ofs.x + wholeSize.width > bufSize.width) {
      level_ofs = c8cv::Point(0, level_ofs.y + level_dy);
      level_dy = wholeSize.height;
    }

    c8cv::Rect linfo(level_ofs.x + border, level_ofs.y + border, sz.width, sz.height);
    layerInfo[level] = linfo;
    layerOfs[level] = linfo.y * bufSize.width + linfo.x;
    level_ofs.x += wholeSize.width;
  }
  bufSize.height = level_ofs.y + level_dy;

  // only re-create if we need more space
  if (imagePyramid_.cols < bufSize.width || imagePyramid_.rows < bufSize.height) {
    imagePyramid_.create(bufSize, CV_8U);
  }
  if (tmpPyramid_.cols < imagePyramid_.cols || tmpPyramid_.rows < imagePyramid_.rows) {
    tmpPyramid_.create(c8cv::Size(imagePyramid_.cols, imagePyramid_.rows), imagePyramid_.type());
  }

  c8cv::Mat prevImg = image;

  {
    // Pre-compute the scale pyramids
    ScopeTimer t2("precompute-scale-pyramids");
    for (level = 0; level < nLevels; ++level) {

      c8cv::Rect linfo = layerInfo[level];
      c8cv::Size sz(linfo.width, linfo.height);
      c8cv::Size wholeSize(sz.width + border * 2, sz.height + border * 2);
      c8cv::Rect wholeLinfo =
        c8cv::Rect(linfo.x - border, linfo.y - border, wholeSize.width, wholeSize.height);
      c8cv::Mat extImg = imagePyramid_(wholeLinfo);
      c8cv::Mat currImg = extImg(c8cv::Rect(border, border, sz.width, sz.height));

      // Compute the resized image
      if (level != firstLevel_) {
        // Switched to NEAREST for faster processing at seemingly little quality impact.
        c8cv::resize(prevImg, currImg, sz, 0, 0, c8cv::INTER_NEAREST);

        copyMakeBorder(
          currImg,
          extImg,
          border,
          border,
          border,
          border,
          c8cv::BORDER_REFLECT_101 + c8cv::BORDER_ISOLATED);
      } else {
        copyMakeBorder(image, extImg, border, border, border, border, c8cv::BORDER_REFLECT_101);
      }
      prevImg = currImg;
    }
  }

  // Get keypoints, those will be far enough from the border that no check will be required for
  // the descriptor
  computeKeyPoints(
    imagePyramid_,
    layerInfo,
    layerScale,
    keypoints,
    nfeatures_,
    scaleFactor_,
    edgeThreshold_,
    scoreType_,
    fastThreshold_,
    umax_,
    &taskQueue_,
    threadPool_.get());

  {
    ScopeTimer t2("do-descriptors");

    constexpr int DSIZE = Gr8::descriptorSize();

    nkeypoints = (int)keypoints.size();
    if (nkeypoints == 0) {
      _descriptors.release();
      return;
    }

    _descriptors.create(nkeypoints, DSIZE, CV_8U);

    if (pattern_.empty()) {
      initGr8Pattern();
    }

    {
      ScopeTimer t3("preprocess-image");

      // preprocess the resized image
      auto preprocessImage = [&](int level, int segment, int nSegments) {
        c8cv::Rect linfo = layerInfo[level];
        int height = linfo.height / nSegments;

        // subimage of level with overlap
        c8cv::Rect rect;
        if (segment == 0) {  // overlap bottom
          rect = c8cv::Rect(linfo.x, linfo.y, linfo.width, height + filterOverlapPx_);
        } else if (segment == nSegments - 1) {  // overlap top
          rect = c8cv::Rect(
            linfo.x,
            linfo.y + height * segment - filterOverlapPx_,
            linfo.width,
            height + filterOverlapPx_);
        } else {  // overlap both
          rect = c8cv::Rect(
            linfo.x,
            linfo.y + height * segment - filterOverlapPx_,
            linfo.width,
            height + filterOverlapPx_ * 2);
        }

        c8cv::Mat workingMat = imagePyramid_(rect);
        c8cv::Mat dst = tmpPyramid_(rect);
        c8cv::Point ofs;
        c8cv::Size wsz(workingMat.cols, workingMat.rows);
        workingMat.locateROI(wsz, ofs);

        // apply Gaussian blur to subimage
        filters_.at(filterNumSegments_ * level + segment)->apply(workingMat, dst, wsz, ofs);

        // no overlap when copying to output
        rect = c8cv::Rect(linfo.x, linfo.y + height * segment, linfo.width, height);
        workingMat = imagePyramid_(rect);
        dst = tmpPyramid_(rect);
        workingMat = dst;
      };

      for (level = 0; level < nLevels; ++level) {
        int nSegments = 2;
        while (layerInfo[level].height / nSegments > 200) {
          nSegments += 2;
        };
        if (nSegments > filterNumSegments_) {
          nSegments = filterNumSegments_;
        }
        // TODO(scott): magic size of 200 seems to be optimum on laptop.
        for (int segment = 0; segment < nSegments; ++segment) {
          taskQueue_.addTask(std::bind(std::cref(preprocessImage), level, segment, nSegments));
        }
      }
      taskQueue_.executeWithThreadPool(threadPool_.get());
    }

    {
      c8cv::Mat descriptors = _descriptors.getMat();
      ScopeTimer t3("compute-descriptors");
      computeGr8Descriptors(
        imagePyramid_,
        layerInfo,
        layerScale,
        keypoints,
        descriptors,
        pattern_,
        wtaK_,
        &taskQueue_,
        threadPool_.get());
    }
  }
}

void Gr8::initGr8Pattern() {
  const int npoints = 512;
  c8cv::Point patternbuf[npoints];
  const c8cv::Point *pattern0 = (const c8cv::Point *)bit_pattern_31_;

  if (Gr8::PATCH_SIZE != 31) {
    // This code should never be reached since PATCH_SIZE is a constant equal to 31.
    pattern0 = patternbuf;
    makeRandomPattern(patternbuf, npoints);
  }

  C8CV_Assert(wtaK_ == 2 || wtaK_ == 3 || wtaK_ == 4);

  if (wtaK_ == 2) {
    std::copy(pattern0, pattern0 + npoints, std::back_inserter(pattern_));
  } else {
    int ntuples = descriptorSize() * 4;
    initializeGr8Pattern(pattern0, pattern_, ntuples, wtaK_, npoints);
  }
}

namespace {

const std::pair<int8_t, int8_t> FEATURE_LUT[512][36] = {
  {
    {8, -3},  {8, -2},  {9, 0},  {8, 1},   {8, 3},   {7, 4},   {7, 5},   {6, 6},   {4, 7},
    {3, 8},   {2, 8},   {0, 9},  {-1, 8},  {-3, 8},  {-4, 7},  {-5, 7},  {-6, 6},  {-7, 4},
    {-8, 3},  {-8, 2},  {-9, 0}, {-8, -1}, {-8, -3}, {-7, -4}, {-7, -5}, {-6, -6}, {-4, -7},
    {-3, -8}, {-2, -8}, {0, -9}, {1, -8},  {3, -8},  {4, -7},  {5, -7},  {6, -6},  {7, -4},
  },
  {
    {9, 5},   {8, 6},   {7, 8},   {5, 9},   {4, 10},   {2, 10},   {0, 10},  {-2, 10},  {-3, 10},
    {-5, 9},  {-6, 8},  {-8, 7},  {-9, 5},  {-10, 4},  {-10, 2},  {-10, 0}, {-10, -2}, {-10, -3},
    {-9, -5}, {-8, -6}, {-7, -8}, {-5, -9}, {-4, -10}, {-2, -10}, {0, -10}, {2, -10},  {3, -10},
    {5, -9},  {6, -8},  {8, -7},  {9, -5},  {10, -4},  {10, -2},  {10, 0},  {10, 2},   {10, 3},
  },
  {
    {4, 2},   {4, 3},   {3, 3},   {2, 4},   {2, 4},   {1, 4},   {0, 4},  {-1, 4},  {-1, 4},
    {-2, 4},  {-3, 4},  {-3, 3},  {-4, 2},  {-4, 2},  {-4, 1},  {-4, 0}, {-4, -1}, {-4, -1},
    {-4, -2}, {-4, -3}, {-3, -3}, {-2, -4}, {-2, -4}, {-1, -4}, {0, -4}, {1, -4},  {1, -4},
    {2, -4},  {3, -4},  {3, -3},  {4, -2},  {4, -2},  {4, -1},  {4, 0},  {4, 1},   {4, 1},
  },
  {
    {7, -12},  {9, -11},  {11, -9},  {12, -7},  {13, -5},  {14, -2},  {14, 0},   {14, 2},
    {13, 5},   {12, 7},   {11, 9},   {9, 11},   {7, 12},   {5, 13},   {2, 14},   {0, 14},
    {-2, 14},  {-5, 13},  {-7, 12},  {-9, 11},  {-11, 9},  {-12, 7},  {-13, 5},  {-14, 2},
    {-14, 0},  {-14, -2}, {-13, -5}, {-12, -7}, {-11, -9}, {-9, -11}, {-7, -12}, {-5, -13},
    {-2, -14}, {0, -14},  {2, -14},  {5, -13},
  },
  {
    {-11, 9},  {-12, 7},  {-13, 5},  {-14, 2},  {-14, 0},  {-14, -3}, {-13, -5}, {-12, -7},
    {-11, -9}, {-9, -11}, {-7, -12}, {-5, -13}, {-2, -14}, {0, -14},  {3, -14},  {5, -13},
    {7, -12},  {9, -11},  {11, -9},  {12, -7},  {13, -5},  {14, -2},  {14, 0},   {14, 3},
    {13, 5},   {12, 7},   {11, 9},   {9, 11},   {7, 12},   {5, 13},   {2, 14},   {0, 14},
    {-3, 14},  {-5, 13},  {-7, 12},  {-9, 11},
  },
  {
    {-8, 2},  {-8, 1},  {-8, -1}, {-8, -2}, {-7, -4}, {-7, -5}, {-6, -6}, {-5, -7}, {-3, -8},
    {-2, -8}, {-1, -8}, {1, -8},  {2, -8},  {4, -7},  {5, -7},  {6, -6},  {7, -5},  {8, -3},
    {8, -2},  {8, -1},  {8, 1},   {8, 2},   {7, 4},   {7, 5},   {6, 6},   {5, 7},   {3, 8},
    {2, 8},   {1, 8},   {-1, 8},  {-2, 8},  {-4, 7},  {-5, 7},  {-6, 6},  {-7, 5},  {-8, 3},
  },
  {
    {7, -12},  {9, -11},  {11, -9},  {12, -7},  {13, -5},  {14, -2},  {14, 0},   {14, 2},
    {13, 5},   {12, 7},   {11, 9},   {9, 11},   {7, 12},   {5, 13},   {2, 14},   {0, 14},
    {-2, 14},  {-5, 13},  {-7, 12},  {-9, 11},  {-11, 9},  {-12, 7},  {-13, 5},  {-14, 2},
    {-14, 0},  {-14, -2}, {-13, -5}, {-12, -7}, {-11, -9}, {-9, -11}, {-7, -12}, {-5, -13},
    {-2, -14}, {0, -14},  {2, -14},  {5, -13},
  },
  {
    {12, -13}, {14, -11}, {16, -8},   {17, -5},   {18, -2},   {18, 1},   {17, 4},   {16, 7},
    {15, 10},  {13, 12},  {11, 14},   {8, 16},    {5, 17},    {2, 18},   {-1, 18},  {-4, 17},
    {-7, 16},  {-10, 15}, {-12, 13},  {-14, 11},  {-16, 8},   {-17, 5},  {-18, 2},  {-18, -1},
    {-17, -4}, {-16, -7}, {-15, -10}, {-13, -12}, {-11, -14}, {-8, -16}, {-5, -17}, {-2, -18},
    {1, -18},  {4, -17},  {7, -16},   {10, -15},
  },
  {
    {2, -13},  {4, -12},  {6, -12},  {8, -10},  {10, -9},  {11, -7},  {12, -5},  {13, -3},
    {13, 0},   {13, 2},   {12, 4},   {12, 6},   {10, 8},   {9, 10},   {7, 11},   {5, 12},
    {3, 13},   {0, 13},   {-2, 13},  {-4, 12},  {-6, 12},  {-8, 10},  {-10, 9},  {-11, 7},
    {-12, 5},  {-13, 3},  {-13, 0},  {-13, -2}, {-12, -4}, {-12, -6}, {-10, -8}, {-9, -10},
    {-7, -11}, {-5, -12}, {-3, -13}, {0, -13},
  },
  {
    {2, 12},   {0, 12},  {-2, 12},  {-4, 11},  {-6, 10},  {-8, 9},  {-9, 8},  {-11, 6},  {-11, 4},
    {-12, 2},  {-12, 0}, {-12, -2}, {-11, -4}, {-10, -6}, {-9, -8}, {-8, -9}, {-6, -11}, {-4, -11},
    {-2, -12}, {0, -12}, {2, -12},  {4, -11},  {6, -10},  {8, -9},  {9, -8},  {11, -6},  {11, -4},
    {12, -2},  {12, 0},  {12, 2},   {11, 4},   {10, 6},   {9, 8},   {8, 9},   {6, 11},   {4, 11},
  },
  {
    {1, -7},  {2, -7},  {3, -6},  {4, -6},  {5, -5},  {6, -4},  {7, -3},  {7, -1},  {7, 0},
    {7, 1},   {7, 2},   {6, 3},   {6, 4},   {5, 5},   {4, 6},   {3, 7},   {1, 7},   {0, 7},
    {-1, 7},  {-2, 7},  {-3, 6},  {-4, 6},  {-5, 5},  {-6, 4},  {-7, 3},  {-7, 1},  {-7, 0},
    {-7, -1}, {-7, -2}, {-6, -3}, {-6, -4}, {-5, -5}, {-4, -6}, {-3, -7}, {-1, -7}, {0, -7},
  },
  {
    {1, 6},   {0, 6},  {-1, 6},  {-2, 6},  {-3, 5},  {-4, 5},  {-5, 4},  {-5, 3},  {-6, 2},
    {-6, 1},  {-6, 0}, {-6, -1}, {-6, -2}, {-5, -3}, {-5, -4}, {-4, -5}, {-3, -5}, {-2, -6},
    {-1, -6}, {0, -6}, {1, -6},  {2, -6},  {3, -5},  {4, -5},  {5, -4},  {5, -3},  {6, -2},
    {6, -1},  {6, 0},  {6, 1},   {6, 2},   {5, 3},   {5, 4},   {4, 5},   {3, 5},   {2, 6},
  },
  {
    {-2, -10}, {0, -10}, {2, -10},  {3, -10},  {5, -9},  {6, -8},  {8, -7},  {9, -5},  {10, -4},
    {10, -2},  {10, 0},  {10, 2},   {10, 3},   {9, 5},   {8, 6},   {7, 8},   {5, 9},   {4, 10},
    {2, 10},   {0, 10},  {-2, 10},  {-3, 10},  {-5, 9},  {-6, 8},  {-8, 7},  {-9, 5},  {-10, 4},
    {-10, 2},  {-10, 0}, {-10, -2}, {-10, -3}, {-9, -5}, {-8, -6}, {-7, -8}, {-5, -9}, {-4, -10},
  },
  {
    {-2, -4}, {-1, -4}, {-1, -4}, {0, -4}, {1, -4},  {2, -4},  {2, -4},  {3, -3},  {4, -3},
    {4, -2},  {4, -1},  {4, -1},  {4, 0},  {4, 1},   {4, 2},   {4, 2},   {3, 3},   {3, 4},
    {2, 4},   {1, 4},   {1, 4},   {0, 4},  {-1, 4},  {-2, 4},  {-2, 4},  {-3, 3},  {-4, 3},
    {-4, 2},  {-4, 1},  {-4, 1},  {-4, 0}, {-4, -1}, {-4, -2}, {-4, -2}, {-3, -3}, {-3, -4},
  },
  {
    {-13, -13}, {-11, -15}, {-8, -17}, {-5, -18},  {-2, -18}, {2, -18}, {5, -18}, {8, -17},
    {11, -15},  {13, -13},  {15, -11}, {17, -8},   {18, -5},  {18, -2}, {18, 2},  {18, 5},
    {17, 8},    {15, 11},   {13, 13},  {11, 15},   {8, 17},   {5, 18},  {2, 18},  {-2, 18},
    {-5, 18},   {-8, 17},   {-11, 15}, {-13, 13},  {-15, 11}, {-17, 8}, {-18, 5}, {-18, 2},
    {-18, -2},  {-18, -5},  {-17, -8}, {-15, -11},
  },
  {
    {-11, -8}, {-9, -10}, {-8, -11}, {-6, -12}, {-3, -13}, {-1, -14}, {1, -14}, {4, -13},
    {6, -12},  {8, -11},  {10, -9},  {11, -8},  {12, -6},  {13, -3},  {14, -1}, {14, 1},
    {13, 4},   {12, 6},   {11, 8},   {9, 10},   {8, 11},   {6, 12},   {3, 13},  {1, 14},
    {-1, 14},  {-4, 13},  {-6, 12},  {-8, 11},  {-10, 9},  {-11, 8},  {-12, 6}, {-13, 3},
    {-14, 1},  {-14, -1}, {-13, -4}, {-12, -6},
  },
  {
    {-13, -3}, {-12, -5}, {-11, -7}, {-10, -9}, {-8, -11}, {-6, -12}, {-4, -13}, {-2, -13},
    {1, -13},  {3, -13},  {5, -12},  {7, -11},  {9, -10},  {11, -8},  {12, -6},  {13, -4},
    {13, -2},  {13, 1},   {13, 3},   {12, 5},   {11, 7},   {10, 9},   {8, 11},   {6, 12},
    {4, 13},   {2, 13},   {-1, 13},  {-3, 13},  {-5, 12},  {-7, 11},  {-9, 10},  {-11, 8},
    {-12, 6},  {-13, 4},  {-13, 2},  {-13, -1},
  },
  {
    {-12, -9}, {-10, -11}, {-8, -13}, {-6, -14}, {-3, -15}, {-1, -15}, {2, -15}, {4, -14},
    {7, -13},  {9, -12},   {11, -10}, {13, -8},  {14, -6},  {15, -3},  {15, -1}, {15, 2},
    {14, 4},   {13, 7},    {12, 9},   {10, 11},  {8, 13},   {6, 14},   {3, 15},  {1, 15},
    {-2, 15},  {-4, 14},   {-7, 13},  {-9, 12},  {-11, 10}, {-13, 8},  {-14, 6}, {-15, 3},
    {-15, 1},  {-15, -2},  {-14, -4}, {-13, -7},
  },
  {
    {10, 4},   {9, 6},   {8, 7},   {7, 8},   {5, 9},   {3, 10},   {2, 11},   {0, 11},  {-2, 11},
    {-4, 10},  {-6, 9},  {-7, 8},  {-8, 7},  {-9, 5},  {-10, 3},  {-11, 2},  {-11, 0}, {-11, -2},
    {-10, -4}, {-9, -6}, {-8, -7}, {-7, -8}, {-5, -9}, {-3, -10}, {-2, -11}, {0, -11}, {2, -11},
    {4, -10},  {6, -9},  {7, -8},  {8, -7},  {9, -5},  {10, -3},  {11, -2},  {11, 0},  {11, 2},
  },
  {
    {11, 9},   {9, 11},   {7, 12},   {5, 13},   {3, 14},   {0, 14},   {-2, 14},  {-5, 13},
    {-7, 12},  {-9, 11},  {-11, 9},  {-12, 7},  {-13, 5},  {-14, 3},  {-14, 0},  {-14, -2},
    {-13, -5}, {-12, -7}, {-11, -9}, {-9, -11}, {-7, -12}, {-5, -13}, {-3, -14}, {0, -14},
    {2, -14},  {5, -13},  {7, -12},  {9, -11},  {11, -9},  {12, -7},  {13, -5},  {14, -3},
    {14, 0},   {14, 2},   {13, 5},   {12, 7},
  },
  {
    {-13, -8}, {-11, -10}, {-9, -12}, {-7, -13}, {-5, -14}, {-2, -15}, {0, -15}, {3, -15},
    {6, -14},  {8, -13},   {10, -11}, {12, -9},  {13, -7},  {14, -5},  {15, -2}, {15, 0},
    {15, 3},   {14, 6},    {13, 8},   {11, 10},  {9, 12},   {7, 13},   {5, 14},  {2, 15},
    {0, 15},   {-3, 15},   {-6, 14},  {-8, 13},  {-10, 11}, {-12, 9},  {-13, 7}, {-14, 5},
    {-15, 2},  {-15, 0},   {-15, -3}, {-14, -6},
  },
  {
    {-8, -9}, {-6, -10}, {-4, -11}, {-2, -12}, {0, -12}, {2, -12},  {4, -11},  {6, -11},  {7, -9},
    {9, -8},  {10, -6},  {11, -4},  {12, -2},  {12, 0},  {12, 2},   {11, 4},   {11, 6},   {9, 7},
    {8, 9},   {6, 10},   {4, 11},   {2, 12},   {0, 12},  {-2, 12},  {-4, 11},  {-6, 11},  {-7, 9},
    {-9, 8},  {-10, 6},  {-11, 4},  {-12, 2},  {-12, 0}, {-12, -2}, {-11, -4}, {-11, -6}, {-9, -7},
  },
  {
    {-11, 7},  {-12, 5},  {-13, 3},  {-13, 1},  {-13, -2}, {-12, -4}, {-12, -6}, {-10, -8},
    {-9, -10}, {-7, -11}, {-5, -12}, {-3, -13}, {-1, -13}, {2, -13},  {4, -12},  {6, -12},
    {8, -10},  {10, -9},  {11, -7},  {12, -5},  {13, -3},  {13, -1},  {13, 2},   {12, 4},
    {12, 6},   {10, 8},   {9, 10},   {7, 11},   {5, 12},   {3, 13},   {1, 13},   {-2, 13},
    {-4, 12},  {-6, 12},  {-8, 10},  {-10, 9},
  },
  {
    {-9, 12},  {-11, 10}, {-13, 8},   {-14, 6},  {-15, 3},  {-15, 1},  {-15, -2}, {-14, -4},
    {-13, -7}, {-12, -9}, {-10, -11}, {-8, -13}, {-6, -14}, {-3, -15}, {-1, -15}, {2, -15},
    {4, -14},  {7, -13},  {9, -12},   {11, -10}, {13, -8},  {14, -6},  {15, -3},  {15, -1},
    {15, 2},   {14, 4},   {13, 7},    {12, 9},   {10, 11},  {8, 13},   {6, 14},   {3, 15},
    {1, 15},   {-2, 15},  {-4, 14},   {-7, 13},
  },
  {
    {7, 7},   {6, 8},   {4, 9},   {3, 10},   {1, 10},   {-1, 10},  {-3, 10},  {-4, 9},  {-6, 8},
    {-7, 7},  {-8, 6},  {-9, 4},  {-10, 3},  {-10, 1},  {-10, -1}, {-10, -3}, {-9, -4}, {-8, -6},
    {-7, -7}, {-6, -8}, {-4, -9}, {-3, -10}, {-1, -10}, {1, -10},  {3, -10},  {4, -9},  {6, -8},
    {7, -7},  {8, -6},  {9, -4},  {10, -3},  {10, -1},  {10, 1},   {10, 3},   {9, 4},   {8, 6},
  },
  {
    {12, 6},   {11, 8},   {9, 10},   {7, 11},   {5, 12},   {3, 13},   {1, 13},   {-2, 13},
    {-4, 13},  {-6, 12},  {-8, 11},  {-10, 9},  {-11, 7},  {-12, 5},  {-13, 3},  {-13, 1},
    {-13, -2}, {-13, -4}, {-12, -6}, {-11, -8}, {-9, -10}, {-7, -11}, {-5, -12}, {-3, -13},
    {-1, -13}, {2, -13},  {4, -13},  {6, -12},  {8, -11},  {10, -9},  {11, -7},  {12, -5},
    {13, -3},  {13, -1},  {13, 2},   {13, 4},
  },
  {
    {-4, -5}, {-3, -6}, {-2, -6}, {-1, -6}, {0, -6}, {1, -6},  {2, -6},  {3, -5},  {4, -5},
    {5, -4},  {6, -3},  {6, -2},  {6, -1},  {6, 0},  {6, 1},   {6, 2},   {5, 3},   {5, 4},
    {4, 5},   {3, 6},   {2, 6},   {1, 6},   {0, 6},  {-1, 6},  {-2, 6},  {-3, 5},  {-4, 5},
    {-5, 4},  {-6, 3},  {-6, 2},  {-6, 1},  {-6, 0}, {-6, -1}, {-6, -2}, {-5, -3}, {-5, -4},
  },
  {
    {-3, 0}, {-3, -1}, {-3, -1}, {-3, -2}, {-2, -2}, {-2, -2}, {-2, -3}, {-1, -3}, {-1, -3},
    {0, -3}, {1, -3},  {1, -3},  {2, -3},  {2, -2},  {2, -2},  {3, -2},  {3, -1},  {3, -1},
    {3, 0},  {3, 1},   {3, 1},   {3, 2},   {2, 2},   {2, 2},   {2, 3},   {1, 3},   {1, 3},
    {0, 3},  {-1, 3},  {-1, 3},  {-2, 3},  {-2, 2},  {-2, 2},  {-3, 2},  {-3, 1},  {-3, 1},
  },
  {
    {-13, 2},  {-13, 0},  {-13, -3}, {-12, -5}, {-11, -7}, {-10, -9}, {-8, -10}, {-6, -12},
    {-4, -12}, {-2, -13}, {0, -13},  {3, -13},  {5, -12},  {7, -11},  {9, -10},  {10, -8},
    {12, -6},  {12, -4},  {13, -2},  {13, 0},   {13, 3},   {12, 5},   {11, 7},   {10, 9},
    {8, 10},   {6, 12},   {4, 12},   {2, 13},   {0, 13},   {-3, 13},  {-5, 12},  {-7, 11},
    {-9, 10},  {-10, 8},  {-12, 6},  {-12, 4},
  },
  {
    {-12, -3}, {-11, -5}, {-10, -7}, {-9, -9},  {-7, -10}, {-5, -11}, {-3, -12}, {-1, -12},
    {1, -12},  {3, -12},  {5, -11},  {7, -10},  {9, -9},   {10, -7},  {11, -5},  {12, -3},
    {12, -1},  {12, 1},   {12, 3},   {11, 5},   {10, 7},   {9, 9},    {7, 10},   {5, 11},
    {3, 12},   {1, 12},   {-1, 12},  {-3, 12},  {-5, 11},  {-7, 10},  {-9, 9},   {-10, 7},
    {-11, 5},  {-12, 3},  {-12, 1},  {-12, -1},
  },
  {
    {-9, 0}, {-9, -2}, {-8, -3}, {-8, -5}, {-7, -6}, {-6, -7}, {-5, -8}, {-3, -8}, {-2, -9},
    {0, -9}, {2, -9},  {3, -8},  {5, -8},  {6, -7},  {7, -6},  {8, -5},  {8, -3},  {9, -2},
    {9, 0},  {9, 2},   {8, 3},   {8, 5},   {7, 6},   {6, 7},   {5, 8},   {3, 8},   {2, 9},
    {0, 9},  {-2, 9},  {-3, 8},  {-5, 8},  {-6, 7},  {-7, 6},  {-8, 5},  {-8, 3},  {-9, 2},
  },
  {
    {-7, 5},  {-8, 4},  {-8, 2},  {-9, 1},  {-9, -1}, {-8, -2}, {-8, -4}, {-7, -5}, {-6, -6},
    {-5, -7}, {-4, -8}, {-2, -8}, {-1, -9}, {1, -9},  {2, -8},  {4, -8},  {5, -7},  {6, -6},
    {7, -5},  {8, -4},  {8, -2},  {9, -1},  {9, 1},   {8, 2},   {8, 4},   {7, 5},   {6, 6},
    {5, 7},   {4, 8},   {2, 8},   {1, 9},   {-1, 9},  {-2, 8},  {-4, 8},  {-5, 7},  {-6, 6},
  },
  {
    {12, -6},  {13, -4},  {13, -2},  {13, 1},   {13, 3},   {12, 5},   {11, 7},   {10, 9},
    {8, 11},   {6, 12},   {4, 13},   {2, 13},   {-1, 13},  {-3, 13},  {-5, 12},  {-7, 11},
    {-9, 10},  {-11, 8},  {-12, 6},  {-13, 4},  {-13, 2},  {-13, -1}, {-13, -3}, {-12, -5},
    {-11, -7}, {-10, -9}, {-8, -11}, {-6, -12}, {-4, -13}, {-2, -13}, {1, -13},  {3, -13},
    {5, -12},  {7, -11},  {9, -10},  {11, -8},
  },
  {
    {12, -1},  {12, 1},   {12, 3},   {11, 5},   {10, 7},   {8, 9},    {7, 10},   {5, 11},
    {3, 12},   {1, 12},   {-1, 12},  {-3, 12},  {-5, 11},  {-7, 10},  {-9, 8},   {-10, 7},
    {-11, 5},  {-12, 3},  {-12, 1},  {-12, -1}, {-12, -3}, {-11, -5}, {-10, -7}, {-8, -9},
    {-7, -10}, {-5, -11}, {-3, -12}, {-1, -12}, {1, -12},  {3, -12},  {5, -11},  {7, -10},
    {9, -8},   {10, -7},  {11, -5},  {12, -3},
  },
  {
    {-3, 6},  {-4, 5},  {-5, 5},  {-6, 4},  {-6, 3},  {-7, 2},  {-7, 0}, {-7, -1}, {-6, -2},
    {-6, -3}, {-5, -4}, {-5, -5}, {-4, -6}, {-3, -6}, {-2, -7}, {0, -7}, {1, -7},  {2, -6},
    {3, -6},  {4, -5},  {5, -5},  {6, -4},  {6, -3},  {7, -2},  {7, 0},  {7, 1},   {6, 2},
    {6, 3},   {5, 4},   {5, 5},   {4, 6},   {3, 6},   {2, 7},   {0, 7},  {-1, 7},  {-2, 6},
  },
  {
    {-2, 12},  {-4, 11},  {-6, 11},  {-8, 9},  {-9, 8},  {-10, 6},  {-11, 4},  {-12, 2},  {-12, 0},
    {-12, -2}, {-11, -4}, {-11, -6}, {-9, -8}, {-8, -9}, {-6, -10}, {-4, -11}, {-2, -12}, {0, -12},
    {2, -12},  {4, -11},  {6, -11},  {8, -9},  {9, -8},  {10, -6},  {11, -4},  {12, -2},  {12, 0},
    {12, 2},   {11, 4},   {11, 6},   {9, 8},   {8, 9},   {6, 10},   {4, 11},   {2, 12},   {0, 12},
  },
  {
    {-6, -13}, {-4, -14}, {-1, -14},  {1, -14},  {4, -14}, {6, -13}, {8, -12},  {10, -10},
    {12, -8},  {13, -6},  {14, -4},   {14, -1},  {14, 1},  {14, 4},  {13, 6},   {12, 8},
    {10, 10},  {8, 12},   {6, 13},    {4, 14},   {1, 14},  {-1, 14}, {-4, 14},  {-6, 13},
    {-8, 12},  {-10, 10}, {-12, 8},   {-13, 6},  {-14, 4}, {-14, 1}, {-14, -1}, {-14, -4},
    {-13, -6}, {-12, -8}, {-10, -10}, {-8, -12},
  },
  {
    {-4, -8}, {-3, -9}, {-1, -9}, {1, -9},  {2, -9},  {4, -8},  {5, -7},  {6, -6},  {7, -5},
    {8, -4},  {9, -3},  {9, -1},  {9, 1},   {9, 2},   {8, 4},   {7, 5},   {6, 6},   {5, 7},
    {4, 8},   {3, 9},   {1, 9},   {-1, 9},  {-2, 9},  {-4, 8},  {-5, 7},  {-6, 6},  {-7, 5},
    {-8, 4},  {-9, 3},  {-9, 1},  {-9, -1}, {-9, -2}, {-8, -4}, {-7, -5}, {-6, -6}, {-5, -7},
  },
  {
    {11, -13}, {13, -11}, {15, -8},  {16, -6},   {17, -3},   {17, 0},   {17, 3},   {16, 6},
    {15, 9},   {13, 11},  {11, 13},  {8, 15},    {6, 16},    {3, 17},   {0, 17},   {-3, 17},
    {-6, 16},  {-9, 15},  {-11, 13}, {-13, 11},  {-15, 8},   {-16, 6},  {-17, 3},  {-17, 0},
    {-17, -3}, {-16, -6}, {-15, -9}, {-13, -11}, {-11, -13}, {-8, -15}, {-6, -16}, {-3, -17},
    {0, -17},  {3, -17},  {6, -16},  {9, -15},
  },
  {
    {12, -8},  {13, -6},  {14, -3},   {14, -1},  {14, 2},   {14, 4},   {13, 6},   {12, 9},
    {10, 10},  {8, 12},   {6, 13},    {3, 14},   {1, 14},   {-2, 14},  {-4, 14},  {-6, 13},
    {-9, 12},  {-10, 10}, {-12, 8},   {-13, 6},  {-14, 3},  {-14, 1},  {-14, -2}, {-14, -4},
    {-13, -6}, {-12, -9}, {-10, -10}, {-8, -12}, {-6, -13}, {-3, -14}, {-1, -14}, {2, -14},
    {4, -14},  {6, -13},  {9, -12},   {10, -10},
  },
  {
    {4, 7},   {3, 8},   {1, 8},   {0, 8},  {-1, 8},  {-3, 8},  {-4, 7},  {-5, 6},  {-6, 5},
    {-7, 4},  {-8, 3},  {-8, 1},  {-8, 0}, {-8, -1}, {-8, -3}, {-7, -4}, {-6, -5}, {-5, -6},
    {-4, -7}, {-3, -8}, {-1, -8}, {0, -8}, {1, -8},  {3, -8},  {4, -7},  {5, -6},  {6, -5},
    {7, -4},  {8, -3},  {8, -1},  {8, 0},  {8, 1},   {8, 3},   {7, 4},   {6, 5},   {5, 6},
  },
  {
    {5, 1},   {5, 2},   {4, 3},   {4, 3},   {3, 4},   {2, 4},   {2, 5},   {1, 5},   {0, 5},
    {-1, 5},  {-2, 5},  {-3, 4},  {-3, 4},  {-4, 3},  {-4, 2},  {-5, 2},  {-5, 1},  {-5, 0},
    {-5, -1}, {-5, -2}, {-4, -3}, {-4, -3}, {-3, -4}, {-2, -4}, {-2, -5}, {-1, -5}, {0, -5},
    {1, -5},  {2, -5},  {3, -4},  {3, -4},  {4, -3},  {4, -2},  {5, -2},  {5, -1},  {5, 0},
  },
  {
    {5, -3},  {5, -2},  {6, -1},  {6, 0},  {6, 1},   {6, 2},   {5, 3},   {5, 4},   {4, 4},
    {3, 5},   {2, 5},   {1, 6},   {0, 6},  {-1, 6},  {-2, 6},  {-3, 5},  {-4, 5},  {-4, 4},
    {-5, 3},  {-5, 2},  {-6, 1},  {-6, 0}, {-6, -1}, {-6, -2}, {-5, -3}, {-5, -4}, {-4, -4},
    {-3, -5}, {-2, -5}, {-1, -6}, {0, -6}, {1, -6},  {2, -6},  {3, -5},  {4, -5},  {4, -4},
  },
  {
    {10, -3},  {10, -1},  {10, 1},   {10, 2},   {10, 4},   {9, 6},   {8, 7},   {6, 8},   {5, 9},
    {3, 10},   {1, 10},   {-1, 10},  {-2, 10},  {-4, 10},  {-6, 9},  {-7, 8},  {-8, 6},  {-9, 5},
    {-10, 3},  {-10, 1},  {-10, -1}, {-10, -2}, {-10, -4}, {-9, -6}, {-8, -7}, {-6, -8}, {-5, -9},
    {-3, -10}, {-1, -10}, {1, -10},  {2, -10},  {4, -10},  {6, -9},  {7, -8},  {8, -6},  {9, -5},
  },
  {
    {3, -7},  {4, -6},  {5, -6},  {6, -5},  {7, -3},  {7, -2},  {8, -1},  {8, 0},  {7, 2},
    {7, 3},   {6, 4},   {6, 5},   {5, 6},   {3, 7},   {2, 7},   {1, 8},   {0, 8},  {-2, 7},
    {-3, 7},  {-4, 6},  {-5, 6},  {-6, 5},  {-7, 3},  {-7, 2},  {-8, 1},  {-8, 0}, {-7, -2},
    {-7, -3}, {-6, -4}, {-6, -5}, {-5, -6}, {-3, -7}, {-2, -7}, {-1, -8}, {0, -8}, {2, -7},
  },
  {
    {6, 12},   {4, 13},   {2, 13},   {-1, 13},  {-3, 13},  {-5, 12},  {-7, 11},  {-9, 10},
    {-11, 8},  {-12, 6},  {-13, 4},  {-13, 2},  {-13, -1}, {-13, -3}, {-12, -5}, {-11, -7},
    {-10, -9}, {-8, -11}, {-6, -12}, {-4, -13}, {-2, -13}, {1, -13},  {3, -13},  {5, -12},
    {7, -11},  {9, -10},  {11, -8},  {12, -6},  {13, -4},  {13, -2},  {13, 1},   {13, 3},
    {12, 5},   {11, 7},   {10, 9},   {8, 11},
  },
  {
    {-8, -7}, {-7, -8}, {-5, -9}, {-3, -10}, {-2, -11}, {0, -11}, {2, -10},  {4, -10},  {6, -9},
    {7, -8},  {8, -7},  {9, -5},  {10, -3},  {11, -2},  {11, 0},  {10, 2},   {10, 4},   {9, 6},
    {8, 7},   {7, 8},   {5, 9},   {3, 10},   {2, 11},   {0, 11},  {-2, 10},  {-4, 10},  {-6, 9},
    {-7, 8},  {-8, 7},  {-9, 5},  {-10, 3},  {-11, 2},  {-11, 0}, {-10, -2}, {-10, -4}, {-9, -6},
  },
  {
    {-6, -2}, {-6, -3}, {-5, -4}, {-4, -5}, {-3, -5}, {-2, -6}, {-1, -6}, {0, -6}, {1, -6},
    {2, -6},  {3, -6},  {4, -5},  {5, -4},  {5, -3},  {6, -2},  {6, -1},  {6, 0},  {6, 1},
    {6, 2},   {6, 3},   {5, 4},   {4, 5},   {3, 5},   {2, 6},   {1, 6},   {0, 6},  {-1, 6},
    {-2, 6},  {-3, 6},  {-4, 5},  {-5, 4},  {-5, 3},  {-6, 2},  {-6, 1},  {-6, 0}, {-6, -1},
  },
  {
    {-2, 11},  {-4, 10},  {-6, 10},  {-7, 9},  {-9, 7},  {-10, 6},  {-11, 4},  {-11, 2},  {-11, 0},
    {-11, -2}, {-10, -4}, {-10, -6}, {-9, -7}, {-7, -9}, {-6, -10}, {-4, -11}, {-2, -11}, {0, -11},
    {2, -11},  {4, -10},  {6, -10},  {7, -9},  {9, -7},  {10, -6},  {11, -4},  {11, -2},  {11, 0},
    {11, 2},   {10, 4},   {10, 6},   {9, 7},   {7, 9},   {6, 10},   {4, 11},   {2, 11},   {0, 11},
  },
  {
    {-1, -10}, {1, -10},  {2, -10},  {4, -9},  {6, -8},  {7, -7},  {8, -6},  {9, -4},  {10, -3},
    {10, -1},  {10, 1},   {10, 2},   {9, 4},   {8, 6},   {7, 7},   {6, 8},   {4, 9},   {3, 10},
    {1, 10},   {-1, 10},  {-2, 10},  {-4, 9},  {-6, 8},  {-7, 7},  {-8, 6},  {-9, 4},  {-10, 3},
    {-10, 1},  {-10, -1}, {-10, -2}, {-9, -4}, {-8, -6}, {-7, -7}, {-6, -8}, {-4, -9}, {-3, -10},
  },
  {
    {-13, 12},  {-15, 10},  {-16, 7},   {-17, 4},  {-18, 1},  {-18, -2}, {-17, -5}, {-16, -8},
    {-14, -11}, {-12, -13}, {-10, -15}, {-7, -16}, {-4, -17}, {-1, -18}, {2, -18},  {5, -17},
    {8, -16},   {11, -14},  {13, -12},  {15, -10}, {16, -7},  {17, -4},  {18, -1},  {18, 2},
    {17, 5},    {16, 8},    {14, 11},   {12, 13},  {10, 15},  {7, 16},   {4, 17},   {1, 18},
    {-2, 18},   {-5, 17},   {-8, 16},   {-11, 14},
  },
  {
    {-8, 10},  {-10, 8},  {-11, 7},  {-12, 5},  {-13, 3},  {-13, 0},  {-13, -2}, {-12, -4},
    {-11, -6}, {-10, -8}, {-8, -10}, {-7, -11}, {-5, -12}, {-3, -13}, {0, -13},  {2, -13},
    {4, -12},  {6, -11},  {8, -10},  {10, -8},  {11, -7},  {12, -5},  {13, -3},  {13, 0},
    {13, 2},   {12, 4},   {11, 6},   {10, 8},   {8, 10},   {7, 11},   {5, 12},   {3, 13},
    {0, 13},   {-2, 13},  {-4, 12},  {-6, 11},
  },
  {
    {-7, 3},  {-7, 2},  {-8, 0}, {-8, -1}, {-7, -2}, {-7, -3}, {-6, -5}, {-5, -6}, {-4, -6},
    {-3, -7}, {-2, -7}, {0, -8}, {1, -8},  {2, -7},  {3, -7},  {5, -6},  {6, -5},  {6, -4},
    {7, -3},  {7, -2},  {8, 0},  {8, 1},   {7, 2},   {7, 3},   {6, 5},   {5, 6},   {4, 6},
    {3, 7},   {2, 7},   {0, 8},  {-1, 8},  {-2, 7},  {-3, 7},  {-5, 6},  {-6, 5},  {-6, 4},
  },
  {
    {-5, -3}, {-4, -4}, {-4, -5}, {-3, -5}, {-2, -6}, {-1, -6}, {0, -6}, {1, -6},  {2, -5},
    {3, -5},  {4, -4},  {5, -4},  {5, -3},  {6, -2},  {6, -1},  {6, 0},  {6, 1},   {5, 2},
    {5, 3},   {4, 4},   {4, 5},   {3, 5},   {2, 6},   {1, 6},   {0, 6},  {-1, 6},  {-2, 5},
    {-3, 5},  {-4, 4},  {-5, 4},  {-5, 3},  {-6, 2},  {-6, 1},  {-6, 0}, {-6, -1}, {-5, -2},
  },
  {
    {-4, 2},  {-4, 1},  {-4, 1},  {-4, 0}, {-4, -1}, {-4, -2}, {-4, -2}, {-3, -3}, {-3, -4},
    {-2, -4}, {-1, -4}, {-1, -4}, {0, -4}, {1, -4},  {2, -4},  {2, -4},  {3, -3},  {4, -3},
    {4, -2},  {4, -1},  {4, -1},  {4, 0},  {4, 1},   {4, 2},   {4, 2},   {3, 3},   {3, 4},
    {2, 4},   {1, 4},   {1, 4},   {0, 4},  {-1, 4},  {-2, 4},  {-2, 4},  {-3, 3},  {-4, 3},
  },
  {
    {-3, 7},  {-4, 6},  {-5, 6},  {-6, 5},  {-7, 3},  {-7, 2},  {-8, 1},  {-8, 0}, {-7, -2},
    {-7, -3}, {-6, -4}, {-6, -5}, {-5, -6}, {-3, -7}, {-2, -7}, {-1, -8}, {0, -8}, {2, -7},
    {3, -7},  {4, -6},  {5, -6},  {6, -5},  {7, -3},  {7, -2},  {8, -1},  {8, 0},  {7, 2},
    {7, 3},   {6, 4},   {6, 5},   {5, 6},   {3, 7},   {2, 7},   {1, 8},   {0, 8},  {-2, 7},
  },
  {
    {-10, -12}, {-8, -14}, {-5, -15}, {-3, -15},  {0, -16}, {3, -15}, {5, -15}, {8, -14},
    {10, -12},  {12, -10}, {14, -8},  {15, -5},   {15, -3}, {16, 0},  {15, 3},  {15, 5},
    {14, 8},    {12, 10},  {10, 12},  {8, 14},    {5, 15},  {3, 15},  {0, 16},  {-3, 15},
    {-5, 15},   {-8, 14},  {-10, 12}, {-12, 10},  {-14, 8}, {-15, 5}, {-15, 3}, {-16, 0},
    {-15, -3},  {-15, -5}, {-14, -8}, {-12, -10},
  },
  {
    {-6, 11},  {-8, 10},  {-9, 8},  {-11, 7},  {-12, 5},  {-12, 2},  {-13, 0}, {-12, -2}, {-12, -4},
    {-11, -6}, {-10, -8}, {-8, -9}, {-7, -11}, {-5, -12}, {-2, -12}, {0, -13}, {2, -12},  {4, -12},
    {6, -11},  {8, -10},  {9, -8},  {11, -7},  {12, -5},  {12, -2},  {13, 0},  {12, 2},   {12, 4},
    {11, 6},   {10, 8},   {8, 9},   {7, 11},   {5, 12},   {2, 12},   {0, 13},  {-2, 12},  {-4, 12},
  },
  {
    {5, -12},  {7, -11},  {9, -10},  {10, -8},  {12, -6},  {12, -4},  {13, -2},  {13, 1},
    {13, 3},   {12, 5},   {11, 7},   {10, 9},   {8, 10},   {6, 12},   {4, 12},   {2, 13},
    {-1, 13},  {-3, 13},  {-5, 12},  {-7, 11},  {-9, 10},  {-10, 8},  {-12, 6},  {-12, 4},
    {-13, 2},  {-13, -1}, {-13, -3}, {-12, -5}, {-11, -7}, {-10, -9}, {-8, -10}, {-6, -12},
    {-4, -12}, {-2, -13}, {1, -13},  {3, -13},
  },
  {
    {6, -7},  {7, -6},  {8, -5},  {9, -3},  {9, -2},  {9, 0},  {9, 2},   {9, 3},   {8, 5},
    {7, 6},   {6, 7},   {5, 8},   {3, 9},   {2, 9},   {0, 9},  {-2, 9},  {-3, 9},  {-5, 8},
    {-6, 7},  {-7, 6},  {-8, 5},  {-9, 3},  {-9, 2},  {-9, 0}, {-9, -2}, {-9, -3}, {-8, -5},
    {-7, -6}, {-6, -7}, {-5, -8}, {-3, -9}, {-2, -9}, {0, -9}, {2, -9},  {3, -9},  {5, -8},
  },
  {
    {5, -6},  {6, -5},  {7, -4},  {7, -3},  {8, -1},  {8, 0},  {8, 1},   {7, 3},   {7, 4},
    {6, 5},   {5, 6},   {4, 7},   {3, 7},   {1, 8},   {0, 8},  {-1, 8},  {-3, 7},  {-4, 7},
    {-5, 6},  {-6, 5},  {-7, 4},  {-7, 3},  {-8, 1},  {-8, 0}, {-8, -1}, {-7, -3}, {-7, -4},
    {-6, -5}, {-5, -6}, {-4, -7}, {-3, -7}, {-1, -8}, {0, -8}, {1, -8},  {3, -7},  {4, -7},
  },
  {
    {7, -1},  {7, 0},  {7, 1},   {7, 3},   {6, 4},   {5, 5},   {4, 6},   {3, 6},   {2, 7},
    {1, 7},   {0, 7},  {-1, 7},  {-3, 7},  {-4, 6},  {-5, 5},  {-6, 4},  {-6, 3},  {-7, 2},
    {-7, 1},  {-7, 0}, {-7, -1}, {-7, -3}, {-6, -4}, {-5, -5}, {-4, -6}, {-3, -6}, {-2, -7},
    {-1, -7}, {0, -7}, {1, -7},  {3, -7},  {4, -6},  {5, -5},  {6, -4},  {6, -3},  {7, -2},
  },
  {
    {1, 0},  {1, 0},  {1, 0},  {1, 1},   {1, 1},   {1, 1},   {1, 1},   {0, 1},  {0, 1},
    {0, 1},  {0, 1},  {0, 1},  {-1, 1},  {-1, 1},  {-1, 1},  {-1, 1},  {-1, 0}, {-1, 0},
    {-1, 0}, {-1, 0}, {-1, 0}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {0, -1}, {0, -1},
    {0, -1}, {0, -1}, {0, -1}, {1, -1},  {1, -1},  {1, -1},  {1, -1},  {1, 0},  {1, 0},
  },
  {
    {4, -5},  {5, -4},  {5, -3},  {6, -2},  {6, -1},  {6, 0},  {6, 1},   {6, 2},   {6, 3},
    {5, 4},   {4, 5},   {3, 5},   {2, 6},   {1, 6},   {0, 6},  {-1, 6},  {-2, 6},  {-3, 6},
    {-4, 5},  {-5, 4},  {-5, 3},  {-6, 2},  {-6, 1},  {-6, 0}, {-6, -1}, {-6, -2}, {-6, -3},
    {-5, -4}, {-4, -5}, {-3, -5}, {-2, -6}, {-1, -6}, {0, -6}, {1, -6},  {2, -6},  {3, -6},
  },
  {
    {9, 11},   {7, 12},   {5, 13},   {2, 14},   {0, 14},   {-3, 14},  {-5, 13},  {-7, 12},
    {-9, 11},  {-11, 9},  {-12, 7},  {-13, 5},  {-14, 2},  {-14, 0},  {-14, -3}, {-13, -5},
    {-12, -7}, {-11, -9}, {-9, -11}, {-7, -12}, {-5, -13}, {-2, -14}, {0, -14},  {3, -14},
    {5, -13},  {7, -12},  {9, -11},  {11, -9},  {12, -7},  {13, -5},  {14, -2},  {14, 0},
    {14, 3},   {13, 5},   {12, 7},   {11, 9},
  },
  {
    {11, -13}, {13, -11}, {15, -8},  {16, -6},   {17, -3},   {17, 0},   {17, 3},   {16, 6},
    {15, 9},   {13, 11},  {11, 13},  {8, 15},    {6, 16},    {3, 17},   {0, 17},   {-3, 17},
    {-6, 16},  {-9, 15},  {-11, 13}, {-13, 11},  {-15, 8},   {-16, 6},  {-17, 3},  {-17, 0},
    {-17, -3}, {-16, -6}, {-15, -9}, {-13, -11}, {-11, -13}, {-8, -15}, {-6, -16}, {-3, -17},
    {0, -17},  {3, -17},  {6, -16},  {9, -15},
  },
  {
    {4, 7},   {3, 8},   {1, 8},   {0, 8},  {-1, 8},  {-3, 8},  {-4, 7},  {-5, 6},  {-6, 5},
    {-7, 4},  {-8, 3},  {-8, 1},  {-8, 0}, {-8, -1}, {-8, -3}, {-7, -4}, {-6, -5}, {-5, -6},
    {-4, -7}, {-3, -8}, {-1, -8}, {0, -8}, {1, -8},  {3, -8},  {4, -7},  {5, -6},  {6, -5},
    {7, -4},  {8, -3},  {8, -1},  {8, 0},  {8, 1},   {8, 3},   {7, 4},   {6, 5},   {5, 6},
  },
  {
    {4, 12},   {2, 13},   {0, 13},  {-3, 12},  {-5, 12},  {-7, 11},  {-8, 9},  {-10, 8},  {-11, 6},
    {-12, 4},  {-13, 2},  {-13, 0}, {-12, -3}, {-12, -5}, {-11, -7}, {-9, -8}, {-8, -10}, {-6, -11},
    {-4, -12}, {-2, -13}, {0, -13}, {3, -12},  {5, -12},  {7, -11},  {8, -9},  {10, -8},  {11, -6},
    {12, -4},  {13, -2},  {13, 0},  {12, 3},   {12, 5},   {11, 7},   {9, 8},   {8, 10},   {6, 11},
  },
  {
    {2, -1},  {2, -1},  {2, 0},  {2, 0},  {2, 1},   {2, 1},   {2, 1},   {2, 2},   {1, 2},
    {1, 2},   {1, 2},   {0, 2},  {0, 2},  {-1, 2},  {-1, 2},  {-1, 2},  {-2, 2},  {-2, 1},
    {-2, 1},  {-2, 1},  {-2, 0}, {-2, 0}, {-2, -1}, {-2, -1}, {-2, -1}, {-2, -2}, {-1, -2},
    {-1, -2}, {-1, -2}, {0, -2}, {0, -2}, {1, -2},  {1, -2},  {1, -2},  {2, -2},  {2, -1},
  },
  {
    {4, 4},   {3, 5},   {2, 5},   {1, 5},   {0, 6},  {0, 6},  {-1, 5},  {-2, 5},  {-3, 5},
    {-4, 4},  {-5, 3},  {-5, 2},  {-5, 1},  {-6, 0}, {-6, 0}, {-5, -1}, {-5, -2}, {-5, -3},
    {-4, -4}, {-3, -5}, {-2, -5}, {-1, -5}, {0, -6}, {0, -6}, {1, -5},  {2, -5},  {3, -5},
    {4, -4},  {5, -3},  {5, -2},  {5, -1},  {6, 0},  {6, 0},  {5, 1},   {5, 2},   {5, 3},
  },
  {
    {-4, -12}, {-2, -13}, {0, -13}, {3, -12},  {5, -12},  {7, -11},  {8, -9},  {10, -8},  {11, -6},
    {12, -4},  {13, -2},  {13, 0},  {12, 3},   {12, 5},   {11, 7},   {9, 8},   {8, 10},   {6, 11},
    {4, 12},   {2, 13},   {0, 13},  {-3, 12},  {-5, 12},  {-7, 11},  {-8, 9},  {-10, 8},  {-11, 6},
    {-12, 4},  {-13, 2},  {-13, 0}, {-12, -3}, {-12, -5}, {-11, -7}, {-9, -8}, {-8, -10}, {-6, -11},
  },
  {
    {-2, 7},  {-3, 7},  {-4, 6},  {-5, 5},  {-6, 4},  {-7, 3},  {-7, 2},  {-7, 1},  {-7, -1},
    {-7, -2}, {-7, -3}, {-6, -4}, {-5, -5}, {-4, -6}, {-3, -7}, {-2, -7}, {-1, -7}, {1, -7},
    {2, -7},  {3, -7},  {4, -6},  {5, -5},  {6, -4},  {7, -3},  {7, -2},  {7, -1},  {7, 1},
    {7, 2},   {7, 3},   {6, 4},   {5, 5},   {4, 6},   {3, 7},   {2, 7},   {1, 7},   {-1, 7},
  },
  {
    {-8, -5}, {-7, -6}, {-6, -7}, {-4, -8}, {-3, -9}, {-1, -9}, {0, -9}, {2, -9},  {4, -9},
    {5, -8},  {6, -7},  {7, -6},  {8, -4},  {9, -3},  {9, -1},  {9, 0},  {9, 2},   {9, 4},
    {8, 5},   {7, 6},   {6, 7},   {4, 8},   {3, 9},   {1, 9},   {0, 9},  {-2, 9},  {-4, 9},
    {-5, 8},  {-6, 7},  {-7, 6},  {-8, 4},  {-9, 3},  {-9, 1},  {-9, 0}, {-9, -2}, {-9, -4},
  },
  {
    {-7, -10}, {-5, -11}, {-3, -12}, {-1, -12}, {1, -12}, {3, -12}, {5, -11}, {7, -10},
    {9, -9},   {10, -7},  {11, -5},  {12, -3},  {12, -1}, {12, 1},  {12, 3},  {11, 5},
    {10, 7},   {9, 9},    {7, 10},   {5, 11},   {3, 12},  {1, 12},  {-1, 12}, {-3, 12},
    {-5, 11},  {-7, 10},  {-9, 9},   {-10, 7},  {-11, 5}, {-12, 3}, {-12, 1}, {-12, -1},
    {-12, -3}, {-11, -5}, {-10, -7}, {-9, -9},
  },
  {
    {4, 11},   {2, 12},   {0, 12},  {-2, 12},  {-4, 11},  {-6, 10},  {-8, 9},  {-9, 8},  {-10, 6},
    {-11, 4},  {-12, 2},  {-12, 0}, {-12, -2}, {-11, -4}, {-10, -6}, {-9, -8}, {-8, -9}, {-6, -10},
    {-4, -11}, {-2, -12}, {0, -12}, {2, -12},  {4, -11},  {6, -10},  {8, -9},  {9, -8},  {10, -6},
    {11, -4},  {12, -2},  {12, 0},  {12, 2},   {11, 4},   {10, 6},   {9, 8},   {8, 9},   {6, 10},
  },
  {
    {9, 12},   {7, 13},    {4, 14},   {2, 15},   {-1, 15},  {-3, 15},  {-6, 14},  {-8, 13},
    {-10, 11}, {-12, 9},   {-13, 7},  {-14, 4},  {-15, 2},  {-15, -1}, {-15, -3}, {-14, -6},
    {-13, -8}, {-11, -10}, {-9, -12}, {-7, -13}, {-4, -14}, {-2, -15}, {1, -15},  {3, -15},
    {6, -14},  {8, -13},   {10, -11}, {12, -9},  {13, -7},  {14, -4},  {15, -2},  {15, 1},
    {15, 3},   {14, 6},    {13, 8},   {11, 10},
  },
  {
    {0, -8}, {1, -8},  {3, -8},  {4, -7},  {5, -6},  {6, -5},  {7, -4},  {8, -3},  {8, -1},
    {8, 0},  {8, 1},   {8, 3},   {7, 4},   {6, 5},   {5, 6},   {4, 7},   {3, 8},   {1, 8},
    {0, 8},  {-1, 8},  {-3, 8},  {-4, 7},  {-5, 6},  {-6, 5},  {-7, 4},  {-8, 3},  {-8, 1},
    {-8, 0}, {-8, -1}, {-8, -3}, {-7, -4}, {-6, -5}, {-5, -6}, {-4, -7}, {-3, -8}, {-1, -8},
  },
  {
    {1, -13},  {3, -13},  {5, -12},  {7, -11},  {9, -9},   {11, -8},  {12, -6},  {13, -4},
    {13, -1},  {13, 1},   {13, 3},   {12, 5},   {11, 7},   {9, 9},    {8, 11},   {6, 12},
    {4, 13},   {1, 13},   {-1, 13},  {-3, 13},  {-5, 12},  {-7, 11},  {-9, 9},   {-11, 8},
    {-12, 6},  {-13, 4},  {-13, 1},  {-13, -1}, {-13, -3}, {-12, -5}, {-11, -7}, {-9, -9},
    {-8, -11}, {-6, -12}, {-4, -13}, {-1, -13},
  },
  {
    {-13, -2}, {-12, -4}, {-12, -6}, {-10, -8}, {-9, -10}, {-7, -11}, {-5, -12}, {-3, -13},
    {0, -13},  {2, -13},  {4, -12},  {6, -12},  {8, -10},  {10, -9},  {11, -7},  {12, -5},
    {13, -3},  {13, 0},   {13, 2},   {12, 4},   {12, 6},   {10, 8},   {9, 10},   {7, 11},
    {5, 12},   {3, 13},   {0, 13},   {-2, 13},  {-4, 12},  {-6, 12},  {-8, 10},  {-10, 9},
    {-11, 7},  {-12, 5},  {-13, 3},  {-13, 0},
  },
  {
    {-8, 2},  {-8, 1},  {-8, -1}, {-8, -2}, {-7, -4}, {-7, -5}, {-6, -6}, {-5, -7}, {-3, -8},
    {-2, -8}, {-1, -8}, {1, -8},  {2, -8},  {4, -7},  {5, -7},  {6, -6},  {7, -5},  {8, -3},
    {8, -2},  {8, -1},  {8, 1},   {8, 2},   {7, 4},   {7, 5},   {6, 6},   {5, 7},   {3, 8},
    {2, 8},   {1, 8},   {-1, 8},  {-2, 8},  {-4, 7},  {-5, 7},  {-6, 6},  {-7, 5},  {-8, 3},
  },
  {
    {-3, -2}, {-3, -2}, {-2, -3}, {-2, -3}, {-1, -3}, {0, -4}, {0, -4}, {1, -4},  {1, -3},
    {2, -3},  {2, -3},  {3, -2},  {3, -2},  {3, -1},  {4, 0},  {4, 0},  {4, 1},   {3, 1},
    {3, 2},   {3, 2},   {2, 3},   {2, 3},   {1, 3},   {0, 4},  {0, 4},  {-1, 4},  {-1, 3},
    {-2, 3},  {-2, 3},  {-3, 2},  {-3, 2},  {-3, 1},  {-4, 0}, {-4, 0}, {-4, -1}, {-3, -1},
  },
  {
    {-2, 3},  {-2, 3},  {-3, 2},  {-3, 2},  {-3, 1},  {-4, 0}, {-4, 0}, {-4, -1}, {-3, -1},
    {-3, -2}, {-3, -2}, {-2, -3}, {-2, -3}, {-1, -3}, {0, -4}, {0, -4}, {1, -4},  {1, -3},
    {2, -3},  {2, -3},  {3, -2},  {3, -2},  {3, -1},  {4, 0},  {4, 0},  {4, 1},   {3, 1},
    {3, 2},   {3, 2},   {2, 3},   {2, 3},   {1, 3},   {0, 4},  {0, 4},  {-1, 4},  {-1, 3},
  },
  {
    {-6, 9},  {-7, 8},  {-9, 6},  {-10, 5},  {-10, 3},  {-11, 1},  {-11, -1}, {-11, -3}, {-10, -4},
    {-9, -6}, {-8, -7}, {-6, -9}, {-5, -10}, {-3, -10}, {-1, -11}, {1, -11},  {3, -11},  {4, -10},
    {6, -9},  {7, -8},  {9, -6},  {10, -5},  {10, -3},  {11, -1},  {11, 1},   {11, 3},   {10, 4},
    {9, 6},   {8, 7},   {6, 9},   {5, 10},   {3, 10},   {1, 11},   {-1, 11},  {-3, 11},  {-4, 10},
  },
  {
    {-4, -9}, {-2, -10}, {-1, -10}, {1, -10},  {3, -9},  {4, -9},  {6, -8},  {7, -7},  {8, -6},
    {9, -4},  {10, -2},  {10, -1},  {10, 1},   {9, 3},   {9, 4},   {8, 6},   {7, 7},   {6, 8},
    {4, 9},   {2, 10},   {1, 10},   {-1, 10},  {-3, 9},  {-4, 9},  {-6, 8},  {-7, 7},  {-8, 6},
    {-9, 4},  {-10, 2},  {-10, 1},  {-10, -1}, {-9, -3}, {-9, -4}, {-8, -6}, {-7, -7}, {-6, -8},
  },
  {
    {8, 12},   {6, 13},    {3, 14},   {1, 14},   {-2, 14},  {-4, 14},  {-6, 13},  {-9, 12},
    {-10, 10}, {-12, 8},   {-13, 6},  {-14, 3},  {-14, 1},  {-14, -2}, {-14, -4}, {-13, -6},
    {-12, -9}, {-10, -10}, {-8, -12}, {-6, -13}, {-3, -14}, {-1, -14}, {2, -14},  {4, -14},
    {6, -13},  {9, -12},   {10, -10}, {12, -8},  {13, -6},  {14, -3},  {14, -1},  {14, 2},
    {14, 4},   {13, 6},    {12, 9},   {10, 10},
  },
  {
    {10, 7},   {9, 9},    {7, 10},   {5, 11},  {3, 12},   {1, 12},   {-1, 12},  {-3, 12},
    {-5, 11},  {-7, 10},  {-9, 9},   {-10, 7}, {-11, 5},  {-12, 3},  {-12, 1},  {-12, -1},
    {-12, -3}, {-11, -5}, {-10, -7}, {-9, -9}, {-7, -10}, {-5, -11}, {-3, -12}, {-1, -12},
    {1, -12},  {3, -12},  {5, -11},  {7, -10}, {9, -9},   {10, -7},  {11, -5},  {12, -3},
    {12, -1},  {12, 1},   {12, 3},   {11, 5},
  },
  {
    {0, 9},  {-2, 9},  {-3, 8},  {-5, 8},  {-6, 7},  {-7, 6},  {-8, 5},  {-8, 3},  {-9, 2},
    {-9, 0}, {-9, -2}, {-8, -3}, {-8, -5}, {-7, -6}, {-6, -7}, {-5, -8}, {-3, -8}, {-2, -9},
    {0, -9}, {2, -9},  {3, -8},  {5, -8},  {6, -7},  {7, -6},  {8, -5},  {8, -3},  {9, -2},
    {9, 0},  {9, 2},   {8, 3},   {8, 5},   {7, 6},   {6, 7},   {5, 8},   {3, 8},   {2, 9},
  },
  {
    {1, 3},   {0, 3},  {0, 3},  {-1, 3},  {-1, 3},  {-2, 3},  {-2, 2},  {-2, 2},  {-3, 2},
    {-3, 1},  {-3, 0}, {-3, 0}, {-3, -1}, {-3, -1}, {-3, -2}, {-2, -2}, {-2, -2}, {-2, -3},
    {-1, -3}, {0, -3}, {0, -3}, {1, -3},  {1, -3},  {2, -3},  {2, -2},  {2, -2},  {3, -2},
    {3, -1},  {3, 0},  {3, 0},  {3, 1},   {3, 1},   {3, 2},   {2, 2},   {2, 2},   {2, 3},
  },
  {
    {7, -5},  {8, -4},  {8, -2},  {9, -1},  {9, 1},   {8, 2},   {8, 4},   {7, 5},   {6, 6},
    {5, 7},   {4, 8},   {2, 8},   {1, 9},   {-1, 9},  {-2, 8},  {-4, 8},  {-5, 7},  {-6, 6},
    {-7, 5},  {-8, 4},  {-8, 2},  {-9, 1},  {-9, -1}, {-8, -2}, {-8, -4}, {-7, -5}, {-6, -6},
    {-5, -7}, {-4, -8}, {-2, -8}, {-1, -9}, {1, -9},  {2, -8},  {4, -8},  {5, -7},  {6, -6},
  },
  {
    {11, -10}, {13, -8},  {14, -6},  {15, -3},   {15, -1},  {15, 2},   {14, 5},   {13, 7},
    {12, 9},   {10, 11},  {8, 13},   {6, 14},    {3, 15},   {1, 15},   {-2, 15},  {-5, 14},
    {-7, 13},  {-9, 12},  {-11, 10}, {-13, 8},   {-14, 6},  {-15, 3},  {-15, 1},  {-15, -2},
    {-14, -5}, {-13, -7}, {-12, -9}, {-10, -11}, {-8, -13}, {-6, -14}, {-3, -15}, {-1, -15},
    {2, -15},  {5, -14},  {7, -13},  {9, -12},
  },
  {
    {-13, -6}, {-12, -8}, {-10, -10}, {-8, -12}, {-6, -13}, {-4, -14}, {-1, -14}, {1, -14},
    {4, -14},  {6, -13},  {8, -12},   {10, -10}, {12, -8},  {13, -6},  {14, -4},  {14, -1},
    {14, 1},   {14, 4},   {13, 6},    {12, 8},   {10, 10},  {8, 12},   {6, 13},   {4, 14},
    {1, 14},   {-1, 14},  {-4, 14},   {-6, 13},  {-8, 12},  {-10, 10}, {-12, 8},  {-13, 6},
    {-14, 4},  {-14, 1},  {-14, -1},  {-14, -4},
  },
  {
    {-11, 0}, {-11, -2}, {-10, -4}, {-10, -6}, {-8, -7}, {-7, -8}, {-6, -10}, {-4, -10}, {-2, -11},
    {0, -11}, {2, -11},  {4, -10},  {6, -10},  {7, -8},  {8, -7},  {10, -6},  {10, -4},  {11, -2},
    {11, 0},  {11, 2},   {10, 4},   {10, 6},   {8, 7},   {7, 8},   {6, 10},   {4, 10},   {2, 11},
    {0, 11},  {-2, 11},  {-4, 10},  {-6, 10},  {-7, 8},  {-8, 7},  {-10, 6},  {-10, 4},  {-11, 2},
  },
  {
    {10, 7},   {9, 9},    {7, 10},   {5, 11},  {3, 12},   {1, 12},   {-1, 12},  {-3, 12},
    {-5, 11},  {-7, 10},  {-9, 9},   {-10, 7}, {-11, 5},  {-12, 3},  {-12, 1},  {-12, -1},
    {-12, -3}, {-11, -5}, {-10, -7}, {-9, -9}, {-7, -10}, {-5, -11}, {-3, -12}, {-1, -12},
    {1, -12},  {3, -12},  {5, -11},  {7, -10}, {9, -9},   {10, -7},  {11, -5},  {12, -3},
    {12, -1},  {12, 1},   {12, 3},   {11, 5},
  },
  {
    {12, 1},   {12, 3},   {11, 5},   {10, 7},   {9, 8},    {7, 10},   {5, 11},  {3, 12},
    {1, 12},   {-1, 12},  {-3, 12},  {-5, 11},  {-7, 10},  {-8, 9},   {-10, 7}, {-11, 5},
    {-12, 3},  {-12, 1},  {-12, -1}, {-12, -3}, {-11, -5}, {-10, -7}, {-9, -8}, {-7, -10},
    {-5, -11}, {-3, -12}, {-1, -12}, {1, -12},  {3, -12},  {5, -11},  {7, -10}, {8, -9},
    {10, -7},  {11, -5},  {12, -3},  {12, -1},
  },
  {
    {-6, -3}, {-5, -4}, {-5, -5}, {-4, -6}, {-3, -6}, {-2, -7}, {0, -7}, {1, -7},  {2, -6},
    {3, -6},  {4, -5},  {5, -5},  {6, -4},  {6, -3},  {7, -2},  {7, 0},  {7, 1},   {6, 2},
    {6, 3},   {5, 4},   {5, 5},   {4, 6},   {3, 6},   {2, 7},   {0, 7},  {-1, 7},  {-2, 6},
    {-3, 6},  {-4, 5},  {-5, 5},  {-6, 4},  {-6, 3},  {-7, 2},  {-7, 0}, {-7, -1}, {-6, -2},
  },
  {
    {-6, 12},  {-8, 11},  {-10, 9},  {-11, 7},  {-12, 5},  {-13, 3},  {-13, 1},  {-13, -2},
    {-13, -4}, {-12, -6}, {-11, -8}, {-9, -10}, {-7, -11}, {-5, -12}, {-3, -13}, {-1, -13},
    {2, -13},  {4, -13},  {6, -12},  {8, -11},  {10, -9},  {11, -7},  {12, -5},  {13, -3},
    {13, -1},  {13, 2},   {13, 4},   {12, 6},   {11, 8},   {9, 10},   {7, 11},   {5, 12},
    {3, 13},   {1, 13},   {-2, 13},  {-4, 13},
  },
  {
    {10, -9},  {11, -7},  {12, -5},  {13, -3},  {13, 0},   {13, 2},   {13, 4},   {12, 6},
    {11, 8},   {9, 10},   {7, 11},   {5, 12},   {3, 13},   {0, 13},   {-2, 13},  {-4, 13},
    {-6, 12},  {-8, 11},  {-10, 9},  {-11, 7},  {-12, 5},  {-13, 3},  {-13, 0},  {-13, -2},
    {-13, -4}, {-12, -6}, {-11, -8}, {-9, -10}, {-7, -11}, {-5, -12}, {-3, -13}, {0, -13},
    {2, -13},  {4, -13},  {6, -12},  {8, -11},
  },
  {
    {12, -4},  {13, -2},  {13, 0},  {12, 3},   {12, 5},   {11, 7},   {9, 8},   {8, 10},   {6, 11},
    {4, 12},   {2, 13},   {0, 13},  {-3, 12},  {-5, 12},  {-7, 11},  {-8, 9},  {-10, 8},  {-11, 6},
    {-12, 4},  {-13, 2},  {-13, 0}, {-12, -3}, {-12, -5}, {-11, -7}, {-9, -8}, {-8, -10}, {-6, -11},
    {-4, -12}, {-2, -13}, {0, -13}, {3, -12},  {5, -12},  {7, -11},  {8, -9},  {10, -8},  {11, -6},
  },
  {
    {-13, 8},   {-14, 6},  {-15, 3},  {-15, 0},  {-15, -2}, {-14, -5}, {-13, -7}, {-12, -9},
    {-10, -11}, {-8, -13}, {-6, -14}, {-3, -15}, {0, -15},  {2, -15},  {5, -14},  {7, -13},
    {9, -12},   {11, -10}, {13, -8},  {14, -6},  {15, -3},  {15, 0},   {15, 2},   {14, 5},
    {13, 7},    {12, 9},   {10, 11},  {8, 13},   {6, 14},   {3, 15},   {0, 15},   {-2, 15},
    {-5, 14},   {-7, 13},  {-9, 12},  {-11, 10},
  },
  {
    {-8, -12}, {-6, -13}, {-3, -14}, {-1, -14},  {2, -14}, {4, -14}, {6, -13}, {9, -12},
    {10, -10}, {12, -8},  {13, -6},  {14, -3},   {14, -1}, {14, 2},  {14, 4},  {13, 6},
    {12, 9},   {10, 10},  {8, 12},   {6, 13},    {3, 14},  {1, 14},  {-2, 14}, {-4, 14},
    {-6, 13},  {-9, 12},  {-10, 10}, {-12, 8},   {-13, 6}, {-14, 3}, {-14, 1}, {-14, -2},
    {-14, -4}, {-13, -6}, {-12, -9}, {-10, -10},
  },
  {
    {-13, 0},  {-13, -2}, {-12, -4}, {-11, -7}, {-10, -8}, {-8, -10}, {-7, -11}, {-4, -12},
    {-2, -13}, {0, -13},  {2, -13},  {4, -12},  {7, -11},  {8, -10},  {10, -8},  {11, -7},
    {12, -4},  {13, -2},  {13, 0},   {13, 2},   {12, 4},   {11, 7},   {10, 8},   {8, 10},
    {7, 11},   {4, 12},   {2, 13},   {0, 13},   {-2, 13},  {-4, 12},  {-7, 11},  {-8, 10},
    {-10, 8},  {-11, 7},  {-12, 4},  {-13, 2},
  },
  {
    {-8, -4}, {-7, -5}, {-6, -6}, {-5, -7}, {-4, -8}, {-2, -9}, {-1, -9}, {1, -9},  {3, -9},
    {4, -8},  {5, -7},  {6, -6},  {7, -5},  {8, -4},  {9, -2},  {9, -1},  {9, 1},   {9, 3},
    {8, 4},   {7, 5},   {6, 6},   {5, 7},   {4, 8},   {2, 9},   {1, 9},   {-1, 9},  {-3, 9},
    {-4, 8},  {-5, 7},  {-6, 6},  {-7, 5},  {-8, 4},  {-9, 2},  {-9, 1},  {-9, -1}, {-9, -3},
  },
  {
    {3, 3},   {2, 3},   {2, 4},   {1, 4},   {0, 4},  {0, 4},  {-1, 4},  {-2, 4},  {-2, 3},
    {-3, 3},  {-3, 2},  {-4, 2},  {-4, 1},  {-4, 0}, {-4, 0}, {-4, -1}, {-4, -2}, {-3, -2},
    {-3, -3}, {-2, -3}, {-2, -4}, {-1, -4}, {0, -4}, {0, -4}, {1, -4},  {2, -4},  {2, -3},
    {3, -3},  {3, -2},  {4, -2},  {4, -1},  {4, 0},  {4, 0},  {4, 1},   {4, 2},   {3, 2},
  },
  {
    {7, 8},   {6, 9},   {4, 10},   {2, 10},   {0, 11},  {-2, 11},  {-3, 10},  {-5, 9},  {-7, 8},
    {-8, 7},  {-9, 6},  {-10, 4},  {-10, 2},  {-11, 0}, {-11, -2}, {-10, -3}, {-9, -5}, {-8, -7},
    {-7, -8}, {-6, -9}, {-4, -10}, {-2, -10}, {0, -11}, {2, -11},  {3, -10},  {5, -9},  {7, -8},
    {8, -7},  {9, -6},  {10, -4},  {10, -2},  {11, 0},  {11, 2},   {10, 3},   {9, 5},   {8, 7},
  },
  {
    {5, 7},   {4, 8},   {2, 8},   {1, 9},   {-1, 9},  {-2, 8},  {-4, 8},  {-5, 7},  {-6, 6},
    {-7, 5},  {-8, 4},  {-8, 2},  {-9, 1},  {-9, -1}, {-8, -2}, {-8, -4}, {-7, -5}, {-6, -6},
    {-5, -7}, {-4, -8}, {-2, -8}, {-1, -9}, {1, -9},  {2, -8},  {4, -8},  {5, -7},  {6, -6},
    {7, -5},  {8, -4},  {8, -2},  {9, -1},  {9, 1},   {8, 2},   {8, 4},   {7, 5},   {6, 6},
  },
  {
    {10, -7},  {11, -5},  {12, -3}, {12, -1},  {12, 1},   {12, 3},   {11, 5},   {10, 7},
    {9, 9},    {7, 10},   {5, 11},  {3, 12},   {1, 12},   {-1, 12},  {-3, 12},  {-5, 11},
    {-7, 10},  {-9, 9},   {-10, 7}, {-11, 5},  {-12, 3},  {-12, 1},  {-12, -1}, {-12, -3},
    {-11, -5}, {-10, -7}, {-9, -9}, {-7, -10}, {-5, -11}, {-3, -12}, {-1, -12}, {1, -12},
    {3, -12},  {5, -11},  {7, -10}, {9, -9},
  },
  {
    {-1, 7},  {-2, 7},  {-3, 6},  {-4, 6},  {-5, 5},  {-6, 4},  {-7, 3},  {-7, 1},  {-7, 0},
    {-7, -1}, {-7, -2}, {-6, -3}, {-6, -4}, {-5, -5}, {-4, -6}, {-3, -7}, {-1, -7}, {0, -7},
    {1, -7},  {2, -7},  {3, -6},  {4, -6},  {5, -5},  {6, -4},  {7, -3},  {7, -1},  {7, 0},
    {7, 1},   {7, 2},   {6, 3},   {6, 4},   {5, 5},   {4, 6},   {3, 7},   {1, 7},   {0, 7},
  },
  {
    {1, -12},  {3, -12},  {5, -11},  {7, -10},  {8, -9},   {10, -7},  {11, -5},  {12, -3},
    {12, -1},  {12, 1},   {12, 3},   {11, 5},   {10, 7},   {9, 8},    {7, 10},   {5, 11},
    {3, 12},   {1, 12},   {-1, 12},  {-3, 12},  {-5, 11},  {-7, 10},  {-8, 9},   {-10, 7},
    {-11, 5},  {-12, 3},  {-12, 1},  {-12, -1}, {-12, -3}, {-11, -5}, {-10, -7}, {-9, -8},
    {-7, -10}, {-5, -11}, {-3, -12}, {-1, -12},
  },
  {
    {3, -10},  {5, -9},  {6, -8},  {8, -7},  {9, -6},  {10, -4},  {10, -2},  {10, -1},  {10, 1},
    {10, 3},   {9, 5},   {8, 6},   {7, 8},   {6, 9},   {4, 10},   {2, 10},   {1, 10},   {-1, 10},
    {-3, 10},  {-5, 9},  {-6, 8},  {-8, 7},  {-9, 6},  {-10, 4},  {-10, 2},  {-10, 1},  {-10, -1},
    {-10, -3}, {-9, -5}, {-8, -6}, {-7, -8}, {-6, -9}, {-4, -10}, {-2, -10}, {-1, -10}, {1, -10},
  },
  {
    {5, 6},   {4, 7},   {3, 7},   {1, 8},   {0, 8},  {-1, 8},  {-3, 7},  {-4, 7},  {-5, 6},
    {-6, 5},  {-7, 4},  {-7, 3},  {-8, 1},  {-8, 0}, {-8, -1}, {-7, -3}, {-7, -4}, {-6, -5},
    {-5, -6}, {-4, -7}, {-3, -7}, {-1, -8}, {0, -8}, {1, -8},  {3, -7},  {4, -7},  {5, -6},
    {6, -5},  {7, -4},  {7, -3},  {8, -1},  {8, 0},  {8, 1},   {7, 3},   {7, 4},   {6, 5},
  },
  {
    {2, -4},  {3, -4},  {3, -3},  {4, -2},  {4, -2},  {4, -1},  {4, 0},  {4, 1},   {4, 1},
    {4, 2},   {4, 3},   {3, 3},   {2, 4},   {2, 4},   {1, 4},   {0, 4},  {-1, 4},  {-1, 4},
    {-2, 4},  {-3, 4},  {-3, 3},  {-4, 2},  {-4, 2},  {-4, 1},  {-4, 0}, {-4, -1}, {-4, -1},
    {-4, -2}, {-4, -3}, {-3, -3}, {-2, -4}, {-2, -4}, {-1, -4}, {0, -4}, {1, -4},  {1, -4},
  },
  {
    {3, -10},  {5, -9},  {6, -8},  {8, -7},  {9, -6},  {10, -4},  {10, -2},  {10, -1},  {10, 1},
    {10, 3},   {9, 5},   {8, 6},   {7, 8},   {6, 9},   {4, 10},   {2, 10},   {1, 10},   {-1, 10},
    {-3, 10},  {-5, 9},  {-6, 8},  {-8, 7},  {-9, 6},  {-10, 4},  {-10, 2},  {-10, 1},  {-10, -1},
    {-10, -3}, {-9, -5}, {-8, -6}, {-7, -8}, {-6, -9}, {-4, -10}, {-2, -10}, {-1, -10}, {1, -10},
  },
  {
    {-13, 0},  {-13, -2}, {-12, -4}, {-11, -7}, {-10, -8}, {-8, -10}, {-7, -11}, {-4, -12},
    {-2, -13}, {0, -13},  {2, -13},  {4, -12},  {7, -11},  {8, -10},  {10, -8},  {11, -7},
    {12, -4},  {13, -2},  {13, 0},   {13, 2},   {12, 4},   {11, 7},   {10, 8},   {8, 10},
    {7, 11},   {4, 12},   {2, 13},   {0, 13},   {-2, 13},  {-4, 12},  {-7, 11},  {-8, 10},
    {-10, 8},  {-11, 7},  {-12, 4},  {-13, 2},
  },
  {
    {-13, 5},  {-14, 3},  {-14, 0},  {-14, -2}, {-13, -5}, {-12, -7}, {-11, -9}, {-9, -11},
    {-7, -12}, {-5, -13}, {-3, -14}, {0, -14},  {2, -14},  {5, -13},  {7, -12},  {9, -11},
    {11, -9},  {12, -7},  {13, -5},  {14, -3},  {14, 0},   {14, 2},   {13, 5},   {12, 7},
    {11, 9},   {9, 11},   {7, 12},   {5, 13},   {3, 14},   {0, 14},   {-2, 14},  {-5, 13},
    {-7, 12},  {-9, 11},  {-11, 9},  {-12, 7},
  },
  {
    {-13, -7}, {-12, -9}, {-10, -11}, {-8, -13}, {-5, -14}, {-3, -14}, {0, -15}, {2, -15},
    {5, -14},  {7, -13},  {9, -12},   {11, -10}, {13, -8},  {14, -5},  {14, -3}, {15, 0},
    {15, 2},   {14, 5},   {13, 7},    {12, 9},   {10, 11},  {8, 13},   {5, 14},  {3, 14},
    {0, 15},   {-2, 15},  {-5, 14},   {-7, 13},  {-9, 12},  {-11, 10}, {-13, 8}, {-14, 5},
    {-14, 3},  {-15, 0},  {-15, -2},  {-14, -5},
  },
  {
    {-12, 12},  {-14, 10},  {-15, 7},   {-16, 4},  {-17, 1},  {-17, -1}, {-16, -4}, {-15, -7},
    {-14, -10}, {-12, -12}, {-10, -14}, {-7, -15}, {-4, -16}, {-1, -17}, {1, -17},  {4, -16},
    {7, -15},   {10, -14},  {12, -12},  {14, -10}, {15, -7},  {16, -4},  {17, -1},  {17, 1},
    {16, 4},    {15, 7},    {14, 10},   {12, 12},  {10, 14},  {7, 15},   {4, 16},   {1, 17},
    {-1, 17},   {-4, 16},   {-7, 15},   {-10, 14},
  },
  {
    {-13, 3},  {-13, 1},  {-13, -2}, {-13, -4}, {-12, -6}, {-11, -8}, {-9, -10}, {-7, -11},
    {-5, -12}, {-3, -13}, {-1, -13}, {2, -13},  {4, -13},  {6, -12},  {8, -11},  {10, -9},
    {11, -7},  {12, -5},  {13, -3},  {13, -1},  {13, 2},   {13, 4},   {12, 6},   {11, 8},
    {9, 10},   {7, 11},   {5, 12},   {3, 13},   {1, 13},   {-2, 13},  {-4, 13},  {-6, 12},
    {-8, 11},  {-10, 9},  {-11, 7},  {-12, 5},
  },
  {
    {-11, 8},  {-12, 6},  {-13, 4},  {-14, 1},  {-14, -1}, {-13, -3}, {-12, -6}, {-11, -8},
    {-10, -9}, {-8, -11}, {-6, -12}, {-4, -13}, {-1, -14}, {1, -14},  {3, -13},  {6, -12},
    {8, -11},  {9, -10},  {11, -8},  {12, -6},  {13, -4},  {14, -1},  {14, 1},   {13, 3},
    {12, 6},   {11, 8},   {10, 9},   {8, 11},   {6, 12},   {4, 13},   {1, 14},   {-1, 14},
    {-3, 13},  {-6, 12},  {-8, 11},  {-9, 10},
  },
  {
    {-7, 12},  {-9, 11},  {-11, 9},  {-12, 7},  {-13, 5},  {-14, 2},  {-14, 0},  {-14, -2},
    {-13, -5}, {-12, -7}, {-11, -9}, {-9, -11}, {-7, -12}, {-5, -13}, {-2, -14}, {0, -14},
    {2, -14},  {5, -13},  {7, -12},  {9, -11},  {11, -9},  {12, -7},  {13, -5},  {14, -2},
    {14, 0},   {14, 2},   {13, 5},   {12, 7},   {11, 9},   {9, 11},   {7, 12},   {5, 13},
    {2, 14},   {0, 14},   {-2, 14},  {-5, 13},
  },
  {
    {-4, 7},  {-5, 6},  {-6, 5},  {-7, 4},  {-8, 3},  {-8, 1},  {-8, 0}, {-8, -1}, {-8, -3},
    {-7, -4}, {-6, -5}, {-5, -6}, {-4, -7}, {-3, -8}, {-1, -8}, {0, -8}, {1, -8},  {3, -8},
    {4, -7},  {5, -6},  {6, -5},  {7, -4},  {8, -3},  {8, -1},  {8, 0},  {8, 1},   {8, 3},
    {7, 4},   {6, 5},   {5, 6},   {4, 7},   {3, 8},   {1, 8},   {0, 8},  {-1, 8},  {-3, 8},
  },
  {
    {6, -10},  {8, -9},  {9, -7},  {10, -6},  {11, -4},  {12, -2},  {12, 0},  {11, 2},   {11, 4},
    {10, 6},   {9, 8},   {7, 9},   {6, 10},   {4, 11},   {2, 12},   {0, 12},  {-2, 11},  {-4, 11},
    {-6, 10},  {-8, 9},  {-9, 7},  {-10, 6},  {-11, 4},  {-12, 2},  {-12, 0}, {-11, -2}, {-11, -4},
    {-10, -6}, {-9, -8}, {-7, -9}, {-6, -10}, {-4, -11}, {-2, -12}, {0, -12}, {2, -11},  {4, -11},
  },
  {
    {12, 8},   {10, 10},  {9, 12},   {6, 13},    {4, 14},   {2, 14},   {-1, 14},  {-3, 14},
    {-6, 13},  {-8, 12},  {-10, 10}, {-12, 9},   {-13, 6},  {-14, 4},  {-14, 2},  {-14, -1},
    {-14, -3}, {-13, -6}, {-12, -8}, {-10, -10}, {-9, -12}, {-6, -13}, {-4, -14}, {-2, -14},
    {1, -14},  {3, -14},  {6, -13},  {8, -12},   {10, -10}, {12, -9},  {13, -6},  {14, -4},
    {14, -2},  {14, 1},   {14, 3},   {13, 6},
  },
  {
    {-9, -1}, {-9, -3}, {-8, -4}, {-7, -5}, {-6, -7}, {-5, -8}, {-4, -8}, {-2, -9}, {-1, -9},
    {1, -9},  {3, -9},  {4, -8},  {5, -7},  {7, -6},  {8, -5},  {8, -4},  {9, -2},  {9, -1},
    {9, 1},   {9, 3},   {8, 4},   {7, 5},   {6, 7},   {5, 8},   {4, 8},   {2, 9},   {1, 9},
    {-1, 9},  {-3, 9},  {-4, 8},  {-5, 7},  {-7, 6},  {-8, 5},  {-8, 4},  {-9, 2},  {-9, 1},
  },
  {
    {-7, -6}, {-6, -7}, {-5, -8}, {-3, -9}, {-2, -9}, {0, -9}, {2, -9},  {3, -9},  {5, -8},
    {6, -7},  {7, -6},  {8, -5},  {9, -3},  {9, -2},  {9, 0},  {9, 2},   {9, 3},   {8, 5},
    {7, 6},   {6, 7},   {5, 8},   {3, 9},   {2, 9},   {0, 9},  {-2, 9},  {-3, 9},  {-5, 8},
    {-6, 7},  {-7, 6},  {-8, 5},  {-9, 3},  {-9, 2},  {-9, 0}, {-9, -2}, {-9, -3}, {-8, -5},
  },
  {
    {-2, -5}, {-1, -5}, {0, -5}, {1, -5},  {2, -5},  {3, -5},  {3, -4},  {4, -4},  {5, -3},
    {5, -2},  {5, -1},  {5, 0},  {5, 1},   {5, 2},   {5, 3},   {4, 3},   {4, 4},   {3, 5},
    {2, 5},   {1, 5},   {0, 5},  {-1, 5},  {-2, 5},  {-3, 5},  {-3, 4},  {-4, 4},  {-5, 3},
    {-5, 2},  {-5, 1},  {-5, 0}, {-5, -1}, {-5, -2}, {-5, -3}, {-4, -3}, {-4, -4}, {-3, -5},
  },
  {
    {0, 12},  {-2, 12},  {-4, 11},  {-6, 10},  {-8, 9},  {-9, 8},  {-10, 6},  {-11, 4},  {-12, 2},
    {-12, 0}, {-12, -2}, {-11, -4}, {-10, -6}, {-9, -8}, {-8, -9}, {-6, -10}, {-4, -11}, {-2, -12},
    {0, -12}, {2, -12},  {4, -11},  {6, -10},  {8, -9},  {9, -8},  {10, -6},  {11, -4},  {12, -2},
    {12, 0},  {12, 2},   {11, 4},   {10, 6},   {9, 8},   {8, 9},   {6, 10},   {4, 11},   {2, 12},
  },
  {
    {-12, 5},  {-13, 3},  {-13, 1},  {-13, -2}, {-12, -4}, {-12, -6}, {-10, -8}, {-9, -10},
    {-7, -11}, {-5, -12}, {-3, -13}, {-1, -13}, {2, -13},  {4, -12},  {6, -12},  {8, -10},
    {10, -9},  {11, -7},  {12, -5},  {13, -3},  {13, -1},  {13, 2},   {12, 4},   {12, 6},
    {10, 8},   {9, 10},   {7, 11},   {5, 12},   {3, 13},   {1, 13},   {-2, 13},  {-4, 12},
    {-6, 12},  {-8, 10},  {-10, 9},  {-11, 7},
  },
  {
    {-7, 5},  {-8, 4},  {-8, 2},  {-9, 1},  {-9, -1}, {-8, -2}, {-8, -4}, {-7, -5}, {-6, -6},
    {-5, -7}, {-4, -8}, {-2, -8}, {-1, -9}, {1, -9},  {2, -8},  {4, -8},  {5, -7},  {6, -6},
    {7, -5},  {8, -4},  {8, -2},  {9, -1},  {9, 1},   {8, 2},   {8, 4},   {7, 5},   {6, 6},
    {5, 7},   {4, 8},   {2, 8},   {1, 9},   {-1, 9},  {-2, 8},  {-4, 8},  {-5, 7},  {-6, 6},
  },
  {
    {3, -10},  {5, -9},  {6, -8},  {8, -7},  {9, -6},  {10, -4},  {10, -2},  {10, -1},  {10, 1},
    {10, 3},   {9, 5},   {8, 6},   {7, 8},   {6, 9},   {4, 10},   {2, 10},   {1, 10},   {-1, 10},
    {-3, 10},  {-5, 9},  {-6, 8},  {-8, 7},  {-9, 6},  {-10, 4},  {-10, 2},  {-10, 1},  {-10, -1},
    {-10, -3}, {-9, -5}, {-8, -6}, {-7, -8}, {-6, -9}, {-4, -10}, {-2, -10}, {-1, -10}, {1, -10},
  },
  {
    {8, -13},  {10, -11}, {12, -9},  {13, -7},  {14, -5},   {15, -2},  {15, 0},   {15, 3},
    {14, 6},   {13, 8},   {11, 10},  {9, 12},   {7, 13},    {5, 14},   {2, 15},   {0, 15},
    {-3, 15},  {-6, 14},  {-8, 13},  {-10, 11}, {-12, 9},   {-13, 7},  {-14, 5},  {-15, 2},
    {-15, 0},  {-15, -3}, {-14, -6}, {-13, -8}, {-11, -10}, {-9, -12}, {-7, -13}, {-5, -14},
    {-2, -15}, {0, -15},  {3, -15},  {6, -14},
  },
  {
    {-7, -7}, {-6, -8}, {-4, -9}, {-3, -10}, {-1, -10}, {1, -10},  {3, -10},  {4, -9},  {6, -8},
    {7, -7},  {8, -6},  {9, -4},  {10, -3},  {10, -1},  {10, 1},   {10, 3},   {9, 4},   {8, 6},
    {7, 7},   {6, 8},   {4, 9},   {3, 10},   {1, 10},   {-1, 10},  {-3, 10},  {-4, 9},  {-6, 8},
    {-7, 7},  {-8, 6},  {-9, 4},  {-10, 3},  {-10, 1},  {-10, -1}, {-10, -3}, {-9, -4}, {-8, -6},
  },
  {
    {-4, 5},  {-5, 4},  {-5, 3},  {-6, 2},  {-6, 1},  {-6, 0}, {-6, -1}, {-6, -2}, {-6, -3},
    {-5, -4}, {-4, -5}, {-3, -5}, {-2, -6}, {-1, -6}, {0, -6}, {1, -6},  {2, -6},  {3, -6},
    {4, -5},  {5, -4},  {5, -3},  {6, -2},  {6, -1},  {6, 0},  {6, 1},   {6, 2},   {6, 3},
    {5, 4},   {4, 5},   {3, 5},   {2, 6},   {1, 6},   {0, 6},  {-1, 6},  {-2, 6},  {-3, 6},
  },
  {
    {-3, -2}, {-3, -2}, {-2, -3}, {-2, -3}, {-1, -3}, {0, -4}, {0, -4}, {1, -4},  {1, -3},
    {2, -3},  {2, -3},  {3, -2},  {3, -2},  {3, -1},  {4, 0},  {4, 0},  {4, 1},   {3, 1},
    {3, 2},   {3, 2},   {2, 3},   {2, 3},   {1, 3},   {0, 4},  {0, 4},  {-1, 4},  {-1, 3},
    {-2, 3},  {-2, 3},  {-3, 2},  {-3, 2},  {-3, 1},  {-4, 0}, {-4, 0}, {-4, -1}, {-3, -1},
  },
  {
    {-1, -7}, {0, -7}, {1, -7},  {3, -7},  {4, -6},  {5, -5},  {6, -4},  {6, -3},  {7, -2},
    {7, -1},  {7, 0},  {7, 1},   {7, 3},   {6, 4},   {5, 5},   {4, 6},   {3, 6},   {2, 7},
    {1, 7},   {0, 7},  {-1, 7},  {-3, 7},  {-4, 6},  {-5, 5},  {-6, 4},  {-6, 3},  {-7, 2},
    {-7, 1},  {-7, 0}, {-7, -1}, {-7, -3}, {-6, -4}, {-5, -5}, {-4, -6}, {-3, -6}, {-2, -7},
  },
  {
    {2, 9},   {0, 9},  {-1, 9},  {-3, 9},  {-4, 8},  {-6, 7},  {-7, 6},  {-8, 5},  {-9, 4},
    {-9, 2},  {-9, 0}, {-9, -1}, {-9, -3}, {-8, -4}, {-7, -6}, {-6, -7}, {-5, -8}, {-4, -9},
    {-2, -9}, {0, -9}, {1, -9},  {3, -9},  {4, -8},  {6, -7},  {7, -6},  {8, -5},  {9, -4},
    {9, -2},  {9, 0},  {9, 1},   {9, 3},   {8, 4},   {7, 6},   {6, 7},   {5, 8},   {4, 9},
  },
  {
    {5, -11},  {7, -10},  {8, -9},   {10, -7},  {11, -5},  {12, -3}, {12, -1},  {12, 1},
    {12, 3},   {11, 5},   {10, 7},   {9, 8},    {7, 10},   {5, 11},  {3, 12},   {1, 12},
    {-1, 12},  {-3, 12},  {-5, 11},  {-7, 10},  {-8, 9},   {-10, 7}, {-11, 5},  {-12, 3},
    {-12, 1},  {-12, -1}, {-12, -3}, {-11, -5}, {-10, -7}, {-9, -8}, {-7, -10}, {-5, -11},
    {-3, -12}, {-1, -12}, {1, -12},  {3, -12},
  },
  {
    {-11, -13}, {-9, -15}, {-6, -16}, {-3, -17},  {0, -17}, {3, -17}, {6, -16}, {8, -15},
    {11, -13},  {13, -11}, {15, -9},  {16, -6},   {17, -3}, {17, 0},  {17, 3},  {16, 6},
    {15, 8},    {13, 11},  {11, 13},  {9, 15},    {6, 16},  {3, 17},  {0, 17},  {-3, 17},
    {-6, 16},   {-8, 15},  {-11, 13}, {-13, 11},  {-15, 9}, {-16, 6}, {-17, 3}, {-17, 0},
    {-17, -3},  {-16, -6}, {-15, -8}, {-13, -11},
  },
  {
    {-5, -13}, {-3, -14}, {0, -14},  {2, -14},  {5, -13}, {7, -12}, {9, -11},  {11, -9},
    {12, -7},  {13, -5},  {14, -3},  {14, 0},   {14, 2},  {13, 5},  {12, 7},   {11, 9},
    {9, 11},   {7, 12},   {5, 13},   {3, 14},   {0, 14},  {-2, 14}, {-5, 13},  {-7, 12},
    {-9, 11},  {-11, 9},  {-12, 7},  {-13, 5},  {-14, 3}, {-14, 0}, {-14, -2}, {-13, -5},
    {-12, -7}, {-11, -9}, {-9, -11}, {-7, -12},
  },
  {
    {-1, 6},  {-2, 6},  {-3, 5},  {-4, 5},  {-5, 4},  {-5, 3},  {-6, 2},  {-6, 1},  {-6, 0},
    {-6, -1}, {-6, -2}, {-5, -3}, {-5, -4}, {-4, -5}, {-3, -5}, {-2, -6}, {-1, -6}, {0, -6},
    {1, -6},  {2, -6},  {3, -5},  {4, -5},  {5, -4},  {5, -3},  {6, -2},  {6, -1},  {6, 0},
    {6, 1},   {6, 2},   {5, 3},   {5, 4},   {4, 5},   {3, 5},   {2, 6},   {1, 6},   {0, 6},
  },
  {
    {0, -1}, {0, -1}, {0, -1}, {1, -1},  {1, -1},  {1, -1},  {1, -1},  {1, 0},  {1, 0},
    {1, 0},  {1, 0},  {1, 0},  {1, 1},   {1, 1},   {1, 1},   {1, 1},   {0, 1},  {0, 1},
    {0, 1},  {0, 1},  {0, 1},  {-1, 1},  {-1, 1},  {-1, 1},  {-1, 1},  {-1, 0}, {-1, 0},
    {-1, 0}, {-1, 0}, {-1, 0}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {0, -1}, {0, -1},
  },
  {
    {5, -3},  {5, -2},  {6, -1},  {6, 0},  {6, 1},   {6, 2},   {5, 3},   {5, 4},   {4, 4},
    {3, 5},   {2, 5},   {1, 6},   {0, 6},  {-1, 6},  {-2, 6},  {-3, 5},  {-4, 5},  {-4, 4},
    {-5, 3},  {-5, 2},  {-6, 1},  {-6, 0}, {-6, -1}, {-6, -2}, {-5, -3}, {-5, -4}, {-4, -4},
    {-3, -5}, {-2, -5}, {-1, -6}, {0, -6}, {1, -6},  {2, -6},  {3, -5},  {4, -5},  {4, -4},
  },
  {
    {5, 2},   {5, 3},   {4, 4},   {3, 4},   {3, 5},   {2, 5},   {1, 5},   {0, 5},  {-1, 5},
    {-2, 5},  {-3, 5},  {-4, 4},  {-4, 3},  {-5, 3},  {-5, 2},  {-5, 1},  {-5, 0}, {-5, -1},
    {-5, -2}, {-5, -3}, {-4, -4}, {-3, -4}, {-3, -5}, {-2, -5}, {-1, -5}, {0, -5}, {1, -5},
    {2, -5},  {3, -5},  {4, -4},  {4, -3},  {5, -3},  {5, -2},  {5, -1},  {5, 0},  {5, 1},
  },
  {
    {-4, -13}, {-2, -13}, {1, -14},  {3, -13},  {5, -13}, {7, -11},  {9, -10},  {11, -8},
    {12, -6},  {13, -4},  {13, -2},  {14, 1},   {13, 3},  {13, 5},   {11, 7},   {10, 9},
    {8, 11},   {6, 12},   {4, 13},   {2, 13},   {-1, 14}, {-3, 13},  {-5, 13},  {-7, 11},
    {-9, 10},  {-11, 8},  {-12, 6},  {-13, 4},  {-13, 2}, {-14, -1}, {-13, -3}, {-13, -5},
    {-11, -7}, {-10, -9}, {-8, -11}, {-6, -12},
  },
  {
    {-4, 12},  {-6, 11},  {-8, 10},  {-9, 8},  {-11, 7},  {-12, 5},  {-12, 3},  {-13, 0}, {-13, -2},
    {-12, -4}, {-11, -6}, {-10, -8}, {-8, -9}, {-7, -11}, {-5, -12}, {-3, -12}, {0, -13}, {2, -13},
    {4, -12},  {6, -11},  {8, -10},  {9, -8},  {11, -7},  {12, -5},  {12, -3},  {13, 0},  {13, 2},
    {12, 4},   {11, 6},   {10, 8},   {8, 9},   {7, 11},   {5, 12},   {3, 12},   {0, 13},  {-2, 13},
  },
  {
    {-9, -6}, {-8, -7}, {-6, -9}, {-5, -10}, {-3, -10}, {-1, -11}, {1, -11},  {3, -11},  {4, -10},
    {6, -9},  {7, -8},  {9, -6},  {10, -5},  {10, -3},  {11, -1},  {11, 1},   {11, 3},   {10, 4},
    {9, 6},   {8, 7},   {6, 9},   {5, 10},   {3, 10},   {1, 11},   {-1, 11},  {-3, 11},  {-4, 10},
    {-6, 9},  {-7, 8},  {-9, 6},  {-10, 5},  {-10, 3},  {-11, 1},  {-11, -1}, {-11, -3}, {-10, -4},
  },
  {
    {-9, 6},  {-10, 4},  {-11, 3},  {-11, 1},  {-11, -1}, {-10, -3}, {-10, -5}, {-9, -6}, {-7, -8},
    {-6, -9}, {-4, -10}, {-3, -11}, {-1, -11}, {1, -11},  {3, -10},  {5, -10},  {6, -9},  {8, -7},
    {9, -6},  {10, -4},  {11, -3},  {11, -1},  {11, 1},   {10, 3},   {10, 5},   {9, 6},   {7, 8},
    {6, 9},   {4, 10},   {3, 11},   {1, 11},   {-1, 11},  {-3, 10},  {-5, 10},  {-6, 9},  {-8, 7},
  },
  {
    {-12, -10}, {-10, -12}, {-8, -14}, {-5, -15}, {-3, -15}, {0, -16}, {3, -15}, {5, -15},
    {8, -14},   {10, -12},  {12, -10}, {14, -8},  {15, -5},  {15, -3}, {16, 0},  {15, 3},
    {15, 5},    {14, 8},    {12, 10},  {10, 12},  {8, 14},   {5, 15},  {3, 15},  {0, 16},
    {-3, 15},   {-5, 15},   {-8, 14},  {-10, 12}, {-12, 10}, {-14, 8}, {-15, 5}, {-15, 3},
    {-16, 0},   {-15, -3},  {-15, -5}, {-14, -8},
  },
  {
    {-8, -4}, {-7, -5}, {-6, -6}, {-5, -7}, {-4, -8}, {-2, -9}, {-1, -9}, {1, -9},  {3, -9},
    {4, -8},  {5, -7},  {6, -6},  {7, -5},  {8, -4},  {9, -2},  {9, -1},  {9, 1},   {9, 3},
    {8, 4},   {7, 5},   {6, 6},   {5, 7},   {4, 8},   {2, 9},   {1, 9},   {-1, 9},  {-3, 9},
    {-4, 8},  {-5, 7},  {-6, 6},  {-7, 5},  {-8, 4},  {-9, 2},  {-9, 1},  {-9, -1}, {-9, -3},
  },
  {
    {10, 2},   {10, 4},   {9, 5},   {8, 7},   {6, 8},   {5, 9},   {3, 10},   {2, 10},   {0, 10},
    {-2, 10},  {-4, 10},  {-5, 9},  {-7, 8},  {-8, 6},  {-9, 5},  {-10, 3},  {-10, 2},  {-10, 0},
    {-10, -2}, {-10, -4}, {-9, -5}, {-8, -7}, {-6, -8}, {-5, -9}, {-3, -10}, {-2, -10}, {0, -10},
    {2, -10},  {4, -10},  {5, -9},  {7, -8},  {8, -6},  {9, -5},  {10, -3},  {10, -2},  {10, 0},
  },
  {
    {12, -3}, {12, -1},  {12, 1},   {12, 3},   {11, 5},   {10, 7},   {9, 9},    {7, 10},
    {5, 11},  {3, 12},   {1, 12},   {-1, 12},  {-3, 12},  {-5, 11},  {-7, 10},  {-9, 9},
    {-10, 7}, {-11, 5},  {-12, 3},  {-12, 1},  {-12, -1}, {-12, -3}, {-11, -5}, {-10, -7},
    {-9, -9}, {-7, -10}, {-5, -11}, {-3, -12}, {-1, -12}, {1, -12},  {3, -12},  {5, -11},
    {7, -10}, {9, -9},   {10, -7},  {11, -5},
  },
  {
    {7, 12},   {5, 13},   {2, 14},   {0, 14},   {-2, 14},  {-5, 13},  {-7, 12},  {-9, 11},
    {-11, 9},  {-12, 7},  {-13, 5},  {-14, 2},  {-14, 0},  {-14, -2}, {-13, -5}, {-12, -7},
    {-11, -9}, {-9, -11}, {-7, -12}, {-5, -13}, {-2, -14}, {0, -14},  {2, -14},  {5, -13},
    {7, -12},  {9, -11},  {11, -9},  {12, -7},  {13, -5},  {14, -2},  {14, 0},   {14, 2},
    {13, 5},   {12, 7},   {11, 9},   {9, 11},
  },
  {
    {12, 12},  {10, 14},   {7, 15},    {4, 16},    {1, 17},   {-1, 17},  {-4, 16},  {-7, 15},
    {-10, 14}, {-12, 12},  {-14, 10},  {-15, 7},   {-16, 4},  {-17, 1},  {-17, -1}, {-16, -4},
    {-15, -7}, {-14, -10}, {-12, -12}, {-10, -14}, {-7, -15}, {-4, -16}, {-1, -17}, {1, -17},
    {4, -16},  {7, -15},   {10, -14},  {12, -12},  {14, -10}, {15, -7},  {16, -4},  {17, -1},
    {17, 1},   {16, 4},    {15, 7},    {14, 10},
  },
  {
    {-7, -13}, {-5, -14}, {-2, -15},  {0, -15},  {3, -14}, {5, -14}, {8, -13}, {10, -11},
    {12, -9},  {13, -7},  {14, -5},   {15, -2},  {15, 0},  {14, 3},  {14, 5},  {13, 8},
    {11, 10},  {9, 12},   {7, 13},    {5, 14},   {2, 15},  {0, 15},  {-3, 14}, {-5, 14},
    {-8, 13},  {-10, 11}, {-12, 9},   {-13, 7},  {-14, 5}, {-15, 2}, {-15, 0}, {-14, -3},
    {-14, -5}, {-13, -8}, {-11, -10}, {-9, -12},
  },
  {
    {-6, 5},  {-7, 4},  {-7, 3},  {-8, 1},  {-8, 0}, {-8, -1}, {-7, -3}, {-7, -4}, {-6, -5},
    {-5, -6}, {-4, -7}, {-3, -7}, {-1, -8}, {0, -8}, {1, -8},  {3, -7},  {4, -7},  {5, -6},
    {6, -5},  {7, -4},  {7, -3},  {8, -1},  {8, 0},  {8, 1},   {7, 3},   {7, 4},   {6, 5},
    {5, 6},   {4, 7},   {3, 7},   {1, 8},   {0, 8},  {-1, 8},  {-3, 7},  {-4, 7},  {-5, 6},
  },
  {
    {-4, 9},  {-6, 8},  {-7, 7},  {-8, 6},  {-9, 4},  {-9, 3},  {-10, 1},  {-10, -1}, {-10, -2},
    {-9, -4}, {-8, -6}, {-7, -7}, {-6, -8}, {-4, -9}, {-3, -9}, {-1, -10}, {1, -10},  {2, -10},
    {4, -9},  {6, -8},  {7, -7},  {8, -6},  {9, -4},  {9, -3},  {10, -1},  {10, 1},   {10, 2},
    {9, 4},   {8, 6},   {7, 7},   {6, 8},   {4, 9},   {3, 9},   {1, 10},   {-1, 10},  {-2, 10},
  },
  {
    {-3, 4},  {-4, 3},  {-4, 3},  {-5, 2},  {-5, 1},  {-5, 0}, {-5, -1}, {-5, -1}, {-4, -2},
    {-4, -3}, {-3, -4}, {-3, -4}, {-2, -5}, {-1, -5}, {0, -5}, {1, -5},  {1, -5},  {2, -4},
    {3, -4},  {4, -3},  {4, -3},  {5, -2},  {5, -1},  {5, 0},  {5, 1},   {5, 1},   {4, 2},
    {4, 3},   {3, 4},   {3, 4},   {2, 5},   {1, 5},   {0, 5},  {-1, 5},  {-1, 5},  {-2, 4},
  },
  {
    {7, -1},  {7, 0},  {7, 1},   {7, 3},   {6, 4},   {5, 5},   {4, 6},   {3, 6},   {2, 7},
    {1, 7},   {0, 7},  {-1, 7},  {-3, 7},  {-4, 6},  {-5, 5},  {-6, 4},  {-6, 3},  {-7, 2},
    {-7, 1},  {-7, 0}, {-7, -1}, {-7, -3}, {-6, -4}, {-5, -5}, {-4, -6}, {-3, -6}, {-2, -7},
    {-1, -7}, {0, -7}, {1, -7},  {3, -7},  {4, -6},  {5, -5},  {6, -4},  {6, -3},  {7, -2},
  },
  {
    {12, 2},   {11, 4},   {11, 6},   {9, 8},   {8, 9},   {6, 10},   {4, 11},   {2, 12},   {0, 12},
    {-2, 12},  {-4, 11},  {-6, 11},  {-8, 9},  {-9, 8},  {-10, 6},  {-11, 4},  {-12, 2},  {-12, 0},
    {-12, -2}, {-11, -4}, {-11, -6}, {-9, -8}, {-8, -9}, {-6, -10}, {-4, -11}, {-2, -12}, {0, -12},
    {2, -12},  {4, -11},  {6, -11},  {8, -9},  {9, -8},  {10, -6},  {11, -4},  {12, -2},  {12, 0},
  },
  {
    {-7, 6},  {-8, 5},  {-9, 3},  {-9, 2},  {-9, 0}, {-9, -2}, {-9, -3}, {-8, -5}, {-7, -6},
    {-6, -7}, {-5, -8}, {-3, -9}, {-2, -9}, {0, -9}, {2, -9},  {3, -9},  {5, -8},  {6, -7},
    {7, -6},  {8, -5},  {9, -3},  {9, -2},  {9, 0},  {9, 2},   {9, 3},   {8, 5},   {7, 6},
    {6, 7},   {5, 8},   {3, 9},   {2, 9},   {0, 9},  {-2, 9},  {-3, 9},  {-5, 8},  {-6, 7},
  },
  {
    {-5, 1},  {-5, 0}, {-5, -1}, {-5, -2}, {-4, -2}, {-4, -3}, {-3, -4}, {-3, -4}, {-2, -5},
    {-1, -5}, {0, -5}, {1, -5},  {2, -5},  {2, -4},  {3, -4},  {4, -3},  {4, -3},  {5, -2},
    {5, -1},  {5, 0},  {5, 1},   {5, 2},   {4, 2},   {4, 3},   {3, 4},   {3, 4},   {2, 5},
    {1, 5},   {0, 5},  {-1, 5},  {-2, 5},  {-2, 4},  {-3, 4},  {-4, 3},  {-4, 3},  {-5, 2},
  },
  {
    {-13, 11},  {-15, 9},   {-16, 6},  {-17, 3},  {-17, 0},  {-17, -3}, {-16, -6}, {-15, -8},
    {-13, -11}, {-11, -13}, {-9, -15}, {-6, -16}, {-3, -17}, {0, -17},  {3, -17},  {6, -16},
    {8, -15},   {11, -13},  {13, -11}, {15, -9},  {16, -6},  {17, -3},  {17, 0},   {17, 3},
    {16, 6},    {15, 8},    {13, 11},  {11, 13},  {9, 15},   {6, 16},   {3, 17},   {0, 17},
    {-3, 17},   {-6, 16},   {-8, 15},  {-11, 13},
  },
  {
    {-12, 5},  {-13, 3},  {-13, 1},  {-13, -2}, {-12, -4}, {-12, -6}, {-10, -8}, {-9, -10},
    {-7, -11}, {-5, -12}, {-3, -13}, {-1, -13}, {2, -13},  {4, -12},  {6, -12},  {8, -10},
    {10, -9},  {11, -7},  {12, -5},  {13, -3},  {13, -1},  {13, 2},   {12, 4},   {12, 6},
    {10, 8},   {9, 10},   {7, 11},   {5, 12},   {3, 13},   {1, 13},   {-2, 13},  {-4, 12},
    {-6, 12},  {-8, 10},  {-10, 9},  {-11, 7},
  },
  {
    {-3, 7},  {-4, 6},  {-5, 6},  {-6, 5},  {-7, 3},  {-7, 2},  {-8, 1},  {-8, 0}, {-7, -2},
    {-7, -3}, {-6, -4}, {-6, -5}, {-5, -6}, {-3, -7}, {-2, -7}, {-1, -8}, {0, -8}, {2, -7},
    {3, -7},  {4, -6},  {5, -6},  {6, -5},  {7, -3},  {7, -2},  {8, -1},  {8, 0},  {7, 2},
    {7, 3},   {6, 4},   {6, 5},   {5, 6},   {3, 7},   {2, 7},   {1, 8},   {0, 8},  {-2, 7},
  },
  {
    {-2, -6}, {-1, -6}, {0, -6}, {1, -6},  {2, -6},  {3, -5},  {4, -5},  {5, -4},  {6, -3},
    {6, -2},  {6, -1},  {6, 0},  {6, 1},   {6, 2},   {5, 3},   {5, 4},   {4, 5},   {3, 6},
    {2, 6},   {1, 6},   {0, 6},  {-1, 6},  {-2, 6},  {-3, 5},  {-4, 5},  {-5, 4},  {-6, 3},
    {-6, 2},  {-6, 1},  {-6, 0}, {-6, -1}, {-6, -2}, {-5, -3}, {-5, -4}, {-4, -5}, {-3, -6},
  },
  {
    {7, -8},  {8, -7},  {9, -5},  {10, -3},  {11, -2},  {11, 0},  {10, 2},   {10, 4},   {9, 6},
    {8, 7},   {7, 8},   {5, 9},   {3, 10},   {2, 11},   {0, 11},  {-2, 10},  {-4, 10},  {-6, 9},
    {-7, 8},  {-8, 7},  {-9, 5},  {-10, 3},  {-11, 2},  {-11, 0}, {-10, -2}, {-10, -4}, {-9, -6},
    {-8, -7}, {-7, -8}, {-5, -9}, {-3, -10}, {-2, -11}, {0, -11}, {2, -10},  {4, -10},  {6, -9},
  },
  {
    {12, -7},  {13, -5},  {14, -2},  {14, 0},   {14, 2},   {13, 5},   {12, 7},   {11, 9},
    {9, 11},   {7, 12},   {5, 13},   {2, 14},   {0, 14},   {-2, 14},  {-5, 13},  {-7, 12},
    {-9, 11},  {-11, 9},  {-12, 7},  {-13, 5},  {-14, 2},  {-14, 0},  {-14, -2}, {-13, -5},
    {-12, -7}, {-11, -9}, {-9, -11}, {-7, -12}, {-5, -13}, {-2, -14}, {0, -14},  {2, -14},
    {5, -13},  {7, -12},  {9, -11},  {11, -9},
  },
  {
    {-13, -7}, {-12, -9}, {-10, -11}, {-8, -13}, {-5, -14}, {-3, -14}, {0, -15}, {2, -15},
    {5, -14},  {7, -13},  {9, -12},   {11, -10}, {13, -8},  {14, -5},  {14, -3}, {15, 0},
    {15, 2},   {14, 5},   {13, 7},    {12, 9},   {10, 11},  {8, 13},   {5, 14},  {3, 14},
    {0, 15},   {-2, 15},  {-5, 14},   {-7, 13},  {-9, 12},  {-11, 10}, {-13, 8}, {-14, 5},
    {-14, 3},  {-15, 0},  {-15, -2},  {-14, -5},
  },
  {
    {-11, -12}, {-9, -14}, {-6, -15}, {-4, -16},  {-1, -16}, {2, -16}, {5, -16}, {8, -14},
    {10, -13},  {12, -11}, {14, -9},  {15, -6},   {16, -4},  {16, -1}, {16, 2},  {16, 5},
    {14, 8},    {13, 10},  {11, 12},  {9, 14},    {6, 15},   {4, 16},  {1, 16},  {-2, 16},
    {-5, 16},   {-8, 14},  {-10, 13}, {-12, 11},  {-14, 9},  {-15, 6}, {-16, 4}, {-16, 1},
    {-16, -2},  {-16, -5}, {-14, -8}, {-13, -10},
  },
  {
    {1, -3},  {2, -3},  {2, -2},  {2, -2},  {3, -2},  {3, -1},  {3, -1},  {3, 0},  {3, 0},
    {3, 1},   {3, 2},   {2, 2},   {2, 2},   {2, 3},   {1, 3},   {1, 3},   {0, 3},  {0, 3},
    {-1, 3},  {-2, 3},  {-2, 2},  {-2, 2},  {-3, 2},  {-3, 1},  {-3, 1},  {-3, 0}, {-3, 0},
    {-3, -1}, {-3, -2}, {-2, -2}, {-2, -2}, {-2, -3}, {-1, -3}, {-1, -3}, {0, -3}, {0, -3},
  },
  {
    {12, 12},  {10, 14},   {7, 15},    {4, 16},    {1, 17},   {-1, 17},  {-4, 16},  {-7, 15},
    {-10, 14}, {-12, 12},  {-14, 10},  {-15, 7},   {-16, 4},  {-17, 1},  {-17, -1}, {-16, -4},
    {-15, -7}, {-14, -10}, {-12, -12}, {-10, -14}, {-7, -15}, {-4, -16}, {-1, -17}, {1, -17},
    {4, -16},  {7, -15},   {10, -14},  {12, -12},  {14, -10}, {15, -7},  {16, -4},  {17, -1},
    {17, 1},   {16, 4},    {15, 7},    {14, 10},
  },
  {
    {2, -6},  {3, -6},  {4, -5},  {5, -4},  {5, -3},  {6, -2},  {6, -1},  {6, 0},  {6, 1},
    {6, 2},   {6, 3},   {5, 4},   {4, 5},   {3, 5},   {2, 6},   {1, 6},   {0, 6},  {-1, 6},
    {-2, 6},  {-3, 6},  {-4, 5},  {-5, 4},  {-5, 3},  {-6, 2},  {-6, 1},  {-6, 0}, {-6, -1},
    {-6, -2}, {-6, -3}, {-5, -4}, {-4, -5}, {-3, -5}, {-2, -6}, {-1, -6}, {0, -6}, {1, -6},
  },
  {
    {3, 0},  {3, 1},   {3, 1},   {3, 2},   {2, 2},   {2, 2},   {2, 3},   {1, 3},   {1, 3},
    {0, 3},  {-1, 3},  {-1, 3},  {-2, 3},  {-2, 2},  {-2, 2},  {-3, 2},  {-3, 1},  {-3, 1},
    {-3, 0}, {-3, -1}, {-3, -1}, {-3, -2}, {-2, -2}, {-2, -2}, {-2, -3}, {-1, -3}, {-1, -3},
    {0, -3}, {1, -3},  {1, -3},  {2, -3},  {2, -2},  {2, -2},  {3, -2},  {3, -1},  {3, -1},
  },
  {
    {-4, 3},  {-4, 2},  {-5, 1},  {-5, 1},  {-5, 0}, {-5, -1}, {-5, -2}, {-4, -3}, {-4, -3},
    {-3, -4}, {-2, -4}, {-1, -5}, {-1, -5}, {0, -5}, {1, -5},  {2, -5},  {3, -4},  {3, -4},
    {4, -3},  {4, -2},  {5, -1},  {5, -1},  {5, 0},  {5, 1},   {5, 2},   {4, 3},   {4, 3},
    {3, 4},   {2, 4},   {1, 5},   {1, 5},   {0, 5},  {-1, 5},  {-2, 5},  {-3, 4},  {-3, 4},
  },
  {
    {-2, -13}, {0, -13},  {3, -13},  {5, -12},  {7, -11}, {9, -10},  {10, -8},  {12, -6},
    {12, -4},  {13, -2},  {13, 0},   {13, 3},   {12, 5},  {11, 7},   {10, 9},   {8, 10},
    {6, 12},   {4, 12},   {2, 13},   {0, 13},   {-3, 13}, {-5, 12},  {-7, 11},  {-9, 10},
    {-10, 8},  {-12, 6},  {-12, 4},  {-13, 2},  {-13, 0}, {-13, -3}, {-12, -5}, {-11, -7},
    {-10, -9}, {-8, -10}, {-6, -12}, {-4, -12},
  },
  {
    {-1, -13}, {1, -13},  {4, -13},  {6, -12},  {8, -11},  {9, -9},   {11, -7},  {12, -5},
    {13, -3},  {13, -1},  {13, 1},   {13, 4},   {12, 6},   {11, 8},   {9, 9},    {7, 11},
    {5, 12},   {3, 13},   {1, 13},   {-1, 13},  {-4, 13},  {-6, 12},  {-8, 11},  {-9, 9},
    {-11, 7},  {-12, 5},  {-13, 3},  {-13, 1},  {-13, -1}, {-13, -4}, {-12, -6}, {-11, -8},
    {-9, -9},  {-7, -11}, {-5, -12}, {-3, -13},
  },
  {
    {1, 9},   {-1, 9},  {-2, 9},  {-4, 8},  {-5, 8},  {-6, 7},  {-7, 5},  {-8, 4},  {-9, 3},
    {-9, 1},  {-9, -1}, {-9, -2}, {-8, -4}, {-8, -5}, {-7, -6}, {-5, -7}, {-4, -8}, {-3, -9},
    {-1, -9}, {1, -9},  {2, -9},  {4, -8},  {5, -8},  {6, -7},  {7, -5},  {8, -4},  {9, -3},
    {9, -1},  {9, 1},   {9, 2},   {8, 4},   {8, 5},   {7, 6},   {5, 7},   {4, 8},   {3, 9},
  },
  {
    {7, 1},   {7, 2},   {6, 3},   {6, 4},   {5, 5},   {4, 6},   {3, 7},   {1, 7},   {0, 7},
    {-1, 7},  {-2, 7},  {-3, 6},  {-4, 6},  {-5, 5},  {-6, 4},  {-7, 3},  {-7, 1},  {-7, 0},
    {-7, -1}, {-7, -2}, {-6, -3}, {-6, -4}, {-5, -5}, {-4, -6}, {-3, -7}, {-1, -7}, {0, -7},
    {1, -7},  {2, -7},  {3, -6},  {4, -6},  {5, -5},  {6, -4},  {7, -3},  {7, -1},  {7, 0},
  },
  {
    {8, -6},  {9, -5},  {10, -3},  {10, -1},  {10, 1},   {10, 2},   {9, 4},   {8, 5},   {7, 7},
    {6, 8},   {5, 9},   {3, 10},   {1, 10},   {-1, 10},  {-2, 10},  {-4, 9},  {-5, 8},  {-7, 7},
    {-8, 6},  {-9, 5},  {-10, 3},  {-10, 1},  {-10, -1}, {-10, -2}, {-9, -4}, {-8, -5}, {-7, -7},
    {-6, -8}, {-5, -9}, {-3, -10}, {-1, -10}, {1, -10},  {2, -10},  {4, -9},  {5, -8},  {7, -7},
  },
  {
    {1, -1},  {1, -1},  {1, -1},  {1, 0},  {1, 0},  {1, 0},  {1, 0},  {1, 1},   {1, 1},
    {1, 1},   {1, 1},   {1, 1},   {0, 1},  {0, 1},  {0, 1},  {0, 1},  {-1, 1},  {-1, 1},
    {-1, 1},  {-1, 1},  {-1, 1},  {-1, 0}, {-1, 0}, {-1, 0}, {-1, 0}, {-1, -1}, {-1, -1},
    {-1, -1}, {-1, -1}, {-1, -1}, {0, -1}, {0, -1}, {0, -1}, {0, -1}, {1, -1},  {1, -1},
  },
  {
    {3, 12},   {1, 12},   {-1, 12},  {-3, 12},  {-5, 11},  {-7, 10},  {-9, 9},   {-10, 7},
    {-11, 5},  {-12, 3},  {-12, 1},  {-12, -1}, {-12, -3}, {-11, -5}, {-10, -7}, {-9, -9},
    {-7, -10}, {-5, -11}, {-3, -12}, {-1, -12}, {1, -12},  {3, -12},  {5, -11},  {7, -10},
    {9, -9},   {10, -7},  {11, -5},  {12, -3},  {12, -1},  {12, 1},   {12, 3},   {11, 5},
    {10, 7},   {9, 9},    {7, 10},   {5, 11},
  },
  {
    {9, 1},   {9, 3},   {8, 4},   {7, 5},   {6, 7},   {5, 8},   {4, 8},   {2, 9},   {1, 9},
    {-1, 9},  {-3, 9},  {-4, 8},  {-5, 7},  {-7, 6},  {-8, 5},  {-8, 4},  {-9, 2},  {-9, 1},
    {-9, -1}, {-9, -3}, {-8, -4}, {-7, -5}, {-6, -7}, {-5, -8}, {-4, -8}, {-2, -9}, {-1, -9},
    {1, -9},  {3, -9},  {4, -8},  {5, -7},  {7, -6},  {8, -5},  {8, -4},  {9, -2},  {9, -1},
  },
  {
    {12, 6},   {11, 8},   {9, 10},   {7, 11},   {5, 12},   {3, 13},   {1, 13},   {-2, 13},
    {-4, 13},  {-6, 12},  {-8, 11},  {-10, 9},  {-11, 7},  {-12, 5},  {-13, 3},  {-13, 1},
    {-13, -2}, {-13, -4}, {-12, -6}, {-11, -8}, {-9, -10}, {-7, -11}, {-5, -12}, {-3, -13},
    {-1, -13}, {2, -13},  {4, -13},  {6, -12},  {8, -11},  {10, -9},  {11, -7},  {12, -5},
    {13, -3},  {13, -1},  {13, 2},   {13, 4},
  },
  {
    {-1, -9}, {1, -9},  {2, -9},  {4, -8},  {5, -8},  {6, -7},  {7, -5},  {8, -4},  {9, -3},
    {9, -1},  {9, 1},   {9, 2},   {8, 4},   {8, 5},   {7, 6},   {5, 7},   {4, 8},   {3, 9},
    {1, 9},   {-1, 9},  {-2, 9},  {-4, 8},  {-5, 8},  {-6, 7},  {-7, 5},  {-8, 4},  {-9, 3},
    {-9, 1},  {-9, -1}, {-9, -2}, {-8, -4}, {-8, -5}, {-7, -6}, {-5, -7}, {-4, -8}, {-3, -9},
  },
  {
    {-1, 3},  {-2, 3},  {-2, 2},  {-2, 2},  {-3, 2},  {-3, 1},  {-3, 1},  {-3, 0}, {-3, 0},
    {-3, -1}, {-3, -2}, {-2, -2}, {-2, -2}, {-2, -3}, {-1, -3}, {-1, -3}, {0, -3}, {0, -3},
    {1, -3},  {2, -3},  {2, -2},  {2, -2},  {3, -2},  {3, -1},  {3, -1},  {3, 0},  {3, 0},
    {3, 1},   {3, 2},   {2, 2},   {2, 2},   {2, 3},   {1, 3},   {1, 3},   {0, 3},  {0, 3},
  },
  {
    {-13, -13}, {-11, -15}, {-8, -17}, {-5, -18},  {-2, -18}, {2, -18}, {5, -18}, {8, -17},
    {11, -15},  {13, -13},  {15, -11}, {17, -8},   {18, -5},  {18, -2}, {18, 2},  {18, 5},
    {17, 8},    {15, 11},   {13, 13},  {11, 15},   {8, 17},   {5, 18},  {2, 18},  {-2, 18},
    {-5, 18},   {-8, 17},   {-11, 15}, {-13, 13},  {-15, 11}, {-17, 8}, {-18, 5}, {-18, 2},
    {-18, -2},  {-18, -5},  {-17, -8}, {-15, -11},
  },
  {
    {-10, 5},  {-11, 3},  {-11, 1},  {-11, -1}, {-11, -3}, {-10, -4}, {-9, -6}, {-8, -8}, {-7, -9},
    {-5, -10}, {-3, -11}, {-1, -11}, {1, -11},  {3, -11},  {4, -10},  {6, -9},  {8, -8},  {9, -7},
    {10, -5},  {11, -3},  {11, -1},  {11, 1},   {11, 3},   {10, 4},   {9, 6},   {8, 8},   {7, 9},
    {5, 10},   {3, 11},   {1, 11},   {-1, 11},  {-3, 11},  {-4, 10},  {-6, 9},  {-8, 8},  {-9, 7},
  },
  {
    {7, 7},   {6, 8},   {4, 9},   {3, 10},   {1, 10},   {-1, 10},  {-3, 10},  {-4, 9},  {-6, 8},
    {-7, 7},  {-8, 6},  {-9, 4},  {-10, 3},  {-10, 1},  {-10, -1}, {-10, -3}, {-9, -4}, {-8, -6},
    {-7, -7}, {-6, -8}, {-4, -9}, {-3, -10}, {-1, -10}, {1, -10},  {3, -10},  {4, -9},  {6, -8},
    {7, -7},  {8, -6},  {9, -4},  {10, -3},  {10, -1},  {10, 1},   {10, 3},   {9, 4},   {8, 6},
  },
  {
    {10, 12},  {8, 14},    {5, 15},    {3, 15},   {0, 16},   {-3, 15},  {-5, 15},  {-8, 14},
    {-10, 12}, {-12, 10},  {-14, 8},   {-15, 5},  {-15, 3},  {-16, 0},  {-15, -3}, {-15, -5},
    {-14, -8}, {-12, -10}, {-10, -12}, {-8, -14}, {-5, -15}, {-3, -15}, {0, -16},  {3, -15},
    {5, -15},  {8, -14},   {10, -12},  {12, -10}, {14, -8},  {15, -5},  {15, -3},  {16, 0},
    {15, 3},   {15, 5},    {14, 8},    {12, 10},
  },
  {
    {12, -5},  {13, -3},  {13, -1},  {13, 2},   {12, 4},   {12, 6},   {10, 8},   {9, 10},
    {7, 11},   {5, 12},   {3, 13},   {1, 13},   {-2, 13},  {-4, 12},  {-6, 12},  {-8, 10},
    {-10, 9},  {-11, 7},  {-12, 5},  {-13, 3},  {-13, 1},  {-13, -2}, {-12, -4}, {-12, -6},
    {-10, -8}, {-9, -10}, {-7, -11}, {-5, -12}, {-3, -13}, {-1, -13}, {2, -13},  {4, -12},
    {6, -12},  {8, -10},  {10, -9},  {11, -7},
  },
  {
    {12, 9},   {10, 11},  {8, 13},   {6, 14},    {3, 15},   {1, 15},   {-2, 15},  {-4, 14},
    {-7, 13},  {-9, 12},  {-11, 10}, {-13, 8},   {-14, 6},  {-15, 3},  {-15, 1},  {-15, -2},
    {-14, -4}, {-13, -7}, {-12, -9}, {-10, -11}, {-8, -13}, {-6, -14}, {-3, -15}, {-1, -15},
    {2, -15},  {4, -14},  {7, -13},  {9, -12},   {11, -10}, {13, -8},  {14, -6},  {15, -3},
    {15, -1},  {15, 2},   {14, 4},   {13, 7},
  },
  {
    {6, 3},   {5, 4},   {5, 5},   {4, 6},   {3, 6},   {2, 7},   {0, 7},  {-1, 7},  {-2, 6},
    {-3, 6},  {-4, 5},  {-5, 5},  {-6, 4},  {-6, 3},  {-7, 2},  {-7, 0}, {-7, -1}, {-6, -2},
    {-6, -3}, {-5, -4}, {-5, -5}, {-4, -6}, {-3, -6}, {-2, -7}, {0, -7}, {1, -7},  {2, -6},
    {3, -6},  {4, -5},  {5, -5},  {6, -4},  {6, -3},  {7, -2},  {7, 0},  {7, 1},   {6, 2},
  },
  {
    {7, 11},   {5, 12},   {3, 13},   {1, 13},   {-2, 13},  {-4, 12},  {-6, 12},  {-8, 10},
    {-10, 9},  {-11, 7},  {-12, 5},  {-13, 3},  {-13, 1},  {-13, -2}, {-12, -4}, {-12, -6},
    {-10, -8}, {-9, -10}, {-7, -11}, {-5, -12}, {-3, -13}, {-1, -13}, {2, -13},  {4, -12},
    {6, -12},  {8, -10},  {10, -9},  {11, -7},  {12, -5},  {13, -3},  {13, -1},  {13, 2},
    {12, 4},   {12, 6},   {10, 8},   {9, 10},
  },
  {
    {5, -13},  {7, -12},  {9, -11},  {11, -9},  {12, -7},  {13, -5},  {14, -2},  {14, 0},
    {14, 3},   {13, 5},   {12, 7},   {11, 9},   {9, 11},   {7, 12},   {5, 13},   {2, 14},
    {0, 14},   {-3, 14},  {-5, 13},  {-7, 12},  {-9, 11},  {-11, 9},  {-12, 7},  {-13, 5},
    {-14, 2},  {-14, 0},  {-14, -3}, {-13, -5}, {-12, -7}, {-11, -9}, {-9, -11}, {-7, -12},
    {-5, -13}, {-2, -14}, {0, -14},  {3, -14},
  },
  {
    {6, 10},   {4, 11},   {2, 11},   {0, 12},  {-2, 12},  {-4, 11},  {-6, 10},  {-7, 9},  {-9, 8},
    {-10, 6},  {-11, 4},  {-11, 2},  {-12, 0}, {-12, -2}, {-11, -4}, {-10, -6}, {-9, -7}, {-8, -9},
    {-6, -10}, {-4, -11}, {-2, -11}, {0, -12}, {2, -12},  {4, -11},  {6, -10},  {7, -9},  {9, -8},
    {10, -6},  {11, -4},  {11, -2},  {12, 0},  {12, 2},   {11, 4},   {10, 6},   {9, 7},   {8, 9},
  },
  {
    {2, -12},  {4, -11},  {6, -11},  {8, -9},  {9, -8},  {10, -6},  {11, -4},  {12, -2},  {12, 0},
    {12, 2},   {11, 4},   {11, 6},   {9, 8},   {8, 9},   {6, 10},   {4, 11},   {2, 12},   {0, 12},
    {-2, 12},  {-4, 11},  {-6, 11},  {-8, 9},  {-9, 8},  {-10, 6},  {-11, 4},  {-12, 2},  {-12, 0},
    {-12, -2}, {-11, -4}, {-11, -6}, {-9, -8}, {-8, -9}, {-6, -10}, {-4, -11}, {-2, -12}, {0, -12},
  },
  {
    {2, 3},   {1, 3},   {1, 4},   {0, 4},  {0, 4},  {-1, 3},  {-2, 3},  {-2, 3},  {-3, 2},
    {-3, 2},  {-3, 1},  {-4, 1},  {-4, 0}, {-4, 0}, {-3, -1}, {-3, -2}, {-3, -2}, {-2, -3},
    {-2, -3}, {-1, -3}, {-1, -4}, {0, -4}, {0, -4}, {1, -3},  {2, -3},  {2, -3},  {3, -2},
    {3, -2},  {3, -1},  {4, -1},  {4, 0},  {4, 0},  {3, 1},   {3, 2},   {3, 2},   {2, 3},
  },
  {
    {3, 8},   {2, 8},   {0, 9},  {-1, 8},  {-3, 8},  {-4, 7},  {-5, 7},  {-6, 6},  {-7, 4},
    {-8, 3},  {-8, 2},  {-9, 0}, {-8, -1}, {-8, -3}, {-7, -4}, {-7, -5}, {-6, -6}, {-4, -7},
    {-3, -8}, {-2, -8}, {0, -9}, {1, -8},  {3, -8},  {4, -7},  {5, -7},  {6, -6},  {7, -4},
    {8, -3},  {8, -2},  {9, 0},  {8, 1},   {8, 3},   {7, 4},   {7, 5},   {6, 6},   {4, 7},
  },
  {
    {4, -6},  {5, -5},  {6, -4},  {6, -3},  {7, -2},  {7, -1},  {7, 0},  {7, 2},   {7, 3},
    {6, 4},   {5, 5},   {4, 6},   {3, 6},   {2, 7},   {1, 7},   {0, 7},  {-2, 7},  {-3, 7},
    {-4, 6},  {-5, 5},  {-6, 4},  {-6, 3},  {-7, 2},  {-7, 1},  {-7, 0}, {-7, -2}, {-7, -3},
    {-6, -4}, {-5, -5}, {-4, -6}, {-3, -6}, {-2, -7}, {-1, -7}, {0, -7}, {2, -7},  {3, -7},
  },
  {
    {2, 6},   {1, 6},   {0, 6},  {-1, 6},  {-2, 6},  {-3, 5},  {-4, 5},  {-5, 4},  {-6, 3},
    {-6, 2},  {-6, 1},  {-6, 0}, {-6, -1}, {-6, -2}, {-5, -3}, {-5, -4}, {-4, -5}, {-3, -6},
    {-2, -6}, {-1, -6}, {0, -6}, {1, -6},  {2, -6},  {3, -5},  {4, -5},  {5, -4},  {6, -3},
    {6, -2},  {6, -1},  {6, 0},  {6, 1},   {6, 2},   {5, 3},   {5, 4},   {4, 5},   {3, 6},
  },
  {
    {12, -13}, {14, -11}, {16, -8},   {17, -5},   {18, -2},   {18, 1},   {17, 4},   {16, 7},
    {15, 10},  {13, 12},  {11, 14},   {8, 16},    {5, 17},    {2, 18},   {-1, 18},  {-4, 17},
    {-7, 16},  {-10, 15}, {-12, 13},  {-14, 11},  {-16, 8},   {-17, 5},  {-18, 2},  {-18, -1},
    {-17, -4}, {-16, -7}, {-15, -10}, {-13, -12}, {-11, -14}, {-8, -16}, {-5, -17}, {-2, -18},
    {1, -18},  {4, -17},  {7, -16},   {10, -15},
  },
  {
    {9, -12},  {11, -10}, {13, -8},  {14, -6},  {15, -3},   {15, -1},  {15, 2},   {14, 4},
    {13, 7},   {12, 9},   {10, 11},  {8, 13},   {6, 14},    {3, 15},   {1, 15},   {-2, 15},
    {-4, 14},  {-7, 13},  {-9, 12},  {-11, 10}, {-13, 8},   {-14, 6},  {-15, 3},  {-15, 1},
    {-15, -2}, {-14, -4}, {-13, -7}, {-12, -9}, {-10, -11}, {-8, -13}, {-6, -14}, {-3, -15},
    {-1, -15}, {2, -15},  {4, -14},  {7, -13},
  },
  {
    {10, 3},   {9, 5},   {8, 6},   {7, 8},   {6, 9},   {4, 10},   {2, 10},   {1, 10},   {-1, 10},
    {-3, 10},  {-5, 9},  {-6, 8},  {-8, 7},  {-9, 6},  {-10, 4},  {-10, 2},  {-10, 1},  {-10, -1},
    {-10, -3}, {-9, -5}, {-8, -6}, {-7, -8}, {-6, -9}, {-4, -10}, {-2, -10}, {-1, -10}, {1, -10},
    {3, -10},  {5, -9},  {6, -8},  {8, -7},  {9, -6},  {10, -4},  {10, -2},  {10, -1},  {10, 1},
  },
  {
    {-8, 4},  {-9, 3},  {-9, 1},  {-9, -1}, {-9, -2}, {-8, -4}, {-7, -5}, {-6, -6}, {-5, -7},
    {-4, -8}, {-3, -9}, {-1, -9}, {1, -9},  {2, -9},  {4, -8},  {5, -7},  {6, -6},  {7, -5},
    {8, -4},  {9, -3},  {9, -1},  {9, 1},   {9, 2},   {8, 4},   {7, 5},   {6, 6},   {5, 7},
    {4, 8},   {3, 9},   {1, 9},   {-1, 9},  {-2, 9},  {-4, 8},  {-5, 7},  {-6, 6},  {-7, 5},
  },
  {
    {-7, 9},  {-8, 8},  {-10, 6},  {-11, 4},  {-11, 2},  {-11, 0}, {-11, -2}, {-11, -3}, {-10, -5},
    {-9, -7}, {-8, -8}, {-6, -10}, {-4, -11}, {-2, -11}, {0, -11}, {2, -11},  {3, -11},  {5, -10},
    {7, -9},  {8, -8},  {10, -6},  {11, -4},  {11, -2},  {11, 0},  {11, 2},   {11, 3},   {10, 5},
    {9, 7},   {8, 8},   {6, 10},   {4, 11},   {2, 11},   {0, 11},  {-2, 11},  {-3, 11},  {-5, 10},
  },
  {
    {-11, 12}, {-13, 10},  {-14, 8},   {-16, 5},  {-16, 2},  {-16, -1}, {-16, -4}, {-15, -6},
    {-14, -9}, {-12, -11}, {-10, -13}, {-8, -14}, {-5, -16}, {-2, -16}, {1, -16},  {4, -16},
    {6, -15},  {9, -14},   {11, -12},  {13, -10}, {14, -8},  {16, -5},  {16, -2},  {16, 1},
    {16, 4},   {15, 6},    {14, 9},    {12, 11},  {10, 13},  {8, 14},   {5, 16},   {2, 16},
    {-1, 16},  {-4, 16},   {-6, 15},   {-9, 14},
  },
  {
    {-4, -6}, {-3, -7}, {-2, -7}, {0, -7}, {1, -7},  {2, -7},  {3, -6},  {4, -6},  {5, -5},
    {6, -4},  {7, -3},  {7, -2},  {7, 0},  {7, 1},   {7, 2},   {6, 3},   {6, 4},   {5, 5},
    {4, 6},   {3, 7},   {2, 7},   {0, 7},  {-1, 7},  {-2, 7},  {-3, 6},  {-4, 6},  {-5, 5},
    {-6, 4},  {-7, 3},  {-7, 2},  {-7, 0}, {-7, -1}, {-7, -2}, {-6, -3}, {-6, -4}, {-5, -5},
  },
  {
    {1, 12},   {-1, 12},  {-3, 12},  {-5, 11},  {-7, 10},  {-9, 8},   {-10, 7}, {-11, 5},
    {-12, 3},  {-12, 1},  {-12, -1}, {-12, -3}, {-11, -5}, {-10, -7}, {-8, -9}, {-7, -10},
    {-5, -11}, {-3, -12}, {-1, -12}, {1, -12},  {3, -12},  {5, -11},  {7, -10}, {9, -8},
    {10, -7},  {11, -5},  {12, -3},  {12, -1},  {12, 1},   {12, 3},   {11, 5},  {10, 7},
    {8, 9},    {7, 10},   {5, 11},   {3, 12},
  },
  {
    {2, -8},  {3, -8},  {5, -7},  {6, -6},  {7, -5},  {7, -4},  {8, -2},  {8, -1},  {8, 1},
    {8, 2},   {8, 3},   {7, 5},   {6, 6},   {5, 7},   {4, 7},   {2, 8},   {1, 8},   {-1, 8},
    {-2, 8},  {-3, 8},  {-5, 7},  {-6, 6},  {-7, 5},  {-7, 4},  {-8, 2},  {-8, 1},  {-8, -1},
    {-8, -2}, {-8, -3}, {-7, -5}, {-6, -6}, {-5, -7}, {-4, -7}, {-2, -8}, {-1, -8}, {1, -8},
  },
  {
    {6, -9},  {7, -8},  {9, -6},  {10, -5},  {10, -3},  {11, -1},  {11, 1},   {11, 3},   {10, 4},
    {9, 6},   {8, 7},   {6, 9},   {5, 10},   {3, 10},   {1, 11},   {-1, 11},  {-3, 11},  {-4, 10},
    {-6, 9},  {-7, 8},  {-9, 6},  {-10, 5},  {-10, 3},  {-11, 1},  {-11, -1}, {-11, -3}, {-10, -4},
    {-9, -6}, {-8, -7}, {-6, -9}, {-5, -10}, {-3, -10}, {-1, -11}, {1, -11},  {3, -11},  {4, -10},
  },
  {
    {7, -4},  {8, -3},  {8, -1},  {8, 0},  {8, 1},   {8, 3},   {7, 4},   {6, 5},   {5, 6},
    {4, 7},   {3, 8},   {1, 8},   {0, 8},  {-1, 8},  {-3, 8},  {-4, 7},  {-5, 6},  {-6, 5},
    {-7, 4},  {-8, 3},  {-8, 1},  {-8, 0}, {-8, -1}, {-8, -3}, {-7, -4}, {-6, -5}, {-5, -6},
    {-4, -7}, {-3, -8}, {-1, -8}, {0, -8}, {1, -8},  {3, -8},  {4, -7},  {5, -6},  {6, -5},
  },
  {
    {2, 3},   {1, 3},   {1, 4},   {0, 4},  {0, 4},  {-1, 3},  {-2, 3},  {-2, 3},  {-3, 2},
    {-3, 2},  {-3, 1},  {-4, 1},  {-4, 0}, {-4, 0}, {-3, -1}, {-3, -2}, {-3, -2}, {-2, -3},
    {-2, -3}, {-1, -3}, {-1, -4}, {0, -4}, {0, -4}, {1, -3},  {2, -3},  {2, -3},  {3, -2},
    {3, -2},  {3, -1},  {4, -1},  {4, 0},  {4, 0},  {3, 1},   {3, 2},   {3, 2},   {2, 3},
  },
  {
    {3, -2},  {3, -1},  {4, -1},  {4, 0},  {4, 0},  {3, 1},   {3, 2},   {3, 2},   {2, 3},
    {2, 3},   {1, 3},   {1, 4},   {0, 4},  {0, 4},  {-1, 3},  {-2, 3},  {-2, 3},  {-3, 2},
    {-3, 2},  {-3, 1},  {-4, 1},  {-4, 0}, {-4, 0}, {-3, -1}, {-3, -2}, {-3, -2}, {-2, -3},
    {-2, -3}, {-1, -3}, {-1, -4}, {0, -4}, {0, -4}, {1, -3},  {2, -3},  {2, -3},  {3, -2},
  },
  {
    {6, 3},   {5, 4},   {5, 5},   {4, 6},   {3, 6},   {2, 7},   {0, 7},  {-1, 7},  {-2, 6},
    {-3, 6},  {-4, 5},  {-5, 5},  {-6, 4},  {-6, 3},  {-7, 2},  {-7, 0}, {-7, -1}, {-6, -2},
    {-6, -3}, {-5, -4}, {-5, -5}, {-4, -6}, {-3, -6}, {-2, -7}, {0, -7}, {1, -7},  {2, -6},
    {3, -6},  {4, -5},  {5, -5},  {6, -4},  {6, -3},  {7, -2},  {7, 0},  {7, 1},   {6, 2},
  },
  {
    {11, 0},  {11, 2},   {10, 4},   {10, 6},   {8, 7},   {7, 8},   {6, 10},   {4, 10},   {2, 11},
    {0, 11},  {-2, 11},  {-4, 10},  {-6, 10},  {-7, 8},  {-8, 7},  {-10, 6},  {-10, 4},  {-11, 2},
    {-11, 0}, {-11, -2}, {-10, -4}, {-10, -6}, {-8, -7}, {-7, -8}, {-6, -10}, {-4, -10}, {-2, -11},
    {0, -11}, {2, -11},  {4, -10},  {6, -10},  {7, -8},  {8, -7},  {10, -6},  {10, -4},  {11, -2},
  },
  {
    {3, -3},  {3, -2},  {4, -2},  {4, -1},  {4, 0},  {4, 0},  {4, 1},   {4, 2},   {3, 2},
    {3, 3},   {2, 3},   {2, 4},   {1, 4},   {0, 4},  {0, 4},  {-1, 4},  {-2, 4},  {-2, 3},
    {-3, 3},  {-3, 2},  {-4, 2},  {-4, 1},  {-4, 0}, {-4, 0}, {-4, -1}, {-4, -2}, {-3, -2},
    {-3, -3}, {-2, -3}, {-2, -4}, {-1, -4}, {0, -4}, {0, -4}, {1, -4},  {2, -4},  {2, -3},
  },
  {
    {8, -8},  {9, -6},  {10, -5},  {11, -3},  {11, -1},  {11, 1},   {11, 3},   {10, 5},   {9, 6},
    {8, 8},   {6, 9},   {5, 10},   {3, 11},   {1, 11},   {-1, 11},  {-3, 11},  {-5, 10},  {-6, 9},
    {-8, 8},  {-9, 6},  {-10, 5},  {-11, 3},  {-11, 1},  {-11, -1}, {-11, -3}, {-10, -5}, {-9, -6},
    {-8, -8}, {-6, -9}, {-5, -10}, {-3, -11}, {-1, -11}, {1, -11},  {3, -11},  {5, -10},  {6, -9},
  },
  {
    {7, 8},   {6, 9},   {4, 10},   {2, 10},   {0, 11},  {-2, 11},  {-3, 10},  {-5, 9},  {-7, 8},
    {-8, 7},  {-9, 6},  {-10, 4},  {-10, 2},  {-11, 0}, {-11, -2}, {-10, -3}, {-9, -5}, {-8, -7},
    {-7, -8}, {-6, -9}, {-4, -10}, {-2, -10}, {0, -11}, {2, -11},  {3, -10},  {5, -9},  {7, -8},
    {8, -7},  {9, -6},  {10, -4},  {10, -2},  {11, 0},  {11, 2},   {10, 3},   {9, 5},   {8, 7},
  },
  {
    {9, 3},   {8, 5},   {7, 6},   {6, 7},   {5, 8},   {3, 9},   {2, 9},   {0, 9},  {-1, 9},
    {-3, 9},  {-5, 8},  {-6, 7},  {-7, 6},  {-8, 5},  {-9, 3},  {-9, 2},  {-9, 0}, {-9, -1},
    {-9, -3}, {-8, -5}, {-7, -6}, {-6, -7}, {-5, -8}, {-3, -9}, {-2, -9}, {0, -9}, {1, -9},
    {3, -9},  {5, -8},  {6, -7},  {7, -6},  {8, -5},  {9, -3},  {9, -2},  {9, 0},  {9, 1},
  },
  {
    {-11, -5}, {-10, -7}, {-9, -8},  {-7, -10}, {-5, -11}, {-3, -12}, {-1, -12}, {1, -12},
    {3, -12},  {5, -11},  {7, -10},  {8, -9},   {10, -7},  {11, -5},  {12, -3},  {12, -1},
    {12, 1},   {12, 3},   {11, 5},   {10, 7},   {9, 8},    {7, 10},   {5, 11},   {3, 12},
    {1, 12},   {-1, 12},  {-3, 12},  {-5, 11},  {-7, 10},  {-8, 9},   {-10, 7},  {-11, 5},
    {-12, 3},  {-12, 1},  {-12, -1}, {-12, -3},
  },
  {
    {-6, -4}, {-5, -5}, {-4, -6}, {-3, -6}, {-2, -7}, {-1, -7}, {0, -7}, {2, -7},  {3, -7},
    {4, -6},  {5, -5},  {6, -4},  {6, -3},  {7, -2},  {7, -1},  {7, 0},  {7, 2},   {7, 3},
    {6, 4},   {5, 5},   {4, 6},   {3, 6},   {2, 7},   {1, 7},   {0, 7},  {-2, 7},  {-3, 7},
    {-4, 6},  {-5, 5},  {-6, 4},  {-6, 3},  {-7, 2},  {-7, 1},  {-7, 0}, {-7, -2}, {-7, -3},
  },
  {
    {-10, 11}, {-12, 9},   {-13, 7},  {-14, 5},  {-15, 2},  {-15, -1}, {-15, -3}, {-14, -6},
    {-13, -8}, {-11, -10}, {-9, -12}, {-7, -13}, {-5, -14}, {-2, -15}, {1, -15},  {3, -15},
    {6, -14},  {8, -13},   {10, -11}, {12, -9},  {13, -7},  {14, -5},  {15, -2},  {15, 1},
    {15, 3},   {14, 6},    {13, 8},   {11, 10},  {9, 12},   {7, 13},   {5, 14},   {2, 15},
    {-1, 15},  {-3, 15},   {-6, 14},  {-8, 13},
  },
  {
    {-5, 10},  {-7, 9},  {-8, 8},  {-9, 6},  {-10, 4},  {-11, 3},  {-11, 1},  {-11, -1}, {-11, -3},
    {-10, -5}, {-9, -7}, {-8, -8}, {-6, -9}, {-4, -10}, {-3, -11}, {-1, -11}, {1, -11},  {3, -11},
    {5, -10},  {7, -9},  {8, -8},  {9, -6},  {10, -4},  {11, -3},  {11, -1},  {11, 1},   {11, 3},
    {10, 5},   {9, 7},   {8, 8},   {6, 9},   {4, 10},   {3, 11},   {1, 11},   {-1, 11},  {-3, 11},
  },
  {
    {-5, -8}, {-4, -9}, {-2, -9}, {0, -9}, {1, -9},  {3, -9},  {4, -8},  {6, -7},  {7, -6},
    {8, -5},  {9, -4},  {9, -2},  {9, 0},  {9, 1},   {9, 3},   {8, 4},   {7, 6},   {6, 7},
    {5, 8},   {4, 9},   {2, 9},   {0, 9},  {-1, 9},  {-3, 9},  {-4, 8},  {-6, 7},  {-7, 6},
    {-8, 5},  {-9, 4},  {-9, 2},  {-9, 0}, {-9, -1}, {-9, -3}, {-8, -4}, {-7, -6}, {-6, -7},
  },
  {
    {-3, 12},  {-5, 11},  {-7, 10},  {-9, 9},   {-10, 7}, {-11, 5},  {-12, 3},  {-12, 1},
    {-12, -1}, {-12, -3}, {-11, -5}, {-10, -7}, {-9, -9}, {-7, -10}, {-5, -11}, {-3, -12},
    {-1, -12}, {1, -12},  {3, -12},  {5, -11},  {7, -10}, {9, -9},   {10, -7},  {11, -5},
    {12, -3},  {12, -1},  {12, 1},   {12, 3},   {11, 5},  {10, 7},   {9, 9},    {7, 10},
    {5, 11},   {3, 12},   {1, 12},   {-1, 12},
  },
  {
    {-10, 5},  {-11, 3},  {-11, 1},  {-11, -1}, {-11, -3}, {-10, -4}, {-9, -6}, {-8, -8}, {-7, -9},
    {-5, -10}, {-3, -11}, {-1, -11}, {1, -11},  {3, -11},  {4, -10},  {6, -9},  {8, -8},  {9, -7},
    {10, -5},  {11, -3},  {11, -1},  {11, 1},   {11, 3},   {10, 4},   {9, 6},   {8, 8},   {7, 9},
    {5, 10},   {3, 11},   {1, 11},   {-1, 11},  {-3, 11},  {-4, 10},  {-6, 9},  {-8, 8},  {-9, 7},
  },
  {
    {-9, 0}, {-9, -2}, {-8, -3}, {-8, -5}, {-7, -6}, {-6, -7}, {-5, -8}, {-3, -8}, {-2, -9},
    {0, -9}, {2, -9},  {3, -8},  {5, -8},  {6, -7},  {7, -6},  {8, -5},  {8, -3},  {9, -2},
    {9, 0},  {9, 2},   {8, 3},   {8, 5},   {7, 6},   {6, 7},   {5, 8},   {3, 8},   {2, 9},
    {0, 9},  {-2, 9},  {-3, 8},  {-5, 8},  {-6, 7},  {-7, 6},  {-8, 5},  {-8, 3},  {-9, 2},
  },
  {
    {8, -1},  {8, 0},  {8, 2},   {7, 3},   {7, 4},   {6, 5},   {5, 6},   {4, 7},   {2, 8},
    {1, 8},   {0, 8},  {-2, 8},  {-3, 7},  {-4, 7},  {-5, 6},  {-6, 5},  {-7, 4},  {-8, 2},
    {-8, 1},  {-8, 0}, {-8, -2}, {-7, -3}, {-7, -4}, {-6, -5}, {-5, -6}, {-4, -7}, {-2, -8},
    {-1, -8}, {0, -8}, {2, -8},  {3, -7},  {4, -7},  {5, -6},  {6, -5},  {7, -4},  {8, -2},
  },
  {
    {12, -6},  {13, -4},  {13, -2},  {13, 1},   {13, 3},   {12, 5},   {11, 7},   {10, 9},
    {8, 11},   {6, 12},   {4, 13},   {2, 13},   {-1, 13},  {-3, 13},  {-5, 12},  {-7, 11},
    {-9, 10},  {-11, 8},  {-12, 6},  {-13, 4},  {-13, 2},  {-13, -1}, {-13, -3}, {-12, -5},
    {-11, -7}, {-10, -9}, {-8, -11}, {-6, -12}, {-4, -13}, {-2, -13}, {1, -13},  {3, -13},
    {5, -12},  {7, -11},  {9, -10},  {11, -8},
  },
  {
    {4, -6},  {5, -5},  {6, -4},  {6, -3},  {7, -2},  {7, -1},  {7, 0},  {7, 2},   {7, 3},
    {6, 4},   {5, 5},   {4, 6},   {3, 6},   {2, 7},   {1, 7},   {0, 7},  {-2, 7},  {-3, 7},
    {-4, 6},  {-5, 5},  {-6, 4},  {-6, 3},  {-7, 2},  {-7, 1},  {-7, 0}, {-7, -2}, {-7, -3},
    {-6, -4}, {-5, -5}, {-4, -6}, {-3, -6}, {-2, -7}, {-1, -7}, {0, -7}, {2, -7},  {3, -7},
  },
  {
    {6, -11},  {8, -10},  {9, -8},  {11, -7},  {12, -5},  {12, -2},  {13, 0},  {12, 2},   {12, 4},
    {11, 6},   {10, 8},   {8, 9},   {7, 11},   {5, 12},   {2, 12},   {0, 13},  {-2, 12},  {-4, 12},
    {-6, 11},  {-8, 10},  {-9, 8},  {-11, 7},  {-12, 5},  {-12, 2},  {-13, 0}, {-12, -2}, {-12, -4},
    {-11, -6}, {-10, -8}, {-8, -9}, {-7, -11}, {-5, -12}, {-2, -12}, {0, -13}, {2, -12},  {4, -12},
  },
  {
    {-10, 12}, {-12, 10},  {-14, 8},   {-15, 5},  {-15, 3},  {-16, 0},  {-15, -3}, {-15, -5},
    {-14, -8}, {-12, -10}, {-10, -12}, {-8, -14}, {-5, -15}, {-3, -15}, {0, -16},  {3, -15},
    {5, -15},  {8, -14},   {10, -12},  {12, -10}, {14, -8},  {15, -5},  {15, -3},  {16, 0},
    {15, 3},   {15, 5},    {14, 8},    {12, 10},  {10, 12},  {8, 14},   {5, 15},   {3, 15},
    {0, 16},   {-3, 15},   {-5, 15},   {-8, 14},
  },
  {
    {-8, 7},  {-9, 6},  {-10, 4},  {-10, 2},  {-11, 0}, {-11, -2}, {-10, -3}, {-9, -5}, {-8, -7},
    {-7, -8}, {-6, -9}, {-4, -10}, {-2, -10}, {0, -11}, {2, -11},  {3, -10},  {5, -9},  {7, -8},
    {8, -7},  {9, -6},  {10, -4},  {10, -2},  {11, 0},  {11, 2},   {10, 3},   {9, 5},   {8, 7},
    {7, 8},   {6, 9},   {4, 10},   {2, 10},   {0, 11},  {-2, 11},  {-3, 10},  {-5, 9},  {-7, 8},
  },
  {
    {4, -2},  {4, -1},  {4, -1},  {4, 0},  {4, 1},   {4, 2},   {4, 2},   {3, 3},   {3, 4},
    {2, 4},   {1, 4},   {1, 4},   {0, 4},  {-1, 4},  {-2, 4},  {-2, 4},  {-3, 3},  {-4, 3},
    {-4, 2},  {-4, 1},  {-4, 1},  {-4, 0}, {-4, -1}, {-4, -2}, {-4, -2}, {-3, -3}, {-3, -4},
    {-2, -4}, {-1, -4}, {-1, -4}, {0, -4}, {1, -4},  {2, -4},  {2, -4},  {3, -3},  {4, -3},
  },
  {
    {6, 7},   {5, 8},   {3, 9},   {2, 9},   {0, 9},  {-2, 9},  {-3, 9},  {-5, 8},  {-6, 7},
    {-7, 6},  {-8, 5},  {-9, 3},  {-9, 2},  {-9, 0}, {-9, -2}, {-9, -3}, {-8, -5}, {-7, -6},
    {-6, -7}, {-5, -8}, {-3, -9}, {-2, -9}, {0, -9}, {2, -9},  {3, -9},  {5, -8},  {6, -7},
    {7, -6},  {8, -5},  {9, -3},  {9, -2},  {9, 0},  {9, 2},   {9, 3},   {8, 5},   {7, 6},
  },
  {
    {-2, 0}, {-2, 0}, {-2, -1}, {-2, -1}, {-2, -1}, {-1, -2}, {-1, -2}, {-1, -2}, {0, -2},
    {0, -2}, {0, -2}, {1, -2},  {1, -2},  {1, -2},  {2, -1},  {2, -1},  {2, -1},  {2, 0},
    {2, 0},  {2, 0},  {2, 1},   {2, 1},   {2, 1},   {1, 2},   {1, 2},   {1, 2},   {0, 2},
    {0, 2},  {0, 2},  {-1, 2},  {-1, 2},  {-1, 2},  {-2, 1},  {-2, 1},  {-2, 1},  {-2, 0},
  },
  {
    {-2, 12},  {-4, 11},  {-6, 11},  {-8, 9},  {-9, 8},  {-10, 6},  {-11, 4},  {-12, 2},  {-12, 0},
    {-12, -2}, {-11, -4}, {-11, -6}, {-9, -8}, {-8, -9}, {-6, -10}, {-4, -11}, {-2, -12}, {0, -12},
    {2, -12},  {4, -11},  {6, -11},  {8, -9},  {9, -8},  {10, -6},  {11, -4},  {12, -2},  {12, 0},
    {12, 2},   {11, 4},   {11, 6},   {9, 8},   {8, 9},   {6, 10},   {4, 11},   {2, 12},   {0, 12},
  },
  {
    {-5, -8}, {-4, -9}, {-2, -9}, {0, -9}, {1, -9},  {3, -9},  {4, -8},  {6, -7},  {7, -6},
    {8, -5},  {9, -4},  {9, -2},  {9, 0},  {9, 1},   {9, 3},   {8, 4},   {7, 6},   {6, 7},
    {5, 8},   {4, 9},   {2, 9},   {0, 9},  {-1, 9},  {-3, 9},  {-4, 8},  {-6, 7},  {-7, 6},
    {-8, 5},  {-9, 4},  {-9, 2},  {-9, 0}, {-9, -1}, {-9, -3}, {-8, -4}, {-7, -6}, {-6, -7},
  },
  {
    {-5, 2},  {-5, 1},  {-5, 0}, {-5, -1}, {-5, -2}, {-5, -3}, {-4, -3}, {-4, -4}, {-3, -5},
    {-2, -5}, {-1, -5}, {0, -5}, {1, -5},  {2, -5},  {3, -5},  {3, -4},  {4, -4},  {5, -3},
    {5, -2},  {5, -1},  {5, 0},  {5, 1},   {5, 2},   {5, 3},   {4, 3},   {4, 4},   {3, 5},
    {2, 5},   {1, 5},   {0, 5},  {-1, 5},  {-2, 5},  {-3, 5},  {-3, 4},  {-4, 4},  {-5, 3},
  },
  {
    {7, -6},  {8, -5},  {9, -3},  {9, -2},  {9, 0},  {9, 2},   {9, 3},   {8, 5},   {7, 6},
    {6, 7},   {5, 8},   {3, 9},   {2, 9},   {0, 9},  {-2, 9},  {-3, 9},  {-5, 8},  {-6, 7},
    {-7, 6},  {-8, 5},  {-9, 3},  {-9, 2},  {-9, 0}, {-9, -2}, {-9, -3}, {-8, -5}, {-7, -6},
    {-6, -7}, {-5, -8}, {-3, -9}, {-2, -9}, {0, -9}, {2, -9},  {3, -9},  {5, -8},  {6, -7},
  },
  {
    {10, 12},  {8, 14},    {5, 15},    {3, 15},   {0, 16},   {-3, 15},  {-5, 15},  {-8, 14},
    {-10, 12}, {-12, 10},  {-14, 8},   {-15, 5},  {-15, 3},  {-16, 0},  {-15, -3}, {-15, -5},
    {-14, -8}, {-12, -10}, {-10, -12}, {-8, -14}, {-5, -15}, {-3, -15}, {0, -16},  {3, -15},
    {5, -15},  {8, -14},   {10, -12},  {12, -10}, {14, -8},  {15, -5},  {15, -3},  {16, 0},
    {15, 3},   {15, 5},    {14, 8},    {12, 10},
  },
  {
    {-9, -13}, {-7, -14}, {-4, -15}, {-1, -16},  {1, -16}, {4, -15}, {7, -14}, {9, -13},
    {11, -11}, {13, -9},  {14, -7},  {15, -4},   {16, -1}, {16, 1},  {15, 4},  {14, 7},
    {13, 9},   {11, 11},  {9, 13},   {7, 14},    {4, 15},  {1, 16},  {-1, 16}, {-4, 15},
    {-7, 14},  {-9, 13},  {-11, 11}, {-13, 9},   {-14, 7}, {-15, 4}, {-16, 1}, {-16, -1},
    {-15, -4}, {-14, -7}, {-13, -9}, {-11, -11},
  },
  {
    {-8, -8}, {-6, -9}, {-5, -10}, {-3, -11}, {-1, -11}, {1, -11},  {3, -11},  {5, -10},  {6, -9},
    {8, -8},  {9, -6},  {10, -5},  {11, -3},  {11, -1},  {11, 1},   {11, 3},   {10, 5},   {9, 6},
    {8, 8},   {6, 9},   {5, 10},   {3, 11},   {1, 11},   {-1, 11},  {-3, 11},  {-5, 10},  {-6, 9},
    {-8, 8},  {-9, 6},  {-10, 5},  {-11, 3},  {-11, 1},  {-11, -1}, {-11, -3}, {-10, -5}, {-9, -6},
  },
  {
    {-5, -13}, {-3, -14}, {0, -14},  {2, -14},  {5, -13}, {7, -12}, {9, -11},  {11, -9},
    {12, -7},  {13, -5},  {14, -3},  {14, 0},   {14, 2},  {13, 5},  {12, 7},   {11, 9},
    {9, 11},   {7, 12},   {5, 13},   {3, 14},   {0, 14},  {-2, 14}, {-5, 13},  {-7, 12},
    {-9, 11},  {-11, 9},  {-12, 7},  {-13, 5},  {-14, 3}, {-14, 0}, {-14, -2}, {-13, -5},
    {-12, -7}, {-11, -9}, {-9, -11}, {-7, -12},
  },
  {
    {-5, -2}, {-5, -3}, {-4, -4}, {-3, -4}, {-3, -5}, {-2, -5}, {-1, -5}, {0, -5}, {1, -5},
    {2, -5},  {3, -5},  {4, -4},  {4, -3},  {5, -3},  {5, -2},  {5, -1},  {5, 0},  {5, 1},
    {5, 2},   {5, 3},   {4, 4},   {3, 4},   {3, 5},   {2, 5},   {1, 5},   {0, 5},  {-1, 5},
    {-2, 5},  {-3, 5},  {-4, 4},  {-4, 3},  {-5, 3},  {-5, 2},  {-5, 1},  {-5, 0}, {-5, -1},
  },
  {
    {8, -8},  {9, -6},  {10, -5},  {11, -3},  {11, -1},  {11, 1},   {11, 3},   {10, 5},   {9, 6},
    {8, 8},   {6, 9},   {5, 10},   {3, 11},   {1, 11},   {-1, 11},  {-3, 11},  {-5, 10},  {-6, 9},
    {-8, 8},  {-9, 6},  {-10, 5},  {-11, 3},  {-11, 1},  {-11, -1}, {-11, -3}, {-10, -5}, {-9, -6},
    {-8, -8}, {-6, -9}, {-5, -10}, {-3, -11}, {-1, -11}, {1, -11},  {3, -11},  {5, -10},  {6, -9},
  },
  {
    {9, -13},  {11, -11}, {13, -9},  {14, -7},  {15, -4},   {16, -1},  {16, 1},   {15, 4},
    {14, 7},   {13, 9},   {11, 11},  {9, 13},   {7, 14},    {4, 15},   {1, 16},   {-1, 16},
    {-4, 15},  {-7, 14},  {-9, 13},  {-11, 11}, {-13, 9},   {-14, 7},  {-15, 4},  {-16, 1},
    {-16, -1}, {-15, -4}, {-14, -7}, {-13, -9}, {-11, -11}, {-9, -13}, {-7, -14}, {-4, -15},
    {-1, -16}, {1, -16},  {4, -15},  {7, -14},
  },
  {
    {-9, -11}, {-7, -12}, {-5, -13}, {-2, -14}, {0, -14}, {3, -14}, {5, -13}, {7, -12},
    {9, -11},  {11, -9},  {12, -7},  {13, -5},  {14, -2}, {14, 0},  {14, 3},  {13, 5},
    {12, 7},   {11, 9},   {9, 11},   {7, 12},   {5, 13},  {2, 14},  {0, 14},  {-3, 14},
    {-5, 13},  {-7, 12},  {-9, 11},  {-11, 9},  {-12, 7}, {-13, 5}, {-14, 2}, {-14, 0},
    {-14, -3}, {-13, -5}, {-12, -7}, {-11, -9},
  },
  {
    {-9, 0}, {-9, -2}, {-8, -3}, {-8, -5}, {-7, -6}, {-6, -7}, {-5, -8}, {-3, -8}, {-2, -9},
    {0, -9}, {2, -9},  {3, -8},  {5, -8},  {6, -7},  {7, -6},  {8, -5},  {8, -3},  {9, -2},
    {9, 0},  {9, 2},   {8, 3},   {8, 5},   {7, 6},   {6, 7},   {5, 8},   {3, 8},   {2, 9},
    {0, 9},  {-2, 9},  {-3, 8},  {-5, 8},  {-6, 7},  {-7, 6},  {-8, 5},  {-8, 3},  {-9, 2},
  },
  {
    {1, -8},  {2, -8},  {4, -7},  {5, -6},  {6, -5},  {7, -4},  {7, -3},  {8, -2},  {8, 0},
    {8, 1},   {8, 2},   {7, 4},   {6, 5},   {5, 6},   {4, 7},   {3, 7},   {2, 8},   {0, 8},
    {-1, 8},  {-2, 8},  {-4, 7},  {-5, 6},  {-6, 5},  {-7, 4},  {-7, 3},  {-8, 2},  {-8, 0},
    {-8, -1}, {-8, -2}, {-7, -4}, {-6, -5}, {-5, -6}, {-4, -7}, {-3, -7}, {-2, -8}, {0, -8},
  },
  {
    {1, -2},  {1, -2},  {2, -2},  {2, -1},  {2, -1},  {2, -1},  {2, 0},  {2, 0},  {2, 1},
    {2, 1},   {2, 1},   {2, 2},   {1, 2},   {1, 2},   {1, 2},   {0, 2},  {0, 2},  {-1, 2},
    {-1, 2},  {-1, 2},  {-2, 2},  {-2, 1},  {-2, 1},  {-2, 1},  {-2, 0}, {-2, 0}, {-2, -1},
    {-2, -1}, {-2, -1}, {-2, -2}, {-1, -2}, {-1, -2}, {-1, -2}, {0, -2}, {0, -2}, {1, -2},
  },
  {
    {7, -4},  {8, -3},  {8, -1},  {8, 0},  {8, 1},   {8, 3},   {7, 4},   {6, 5},   {5, 6},
    {4, 7},   {3, 8},   {1, 8},   {0, 8},  {-1, 8},  {-3, 8},  {-4, 7},  {-5, 6},  {-6, 5},
    {-7, 4},  {-8, 3},  {-8, 1},  {-8, 0}, {-8, -1}, {-8, -3}, {-7, -4}, {-6, -5}, {-5, -6},
    {-4, -7}, {-3, -8}, {-1, -8}, {0, -8}, {1, -8},  {3, -8},  {4, -7},  {5, -6},  {6, -5},
  },
  {
    {9, 1},   {9, 3},   {8, 4},   {7, 5},   {6, 7},   {5, 8},   {4, 8},   {2, 9},   {1, 9},
    {-1, 9},  {-3, 9},  {-4, 8},  {-5, 7},  {-7, 6},  {-8, 5},  {-8, 4},  {-9, 2},  {-9, 1},
    {-9, -1}, {-9, -3}, {-8, -4}, {-7, -5}, {-6, -7}, {-5, -8}, {-4, -8}, {-2, -9}, {-1, -9},
    {1, -9},  {3, -9},  {4, -8},  {5, -7},  {7, -6},  {8, -5},  {8, -4},  {9, -2},  {9, -1},
  },
  {
    {-2, 1},  {-2, 1},  {-2, 0}, {-2, 0}, {-2, -1}, {-2, -1}, {-2, -1}, {-2, -2}, {-1, -2},
    {-1, -2}, {-1, -2}, {0, -2}, {0, -2}, {1, -2},  {1, -2},  {1, -2},  {2, -2},  {2, -1},
    {2, -1},  {2, -1},  {2, 0},  {2, 0},  {2, 1},   {2, 1},   {2, 1},   {2, 2},   {1, 2},
    {1, 2},   {1, 2},   {0, 2},  {0, 2},  {-1, 2},  {-1, 2},  {-1, 2},  {-2, 2},  {-2, 1},
  },
  {
    {-1, -4}, {0, -4}, {0, -4}, {1, -4},  {2, -4},  {2, -3},  {3, -3},  {3, -2},  {4, -2},
    {4, -1},  {4, 0},  {4, 0},  {4, 1},   {4, 2},   {3, 2},   {3, 3},   {2, 3},   {2, 4},
    {1, 4},   {0, 4},  {0, 4},  {-1, 4},  {-2, 4},  {-2, 3},  {-3, 3},  {-3, 2},  {-4, 2},
    {-4, 1},  {-4, 0}, {-4, 0}, {-4, -1}, {-4, -2}, {-3, -2}, {-3, -3}, {-2, -3}, {-2, -4},
  },
  {
    {11, -6},  {12, -4},  {12, -2},  {13, 0},  {12, 2},   {12, 5},   {11, 7},   {9, 8},   {8, 10},
    {6, 11},   {4, 12},   {2, 12},   {0, 13},  {-2, 12},  {-5, 12},  {-7, 11},  {-8, 9},  {-10, 8},
    {-11, 6},  {-12, 4},  {-12, 2},  {-13, 0}, {-12, -2}, {-12, -5}, {-11, -7}, {-9, -8}, {-8, -10},
    {-6, -11}, {-4, -12}, {-2, -12}, {0, -13}, {2, -12},  {5, -12},  {7, -11},  {8, -9},  {10, -8},
  },
  {
    {12, -11}, {14, -9},  {15, -6},   {16, -4},   {16, -1},  {16, 2},   {16, 5},   {14, 8},
    {13, 10},  {11, 12},  {9, 14},    {6, 15},    {4, 16},   {1, 16},   {-2, 16},  {-5, 16},
    {-8, 14},  {-10, 13}, {-12, 11},  {-14, 9},   {-15, 6},  {-16, 4},  {-16, 1},  {-16, -2},
    {-16, -5}, {-14, -8}, {-13, -10}, {-11, -12}, {-9, -14}, {-6, -15}, {-4, -16}, {-1, -16},
    {2, -16},  {5, -16},  {8, -14},   {10, -13},
  },
  {
    {-12, -9}, {-10, -11}, {-8, -13}, {-6, -14}, {-3, -15}, {-1, -15}, {2, -15}, {4, -14},
    {7, -13},  {9, -12},   {11, -10}, {13, -8},  {14, -6},  {15, -3},  {15, -1}, {15, 2},
    {14, 4},   {13, 7},    {12, 9},   {10, 11},  {8, 13},   {6, 14},   {3, 15},  {1, 15},
    {-2, 15},  {-4, 14},   {-7, 13},  {-9, 12},  {-11, 10}, {-13, 8},  {-14, 6}, {-15, 3},
    {-15, 1},  {-15, -2},  {-14, -4}, {-13, -7},
  },
  {
    {-6, 4},  {-7, 3},  {-7, 2},  {-7, 0}, {-7, -1}, {-7, -2}, {-6, -3}, {-6, -4}, {-5, -5},
    {-4, -6}, {-3, -7}, {-2, -7}, {0, -7}, {1, -7},  {2, -7},  {3, -6},  {4, -6},  {5, -5},
    {6, -4},  {7, -3},  {7, -2},  {7, 0},  {7, 1},   {7, 2},   {6, 3},   {6, 4},   {5, 5},
    {4, 6},   {3, 7},   {2, 7},   {0, 7},  {-1, 7},  {-2, 7},  {-3, 6},  {-4, 6},  {-5, 5},
  },
  {
    {3, 7},   {2, 7},   {0, 8},  {-1, 8},  {-2, 7},  {-3, 7},  {-5, 6},  {-6, 5},  {-6, 4},
    {-7, 3},  {-7, 2},  {-8, 0}, {-8, -1}, {-7, -2}, {-7, -3}, {-6, -5}, {-5, -6}, {-4, -6},
    {-3, -7}, {-2, -7}, {0, -8}, {1, -8},  {2, -7},  {3, -7},  {5, -6},  {6, -5},  {6, -4},
    {7, -3},  {7, -2},  {8, 0},  {8, 1},   {7, 2},   {7, 3},   {6, 5},   {5, 6},   {4, 6},
  },
  {
    {7, 12},   {5, 13},   {2, 14},   {0, 14},   {-2, 14},  {-5, 13},  {-7, 12},  {-9, 11},
    {-11, 9},  {-12, 7},  {-13, 5},  {-14, 2},  {-14, 0},  {-14, -2}, {-13, -5}, {-12, -7},
    {-11, -9}, {-9, -11}, {-7, -12}, {-5, -13}, {-2, -14}, {0, -14},  {2, -14},  {5, -13},
    {7, -12},  {9, -11},  {11, -9},  {12, -7},  {13, -5},  {14, -2},  {14, 0},   {14, 2},
    {13, 5},   {12, 7},   {11, 9},   {9, 11},
  },
  {
    {5, 5},   {4, 6},   {3, 6},   {2, 7},   {1, 7},   {-1, 7},  {-2, 7},  {-3, 6},  {-4, 6},
    {-5, 5},  {-6, 4},  {-6, 3},  {-7, 2},  {-7, 1},  {-7, -1}, {-7, -2}, {-6, -3}, {-6, -4},
    {-5, -5}, {-4, -6}, {-3, -6}, {-2, -7}, {-1, -7}, {1, -7},  {2, -7},  {3, -6},  {4, -6},
    {5, -5},  {6, -4},  {6, -3},  {7, -2},  {7, -1},  {7, 1},   {7, 2},   {6, 3},   {6, 4},
  },
  {
    {10, 8},   {8, 10},   {7, 11},   {5, 12},   {3, 13},   {0, 13},   {-2, 13},  {-4, 12},
    {-6, 11},  {-8, 10},  {-10, 8},  {-11, 7},  {-12, 5},  {-13, 3},  {-13, 0},  {-13, -2},
    {-12, -4}, {-11, -6}, {-10, -8}, {-8, -10}, {-7, -11}, {-5, -12}, {-3, -13}, {0, -13},
    {2, -13},  {4, -12},  {6, -11},  {8, -10},  {10, -8},  {11, -7},  {12, -5},  {13, -3},
    {13, 0},   {13, 2},   {12, 4},   {11, 6},
  },
  {
    {0, -4}, {1, -4},  {1, -4},  {2, -3},  {3, -3},  {3, -3},  {3, -2},  {4, -1},  {4, -1},
    {4, 0},  {4, 1},   {4, 1},   {3, 2},   {3, 3},   {3, 3},   {2, 3},   {1, 4},   {1, 4},
    {0, 4},  {-1, 4},  {-1, 4},  {-2, 3},  {-3, 3},  {-3, 3},  {-3, 2},  {-4, 1},  {-4, 1},
    {-4, 0}, {-4, -1}, {-4, -1}, {-3, -2}, {-3, -3}, {-3, -3}, {-2, -3}, {-1, -4}, {-1, -4},
  },
  {
    {2, 8},   {1, 8},   {-1, 8},  {-2, 8},  {-4, 7},  {-5, 7},  {-6, 6},  {-7, 5},  {-8, 3},
    {-8, 2},  {-8, 1},  {-8, -1}, {-8, -2}, {-7, -4}, {-7, -5}, {-6, -6}, {-5, -7}, {-3, -8},
    {-2, -8}, {-1, -8}, {1, -8},  {2, -8},  {4, -7},  {5, -7},  {6, -6},  {7, -5},  {8, -3},
    {8, -2},  {8, -1},  {8, 1},   {8, 2},   {7, 4},   {7, 5},   {6, 6},   {5, 7},   {3, 8},
  },
  {
    {-9, 12},  {-11, 10}, {-13, 8},   {-14, 6},  {-15, 3},  {-15, 1},  {-15, -2}, {-14, -4},
    {-13, -7}, {-12, -9}, {-10, -11}, {-8, -13}, {-6, -14}, {-3, -15}, {-1, -15}, {2, -15},
    {4, -14},  {7, -13},  {9, -12},   {11, -10}, {13, -8},  {14, -6},  {15, -3},  {15, -1},
    {15, 2},   {14, 4},   {13, 7},    {12, 9},   {10, 11},  {8, 13},   {6, 14},   {3, 15},
    {1, 15},   {-2, 15},  {-4, 14},   {-7, 13},
  },
  {
    {-5, -13}, {-3, -14}, {0, -14},  {2, -14},  {5, -13}, {7, -12}, {9, -11},  {11, -9},
    {12, -7},  {13, -5},  {14, -3},  {14, 0},   {14, 2},  {13, 5},  {12, 7},   {11, 9},
    {9, 11},   {7, 12},   {5, 13},   {3, 14},   {0, 14},  {-2, 14}, {-5, 13},  {-7, 12},
    {-9, 11},  {-11, 9},  {-12, 7},  {-13, 5},  {-14, 3}, {-14, 0}, {-14, -2}, {-13, -5},
    {-12, -7}, {-11, -9}, {-9, -11}, {-7, -12},
  },
  {
    {0, 7},  {-1, 7},  {-2, 7},  {-4, 6},  {-4, 5},  {-5, 4},  {-6, 4},  {-7, 2},  {-7, 1},
    {-7, 0}, {-7, -1}, {-7, -2}, {-6, -4}, {-5, -4}, {-4, -5}, {-4, -6}, {-2, -7}, {-1, -7},
    {0, -7}, {1, -7},  {2, -7},  {4, -6},  {4, -5},  {5, -4},  {6, -4},  {7, -2},  {7, -1},
    {7, 0},  {7, 1},   {7, 2},   {6, 4},   {5, 4},   {4, 5},   {4, 6},   {2, 7},   {1, 7},
  },
  {
    {2, 12},   {0, 12},  {-2, 12},  {-4, 11},  {-6, 10},  {-8, 9},  {-9, 8},  {-11, 6},  {-11, 4},
    {-12, 2},  {-12, 0}, {-12, -2}, {-11, -4}, {-10, -6}, {-9, -8}, {-8, -9}, {-6, -11}, {-4, -11},
    {-2, -12}, {0, -12}, {2, -12},  {4, -11},  {6, -10},  {8, -9},  {9, -8},  {11, -6},  {11, -4},
    {12, -2},  {12, 0},  {12, 2},   {11, 4},   {10, 6},   {9, 8},   {8, 9},   {6, 11},   {4, 11},
  },
  {
    {-1, 2},  {-1, 2},  {-2, 2},  {-2, 1},  {-2, 1},  {-2, 1},  {-2, 0}, {-2, 0}, {-2, -1},
    {-2, -1}, {-2, -1}, {-2, -2}, {-1, -2}, {-1, -2}, {-1, -2}, {0, -2}, {0, -2}, {1, -2},
    {1, -2},  {1, -2},  {2, -2},  {2, -1},  {2, -1},  {2, -1},  {2, 0},  {2, 0},  {2, 1},
    {2, 1},   {2, 1},   {2, 2},   {1, 2},   {1, 2},   {1, 2},   {0, 2},  {0, 2},  {-1, 2},
  },
  {
    {1, 7},   {0, 7},  {-1, 7},  {-3, 7},  {-4, 6},  {-5, 5},  {-6, 4},  {-6, 3},  {-7, 2},
    {-7, 1},  {-7, 0}, {-7, -1}, {-7, -3}, {-6, -4}, {-5, -5}, {-4, -6}, {-3, -6}, {-2, -7},
    {-1, -7}, {0, -7}, {1, -7},  {3, -7},  {4, -6},  {5, -5},  {6, -4},  {6, -3},  {7, -2},
    {7, -1},  {7, 0},  {7, 1},   {7, 3},   {6, 4},   {5, 5},   {4, 6},   {3, 6},   {2, 7},
  },
  {
    {5, 11},  {3, 12},   {1, 12},   {-1, 12},  {-3, 12},  {-5, 11},  {-7, 10},  {-9, 8},
    {-10, 7}, {-11, 5},  {-12, 3},  {-12, 1},  {-12, -1}, {-12, -3}, {-11, -5}, {-10, -7},
    {-8, -9}, {-7, -10}, {-5, -11}, {-3, -12}, {-1, -12}, {1, -12},  {3, -12},  {5, -11},
    {7, -10}, {9, -8},   {10, -7},  {11, -5},  {12, -3},  {12, -1},  {12, 1},   {12, 3},
    {11, 5},  {10, 7},   {8, 9},    {7, 10},
  },
  {
    {7, -9},  {8, -8},  {10, -6},  {11, -4},  {11, -2},  {11, 0},  {11, 2},   {11, 3},   {10, 5},
    {9, 7},   {8, 8},   {6, 10},   {4, 11},   {2, 11},   {0, 11},  {-2, 11},  {-3, 11},  {-5, 10},
    {-7, 9},  {-8, 8},  {-10, 6},  {-11, 4},  {-11, 2},  {-11, 0}, {-11, -2}, {-11, -3}, {-10, -5},
    {-9, -7}, {-8, -8}, {-6, -10}, {-4, -11}, {-2, -11}, {0, -11}, {2, -11},  {3, -11},  {5, -10},
  },
  {
    {3, 5},   {2, 5},   {1, 6},   {0, 6},  {-1, 6},  {-2, 6},  {-3, 5},  {-4, 5},  {-4, 4},
    {-5, 3},  {-5, 2},  {-6, 1},  {-6, 0}, {-6, -1}, {-6, -2}, {-5, -3}, {-5, -4}, {-4, -4},
    {-3, -5}, {-2, -5}, {-1, -6}, {0, -6}, {1, -6},  {2, -6},  {3, -5},  {4, -5},  {4, -4},
    {5, -3},  {5, -2},  {6, -1},  {6, 0},  {6, 1},   {6, 2},   {5, 3},   {5, 4},   {4, 4},
  },
  {
    {6, -8},  {7, -7},  {8, -5},  {9, -4},  {10, -2},  {10, -1},  {10, 1},   {10, 3},   {9, 5},
    {8, 6},   {7, 7},   {5, 8},   {4, 9},   {2, 10},   {1, 10},   {-1, 10},  {-3, 10},  {-5, 9},
    {-6, 8},  {-7, 7},  {-8, 5},  {-9, 4},  {-10, 2},  {-10, 1},  {-10, -1}, {-10, -3}, {-9, -5},
    {-8, -6}, {-7, -7}, {-5, -8}, {-4, -9}, {-2, -10}, {-1, -10}, {1, -10},  {3, -10},  {5, -9},
  },
  {
    {-13, -4}, {-12, -6}, {-11, -8}, {-9, -10}, {-7, -11}, {-5, -13}, {-3, -13}, {-1, -14},
    {2, -13},  {4, -13},  {6, -12},  {8, -11},  {10, -9},  {11, -7},  {13, -5},  {13, -3},
    {14, -1},  {13, 2},   {13, 4},   {12, 6},   {11, 8},   {9, 10},   {7, 11},   {5, 13},
    {3, 13},   {1, 14},   {-2, 13},  {-4, 13},  {-6, 12},  {-8, 11},  {-10, 9},  {-11, 7},
    {-13, 5},  {-13, 3},  {-14, 1},  {-13, -2},
  },
  {
    {-8, 9},  {-9, 7},  {-11, 6},  {-11, 4},  {-12, 2},  {-12, 0}, {-12, -2}, {-11, -4}, {-10, -6},
    {-9, -8}, {-7, -9}, {-6, -11}, {-4, -11}, {-2, -12}, {0, -12}, {2, -12},  {4, -11},  {6, -10},
    {8, -9},  {9, -7},  {11, -6},  {11, -4},  {12, -2},  {12, 0},  {12, 2},   {11, 4},   {10, 6},
    {9, 8},   {7, 9},   {6, 11},   {4, 11},   {2, 12},   {0, 12},  {-2, 12},  {-4, 11},  {-6, 10},
  },
  {
    {-5, 9},  {-6, 8},  {-8, 7},  {-9, 5},  {-10, 4},  {-10, 2},  {-10, 0}, {-10, -2}, {-10, -3},
    {-9, -5}, {-8, -6}, {-7, -8}, {-5, -9}, {-4, -10}, {-2, -10}, {0, -10}, {2, -10},  {3, -10},
    {5, -9},  {6, -8},  {8, -7},  {9, -5},  {10, -4},  {10, -2},  {10, 0},  {10, 2},   {10, 3},
    {9, 5},   {8, 6},   {7, 8},   {5, 9},   {4, 10},   {2, 10},   {0, 10},  {-2, 10},  {-3, 10},
  },
  {
    {-3, -3}, {-2, -3}, {-2, -4}, {-1, -4}, {0, -4}, {0, -4}, {1, -4},  {2, -4},  {2, -3},
    {3, -3},  {3, -2},  {4, -2},  {4, -1},  {4, 0},  {4, 0},  {4, 1},   {4, 2},   {3, 2},
    {3, 3},   {2, 3},   {2, 4},   {1, 4},   {0, 4},  {0, 4},  {-1, 4},  {-2, 4},  {-2, 3},
    {-3, 3},  {-3, 2},  {-4, 2},  {-4, 1},  {-4, 0}, {-4, 0}, {-4, -1}, {-4, -2}, {-3, -2},
  },
  {
    {-4, -7}, {-3, -8}, {-1, -8}, {0, -8}, {1, -8},  {3, -8},  {4, -7},  {5, -6},  {6, -5},
    {7, -4},  {8, -3},  {8, -1},  {8, 0},  {8, 1},   {8, 3},   {7, 4},   {6, 5},   {5, 6},
    {4, 7},   {3, 8},   {1, 8},   {0, 8},  {-1, 8},  {-3, 8},  {-4, 7},  {-5, 6},  {-6, 5},
    {-7, 4},  {-8, 3},  {-8, 1},  {-8, 0}, {-8, -1}, {-8, -3}, {-7, -4}, {-6, -5}, {-5, -6},
  },
  {
    {-3, -12}, {-1, -12}, {1, -12},  {3, -12},  {5, -11}, {7, -10},  {9, -9},   {10, -7},
    {11, -5},  {12, -3},  {12, -1},  {12, 1},   {12, 3},  {11, 5},   {10, 7},   {9, 9},
    {7, 10},   {5, 11},   {3, 12},   {1, 12},   {-1, 12}, {-3, 12},  {-5, 11},  {-7, 10},
    {-9, 9},   {-10, 7},  {-11, 5},  {-12, 3},  {-12, 1}, {-12, -1}, {-12, -3}, {-11, -5},
    {-10, -7}, {-9, -9},  {-7, -10}, {-5, -11},
  },
  {
    {6, 5},   {5, 6},   {4, 7},   {3, 7},   {1, 8},   {0, 8},  {-1, 8},  {-3, 7},  {-4, 7},
    {-5, 6},  {-6, 5},  {-7, 4},  {-7, 3},  {-8, 1},  {-8, 0}, {-8, -1}, {-7, -3}, {-7, -4},
    {-6, -5}, {-5, -6}, {-4, -7}, {-3, -7}, {-1, -8}, {0, -8}, {1, -8},  {3, -7},  {4, -7},
    {5, -6},  {6, -5},  {7, -4},  {7, -3},  {8, -1},  {8, 0},  {8, 1},   {7, 3},   {7, 4},
  },
  {
    {8, 0},  {8, 1},   {8, 3},   {7, 4},   {6, 5},   {5, 6},   {4, 7},   {3, 8},   {1, 8},
    {0, 8},  {-1, 8},  {-3, 8},  {-4, 7},  {-5, 6},  {-6, 5},  {-7, 4},  {-8, 3},  {-8, 1},
    {-8, 0}, {-8, -1}, {-8, -3}, {-7, -4}, {-6, -5}, {-5, -6}, {-4, -7}, {-3, -8}, {-1, -8},
    {0, -8}, {1, -8},  {3, -8},  {4, -7},  {5, -6},  {6, -5},  {7, -4},  {8, -3},  {8, -1},
  },
  {
    {-7, 6},  {-8, 5},  {-9, 3},  {-9, 2},  {-9, 0}, {-9, -2}, {-9, -3}, {-8, -5}, {-7, -6},
    {-6, -7}, {-5, -8}, {-3, -9}, {-2, -9}, {0, -9}, {2, -9},  {3, -9},  {5, -8},  {6, -7},
    {7, -6},  {8, -5},  {9, -3},  {9, -2},  {9, 0},  {9, 2},   {9, 3},   {8, 5},   {7, 6},
    {6, 7},   {5, 8},   {3, 9},   {2, 9},   {0, 9},  {-2, 9},  {-3, 9},  {-5, 8},  {-6, 7},
  },
  {
    {-6, 12},  {-8, 11},  {-10, 9},  {-11, 7},  {-12, 5},  {-13, 3},  {-13, 1},  {-13, -2},
    {-13, -4}, {-12, -6}, {-11, -8}, {-9, -10}, {-7, -11}, {-5, -12}, {-3, -13}, {-1, -13},
    {2, -13},  {4, -13},  {6, -12},  {8, -11},  {10, -9},  {11, -7},  {12, -5},  {13, -3},
    {13, -1},  {13, 2},   {13, 4},   {12, 6},   {11, 8},   {9, 10},   {7, 11},   {5, 12},
    {3, 13},   {1, 13},   {-2, 13},  {-4, 13},
  },
  {
    {-13, 6},  {-14, 4},  {-14, 1},  {-14, -1}, {-14, -4}, {-13, -6}, {-12, -8}, {-10, -10},
    {-8, -12}, {-6, -13}, {-4, -14}, {-1, -14}, {1, -14},  {4, -14},  {6, -13},  {8, -12},
    {10, -10}, {12, -8},  {13, -6},  {14, -4},  {14, -1},  {14, 1},   {14, 4},   {13, 6},
    {12, 8},   {10, 10},  {8, 12},   {6, 13},   {4, 14},   {1, 14},   {-1, 14},  {-4, 14},
    {-6, 13},  {-8, 12},  {-10, 10}, {-12, 8},
  },
  {
    {-5, -2}, {-5, -3}, {-4, -4}, {-3, -4}, {-3, -5}, {-2, -5}, {-1, -5}, {0, -5}, {1, -5},
    {2, -5},  {3, -5},  {4, -4},  {4, -3},  {5, -3},  {5, -2},  {5, -1},  {5, 0},  {5, 1},
    {5, 2},   {5, 3},   {4, 4},   {3, 4},   {3, 5},   {2, 5},   {1, 5},   {0, 5},  {-1, 5},
    {-2, 5},  {-3, 5},  {-4, 4},  {-4, 3},  {-5, 3},  {-5, 2},  {-5, 1},  {-5, 0}, {-5, -1},
  },
  {
    {1, -10},  {3, -10},  {4, -9},  {6, -8},  {7, -7},  {8, -6},  {9, -4},  {10, -2},  {10, -1},
    {10, 1},   {10, 3},   {9, 4},   {8, 6},   {7, 7},   {6, 8},   {4, 9},   {2, 10},   {1, 10},
    {-1, 10},  {-3, 10},  {-4, 9},  {-6, 8},  {-7, 7},  {-8, 6},  {-9, 4},  {-10, 2},  {-10, 1},
    {-10, -1}, {-10, -3}, {-9, -4}, {-8, -6}, {-7, -7}, {-6, -8}, {-4, -9}, {-2, -10}, {-1, -10},
  },
  {
    {3, 10},   {1, 10},   {-1, 10},  {-2, 10},  {-4, 10},  {-6, 9},  {-7, 8},  {-8, 6},  {-9, 5},
    {-10, 3},  {-10, 1},  {-10, -1}, {-10, -2}, {-10, -4}, {-9, -6}, {-8, -7}, {-6, -8}, {-5, -9},
    {-3, -10}, {-1, -10}, {1, -10},  {2, -10},  {4, -10},  {6, -9},  {7, -8},  {8, -6},  {9, -5},
    {10, -3},  {10, -1},  {10, 1},   {10, 2},   {10, 4},   {9, 6},   {8, 7},   {6, 8},   {5, 9},
  },
  {
    {4, 1},   {4, 2},   {3, 2},   {3, 3},   {2, 3},   {2, 4},   {1, 4},   {0, 4},  {0, 4},
    {-1, 4},  {-2, 4},  {-2, 3},  {-3, 3},  {-3, 2},  {-4, 2},  {-4, 1},  {-4, 0}, {-4, 0},
    {-4, -1}, {-4, -2}, {-3, -2}, {-3, -3}, {-2, -3}, {-2, -4}, {-1, -4}, {0, -4}, {0, -4},
    {1, -4},  {2, -4},  {2, -3},  {3, -3},  {3, -2},  {4, -2},  {4, -1},  {4, 0},  {4, 0},
  },
  {
    {8, -4},  {9, -3},  {9, -1},  {9, 1},   {9, 2},   {8, 4},   {7, 5},   {6, 6},   {5, 7},
    {4, 8},   {3, 9},   {1, 9},   {-1, 9},  {-2, 9},  {-4, 8},  {-5, 7},  {-6, 6},  {-7, 5},
    {-8, 4},  {-9, 3},  {-9, 1},  {-9, -1}, {-9, -2}, {-8, -4}, {-7, -5}, {-6, -6}, {-5, -7},
    {-4, -8}, {-3, -9}, {-1, -9}, {1, -9},  {2, -9},  {4, -8},  {5, -7},  {6, -6},  {7, -5},
  },
  {
    {-2, -2}, {-2, -2}, {-1, -3}, {-1, -3}, {0, -3}, {0, -3}, {1, -3},  {1, -3},  {2, -2},
    {2, -2},  {2, -2},  {3, -1},  {3, -1},  {3, 0},  {3, 0},  {3, 1},   {3, 1},   {2, 2},
    {2, 2},   {2, 2},   {1, 3},   {1, 3},   {0, 3},  {0, 3},  {-1, 3},  {-1, 3},  {-2, 2},
    {-2, 2},  {-2, 2},  {-3, 1},  {-3, 1},  {-3, 0}, {-3, 0}, {-3, -1}, {-3, -1}, {-2, -2},
  },
  {
    {2, -13},  {4, -12},  {6, -12},  {8, -10},  {10, -9},  {11, -7},  {12, -5},  {13, -3},
    {13, 0},   {13, 2},   {12, 4},   {12, 6},   {10, 8},   {9, 10},   {7, 11},   {5, 12},
    {3, 13},   {0, 13},   {-2, 13},  {-4, 12},  {-6, 12},  {-8, 10},  {-10, 9},  {-11, 7},
    {-12, 5},  {-13, 3},  {-13, 0},  {-13, -2}, {-12, -4}, {-12, -6}, {-10, -8}, {-9, -10},
    {-7, -11}, {-5, -12}, {-3, -13}, {0, -13},
  },
  {
    {2, -12},  {4, -11},  {6, -11},  {8, -9},  {9, -8},  {10, -6},  {11, -4},  {12, -2},  {12, 0},
    {12, 2},   {11, 4},   {11, 6},   {9, 8},   {8, 9},   {6, 10},   {4, 11},   {2, 12},   {0, 12},
    {-2, 12},  {-4, 11},  {-6, 11},  {-8, 9},  {-9, 8},  {-10, 6},  {-11, 4},  {-12, 2},  {-12, 0},
    {-12, -2}, {-11, -4}, {-11, -6}, {-9, -8}, {-8, -9}, {-6, -10}, {-4, -11}, {-2, -12}, {0, -12},
  },
  {
    {12, 12},  {10, 14},   {7, 15},    {4, 16},    {1, 17},   {-1, 17},  {-4, 16},  {-7, 15},
    {-10, 14}, {-12, 12},  {-14, 10},  {-15, 7},   {-16, 4},  {-17, 1},  {-17, -1}, {-16, -4},
    {-15, -7}, {-14, -10}, {-12, -12}, {-10, -14}, {-7, -15}, {-4, -16}, {-1, -17}, {1, -17},
    {4, -16},  {7, -15},   {10, -14},  {12, -12},  {14, -10}, {15, -7},  {16, -4},  {17, -1},
    {17, 1},   {16, 4},    {15, 7},    {14, 10},
  },
  {
    {-2, -13}, {0, -13},  {3, -13},  {5, -12},  {7, -11}, {9, -10},  {10, -8},  {12, -6},
    {12, -4},  {13, -2},  {13, 0},   {13, 3},   {12, 5},  {11, 7},   {10, 9},   {8, 10},
    {6, 12},   {4, 12},   {2, 13},   {0, 13},   {-3, 13}, {-5, 12},  {-7, 11},  {-9, 10},
    {-10, 8},  {-12, 6},  {-12, 4},  {-13, 2},  {-13, 0}, {-13, -3}, {-12, -5}, {-11, -7},
    {-10, -9}, {-8, -10}, {-6, -12}, {-4, -12},
  },
  {
    {0, -6}, {1, -6},  {2, -6},  {3, -5},  {4, -5},  {5, -4},  {5, -3},  {6, -2},  {6, -1},
    {6, 0},  {6, 1},   {6, 2},   {5, 3},   {5, 4},   {4, 5},   {3, 5},   {2, 6},   {1, 6},
    {0, 6},  {-1, 6},  {-2, 6},  {-3, 5},  {-4, 5},  {-5, 4},  {-5, 3},  {-6, 2},  {-6, 1},
    {-6, 0}, {-6, -1}, {-6, -2}, {-5, -3}, {-5, -4}, {-4, -5}, {-3, -5}, {-2, -6}, {-1, -6},
  },
  {
    {4, 1},   {4, 2},   {3, 2},   {3, 3},   {2, 3},   {2, 4},   {1, 4},   {0, 4},  {0, 4},
    {-1, 4},  {-2, 4},  {-2, 3},  {-3, 3},  {-3, 2},  {-4, 2},  {-4, 1},  {-4, 0}, {-4, 0},
    {-4, -1}, {-4, -2}, {-3, -2}, {-3, -3}, {-2, -3}, {-2, -4}, {-1, -4}, {0, -4}, {0, -4},
    {1, -4},  {2, -4},  {2, -3},  {3, -3},  {3, -2},  {4, -2},  {4, -1},  {4, 0},  {4, 0},
  },
  {
    {9, 3},   {8, 5},   {7, 6},   {6, 7},   {5, 8},   {3, 9},   {2, 9},   {0, 9},  {-1, 9},
    {-3, 9},  {-5, 8},  {-6, 7},  {-7, 6},  {-8, 5},  {-9, 3},  {-9, 2},  {-9, 0}, {-9, -1},
    {-9, -3}, {-8, -5}, {-7, -6}, {-6, -7}, {-5, -8}, {-3, -9}, {-2, -9}, {0, -9}, {1, -9},
    {3, -9},  {5, -8},  {6, -7},  {7, -6},  {8, -5},  {9, -3},  {9, -2},  {9, 0},  {9, 1},
  },
  {
    {-6, -10}, {-4, -11}, {-2, -11}, {0, -12}, {2, -12},  {4, -11},  {6, -10},  {7, -9},  {9, -8},
    {10, -6},  {11, -4},  {11, -2},  {12, 0},  {12, 2},   {11, 4},   {10, 6},   {9, 7},   {8, 9},
    {6, 10},   {4, 11},   {2, 11},   {0, 12},  {-2, 12},  {-4, 11},  {-6, 10},  {-7, 9},  {-9, 8},
    {-10, 6},  {-11, 4},  {-11, 2},  {-12, 0}, {-12, -2}, {-11, -4}, {-10, -6}, {-9, -7}, {-8, -9},
  },
  {
    {-3, -5}, {-2, -5}, {-1, -6}, {0, -6}, {1, -6},  {2, -6},  {3, -5},  {4, -5},  {4, -4},
    {5, -3},  {5, -2},  {6, -1},  {6, 0},  {6, 1},   {6, 2},   {5, 3},   {5, 4},   {4, 4},
    {3, 5},   {2, 5},   {1, 6},   {0, 6},  {-1, 6},  {-2, 6},  {-3, 5},  {-4, 5},  {-4, 4},
    {-5, 3},  {-5, 2},  {-6, 1},  {-6, 0}, {-6, -1}, {-6, -2}, {-5, -3}, {-5, -4}, {-4, -4},
  },
  {
    {-3, -13}, {-1, -13}, {2, -13},  {4, -13},  {6, -12}, {8, -11},  {10, -9},  {11, -7},
    {12, -5},  {13, -3},  {13, -1},  {13, 2},   {13, 4},  {12, 6},   {11, 8},   {9, 10},
    {7, 11},   {5, 12},   {3, 13},   {1, 13},   {-2, 13}, {-4, 13},  {-6, 12},  {-8, 11},
    {-10, 9},  {-11, 7},  {-12, 5},  {-13, 3},  {-13, 1}, {-13, -2}, {-13, -4}, {-12, -6},
    {-11, -8}, {-9, -10}, {-7, -11}, {-5, -12},
  },
  {
    {-1, 1},  {-1, 1},  {-1, 1},  {-1, 0}, {-1, 0}, {-1, 0}, {-1, 0}, {-1, -1}, {-1, -1},
    {-1, -1}, {-1, -1}, {-1, -1}, {0, -1}, {0, -1}, {0, -1}, {0, -1}, {1, -1},  {1, -1},
    {1, -1},  {1, -1},  {1, -1},  {1, 0},  {1, 0},  {1, 0},  {1, 0},  {1, 1},   {1, 1},
    {1, 1},   {1, 1},   {1, 1},   {0, 1},  {0, 1},  {0, 1},  {0, 1},  {-1, 1},  {-1, 1},
  },
  {
    {7, 5},   {6, 6},   {5, 7},   {4, 8},   {2, 8},   {1, 9},   {-1, 9},  {-2, 8},  {-4, 8},
    {-5, 7},  {-6, 6},  {-7, 5},  {-8, 4},  {-8, 2},  {-9, 1},  {-9, -1}, {-8, -2}, {-8, -4},
    {-7, -5}, {-6, -6}, {-5, -7}, {-4, -8}, {-2, -8}, {-1, -9}, {1, -9},  {2, -8},  {4, -8},
    {5, -7},  {6, -6},  {7, -5},  {8, -4},  {8, -2},  {9, -1},  {9, 1},   {8, 2},   {8, 4},
  },
  {
    {12, -11}, {14, -9},  {15, -6},   {16, -4},   {16, -1},  {16, 2},   {16, 5},   {14, 8},
    {13, 10},  {11, 12},  {9, 14},    {6, 15},    {4, 16},   {1, 16},   {-2, 16},  {-5, 16},
    {-8, 14},  {-10, 13}, {-12, 11},  {-14, 9},   {-15, 6},  {-16, 4},  {-16, 1},  {-16, -2},
    {-16, -5}, {-14, -8}, {-13, -10}, {-11, -12}, {-9, -14}, {-6, -15}, {-4, -16}, {-1, -16},
    {2, -16},  {5, -16},  {8, -14},   {10, -13},
  },
  {
    {4, -2},  {4, -1},  {4, -1},  {4, 0},  {4, 1},   {4, 2},   {4, 2},   {3, 3},   {3, 4},
    {2, 4},   {1, 4},   {1, 4},   {0, 4},  {-1, 4},  {-2, 4},  {-2, 4},  {-3, 3},  {-4, 3},
    {-4, 2},  {-4, 1},  {-4, 1},  {-4, 0}, {-4, -1}, {-4, -2}, {-4, -2}, {-3, -3}, {-3, -4},
    {-2, -4}, {-1, -4}, {-1, -4}, {0, -4}, {1, -4},  {2, -4},  {2, -4},  {3, -3},  {4, -3},
  },
  {
    {5, -7},  {6, -6},  {7, -5},  {8, -4},  {8, -2},  {9, -1},  {9, 1},   {8, 2},   {8, 4},
    {7, 5},   {6, 6},   {5, 7},   {4, 8},   {2, 8},   {1, 9},   {-1, 9},  {-2, 8},  {-4, 8},
    {-5, 7},  {-6, 6},  {-7, 5},  {-8, 4},  {-8, 2},  {-9, 1},  {-9, -1}, {-8, -2}, {-8, -4},
    {-7, -5}, {-6, -6}, {-5, -7}, {-4, -8}, {-2, -8}, {-1, -9}, {1, -9},  {2, -8},  {4, -8},
  },
  {
    {-13, 9},   {-14, 7},  {-15, 4},  {-16, 1},  {-16, -1}, {-15, -4}, {-14, -7}, {-13, -9},
    {-11, -11}, {-9, -13}, {-7, -14}, {-4, -15}, {-1, -16}, {1, -16},  {4, -15},  {7, -14},
    {9, -13},   {11, -11}, {13, -9},  {14, -7},  {15, -4},  {16, -1},  {16, 1},   {15, 4},
    {14, 7},    {13, 9},   {11, 11},  {9, 13},   {7, 14},   {4, 15},   {1, 16},   {-1, 16},
    {-4, 15},   {-7, 14},  {-9, 13},  {-11, 11},
  },
  {
    {-9, -5}, {-8, -6}, {-7, -8}, {-5, -9}, {-4, -10}, {-2, -10}, {0, -10}, {2, -10},  {3, -10},
    {5, -9},  {6, -8},  {8, -7},  {9, -5},  {10, -4},  {10, -2},  {10, 0},  {10, 2},   {10, 3},
    {9, 5},   {8, 6},   {7, 8},   {5, 9},   {4, 10},   {2, 10},   {0, 10},  {-2, 10},  {-3, 10},
    {-5, 9},  {-6, 8},  {-8, 7},  {-9, 5},  {-10, 4},  {-10, 2},  {-10, 0}, {-10, -2}, {-10, -3},
  },
  {
    {7, 1},   {7, 2},   {6, 3},   {6, 4},   {5, 5},   {4, 6},   {3, 7},   {1, 7},   {0, 7},
    {-1, 7},  {-2, 7},  {-3, 6},  {-4, 6},  {-5, 5},  {-6, 4},  {-7, 3},  {-7, 1},  {-7, 0},
    {-7, -1}, {-7, -2}, {-6, -3}, {-6, -4}, {-5, -5}, {-4, -6}, {-3, -7}, {-1, -7}, {0, -7},
    {1, -7},  {2, -7},  {3, -6},  {4, -6},  {5, -5},  {6, -4},  {7, -3},  {7, -1},  {7, 0},
  },
  {
    {8, 6},   {7, 7},   {5, 8},   {4, 9},   {2, 10},   {1, 10},   {-1, 10},  {-3, 10},  {-5, 9},
    {-6, 8},  {-7, 7},  {-8, 5},  {-9, 4},  {-10, 2},  {-10, 1},  {-10, -1}, {-10, -3}, {-9, -5},
    {-8, -6}, {-7, -7}, {-5, -8}, {-4, -9}, {-2, -10}, {-1, -10}, {1, -10},  {3, -10},  {5, -9},
    {6, -8},  {7, -7},  {8, -5},  {9, -4},  {10, -2},  {10, -1},  {10, 1},   {10, 3},   {9, 5},
  },
  {
    {7, -8},  {8, -7},  {9, -5},  {10, -3},  {11, -2},  {11, 0},  {10, 2},   {10, 4},   {9, 6},
    {8, 7},   {7, 8},   {5, 9},   {3, 10},   {2, 11},   {0, 11},  {-2, 10},  {-4, 10},  {-6, 9},
    {-7, 8},  {-8, 7},  {-9, 5},  {-10, 3},  {-11, 2},  {-11, 0}, {-10, -2}, {-10, -4}, {-9, -6},
    {-8, -7}, {-7, -8}, {-5, -9}, {-3, -10}, {-2, -11}, {0, -11}, {2, -10},  {4, -10},  {6, -9},
  },
  {
    {7, 6},   {6, 7},   {5, 8},   {3, 9},   {2, 9},   {0, 9},  {-2, 9},  {-3, 9},  {-5, 8},
    {-6, 7},  {-7, 6},  {-8, 5},  {-9, 3},  {-9, 2},  {-9, 0}, {-9, -2}, {-9, -3}, {-8, -5},
    {-7, -6}, {-6, -7}, {-5, -8}, {-3, -9}, {-2, -9}, {0, -9}, {2, -9},  {3, -9},  {5, -8},
    {6, -7},  {7, -6},  {8, -5},  {9, -3},  {9, -2},  {9, 0},  {9, 2},   {9, 3},   {8, 5},
  },
  {
    {-7, -4}, {-6, -5}, {-5, -6}, {-4, -7}, {-3, -8}, {-1, -8}, {0, -8}, {1, -8},  {3, -8},
    {4, -7},  {5, -6},  {6, -5},  {7, -4},  {8, -3},  {8, -1},  {8, 0},  {8, 1},   {8, 3},
    {7, 4},   {6, 5},   {5, 6},   {4, 7},   {3, 8},   {1, 8},   {0, 8},  {-1, 8},  {-3, 8},
    {-4, 7},  {-5, 6},  {-6, 5},  {-7, 4},  {-8, 3},  {-8, 1},  {-8, 0}, {-8, -1}, {-8, -3},
  },
  {
    {-7, 1},  {-7, 0}, {-7, -1}, {-7, -3}, {-6, -4}, {-5, -5}, {-4, -6}, {-3, -6}, {-2, -7},
    {-1, -7}, {0, -7}, {1, -7},  {3, -7},  {4, -6},  {5, -5},  {6, -4},  {6, -3},  {7, -2},
    {7, -1},  {7, 0},  {7, 1},   {7, 3},   {6, 4},   {5, 5},   {4, 6},   {3, 6},   {2, 7},
    {1, 7},   {0, 7},  {-1, 7},  {-3, 7},  {-4, 6},  {-5, 5},  {-6, 4},  {-6, 3},  {-7, 2},
  },
  {
    {-8, 11},  {-10, 9},  {-11, 8},  {-12, 6},  {-13, 3},  {-14, 1},  {-14, -1}, {-13, -4},
    {-12, -6}, {-11, -8}, {-9, -10}, {-8, -11}, {-6, -12}, {-3, -13}, {-1, -14}, {1, -14},
    {4, -13},  {6, -12},  {8, -11},  {10, -9},  {11, -8},  {12, -6},  {13, -3},  {14, -1},
    {14, 1},   {13, 4},   {12, 6},   {11, 8},   {9, 10},   {8, 11},   {6, 12},   {3, 13},
    {1, 14},   {-1, 14},  {-4, 13},  {-6, 12},
  },
  {
    {-7, -8}, {-6, -9}, {-4, -10}, {-2, -10}, {0, -11}, {2, -11},  {3, -10},  {5, -9},  {7, -8},
    {8, -7},  {9, -6},  {10, -4},  {10, -2},  {11, 0},  {11, 2},   {10, 3},   {9, 5},   {8, 7},
    {7, 8},   {6, 9},   {4, 10},   {2, 10},   {0, 11},  {-2, 11},  {-3, 10},  {-5, 9},  {-7, 8},
    {-8, 7},  {-9, 6},  {-10, 4},  {-10, 2},  {-11, 0}, {-11, -2}, {-10, -3}, {-9, -5}, {-8, -7},
  },
  {
    {-13, 6},  {-14, 4},  {-14, 1},  {-14, -1}, {-14, -4}, {-13, -6}, {-12, -8}, {-10, -10},
    {-8, -12}, {-6, -13}, {-4, -14}, {-1, -14}, {1, -14},  {4, -14},  {6, -13},  {8, -12},
    {10, -10}, {12, -8},  {13, -6},  {14, -4},  {14, -1},  {14, 1},   {14, 4},   {13, 6},
    {12, 8},   {10, 10},  {8, 12},   {6, 13},   {4, 14},   {1, 14},   {-1, 14},  {-4, 14},
    {-6, 13},  {-8, 12},  {-10, 10}, {-12, 8},
  },
  {
    {-12, -8}, {-10, -10}, {-9, -12}, {-6, -13}, {-4, -14}, {-2, -14}, {1, -14}, {3, -14},
    {6, -13},  {8, -12},   {10, -10}, {12, -9},  {13, -6},  {14, -4},  {14, -2}, {14, 1},
    {14, 3},   {13, 6},    {12, 8},   {10, 10},  {9, 12},   {6, 13},   {4, 14},  {2, 14},
    {-1, 14},  {-3, 14},   {-6, 13},  {-8, 12},  {-10, 10}, {-12, 9},  {-13, 6}, {-14, 4},
    {-14, 2},  {-14, -1},  {-14, -3}, {-13, -6},
  },
  {
    {2, 4},   {1, 4},   {1, 4},   {0, 4},  {-1, 4},  {-2, 4},  {-2, 4},  {-3, 3},  {-4, 3},
    {-4, 2},  {-4, 1},  {-4, 1},  {-4, 0}, {-4, -1}, {-4, -2}, {-4, -2}, {-3, -3}, {-3, -4},
    {-2, -4}, {-1, -4}, {-1, -4}, {0, -4}, {1, -4},  {2, -4},  {2, -4},  {3, -3},  {4, -3},
    {4, -2},  {4, -1},  {4, -1},  {4, 0},  {4, 1},   {4, 2},   {4, 2},   {3, 3},   {3, 4},
  },
  {
    {3, 9},   {1, 9},   {0, 9},  {-2, 9},  {-3, 9},  {-5, 8},  {-6, 7},  {-7, 6},  {-8, 5},
    {-9, 3},  {-9, 1},  {-9, 0}, {-9, -2}, {-9, -3}, {-8, -5}, {-7, -6}, {-6, -7}, {-5, -8},
    {-3, -9}, {-1, -9}, {0, -9}, {2, -9},  {3, -9},  {5, -8},  {6, -7},  {7, -6},  {8, -5},
    {9, -3},  {9, -1},  {9, 0},  {9, 2},   {9, 3},   {8, 5},   {7, 6},   {6, 7},   {5, 8},
  },
  {
    {10, -5},  {11, -3},  {11, -1},  {11, 1},   {11, 3},   {10, 4},   {9, 6},   {8, 8},   {7, 9},
    {5, 10},   {3, 11},   {1, 11},   {-1, 11},  {-3, 11},  {-4, 10},  {-6, 9},  {-8, 8},  {-9, 7},
    {-10, 5},  {-11, 3},  {-11, 1},  {-11, -1}, {-11, -3}, {-10, -4}, {-9, -6}, {-8, -8}, {-7, -9},
    {-5, -10}, {-3, -11}, {-1, -11}, {1, -11},  {3, -11},  {4, -10},  {6, -9},  {8, -8},  {9, -7},
  },
  {
    {12, 3},   {11, 5},   {10, 7},   {9, 9},    {7, 10},   {5, 11},  {3, 12},   {1, 12},
    {-1, 12},  {-3, 12},  {-5, 11},  {-7, 10},  {-9, 9},   {-10, 7}, {-11, 5},  {-12, 3},
    {-12, 1},  {-12, -1}, {-12, -3}, {-11, -5}, {-10, -7}, {-9, -9}, {-7, -10}, {-5, -11},
    {-3, -12}, {-1, -12}, {1, -12},  {3, -12},  {5, -11},  {7, -10}, {9, -9},   {10, -7},
    {11, -5},  {12, -3},  {12, -1},  {12, 1},
  },
  {
    {-6, -5}, {-5, -6}, {-4, -7}, {-3, -7}, {-1, -8}, {0, -8}, {1, -8},  {3, -7},  {4, -7},
    {5, -6},  {6, -5},  {7, -4},  {7, -3},  {8, -1},  {8, 0},  {8, 1},   {7, 3},   {7, 4},
    {6, 5},   {5, 6},   {4, 7},   {3, 7},   {1, 8},   {0, 8},  {-1, 8},  {-3, 7},  {-4, 7},
    {-5, 6},  {-6, 5},  {-7, 4},  {-7, 3},  {-8, 1},  {-8, 0}, {-8, -1}, {-7, -3}, {-7, -4},
  },
  {
    {-6, 7},  {-7, 6},  {-8, 5},  {-9, 3},  {-9, 2},  {-9, 0}, {-9, -2}, {-9, -3}, {-8, -5},
    {-7, -6}, {-6, -7}, {-5, -8}, {-3, -9}, {-2, -9}, {0, -9}, {2, -9},  {3, -9},  {5, -8},
    {6, -7},  {7, -6},  {8, -5},  {9, -3},  {9, -2},  {9, 0},  {9, 2},   {9, 3},   {8, 5},
    {7, 6},   {6, 7},   {5, 8},   {3, 9},   {2, 9},   {0, 9},  {-2, 9},  {-3, 9},  {-5, 8},
  },
  {
    {8, -3},  {8, -2},  {9, 0},  {8, 1},   {8, 3},   {7, 4},   {7, 5},   {6, 6},   {4, 7},
    {3, 8},   {2, 8},   {0, 9},  {-1, 8},  {-3, 8},  {-4, 7},  {-5, 7},  {-6, 6},  {-7, 4},
    {-8, 3},  {-8, 2},  {-9, 0}, {-8, -1}, {-8, -3}, {-7, -4}, {-7, -5}, {-6, -6}, {-4, -7},
    {-3, -8}, {-2, -8}, {0, -9}, {1, -8},  {3, -8},  {4, -7},  {5, -7},  {6, -6},  {7, -4},
  },
  {
    {9, -8},  {10, -6},  {11, -4},  {12, -2},  {12, 0},  {12, 2},   {11, 4},   {11, 6},   {9, 7},
    {8, 9},   {6, 10},   {4, 11},   {2, 12},   {0, 12},  {-2, 12},  {-4, 11},  {-6, 11},  {-7, 9},
    {-9, 8},  {-10, 6},  {-11, 4},  {-12, 2},  {-12, 0}, {-12, -2}, {-11, -4}, {-11, -6}, {-9, -7},
    {-8, -9}, {-6, -10}, {-4, -11}, {-2, -12}, {0, -12}, {2, -12},  {4, -11},  {6, -11},  {7, -9},
  },
  {
    {2, -12},  {4, -11},  {6, -11},  {8, -9},  {9, -8},  {10, -6},  {11, -4},  {12, -2},  {12, 0},
    {12, 2},   {11, 4},   {11, 6},   {9, 8},   {8, 9},   {6, 10},   {4, 11},   {2, 12},   {0, 12},
    {-2, 12},  {-4, 11},  {-6, 11},  {-8, 9},  {-9, 8},  {-10, 6},  {-11, 4},  {-12, 2},  {-12, 0},
    {-12, -2}, {-11, -4}, {-11, -6}, {-9, -8}, {-8, -9}, {-6, -10}, {-4, -11}, {-2, -12}, {0, -12},
  },
  {
    {2, 8},   {1, 8},   {-1, 8},  {-2, 8},  {-4, 7},  {-5, 7},  {-6, 6},  {-7, 5},  {-8, 3},
    {-8, 2},  {-8, 1},  {-8, -1}, {-8, -2}, {-7, -4}, {-7, -5}, {-6, -6}, {-5, -7}, {-3, -8},
    {-2, -8}, {-1, -8}, {1, -8},  {2, -8},  {4, -7},  {5, -7},  {6, -6},  {7, -5},  {8, -3},
    {8, -2},  {8, -1},  {8, 1},   {8, 2},   {7, 4},   {7, 5},   {6, 6},   {5, 7},   {3, 8},
  },
  {
    {-11, -2}, {-10, -4}, {-10, -6}, {-9, -7}, {-7, -9}, {-6, -10}, {-4, -11}, {-2, -11}, {0, -11},
    {2, -11},  {4, -10},  {6, -10},  {7, -9},  {9, -7},  {10, -6},  {11, -4},  {11, -2},  {11, 0},
    {11, 2},   {10, 4},   {10, 6},   {9, 7},   {7, 9},   {6, 10},   {4, 11},   {2, 11},   {0, 11},
    {-2, 11},  {-4, 10},  {-6, 10},  {-7, 9},  {-9, 7},  {-10, 6},  {-11, 4},  {-11, 2},  {-11, 0},
  },
  {
    {-10, 3},  {-10, 1},  {-10, -1}, {-10, -2}, {-10, -4}, {-9, -6}, {-8, -7}, {-6, -8}, {-5, -9},
    {-3, -10}, {-1, -10}, {1, -10},  {2, -10},  {4, -10},  {6, -9},  {7, -8},  {8, -6},  {9, -5},
    {10, -3},  {10, -1},  {10, 1},   {10, 2},   {10, 4},   {9, 6},   {8, 7},   {6, 8},   {5, 9},
    {3, 10},   {1, 10},   {-1, 10},  {-2, 10},  {-4, 10},  {-6, 9},  {-7, 8},  {-8, 6},  {-9, 5},
  },
  {
    {-12, -13}, {-10, -15}, {-7, -16}, {-4, -17},  {-1, -18}, {2, -18}, {5, -17}, {8, -16},
    {11, -14},  {13, -12},  {15, -10}, {16, -7},   {17, -4},  {18, -1}, {18, 2},  {17, 5},
    {16, 8},    {14, 11},   {12, 13},  {10, 15},   {7, 16},   {4, 17},  {1, 18},  {-2, 18},
    {-5, 17},   {-8, 16},   {-11, 14}, {-13, 12},  {-15, 10}, {-16, 7}, {-17, 4}, {-18, 1},
    {-18, -2},  {-17, -5},  {-16, -8}, {-14, -11},
  },
  {
    {-7, -9}, {-5, -10}, {-3, -11}, {-2, -11}, {0, -11}, {2, -11},  {4, -11},  {6, -10},  {8, -8},
    {9, -7},  {10, -5},  {11, -3},  {11, -2},  {11, 0},  {11, 2},   {11, 4},   {10, 6},   {8, 8},
    {7, 9},   {5, 10},   {3, 11},   {2, 11},   {0, 11},  {-2, 11},  {-4, 11},  {-6, 10},  {-8, 8},
    {-9, 7},  {-10, 5},  {-11, 3},  {-11, 2},  {-11, 0}, {-11, -2}, {-11, -4}, {-10, -6}, {-8, -8},
  },
  {
    {-11, 0}, {-11, -2}, {-10, -4}, {-10, -6}, {-8, -7}, {-7, -8}, {-6, -10}, {-4, -10}, {-2, -11},
    {0, -11}, {2, -11},  {4, -10},  {6, -10},  {7, -8},  {8, -7},  {10, -6},  {10, -4},  {11, -2},
    {11, 0},  {11, 2},   {10, 4},   {10, 6},   {8, 7},   {7, 8},   {6, 10},   {4, 10},   {2, 11},
    {0, 11},  {-2, 11},  {-4, 10},  {-6, 10},  {-7, 8},  {-8, 7},  {-10, 6},  {-10, 4},  {-11, 2},
  },
  {
    {-10, -5}, {-9, -7}, {-8, -8}, {-6, -9}, {-4, -10}, {-3, -11}, {-1, -11}, {1, -11},  {3, -11},
    {5, -10},  {7, -9},  {8, -8},  {9, -6},  {10, -4},  {11, -3},  {11, -1},  {11, 1},   {11, 3},
    {10, 5},   {9, 7},   {8, 8},   {6, 9},   {4, 10},   {3, 11},   {1, 11},   {-1, 11},  {-3, 11},
    {-5, 10},  {-7, 9},  {-8, 8},  {-9, 6},  {-10, 4},  {-11, 3},  {-11, 1},  {-11, -1}, {-11, -3},
  },
  {
    {5, -3},  {5, -2},  {6, -1},  {6, 0},  {6, 1},   {6, 2},   {5, 3},   {5, 4},   {4, 4},
    {3, 5},   {2, 5},   {1, 6},   {0, 6},  {-1, 6},  {-2, 6},  {-3, 5},  {-4, 5},  {-4, 4},
    {-5, 3},  {-5, 2},  {-6, 1},  {-6, 0}, {-6, -1}, {-6, -2}, {-5, -3}, {-5, -4}, {-4, -4},
    {-3, -5}, {-2, -5}, {-1, -6}, {0, -6}, {1, -6},  {2, -6},  {3, -5},  {4, -5},  {4, -4},
  },
  {
    {11, 8},   {9, 10},   {8, 11},   {6, 12},   {3, 13},   {1, 14},   {-1, 14},  {-4, 13},
    {-6, 12},  {-8, 11},  {-10, 9},  {-11, 8},  {-12, 6},  {-13, 3},  {-14, 1},  {-14, -1},
    {-13, -4}, {-12, -6}, {-11, -8}, {-9, -10}, {-8, -11}, {-6, -12}, {-3, -13}, {-1, -14},
    {1, -14},  {4, -13},  {6, -12},  {8, -11},  {10, -9},  {11, -8},  {12, -6},  {13, -3},
    {14, -1},  {14, 1},   {13, 4},   {12, 6},
  },
  {
    {-2, -13}, {0, -13},  {3, -13},  {5, -12},  {7, -11}, {9, -10},  {10, -8},  {12, -6},
    {12, -4},  {13, -2},  {13, 0},   {13, 3},   {12, 5},  {11, 7},   {10, 9},   {8, 10},
    {6, 12},   {4, 12},   {2, 13},   {0, 13},   {-3, 13}, {-5, 12},  {-7, 11},  {-9, 10},
    {-10, 8},  {-12, 6},  {-12, 4},  {-13, 2},  {-13, 0}, {-13, -3}, {-12, -5}, {-11, -7},
    {-10, -9}, {-8, -10}, {-6, -12}, {-4, -12},
  },
  {
    {-1, 12},  {-3, 12},  {-5, 11},  {-7, 10},  {-8, 9},   {-10, 7}, {-11, 5},  {-12, 3},
    {-12, 1},  {-12, -1}, {-12, -3}, {-11, -5}, {-10, -7}, {-9, -8}, {-7, -10}, {-5, -11},
    {-3, -12}, {-1, -12}, {1, -12},  {3, -12},  {5, -11},  {7, -10}, {8, -9},   {10, -7},
    {11, -5},  {12, -3},  {12, -1},  {12, 1},   {12, 3},   {11, 5},  {10, 7},   {9, 8},
    {7, 10},   {5, 11},   {3, 12},   {1, 12},
  },
  {
    {-1, -8}, {0, -8}, {2, -8},  {3, -7},  {4, -7},  {5, -6},  {6, -5},  {7, -4},  {8, -2},
    {8, -1},  {8, 0},  {8, 2},   {7, 3},   {7, 4},   {6, 5},   {5, 6},   {4, 7},   {2, 8},
    {1, 8},   {0, 8},  {-2, 8},  {-3, 7},  {-4, 7},  {-5, 6},  {-6, 5},  {-7, 4},  {-8, 2},
    {-8, 1},  {-8, 0}, {-8, -2}, {-7, -3}, {-7, -4}, {-6, -5}, {-5, -6}, {-4, -7}, {-2, -8},
  },
  {
    {0, 9},  {-2, 9},  {-3, 8},  {-5, 8},  {-6, 7},  {-7, 6},  {-8, 5},  {-8, 3},  {-9, 2},
    {-9, 0}, {-9, -2}, {-8, -3}, {-8, -5}, {-7, -6}, {-6, -7}, {-5, -8}, {-3, -8}, {-2, -9},
    {0, -9}, {2, -9},  {3, -8},  {5, -8},  {6, -7},  {7, -6},  {8, -5},  {8, -3},  {9, -2},
    {9, 0},  {9, 2},   {8, 3},   {8, 5},   {7, 6},   {6, 7},   {5, 8},   {3, 8},   {2, 9},
  },
  {
    {-13, -11}, {-11, -13}, {-8, -15}, {-6, -16}, {-3, -17}, {0, -17}, {3, -17}, {6, -16},
    {9, -15},   {11, -13},  {13, -11}, {15, -8},  {16, -6},  {17, -3}, {17, 0},  {17, 3},
    {16, 6},    {15, 9},    {13, 11},  {11, 13},  {8, 15},   {6, 16},  {3, 17},  {0, 17},
    {-3, 17},   {-6, 16},   {-9, 15},  {-11, 13}, {-13, 11}, {-15, 8}, {-16, 6}, {-17, 3},
    {-17, 0},   {-17, -3},  {-16, -6}, {-15, -9},
  },
  {
    {-12, -5}, {-11, -7}, {-10, -9}, {-8, -10}, {-6, -12}, {-4, -12}, {-2, -13}, {1, -13},
    {3, -13},  {5, -12},  {7, -11},  {9, -10},  {10, -8},  {12, -6},  {12, -4},  {13, -2},
    {13, 1},   {13, 3},   {12, 5},   {11, 7},   {10, 9},   {8, 10},   {6, 12},   {4, 12},
    {2, 13},   {-1, 13},  {-3, 13},  {-5, 12},  {-7, 11},  {-9, 10},  {-10, 8},  {-12, 6},
    {-12, 4},  {-13, 2},  {-13, -1}, {-13, -3},
  },
  {
    {-10, -2}, {-10, -4}, {-9, -5}, {-8, -7}, {-6, -8}, {-5, -9}, {-3, -10}, {-2, -10}, {0, -10},
    {2, -10},  {4, -10},  {5, -9},  {7, -8},  {8, -6},  {9, -5},  {10, -3},  {10, -2},  {10, 0},
    {10, 2},   {10, 4},   {9, 5},   {8, 7},   {6, 8},   {5, 9},   {3, 10},   {2, 10},   {0, 10},
    {-2, 10},  {-4, 10},  {-5, 9},  {-7, 8},  {-8, 6},  {-9, 5},  {-10, 3},  {-10, 2},  {-10, 0},
  },
  {
    {-10, 11}, {-12, 9},   {-13, 7},  {-14, 5},  {-15, 2},  {-15, -1}, {-15, -3}, {-14, -6},
    {-13, -8}, {-11, -10}, {-9, -12}, {-7, -13}, {-5, -14}, {-2, -15}, {1, -15},  {3, -15},
    {6, -14},  {8, -13},   {10, -11}, {12, -9},  {13, -7},  {14, -5},  {15, -2},  {15, 1},
    {15, 3},   {14, 6},    {13, 8},   {11, 10},  {9, 12},   {7, 13},   {5, 14},   {2, 15},
    {-1, 15},  {-3, 15},   {-6, 14},  {-8, 13},
  },
  {
    {-3, 9},  {-5, 8},  {-6, 7},  {-7, 6},  {-8, 5},  {-9, 3},  {-9, 2},  {-9, 0}, {-9, -1},
    {-9, -3}, {-8, -5}, {-7, -6}, {-6, -7}, {-5, -8}, {-3, -9}, {-2, -9}, {0, -9}, {1, -9},
    {3, -9},  {5, -8},  {6, -7},  {7, -6},  {8, -5},  {9, -3},  {9, -2},  {9, 0},  {9, 1},
    {9, 3},   {8, 5},   {7, 6},   {6, 7},   {5, 8},   {3, 9},   {2, 9},   {0, 9},  {-1, 9},
  },
  {
    {-2, -13}, {0, -13},  {3, -13},  {5, -12},  {7, -11}, {9, -10},  {10, -8},  {12, -6},
    {12, -4},  {13, -2},  {13, 0},   {13, 3},   {12, 5},  {11, 7},   {10, 9},   {8, 10},
    {6, 12},   {4, 12},   {2, 13},   {0, 13},   {-3, 13}, {-5, 12},  {-7, 11},  {-9, 10},
    {-10, 8},  {-12, 6},  {-12, 4},  {-13, 2},  {-13, 0}, {-13, -3}, {-12, -5}, {-11, -7},
    {-10, -9}, {-8, -10}, {-6, -12}, {-4, -12},
  },
  {
    {2, -3},  {2, -3},  {3, -2},  {3, -2},  {3, -1},  {4, 0},  {4, 0},  {4, 1},   {3, 1},
    {3, 2},   {3, 2},   {2, 3},   {2, 3},   {1, 3},   {0, 4},  {0, 4},  {-1, 4},  {-1, 3},
    {-2, 3},  {-2, 3},  {-3, 2},  {-3, 2},  {-3, 1},  {-4, 0}, {-4, 0}, {-4, -1}, {-3, -1},
    {-3, -2}, {-3, -2}, {-2, -3}, {-2, -3}, {-1, -3}, {0, -4}, {0, -4}, {1, -4},  {1, -3},
  },
  {
    {3, 2},   {3, 2},   {2, 3},   {2, 3},   {1, 3},   {0, 4},  {0, 4},  {-1, 4},  {-1, 3},
    {-2, 3},  {-2, 3},  {-3, 2},  {-3, 2},  {-3, 1},  {-4, 0}, {-4, 0}, {-4, -1}, {-3, -1},
    {-3, -2}, {-3, -2}, {-2, -3}, {-2, -3}, {-1, -3}, {0, -4}, {0, -4}, {1, -4},  {1, -3},
    {2, -3},  {2, -3},  {3, -2},  {3, -2},  {3, -1},  {4, 0},  {4, 0},  {4, 1},   {3, 1},
  },
  {
    {-9, -13}, {-7, -14}, {-4, -15}, {-1, -16},  {1, -16}, {4, -15}, {7, -14}, {9, -13},
    {11, -11}, {13, -9},  {14, -7},  {15, -4},   {16, -1}, {16, 1},  {15, 4},  {14, 7},
    {13, 9},   {11, 11},  {9, 13},   {7, 14},    {4, 15},  {1, 16},  {-1, 16}, {-4, 15},
    {-7, 14},  {-9, 13},  {-11, 11}, {-13, 9},   {-14, 7}, {-15, 4}, {-16, 1}, {-16, -1},
    {-15, -4}, {-14, -7}, {-13, -9}, {-11, -11},
  },
  {
    {-4, 0}, {-4, -1}, {-4, -1}, {-3, -2}, {-3, -3}, {-3, -3}, {-2, -3}, {-1, -4}, {-1, -4},
    {0, -4}, {1, -4},  {1, -4},  {2, -3},  {3, -3},  {3, -3},  {3, -2},  {4, -1},  {4, -1},
    {4, 0},  {4, 1},   {4, 1},   {3, 2},   {3, 3},   {3, 3},   {2, 3},   {1, 4},   {1, 4},
    {0, 4},  {-1, 4},  {-1, 4},  {-2, 3},  {-3, 3},  {-3, 3},  {-3, 2},  {-4, 1},  {-4, 1},
  },
  {
    {-4, 6},  {-5, 5},  {-6, 4},  {-6, 3},  {-7, 2},  {-7, 1},  {-7, 0}, {-7, -2}, {-7, -3},
    {-6, -4}, {-5, -5}, {-4, -6}, {-3, -6}, {-2, -7}, {-1, -7}, {0, -7}, {2, -7},  {3, -7},
    {4, -6},  {5, -5},  {6, -4},  {6, -3},  {7, -2},  {7, -1},  {7, 0},  {7, 2},   {7, 3},
    {6, 4},   {5, 5},   {4, 6},   {3, 6},   {2, 7},   {1, 7},   {0, 7},  {-2, 7},  {-3, 7},
  },
  {
    {-3, -10}, {-1, -10}, {1, -10},  {2, -10},  {4, -10},  {6, -9},  {7, -8},  {8, -6},  {9, -5},
    {10, -3},  {10, -1},  {10, 1},   {10, 2},   {10, 4},   {9, 6},   {8, 7},   {6, 8},   {5, 9},
    {3, 10},   {1, 10},   {-1, 10},  {-2, 10},  {-4, 10},  {-6, 9},  {-7, 8},  {-8, 6},  {-9, 5},
    {-10, 3},  {-10, 1},  {-10, -1}, {-10, -2}, {-10, -4}, {-9, -6}, {-8, -7}, {-6, -8}, {-5, -9},
  },
  {
    {-4, 12},  {-6, 11},  {-8, 10},  {-9, 8},  {-11, 7},  {-12, 5},  {-12, 3},  {-13, 0}, {-13, -2},
    {-12, -4}, {-11, -6}, {-10, -8}, {-8, -9}, {-7, -11}, {-5, -12}, {-3, -12}, {0, -13}, {2, -13},
    {4, -12},  {6, -11},  {8, -10},  {9, -8},  {11, -7},  {12, -5},  {12, -3},  {13, 0},  {13, 2},
    {12, 4},   {11, 6},   {10, 8},   {8, 9},   {7, 11},   {5, 12},   {3, 12},   {0, 13},  {-2, 13},
  },
  {
    {-2, -7}, {-1, -7}, {1, -7},  {2, -7},  {3, -7},  {4, -6},  {5, -5},  {6, -4},  {7, -3},
    {7, -2},  {7, -1},  {7, 1},   {7, 2},   {7, 3},   {6, 4},   {5, 5},   {4, 6},   {3, 7},
    {2, 7},   {1, 7},   {-1, 7},  {-2, 7},  {-3, 7},  {-4, 6},  {-5, 5},  {-6, 4},  {-7, 3},
    {-7, 2},  {-7, 1},  {-7, -1}, {-7, -2}, {-7, -3}, {-6, -4}, {-5, -5}, {-4, -6}, {-3, -7},
  },
  {
    {-6, -11}, {-4, -12}, {-2, -12}, {0, -13}, {2, -12},  {5, -12},  {7, -11},  {8, -9},  {10, -8},
    {11, -6},  {12, -4},  {12, -2},  {13, 0},  {12, 2},   {12, 5},   {11, 7},   {9, 8},   {8, 10},
    {6, 11},   {4, 12},   {2, 12},   {0, 13},  {-2, 12},  {-5, 12},  {-7, 11},  {-8, 9},  {-10, 8},
    {-11, 6},  {-12, 4},  {-12, 2},  {-13, 0}, {-12, -2}, {-12, -5}, {-11, -7}, {-9, -8}, {-8, -10},
  },
  {
    {-4, 9},  {-6, 8},  {-7, 7},  {-8, 6},  {-9, 4},  {-9, 3},  {-10, 1},  {-10, -1}, {-10, -2},
    {-9, -4}, {-8, -6}, {-7, -7}, {-6, -8}, {-4, -9}, {-3, -9}, {-1, -10}, {1, -10},  {2, -10},
    {4, -9},  {6, -8},  {7, -7},  {8, -6},  {9, -4},  {9, -3},  {10, -1},  {10, 1},   {10, 2},
    {9, 4},   {8, 6},   {7, 7},   {6, 8},   {4, 9},   {3, 9},   {1, 10},   {-1, 10},  {-2, 10},
  },
  {
    {6, -3},  {6, -2},  {7, -1},  {7, 0},  {7, 2},   {6, 3},   {6, 4},   {5, 5},   {4, 5},
    {3, 6},   {2, 6},   {1, 7},   {0, 7},  {-2, 7},  {-3, 6},  {-4, 6},  {-5, 5},  {-5, 4},
    {-6, 3},  {-6, 2},  {-7, 1},  {-7, 0}, {-7, -2}, {-6, -3}, {-6, -4}, {-5, -5}, {-4, -5},
    {-3, -6}, {-2, -6}, {-1, -7}, {0, -7}, {2, -7},  {3, -6},  {4, -6},  {5, -5},  {5, -4},
  },
  {
    {6, 11},   {4, 12},   {2, 12},   {0, 13},  {-2, 12},  {-5, 12},  {-7, 11},  {-8, 9},  {-10, 8},
    {-11, 6},  {-12, 4},  {-12, 2},  {-13, 0}, {-12, -2}, {-12, -5}, {-11, -7}, {-9, -8}, {-8, -10},
    {-6, -11}, {-4, -12}, {-2, -12}, {0, -13}, {2, -12},  {5, -12},  {7, -11},  {8, -9},  {10, -8},
    {11, -6},  {12, -4},  {12, -2},  {13, 0},  {12, 2},   {12, 5},   {11, 7},   {9, 8},   {8, 10},
  },
  {
    {-13, 11},  {-15, 9},   {-16, 6},  {-17, 3},  {-17, 0},  {-17, -3}, {-16, -6}, {-15, -8},
    {-13, -11}, {-11, -13}, {-9, -15}, {-6, -16}, {-3, -17}, {0, -17},  {3, -17},  {6, -16},
    {8, -15},   {11, -13},  {13, -11}, {15, -9},  {16, -6},  {17, -3},  {17, 0},   {17, 3},
    {16, 6},    {15, 8},    {13, 11},  {11, 13},  {9, 15},   {6, 16},   {3, 17},   {0, 17},
    {-3, 17},   {-6, 16},   {-8, 15},  {-11, 13},
  },
  {
    {-5, 5},  {-6, 4},  {-6, 3},  {-7, 2},  {-7, 1},  {-7, -1}, {-7, -2}, {-6, -3}, {-6, -4},
    {-5, -5}, {-4, -6}, {-3, -6}, {-2, -7}, {-1, -7}, {1, -7},  {2, -7},  {3, -6},  {4, -6},
    {5, -5},  {6, -4},  {6, -3},  {7, -2},  {7, -1},  {7, 1},   {7, 2},   {6, 3},   {6, 4},
    {5, 5},   {4, 6},   {3, 6},   {2, 7},   {1, 7},   {-1, 7},  {-2, 7},  {-3, 6},  {-4, 6},
  },
  {
    {11, 11},  {9, 13},   {7, 14},    {4, 15},   {1, 15},   {-1, 15},  {-4, 15},  {-7, 14},
    {-9, 13},  {-11, 11}, {-13, 9},   {-14, 7},  {-15, 4},  {-15, 1},  {-15, -1}, {-15, -4},
    {-14, -7}, {-13, -9}, {-11, -11}, {-9, -13}, {-7, -14}, {-4, -15}, {-1, -15}, {1, -15},
    {4, -15},  {7, -14},  {9, -13},   {11, -11}, {13, -9},  {14, -7},  {15, -4},  {15, -1},
    {15, 1},   {15, 4},   {14, 7},    {13, 9},
  },
  {
    {12, 6},   {11, 8},   {9, 10},   {7, 11},   {5, 12},   {3, 13},   {1, 13},   {-2, 13},
    {-4, 13},  {-6, 12},  {-8, 11},  {-10, 9},  {-11, 7},  {-12, 5},  {-13, 3},  {-13, 1},
    {-13, -2}, {-13, -4}, {-12, -6}, {-11, -8}, {-9, -10}, {-7, -11}, {-5, -12}, {-3, -13},
    {-1, -13}, {2, -13},  {4, -13},  {6, -12},  {8, -11},  {10, -9},  {11, -7},  {12, -5},
    {13, -3},  {13, -1},  {13, 2},   {13, 4},
  },
  {
    {7, -5},  {8, -4},  {8, -2},  {9, -1},  {9, 1},   {8, 2},   {8, 4},   {7, 5},   {6, 6},
    {5, 7},   {4, 8},   {2, 8},   {1, 9},   {-1, 9},  {-2, 8},  {-4, 8},  {-5, 7},  {-6, 6},
    {-7, 5},  {-8, 4},  {-8, 2},  {-9, 1},  {-9, -1}, {-8, -2}, {-8, -4}, {-7, -5}, {-6, -6},
    {-5, -7}, {-4, -8}, {-2, -8}, {-1, -9}, {1, -9},  {2, -8},  {4, -8},  {5, -7},  {6, -6},
  },
  {
    {12, -2},  {12, 0},  {12, 2},   {11, 4},   {10, 6},   {9, 8},   {8, 9},   {6, 11},   {4, 11},
    {2, 12},   {0, 12},  {-2, 12},  {-4, 11},  {-6, 10},  {-8, 9},  {-9, 8},  {-11, 6},  {-11, 4},
    {-12, 2},  {-12, 0}, {-12, -2}, {-11, -4}, {-10, -6}, {-9, -8}, {-8, -9}, {-6, -11}, {-4, -11},
    {-2, -12}, {0, -12}, {2, -12},  {4, -11},  {6, -10},  {8, -9},  {9, -8},  {11, -6},  {11, -4},
  },
  {
    {-1, 12},  {-3, 12},  {-5, 11},  {-7, 10},  {-8, 9},   {-10, 7}, {-11, 5},  {-12, 3},
    {-12, 1},  {-12, -1}, {-12, -3}, {-11, -5}, {-10, -7}, {-9, -8}, {-7, -10}, {-5, -11},
    {-3, -12}, {-1, -12}, {1, -12},  {3, -12},  {5, -11},  {7, -10}, {8, -9},   {10, -7},
    {11, -5},  {12, -3},  {12, -1},  {12, 1},   {12, 3},   {11, 5},  {10, 7},   {9, 8},
    {7, 10},   {5, 11},   {3, 12},   {1, 12},
  },
  {
    {0, 7},  {-1, 7},  {-2, 7},  {-4, 6},  {-4, 5},  {-5, 4},  {-6, 4},  {-7, 2},  {-7, 1},
    {-7, 0}, {-7, -1}, {-7, -2}, {-6, -4}, {-5, -4}, {-4, -5}, {-4, -6}, {-2, -7}, {-1, -7},
    {0, -7}, {1, -7},  {2, -7},  {4, -6},  {4, -5},  {5, -4},  {6, -4},  {7, -2},  {7, -1},
    {7, 0},  {7, 1},   {7, 2},   {6, 4},   {5, 4},   {4, 5},   {4, 6},   {2, 7},   {1, 7},
  },
  {
    {-4, -8}, {-3, -9}, {-1, -9}, {1, -9},  {2, -9},  {4, -8},  {5, -7},  {6, -6},  {7, -5},
    {8, -4},  {9, -3},  {9, -1},  {9, 1},   {9, 2},   {8, 4},   {7, 5},   {6, 6},   {5, 7},
    {4, 8},   {3, 9},   {1, 9},   {-1, 9},  {-2, 9},  {-4, 8},  {-5, 7},  {-6, 6},  {-7, 5},
    {-8, 4},  {-9, 3},  {-9, 1},  {-9, -1}, {-9, -2}, {-8, -4}, {-7, -5}, {-6, -6}, {-5, -7},
  },
  {
    {-3, -2}, {-3, -2}, {-2, -3}, {-2, -3}, {-1, -3}, {0, -4}, {0, -4}, {1, -4},  {1, -3},
    {2, -3},  {2, -3},  {3, -2},  {3, -2},  {3, -1},  {4, 0},  {4, 0},  {4, 1},   {3, 1},
    {3, 2},   {3, 2},   {2, 3},   {2, 3},   {1, 3},   {0, 4},  {0, 4},  {-1, 4},  {-1, 3},
    {-2, 3},  {-2, 3},  {-3, 2},  {-3, 2},  {-3, 1},  {-4, 0}, {-4, 0}, {-4, -1}, {-3, -1},
  },
  {
    {-7, 1},  {-7, 0}, {-7, -1}, {-7, -3}, {-6, -4}, {-5, -5}, {-4, -6}, {-3, -6}, {-2, -7},
    {-1, -7}, {0, -7}, {1, -7},  {3, -7},  {4, -6},  {5, -5},  {6, -4},  {6, -3},  {7, -2},
    {7, -1},  {7, 0},  {7, 1},   {7, 3},   {6, 4},   {5, 5},   {4, 6},   {3, 6},   {2, 7},
    {1, 7},   {0, 7},  {-1, 7},  {-3, 7},  {-4, 6},  {-5, 5},  {-6, 4},  {-6, 3},  {-7, 2},
  },
  {
    {-6, 7},  {-7, 6},  {-8, 5},  {-9, 3},  {-9, 2},  {-9, 0}, {-9, -2}, {-9, -3}, {-8, -5},
    {-7, -6}, {-6, -7}, {-5, -8}, {-3, -9}, {-2, -9}, {0, -9}, {2, -9},  {3, -9},  {5, -8},
    {6, -7},  {7, -6},  {8, -5},  {9, -3},  {9, -2},  {9, 0},  {9, 2},   {9, 3},   {8, 5},
    {7, 6},   {6, 7},   {5, 8},   {3, 9},   {2, 9},   {0, 9},  {-2, 9},  {-3, 9},  {-5, 8},
  },
  {
    {-13, -12}, {-11, -14}, {-8, -16}, {-5, -17},  {-2, -18}, {1, -18}, {4, -17}, {7, -16},
    {10, -15},  {12, -13},  {14, -11}, {16, -8},   {17, -5},  {18, -2}, {18, 1},  {17, 4},
    {16, 7},    {15, 10},   {13, 12},  {11, 14},   {8, 16},   {5, 17},  {2, 18},  {-1, 18},
    {-4, 17},   {-7, 16},   {-10, 15}, {-12, 13},  {-14, 11}, {-16, 8}, {-17, 5}, {-18, 2},
    {-18, -1},  {-17, -4},  {-16, -7}, {-15, -10},
  },
  {
    {-8, -13}, {-6, -14}, {-3, -15}, {0, -15},   {2, -15}, {5, -14}, {7, -13}, {9, -12},
    {11, -10}, {13, -8},  {14, -6},  {15, -3},   {15, 0},  {15, 2},  {14, 5},  {13, 7},
    {12, 9},   {10, 11},  {8, 13},   {6, 14},    {3, 15},  {0, 15},  {-2, 15}, {-5, 14},
    {-7, 13},  {-9, 12},  {-11, 10}, {-13, 8},   {-14, 6}, {-15, 3}, {-15, 0}, {-15, -2},
    {-14, -5}, {-13, -7}, {-12, -9}, {-10, -11},
  },
  {
    {-7, -2}, {-7, -3}, {-6, -4}, {-5, -5}, {-4, -6}, {-3, -7}, {-2, -7}, {-1, -7}, {1, -7},
    {2, -7},  {3, -7},  {4, -6},  {5, -5},  {6, -4},  {7, -3},  {7, -2},  {7, -1},  {7, 1},
    {7, 2},   {7, 3},   {6, 4},   {5, 5},   {4, 6},   {3, 7},   {2, 7},   {1, 7},   {-1, 7},
    {-2, 7},  {-3, 7},  {-4, 6},  {-5, 5},  {-6, 4},  {-7, 3},  {-7, 2},  {-7, 1},  {-7, -1},
  },
  {
    {-6, -8}, {-5, -9}, {-3, -10}, {-1, -10}, {1, -10},  {2, -10},  {4, -9},  {5, -8},  {7, -7},
    {8, -6},  {9, -5},  {10, -3},  {10, -1},  {10, 1},   {10, 2},   {9, 4},   {8, 5},   {7, 7},
    {6, 8},   {5, 9},   {3, 10},   {1, 10},   {-1, 10},  {-2, 10},  {-4, 9},  {-5, 8},  {-7, 7},
    {-8, 6},  {-9, 5},  {-10, 3},  {-10, 1},  {-10, -1}, {-10, -2}, {-9, -4}, {-8, -5}, {-7, -7},
  },
  {
    {-8, 5},  {-9, 4},  {-9, 2},  {-9, 0}, {-9, -1}, {-9, -3}, {-8, -4}, {-7, -6}, {-6, -7},
    {-5, -8}, {-4, -9}, {-2, -9}, {0, -9}, {1, -9},  {3, -9},  {4, -8},  {6, -7},  {7, -6},
    {8, -5},  {9, -4},  {9, -2},  {9, 0},  {9, 1},   {9, 3},   {8, 4},   {7, 6},   {6, 7},
    {5, 8},   {4, 9},   {2, 9},   {0, 9},  {-1, 9},  {-3, 9},  {-4, 8},  {-6, 7},  {-7, 6},
  },
  {
    {-6, -9}, {-4, -10}, {-3, -11}, {-1, -11}, {1, -11},  {3, -10},  {5, -10},  {6, -9},  {8, -7},
    {9, -6},  {10, -4},  {11, -3},  {11, -1},  {11, 1},   {10, 3},   {10, 5},   {9, 6},   {7, 8},
    {6, 9},   {4, 10},   {3, 11},   {1, 11},   {-1, 11},  {-3, 10},  {-5, 10},  {-6, 9},  {-8, 7},
    {-9, 6},  {-10, 4},  {-11, 3},  {-11, 1},  {-11, -1}, {-10, -3}, {-10, -5}, {-9, -6}, {-7, -8},
  },
  {
    {-5, -1}, {-5, -2}, {-4, -3}, {-4, -3}, {-3, -4}, {-2, -4}, {-2, -5}, {-1, -5}, {0, -5},
    {1, -5},  {2, -5},  {3, -4},  {3, -4},  {4, -3},  {4, -2},  {5, -2},  {5, -1},  {5, 0},
    {5, 1},   {5, 2},   {4, 3},   {4, 3},   {3, 4},   {2, 4},   {2, 5},   {1, 5},   {0, 5},
    {-1, 5},  {-2, 5},  {-3, 4},  {-3, 4},  {-4, 3},  {-4, 2},  {-5, 2},  {-5, 1},  {-5, 0},
  },
  {
    {-4, 5},  {-5, 4},  {-5, 3},  {-6, 2},  {-6, 1},  {-6, 0}, {-6, -1}, {-6, -2}, {-6, -3},
    {-5, -4}, {-4, -5}, {-3, -5}, {-2, -6}, {-1, -6}, {0, -6}, {1, -6},  {2, -6},  {3, -6},
    {4, -5},  {5, -4},  {5, -3},  {6, -2},  {6, -1},  {6, 0},  {6, 1},   {6, 2},   {6, 3},
    {5, 4},   {4, 5},   {3, 5},   {2, 6},   {1, 6},   {0, 6},  {-1, 6},  {-2, 6},  {-3, 6},
  },
  {
    {-13, 7},  {-14, 5},  {-15, 2},  {-15, 0},  {-14, -3}, {-14, -5}, {-13, -8}, {-11, -10},
    {-9, -12}, {-7, -13}, {-5, -14}, {-2, -15}, {0, -15},  {3, -14},  {5, -14},  {8, -13},
    {10, -11}, {12, -9},  {13, -7},  {14, -5},  {15, -2},  {15, 0},   {14, 3},   {14, 5},
    {13, 8},   {11, 10},  {9, 12},   {7, 13},   {5, 14},   {2, 15},   {0, 15},   {-3, 14},
    {-5, 14},  {-8, 13},  {-10, 11}, {-12, 9},
  },
  {
    {-8, 10},  {-10, 8},  {-11, 7},  {-12, 5},  {-13, 3},  {-13, 0},  {-13, -2}, {-12, -4},
    {-11, -6}, {-10, -8}, {-8, -10}, {-7, -11}, {-5, -12}, {-3, -13}, {0, -13},  {2, -13},
    {4, -12},  {6, -11},  {8, -10},  {10, -8},  {11, -7},  {12, -5},  {13, -3},  {13, 0},
    {13, 2},   {12, 4},   {11, 6},   {10, 8},   {8, 10},   {7, 11},   {5, 12},   {3, 13},
    {0, 13},   {-2, 13},  {-4, 12},  {-6, 11},
  },
  {
    {1, 5},   {0, 5},  {-1, 5},  {-2, 5},  {-2, 4},  {-3, 4},  {-4, 3},  {-4, 3},  {-5, 2},
    {-5, 1},  {-5, 0}, {-5, -1}, {-5, -2}, {-4, -2}, {-4, -3}, {-3, -4}, {-3, -4}, {-2, -5},
    {-1, -5}, {0, -5}, {1, -5},  {2, -5},  {2, -4},  {3, -4},  {4, -3},  {4, -3},  {5, -2},
    {5, -1},  {5, 0},  {5, 1},   {5, 2},   {4, 2},   {4, 3},   {3, 4},   {3, 4},   {2, 5},
  },
  {
    {5, -13},  {7, -12},  {9, -11},  {11, -9},  {12, -7},  {13, -5},  {14, -2},  {14, 0},
    {14, 3},   {13, 5},   {12, 7},   {11, 9},   {9, 11},   {7, 12},   {5, 13},   {2, 14},
    {0, 14},   {-3, 14},  {-5, 13},  {-7, 12},  {-9, 11},  {-11, 9},  {-12, 7},  {-13, 5},
    {-14, 2},  {-14, 0},  {-14, -3}, {-13, -5}, {-12, -7}, {-11, -9}, {-9, -11}, {-7, -12},
    {-5, -13}, {-2, -14}, {0, -14},  {3, -14},
  },
  {
    {1, 0},  {1, 0},  {1, 0},  {1, 1},   {1, 1},   {1, 1},   {1, 1},   {0, 1},  {0, 1},
    {0, 1},  {0, 1},  {0, 1},  {-1, 1},  {-1, 1},  {-1, 1},  {-1, 1},  {-1, 0}, {-1, 0},
    {-1, 0}, {-1, 0}, {-1, 0}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {0, -1}, {0, -1},
    {0, -1}, {0, -1}, {0, -1}, {1, -1},  {1, -1},  {1, -1},  {1, -1},  {1, 0},  {1, 0},
  },
  {
    {10, -13}, {12, -11}, {14, -9},  {15, -6},   {16, -4},   {16, -1},  {16, 2},   {16, 5},
    {15, 8},   {13, 10},  {11, 12},  {9, 14},    {6, 15},    {4, 16},   {1, 16},   {-2, 16},
    {-5, 16},  {-8, 15},  {-10, 13}, {-12, 11},  {-14, 9},   {-15, 6},  {-16, 4},  {-16, 1},
    {-16, -2}, {-16, -5}, {-15, -8}, {-13, -10}, {-11, -12}, {-9, -14}, {-6, -15}, {-4, -16},
    {-1, -16}, {2, -16},  {5, -16},  {8, -15},
  },
  {
    {9, 12},   {7, 13},    {4, 14},   {2, 15},   {-1, 15},  {-3, 15},  {-6, 14},  {-8, 13},
    {-10, 11}, {-12, 9},   {-13, 7},  {-14, 4},  {-15, 2},  {-15, -1}, {-15, -3}, {-14, -6},
    {-13, -8}, {-11, -10}, {-9, -12}, {-7, -13}, {-4, -14}, {-2, -15}, {1, -15},  {3, -15},
    {6, -14},  {8, -13},   {10, -11}, {12, -9},  {13, -7},  {14, -4},  {15, -2},  {15, 1},
    {15, 3},   {14, 6},    {13, 8},   {11, 10},
  },
  {
    {10, -1},  {10, 1},   {10, 2},   {9, 4},   {8, 6},   {7, 7},   {6, 8},   {4, 9},   {3, 10},
    {1, 10},   {-1, 10},  {-2, 10},  {-4, 9},  {-6, 8},  {-7, 7},  {-8, 6},  {-9, 4},  {-10, 3},
    {-10, 1},  {-10, -1}, {-10, -2}, {-9, -4}, {-8, -6}, {-7, -7}, {-6, -8}, {-4, -9}, {-3, -10},
    {-1, -10}, {1, -10},  {2, -10},  {4, -9},  {6, -8},  {7, -7},  {8, -6},  {9, -4},  {10, -3},
  },
  {
    {5, -8},  {6, -7},  {7, -6},  {8, -4},  {9, -3},  {9, -1},  {9, 0},  {9, 2},   {9, 4},
    {8, 5},   {7, 6},   {6, 7},   {4, 8},   {3, 9},   {1, 9},   {0, 9},  {-2, 9},  {-4, 9},
    {-5, 8},  {-6, 7},  {-7, 6},  {-8, 4},  {-9, 3},  {-9, 1},  {-9, 0}, {-9, -2}, {-9, -4},
    {-8, -5}, {-7, -6}, {-6, -7}, {-4, -8}, {-3, -9}, {-1, -9}, {0, -9}, {2, -9},  {4, -9},
  },
  {
    {10, -9},  {11, -7},  {12, -5},  {13, -3},  {13, 0},   {13, 2},   {13, 4},   {12, 6},
    {11, 8},   {9, 10},   {7, 11},   {5, 12},   {3, 13},   {0, 13},   {-2, 13},  {-4, 13},
    {-6, 12},  {-8, 11},  {-10, 9},  {-11, 7},  {-12, 5},  {-13, 3},  {-13, 0},  {-13, -2},
    {-13, -4}, {-12, -6}, {-11, -8}, {-9, -10}, {-7, -11}, {-5, -12}, {-3, -13}, {0, -13},
    {2, -13},  {4, -13},  {6, -12},  {8, -11},
  },
  {
    {-1, 11},  {-3, 11},  {-5, 10},  {-6, 9},  {-8, 8},  {-9, 6},  {-10, 5},  {-11, 3},  {-11, 1},
    {-11, -1}, {-11, -3}, {-10, -5}, {-9, -6}, {-8, -8}, {-6, -9}, {-5, -10}, {-3, -11}, {-1, -11},
    {1, -11},  {3, -11},  {5, -10},  {6, -9},  {8, -8},  {9, -6},  {10, -5},  {11, -3},  {11, -1},
    {11, 1},   {11, 3},   {10, 5},   {9, 6},   {8, 8},   {6, 9},   {5, 10},   {3, 11},   {1, 11},
  },
  {
    {1, -13},  {3, -13},  {5, -12},  {7, -11},  {9, -9},   {11, -8},  {12, -6},  {13, -4},
    {13, -1},  {13, 1},   {13, 3},   {12, 5},   {11, 7},   {9, 9},    {8, 11},   {6, 12},
    {4, 13},   {1, 13},   {-1, 13},  {-3, 13},  {-5, 12},  {-7, 11},  {-9, 9},   {-11, 8},
    {-12, 6},  {-13, 4},  {-13, 1},  {-13, -1}, {-13, -3}, {-12, -5}, {-11, -7}, {-9, -9},
    {-8, -11}, {-6, -12}, {-4, -13}, {-1, -13},
  },
  {
    {-9, -3}, {-8, -5}, {-7, -6}, {-6, -7}, {-5, -8}, {-3, -9}, {-2, -9}, {0, -9}, {1, -9},
    {3, -9},  {5, -8},  {6, -7},  {7, -6},  {8, -5},  {9, -3},  {9, -2},  {9, 0},  {9, 1},
    {9, 3},   {8, 5},   {7, 6},   {6, 7},   {5, 8},   {3, 9},   {2, 9},   {0, 9},  {-1, 9},
    {-3, 9},  {-5, 8},  {-6, 7},  {-7, 6},  {-8, 5},  {-9, 3},  {-9, 2},  {-9, 0}, {-9, -1},
  },
  {
    {-6, 2},  {-6, 1},  {-6, 0}, {-6, -1}, {-6, -2}, {-5, -3}, {-5, -4}, {-4, -5}, {-3, -6},
    {-2, -6}, {-1, -6}, {0, -6}, {1, -6},  {2, -6},  {3, -5},  {4, -5},  {5, -4},  {6, -3},
    {6, -2},  {6, -1},  {6, 0},  {6, 1},   {6, 2},   {5, 3},   {5, 4},   {4, 5},   {3, 6},
    {2, 6},   {1, 6},   {0, 6},  {-1, 6},  {-2, 6},  {-3, 5},  {-4, 5},  {-5, 4},  {-6, 3},
  },
  {
    {-1, -10}, {1, -10},  {2, -10},  {4, -9},  {6, -8},  {7, -7},  {8, -6},  {9, -4},  {10, -3},
    {10, -1},  {10, 1},   {10, 2},   {9, 4},   {8, 6},   {7, 7},   {6, 8},   {4, 9},   {3, 10},
    {1, 10},   {-1, 10},  {-2, 10},  {-4, 9},  {-6, 8},  {-7, 7},  {-8, 6},  {-9, 4},  {-10, 3},
    {-10, 1},  {-10, -1}, {-10, -2}, {-9, -4}, {-8, -6}, {-7, -7}, {-6, -8}, {-4, -9}, {-3, -10},
  },
  {
    {1, 12},   {-1, 12},  {-3, 12},  {-5, 11},  {-7, 10},  {-9, 8},   {-10, 7}, {-11, 5},
    {-12, 3},  {-12, 1},  {-12, -1}, {-12, -3}, {-11, -5}, {-10, -7}, {-8, -9}, {-7, -10},
    {-5, -11}, {-3, -12}, {-1, -12}, {1, -12},  {3, -12},  {5, -11},  {7, -10}, {9, -8},
    {10, -7},  {11, -5},  {12, -3},  {12, -1},  {12, 1},   {12, 3},   {11, 5},  {10, 7},
    {8, 9},    {7, 10},   {5, 11},   {3, 12},
  },
  {
    {-13, 1},  {-13, -1}, {-13, -4}, {-12, -6}, {-11, -8}, {-9, -9}, {-7, -11}, {-5, -12},
    {-3, -13}, {-1, -13}, {1, -13},  {4, -13},  {6, -12},  {8, -11}, {9, -9},   {11, -7},
    {12, -5},  {13, -3},  {13, -1},  {13, 1},   {13, 4},   {12, 6},  {11, 8},   {9, 9},
    {7, 11},   {5, 12},   {3, 13},   {1, 13},   {-1, 13},  {-4, 13}, {-6, 12},  {-8, 11},
    {-9, 9},   {-11, 7},  {-12, 5},  {-13, 3},
  },
  {
    {-8, -10}, {-6, -11}, {-4, -12}, {-2, -13}, {0, -13}, {3, -13}, {5, -12}, {7, -11},
    {8, -10},  {10, -8},  {11, -6},  {12, -4},  {13, -2}, {13, 0},  {13, 3},  {12, 5},
    {11, 7},   {10, 8},   {8, 10},   {6, 11},   {4, 12},  {2, 13},  {0, 13},  {-3, 13},
    {-5, 12},  {-7, 11},  {-8, 10},  {-10, 8},  {-11, 6}, {-12, 4}, {-13, 2}, {-13, 0},
    {-13, -3}, {-12, -5}, {-11, -7}, {-10, -8},
  },
  {
    {8, -11},  {10, -9},  {11, -8},  {12, -6},  {13, -3},  {14, -1},  {14, 1},   {13, 4},
    {12, 6},   {11, 8},   {9, 10},   {8, 11},   {6, 12},   {3, 13},   {1, 14},   {-1, 14},
    {-4, 13},  {-6, 12},  {-8, 11},  {-10, 9},  {-11, 8},  {-12, 6},  {-13, 3},  {-14, 1},
    {-14, -1}, {-13, -4}, {-12, -6}, {-11, -8}, {-9, -10}, {-8, -11}, {-6, -12}, {-3, -13},
    {-1, -14}, {1, -14},  {4, -13},  {6, -12},
  },
  {
    {10, -6},  {11, -4},  {11, -2},  {12, 0},  {12, 2},   {11, 4},   {10, 6},   {9, 7},   {8, 9},
    {6, 10},   {4, 11},   {2, 11},   {0, 12},  {-2, 12},  {-4, 11},  {-6, 10},  {-7, 9},  {-9, 8},
    {-10, 6},  {-11, 4},  {-11, 2},  {-12, 0}, {-12, -2}, {-11, -4}, {-10, -6}, {-9, -7}, {-8, -9},
    {-6, -10}, {-4, -11}, {-2, -11}, {0, -12}, {2, -12},  {4, -11},  {6, -10},  {7, -9},  {9, -8},
  },
  {
    {2, -13},  {4, -12},  {6, -12},  {8, -10},  {10, -9},  {11, -7},  {12, -5},  {13, -3},
    {13, 0},   {13, 2},   {12, 4},   {12, 6},   {10, 8},   {9, 10},   {7, 11},   {5, 12},
    {3, 13},   {0, 13},   {-2, 13},  {-4, 12},  {-6, 12},  {-8, 10},  {-10, 9},  {-11, 7},
    {-12, 5},  {-13, 3},  {-13, 0},  {-13, -2}, {-12, -4}, {-12, -6}, {-10, -8}, {-9, -10},
    {-7, -11}, {-5, -12}, {-3, -13}, {0, -13},
  },
  {
    {3, -6},  {4, -5},  {5, -5},  {6, -4},  {6, -3},  {7, -2},  {7, 0},  {7, 1},   {6, 2},
    {6, 3},   {5, 4},   {5, 5},   {4, 6},   {3, 6},   {2, 7},   {0, 7},  {-1, 7},  {-2, 6},
    {-3, 6},  {-4, 5},  {-5, 5},  {-6, 4},  {-6, 3},  {-7, 2},  {-7, 0}, {-7, -1}, {-6, -2},
    {-6, -3}, {-5, -4}, {-5, -5}, {-4, -6}, {-3, -6}, {-2, -7}, {0, -7}, {1, -7},  {2, -6},
  },
  {
    {7, -13},  {9, -12},  {11, -10}, {13, -8},  {14, -5},  {14, -3},   {15, 0},   {15, 2},
    {14, 5},   {13, 7},   {12, 9},   {10, 11},  {8, 13},   {5, 14},    {3, 14},   {0, 15},
    {-2, 15},  {-5, 14},  {-7, 13},  {-9, 12},  {-11, 10}, {-13, 8},   {-14, 5},  {-14, 3},
    {-15, 0},  {-15, -2}, {-14, -5}, {-13, -7}, {-12, -9}, {-10, -11}, {-8, -13}, {-5, -14},
    {-3, -14}, {0, -15},  {2, -15},  {5, -14},
  },
  {
    {12, -9},  {13, -7},  {14, -4},   {15, -2},  {15, 1},   {15, 3},   {14, 6},   {13, 8},
    {11, 10},  {9, 12},   {7, 13},    {4, 14},   {2, 15},   {-1, 15},  {-3, 15},  {-6, 14},
    {-8, 13},  {-10, 11}, {-12, 9},   {-13, 7},  {-14, 4},  {-15, 2},  {-15, -1}, {-15, -3},
    {-14, -6}, {-13, -8}, {-11, -10}, {-9, -12}, {-7, -13}, {-4, -14}, {-2, -15}, {1, -15},
    {3, -15},  {6, -14},  {8, -13},   {10, -11},
  },
  {
    {-10, -10}, {-8, -12}, {-6, -13}, {-4, -14}, {-1, -14}, {1, -14}, {4, -14}, {6, -13},
    {8, -12},   {10, -10}, {12, -8},  {13, -6},  {14, -4},  {14, -1}, {14, 1},  {14, 4},
    {13, 6},    {12, 8},   {10, 10},  {8, 12},   {6, 13},   {4, 14},  {1, 14},  {-1, 14},
    {-4, 14},   {-6, 13},  {-8, 12},  {-10, 10}, {-12, 8},  {-13, 6}, {-14, 4}, {-14, 1},
    {-14, -1},  {-14, -4}, {-13, -6}, {-12, -8},
  },
  {
    {-5, -7}, {-4, -8}, {-2, -8}, {-1, -9}, {1, -9},  {2, -8},  {4, -8},  {5, -7},  {6, -6},
    {7, -5},  {8, -4},  {8, -2},  {9, -1},  {9, 1},   {8, 2},   {8, 4},   {7, 5},   {6, 6},
    {5, 7},   {4, 8},   {2, 8},   {1, 9},   {-1, 9},  {-2, 8},  {-4, 8},  {-5, 7},  {-6, 6},
    {-7, 5},  {-8, 4},  {-8, 2},  {-9, 1},  {-9, -1}, {-8, -2}, {-8, -4}, {-7, -5}, {-6, -6},
  },
  {
    {-10, -8}, {-8, -10}, {-7, -11}, {-5, -12}, {-3, -13}, {0, -13}, {2, -13}, {4, -12},
    {6, -11},  {8, -10},  {10, -8},  {11, -7},  {12, -5},  {13, -3}, {13, 0},  {13, 2},
    {12, 4},   {11, 6},   {10, 8},   {8, 10},   {7, 11},   {5, 12},  {3, 13},  {0, 13},
    {-2, 13},  {-4, 12},  {-6, 11},  {-8, 10},  {-10, 8},  {-11, 7}, {-12, 5}, {-13, 3},
    {-13, 0},  {-13, -2}, {-12, -4}, {-11, -6},
  },
  {
    {-8, -13}, {-6, -14}, {-3, -15}, {0, -15},   {2, -15}, {5, -14}, {7, -13}, {9, -12},
    {11, -10}, {13, -8},  {14, -6},  {15, -3},   {15, 0},  {15, 2},  {14, 5},  {13, 7},
    {12, 9},   {10, 11},  {8, 13},   {6, 14},    {3, 15},  {0, 15},  {-2, 15}, {-5, 14},
    {-7, 13},  {-9, 12},  {-11, 10}, {-13, 8},   {-14, 6}, {-15, 3}, {-15, 0}, {-15, -2},
    {-14, -5}, {-13, -7}, {-12, -9}, {-10, -11},
  },
  {
    {4, -6},  {5, -5},  {6, -4},  {6, -3},  {7, -2},  {7, -1},  {7, 0},  {7, 2},   {7, 3},
    {6, 4},   {5, 5},   {4, 6},   {3, 6},   {2, 7},   {1, 7},   {0, 7},  {-2, 7},  {-3, 7},
    {-4, 6},  {-5, 5},  {-6, 4},  {-6, 3},  {-7, 2},  {-7, 1},  {-7, 0}, {-7, -2}, {-7, -3},
    {-6, -4}, {-5, -5}, {-4, -6}, {-3, -6}, {-2, -7}, {-1, -7}, {0, -7}, {2, -7},  {3, -7},
  },
  {
    {8, 5},   {7, 6},   {6, 7},   {4, 8},   {3, 9},   {1, 9},   {0, 9},  {-2, 9},  {-4, 9},
    {-5, 8},  {-6, 7},  {-7, 6},  {-8, 4},  {-9, 3},  {-9, 1},  {-9, 0}, {-9, -2}, {-9, -4},
    {-8, -5}, {-7, -6}, {-6, -7}, {-4, -8}, {-3, -9}, {-1, -9}, {0, -9}, {2, -9},  {4, -9},
    {5, -8},  {6, -7},  {7, -6},  {8, -4},  {9, -3},  {9, -1},  {9, 0},  {9, 2},   {9, 4},
  },
  {
    {3, 12},   {1, 12},   {-1, 12},  {-3, 12},  {-5, 11},  {-7, 10},  {-9, 9},   {-10, 7},
    {-11, 5},  {-12, 3},  {-12, 1},  {-12, -1}, {-12, -3}, {-11, -5}, {-10, -7}, {-9, -9},
    {-7, -10}, {-5, -11}, {-3, -12}, {-1, -12}, {1, -12},  {3, -12},  {5, -11},  {7, -10},
    {9, -9},   {10, -7},  {11, -5},  {12, -3},  {12, -1},  {12, 1},   {12, 3},   {11, 5},
    {10, 7},   {9, 9},    {7, 10},   {5, 11},
  },
  {
    {8, -13},  {10, -11}, {12, -9},  {13, -7},  {14, -5},   {15, -2},  {15, 0},   {15, 3},
    {14, 6},   {13, 8},   {11, 10},  {9, 12},   {7, 13},    {5, 14},   {2, 15},   {0, 15},
    {-3, 15},  {-6, 14},  {-8, 13},  {-10, 11}, {-12, 9},   {-13, 7},  {-14, 5},  {-15, 2},
    {-15, 0},  {-15, -3}, {-14, -6}, {-13, -8}, {-11, -10}, {-9, -12}, {-7, -13}, {-5, -14},
    {-2, -15}, {0, -15},  {3, -15},  {6, -14},
  },
  {
    {-4, 2},  {-4, 1},  {-4, 1},  {-4, 0}, {-4, -1}, {-4, -2}, {-4, -2}, {-3, -3}, {-3, -4},
    {-2, -4}, {-1, -4}, {-1, -4}, {0, -4}, {1, -4},  {2, -4},  {2, -4},  {3, -3},  {4, -3},
    {4, -2},  {4, -1},  {4, -1},  {4, 0},  {4, 1},   {4, 2},   {4, 2},   {3, 3},   {3, 4},
    {2, 4},   {1, 4},   {1, 4},   {0, 4},  {-1, 4},  {-2, 4},  {-2, 4},  {-3, 3},  {-4, 3},
  },
  {
    {-3, -3}, {-2, -3}, {-2, -4}, {-1, -4}, {0, -4}, {0, -4}, {1, -4},  {2, -4},  {2, -3},
    {3, -3},  {3, -2},  {4, -2},  {4, -1},  {4, 0},  {4, 0},  {4, 1},   {4, 2},   {3, 2},
    {3, 3},   {2, 3},   {2, 4},   {1, 4},   {0, 4},  {0, 4},  {-1, 4},  {-2, 4},  {-2, 3},
    {-3, 3},  {-3, 2},  {-4, 2},  {-4, 1},  {-4, 0}, {-4, 0}, {-4, -1}, {-4, -2}, {-3, -2},
  },
  {
    {5, -13},  {7, -12},  {9, -11},  {11, -9},  {12, -7},  {13, -5},  {14, -2},  {14, 0},
    {14, 3},   {13, 5},   {12, 7},   {11, 9},   {9, 11},   {7, 12},   {5, 13},   {2, 14},
    {0, 14},   {-3, 14},  {-5, 13},  {-7, 12},  {-9, 11},  {-11, 9},  {-12, 7},  {-13, 5},
    {-14, 2},  {-14, 0},  {-14, -3}, {-13, -5}, {-12, -7}, {-11, -9}, {-9, -11}, {-7, -12},
    {-5, -13}, {-2, -14}, {0, -14},  {3, -14},
  },
  {
    {10, -12}, {12, -10}, {14, -8},  {15, -5},   {15, -3},   {16, 0},   {15, 3},   {15, 5},
    {14, 8},   {12, 10},  {10, 12},  {8, 14},    {5, 15},    {3, 15},   {0, 16},   {-3, 15},
    {-5, 15},  {-8, 14},  {-10, 12}, {-12, 10},  {-14, 8},   {-15, 5},  {-15, 3},  {-16, 0},
    {-15, -3}, {-15, -5}, {-14, -8}, {-12, -10}, {-10, -12}, {-8, -14}, {-5, -15}, {-3, -15},
    {0, -16},  {3, -15},  {5, -15},  {8, -14},
  },
  {
    {4, -13},  {6, -12},  {8, -11},  {10, -9},  {11, -7},  {13, -5},  {13, -3},  {14, -1},
    {13, 2},   {13, 4},   {12, 6},   {11, 8},   {9, 10},   {7, 11},   {5, 13},   {3, 13},
    {1, 14},   {-2, 13},  {-4, 13},  {-6, 12},  {-8, 11},  {-10, 9},  {-11, 7},  {-13, 5},
    {-13, 3},  {-14, 1},  {-13, -2}, {-13, -4}, {-12, -6}, {-11, -8}, {-9, -10}, {-7, -11},
    {-5, -13}, {-3, -13}, {-1, -14}, {2, -13},
  },
  {
    {5, -1},  {5, 0},  {5, 1},   {5, 2},   {4, 2},   {4, 3},   {3, 4},   {3, 4},   {2, 5},
    {1, 5},   {0, 5},  {-1, 5},  {-2, 5},  {-2, 4},  {-3, 4},  {-4, 3},  {-4, 3},  {-5, 2},
    {-5, 1},  {-5, 0}, {-5, -1}, {-5, -2}, {-4, -2}, {-4, -3}, {-3, -4}, {-3, -4}, {-2, -5},
    {-1, -5}, {0, -5}, {1, -5},  {2, -5},  {2, -4},  {3, -4},  {4, -3},  {4, -3},  {5, -2},
  },
  {
    {-9, 9},   {-10, 7}, {-12, 5},  {-12, 3},  {-13, 1},  {-13, -1}, {-12, -3}, {-12, -5},
    {-10, -7}, {-9, -9}, {-7, -10}, {-5, -12}, {-3, -12}, {-1, -13}, {1, -13},  {3, -12},
    {5, -12},  {7, -10}, {9, -9},   {10, -7},  {12, -5},  {12, -3},  {13, -1},  {13, 1},
    {12, 3},   {12, 5},  {10, 7},   {9, 9},    {7, 10},   {5, 12},   {3, 12},   {1, 13},
    {-1, 13},  {-3, 12}, {-5, 12},  {-7, 10},
  },
  {
    {-4, 3},  {-4, 2},  {-5, 1},  {-5, 1},  {-5, 0}, {-5, -1}, {-5, -2}, {-4, -3}, {-4, -3},
    {-3, -4}, {-2, -4}, {-1, -5}, {-1, -5}, {0, -5}, {1, -5},  {2, -5},  {3, -4},  {3, -4},
    {4, -3},  {4, -2},  {5, -1},  {5, -1},  {5, 0},  {5, 1},   {5, 2},   {4, 3},   {4, 3},
    {3, 4},   {2, 4},   {1, 5},   {1, 5},   {0, 5},  {-1, 5},  {-2, 5},  {-3, 4},  {-3, 4},
  },
  {
    {0, 3},  {-1, 3},  {-1, 3},  {-2, 3},  {-2, 2},  {-2, 2},  {-3, 2},  {-3, 1},  {-3, 1},
    {-3, 0}, {-3, -1}, {-3, -1}, {-3, -2}, {-2, -2}, {-2, -2}, {-2, -3}, {-1, -3}, {-1, -3},
    {0, -3}, {1, -3},  {1, -3},  {2, -3},  {2, -2},  {2, -2},  {3, -2},  {3, -1},  {3, -1},
    {3, 0},  {3, 1},   {3, 1},   {3, 2},   {2, 2},   {2, 2},   {2, 3},   {1, 3},   {1, 3},
  },
  {
    {3, -9},  {5, -8},  {6, -7},  {7, -6},  {8, -5},  {9, -3},  {9, -2},  {9, 0},  {9, 1},
    {9, 3},   {8, 5},   {7, 6},   {6, 7},   {5, 8},   {3, 9},   {2, 9},   {0, 9},  {-1, 9},
    {-3, 9},  {-5, 8},  {-6, 7},  {-7, 6},  {-8, 5},  {-9, 3},  {-9, 2},  {-9, 0}, {-9, -1},
    {-9, -3}, {-8, -5}, {-7, -6}, {-6, -7}, {-5, -8}, {-3, -9}, {-2, -9}, {0, -9}, {1, -9},
  },
  {
    {-12, 1},  {-12, -1}, {-12, -3}, {-11, -5}, {-10, -7}, {-8, -9}, {-7, -10}, {-5, -11},
    {-3, -12}, {-1, -12}, {1, -12},  {3, -12},  {5, -11},  {7, -10}, {9, -8},   {10, -7},
    {11, -5},  {12, -3},  {12, -1},  {12, 1},   {12, 3},   {11, 5},  {10, 7},   {8, 9},
    {7, 10},   {5, 11},   {3, 12},   {1, 12},   {-1, 12},  {-3, 12}, {-5, 11},  {-7, 10},
    {-9, 8},   {-10, 7},  {-11, 5},  {-12, 3},
  },
  {
    {-6, 1},  {-6, 0}, {-6, -1}, {-6, -2}, {-5, -3}, {-5, -4}, {-4, -5}, {-3, -5}, {-2, -6},
    {-1, -6}, {0, -6}, {1, -6},  {2, -6},  {3, -5},  {4, -5},  {5, -4},  {5, -3},  {6, -2},
    {6, -1},  {6, 0},  {6, 1},   {6, 2},   {5, 3},   {5, 4},   {4, 5},   {3, 5},   {2, 6},
    {1, 6},   {0, 6},  {-1, 6},  {-2, 6},  {-3, 5},  {-4, 5},  {-5, 4},  {-5, 3},  {-6, 2},
  },
  {
    {3, 2},   {3, 2},   {2, 3},   {2, 3},   {1, 3},   {0, 4},  {0, 4},  {-1, 4},  {-1, 3},
    {-2, 3},  {-2, 3},  {-3, 2},  {-3, 2},  {-3, 1},  {-4, 0}, {-4, 0}, {-4, -1}, {-3, -1},
    {-3, -2}, {-3, -2}, {-2, -3}, {-2, -3}, {-1, -3}, {0, -4}, {0, -4}, {1, -4},  {1, -3},
    {2, -3},  {2, -3},  {3, -2},  {3, -2},  {3, -1},  {4, 0},  {4, 0},  {4, 1},   {3, 1},
  },
  {
    {4, -8},  {5, -7},  {6, -6},  {7, -5},  {8, -4},  {9, -2},  {9, -1},  {9, 1},   {9, 3},
    {8, 4},   {7, 5},   {6, 6},   {5, 7},   {4, 8},   {2, 9},   {1, 9},   {-1, 9},  {-3, 9},
    {-4, 8},  {-5, 7},  {-6, 6},  {-7, 5},  {-8, 4},  {-9, 2},  {-9, 1},  {-9, -1}, {-9, -3},
    {-8, -4}, {-7, -5}, {-6, -6}, {-5, -7}, {-4, -8}, {-2, -9}, {-1, -9}, {1, -9},  {3, -9},
  },
  {
    {-10, -10}, {-8, -12}, {-6, -13}, {-4, -14}, {-1, -14}, {1, -14}, {4, -14}, {6, -13},
    {8, -12},   {10, -10}, {12, -8},  {13, -6},  {14, -4},  {14, -1}, {14, 1},  {14, 4},
    {13, 6},    {12, 8},   {10, 10},  {8, 12},   {6, 13},   {4, 14},  {1, 14},  {-1, 14},
    {-4, 14},   {-6, 13},  {-8, 12},  {-10, 10}, {-12, 8},  {-13, 6}, {-14, 4}, {-14, 1},
    {-14, -1},  {-14, -4}, {-13, -6}, {-12, -8},
  },
  {
    {-10, 9},  {-11, 7},  {-12, 5},  {-13, 3},  {-13, 0},  {-13, -2}, {-13, -4}, {-12, -6},
    {-11, -8}, {-9, -10}, {-7, -11}, {-5, -12}, {-3, -13}, {0, -13},  {2, -13},  {4, -13},
    {6, -12},  {8, -11},  {10, -9},  {11, -7},  {12, -5},  {13, -3},  {13, 0},   {13, 2},
    {13, 4},   {12, 6},   {11, 8},   {9, 10},   {7, 11},   {5, 12},   {3, 13},   {0, 13},
    {-2, 13},  {-4, 13},  {-6, 12},  {-8, 11},
  },
  {
    {8, -13},  {10, -11}, {12, -9},  {13, -7},  {14, -5},   {15, -2},  {15, 0},   {15, 3},
    {14, 6},   {13, 8},   {11, 10},  {9, 12},   {7, 13},    {5, 14},   {2, 15},   {0, 15},
    {-3, 15},  {-6, 14},  {-8, 13},  {-10, 11}, {-12, 9},   {-13, 7},  {-14, 5},  {-15, 2},
    {-15, 0},  {-15, -3}, {-14, -6}, {-13, -8}, {-11, -10}, {-9, -12}, {-7, -13}, {-5, -14},
    {-2, -15}, {0, -15},  {3, -15},  {6, -14},
  },
  {
    {12, 12},  {10, 14},   {7, 15},    {4, 16},    {1, 17},   {-1, 17},  {-4, 16},  {-7, 15},
    {-10, 14}, {-12, 12},  {-14, 10},  {-15, 7},   {-16, 4},  {-17, 1},  {-17, -1}, {-16, -4},
    {-15, -7}, {-14, -10}, {-12, -12}, {-10, -14}, {-7, -15}, {-4, -16}, {-1, -17}, {1, -17},
    {4, -16},  {7, -15},   {10, -14},  {12, -12},  {14, -10}, {15, -7},  {16, -4},  {17, -1},
    {17, 1},   {16, 4},    {15, 7},    {14, 10},
  },
  {
    {-8, -12}, {-6, -13}, {-3, -14}, {-1, -14},  {2, -14}, {4, -14}, {6, -13}, {9, -12},
    {10, -10}, {12, -8},  {13, -6},  {14, -3},   {14, -1}, {14, 2},  {14, 4},  {13, 6},
    {12, 9},   {10, 10},  {8, 12},   {6, 13},    {3, 14},  {1, 14},  {-2, 14}, {-4, 14},
    {-6, 13},  {-9, 12},  {-10, 10}, {-12, 8},   {-13, 6}, {-14, 3}, {-14, 1}, {-14, -2},
    {-14, -4}, {-13, -6}, {-12, -9}, {-10, -10},
  },
  {
    {-6, -5}, {-5, -6}, {-4, -7}, {-3, -7}, {-1, -8}, {0, -8}, {1, -8},  {3, -7},  {4, -7},
    {5, -6},  {6, -5},  {7, -4},  {7, -3},  {8, -1},  {8, 0},  {8, 1},   {7, 3},   {7, 4},
    {6, 5},   {5, 6},   {4, 7},   {3, 7},   {1, 8},   {0, 8},  {-1, 8},  {-3, 7},  {-4, 7},
    {-5, 6},  {-6, 5},  {-7, 4},  {-7, 3},  {-8, 1},  {-8, 0}, {-8, -1}, {-7, -3}, {-7, -4},
  },
  {
    {2, 2},   {2, 2},   {1, 3},   {1, 3},   {0, 3},  {0, 3},  {-1, 3},  {-1, 3},  {-2, 2},
    {-2, 2},  {-2, 2},  {-3, 1},  {-3, 1},  {-3, 0}, {-3, 0}, {-3, -1}, {-3, -1}, {-2, -2},
    {-2, -2}, {-2, -2}, {-1, -3}, {-1, -3}, {0, -3}, {0, -3}, {1, -3},  {1, -3},  {2, -2},
    {2, -2},  {2, -2},  {3, -1},  {3, -1},  {3, 0},  {3, 0},  {3, 1},   {3, 1},   {2, 2},
  },
  {
    {3, 7},   {2, 7},   {0, 8},  {-1, 8},  {-2, 7},  {-3, 7},  {-5, 6},  {-6, 5},  {-6, 4},
    {-7, 3},  {-7, 2},  {-8, 0}, {-8, -1}, {-7, -2}, {-7, -3}, {-6, -5}, {-5, -6}, {-4, -6},
    {-3, -7}, {-2, -7}, {0, -8}, {1, -8},  {2, -7},  {3, -7},  {5, -6},  {6, -5},  {6, -4},
    {7, -3},  {7, -2},  {8, 0},  {8, 1},   {7, 2},   {7, 3},   {6, 5},   {5, 6},   {4, 6},
  },
  {
    {10, 6},   {9, 8},   {7, 9},   {6, 10},   {4, 11},   {2, 12},   {0, 12},  {-2, 11},  {-4, 11},
    {-6, 10},  {-8, 9},  {-9, 7},  {-10, 6},  {-11, 4},  {-12, 2},  {-12, 0}, {-11, -2}, {-11, -4},
    {-10, -6}, {-9, -8}, {-7, -9}, {-6, -10}, {-4, -11}, {-2, -12}, {0, -12}, {2, -11},  {4, -11},
    {6, -10},  {8, -9},  {9, -7},  {10, -6},  {11, -4},  {12, -2},  {12, 0},  {11, 2},   {11, 4},
  },
  {
    {11, -8},  {12, -6},  {13, -4},  {14, -1},  {14, 1},   {13, 3},   {12, 6},   {11, 8},
    {10, 9},   {8, 11},   {6, 12},   {4, 13},   {1, 14},   {-1, 14},  {-3, 13},  {-6, 12},
    {-8, 11},  {-9, 10},  {-11, 8},  {-12, 6},  {-13, 4},  {-14, 1},  {-14, -1}, {-13, -3},
    {-12, -6}, {-11, -8}, {-10, -9}, {-8, -11}, {-6, -12}, {-4, -13}, {-1, -14}, {1, -14},
    {3, -13},  {6, -12},  {8, -11},  {9, -10},
  },
  {
    {6, 8},   {5, 9},   {3, 10},   {1, 10},   {-1, 10},  {-2, 10},  {-4, 9},  {-5, 8},  {-7, 7},
    {-8, 6},  {-9, 5},  {-10, 3},  {-10, 1},  {-10, -1}, {-10, -2}, {-9, -4}, {-8, -5}, {-7, -7},
    {-6, -8}, {-5, -9}, {-3, -10}, {-1, -10}, {1, -10},  {2, -10},  {4, -9},  {5, -8},  {7, -7},
    {8, -6},  {9, -5},  {10, -3},  {10, -1},  {10, 1},   {10, 2},   {9, 4},   {8, 5},   {7, 7},
  },
  {
    {8, -12},  {10, -10}, {12, -9},  {13, -6},  {14, -4},   {14, -2},  {14, 1},   {14, 3},
    {13, 6},   {12, 8},   {10, 10},  {9, 12},   {6, 13},    {4, 14},   {2, 14},   {-1, 14},
    {-3, 14},  {-6, 13},  {-8, 12},  {-10, 10}, {-12, 9},   {-13, 6},  {-14, 4},  {-14, 2},
    {-14, -1}, {-14, -3}, {-13, -6}, {-12, -8}, {-10, -10}, {-9, -12}, {-6, -13}, {-4, -14},
    {-2, -14}, {1, -14},  {3, -14},  {6, -13},
  },
  {
    {-7, 10},  {-9, 9},   {-10, 7}, {-11, 5},  {-12, 3},  {-12, 1},  {-12, -1}, {-12, -3},
    {-11, -5}, {-10, -7}, {-9, -9}, {-7, -10}, {-5, -11}, {-3, -12}, {-1, -12}, {1, -12},
    {3, -12},  {5, -11},  {7, -10}, {9, -9},   {10, -7},  {11, -5},  {12, -3},  {12, -1},
    {12, 1},   {12, 3},   {11, 5},  {10, 7},   {9, 9},    {7, 10},   {5, 11},   {3, 12},
    {1, 12},   {-1, 12},  {-3, 12}, {-5, 11},
  },
  {
    {-6, 5},  {-7, 4},  {-7, 3},  {-8, 1},  {-8, 0}, {-8, -1}, {-7, -3}, {-7, -4}, {-6, -5},
    {-5, -6}, {-4, -7}, {-3, -7}, {-1, -8}, {0, -8}, {1, -8},  {3, -7},  {4, -7},  {5, -6},
    {6, -5},  {7, -4},  {7, -3},  {8, -1},  {8, 0},  {8, 1},   {7, 3},   {7, 4},   {6, 5},
    {5, 6},   {4, 7},   {3, 7},   {1, 8},   {0, 8},  {-1, 8},  {-3, 7},  {-4, 7},  {-5, 6},
  },
  {
    {-3, -9}, {-1, -9}, {0, -9}, {2, -9},  {3, -9},  {5, -8},  {6, -7},  {7, -6},  {8, -5},
    {9, -3},  {9, -1},  {9, 0},  {9, 2},   {9, 3},   {8, 5},   {7, 6},   {6, 7},   {5, 8},
    {3, 9},   {1, 9},   {0, 9},  {-2, 9},  {-3, 9},  {-5, 8},  {-6, 7},  {-7, 6},  {-8, 5},
    {-9, 3},  {-9, 1},  {-9, 0}, {-9, -2}, {-9, -3}, {-8, -5}, {-7, -6}, {-6, -7}, {-5, -8},
  },
  {
    {-3, 9},  {-5, 8},  {-6, 7},  {-7, 6},  {-8, 5},  {-9, 3},  {-9, 2},  {-9, 0}, {-9, -1},
    {-9, -3}, {-8, -5}, {-7, -6}, {-6, -7}, {-5, -8}, {-3, -9}, {-2, -9}, {0, -9}, {1, -9},
    {3, -9},  {5, -8},  {6, -7},  {7, -6},  {8, -5},  {9, -3},  {9, -2},  {9, 0},  {9, 1},
    {9, 3},   {8, 5},   {7, 6},   {6, 7},   {5, 8},   {3, 9},   {2, 9},   {0, 9},  {-1, 9},
  },
  {
    {-1, -13}, {1, -13},  {4, -13},  {6, -12},  {8, -11},  {9, -9},   {11, -7},  {12, -5},
    {13, -3},  {13, -1},  {13, 1},   {13, 4},   {12, 6},   {11, 8},   {9, 9},    {7, 11},
    {5, 12},   {3, 13},   {1, 13},   {-1, 13},  {-4, 13},  {-6, 12},  {-8, 11},  {-9, 9},
    {-11, 7},  {-12, 5},  {-13, 3},  {-13, 1},  {-13, -1}, {-13, -4}, {-12, -6}, {-11, -8},
    {-9, -9},  {-7, -11}, {-5, -12}, {-3, -13},
  },
  {
    {-1, 5},  {-2, 5},  {-3, 4},  {-3, 4},  {-4, 3},  {-4, 2},  {-5, 2},  {-5, 1},  {-5, 0},
    {-5, -1}, {-5, -2}, {-4, -3}, {-4, -3}, {-3, -4}, {-2, -4}, {-2, -5}, {-1, -5}, {0, -5},
    {1, -5},  {2, -5},  {3, -4},  {3, -4},  {4, -3},  {4, -2},  {5, -2},  {5, -1},  {5, 0},
    {5, 1},   {5, 2},   {4, 3},   {4, 3},   {3, 4},   {2, 4},   {2, 5},   {1, 5},   {0, 5},
  },
  {
    {-3, -7}, {-2, -7}, {0, -8}, {1, -8},  {2, -7},  {3, -7},  {5, -6},  {6, -5},  {6, -4},
    {7, -3},  {7, -2},  {8, 0},  {8, 1},   {7, 2},   {7, 3},   {6, 5},   {5, 6},   {4, 6},
    {3, 7},   {2, 7},   {0, 8},  {-1, 8},  {-2, 7},  {-3, 7},  {-5, 6},  {-6, 5},  {-6, 4},
    {-7, 3},  {-7, 2},  {-8, 0}, {-8, -1}, {-7, -2}, {-7, -3}, {-6, -5}, {-5, -6}, {-4, -6},
  },
  {
    {-3, 4},  {-4, 3},  {-4, 3},  {-5, 2},  {-5, 1},  {-5, 0}, {-5, -1}, {-5, -1}, {-4, -2},
    {-4, -3}, {-3, -4}, {-3, -4}, {-2, -5}, {-1, -5}, {0, -5}, {1, -5},  {1, -5},  {2, -4},
    {3, -4},  {4, -3},  {4, -3},  {5, -2},  {5, -1},  {5, 0},  {5, 1},   {5, 1},   {4, 2},
    {4, 3},   {3, 4},   {3, 4},   {2, 5},   {1, 5},   {0, 5},  {-1, 5},  {-1, 5},  {-2, 4},
  },
  {
    {-8, -2}, {-8, -3}, {-7, -5}, {-6, -6}, {-5, -7}, {-4, -7}, {-2, -8}, {-1, -8}, {1, -8},
    {2, -8},  {3, -8},  {5, -7},  {6, -6},  {7, -5},  {7, -4},  {8, -2},  {8, -1},  {8, 1},
    {8, 2},   {8, 3},   {7, 5},   {6, 6},   {5, 7},   {4, 7},   {2, 8},   {1, 8},   {-1, 8},
    {-2, 8},  {-3, 8},  {-5, 7},  {-6, 6},  {-7, 5},  {-7, 4},  {-8, 2},  {-8, 1},  {-8, -1},
  },
  {
    {-8, 3},  {-8, 2},  {-9, 0}, {-8, -1}, {-8, -3}, {-7, -4}, {-7, -5}, {-6, -6}, {-4, -7},
    {-3, -8}, {-2, -8}, {0, -9}, {1, -8},  {3, -8},  {4, -7},  {5, -7},  {6, -6},  {7, -4},
    {8, -3},  {8, -2},  {9, 0},  {8, 1},   {8, 3},   {7, 4},   {7, 5},   {6, 6},   {4, 7},
    {3, 8},   {2, 8},   {0, 9},  {-1, 8},  {-3, 8},  {-4, 7},  {-5, 7},  {-6, 6},  {-7, 4},
  },
  {
    {4, 2},   {4, 3},   {3, 3},   {2, 4},   {2, 4},   {1, 4},   {0, 4},  {-1, 4},  {-1, 4},
    {-2, 4},  {-3, 4},  {-3, 3},  {-4, 2},  {-4, 2},  {-4, 1},  {-4, 0}, {-4, -1}, {-4, -1},
    {-4, -2}, {-4, -3}, {-3, -3}, {-2, -4}, {-2, -4}, {-1, -4}, {0, -4}, {1, -4},  {1, -4},
    {2, -4},  {3, -4},  {3, -3},  {4, -2},  {4, -2},  {4, -1},  {4, 0},  {4, 1},   {4, 1},
  },
  {
    {12, 12},  {10, 14},   {7, 15},    {4, 16},    {1, 17},   {-1, 17},  {-4, 16},  {-7, 15},
    {-10, 14}, {-12, 12},  {-14, 10},  {-15, 7},   {-16, 4},  {-17, 1},  {-17, -1}, {-16, -4},
    {-15, -7}, {-14, -10}, {-12, -12}, {-10, -14}, {-7, -15}, {-4, -16}, {-1, -17}, {1, -17},
    {4, -16},  {7, -15},   {10, -14},  {12, -12},  {14, -10}, {15, -7},  {16, -4},  {17, -1},
    {17, 1},   {16, 4},    {15, 7},    {14, 10},
  },
  {
    {2, -5},  {3, -5},  {4, -4},  {4, -3},  {5, -3},  {5, -2},  {5, -1},  {5, 0},  {5, 1},
    {5, 2},   {5, 3},   {4, 4},   {3, 4},   {3, 5},   {2, 5},   {1, 5},   {0, 5},  {-1, 5},
    {-2, 5},  {-3, 5},  {-4, 4},  {-4, 3},  {-5, 3},  {-5, 2},  {-5, 1},  {-5, 0}, {-5, -1},
    {-5, -2}, {-5, -3}, {-4, -4}, {-3, -4}, {-3, -5}, {-2, -5}, {-1, -5}, {0, -5}, {1, -5},
  },
  {
    {3, 11},   {1, 11},   {-1, 11},  {-3, 11},  {-5, 10},  {-6, 9},  {-8, 8},  {-9, 7},  {-10, 5},
    {-11, 3},  {-11, 1},  {-11, -1}, {-11, -3}, {-10, -5}, {-9, -6}, {-8, -8}, {-7, -9}, {-5, -10},
    {-3, -11}, {-1, -11}, {1, -11},  {3, -11},  {5, -10},  {6, -9},  {8, -8},  {9, -7},  {10, -5},
    {11, -3},  {11, -1},  {11, 1},   {11, 3},   {10, 5},   {9, 6},   {8, 8},   {7, 9},   {5, 10},
  },
  {
    {6, -9},  {7, -8},  {9, -6},  {10, -5},  {10, -3},  {11, -1},  {11, 1},   {11, 3},   {10, 4},
    {9, 6},   {8, 7},   {6, 9},   {5, 10},   {3, 10},   {1, 11},   {-1, 11},  {-3, 11},  {-4, 10},
    {-6, 9},  {-7, 8},  {-9, 6},  {-10, 5},  {-10, 3},  {-11, 1},  {-11, -1}, {-11, -3}, {-10, -4},
    {-9, -6}, {-8, -7}, {-6, -9}, {-5, -10}, {-3, -10}, {-1, -11}, {1, -11},  {3, -11},  {4, -10},
  },
  {
    {11, -13}, {13, -11}, {15, -8},  {16, -6},   {17, -3},   {17, 0},   {17, 3},   {16, 6},
    {15, 9},   {13, 11},  {11, 13},  {8, 15},    {6, 16},    {3, 17},   {0, 17},   {-3, 17},
    {-6, 16},  {-9, 15},  {-11, 13}, {-13, 11},  {-15, 8},   {-16, 6},  {-17, 3},  {-17, 0},
    {-17, -3}, {-16, -6}, {-15, -9}, {-13, -11}, {-11, -13}, {-8, -15}, {-6, -16}, {-3, -17},
    {0, -17},  {3, -17},  {6, -16},  {9, -15},
  },
  {
    {3, -1},  {3, 0},  {3, 0},  {3, 1},   {3, 1},   {3, 2},   {2, 2},   {2, 2},   {2, 3},
    {1, 3},   {0, 3},  {0, 3},  {-1, 3},  {-1, 3},  {-2, 3},  {-2, 2},  {-2, 2},  {-3, 2},
    {-3, 1},  {-3, 0}, {-3, 0}, {-3, -1}, {-3, -1}, {-3, -2}, {-2, -2}, {-2, -2}, {-2, -3},
    {-1, -3}, {0, -3}, {0, -3}, {1, -3},  {1, -3},  {2, -3},  {2, -2},  {2, -2},  {3, -2},
  },
  {
    {7, 12},   {5, 13},   {2, 14},   {0, 14},   {-2, 14},  {-5, 13},  {-7, 12},  {-9, 11},
    {-11, 9},  {-12, 7},  {-13, 5},  {-14, 2},  {-14, 0},  {-14, -2}, {-13, -5}, {-12, -7},
    {-11, -9}, {-9, -11}, {-7, -12}, {-5, -13}, {-2, -14}, {0, -14},  {2, -14},  {5, -13},
    {7, -12},  {9, -11},  {11, -9},  {12, -7},  {13, -5},  {14, -2},  {14, 0},   {14, 2},
    {13, 5},   {12, 7},   {11, 9},   {9, 11},
  },
  {
    {11, -1},  {11, 1},   {11, 3},   {10, 5},   {9, 6},   {8, 8},   {6, 9},   {5, 10},   {3, 11},
    {1, 11},   {-1, 11},  {-3, 11},  {-5, 10},  {-6, 9},  {-8, 8},  {-9, 6},  {-10, 5},  {-11, 3},
    {-11, 1},  {-11, -1}, {-11, -3}, {-10, -5}, {-9, -6}, {-8, -8}, {-6, -9}, {-5, -10}, {-3, -11},
    {-1, -11}, {1, -11},  {3, -11},  {5, -10},  {6, -9},  {8, -8},  {9, -6},  {10, -5},  {11, -3},
  },
  {
    {12, 4},   {11, 6},   {10, 8},   {8, 9},   {7, 11},   {5, 12},   {3, 12},   {0, 13},  {-2, 13},
    {-4, 12},  {-6, 11},  {-8, 10},  {-9, 8},  {-11, 7},  {-12, 5},  {-12, 3},  {-13, 0}, {-13, -2},
    {-12, -4}, {-11, -6}, {-10, -8}, {-8, -9}, {-7, -11}, {-5, -12}, {-3, -12}, {0, -13}, {2, -13},
    {4, -12},  {6, -11},  {8, -10},  {9, -8},  {11, -7},  {12, -5},  {12, -3},  {13, 0},  {13, 2},
  },
  {
    {-3, 0}, {-3, -1}, {-3, -1}, {-3, -2}, {-2, -2}, {-2, -2}, {-2, -3}, {-1, -3}, {-1, -3},
    {0, -3}, {1, -3},  {1, -3},  {2, -3},  {2, -2},  {2, -2},  {3, -2},  {3, -1},  {3, -1},
    {3, 0},  {3, 1},   {3, 1},   {3, 2},   {2, 2},   {2, 2},   {2, 3},   {1, 3},   {1, 3},
    {0, 3},  {-1, 3},  {-1, 3},  {-2, 3},  {-2, 2},  {-2, 2},  {-3, 2},  {-3, 1},  {-3, 1},
  },
  {
    {-3, 6},  {-4, 5},  {-5, 5},  {-6, 4},  {-6, 3},  {-7, 2},  {-7, 0}, {-7, -1}, {-6, -2},
    {-6, -3}, {-5, -4}, {-5, -5}, {-4, -6}, {-3, -6}, {-2, -7}, {0, -7}, {1, -7},  {2, -6},
    {3, -6},  {4, -5},  {5, -5},  {6, -4},  {6, -3},  {7, -2},  {7, 0},  {7, 1},   {6, 2},
    {6, 3},   {5, 4},   {5, 5},   {4, 6},   {3, 6},   {2, 7},   {0, 7},  {-1, 7},  {-2, 6},
  },
  {
    {4, -11},  {6, -10},  {8, -9},  {9, -8},  {10, -6},  {11, -4},  {12, -2},  {12, 0},  {12, 2},
    {11, 4},   {10, 6},   {9, 8},   {8, 9},   {6, 10},   {4, 11},   {2, 12},   {0, 12},  {-2, 12},
    {-4, 11},  {-6, 10},  {-8, 9},  {-9, 8},  {-10, 6},  {-11, 4},  {-12, 2},  {-12, 0}, {-12, -2},
    {-11, -4}, {-10, -6}, {-9, -8}, {-8, -9}, {-6, -10}, {-4, -11}, {-2, -12}, {0, -12}, {2, -12},
  },
  {
    {4, 12},   {2, 13},   {0, 13},  {-3, 12},  {-5, 12},  {-7, 11},  {-8, 9},  {-10, 8},  {-11, 6},
    {-12, 4},  {-13, 2},  {-13, 0}, {-12, -3}, {-12, -5}, {-11, -7}, {-9, -8}, {-8, -10}, {-6, -11},
    {-4, -12}, {-2, -13}, {0, -13}, {3, -12},  {5, -12},  {7, -11},  {8, -9},  {10, -8},  {11, -6},
    {12, -4},  {13, -2},  {13, 0},  {12, 3},   {12, 5},   {11, 7},   {9, 8},   {8, 10},   {6, 11},
  },
  {
    {2, -4},  {3, -4},  {3, -3},  {4, -2},  {4, -2},  {4, -1},  {4, 0},  {4, 1},   {4, 1},
    {4, 2},   {4, 3},   {3, 3},   {2, 4},   {2, 4},   {1, 4},   {0, 4},  {-1, 4},  {-1, 4},
    {-2, 4},  {-3, 4},  {-3, 3},  {-4, 2},  {-4, 2},  {-4, 1},  {-4, 0}, {-4, -1}, {-4, -1},
    {-4, -2}, {-4, -3}, {-3, -3}, {-2, -4}, {-2, -4}, {-1, -4}, {0, -4}, {1, -4},  {1, -4},
  },
  {
    {2, 1},   {2, 1},   {2, 2},   {1, 2},   {1, 2},   {1, 2},   {0, 2},  {0, 2},  {-1, 2},
    {-1, 2},  {-1, 2},  {-2, 2},  {-2, 1},  {-2, 1},  {-2, 1},  {-2, 0}, {-2, 0}, {-2, -1},
    {-2, -1}, {-2, -1}, {-2, -2}, {-1, -2}, {-1, -2}, {-1, -2}, {0, -2}, {0, -2}, {1, -2},
    {1, -2},  {1, -2},  {2, -2},  {2, -1},  {2, -1},  {2, -1},  {2, 0},  {2, 0},  {2, 1},
  },
  {
    {-10, -6}, {-9, -8}, {-7, -9}, {-6, -10}, {-4, -11}, {-2, -12}, {0, -12}, {2, -11},  {4, -11},
    {6, -10},  {8, -9},  {9, -7},  {10, -6},  {11, -4},  {12, -2},  {12, 0},  {11, 2},   {11, 4},
    {10, 6},   {9, 8},   {7, 9},   {6, 10},   {4, 11},   {2, 12},   {0, 12},  {-2, 11},  {-4, 11},
    {-6, 10},  {-8, 9},  {-9, 7},  {-10, 6},  {-11, 4},  {-12, 2},  {-12, 0}, {-11, -2}, {-11, -4},
  },
  {
    {-8, 1},  {-8, 0}, {-8, -2}, {-7, -3}, {-7, -4}, {-6, -5}, {-5, -6}, {-4, -7}, {-2, -8},
    {-1, -8}, {0, -8}, {2, -8},  {3, -7},  {4, -7},  {5, -6},  {6, -5},  {7, -4},  {8, -2},
    {8, -1},  {8, 0},  {8, 2},   {7, 3},   {7, 4},   {6, 5},   {5, 6},   {4, 7},   {2, 8},
    {1, 8},   {0, 8},  {-2, 8},  {-3, 7},  {-4, 7},  {-5, 6},  {-6, 5},  {-7, 4},  {-8, 2},
  },
  {
    {-13, 7},  {-14, 5},  {-15, 2},  {-15, 0},  {-14, -3}, {-14, -5}, {-13, -8}, {-11, -10},
    {-9, -12}, {-7, -13}, {-5, -14}, {-2, -15}, {0, -15},  {3, -14},  {5, -14},  {8, -13},
    {10, -11}, {12, -9},  {13, -7},  {14, -5},  {15, -2},  {15, 0},   {14, 3},   {14, 5},
    {13, 8},   {11, 10},  {9, 12},   {7, 13},   {5, 14},   {2, 15},   {0, 15},   {-3, 14},
    {-5, 14},  {-8, 13},  {-10, 11}, {-12, 9},
  },
  {
    {-11, 1},  {-11, -1}, {-11, -3}, {-10, -5}, {-9, -6}, {-8, -8}, {-6, -9}, {-5, -10}, {-3, -11},
    {-1, -11}, {1, -11},  {3, -11},  {5, -10},  {6, -9},  {8, -8},  {9, -6},  {10, -5},  {11, -3},
    {11, -1},  {11, 1},   {11, 3},   {10, 5},   {9, 6},   {8, 8},   {6, 9},   {5, 10},   {3, 11},
    {1, 11},   {-1, 11},  {-3, 11},  {-5, 10},  {-6, 9},  {-8, 8},  {-9, 6},  {-10, 5},  {-11, 3},
  },
  {
    {-13, 12},  {-15, 10},  {-16, 7},   {-17, 4},  {-18, 1},  {-18, -2}, {-17, -5}, {-16, -8},
    {-14, -11}, {-12, -13}, {-10, -15}, {-7, -16}, {-4, -17}, {-1, -18}, {2, -18},  {5, -17},
    {8, -16},   {11, -14},  {13, -12},  {15, -10}, {16, -7},  {17, -4},  {18, -1},  {18, 2},
    {17, 5},    {16, 8},    {14, 11},   {12, 13},  {10, 15},  {7, 16},   {4, 17},   {1, 18},
    {-2, 18},   {-5, 17},   {-8, 16},   {-11, 14},
  },
  {
    {-11, -13}, {-9, -15}, {-6, -16}, {-3, -17},  {0, -17}, {3, -17}, {6, -16}, {8, -15},
    {11, -13},  {13, -11}, {15, -9},  {16, -6},   {17, -3}, {17, 0},  {17, 3},  {16, 6},
    {15, 8},    {13, 11},  {11, 13},  {9, 15},    {6, 16},  {3, 17},  {0, 17},  {-3, 17},
    {-6, 16},   {-8, 15},  {-11, 13}, {-13, 11},  {-15, 9}, {-16, 6}, {-17, 3}, {-17, 0},
    {-17, -3},  {-16, -6}, {-15, -8}, {-13, -11},
  },
  {
    {6, 0},  {6, 1},   {6, 2},   {5, 3},   {5, 4},   {4, 5},   {3, 5},   {2, 6},   {1, 6},
    {0, 6},  {-1, 6},  {-2, 6},  {-3, 5},  {-4, 5},  {-5, 4},  {-5, 3},  {-6, 2},  {-6, 1},
    {-6, 0}, {-6, -1}, {-6, -2}, {-5, -3}, {-5, -4}, {-4, -5}, {-3, -5}, {-2, -6}, {-1, -6},
    {0, -6}, {1, -6},  {2, -6},  {3, -5},  {4, -5},  {5, -4},  {5, -3},  {6, -2},  {6, -1},
  },
  {
    {11, -13}, {13, -11}, {15, -8},  {16, -6},   {17, -3},   {17, 0},   {17, 3},   {16, 6},
    {15, 9},   {13, 11},  {11, 13},  {8, 15},    {6, 16},    {3, 17},   {0, 17},   {-3, 17},
    {-6, 16},  {-9, 15},  {-11, 13}, {-13, 11},  {-15, 8},   {-16, 6},  {-17, 3},  {-17, 0},
    {-17, -3}, {-16, -6}, {-15, -9}, {-13, -11}, {-11, -13}, {-8, -15}, {-6, -16}, {-3, -17},
    {0, -17},  {3, -17},  {6, -16},  {9, -15},
  },
  {
    {0, -1}, {0, -1}, {0, -1}, {1, -1},  {1, -1},  {1, -1},  {1, -1},  {1, 0},  {1, 0},
    {1, 0},  {1, 0},  {1, 0},  {1, 1},   {1, 1},   {1, 1},   {1, 1},   {0, 1},  {0, 1},
    {0, 1},  {0, 1},  {0, 1},  {-1, 1},  {-1, 1},  {-1, 1},  {-1, 1},  {-1, 0}, {-1, 0},
    {-1, 0}, {-1, 0}, {-1, 0}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {0, -1}, {0, -1},
  },
  {
    {1, 4},   {0, 4},  {0, 4},  {-1, 4},  {-2, 4},  {-2, 3},  {-3, 3},  {-3, 2},  {-4, 2},
    {-4, 1},  {-4, 0}, {-4, 0}, {-4, -1}, {-4, -2}, {-3, -2}, {-3, -3}, {-2, -3}, {-2, -4},
    {-1, -4}, {0, -4}, {0, -4}, {1, -4},  {2, -4},  {2, -3},  {3, -3},  {3, -2},  {4, -2},
    {4, -1},  {4, 0},  {4, 0},  {4, 1},   {4, 2},   {3, 2},   {3, 3},   {2, 3},   {2, 4},
  },
  {
    {-13, 3},  {-13, 1},  {-13, -2}, {-13, -4}, {-12, -6}, {-11, -8}, {-9, -10}, {-7, -11},
    {-5, -12}, {-3, -13}, {-1, -13}, {2, -13},  {4, -13},  {6, -12},  {8, -11},  {10, -9},
    {11, -7},  {12, -5},  {13, -3},  {13, -1},  {13, 2},   {13, 4},   {12, 6},   {11, 8},
    {9, 10},   {7, 11},   {5, 12},   {3, 13},   {1, 13},   {-2, 13},  {-4, 13},  {-6, 12},
    {-8, 11},  {-10, 9},  {-11, 7},  {-12, 5},
  },
  {
    {-9, -2}, {-9, -4}, {-8, -5}, {-7, -6}, {-6, -7}, {-4, -8}, {-3, -9}, {-1, -9}, {0, -9},
    {2, -9},  {4, -9},  {5, -8},  {6, -7},  {7, -6},  {8, -4},  {9, -3},  {9, -1},  {9, 0},
    {9, 2},   {9, 4},   {8, 5},   {7, 6},   {6, 7},   {4, 8},   {3, 9},   {1, 9},   {0, 9},
    {-2, 9},  {-4, 9},  {-5, 8},  {-6, 7},  {-7, 6},  {-8, 4},  {-9, 3},  {-9, 1},  {-9, 0},
  },
  {
    {-9, 8},  {-10, 6},  {-11, 4},  {-12, 2},  {-12, 0}, {-12, -2}, {-11, -4}, {-11, -6}, {-9, -7},
    {-8, -9}, {-6, -10}, {-4, -11}, {-2, -12}, {0, -12}, {2, -12},  {4, -11},  {6, -11},  {7, -9},
    {9, -8},  {10, -6},  {11, -4},  {12, -2},  {12, 0},  {12, 2},   {11, 4},   {11, 6},   {9, 7},
    {8, 9},   {6, 10},   {4, 11},   {2, 12},   {0, 12},  {-2, 12},  {-4, 11},  {-6, 11},  {-7, 9},
  },
  {
    {-6, -3}, {-5, -4}, {-5, -5}, {-4, -6}, {-3, -6}, {-2, -7}, {0, -7}, {1, -7},  {2, -6},
    {3, -6},  {4, -5},  {5, -5},  {6, -4},  {6, -3},  {7, -2},  {7, 0},  {7, 1},   {6, 2},
    {6, 3},   {5, 4},   {5, 5},   {4, 6},   {3, 6},   {2, 7},   {0, 7},  {-1, 7},  {-2, 6},
    {-3, 6},  {-4, 5},  {-5, 5},  {-6, 4},  {-6, 3},  {-7, 2},  {-7, 0}, {-7, -1}, {-6, -2},
  },
  {
    {-13, -6}, {-12, -8}, {-10, -10}, {-8, -12}, {-6, -13}, {-4, -14}, {-1, -14}, {1, -14},
    {4, -14},  {6, -13},  {8, -12},   {10, -10}, {12, -8},  {13, -6},  {14, -4},  {14, -1},
    {14, 1},   {14, 4},   {13, 6},    {12, 8},   {10, 10},  {8, 12},   {6, 13},   {4, 14},
    {1, 14},   {-1, 14},  {-4, 14},   {-6, 13},  {-8, 12},  {-10, 10}, {-12, 8},  {-13, 6},
    {-14, 4},  {-14, 1},  {-14, -1},  {-14, -4},
  },
  {
    {-8, -2}, {-8, -3}, {-7, -5}, {-6, -6}, {-5, -7}, {-4, -7}, {-2, -8}, {-1, -8}, {1, -8},
    {2, -8},  {3, -8},  {5, -7},  {6, -6},  {7, -5},  {7, -4},  {8, -2},  {8, -1},  {8, 1},
    {8, 2},   {8, 3},   {7, 5},   {6, 6},   {5, 7},   {4, 7},   {2, 8},   {1, 8},   {-1, 8},
    {-2, 8},  {-3, 8},  {-5, 7},  {-6, 6},  {-7, 5},  {-7, 4},  {-8, 2},  {-8, 1},  {-8, -1},
  },
  {
    {5, -9},  {6, -8},  {8, -7},  {9, -5},  {10, -4},  {10, -2},  {10, 0},  {10, 2},   {10, 3},
    {9, 5},   {8, 6},   {7, 8},   {5, 9},   {4, 10},   {2, 10},   {0, 10},  {-2, 10},  {-3, 10},
    {-5, 9},  {-6, 8},  {-8, 7},  {-9, 5},  {-10, 4},  {-10, 2},  {-10, 0}, {-10, -2}, {-10, -3},
    {-9, -5}, {-8, -6}, {-7, -8}, {-5, -9}, {-4, -10}, {-2, -10}, {0, -10}, {2, -10},  {3, -10},
  },
  {
    {8, 10},   {6, 11},   {4, 12},   {2, 13},   {0, 13},   {-3, 13},  {-5, 12},  {-7, 11},
    {-8, 10},  {-10, 8},  {-11, 6},  {-12, 4},  {-13, 2},  {-13, 0},  {-13, -3}, {-12, -5},
    {-11, -7}, {-10, -8}, {-8, -10}, {-6, -11}, {-4, -12}, {-2, -13}, {0, -13},  {3, -13},
    {5, -12},  {7, -11},  {8, -10},  {10, -8},  {11, -6},  {12, -4},  {13, -2},  {13, 0},
    {13, 3},   {12, 5},   {11, 7},   {10, 8},
  },
  {
    {2, 7},   {1, 7},   {-1, 7},  {-2, 7},  {-3, 7},  {-4, 6},  {-5, 5},  {-6, 4},  {-7, 3},
    {-7, 2},  {-7, 1},  {-7, -1}, {-7, -2}, {-7, -3}, {-6, -4}, {-5, -5}, {-4, -6}, {-3, -7},
    {-2, -7}, {-1, -7}, {1, -7},  {2, -7},  {3, -7},  {4, -6},  {5, -5},  {6, -4},  {7, -3},
    {7, -2},  {7, -1},  {7, 1},   {7, 2},   {7, 3},   {6, 4},   {5, 5},   {4, 6},   {3, 7},
  },
  {
    {3, -9},  {5, -8},  {6, -7},  {7, -6},  {8, -5},  {9, -3},  {9, -2},  {9, 0},  {9, 1},
    {9, 3},   {8, 5},   {7, 6},   {6, 7},   {5, 8},   {3, 9},   {2, 9},   {0, 9},  {-1, 9},
    {-3, 9},  {-5, 8},  {-6, 7},  {-7, 6},  {-8, 5},  {-9, 3},  {-9, 2},  {-9, 0}, {-9, -1},
    {-9, -3}, {-8, -5}, {-7, -6}, {-6, -7}, {-5, -8}, {-3, -9}, {-2, -9}, {0, -9}, {1, -9},
  },
  {
    {-1, -6}, {0, -6}, {1, -6},  {2, -6},  {3, -5},  {4, -5},  {5, -4},  {5, -3},  {6, -2},
    {6, -1},  {6, 0},  {6, 1},   {6, 2},   {5, 3},   {5, 4},   {4, 5},   {3, 5},   {2, 6},
    {1, 6},   {0, 6},  {-1, 6},  {-2, 6},  {-3, 5},  {-4, 5},  {-5, 4},  {-5, 3},  {-6, 2},
    {-6, 1},  {-6, 0}, {-6, -1}, {-6, -2}, {-5, -3}, {-5, -4}, {-4, -5}, {-3, -5}, {-2, -6},
  },
  {
    {-1, -1}, {-1, -1}, {-1, -1}, {0, -1}, {0, -1}, {0, -1}, {0, -1}, {1, -1},  {1, -1},
    {1, -1},  {1, -1},  {1, -1},  {1, 0},  {1, 0},  {1, 0},  {1, 0},  {1, 1},   {1, 1},
    {1, 1},   {1, 1},   {1, 1},   {0, 1},  {0, 1},  {0, 1},  {0, 1},  {-1, 1},  {-1, 1},
    {-1, 1},  {-1, 1},  {-1, 1},  {-1, 0}, {-1, 0}, {-1, 0}, {-1, 0}, {-1, -1}, {-1, -1},
  },
  {
    {9, 5},   {8, 6},   {7, 8},   {5, 9},   {4, 10},   {2, 10},   {0, 10},  {-2, 10},  {-3, 10},
    {-5, 9},  {-6, 8},  {-8, 7},  {-9, 5},  {-10, 4},  {-10, 2},  {-10, 0}, {-10, -2}, {-10, -3},
    {-9, -5}, {-8, -6}, {-7, -8}, {-5, -9}, {-4, -10}, {-2, -10}, {0, -10}, {2, -10},  {3, -10},
    {5, -9},  {6, -8},  {8, -7},  {9, -5},  {10, -4},  {10, -2},  {10, 0},  {10, 2},   {10, 3},
  },
  {
    {11, -2},  {11, 0},  {11, 2},   {11, 4},   {10, 6},   {9, 7},   {7, 9},   {6, 10},   {4, 10},
    {2, 11},   {0, 11},  {-2, 11},  {-4, 11},  {-6, 10},  {-7, 9},  {-9, 7},  {-10, 6},  {-10, 4},
    {-11, 2},  {-11, 0}, {-11, -2}, {-11, -4}, {-10, -6}, {-9, -7}, {-7, -9}, {-6, -10}, {-4, -10},
    {-2, -11}, {0, -11}, {2, -11},  {4, -11},  {6, -10},  {7, -9},  {9, -7},  {10, -6},  {10, -4},
  },
  {
    {11, -3},  {11, -1},  {11, 1},   {11, 3},   {10, 5},   {9, 6},   {8, 8},   {7, 9},   {5, 10},
    {3, 11},   {1, 11},   {-1, 11},  {-3, 11},  {-5, 10},  {-6, 9},  {-8, 8},  {-9, 7},  {-10, 5},
    {-11, 3},  {-11, 1},  {-11, -1}, {-11, -3}, {-10, -5}, {-9, -6}, {-8, -8}, {-7, -9}, {-5, -10},
    {-3, -11}, {-1, -11}, {1, -11},  {3, -11},  {5, -10},  {6, -9},  {8, -8},  {9, -7},  {10, -5},
  },
  {
    {12, -8},  {13, -6},  {14, -3},   {14, -1},  {14, 2},   {14, 4},   {13, 6},   {12, 9},
    {10, 10},  {8, 12},   {6, 13},    {3, 14},   {1, 14},   {-2, 14},  {-4, 14},  {-6, 13},
    {-9, 12},  {-10, 10}, {-12, 8},   {-13, 6},  {-14, 3},  {-14, 1},  {-14, -2}, {-14, -4},
    {-13, -6}, {-12, -9}, {-10, -10}, {-8, -12}, {-6, -13}, {-3, -14}, {-1, -14}, {2, -14},
    {4, -14},  {6, -13},  {9, -12},   {10, -10},
  },
  {
    {3, 0},  {3, 1},   {3, 1},   {3, 2},   {2, 2},   {2, 2},   {2, 3},   {1, 3},   {1, 3},
    {0, 3},  {-1, 3},  {-1, 3},  {-2, 3},  {-2, 2},  {-2, 2},  {-3, 2},  {-3, 1},  {-3, 1},
    {-3, 0}, {-3, -1}, {-3, -1}, {-3, -2}, {-2, -2}, {-2, -2}, {-2, -3}, {-1, -3}, {-1, -3},
    {0, -3}, {1, -3},  {1, -3},  {2, -3},  {2, -2},  {2, -2},  {3, -2},  {3, -1},  {3, -1},
  },
  {
    {3, 5},   {2, 5},   {1, 6},   {0, 6},  {-1, 6},  {-2, 6},  {-3, 5},  {-4, 5},  {-4, 4},
    {-5, 3},  {-5, 2},  {-6, 1},  {-6, 0}, {-6, -1}, {-6, -2}, {-5, -3}, {-5, -4}, {-4, -4},
    {-3, -5}, {-2, -5}, {-1, -6}, {0, -6}, {1, -6},  {2, -6},  {3, -5},  {4, -5},  {4, -4},
    {5, -3},  {5, -2},  {6, -1},  {6, 0},  {6, 1},   {6, 2},   {5, 3},   {5, 4},   {4, 4},
  },
  {
    {-1, 4},  {-2, 4},  {-2, 3},  {-3, 3},  {-3, 2},  {-4, 2},  {-4, 1},  {-4, 0}, {-4, 0},
    {-4, -1}, {-4, -2}, {-3, -2}, {-3, -3}, {-2, -3}, {-2, -4}, {-1, -4}, {0, -4}, {0, -4},
    {1, -4},  {2, -4},  {2, -3},  {3, -3},  {3, -2},  {4, -2},  {4, -1},  {4, 0},  {4, 0},
    {4, 1},   {4, 2},   {3, 2},   {3, 3},   {2, 3},   {2, 4},   {1, 4},   {0, 4},  {0, 4},
  },
  {
    {0, 10},  {-2, 10},  {-3, 9},  {-5, 9},  {-6, 8},  {-8, 6},  {-9, 5},  {-9, 3},  {-10, 2},
    {-10, 0}, {-10, -2}, {-9, -3}, {-9, -5}, {-8, -6}, {-6, -8}, {-5, -9}, {-3, -9}, {-2, -10},
    {0, -10}, {2, -10},  {3, -9},  {5, -9},  {6, -8},  {8, -6},  {9, -5},  {9, -3},  {10, -2},
    {10, 0},  {10, 2},   {9, 3},   {9, 5},   {8, 6},   {6, 8},   {5, 9},   {3, 9},   {2, 10},
  },
  {
    {3, -6},  {4, -5},  {5, -5},  {6, -4},  {6, -3},  {7, -2},  {7, 0},  {7, 1},   {6, 2},
    {6, 3},   {5, 4},   {5, 5},   {4, 6},   {3, 6},   {2, 7},   {0, 7},  {-1, 7},  {-2, 6},
    {-3, 6},  {-4, 5},  {-5, 5},  {-6, 4},  {-6, 3},  {-7, 2},  {-7, 0}, {-7, -1}, {-6, -2},
    {-6, -3}, {-5, -4}, {-5, -5}, {-4, -6}, {-3, -6}, {-2, -7}, {0, -7}, {1, -7},  {2, -6},
  },
  {
    {4, 5},   {3, 6},   {2, 6},   {1, 6},   {0, 6},  {-1, 6},  {-2, 6},  {-3, 5},  {-4, 5},
    {-5, 4},  {-6, 3},  {-6, 2},  {-6, 1},  {-6, 0}, {-6, -1}, {-6, -2}, {-5, -3}, {-5, -4},
    {-4, -5}, {-3, -6}, {-2, -6}, {-1, -6}, {0, -6}, {1, -6},  {2, -6},  {3, -5},  {4, -5},
    {5, -4},  {6, -3},  {6, -2},  {6, -1},  {6, 0},  {6, 1},   {6, 2},   {5, 3},   {5, 4},
  },
  {
    {-13, 0},  {-13, -2}, {-12, -4}, {-11, -7}, {-10, -8}, {-8, -10}, {-7, -11}, {-4, -12},
    {-2, -13}, {0, -13},  {2, -13},  {4, -12},  {7, -11},  {8, -10},  {10, -8},  {11, -7},
    {12, -4},  {13, -2},  {13, 0},   {13, 2},   {12, 4},   {11, 7},   {10, 8},   {8, 10},
    {7, 11},   {4, 12},   {2, 13},   {0, 13},   {-2, 13},  {-4, 12},  {-7, 11},  {-8, 10},
    {-10, 8},  {-11, 7},  {-12, 4},  {-13, 2},
  },
  {
    {-10, 5},  {-11, 3},  {-11, 1},  {-11, -1}, {-11, -3}, {-10, -4}, {-9, -6}, {-8, -8}, {-7, -9},
    {-5, -10}, {-3, -11}, {-1, -11}, {1, -11},  {3, -11},  {4, -10},  {6, -9},  {8, -8},  {9, -7},
    {10, -5},  {11, -3},  {11, -1},  {11, 1},   {11, 3},   {10, 4},   {9, 6},   {8, 8},   {7, 9},
    {5, 10},   {3, 11},   {1, 11},   {-1, 11},  {-3, 11},  {-4, 10},  {-6, 9},  {-8, 8},  {-9, 7},
  },
  {
    {5, 8},   {4, 9},   {2, 9},   {0, 9},  {-1, 9},  {-3, 9},  {-4, 8},  {-6, 7},  {-7, 6},
    {-8, 5},  {-9, 4},  {-9, 2},  {-9, 0}, {-9, -1}, {-9, -3}, {-8, -4}, {-7, -6}, {-6, -7},
    {-5, -8}, {-4, -9}, {-2, -9}, {0, -9}, {1, -9},  {3, -9},  {4, -8},  {6, -7},  {7, -6},
    {8, -5},  {9, -4},  {9, -2},  {9, 0},  {9, 1},   {9, 3},   {8, 4},   {7, 6},   {6, 7},
  },
  {
    {12, 11},  {10, 13},  {8, 14},    {5, 16},    {2, 16},   {-1, 16},  {-4, 16},  {-6, 15},
    {-9, 14},  {-11, 12}, {-13, 10},  {-14, 8},   {-16, 5},  {-16, 2},  {-16, -1}, {-16, -4},
    {-15, -6}, {-14, -9}, {-12, -11}, {-10, -13}, {-8, -14}, {-5, -16}, {-2, -16}, {1, -16},
    {4, -16},  {6, -15},  {9, -14},   {11, -12},  {13, -10}, {14, -8},  {16, -5},  {16, -2},
    {16, 1},   {16, 4},   {15, 6},    {14, 9},
  },
  {
    {8, 9},   {6, 10},   {4, 11},   {2, 12},   {0, 12},  {-2, 12},  {-4, 11},  {-6, 11},  {-7, 9},
    {-9, 8},  {-10, 6},  {-11, 4},  {-12, 2},  {-12, 0}, {-12, -2}, {-11, -4}, {-11, -6}, {-9, -7},
    {-8, -9}, {-6, -10}, {-4, -11}, {-2, -12}, {0, -12}, {2, -12},  {4, -11},  {6, -11},  {7, -9},
    {9, -8},  {10, -6},  {11, -4},  {12, -2},  {12, 0},  {12, 2},   {11, 4},   {11, 6},   {9, 7},
  },
  {
    {9, -6},  {10, -4},  {11, -3},  {11, -1},  {11, 1},   {10, 3},   {10, 5},   {9, 6},   {7, 8},
    {6, 9},   {4, 10},   {3, 11},   {1, 11},   {-1, 11},  {-3, 10},  {-5, 10},  {-6, 9},  {-8, 7},
    {-9, 6},  {-10, 4},  {-11, 3},  {-11, 1},  {-11, -1}, {-10, -3}, {-10, -5}, {-9, -6}, {-7, -8},
    {-6, -9}, {-4, -10}, {-3, -11}, {-1, -11}, {1, -11},  {3, -10},  {5, -10},  {6, -9},  {8, -7},
  },
  {
    {7, -4},  {8, -3},  {8, -1},  {8, 0},  {8, 1},   {8, 3},   {7, 4},   {6, 5},   {5, 6},
    {4, 7},   {3, 8},   {1, 8},   {0, 8},  {-1, 8},  {-3, 8},  {-4, 7},  {-5, 6},  {-6, 5},
    {-7, 4},  {-8, 3},  {-8, 1},  {-8, 0}, {-8, -1}, {-8, -3}, {-7, -4}, {-6, -5}, {-5, -6},
    {-4, -7}, {-3, -8}, {-1, -8}, {0, -8}, {1, -8},  {3, -8},  {4, -7},  {5, -6},  {6, -5},
  },
  {
    {8, -12},  {10, -10}, {12, -9},  {13, -6},  {14, -4},   {14, -2},  {14, 1},   {14, 3},
    {13, 6},   {12, 8},   {10, 10},  {9, 12},   {6, 13},    {4, 14},   {2, 14},   {-1, 14},
    {-3, 14},  {-6, 13},  {-8, 12},  {-10, 10}, {-12, 9},   {-13, 6},  {-14, 4},  {-14, 2},
    {-14, -1}, {-14, -3}, {-13, -6}, {-12, -8}, {-10, -10}, {-9, -12}, {-6, -13}, {-4, -14},
    {-2, -14}, {1, -14},  {3, -14},  {6, -13},
  },
  {
    {-10, 4},  {-11, 2},  {-11, 0}, {-11, -2}, {-10, -3}, {-9, -5}, {-8, -7}, {-7, -8}, {-6, -9},
    {-4, -10}, {-2, -11}, {0, -11}, {2, -11},  {3, -10},  {5, -9},  {7, -8},  {8, -7},  {9, -6},
    {10, -4},  {11, -2},  {11, 0},  {11, 2},   {10, 3},   {9, 5},   {8, 7},   {7, 8},   {6, 9},
    {4, 10},   {2, 11},   {0, 11},  {-2, 11},  {-3, 10},  {-5, 9},  {-7, 8},  {-8, 7},  {-9, 6},
  },
  {
    {-10, 9},  {-11, 7},  {-12, 5},  {-13, 3},  {-13, 0},  {-13, -2}, {-13, -4}, {-12, -6},
    {-11, -8}, {-9, -10}, {-7, -11}, {-5, -12}, {-3, -13}, {0, -13},  {2, -13},  {4, -13},
    {6, -12},  {8, -11},  {10, -9},  {11, -7},  {12, -5},  {13, -3},  {13, 0},   {13, 2},
    {13, 4},   {12, 6},   {11, 8},   {9, 10},   {7, 11},   {5, 12},   {3, 13},   {0, 13},
    {-2, 13},  {-4, 13},  {-6, 12},  {-8, 11},
  },
  {
    {7, 3},   {6, 4},   {6, 5},   {5, 6},   {3, 7},   {2, 7},   {1, 8},   {0, 8},  {-2, 7},
    {-3, 7},  {-4, 6},  {-5, 6},  {-6, 5},  {-7, 3},  {-7, 2},  {-8, 1},  {-8, 0}, {-7, -2},
    {-7, -3}, {-6, -4}, {-6, -5}, {-5, -6}, {-3, -7}, {-2, -7}, {-1, -8}, {0, -8}, {2, -7},
    {3, -7},  {4, -6},  {5, -6},  {6, -5},  {7, -3},  {7, -2},  {8, -1},  {8, 0},  {7, 2},
  },
  {
    {12, 4},   {11, 6},   {10, 8},   {8, 9},   {7, 11},   {5, 12},   {3, 12},   {0, 13},  {-2, 13},
    {-4, 12},  {-6, 11},  {-8, 10},  {-9, 8},  {-11, 7},  {-12, 5},  {-12, 3},  {-13, 0}, {-13, -2},
    {-12, -4}, {-11, -6}, {-10, -8}, {-8, -9}, {-7, -11}, {-5, -12}, {-3, -12}, {0, -13}, {2, -13},
    {4, -12},  {6, -11},  {8, -10},  {9, -8},  {11, -7},  {12, -5},  {12, -3},  {13, 0},  {13, 2},
  },
  {
    {9, -7},  {10, -5},  {11, -3},  {11, -2},  {11, 0},  {11, 2},   {11, 4},   {10, 6},   {8, 8},
    {7, 9},   {5, 10},   {3, 11},   {2, 11},   {0, 11},  {-2, 11},  {-4, 11},  {-6, 10},  {-8, 8},
    {-9, 7},  {-10, 5},  {-11, 3},  {-11, 2},  {-11, 0}, {-11, -2}, {-11, -4}, {-10, -6}, {-8, -8},
    {-7, -9}, {-5, -10}, {-3, -11}, {-2, -11}, {0, -11}, {2, -11},  {4, -11},  {6, -10},  {8, -8},
  },
  {
    {10, -2},  {10, 0},  {10, 2},   {10, 3},   {9, 5},   {8, 6},   {7, 8},   {5, 9},   {4, 10},
    {2, 10},   {0, 10},  {-2, 10},  {-3, 10},  {-5, 9},  {-6, 8},  {-8, 7},  {-9, 5},  {-10, 4},
    {-10, 2},  {-10, 0}, {-10, -2}, {-10, -3}, {-9, -5}, {-8, -6}, {-7, -8}, {-5, -9}, {-4, -10},
    {-2, -10}, {0, -10}, {2, -10},  {3, -10},  {5, -9},  {6, -8},  {8, -7},  {9, -5},  {10, -4},
  },
  {
    {7, 0},  {7, 1},   {7, 2},   {6, 4},   {5, 4},   {4, 5},   {4, 6},   {2, 7},   {1, 7},
    {0, 7},  {-1, 7},  {-2, 7},  {-4, 6},  {-4, 5},  {-5, 4},  {-6, 4},  {-7, 2},  {-7, 1},
    {-7, 0}, {-7, -1}, {-7, -2}, {-6, -4}, {-5, -4}, {-4, -5}, {-4, -6}, {-2, -7}, {-1, -7},
    {0, -7}, {1, -7},  {2, -7},  {4, -6},  {4, -5},  {5, -4},  {6, -4},  {7, -2},  {7, -1},
  },
  {
    {12, -2},  {12, 0},  {12, 2},   {11, 4},   {10, 6},   {9, 8},   {8, 9},   {6, 11},   {4, 11},
    {2, 12},   {0, 12},  {-2, 12},  {-4, 11},  {-6, 10},  {-8, 9},  {-9, 8},  {-11, 6},  {-11, 4},
    {-12, 2},  {-12, 0}, {-12, -2}, {-11, -4}, {-10, -6}, {-9, -8}, {-8, -9}, {-6, -11}, {-4, -11},
    {-2, -12}, {0, -12}, {2, -12},  {4, -11},  {6, -10},  {8, -9},  {9, -8},  {11, -6},  {11, -4},
  },
  {
    {-1, -6}, {0, -6}, {1, -6},  {2, -6},  {3, -5},  {4, -5},  {5, -4},  {5, -3},  {6, -2},
    {6, -1},  {6, 0},  {6, 1},   {6, 2},   {5, 3},   {5, 4},   {4, 5},   {3, 5},   {2, 6},
    {1, 6},   {0, 6},  {-1, 6},  {-2, 6},  {-3, 5},  {-4, 5},  {-5, 4},  {-5, 3},  {-6, 2},
    {-6, 1},  {-6, 0}, {-6, -1}, {-6, -2}, {-5, -3}, {-5, -4}, {-4, -5}, {-3, -5}, {-2, -6},
  },
  {
    {0, -11}, {2, -11},  {4, -10},  {6, -10},  {7, -8},  {8, -7},  {10, -6},  {10, -4},  {11, -2},
    {11, 0},  {11, 2},   {10, 4},   {10, 6},   {8, 7},   {7, 8},   {6, 10},   {4, 10},   {2, 11},
    {0, 11},  {-2, 11},  {-4, 10},  {-6, 10},  {-7, 8},  {-8, 7},  {-10, 6},  {-10, 4},  {-11, 2},
    {-11, 0}, {-11, -2}, {-10, -4}, {-10, -6}, {-8, -7}, {-7, -8}, {-6, -10}, {-4, -10}, {-2, -11},
  },
};

}  // namespace

}  // namespace c8
