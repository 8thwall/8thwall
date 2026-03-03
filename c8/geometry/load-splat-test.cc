// Copyright (c) 2023 Niantic, Inc.
// Original Author: Dat Chu (datchu@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  data =
    {
      "//c8/geometry/testdata:splat-asset",
    },
  deps = {
    ":load-splat",
    "//c8/geometry:splat",
    "@com_google_googletest//:gtest_main",
  };
  data = {
    "//c8:c8-log",
  };
}
cc_end(0xb2c5f5f2);

#include "c8/geometry/load-splat.h"
#include "c8/geometry/splat.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace c8 {

class LoadSplatTest : public ::testing::Test {};

const char *SPZ_FILE = "c8/geometry/testdata/doty-platformer.spz";

TEST_F(LoadSplatTest, TestLoadSpzSplat) {
  auto splatRaw = loadSplatSpzFile(SPZ_FILE);
  EXPECT_EQ(323717, splatRaw.header.numPoints);
  EXPECT_EQ(3, splatRaw.header.shDegree);
  EXPECT_EQ(0, splatRaw.header.antialiased);
  EXPECT_EQ(971151, splatRaw.positions.size());
  EXPECT_EQ(971151, splatRaw.scales.size());
  EXPECT_EQ(1294868, splatRaw.rotations.size());
  EXPECT_EQ(323717, splatRaw.alphas.size());
  EXPECT_EQ(971151, splatRaw.colors.size());
  EXPECT_EQ(14567265, splatRaw.sh.size());
}

TEST_F(LoadSplatTest, TestAutoLoadSplatByHeader) {
  auto splatRaw = loadSplat(SPZ_FILE);
  EXPECT_EQ(323717, splatRaw.header.numPoints);
  EXPECT_EQ(3, splatRaw.header.shDegree);
  EXPECT_EQ(0, splatRaw.header.antialiased);
  EXPECT_EQ(971151, splatRaw.positions.size());
  EXPECT_EQ(971151, splatRaw.scales.size());
  EXPECT_EQ(1294868, splatRaw.rotations.size());
  EXPECT_EQ(323717, splatRaw.alphas.size());
  EXPECT_EQ(971151, splatRaw.colors.size());
  EXPECT_EQ(14567265, splatRaw.sh.size());
}

float quantize(float v, int bits) {
  return int((v + 1.0f) * 128.0f / float(1 << (8 - bits))) * float(1 << (8 - bits)) / 128.0f - 1.0f;
}

TEST_F(LoadSplatTest, TestPackingSplat) {
  auto splatRaw = loadSplat(SPZ_FILE);
  bool rubToRuf = true;
  auto d = splatAttributes(splatRaw, rubToRuf);
  auto texture = splatTexture(d);
  auto *data = texture.pixels().pixels();
  constexpr float MAX_RAW_SH = 254.0f / 256.0f;

  EXPECT_EQ(splatRaw.sh.size() / 45, d.positions.size());

  for (int i = 0; i < d.positions.size(); i++, data += sizeof(PackedSplat) / sizeof(data[0])) {
    PackedSplat &ps = *reinterpret_cast<PackedSplat *>(data);
    float raw0 = splatRaw.sh[45 * i + 0];
    float raw1 = splatRaw.sh[45 * i + 1];
    float raw2 = splatRaw.sh[45 * i + 2];
    float r0 = float((ps.shR[0] >> 27) & 0x1f) * (1 << (8 - 5)) / 128.0f - 1.0f;
    float g1 = float((ps.shG[0] >> 27) & 0x1f) * (1 << (8 - 5)) / 128.0f - 1.0f;
    float b2 = float((ps.shB[0] >> 27) & 0x1f) * (1 << (8 - 5)) / 128.0f - 1.0f;
    if (raw0 != MAX_RAW_SH) {
      EXPECT_EQ(raw0, r0);
    } else {
      EXPECT_EQ(0.9375f, r0);
    }
    if (raw1 != MAX_RAW_SH) {
      EXPECT_EQ(raw1, g1);
    } else {
      EXPECT_EQ(0.9375f, g1);
    }
    if (raw2 != MAX_RAW_SH) {
      EXPECT_EQ(raw2, b2);
    } else {
      EXPECT_EQ(0.9375f, b2);
    }
    EXPECT_EQ(quantize(raw0, 5), r0);
    EXPECT_EQ(quantize(raw1, 5), g1);
    EXPECT_EQ(quantize(raw2, 5), b2);

    float raw39 = -1 * splatRaw.sh[45 * i + 39];
    float raw40 = -1 * splatRaw.sh[45 * i + 40];
    float raw41 = -1 * splatRaw.sh[45 * i + 41];
    float r39 = float((ps.shR[1] >> 4) & 0x0f) * (1 << (8 - 4)) / 128.0f - 1.0f;
    float g40 = float((ps.shG[1] >> 4) & 0x0f) * (1 << (8 - 4)) / 128.0f - 1.0f;
    float b41 = float((ps.shB[1] >> 4) & 0x0f) * (1 << (8 - 4)) / 128.0f - 1.0f;
    EXPECT_EQ(quantize(raw39, 4), r39);
    EXPECT_EQ(quantize(raw40, 4), g40);
    if (raw41 != 1) {  // TODO: figure out why there is a 1
      EXPECT_EQ(quantize(raw41, 4), b41);
    }

    float raw42 = splatRaw.sh[45 * i + 42];
    float raw43 = splatRaw.sh[45 * i + 43];
    float raw44 = splatRaw.sh[45 * i + 44];
    float r42 = float((ps.shR[1] >> 0) & 0x0f) * (1 << (8 - 4)) / 128.0f - 1.0f;
    float g43 = float((ps.shG[1] >> 0) & 0x0f) * (1 << (8 - 4)) / 128.0f - 1.0f;
    float b44 = float((ps.shB[1] >> 0) & 0x0f) * (1 << (8 - 4)) / 128.0f - 1.0f;
    EXPECT_EQ(quantize(raw42, 4), r42);
    EXPECT_EQ(quantize(raw43, 4), g43);
    EXPECT_EQ(quantize(raw44, 4), b44);
  }
}

}  // namespace c8
