// Copyright (c) 2023 Niantic, Inc.
// Original Author: Nicholas Butko (nbutko@nianticlabs.com)

#include "bzl/inliner/rules2.h"
#include "c8/c8-log.h"

cc_library {
  hdrs = {
    "splat.h",
  };
  deps = {
    "//c8/spz:load-spz",
    "//c8:color",
    "//c8:half",
    "//c8:hmatrix",
    "//c8:hpoint",
    "//c8:hvector",
    "//c8:quaternion",
    "//c8/geometry:octree",
    "//c8:vector",
    "//c8/geometry:splat-io",
    "//c8/pixels:pixel-buffer",
    "//c8/stats:scope-timer",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xab1c7343);

#include <numeric>

#include "c8/geometry/splat.h"
#include "c8/half.h"
#include "c8/stats/scope-timer.h"

namespace c8 {

namespace {

float medianOfThree(float a, float b, float c) {
  return a + b + c - std::max({a, b, c}) - std::min({a, b, c});
}

int dimForDegree(int degree) {
  if (degree == 3) {
    return 15;
  }
  if (degree == 2) {
    return 8;
  }
  if (degree == 1) {
    return 3;
  }
  return 0;
}

uint8_t clamp(double f, double val) {
  if (f < 0.0) {
    return 0;
  }
  if (f > val) {
    return val;
  }
  return static_cast<uint8_t>(f);
}

// pads the last 10 bits of x with two zeros between each digit
uint32_t part1By2(uint32_t x) {
  x &= 0x000003ff;                   // x = ---- ---- ---- ---- ---- --98 7654 3210
  x = (x ^ (x << 16)) & 0xff0000ff;  // x = ---- --98 ---- ---- ---- ---- 7654 3210
  x = (x ^ (x << 8)) & 0x0300f00f;   // x = ---- --98 ---- ---- 7654 ---- ---- 3210
  x = (x ^ (x << 4)) & 0x030c30c3;   // x = ---- --98 ---- 76-- --54 ---- 32-- --10
  x = (x ^ (x << 2)) & 0x09249249;   // x = ---- 9--8 --7- -6-- 5--4 --3- -2-- 1--0
  return x;
}

// Returns a new SplatAttributes with the odering given by orderedIds
SplatAttributes reorderSplatAttributes(const SplatAttributes &d, Vector<uint32_t> orderedIds) {
  SplatAttributes newSplat;
  newSplat.header = d.header;
  newSplat.header.numPoints = orderedIds.size();
  {
    ScopeTimer t1("build-attributes");
    newSplat.positions.reserve(orderedIds.size());
    newSplat.rotations.reserve(orderedIds.size());
    newSplat.scales.reserve(orderedIds.size());
    newSplat.colors.reserve(orderedIds.size());

    for (int i = 0; i < orderedIds.size(); i++) {
      newSplat.positions.push_back(d.positions[orderedIds[i]]);
      newSplat.rotations.push_back(d.rotations[orderedIds[i]]);
      newSplat.scales.push_back(d.scales[orderedIds[i]]);
      newSplat.colors.push_back(d.colors[orderedIds[i]]);
      newSplat.shRed.push_back(d.shRed[orderedIds[i]]);
      newSplat.shGreen.push_back(d.shGreen[orderedIds[i]]);
      newSplat.shBlue.push_back(d.shBlue[orderedIds[i]]);
    }
  }

  return newSplat;
}

Quaternion normalizeQ(Quaternion q) {
  auto qn = q.normalize();
  return qn.w() < 0.0f ? qn.negate() : qn;
}

// Reinterpret a float as a uint32_t. reinterpret_cast<int>(float) is not allowed in c++ because
// some architectures have non-32-bit floats.
uint32_t floatAsInt(float f) {
  static_assert(sizeof(float) == sizeof(uint32_t), "float and uint32_t must be the same size");
  uint32_t i;
  std::memcpy(&i, &f, sizeof(i));
  return i;
}

}  // namespace

SplatAttributes splatAttributes(const PackedGaussians &d, bool rubToRuf) {
  SplatAttributes result;
  result.positions.resize(d.numPoints);
  result.rotations.resize(d.numPoints);
  result.scales.resize(d.numPoints);
  result.colors.resize(d.numPoints);
  result.shRed.resize(d.numPoints);
  result.shGreen.resize(d.numPoints);
  result.shBlue.resize(d.numPoints);

  float maxX = -std::numeric_limits<float>::infinity();
  float minX = std::numeric_limits<float>::infinity();
  float maxY = -std::numeric_limits<float>::infinity();
  float minY = std::numeric_limits<float>::infinity();
  float maxZ = -std::numeric_limits<float>::infinity();
  float minZ = std::numeric_limits<float>::infinity();

  spz::CoordinateSystem from = spz::CoordinateSystem::RUB;
  spz::CoordinateSystem to = rubToRuf ? spz::CoordinateSystem::RUF : spz::CoordinateSystem::RUB;
  spz::CoordinateConverter c = coordinateConverter(from, to);

  for (int i = 0; i < d.numPoints; i++) {
    auto splat = d.unpack(i, c);

    float x = splat.position[0];
    float y = splat.position[1];
    float z = splat.position[2];

    result.positions[i] = HPoint3{x, y, z};

    maxX = std::max(maxX, x);
    minX = std::min(minX, x);
    maxY = std::max(maxY, y);
    minY = std::min(minY, y);
    maxZ = std::max(maxZ, z);
    minZ = std::min(minZ, z);

    result.rotations[i] = normalizeQ(
      Quaternion{splat.rotation[3], splat.rotation[0], splat.rotation[1], splat.rotation[2]});
    result.scales[i] =
      HVector3{std::exp(splat.scale[0]), std::exp(splat.scale[1]), std::exp(splat.scale[2])};
    result.colors[i] = Color{
      clamp((0.5 + 0.282095 * splat.color[0]) * 255.0, 255.0),
      clamp((0.5 + 0.282095 * splat.color[1]) * 255.0, 255.0),
      clamp((0.5 + 0.282095 * splat.color[2]) * 255.0, 255.0),
      clamp(1.0f / (1.0f + std::exp(-splat.alpha)) * 255.0, 255.0)};

    std::array<uint8_t, 15> shR;
    std::array<uint8_t, 15> shG;
    std::array<uint8_t, 15> shB;
    for (int j = 0; j < 15; ++j) {
      int numBits = j < 3 ? 5 : 4;
      double val = j < 3 ? 31.0f : 15.0f;
      shR[j] = clamp(((splat.shR[j] + 1.0f) * 128.0f) / (1 << (8 - numBits)), val);
      shG[j] = clamp(((splat.shG[j] + 1.0f) * 128.0f) / (1 << (8 - numBits)), val);
      shB[j] = clamp(((splat.shB[j] + 1.0f) * 128.0f) / (1 << (8 - numBits)), val);
    }
    result.shRed[i] = {
      {static_cast<uint32_t>(
         shR[0] << 27 | shR[1] << 22 | shR[2] << 17 | shR[3] << 13 | shR[4] << 9 | shR[5] << 5
         | shR[6] << 1),
       static_cast<uint32_t>(
         shR[7] << 28 | shR[8] << 24 | shR[9] << 20 | shR[10] << 16 | shR[11] << 12 | shR[12] << 8
         | shR[13] << 4 | shR[14])}};
    result.shGreen[i] = {
      {static_cast<uint32_t>(
         shG[0] << 27 | shG[1] << 22 | shG[2] << 17 | shG[3] << 13 | shG[4] << 9 | shG[5] << 5
         | shG[6] << 1),
       static_cast<uint32_t>(
         shG[7] << 28 | shG[8] << 24 | shG[9] << 20 | shG[10] << 16 | shG[11] << 12 | shG[12] << 8
         | shG[13] << 4 | shG[14])}};
    result.shBlue[i] = {
      {static_cast<uint32_t>(
         shB[0] << 27 | shB[1] << 22 | shB[2] << 17 | shB[3] << 13 | shB[4] << 9 | shB[5] << 5
         | shB[6] << 1),
       static_cast<uint32_t>(
         shB[7] << 28 | shB[8] << 24 | shB[9] << 20 | shB[10] << 16 | shB[11] << 12 | shB[12] << 8
         | shB[13] << 4 | shB[14])}};
  }

  result.header = {
    .numPoints = d.numPoints,
    .maxNumPoints = d.numPoints,
    .shDegree = d.shDegree,
    .antialiased = d.antialiased,
    .width = maxX - minX,
    .height = maxY - minY,
    .depth = maxZ - minZ,
    .centerX = (maxX + minX) / 2,
    .centerY = (maxY + minY) / 2,
    .centerZ = (maxZ + minZ) / 2};

  return result;
}

SplatAttributes splatAttributes(const SplatRawData &d, bool rubToRuf) {
  SplatAttributes result;
  result.header = d.header;
  result.positions.reserve(d.header.numPoints);
  result.rotations.reserve(d.header.numPoints);
  result.scales.reserve(d.header.numPoints);
  result.colors.reserve(d.header.numPoints);
  result.shRed.reserve(d.header.numPoints);
  result.shGreen.reserve(d.header.numPoints);
  result.shBlue.reserve(d.header.numPoints);

  std::array<int, 6> shZIndexes = {
    1, 4, 6, 9, 11, 13  // The indexes of the z elements in the SH coefficients.
  };

  std::array<float, 15> sign;
  std::fill(sign.begin(), sign.end(), 1.0f);
  if (rubToRuf) {
    for (auto idx : shZIndexes) {
      sign[idx] = -1.0f;
    }
  }

  float rubToRufMul = rubToRuf ? -1.0 : 1.0;
  int shDim = dimForDegree(d.header.shDegree);
  int shStride = 3 * shDim;
  for (int i = 0; i < d.header.numPoints; i++) {
    result.positions.push_back(
      HPoint3{d.positions[i * 3], d.positions[i * 3 + 1], rubToRufMul * d.positions[i * 3 + 2]});
    result.rotations.push_back(normalizeQ(
      Quaternion{
        rubToRufMul * d.rotations[i * 4 + 0],
        d.rotations[i * 4 + 1],
        d.rotations[i * 4 + 2],
        rubToRufMul * d.rotations[i * 4 + 3]}));
    result.scales.push_back(
      HVector3{
        std::exp(d.scales[i * 3]), std::exp(d.scales[i * 3 + 1]), std::exp(d.scales[i * 3 + 2])});
    result.colors.push_back(
      Color{
        clamp((0.5 + 0.282095 * d.colors[i * 3]) * 255.0, 255.0),
        clamp((0.5 + 0.282095 * d.colors[i * 3 + 1]) * 255.0, 255.0),
        clamp((0.5 + 0.282095 * d.colors[i * 3 + 2]) * 255.0, 255.0),
        clamp(1.0f / (1.0f + std::exp(-d.alphas[i])) * 255.0, 255.0)});

    const auto *shi = &d.sh[i * shStride];
    std::array<uint8_t, 15> shR;
    std::array<uint8_t, 15> shG;
    std::array<uint8_t, 15> shB;
    std::fill(shR.begin(), shR.end(), 127);
    std::fill(shG.begin(), shG.end(), 127);
    std::fill(shB.begin(), shB.end(), 127);
    for (int j = 0, k = 0; j < shDim; ++j, k += 3) {
      int numBits = j < 3 ? 5 : 4;
      double val = j < 3 ? 31.0f : 15.0f;
      shR[j] = clamp(((sign[j] * shi[k] + 1.0f) * 128.0f) / (1 << (8 - numBits)), val);
      shG[j] = clamp(((sign[j] * shi[k + 1] + 1.0f) * 128.0f) / (1 << (8 - numBits)), val);
      shB[j] = clamp(((sign[j] * shi[k + 2] + 1.0f) * 128.0f) / (1 << (8 - numBits)), val);
    }
    result.shRed.push_back(
      {{static_cast<uint32_t>(
          shR[0] << 27 | shR[1] << 22 | shR[2] << 17 | shR[3] << 13 | shR[4] << 9 | shR[5] << 5
          | shR[6] << 1),
        static_cast<uint32_t>(
          shR[7] << 28 | shR[8] << 24 | shR[9] << 20 | shR[10] << 16 | shR[11] << 12 | shR[12] << 8
          | shR[13] << 4 | shR[14])}});
    result.shGreen.push_back(
      {{static_cast<uint32_t>(
          shG[0] << 27 | shG[1] << 22 | shG[2] << 17 | shG[3] << 13 | shG[4] << 9 | shG[5] << 5
          | shG[6] << 1),
        static_cast<uint32_t>(
          shG[7] << 28 | shG[8] << 24 | shG[9] << 20 | shG[10] << 16 | shG[11] << 12 | shG[12] << 8
          | shG[13] << 4 | shG[14])}});
    result.shBlue.push_back(
      {{static_cast<uint32_t>(
          shB[0] << 27 | shB[1] << 22 | shB[2] << 17 | shB[3] << 13 | shB[4] << 9 | shB[5] << 5
          | shB[6] << 1),
        static_cast<uint32_t>(
          shB[7] << 28 | shB[8] << 24 | shB[9] << 20 | shB[10] << 16 | shB[11] << 12 | shB[12] << 8
          | shB[13] << 4 | shB[14])}});
  }
  return result;
}

R32PlanePixelBuffer sortedIds(const SplatAttributes &d) {

  int numRows = std::ceil(std::sqrt(static_cast<float>(d.header.maxNumPoints)));
  int splatsPerRow = std::min(numRows, 4096);

  R32PlanePixelBuffer result(
    std::ceil(static_cast<float>(d.header.maxNumPoints) / splatsPerRow),
    splatsPerRow);  // Square-ish texture.

  auto *data = result.pixels().pixels();
  for (int i = 0; i < d.positions.size(); i++, data += sizeof(SortedId) / sizeof(data[0])) {
    SortedId &si = *reinterpret_cast<SortedId *>(data);
    si.id = i;
  }
  return result;
}

RGBA32PlanePixelBuffer positionColor(const SplatAttributes &d) {

  int numRows = std::ceil(std::sqrt(static_cast<float>(d.header.maxNumPoints)));
  int splatsPerRow = std::min(numRows, 4096);

  RGBA32PlanePixelBuffer result(
    std::ceil(static_cast<float>(d.header.maxNumPoints) / splatsPerRow),
    splatsPerRow);  // Square-ish texture.

  auto *data = result.pixels().pixels();
  for (int i = 0; i < d.positions.size(); i++, data += sizeof(PositionColor) / sizeof(data[0])) {
    PositionColor &ps = *reinterpret_cast<PositionColor *>(data);
    ps.position = {d.positions[i].x(), d.positions[i].y(), d.positions[i].z()};
    ps.color = {d.colors[i].r(), d.colors[i].g(), d.colors[i].b(), d.colors[i].a()};
  }

  // Fill from the last splat to the end of the texture with 0.
  std::fill(
    data, result.pixels().pixels() + result.pixels().rows() * result.pixels().rowElements(), 0);

  return result;
}

RGBA32PlanePixelBuffer rotationScale(const SplatAttributes &d) {
  int numRows = std::ceil(std::sqrt(static_cast<float>(d.header.maxNumPoints)));
  int splatsPerRow = std::min(numRows, 4096);

  RGBA32PlanePixelBuffer result(
    std::ceil(static_cast<float>(d.header.maxNumPoints) / splatsPerRow),
    splatsPerRow);  // Square-ish texture.

  auto *data = result.pixels().pixels();
  for (int i = 0; i < d.positions.size(); i++, data += sizeof(RotationScale) / sizeof(data[0])) {
    RotationScale &ps = *reinterpret_cast<RotationScale *>(data);

    auto w = d.rotations[i].w();
    auto x = d.rotations[i].x();
    auto y = d.rotations[i].y();
    auto z = d.rotations[i].z();
    auto s0 = d.scales[i].x();
    auto s1 = d.scales[i].y();
    auto s2 = d.scales[i].z();

    // R.T * S.T * S * R = [c0 c1 c2,
    //                      c1 c3 c4,
    //                      c2 c4 c5]
    // where
    // c0 = s0r00 * s0r00 + s1r10 * s1r10 + s2r20 * s2r20
    // c1 = s0r00 * s0r01 + s1r10 * s1r11 + s2r20 * s2r21
    // c2 = s0r00 * s0r02 + s1r10 * s1r12 + s2r20 * s2r22
    // c3 = s0r01 * s0r01 + s1r11 * s1r11 + s2r21 * s2r21
    // c4 = s0r01 * s0r02 + s1r11 * s1r12 + s2r21 * s2r22
    // c5 = s0r02 * s0r02 + s1r12 * s1r12 + s2r22 * s2r22
    auto s0r00 = s0 * (1.0f - 2.0f * (y * y + z * z));
    auto s1r01 = s1 * (2.0f * (x * y - w * z));
    auto s2r02 = s2 * (2.0f * (x * z + w * y));
    auto s0r10 = s0 * (2.0f * (x * y + w * z));
    auto s1r11 = s1 * (1.0f - 2.0f * (x * x + z * z));
    auto s2r12 = s2 * (2.0f * (y * z - w * x));
    auto s0r20 = s0 * (2.0f * (x * z - w * y));
    auto s1r21 = s1 * (2.0f * (y * z + w * x));
    auto s2r22 = s2 * (1.0f - 2.0f * (x * x + y * y));
    std::array<float, 6> sigma = {
      s0r00 * s0r00 + s1r01 * s1r01 + s2r02 * s2r02,
      s0r00 * s0r10 + s1r01 * s1r11 + s2r02 * s2r12,
      s0r00 * s0r20 + s1r01 * s1r21 + s2r02 * s2r22,
      s0r10 * s0r10 + s1r11 * s1r11 + s2r12 * s2r12,
      s0r10 * s0r20 + s1r11 * s1r21 + s2r12 * s2r22,
      s0r20 * s0r20 + s1r21 * s1r21 + s2r22 * s2r22};

    ps.encodedCoeffs = {
      packHalf2x16({4.0f * sigma[0], 4.0f * sigma[1]}),
      packHalf2x16({4.0f * sigma[2], 4.0f * sigma[3]}),
      packHalf2x16({4.0f * sigma[4], 4.0f * sigma[5]})};
  }
  return result;
}

RGBA32PlanePixelBuffer sh(const SplatAttributes &d, int color) {
  int numRows = std::ceil(std::sqrt(static_cast<float>(d.header.maxNumPoints)));
  int splatsPerRow = std::min(numRows, 4096);

  RGBA32PlanePixelBuffer result(
    std::ceil(static_cast<float>(d.header.maxNumPoints) / splatsPerRow),
    splatsPerRow);  // Square-ish texture.

  auto *data = result.pixels().pixels();

  if (color == 0) {  // red + green
    for (int i = 0; i < d.positions.size(); i++, data += sizeof(SH) / sizeof(data[0])) {
      SH &ps = *reinterpret_cast<SH *>(data);
      ps.sh1 = d.shRed[i];
      ps.sh2 = d.shGreen[i];
    }
  }

  if (color == 2) {  // blue
    for (int i = 0; i < d.positions.size(); i++, data += sizeof(SH) / sizeof(data[0])) {
      SH &ps = *reinterpret_cast<SH *>(data);
      ps.sh1 = d.shBlue[i];
    }
  }
  return result;
}

RGBA32PlanePixelBuffer splatTexture(const SplatAttributes &d) {
  // Each splat takes `stride` RGBA32 pixels to fill, so we need sride-x the number of splats per
  // row. The max texture size is 4096.
  int totalSize = d.positions.size() * PackedSplat::stride();
  int width = std::ceil(std::sqrt(static_cast<float>(totalSize)));
  int splatsPerRow = std::min(width, 4096) / PackedSplat::stride();

  RGBA32PlanePixelBuffer result(
    std::ceil(static_cast<float>(d.positions.size()) / splatsPerRow),
    splatsPerRow * PackedSplat::stride());

  auto *data = result.pixels().pixels();
  for (int i = 0; i < d.positions.size(); i++, data += sizeof(PackedSplat) / sizeof(data[0])) {
    PackedSplat &ps = *reinterpret_cast<PackedSplat *>(data);
    ps.position = {d.positions[i].x(), d.positions[i].y(), d.positions[i].z()};
    ps.color = {d.colors[i].r(), d.colors[i].g(), d.colors[i].b(), d.colors[i].a()};

    auto w = d.rotations[i].w();
    auto x = d.rotations[i].x();
    auto y = d.rotations[i].y();
    auto z = d.rotations[i].z();
    auto s0 = d.scales[i].x();
    auto s1 = d.scales[i].y();
    auto s2 = d.scales[i].z();

    // R.T * S.T * S * R = [c0 c1 c2,
    //                      c1 c3 c4,
    //                      c2 c4 c5]
    // where
    // c0 = s0r00 * s0r00 + s1r10 * s1r10 + s2r20 * s2r20
    // c1 = s0r00 * s0r01 + s1r10 * s1r11 + s2r20 * s2r21
    // c2 = s0r00 * s0r02 + s1r10 * s1r12 + s2r20 * s2r22
    // c3 = s0r01 * s0r01 + s1r11 * s1r11 + s2r21 * s2r21
    // c4 = s0r01 * s0r02 + s1r11 * s1r12 + s2r21 * s2r22
    // c5 = s0r02 * s0r02 + s1r12 * s1r12 + s2r22 * s2r22
    auto s0r00 = s0 * (1.0f - 2.0f * (y * y + z * z));
    auto s1r01 = s1 * (2.0f * (x * y - w * z));
    auto s2r02 = s2 * (2.0f * (x * z + w * y));
    auto s0r10 = s0 * (2.0f * (x * y + w * z));
    auto s1r11 = s1 * (1.0f - 2.0f * (x * x + z * z));
    auto s2r12 = s2 * (2.0f * (y * z - w * x));
    auto s0r20 = s0 * (2.0f * (x * z - w * y));
    auto s1r21 = s1 * (2.0f * (y * z + w * x));
    auto s2r22 = s2 * (1.0f - 2.0f * (x * x + y * y));
    std::array<float, 6> sigma = {
      s0r00 * s0r00 + s1r01 * s1r01 + s2r02 * s2r02,
      s0r00 * s0r10 + s1r01 * s1r11 + s2r02 * s2r12,
      s0r00 * s0r20 + s1r01 * s1r21 + s2r02 * s2r22,
      s0r10 * s0r10 + s1r11 * s1r11 + s2r12 * s2r12,
      s0r10 * s0r20 + s1r11 * s1r21 + s2r12 * s2r22,
      s0r20 * s0r20 + s1r21 * s1r21 + s2r22 * s2r22};

    ps.encodedCoeffs = {
      packHalf2x16({4.0f * sigma[0], 4.0f * sigma[1]}),
      packHalf2x16({4.0f * sigma[2], 4.0f * sigma[3]}),
      packHalf2x16({4.0f * sigma[4], 4.0f * sigma[5]})};

    ps.shR = d.shRed[i];
    ps.shG = d.shGreen[i];
    ps.shB = d.shBlue[i];
  }
  return result;
}

SplatAttributes mortonSortSplatAttributes(const SplatAttributes &d) {
  Vector<uint32_t> mortonIds;
  float maxX = std::numeric_limits<float>::min();
  float minX = std::numeric_limits<float>::max();
  float maxY = std::numeric_limits<float>::min();
  float minY = std::numeric_limits<float>::max();
  float maxZ = std::numeric_limits<float>::min();
  float minZ = std::numeric_limits<float>::max();

  auto numPoints = d.positions.size();
  for (int i = 0; i < numPoints; i++) {
    const auto &p = d.positions[i].raw();
    minX = std::min(minX, p[0]);
    maxX = std::max(maxX, p[0]);
    minY = std::min(minY, p[1]);
    maxY = std::max(maxY, p[1]);
    minZ = std::min(minZ, p[2]);
    maxZ = std::max(maxZ, p[2]);
  }

  float scaleX = maxX - minX;
  float scaleY = maxY - minY;
  float scaleZ = maxZ - minZ;

  const float maxUIntTenBits = (float)(std::numeric_limits<uint32_t>::max() >> 22);

  for (int i = 0; i < numPoints; i++) {
    const auto &p = d.positions[i].raw();

    // Quantise, pad and interleave the bits of position components
    uint32_t morton = part1By2((uint32_t)(maxUIntTenBits * (p[0] - minX) / scaleX));
    morton += part1By2((uint32_t)(maxUIntTenBits * (p[1] - minY) / scaleY)) << 1;
    morton += part1By2((uint32_t)(maxUIntTenBits * (p[2] - minZ) / scaleZ)) << 2;
    mortonIds.push_back(morton);
  }

  Vector<uint32_t> splatIds(numPoints);
  std::iota(splatIds.begin(), splatIds.end(), 0);

  std::sort(
    splatIds.begin(), splatIds.end(), [&](int i, int j) { return mortonIds[i] < mortonIds[j]; });

  return reorderSplatAttributes(d, splatIds);
}

// Sort splats by depth (far to near), using the camera matrix to compute depth.
SplatAttributes sortSplatAttributes(
  const SplatAttributes &d,
  const SplatOctreeNode &root,
  const HMatrix &camera,
  SortSplatOptions options) {
  ScopeTimer t("sort-splat-attributes");
  Vector<uint32_t> sortedIds;
  sortSplatIds(d, root, camera, options, &sortedIds);

  return reorderSplatAttributes(d, sortedIds);
}

int32_t sortSplatIds(
  const SplatAttributes &d,
  const SplatOctreeNode &root,
  const HMatrix &camera,
  SortSplatOptions options,
  R32PlanePixelBuffer *out) {

  Vector<uint32_t> sortedIds;
  sortSplatIds(d, root, camera, options, &sortedIds);
  int32_t numKeep = sortedIds.size();

  // fill out based on sortedIds
  memcpy(out->pixels().pixels(), sortedIds.data(), numKeep * sizeof(uint32_t));

  return numKeep;
}

void sortSplatIds(
  const SplatAttributes &loadedAttributes,
  const SplatOctreeNode &root,
  const HMatrix &camera,
  SortSplatOptions options,
  Vector<uint32_t> *out) {

  // If !lowToHigh, invert the depth so we sort from far to near.
  float inv = options.lowToHigh ? 1.0f : -1.0f;
  auto camInv = camera.inv();
  SplatAttributes d;
  // do visibility check if octree is available, to reduce number of splats needed to be sorted
  Vector<int> visibleSplatIdxs;
  if (root.numPoints() > 0) {
    int level = 6;
    root.collectVisiblePointIdxs(camera, level, &visibleSplatIdxs);
    for (auto splatIdx : visibleSplatIdxs) {
      d.positions.push_back(loadedAttributes.positions[splatIdx]);
      d.scales.push_back(loadedAttributes.scales[splatIdx]);
    }
  } else {
    d = loadedAttributes;
  }

  static std::unique_ptr<SortSplatScratch> staticScratch;
  auto *scratch = options.scratch;
  if (scratch == nullptr) {
    if (staticScratch == nullptr) {
      staticScratch = std::make_unique<SortSplatScratch>();
    }
    scratch = staticScratch.get();
  }
  auto &depths = scratch->depths;
  auto &binHistogram = scratch->binHistogram;
  auto &bins = scratch->bins;
  auto &binPtrs = scratch->binPtrs;
  auto &depthInds = scratch->depthInds;
  auto &keepInds = scratch->keepInds;

  ScopeTimer t("sort-splat-ids");

  auto numPoints = d.positions.size();
  auto numKeep = numPoints;
  float maxDepth = std::numeric_limits<float>::min();
  float minDepth = std::numeric_limits<float>::max();

  // Although depths is static, we don't need to clear it prior to resizing because we're writing to
  // every element in the next loop.
  depths.resize(numPoints);
  uint8_t keepAllDist = options.maxDistToOrigin <= 0.0f;
  uint8_t keepAllRay = options.cullRayX <= 0.0f && options.cullRayY <= 0.0f;
  uint8_t keepAllMinDist = options.minDistToCamera <= 0.0f;
  uint8_t keepAllSize = options.minSplatSize <= 0.0f;
  bool keepAll = keepAllDist & keepAllRay & keepAllSize & keepAllMinDist;
  if (keepAll) {
    // Top level branch for euclidean sort vs depth sort to avoid branching in the loop.
    if (options.euclideanSort) {
      ScopeTimer t1("find-depth-range-euclidean");
      for (int i = 0; i < numPoints; i++) {
        auto p3 = camInv * d.positions[i];
        float depth = -(p3.x() * p3.x() + p3.y() * p3.y() + p3.z() * p3.z());
        depths[i] = depth;
        maxDepth = std::max(maxDepth, depth);
        minDepth = std::min(minDepth, depth);
      }
    } else {
      ScopeTimer t1("find-depth-range-depth");
      // Sort from back to front along the z axis of the camera. This is independent of the camera's
      // current position, so we only need to do rotation and we only need to compute the z element.
      auto m20 = inv * camInv(2, 0);
      auto m21 = inv * camInv(2, 1);
      auto m22 = inv * camInv(2, 2);

      for (int i = 0; i < numPoints; i++) {
        const auto &p = d.positions[i].raw();
        float depth = m20 * p[0] + m21 * p[1] + m22 * p[2];
        depths[i] = depth;
        maxDepth = std::max(maxDepth, depth);
        minDepth = std::min(minDepth, depth);
      }
    }
  } else {
    ScopeTimer t1("find-depth-range-and-cull");
    float sqDistThreshold = options.maxDistToOrigin * options.maxDistToOrigin;
    float sqMinCameraDist = options.minDistToCamera * options.minDistToCamera;
    keepInds.resize(numPoints);

    int cullDistCount = 0;
    int cullRayCount = 0;
    int cullSizeCount = 0;

    // This implements an unconditional "or" for figuring out the depth without branching.
    // If euclideanSort is true, we sort by euclidean distance (high to low), otherwise by depth
    // (high to low unless requested otherwise).
    float sqDistMul = options.euclideanSort ? -1.0f : 0.0f;
    float depthMul = options.euclideanSort ? 0.0f : inv;

    for (int i = 0; i < numPoints; i++) {
      const auto &p = d.positions[i].raw();
      const auto px = p[0];
      const auto py = p[1];
      const auto pz = p[2];
      bool keep = true;
      auto sqDist = px * px + py * py + pz * pz;
      auto keepDist = keepAllDist | (sqDist <= sqDistThreshold);
      cullDistCount += !keepDist;
      keep &= keepDist;
      auto p3 = camInv * d.positions[i];
      auto depthForSort = sqDistMul * sqDist + depthMul * p3.z();
      auto sqrCamDist = p3.x() * p3.x() + p3.y() * p3.y() + p3.z() * p3.z();
      keep &= keepAllMinDist | (sqrCamDist >= sqMinCameraDist);
      depths[i] = depthForSort;

      auto p2 = p3.flatten();
      auto p2x = p2.x();
      auto p2y = p2.y();
      auto keepRay = keepAllRay
        | ((depthForSort < 0.0f) & (p2x >= -options.cullRayX) & (p2x <= options.cullRayX)
           & (p2y >= -options.cullRayY) & (p2y <= options.cullRayY));
      cullRayCount += !keepRay;
      keep &= keepRay;

      const auto &s = d.scales[i].raw();
      const auto sx = s[0];
      const auto sy = s[1];
      const auto sz = s[2];
      // Find the median splat scale. This is the smaller of the two splat dimensions when projected
      // into its largest size and is an upper bound.
      const auto splatSize = medianOfThree(sx, sy, sz) / std::abs(depthForSort);
      auto keepSize = keepAllSize | (splatSize > options.minSplatSize);
      cullSizeCount += !keepSize;
      keep &= keepSize;

      keepInds[i] = keep;
      maxDepth = std::max(maxDepth, depthForSort);
      minDepth = std::min(minDepth, depthForSort);
      numKeep -= !keep;
    }
    t1.addCounter("cull-dist", cullDistCount, numPoints);
    t1.addCounter("cull-ray", cullRayCount, numPoints);
    t1.addCounter("cull-size", cullSizeCount, numPoints);
    t1.addCounter("cull-total", numPoints - numKeep, numPoints);
  }

  // Below we do a bucket sort and then sort buckets. Depending on the splat, this is sometimes
  // about the same, but sometimes noticeably faster than this equivalent full sort:
  // {
  //   ScopeTimer t1("sort-without-buckets");
  //   out->resize(numPoints);
  //   std::iota(out->begin(), out->end(), 0);
  //   std::sort(out->begin(), out->begin() + numPoints, [&](int a, int b) {
  //     if (keepAll || (keepInds[a] == keepInds[b])) {
  //       return depths[a] < depths[b];
  //     }
  //     return keepInds[a] > keepInds[b];
  //   });
  //   out->resize(numKeep);
  // }

  {
    ScopeTimer t1("histogram-depth");
    float depthInv = (SORT_SPLAT_NUM_BINS - 1) / (maxDepth - minDepth);
    binHistogram.fill(0.0f);
    bins.resize(numPoints);
    for (int i = 0; i < numPoints; i++) {
      int32_t bin = static_cast<int32_t>((depths[i] - minDepth) * depthInv);
      binHistogram[bin]++;
      bins[i] = bin;
    }
  }

  {
    ScopeTimer t1("pointer-compute");
    binPtrs[0] = 0;
    for (int i = 1; i < binHistogram.size(); i++) {
      binPtrs[i] = binPtrs[i - 1] + binHistogram[i - 1];
    }
  }

  depthInds.resize(numPoints);
  {
    ScopeTimer t1("sort-to-buckets");
    // At the beginning of this loop, binPtrs points to the start of each bin (inclusive). After the
    // loop, binPtrs points to the end of each bin (exclusive).
    for (int i = 0; i < numPoints; i++) {
      // Cast depth to int. As long as this is positive, it will have the same sort order as the
      // original float. Use this as the most significant bits of the uint64_t so it determines the
      // sort order.
      uint32_t positiveDepthInt = floatAsInt(depths[i] - minDepth);
      int sortedIdx = binPtrs[bins[i]]++;
      depthInds[sortedIdx] = static_cast<uint64_t>(positiveDepthInt) << 32 | i;
    }
  }

  {
    ScopeTimer t1("sort-within-buckets");
    // At this point, depthInds is sorted by bin, but we need to sort within each bin. This is the
    // most expensive step in the process.
    int start = 0;
    for (auto end : binPtrs) {
      if (end - start > 1) {
        std::sort(depthInds.data() + start, depthInds.data() + end);
      }
      start = end;
    }
  }

  if (keepAll) {
    ScopeTimer t1("write-indexes");
    out->resize(numPoints);
    for (int i = 0; i < numPoints; i++) {
      (*out)[i] = depthInds[i] & 0xFFFFFFFF;
    }
  } else {
    ScopeTimer t1("write-filtered-indexes");
    out->resize(numKeep);
    int index = 0;
    for (int i = 0; index < numKeep; i++) {
      // This will write consecutive values to the same index until we come across one we should
      // keep, the idea being that we can do this unconditional write faster than a conditional
      // check for whether or not we should write.
      auto idx = depthInds[i] & 0xFFFFFFFF;
      (*out)[index] = idx;
      // If we should keep this value, increment the destination and write values there until we
      // find the next one we should keep.
      index += keepInds[idx];
    }
  }
  if (root.numPoints() > 0) {
    for (int i = 0; i < out->size(); i++) {
      (*out)[i] = visibleSplatIdxs[(*out)[i]];
    }
  }
}

SplatSkybox skybox(const uint8_t *data, size_t size) {
  int bytesPerIm = size / 6;                                // 6 faces on the skybox.
  int dim = static_cast<int>(std::sqrt(bytesPerIm / 4.0));  // 4 bytes per pixel.
  if (dim * dim * 24 != size) {
    C8Log("[splat.cc] Invalid skybox data size: %d", size);
    return {};
  }

  return {
    .px = {dim, dim, 4 * dim, data + 0 * bytesPerIm},
    .nx = {dim, dim, 4 * dim, data + 1 * bytesPerIm},
    .py = {dim, dim, 4 * dim, data + 2 * bytesPerIm},
    .ny = {dim, dim, 4 * dim, data + 3 * bytesPerIm},
    .pz = {dim, dim, 4 * dim, data + 4 * bytesPerIm},
    .nz = {dim, dim, 4 * dim, data + 5 * bytesPerIm},
  };
}

SplatSkybox SplatSkyboxData::skybox() { return c8::skybox(data.data(), data.size()); }

}  // namespace c8
