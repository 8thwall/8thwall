
#include "load-spz.h"

#include <gtest/gtest.h>

#include <fstream>
#include <random>
#include <sstream>
#include <string>

namespace spz {

using namespace testing;

namespace {

template <typename T>
bool vectorEquals(const std::vector<T> &a, const std::vector<T> &b, T epsilon) {
  if (a.size() != b.size()) {
    return false;
  }
  for (size_t i = 0; i < a.size(); i++) {
    if (std::abs(a[i] - b[i]) > epsilon) {
      return false;
    }
  }
  return true;
}

constexpr float SH_4BIT_EPSILON = 2.0f / 32.0f + 0.5f / 255.0f;
constexpr float SH_5BIT_EPSILON = 2.0f / 64.0f + 0.5f / 255.0f;

std::string readFile(const std::string &path) {
  std::ifstream in(path.c_str(), std::ios::binary);
  std::stringstream s;
  s << in.rdbuf();
  return s.str();
}

GaussianCloud makeTestGaussianCloud(bool includeSH) {
  GaussianCloud gaussians = {
    .numPoints = 2,
    .antialiased = true,
    .positions = {0, 0.1, -0.2, 0.3, 0.4, 0.5},
    .scales = {-3, -2, -1.5, -1, 0, 0.1},
    .rotations = {-0.5, 0.2, 1, -0.2, 0.1, -0.4, -0.3, 0.5},
    .alphas = {-1.0, 1.0},
    .colors = {-1, 0, 1, -0.5, 0.5, 0.1},
  };
  if (includeSH) {
    for (int i = 0; i < 90; i++) {
      gaussians.sh.push_back(static_cast<float>(i) / 45.0f - 1.0f);
    }
    gaussians.shDegree = 3;
  }
  return gaussians;
}

}  // namespace

TEST(SplatIOTest, SaveLoadPackedFormat) {
  GaussianCloud src = makeTestGaussianCloud(true);

  std::string filename = ::testing::TempDir() + "SplatIOTest_SaveLoad.spz";
  EXPECT_TRUE(saveSpz(src, {}, filename));

  // Read back in and verify we get the same thing we wrote, accounting for lossy packing.
  // The packing/unpacking is tested in the PackUnpackGaussians test, so we assume it's correct here
  GaussianCloud dst = loadSpz(filename, {});
  EXPECT_EQ(dst.numPoints, 2);
  EXPECT_EQ(dst.shDegree, 3);
  EXPECT_EQ(dst.antialiased, true);
  EXPECT_TRUE(vectorEquals(dst.positions, src.positions, 1 / 2048.0f));
  EXPECT_TRUE(vectorEquals(dst.scales, src.scales, 1 / 32.0f));

  // Check that the old and new quaternions represent the same rotation
  EXPECT_EQ(dst.rotations.size(), 8);
  auto q0 = quat4f(&dst.rotations[0]);
  auto q1 = quat4f(&dst.rotations[4]);
  auto origQ0 = normalized(quat4f(&src.rotations[0]));
  auto origQ1 = normalized(quat4f(&src.rotations[4]));
  printf("origQ0: %f %f %f %f\n", origQ0[0], origQ0[1], origQ0[2], origQ0[3]);
  printf("q0: %f %f %f %f\n", q0[0], q0[1], q0[2], q0[3]);
  printf("origQ1: %f %f %f %f\n", origQ1[0], origQ1[1], origQ1[2], origQ1[3]);
  printf("q1: %f %f %f %f\n", q1[0], q1[1], q1[2], q1[3]);
  EXPECT_NEAR(norm(q0), 1.0f, 1e-6f);
  EXPECT_NEAR(norm(q1), 1.0f, 1e-6f);
  auto v1 = Vec3f{3.0f, -2.0f, 0.2f}, v2 = Vec3f{-1.0f, 0.5f, -3.0f};
  auto a = times(q0, v1), b = times(origQ0, v1);
  EXPECT_NEAR(dot(a, b) / (norm(a) * norm(b)), 1.0f, 1e-4f);
  a = times(q0, v2), b = times(origQ0, v2);
  EXPECT_NEAR(dot(a, b) / (norm(a) * norm(b)), 1.0f, 1e-4f);
  a = times(q1, v1), b = times(origQ1, v1);
  EXPECT_NEAR(dot(a, b) / (norm(a) * norm(b)), 1.0f, 1e-4f);
  a = times(q1, v2), b = times(origQ1, v2);
  EXPECT_NEAR(dot(a, b) / (norm(a) * norm(b)), 1.0f, 1e-4f);

  EXPECT_TRUE(vectorEquals(dst.alphas, src.alphas, 0.01f));
  EXPECT_TRUE(vectorEquals(dst.sh, src.sh, SH_4BIT_EPSILON));
  // The degree 1 spherical harmonics for each point has an extra bit and should be more precise
  EXPECT_TRUE(vectorEquals(
    std::vector<float>(dst.sh.begin(), dst.sh.begin() + 9),
    std::vector<float>(src.sh.begin(), src.sh.begin() + 9),
    SH_5BIT_EPSILON));
  EXPECT_TRUE(vectorEquals(
    std::vector<float>(dst.sh.begin() + 45, dst.sh.begin() + 45 + 9),
    std::vector<float>(src.sh.begin() + 45, src.sh.begin() + 45 + 9),
    SH_5BIT_EPSILON));
}

TEST(SplatIOTest, SpzRdfToRub) {
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

  std::string filename = ::testing::TempDir() + "SplatIOTest_SaveLoad.spz";
  EXPECT_TRUE(saveSpz(original, {.from = CoordinateSystem::RDF}, filename));

  // Load loads in RUB by default.
  GaussianCloud rotated = loadSpz(filename, {});

  // Verify position
  EXPECT_FLOAT_EQ(times(expectedRotation, pos)[0], rotated.positions[0]);
  EXPECT_FLOAT_EQ(times(expectedRotation, pos)[1], rotated.positions[1]);
  EXPECT_FLOAT_EQ(times(expectedRotation, pos)[2], rotated.positions[2]);

  // Verify rotation
  auto expectedQuat = times(times(expectedRotation, quat), expectedRotation);
  if (expectedQuat[0] < 0) {
    expectedQuat = times(expectedQuat, -1.0f);
  }
  EXPECT_NEAR(expectedQuat[1], rotated.rotations[0], 5e-3f);
  EXPECT_NEAR(expectedQuat[2], rotated.rotations[1], 5e-3f);
  EXPECT_NEAR(expectedQuat[3], rotated.rotations[2], 5e-3f);
  EXPECT_NEAR(expectedQuat[0], rotated.rotations[3], 5e-3f);

  // Verify SH
  CoordinateConverter c = coordinateConverter(CoordinateSystem::RDF, CoordinateSystem::RUB);
  auto expectedSh = original.sh;
  for (int i = 0; i < 15; ++i) {
    expectedSh[i * 3 + 0] *= c.flipSh[i];
    expectedSh[i * 3 + 1] *= c.flipSh[i];
    expectedSh[i * 3 + 2] *= c.flipSh[i];
  }
  EXPECT_TRUE(vectorEquals(rotated.sh, expectedSh, SH_4BIT_EPSILON));

  // These don't change:
  EXPECT_TRUE(vectorEquals(original.scales, rotated.scales, 1 / 32.0f));
  EXPECT_TRUE(vectorEquals(original.alphas, rotated.alphas, 0.01f));
  EXPECT_TRUE(vectorEquals(original.colors, rotated.colors, 0.01f));

  // Load in RDF to get the original splat back.
  rotated = loadSpz(filename, {.to = CoordinateSystem::RDF});
  EXPECT_TRUE(vectorEquals(original.positions, rotated.positions, 1 / 2048.0f));
  EXPECT_TRUE(vectorEquals(original.scales, rotated.scales, 1 / 32.0f));
  EXPECT_TRUE(vectorEquals(original.rotations, rotated.rotations, 0.01f));
  EXPECT_TRUE(vectorEquals(original.alphas, rotated.alphas, 0.01f));
  EXPECT_TRUE(vectorEquals(original.colors, rotated.colors, 0.01f));
  EXPECT_TRUE(vectorEquals(original.sh, rotated.sh, SH_4BIT_EPSILON));
}

TEST(SplatIOTest, SpzRdfToRubPacked) {
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

  std::string filename = ::testing::TempDir() + "SplatIOTest_SaveLoad.spz";
  EXPECT_TRUE(saveSpz(original, {.from = CoordinateSystem::RDF}, filename));

  // Load loads in RUB by default.
  auto packed = loadSpzPacked(filename);
  auto rotated = packed.unpack(0, {});

  // Verify position
  EXPECT_FLOAT_EQ(times(expectedRotation, pos)[0], rotated.position[0]);
  EXPECT_FLOAT_EQ(times(expectedRotation, pos)[1], rotated.position[1]);
  EXPECT_FLOAT_EQ(times(expectedRotation, pos)[2], rotated.position[2]);

  // Verify rotation
  auto expectedQuat = times(times(expectedRotation, quat), expectedRotation);
  if (expectedQuat[0] < 0) {
    expectedQuat = times(expectedQuat, -1.0f);
  }
  EXPECT_NEAR(expectedQuat[1], rotated.rotation[0], 5e-3f);
  EXPECT_NEAR(expectedQuat[2], rotated.rotation[1], 5e-3f);
  EXPECT_NEAR(expectedQuat[3], rotated.rotation[2], 5e-3f);
  EXPECT_NEAR(expectedQuat[0], rotated.rotation[3], 5e-3f);

  // Verify SH
  CoordinateConverter c = coordinateConverter(CoordinateSystem::RDF, CoordinateSystem::RUB);
  for (int i = 0; i < 15; ++i) {
    EXPECT_NEAR(c.flipSh[i] * original.sh[i * 3 + 0], rotated.shR[i], SH_4BIT_EPSILON);
    EXPECT_NEAR(c.flipSh[i] * original.sh[i * 3 + 1],  rotated.shG[i], SH_4BIT_EPSILON);
    EXPECT_NEAR(c.flipSh[i] * original.sh[i * 3 + 2],  rotated.shB[i], SH_4BIT_EPSILON);
  }

  // These don't change:
  EXPECT_NEAR(original.scales[0], rotated.scale[0], 1 / 32.0f);
  EXPECT_NEAR(original.scales[1], rotated.scale[1], 1 / 32.0f);
  EXPECT_NEAR(original.scales[2], rotated.scale[2], 1 / 32.0f);
  EXPECT_NEAR(original.alphas[0], rotated.alpha, 0.01f);
  EXPECT_NEAR(original.colors[0], rotated.color[0], 0.01f);
  EXPECT_NEAR(original.colors[1], rotated.color[1], 0.01f);
  EXPECT_NEAR(original.colors[2], rotated.color[2], 0.01f);

  // Load in RDF to get the original splat back. c is symmetric for to/from, so we can use the same
  // one.
  rotated = packed.unpack(0, c);
  EXPECT_NEAR(original.positions[0], rotated.position[0], 1 / 2048.0f);
  EXPECT_NEAR(original.positions[1], rotated.position[1], 1 / 2048.0f);
  EXPECT_NEAR(original.positions[2], rotated.position[2], 1 / 2048.0f);
  EXPECT_NEAR(original.scales[0], rotated.scale[0], 1 / 32.0f);
  EXPECT_NEAR(original.scales[1], rotated.scale[1], 1 / 32.0f);
  EXPECT_NEAR(original.scales[2], rotated.scale[2], 1 / 32.0f);
  EXPECT_NEAR(original.rotations[0], rotated.rotation[0], 0.01f);
  EXPECT_NEAR(original.rotations[1], rotated.rotation[1], 0.01f);
  EXPECT_NEAR(original.rotations[2], rotated.rotation[2], 0.01f);
  EXPECT_NEAR(original.rotations[3], rotated.rotation[3], 0.01f);
  EXPECT_NEAR(original.alphas[0], rotated.alpha, 0.01f);
  EXPECT_NEAR(original.colors[0], rotated.color[0], 0.01f);
  EXPECT_NEAR(original.colors[1], rotated.color[1], 0.01f);
  EXPECT_NEAR(original.colors[2], rotated.color[2], 0.01f);
  for (int i = 0; i < 15; ++i) {
    EXPECT_NEAR(original.sh[i * 3 + 0], rotated.shR[i], SH_4BIT_EPSILON);
    EXPECT_NEAR(original.sh[i * 3 + 1],  rotated.shG[i], SH_4BIT_EPSILON);
    EXPECT_NEAR(original.sh[i * 3 + 2],  rotated.shB[i], SH_4BIT_EPSILON);
  }
}

TEST(SplatIOTest, SaveLoadPackedFormatLargeSplat) {
  GaussianCloud src = {.numPoints = 50000, .shDegree = 3};
  std::mt19937 rng(1);
  std::uniform_real_distribution<float> dist(0.0f, 1.0f);
  for (int i = 0; i < src.numPoints; i++) {
    for (int j = 0; j < 3; j++) {
      src.positions.push_back(dist(rng) * 2.0f - 1.0f);
      src.scales.push_back(dist(rng) - 1.0f);
      src.rotations.push_back(dist(rng) * 2.0f - 1.0f);
      src.colors.push_back(dist(rng));
    }
    src.rotations.push_back(dist(rng) * 2.0f - 1.0f);
    src.alphas.push_back(dist(rng));
    for (int j = 0; j < 45; j++) {
      src.sh.push_back(dist(rng) - 0.5f);
    }
  }

  std::vector<uint8_t> data;
  EXPECT_TRUE(saveSpz(src, {}, &data));
  EXPECT_LE(data.size(), 40 * src.numPoints);
  GaussianCloud dst = loadSpz(data, {});
  EXPECT_EQ(dst.numPoints, src.numPoints);
  EXPECT_EQ(dst.shDegree, src.shDegree);
  EXPECT_TRUE(vectorEquals(dst.positions, src.positions, 1 / 2048.0f));
  EXPECT_TRUE(vectorEquals(dst.scales, src.scales, 1 / 16.0f));
  EXPECT_EQ(dst.rotations.size(), src.rotations.size());
  EXPECT_TRUE(vectorEquals(dst.alphas, src.alphas, 0.01f));
  EXPECT_TRUE(vectorEquals(dst.sh, src.sh, 2.0f / 32.0f + 1.0f / 255.0f));
}

TEST(SplatIOTest, SHEncodingForZerosAndEdges) {
  GaussianCloud src = {
    .numPoints = 1,
    .shDegree = 1,
    .positions = {0, 0, 0},
    .scales = {0, 0, 0},
    .rotations = {0, 0, 0, 1},
    .alphas = {0},
    .colors = {0, 0, 0},
    .sh = {-0.01f, 0.0f, 0.01f, -1.0f, -0.99f, -0.95f, 0.95f, 0.99f, 1.0f}};
  std::vector<uint8_t> data;
  EXPECT_TRUE(saveSpz(src, {}, &data));
  GaussianCloud dst = loadSpz(data, {});
  EXPECT_EQ(dst.numPoints, 1);
  EXPECT_EQ(dst.shDegree, 1);
  EXPECT_TRUE(vectorEquals(
    dst.sh,
    std::vector<float>{0.0f, 0.0f, 0.0f, -1.0f, -1.0f, -0.9375f, 0.9375f, 0.9922f, 0.9922f},
    2e-5f));
}

TEST(SplatIOTest, SaveLoadPLY) {
  GaussianCloud src = makeTestGaussianCloud(false);

  // Write to a file
  std::string filename = ::testing::TempDir() + "SplatIOTest_SaveLoad.ply";
  EXPECT_TRUE(saveSplatToPly(src, {}, filename));

  // Make sure it looks like a PLY
  std::string ply = readFile(filename);
  EXPECT_TRUE(ply.starts_with("ply\nformat binary_little_endian 1.0\nelement vertex 2\n"));

  // Read back in and verify we get the same thing we wrote
  GaussianCloud dst = loadSplatFromPly(filename, {});
  EXPECT_EQ(dst.numPoints, 2);
  EXPECT_EQ(dst.shDegree, 0);
  EXPECT_EQ(dst.positions, src.positions);
  EXPECT_EQ(dst.scales, src.scales);
  EXPECT_EQ(dst.rotations, src.rotations);
  EXPECT_EQ(dst.alphas, src.alphas);
  EXPECT_EQ(dst.colors, src.colors);
  EXPECT_EQ(dst.sh.size(), 0);

  // Repeat with spherical harmonics
  src = makeTestGaussianCloud(true);
  saveSplatToPly(src, {}, filename);
  dst = loadSplatFromPly(filename, {});
  EXPECT_EQ(dst.numPoints, 2);
  EXPECT_EQ(dst.shDegree, 3);
  EXPECT_EQ(dst.positions, src.positions);
  EXPECT_EQ(dst.scales, src.scales);
  EXPECT_EQ(dst.rotations, src.rotations);
  EXPECT_EQ(dst.alphas, src.alphas);
  EXPECT_EQ(dst.colors, src.colors);
  EXPECT_EQ(dst.sh, src.sh);
}

TEST(SplatIOTest, PlyRdfToRub) {
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

  std::string filename = ::testing::TempDir() + "SplatIOTest_SaveLoad.spz";
  EXPECT_TRUE(saveSplatToPly(original, {.from = CoordinateSystem::RUB}, filename));

  // Load loads in RDF by default.
  GaussianCloud rotated = loadSplatFromPly(filename, {});

  // Verify position
  EXPECT_FLOAT_EQ(times(expectedRotation, pos)[0], rotated.positions[0]);
  EXPECT_FLOAT_EQ(times(expectedRotation, pos)[1], rotated.positions[1]);
  EXPECT_FLOAT_EQ(times(expectedRotation, pos)[2], rotated.positions[2]);

  // Verify rotation
  auto expectedQuat = times(times(expectedRotation, quat), expectedRotation);
  if (expectedQuat[0] < 0) {
    expectedQuat = times(expectedQuat, -1.0f);
  }
  EXPECT_NEAR(expectedQuat[1], rotated.rotations[0], 5e-3f);
  EXPECT_NEAR(expectedQuat[2], rotated.rotations[1], 5e-3f);
  EXPECT_NEAR(expectedQuat[3], rotated.rotations[2], 5e-3f);
  EXPECT_NEAR(expectedQuat[0], rotated.rotations[3], 5e-3f);

  // Verify SH
  CoordinateConverter c = coordinateConverter(CoordinateSystem::RDF, CoordinateSystem::RUB);
  auto expectedSh = original.sh;
  for (int i = 0; i < 15; ++i) {
    expectedSh[i * 3 + 0] *= c.flipSh[i];
    expectedSh[i * 3 + 1] *= c.flipSh[i];
    expectedSh[i * 3 + 2] *= c.flipSh[i];
  }
  EXPECT_TRUE(vectorEquals(rotated.sh, expectedSh, SH_4BIT_EPSILON));

  // These don't change:
  EXPECT_TRUE(vectorEquals(original.scales, rotated.scales, 1 / 32.0f));
  EXPECT_TRUE(vectorEquals(original.alphas, rotated.alphas, 0.01f));
  EXPECT_TRUE(vectorEquals(original.colors, rotated.colors, 0.01f));

  // Load in RUB to get the original splat back.
  rotated = loadSplatFromPly(filename, {.to = CoordinateSystem::RUB});
  EXPECT_TRUE(vectorEquals(original.positions, rotated.positions, 1 / 2048.0f));
  EXPECT_TRUE(vectorEquals(original.scales, rotated.scales, 1 / 32.0f));
  EXPECT_TRUE(vectorEquals(original.rotations, rotated.rotations, 0.01f));
  EXPECT_TRUE(vectorEquals(original.alphas, rotated.alphas, 0.01f));
  EXPECT_TRUE(vectorEquals(original.colors, rotated.colors, 0.01f));
  EXPECT_TRUE(vectorEquals(original.sh, rotated.sh, SH_4BIT_EPSILON));
}

}  // namespace spz
