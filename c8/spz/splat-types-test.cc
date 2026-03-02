// Copyright © 2024 Niantic, Inc. All rights reserved.
#include "splat-types.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <array>

using testing::Eq;
using testing::Pointwise;

namespace spz {

namespace {

decltype(auto) equals(const std::array<bool, 3> &arr) {
  return Pointwise(Eq(), arr);
}

// This implementation is copied from ScanKit/ScanKit/Neural/SplatModelCPU.cpp.
Vec3f shToColor(
  const Vec3f &dir, const float *baseColor, const float *sh) {
  std::vector<Vec3f> s(15);
  for (int i = 0; i < 15; i++) s[i] = vec3f(&sh[i * 3]);

  Vec3f color = plus(times(vec3f(baseColor), 0.282095f), {0.5f, 0.5f, 0.5f});
  const float x = dir[0], y = dir[1], z = dir[2];

  color = plus(color, times(s[0], (-0.488603f * y)));  // 1:-1
  color = plus(color, times(s[1], (0.488603f * z)));   // 1:0
  color = plus(color, times(s[2], (-0.488603f * x)));  // 1:1

  const float xy = x * y, xz = x * z, yz = y * z;
  const float x2 = x * x, y2 = y * y, z2 = z * z;
  color = plus(color, times(s[3], (1.092548f * xy)));                   // 2:-2
  color = plus(color, times(s[4], (-1.092548f * yz)));                  // 2:-1
  color = plus(color, times(s[5], (0.946175f * z2 - 0.315392f)));       // 2:0
  color = plus(color, times(s[6], (-1.092548f * xz)));                  // 2:1
  color = plus(color, times(s[7], (0.546274f * (x2 - y2))));            // 2:2
  color = plus(color, times(s[8], (-0.590044f * y * (3 * x2 - y2))));   // 3:-3
  color = plus(color, times(s[9], (2.890611f * xy * z)));               // 3:-2
  color = plus(color, times(s[10], (0.457046f * y * (1 - 5 * z2))));    // 3:-1
  color = plus(color, times(s[11], (0.373176f * z * (5 * z2 - 3))));    // 3:0
  color = plus(color, times(s[12], (0.457046f * x * (1 - 5 * z2))));    // 3:1
  color = plus(color, times(s[13], (1.445306f * z * (x2 - y2))));       // 3:2
  color = plus(color, times(s[14], (-0.590044f * x * (x2 - 3 * y2))));  // 3:3
  return color;
}

TEST(GaussianCloudTest, MedianVolume) {
  const Vec3f s0{0.5f, 0.1f, 0.1f};
  const Vec3f s1{1.0f, 0.4f, 0.2f};
  const Vec3f s2{0.2f, 0.3f, 0.6f};  // this one is the median
  const std::vector<float> logScales = {
    std::log(s0[0]),
    std::log(s0[1]),
    std::log(s0[2]),
    std::log(s1[0]),
    std::log(s1[1]),
    std::log(s1[2]),
    std::log(s2[0]),
    std::log(s2[1]),
    std::log(s2[2]),
  };
  const GaussianCloud splat = {
    .numPoints = 3,
    .shDegree = 0,
    .positions = {0, 0, 0, 0, 0, 0, 0, 0, 0},
    .scales = logScales,
    .rotations = {0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1},
    .alphas = {0, 0, 0},
    .colors = {0, 0, 0, 0, 0, 0, 0, 0, 0}};
  const float expectedVolume = 4.0f / 3.0f * M_PI * s2[0] * s2[1] * s2[2];
  EXPECT_FLOAT_EQ(expectedVolume, splat.medianVolume());
}

TEST(GaussianCloudTest, Rotate180DegAboutX) {
  const Vec3f pos{1.0f, -2.0f, 3.0f};

  Vec3f angleAxis = times(normalized(Vec3f{-1.0f, 4.0f, 2.0f}), 0.5f);

  auto quat = axisAngleQuat(angleAxis);
  std::vector<float> sh;
  for (int i = 0; i < 45; i++) {
    sh.push_back(i / 45.0f - 0.5f);
  }
  const GaussianCloud original = {
    .numPoints = 1,
    .shDegree = 3,
    .positions = {pos[0], pos[1], pos[2]},
    .scales = {2, -3, 4},
    .rotations = {quat[1], quat[2], quat[3], quat[0]},
    .alphas = {1},
    .colors = {0.5, -0.5, -0.1},
    .sh = sh};

  auto expectedRotation = axisAngleQuat(Vec3f{M_PI, 0.0f, 0.0f});

  GaussianCloud rotated = original;
  rotated.rotate180DegAboutX();

  // Verify position
  EXPECT_FLOAT_EQ(times(expectedRotation, pos)[0], rotated.positions[0]);
  EXPECT_FLOAT_EQ(times(expectedRotation, pos)[1], rotated.positions[1]);
  EXPECT_FLOAT_EQ(times(expectedRotation, pos)[2], rotated.positions[2]);

  // Verify rotation
  auto expectedQuat = times(times(expectedRotation, quat), expectedRotation);
  if (expectedQuat[0] < 0) {
    expectedQuat = times(expectedQuat, -1.0f);
  }
  EXPECT_NEAR(expectedQuat[1], rotated.rotations[0], 1e-6f);
  EXPECT_NEAR(expectedQuat[2], rotated.rotations[1], 1e-6f);
  EXPECT_NEAR(expectedQuat[3], rotated.rotations[2], 1e-6f);
  EXPECT_NEAR(expectedQuat[0], rotated.rotations[3], 1e-6f);

  // Verify color from several directions
  const auto dir1 = normalized(Vec3f{1, -2, 3});
  EXPECT_NEAR(
    shToColor(dir1, &original.colors[0], &original.sh[0])[0],
    shToColor(times(expectedRotation, dir1), &rotated.colors[0], &rotated.sh[0])[0],
    1e-6f);
  EXPECT_NEAR(
    shToColor(dir1, &original.colors[0], &original.sh[0])[1],
    shToColor(times(expectedRotation, dir1), &rotated.colors[0], &rotated.sh[0])[1],
    1e-6f);
  EXPECT_NEAR(
    shToColor(dir1, &original.colors[0], &original.sh[0])[2],
    shToColor(times(expectedRotation, dir1), &rotated.colors[0], &rotated.sh[0])[2],
    1e-6f);

  const auto dir2 = normalized(Vec3f{2, 1, -4});
  EXPECT_NEAR(
    shToColor(dir2, &original.colors[0], &original.sh[0])[0],
    shToColor(times(expectedRotation, dir2), &rotated.colors[0], &rotated.sh[0])[0],
    1e-6f);
  EXPECT_NEAR(
    shToColor(dir2, &original.colors[0], &original.sh[0])[1],
    shToColor(times(expectedRotation, dir2), &rotated.colors[0], &rotated.sh[0])[1],
    1e-6f);
  EXPECT_NEAR(
    shToColor(dir2, &original.colors[0], &original.sh[0])[2],
    shToColor(times(expectedRotation, dir2), &rotated.colors[0], &rotated.sh[0])[2],
    1e-6f);

  // These don't change:
  EXPECT_EQ(original.scales, rotated.scales);
  EXPECT_EQ(original.alphas, rotated.alphas);
  EXPECT_EQ(original.colors, rotated.colors);

  // Rotate again to get the original splat back.
  rotated.rotate180DegAboutX();
  EXPECT_EQ(original.positions, rotated.positions);
  EXPECT_EQ(original.scales, rotated.scales);
  EXPECT_EQ(original.rotations, rotated.rotations);
  EXPECT_EQ(original.alphas, rotated.alphas);
  EXPECT_EQ(original.colors, rotated.colors);
  EXPECT_EQ(original.sh, rotated.sh);
}

TEST(GaussianCloudTest, ConvertCoordinates) {
  const Vec3f pos{1.0f, -2.0f, 3.0f};

  Vec3f angleAxis = times(normalized(Vec3f{-1.0f, 4.0f, 2.0f}), 0.5f);

  auto quat = axisAngleQuat(angleAxis);
  std::vector<float> sh;
  for (int i = 0; i < 45; i++) {
    sh.push_back(i / 45.0f - 0.5f);
  }
  const GaussianCloud original = {
    .numPoints = 1,
    .shDegree = 3,
    .positions = {pos[0], pos[1], pos[2]},
    .scales = {2, -3, 4},
    .rotations = {quat[1], quat[2], quat[3], quat[0]},
    .alphas = {1},
    .colors = {0.5, -0.5, -0.1},
    .sh = sh};

  GaussianCloud rotated = original;
  rotated.convertCoordinates(CoordinateSystem::RUB, CoordinateSystem::RUF);

  // Verify position
  EXPECT_FLOAT_EQ(pos[0], rotated.positions[0]);
  EXPECT_FLOAT_EQ(pos[1], rotated.positions[1]);
  EXPECT_FLOAT_EQ(-pos[2], rotated.positions[2]);

  // Verify rotation
  Quat4f expectedQuat = {quat[0], -quat[1], -quat[2], quat[3]};
  EXPECT_NEAR(expectedQuat[1], rotated.rotations[0], 1e-6f);
  EXPECT_NEAR(expectedQuat[2], rotated.rotations[1], 1e-6f);
  EXPECT_NEAR(expectedQuat[3], rotated.rotations[2], 1e-6f);
  EXPECT_NEAR(expectedQuat[0], rotated.rotations[3], 1e-6f);

  // Verify color from several directions
  const auto dir1 = normalized(Vec3f{1, -2, 3});
  const auto dir1a = normalized(Vec3f{1, -2, -3});
  EXPECT_NEAR(
    shToColor(dir1, &original.colors[0], &original.sh[0])[0],
    shToColor(dir1a, &rotated.colors[0], &rotated.sh[0])[0],
    1e-6f);
  EXPECT_NEAR(
    shToColor(dir1, &original.colors[0], &original.sh[0])[1],
    shToColor(dir1a, &rotated.colors[0], &rotated.sh[0])[1],
    1e-6f);
  EXPECT_NEAR(
    shToColor(dir1, &original.colors[0], &original.sh[0])[2],
    shToColor(dir1a, &rotated.colors[0], &rotated.sh[0])[2],
    1e-6f);

  const auto dir2 = normalized(Vec3f{2, 1, -4});
  const auto dir2a = normalized(Vec3f{2, 1, 4});
  EXPECT_NEAR(
    shToColor(dir2, &original.colors[0], &original.sh[0])[0],
    shToColor(dir2a, &rotated.colors[0], &rotated.sh[0])[0],
    1e-6f);
  EXPECT_NEAR(
    shToColor(dir2, &original.colors[0], &original.sh[0])[1],
    shToColor(dir2a, &rotated.colors[0], &rotated.sh[0])[1],
    1e-6f);
  EXPECT_NEAR(
    shToColor(dir2, &original.colors[0], &original.sh[0])[2],
    shToColor(dir2a, &rotated.colors[0], &rotated.sh[0])[2],
    1e-6f);

  // These don't change:
  EXPECT_EQ(original.scales, rotated.scales);
  EXPECT_EQ(original.alphas, rotated.alphas);
  EXPECT_EQ(original.colors, rotated.colors);

  // Rotate again to get the original splat back.
  rotated.convertCoordinates(CoordinateSystem::RUF, CoordinateSystem::RUB);
  EXPECT_EQ(original.positions, rotated.positions);
  EXPECT_EQ(original.scales, rotated.scales);
  EXPECT_EQ(original.rotations, rotated.rotations);
  EXPECT_EQ(original.alphas, rotated.alphas);
  EXPECT_EQ(original.colors, rotated.colors);
  EXPECT_EQ(original.sh, rotated.sh);
}

TEST(GaussianCloudTest, AxesMatch) {
  std::array<bool, 3> ttt = {true, true, true};
  std::array<bool, 3> ttf = {true, true, false};
  std::array<bool, 3> tft = {true, false, true};
  std::array<bool, 3> tff = {true, false, false};
  std::array<bool, 3> ftt = {false, true, true};
  std::array<bool, 3> ftf = {false, true, false};
  std::array<bool, 3> fft = {false, false, true};
  std::array<bool, 3> fff = {false, false, false};

  // LDB
  EXPECT_THAT(axesMatch(CoordinateSystem::LDB, CoordinateSystem::UNSPECIFIED), equals(ttt));
  EXPECT_THAT(axesMatch(CoordinateSystem::UNSPECIFIED, CoordinateSystem::LDB), equals(ttt));
  EXPECT_THAT(axesMatch(CoordinateSystem::LDB, CoordinateSystem::LDB), equals(ttt));
  EXPECT_THAT(axesMatch(CoordinateSystem::LDB, CoordinateSystem::RDB), equals(ftt));
  EXPECT_THAT(axesMatch(CoordinateSystem::RDB, CoordinateSystem::LDB), equals(ftt));
  EXPECT_THAT(axesMatch(CoordinateSystem::LDB, CoordinateSystem::LUB), equals(tft));
  EXPECT_THAT(axesMatch(CoordinateSystem::LUB, CoordinateSystem::LDB), equals(tft));
  EXPECT_THAT(axesMatch(CoordinateSystem::LDB, CoordinateSystem::RUB), equals(fft));
  EXPECT_THAT(axesMatch(CoordinateSystem::RUB, CoordinateSystem::LDB), equals(fft));
  EXPECT_THAT(axesMatch(CoordinateSystem::LDB, CoordinateSystem::LDF), equals(ttf));
  EXPECT_THAT(axesMatch(CoordinateSystem::LDF, CoordinateSystem::LDB), equals(ttf));
  EXPECT_THAT(axesMatch(CoordinateSystem::LDB, CoordinateSystem::RDF), equals(ftf));
  EXPECT_THAT(axesMatch(CoordinateSystem::RDF, CoordinateSystem::LDB), equals(ftf));
  EXPECT_THAT(axesMatch(CoordinateSystem::LDB, CoordinateSystem::LUF), equals(tff));
  EXPECT_THAT(axesMatch(CoordinateSystem::LUF, CoordinateSystem::LDB), equals(tff));
  EXPECT_THAT(axesMatch(CoordinateSystem::LDB, CoordinateSystem::RUF), equals(fff));
  EXPECT_THAT(axesMatch(CoordinateSystem::RUF, CoordinateSystem::LDB), equals(fff));

  // RDB
  EXPECT_THAT(axesMatch(CoordinateSystem::RDB, CoordinateSystem::UNSPECIFIED), equals(ttt));
  EXPECT_THAT(axesMatch(CoordinateSystem::UNSPECIFIED, CoordinateSystem::RDB), equals(ttt));
  EXPECT_THAT(axesMatch(CoordinateSystem::RDB, CoordinateSystem::RDB), equals(ttt));
  EXPECT_THAT(axesMatch(CoordinateSystem::RDB, CoordinateSystem::LUB), equals(fft));
  EXPECT_THAT(axesMatch(CoordinateSystem::LUB, CoordinateSystem::RDB), equals(fft));
  EXPECT_THAT(axesMatch(CoordinateSystem::RDB, CoordinateSystem::RUB), equals(tft));
  EXPECT_THAT(axesMatch(CoordinateSystem::RUB, CoordinateSystem::RDB), equals(tft));
  EXPECT_THAT(axesMatch(CoordinateSystem::RDB, CoordinateSystem::LDF), equals(ftf));
  EXPECT_THAT(axesMatch(CoordinateSystem::LDF, CoordinateSystem::RDB), equals(ftf));
  EXPECT_THAT(axesMatch(CoordinateSystem::RDB, CoordinateSystem::RDF), equals(ttf));
  EXPECT_THAT(axesMatch(CoordinateSystem::RDF, CoordinateSystem::RDB), equals(ttf));
  EXPECT_THAT(axesMatch(CoordinateSystem::RDB, CoordinateSystem::LUF), equals(fff));
  EXPECT_THAT(axesMatch(CoordinateSystem::LUF, CoordinateSystem::RDB), equals(fff));
  EXPECT_THAT(axesMatch(CoordinateSystem::RDB, CoordinateSystem::RUF), equals(tff));
  EXPECT_THAT(axesMatch(CoordinateSystem::RUF, CoordinateSystem::RDB), equals(tff));

  // LUB
  EXPECT_THAT(axesMatch(CoordinateSystem::LUB, CoordinateSystem::UNSPECIFIED), equals(ttt));
  EXPECT_THAT(axesMatch(CoordinateSystem::UNSPECIFIED, CoordinateSystem::LUB), equals(ttt));
  EXPECT_THAT(axesMatch(CoordinateSystem::LUB, CoordinateSystem::LUB), equals(ttt));
  EXPECT_THAT(axesMatch(CoordinateSystem::LUB, CoordinateSystem::RUB), equals(ftt));
  EXPECT_THAT(axesMatch(CoordinateSystem::RUB, CoordinateSystem::LUB), equals(ftt));
  EXPECT_THAT(axesMatch(CoordinateSystem::LUB, CoordinateSystem::LDF), equals(tff));
  EXPECT_THAT(axesMatch(CoordinateSystem::LDF, CoordinateSystem::LUB), equals(tff));
  EXPECT_THAT(axesMatch(CoordinateSystem::LUB, CoordinateSystem::RDF), equals(fff));
  EXPECT_THAT(axesMatch(CoordinateSystem::RDF, CoordinateSystem::LUB), equals(fff));
  EXPECT_THAT(axesMatch(CoordinateSystem::LUB, CoordinateSystem::LUF), equals(ttf));
  EXPECT_THAT(axesMatch(CoordinateSystem::LUF, CoordinateSystem::LUB), equals(ttf));
  EXPECT_THAT(axesMatch(CoordinateSystem::LUB, CoordinateSystem::RUF), equals(ftf));
  EXPECT_THAT(axesMatch(CoordinateSystem::RUF, CoordinateSystem::LUB), equals(ftf));

  // RUB
  EXPECT_THAT(axesMatch(CoordinateSystem::RUB, CoordinateSystem::UNSPECIFIED), equals(ttt));
  EXPECT_THAT(axesMatch(CoordinateSystem::UNSPECIFIED, CoordinateSystem::RUB), equals(ttt));
  EXPECT_THAT(axesMatch(CoordinateSystem::RUB, CoordinateSystem::RUB), equals(ttt));
  EXPECT_THAT(axesMatch(CoordinateSystem::RUB, CoordinateSystem::LDF), equals(fff));
  EXPECT_THAT(axesMatch(CoordinateSystem::LDF, CoordinateSystem::RUB), equals(fff));
  EXPECT_THAT(axesMatch(CoordinateSystem::RUB, CoordinateSystem::RDF), equals(tff));
  EXPECT_THAT(axesMatch(CoordinateSystem::RDF, CoordinateSystem::RUB), equals(tff));
  EXPECT_THAT(axesMatch(CoordinateSystem::RUB, CoordinateSystem::LUF), equals(ftf));
  EXPECT_THAT(axesMatch(CoordinateSystem::LUF, CoordinateSystem::RUB), equals(ftf));
  EXPECT_THAT(axesMatch(CoordinateSystem::RUB, CoordinateSystem::RUF), equals(ttf));
  EXPECT_THAT(axesMatch(CoordinateSystem::RUF, CoordinateSystem::RUB), equals(ttf));

  // LDF
  EXPECT_THAT(axesMatch(CoordinateSystem::LDF, CoordinateSystem::UNSPECIFIED), equals(ttt));
  EXPECT_THAT(axesMatch(CoordinateSystem::UNSPECIFIED, CoordinateSystem::LDF), equals(ttt));
  EXPECT_THAT(axesMatch(CoordinateSystem::LDF, CoordinateSystem::LDF), equals(ttt));
  EXPECT_THAT(axesMatch(CoordinateSystem::LDF, CoordinateSystem::RDF), equals(ftt));
  EXPECT_THAT(axesMatch(CoordinateSystem::RDF, CoordinateSystem::LDF), equals(ftt));
  EXPECT_THAT(axesMatch(CoordinateSystem::LDF, CoordinateSystem::LUF), equals(tft));
  EXPECT_THAT(axesMatch(CoordinateSystem::LUF, CoordinateSystem::LDF), equals(tft));
  EXPECT_THAT(axesMatch(CoordinateSystem::LDF, CoordinateSystem::RUF), equals(fft));
  EXPECT_THAT(axesMatch(CoordinateSystem::RUF, CoordinateSystem::LDF), equals(fft));

  // RDF
  EXPECT_THAT(axesMatch(CoordinateSystem::RDF, CoordinateSystem::UNSPECIFIED), equals(ttt));
  EXPECT_THAT(axesMatch(CoordinateSystem::UNSPECIFIED, CoordinateSystem::RDF), equals(ttt));
  EXPECT_THAT(axesMatch(CoordinateSystem::RDF, CoordinateSystem::RDF), equals(ttt));
  EXPECT_THAT(axesMatch(CoordinateSystem::RDF, CoordinateSystem::LUF), equals(fft));
  EXPECT_THAT(axesMatch(CoordinateSystem::LUF, CoordinateSystem::RDF), equals(fft));
  EXPECT_THAT(axesMatch(CoordinateSystem::RDF, CoordinateSystem::RUF), equals(tft));
  EXPECT_THAT(axesMatch(CoordinateSystem::RUF, CoordinateSystem::RDF), equals(tft));

  // LUF
  EXPECT_THAT(axesMatch(CoordinateSystem::LUF, CoordinateSystem::UNSPECIFIED), equals(ttt));
  EXPECT_THAT(axesMatch(CoordinateSystem::UNSPECIFIED, CoordinateSystem::LUF), equals(ttt));
  EXPECT_THAT(axesMatch(CoordinateSystem::LUF, CoordinateSystem::LUF), equals(ttt));
  EXPECT_THAT(axesMatch(CoordinateSystem::LUF, CoordinateSystem::RUF), equals(ftt));
  EXPECT_THAT(axesMatch(CoordinateSystem::RUF, CoordinateSystem::LUF), equals(ftt));

  // RUF
  EXPECT_THAT(axesMatch(CoordinateSystem::RUF, CoordinateSystem::UNSPECIFIED), equals(ttt));
  EXPECT_THAT(axesMatch(CoordinateSystem::UNSPECIFIED, CoordinateSystem::RUF), equals(ttt));
  EXPECT_THAT(axesMatch(CoordinateSystem::RUF, CoordinateSystem::RUF), equals(ttt));

  // UNSPECIFIED
  EXPECT_THAT(axesMatch(CoordinateSystem::UNSPECIFIED, CoordinateSystem::UNSPECIFIED), equals(ttt));
}



}  // namespace

}  // namespace spz
