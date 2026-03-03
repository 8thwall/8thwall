// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "c8/hmatrix.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "c8/exceptions.h"
#include "c8/hpoint.h"
#include "c8/hvector.h"

using testing::Eq;
using testing::Pointwise;

namespace c8 {

class HMatrixTest : public ::testing::Test {};

MATCHER_P(AreWithin, eps, "") { return fabs(testing::get<0>(arg) - testing::get<1>(arg)) < eps; }

decltype(auto) equalsMatrix(const HMatrix &matrix) {
  return Pointwise(AreWithin(0.0001), matrix.data());
}

decltype(auto) equalsMatrix(
  const float (&row0)[4], const float (&row1)[4], const float (&row2)[4], const float (&row3)[4]) {
  HMatrix matrix(row0, row1, row2, row3);
  return Pointwise(AreWithin(0.0001), matrix.data());
}

decltype(auto) equalsPoint(const HPoint2 &point) {
  return Pointwise(AreWithin(0.0001), point.data());
}

decltype(auto) equalsPoint(const HPoint3 &point) {
  return Pointwise(AreWithin(0.0001), point.data());
}

decltype(auto) equalsPoint(float x, float y) {
  return Pointwise(AreWithin(0.0001), HPoint2(x, y).data());
}

decltype(auto) equalsPoint(float x, float y, float z) {
  return Pointwise(AreWithin(0.0001), HPoint3(x, y, z).data());
}

decltype(auto) equalsHVector(const HVector2 &vec) {
  return Pointwise(AreWithin(0.0001), vec.data());
}

decltype(auto) equalsHVector(const HVector3 &vec) {
  return Pointwise(AreWithin(0.0001), vec.data());
}

decltype(auto) equalsHVector(float x, float y) {
  return Pointwise(AreWithin(0.0001), HVector2(x, y).data());
}

decltype(auto) equalsHVector(float x, float y, float z) {
  return Pointwise(AreWithin(0.0001), HVector3(x, y, z).data());
}

TEST_F(HMatrixTest, IdentityTest) {
  HMatrix identity = HMatrixGen::i();

  HPoint3 pt0 = {1.0f, 2.0f, 3.0f};
  HPoint3 pt1 = identity * pt0;
  EXPECT_THAT(pt0.data(), equalsPoint(pt1));

  HVector3 v0 = {1.0f, 2.0f, 3.0f};
  HVector3 v1 = identity * v0;
  EXPECT_THAT(v0.data(), equalsHVector(v1));

  EXPECT_THAT(
    identity.data(),
    equalsMatrix(
      {1.0f, 0.0f, 0.0f, 0.0f},
      {0.0f, 1.0f, 0.0f, 0.0f},
      {0.0f, 0.0f, 1.0f, 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f}));
}

TEST_F(HMatrixTest, InversionTest) {
  HMatrix matrix{
    {2.0f, 0.0f, 0.0f, 4.0f},
    {0.0f, 2.0f, 0.0f, -4.0f},
    {0.0f, 0.0f, 1.0f, 8.0f},
    {0.0f, 0.0f, 0.0f, 1.0f}};

  EXPECT_THAT(
    matrix.inverseData(),
    equalsMatrix(
      {0.5f, 0.0f, 0.0f, -2.0f},
      {0.0f, 0.5f, 0.0f, 2.0f},
      {0.0f, 0.0f, 1.0f, -8.0f},
      {0.0f, 0.0f, 0.0f, 1.0f}));
}

TEST_F(HMatrixTest, RotationsTest) {
  EXPECT_THAT(
    HMatrixGen::xDegrees(90.0f).data(),
    equalsMatrix(
      {1.0f, 0.0f, 0.0f, 0.0f},
      {0.0f, 0.0f, -1.0f, 0.0f},
      {0.0f, 1.0f, 0.0f, 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f}));

  EXPECT_THAT(
    HMatrixGen::yDegrees(90.0f).data(),
    equalsMatrix(
      {0.0f, 0.0f, 1.0f, 0.0f},
      {0.0f, 1.0f, 0.0f, 0.0f},
      {-1.0f, 0.0f, 0.0f, 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f}));

  EXPECT_THAT(
    HMatrixGen::zDegrees(90.0f).data(),
    equalsMatrix(
      {0.0f, -1.0f, 0.0f, 0.0f},
      {1.0f, 0.0f, 0.0f, 0.0f},
      {0.0f, 0.0f, 1.0f, 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f}));
}

TEST_F(HMatrixTest, z90Test) {
  EXPECT_THAT(HMatrixGen::z90().data(), equalsMatrix(HMatrixGen::zDegrees(90.0f)));

  EXPECT_THAT(HMatrixGen::z180().data(), equalsMatrix(HMatrixGen::zDegrees(180.0f)));

  EXPECT_THAT(HMatrixGen::z270().data(), equalsMatrix(HMatrixGen::zDegrees(270.0f)));
}

TEST_F(HMatrixTest, TransposeTest) {
  HMatrix m{
    {1.0f, -1.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, -1.0f, 1.0f},
    {-1.0f, 1.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 1.0f, -1.0f},
    {0.25f, 0.25f, -0.25f, 0.25f},
    {-0.25f, 0.25f, 0.25f, 0.25f},
    {0.25f, -0.25f, 0.25f, 0.25f},
    {0.25f, 0.25f, 0.25f, -0.25f}};

  HMatrix t{
    {1.0f, 1.0f, -1.0f, 1.0f},
    {-1.0f, 1.0f, 1.0f, 1.0f},
    {1.0f, -1.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 1.0f, -1.0f},
    {0.25f, -0.25f, 0.25f, 0.25f},
    {0.25f, 0.25f, -0.25f, 0.25f},
    {-0.25f, 0.25f, 0.25f, 0.25f},
    {0.25f, 0.25f, 0.25f, -0.25f}};

  EXPECT_THAT(m.t().data(), equalsMatrix(t));
  EXPECT_THAT(m.t().inv().data(), equalsMatrix(t.inv()));

  HMatrix mni{
    {1.0f, -1.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, -1.0f, 1.0f},
    {-1.0f, 1.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 1.0f, -1.0f},
    true};

  EXPECT_THAT(mni.t().data(), equalsMatrix(t));

  try {
    mni.t().inv();
    FAIL() << "Error expected; Matrix is not invertable.";
  } catch (RuntimeError e) {
    // Pass.
  }
}

TEST_F(HMatrixTest, InverseRotationsTest) {
  auto x90 = HMatrixGen::xDegrees(60.0f);
  auto y90 = HMatrixGen::yDegrees(-45.0f);
  auto z90 = HMatrixGen::zDegrees(23.0f);

  auto xInv90 = x90.inv();
  auto yInv90 = y90.inv();
  auto zInv90 = z90.inv();

  EXPECT_THAT((xInv90 * x90).data(), equalsMatrix(HMatrixGen::i()));
  EXPECT_THAT((yInv90 * y90).data(), equalsMatrix(HMatrixGen::i()));
  EXPECT_THAT((zInv90 * z90).data(), equalsMatrix(HMatrixGen::i()));
}

TEST_F(HMatrixTest, InverseMultiplicationTest) {
  auto x90 = HMatrixGen::xDegrees(60.0f);
  auto y90 = HMatrixGen::yDegrees(-45.0f);
  auto z90 = HMatrixGen::zDegrees(23.0f);

  auto rot = x90 * y90 * z90;

  EXPECT_THAT((rot.inv() * rot).data(), equalsMatrix(HMatrixGen::i()));
}

TEST_F(HMatrixTest, MultiplicationVecHPoint3Test) {
  Vector<HPoint3> points = {{1.0f, 2.0f, 3.0f}, {2.0f, 5.0f, 7.0f}};

  auto matrix = HMatrix{
    {-1.0f, 2.0f, 3.0f, 4.0f},
    {5.0f, -6.0f, 7.0f, 8.0f},
    {9.0f, -8.0f, 7.0f, 6.0f},
    {5.0f, 4.0f, -3.0f, 1.0f}};

  auto result = matrix * points;
  ASSERT_EQ(2, result.size());

  EXPECT_THAT(result[0].data(), equalsPoint(16.0f / 5.0f, 22.0f / 5.0f, 20.0f / 5.0f));
  EXPECT_THAT(result[1].data(), equalsPoint(33.0f / 10.0f, 37.0f / 10.0f, 33.0f / 10.0f));
}

TEST_F(HMatrixTest, MultiplicationVecHPoint3LongTest) {
  Vector<HPoint3> points{
    {0.54342354f, 0.65875207f, 0.89720247f},
    {0.51389245f, 0.97631843f, 0.48467117f},
    {0.25009358f, 0.71512308f, 0.3240954f},
    {0.99413048f, 0.50010172f, 0.12262631f},
    {0.45707725f, 0.08260504f, 0.16636445f},
    {0.56608858f, 0.70367859f, 0.3257262f},
    {0.04110697f, 0.74535412f, 0.96589189f},
    {0.01032942f, 0.84072808f, 0.30515309f},
    {0.87960067f, 0.85525782f, 0.49250595f},
    {0.53983367f, 0.9451015f, 0.06532676f},
    {0.00458787f, 0.62535925f, 0.96895621f},
    {0.2676625f, 0.34012995f, 0.41140411f},
    {0.96016821f, 0.45234735f, 0.98342685f},
    {0.19080576f, 0.46184871f, 0.45606579f},
    {0.10908995f, 0.39252267f, 0.90102594f},
    {0.84180821f, 0.80859068f, 0.22400568f},
    {0.56711724f, 0.14736189f, 0.714398f}};

  HMatrix matrix{
    {0.74546921, 0.71952996, 0.76345238, 0.71337735},
    {0.78034825, 0.54154092, 0.55100812, 0.16757722},
    {0.49670883, 0.81379888, 0.13691469, 0.86618041},
    {0.5860506, 0.5669385, 0.05191393, 0.3886459}};

  auto result = matrix * points;
  ASSERT_EQ(points.size(), result.size());

  Vector<HPoint3> groundTruth{
    {2.02050135f, 1.27997144f, 1.59251709f},
    {1.70989756f, 1.07558602f, 1.5627448f},
    {1.7356153f, 0.96983261f, 1.68855957f},
    {1.51284871f, 1.01632577f, 1.41438048f},
    {1.74240001f, 0.92791128f, 1.66185595f},
    {1.66368799f, 1.02958608f, 1.55300676f},
    {2.2787721f, 1.28240916f, 1.83569035f},
    {1.75722234f, 0.90068103f, 1.80039062f},
    {1.66867636f, 1.12294864f, 1.46087231f},
    {1.48342509f, 0.91353682f, 1.53701511f},
    {2.39458522f, 1.31091136f, 1.89661333f},
    {1.93725495f, 1.03636756f, 1.75365977f},
    {1.99023781f, 1.35335458f, 1.466303f},
    {1.95438614f, 1.04057893f, 1.78025241f},
    {2.44500192f, 1.3322596f, 1.88832688f},
    {1.54858212f, 1.02496702f, 1.45928842f},
    {2.12393776f, 1.28745165f, 1.62255944f}};

  for (size_t i = 0; i < points.size(); i++) {
    EXPECT_THAT(result[i].data(), equalsPoint(groundTruth[i]));
  }
}

TEST_F(HMatrixTest, MatrixMultiplicationTest) {
  // These were randomly generated in numpy with result found in numpy
  // a = np.random.random((4,4))
  // b = np.random.random((4,4))
  // c = np.dot(a * b)
  HMatrix a = {
    {0.08194541, 0.54210111, 0.21633583, 0.57727269},
    {0.08466542, 0.17222077, 0.6817834, 0.14777765},
    {0.39067386, 0.9393156, 0.66409698, 0.5091621},
    {0.43453027, 0.45362631, 0.02828353, 0.36280876}};

  HMatrix b = {
    {0.90509914, 0.79262353, 0.37816378, 0.49639195},
    {0.19324303, 0.93115695, 0.35838508, 0.24941946},
    {0.75064763, 0.56530796, 0.09001518, 0.1878201},
    {0.34911864, 0.18023588, 0.12398601, 0.68739742}};

  HMatrix cTruth = {
    {0.54285462, 0.79607469, 0.31631698, 0.61333558},
    {0.67328209, 0.63952479, 0.17343197, 0.31461705},
    {1.21137557, 1.65149613, 0.60728319, 0.90293842},
    {0.62884736, 0.84819627, 0.37442567, 0.58354658}};

  auto c = a * b;
  EXPECT_THAT(cTruth.data(), equalsMatrix(c));
  EXPECT_THAT(cTruth.inv().data(), equalsMatrix(c.inv()));
}

TEST_F(HMatrixTest, VectorMultiplicationTest) {
  Vector<HVector3> points = {{1.0f, 2.0f, 3.0f}, {2.0f, 5.0f, 7.0f}};

  auto matrix = HMatrix{
    {-1.0f, 2.0f, 3.0f, 4.0f},
    {5.0f, -6.0f, 7.0f, 8.0f},
    {9.0f, -8.0f, 7.0f, 6.0f},
    {0.0f, 00.0f, 0.0f, 1.0f}};

  auto result = matrix * points;
  ASSERT_EQ(2, result.size());

  EXPECT_THAT(result[0].data(), equalsPoint(12.0f, 14.0f, 14.0f));
  EXPECT_THAT(result[1].data(), equalsPoint(29.0f, 29.0f, 27.0f));

  HVector3 result2 = matrix * points[0];
  EXPECT_THAT(result2.data(), equalsPoint(12.0f, 14.0f, 14.0f));
}

TEST_F(HMatrixTest, TranslationTest) {
  auto point = HPoint3(2.0f, 5.0f, 7.0f);

  auto matrix = HMatrixGen::i().translate(1.0f, -7.0f, -2.0f);

  EXPECT_THAT((matrix * point).data(), equalsPoint(3.0f, -2.0f, 5.0f));
  EXPECT_THAT((matrix.inv() * point).data(), equalsPoint(1.0f, 12.0f, 9.0f));
}

TEST_F(HMatrixTest, RotateTest) {
  auto mat = HMatrixGen::i();

  // Test known rotations.
  EXPECT_THAT(mat.rotateD(90.0f, 0.0f, 0.0f).data(), equalsMatrix(HMatrixGen::xDegrees(90.0f)));
  EXPECT_THAT(mat.rotateD(0.0f, -90.0f, 0.0f).data(), equalsMatrix(HMatrixGen::yDegrees(-90.0f)));
  EXPECT_THAT(mat.rotateD(0.0f, 0.0f, 90.0f).data(), equalsMatrix(HMatrixGen::zDegrees(90.0f)));

  // Test that inversion is correct.
  EXPECT_THAT(
    mat.rotateD(23.0f, -16.0f, 190.0f).data(),
    equalsMatrix(mat.rotateD(-23.0f, 16.0f, -190.0f).inv()));

  // Now start with a non-identity matrix.
  mat = HMatrix{
    {-1.0f, 2.0f, 3.0f, 4.0f},
    {5.0f, -6.0f, 7.0f, 8.0f},
    {9.0f, -8.0f, 7.0f, 6.0f},
    {5.0f, 4.0f, -3.0f, 3.0f}};

  // Test that opposite rotation recovers original position.
  EXPECT_THAT(mat.rotateD(90.0f, 0.0f, 0.0f).rotateD(-90.0f, 0.0f, 0.0f).data(), equalsMatrix(mat));

  // Test that a more complicated opposite rotation recovers original position.
  EXPECT_THAT(
    mat.rotateD(23.0f, -16.0f, 190.0f).rotateD(-23.0f, 16.0f, -190.0f).data(), equalsMatrix(mat));
}

TEST_F(HMatrixTest, ScalarMultiplicationTest) {
  auto matrix = HMatrix{
    {-1.0f, 2.0f, 3.0f, 4.0f},
    {5.0f, -6.0f, 7.0f, 8.0f},
    {9.0f, -8.0f, 7.0f, 6.0f},
    {5.0f, 4.0f, -3.0f, 1.0f}};
  EXPECT_THAT(
    (0.5f * matrix).data(),
    equalsMatrix(
      {-0.5f, 1.0f, 1.5f, 2.0f},
      {2.5f, -3.0f, 3.5f, 4.0f},
      {4.5f, -4.0f, 3.5f, 3.0f},
      {2.5f, 2.0f, -1.5f, 0.5f}));

  EXPECT_THAT(
    (matrix * 0.5f).data(),
    equalsMatrix(
      {-0.5f, 1.0f, 1.5f, 2.0f},
      {2.5f, -3.0f, 3.5f, 4.0f},
      {4.5f, -4.0f, 3.5f, 3.0f},
      {2.5f, 2.0f, -1.5f, 0.5f}));

  // Test that inverses work as well.
  EXPECT_THAT((0.25f * matrix.inv() * matrix * 4.0f).data(), equalsMatrix(HMatrixGen::i()));
  EXPECT_THAT(((0.25f * matrix.inv()) * (matrix * 4.0f)).data(), equalsMatrix(HMatrixGen::i()));
  EXPECT_THAT(((4.0f * matrix).inv() * (matrix * 4.0f)).data(), equalsMatrix(HMatrixGen::i()));
}

TEST_F(HMatrixTest, project2DTest) {
  Vector<HPoint3> points = {{1.0f, 2.0f, 3.0f}, {2.0f, 5.0f, 7.0f}};

  auto matrix = HMatrix{
    {-1.0f, 2.0f, 3.0f, 4.0f},
    {5.0f, -6.0f, 7.0f, 8.0f},
    {9.0f, -8.0f, 7.0f, 6.0f},
    {5.0f, 4.0f, -3.0f, 1.0f}};

  // Project 3D points into 2D.
  Vector<HPoint2> result = project2D(matrix, points);
  ASSERT_EQ(2, result.size());

  EXPECT_THAT(result[0].data(), equalsPoint(16.0f / 20.0f, 22.0f / 20.0f));
  EXPECT_THAT(result[1].data(), equalsPoint(33.0f / 33.0f, 37.0f / 33.0f));

  // Flatten the input to 2D and project to 2D.
  result = project2D(matrix, flatten<2>(points));
  ASSERT_EQ(2, result.size());

  EXPECT_THAT(result[0].data(), equalsPoint(16.0f / 20.0f, 22.0f / 20.0f));
  EXPECT_THAT(result[1].data(), equalsPoint(33.0f / 33.0f, 37.0f / 33.0f));
}

TEST_F(HMatrixTest, noInvertTest) {
  auto matrix = HMatrix{
    {0.0f, -1.0f, -1.0f, 0.0f},
    {1.0f, 0.00f, -1.0f, 0.0f},
    {1.0f, 1.00f, 0.00f, 0.0f},
    {0.0f, 0.00f, 0.00f, 1.0f}};
  try {
    matrix.inv();
    FAIL() << "Error expected; Matrix is not invertable.";
  } catch (RuntimeError e) {
    // Pass.
  }
}

TEST_F(HMatrixTest, determinantTest) {
  auto eye = HMatrixGen::i();
  EXPECT_FLOAT_EQ(1.0f, eye.determinant());
  EXPECT_FLOAT_EQ(1.0f, eye.inv().determinant());

  auto rotation = HMatrixGen::rotationD(10.0f, -20.0f, 30.0f);
  EXPECT_FLOAT_EQ(1.0f, rotation.determinant());
  EXPECT_FLOAT_EQ(1.0f, rotation.inv().determinant());

  auto mat = HMatrix{
    {-1.0f, 2.00f, 3.000f, -4.00f},
    {5.00f, -6.0f, -7.00f, 8.000f},
    {-9.0f, 10.0f, -11.0f, 12.00f},
    {13.0f, -14.0f, 15.0f, -16.0f}};
  EXPECT_FLOAT_EQ(64.0f, mat.determinant());
  EXPECT_FLOAT_EQ(1.0f / 64.0f, mat.inv().determinant());

  auto normMat = HMatrix{
    {-0.92f, -2.12f, -1.58f, 0.870f},
    {0.070f, 0.000f, -0.67f, -0.81f},
    {-0.53f, 1.340f, -0.18f, -0.39f},
    {1.330f, 0.840f, 0.890f, 1.060f}};

  EXPECT_FLOAT_EQ(-4.1393304f, normMat.determinant());
  EXPECT_FLOAT_EQ(-1.0f / 4.1393304f, normMat.inv().determinant());
}

TEST_F(HMatrixTest, intrinsicTest) {
  HPoint3 pt0 = {2.0f, 3.0f, 5.0f};
  HPoint3 pte = {61.0f, 4.0f, 5.0f};  // 61=2*13+5*7; 4=-3*17+5*11

  c8_PixelPinholeCameraModel a{640, 480, 7.0f, 11.0f, 13.0f, 17.0f};
  auto Ka = HMatrixGen::intrinsic(a);
  HPoint3 pta = Ka * pt0;
  HPoint3 pta0 = Ka.inv() * pta;
  EXPECT_THAT(pta.data(), equalsPoint(pte));
  EXPECT_THAT(pta0.data(), equalsPoint(pt0));

  c8_PixelPinholeCameraModel b;
  auto Kb = HMatrixGen::intrinsic(b);
  HPoint3 ptb = Kb * pt0;
  HPoint3 ptb0 = Kb.inv() * ptb;
  EXPECT_THAT(ptb.data(), equalsPoint(pt0));
  EXPECT_THAT(ptb0.data(), equalsPoint(pt0));
}

TEST_F(HMatrixTest, crossTest) {
  HVector3 t(1.0f, -7.0f, -2.0f);
  auto pt = HPoint3(2.0f, 5.0f, 7.0f);

  // Model an pure translation camera motion.
  auto imat = HMatrixGen::i();
  auto tmat = HMatrixGen::translation(t.x(), t.y(), t.z());

  // Compute the cross matrix, which is the essential matrix when there is no rotation.
  auto matrix = HMatrixGen::cross(t.x(), t.y(), t.z());

  // Cross matrices are not invertable.
  try {
    matrix.inv();
    FAIL() << "Error expected; Matrix is not invertable.";
  } catch (RuntimeError e) {
    // Pass.
  }

  // Cross matrices have 0 determinant.
  EXPECT_FLOAT_EQ(0.0f, matrix.determinant());

  // Verify that x'Ex = 0
  auto p1 = (imat.inv() * pt).flatten().extrude();
  auto p2 = (tmat.inv() * pt).flatten().extrude();

  auto p3 = matrix * p1;                                            // Ex
  float dot = p3.x() * p2.x() + p3.y() * p2.y() + p3.z() * p2.z();  // x'Ex
  EXPECT_FLOAT_EQ(0.0f, dot);

  // Run the epipolar verification in the opposite direction.
  auto p4 = matrix.t() * p2;                                         // x'E
  float dot2 = p4.x() * p1.x() + p4.y() * p1.y() + p4.z() * p1.z();  // x'Ex
  EXPECT_FLOAT_EQ(0.0f, dot2);

  // Cross matrices can also be used to perform vector cross products using matrix-vector
  // multiplication.
  HVector3 v(8.0f, 9.0f, -12.0f);
  HVector3 cv1 = t.cross(v);
  HVector3 cv2 = matrix * v;
  EXPECT_THAT(cv1.data(), equalsHVector(cv2));
}

TEST_F(HMatrixTest, scaleTest) {
  HPoint3 pt0 = {2.0f, 3.0f, 5.0f};
  HPoint3 pex = {8.0f, 3.0f, 5.0f};
  HPoint3 pey = {2.0f, 12.0f, 5.0f};
  HPoint3 pez = {2.0f, 3.0f, 20.0f};

  HPoint3 px = HMatrixGen::scaleX(4) * pt0;
  HPoint3 px0 = HMatrixGen::scaleX(4).inv() * px;
  EXPECT_THAT(px.data(), equalsPoint(pex));
  EXPECT_THAT(px0.data(), equalsPoint(pt0));

  HPoint3 py = HMatrixGen::scaleY(4) * pt0;
  HPoint3 py0 = HMatrixGen::scaleY(4).inv() * py;
  EXPECT_THAT(py.data(), equalsPoint(pey));
  EXPECT_THAT(py0.data(), equalsPoint(pt0));

  HPoint3 pz = HMatrixGen::scaleZ(4) * pt0;
  HPoint3 pz0 = HMatrixGen::scaleZ(4).inv() * pz;
  EXPECT_THAT(pz.data(), equalsPoint(pez));
  EXPECT_THAT(pz0.data(), equalsPoint(pt0));
}

TEST_F(HMatrixTest, copyTest) {
  HMatrix m = HMatrixGen::scale(7.0f, 8.0f, 9.0f) * HMatrixGen::translation(-4.0f, 5.0f, 6.0f)
    * HMatrixGen::rotationD(1.0f, -2.0f, 3.0f);

  HMatrix c{m.data().data(), m.inverseData().data()};

  EXPECT_THAT(c.data(), equalsMatrix(m));
  EXPECT_THAT(c.inv().data(), equalsMatrix(m.inv()));
}

TEST_F(HMatrixTest, TestUnitRotationAlignment) {
  // test 1 - typical case
  HVector3 a = {1.0f, 0.0f, 0.0f};
  HVector3 b = {0.0f, 0.0f, 1.0f};
  HMatrix r = HMatrixGen::unitRotationAlignment(a, b);
  EXPECT_THAT((r * a).data(), equalsHVector(b));
  EXPECT_THAT((r.inv() * b).data(), equalsHVector(a));

  a = {1.0f, 0.0f, 0.0f};
  b = {0.9f, 0.1f, 0.0f};
  r = HMatrixGen::unitRotationAlignment(a, b);
  EXPECT_THAT((r * a).data(), equalsHVector(b));
  EXPECT_THAT((r.inv() * b).data(), equalsHVector(a));

  // test 2 - equal
  a = {1.0f, 0.0f, 0.0f};
  b = {1.0f, 0.0f, 0.0f};
  r = HMatrixGen::unitRotationAlignment(a, b);
  EXPECT_THAT((r * a).data(), equalsHVector(b));
  EXPECT_THAT((r.inv() * b).data(), equalsHVector(a));

  a = {0.0f, 1.0f, 0.0f};
  b = {0.0f, 1.0f, 0.0f};
  r = HMatrixGen::unitRotationAlignment(a, b);
  EXPECT_THAT((r * a).data(), equalsHVector(b));
  EXPECT_THAT((r.inv() * b).data(), equalsHVector(a));

  // test 3 - opposite directions
  a = {1.0f, 0.0f, 0.0f};
  b = {-1.0f, 0.0f, 0.0f};
  r = HMatrixGen::unitRotationAlignment(a, b);
  EXPECT_THAT((r * a).data(), equalsHVector(b));
  EXPECT_THAT((r.inv() * b).data(), equalsHVector(a));

  a = {0.5773502f, -0.5773502f, 0.5773502f};
  b = {-0.5773502f, 0.5773502f, -0.5773502f};
  r = HMatrixGen::unitRotationAlignment(a, b);
  EXPECT_THAT((r * a).data(), equalsHVector(b));
  EXPECT_THAT((r.inv() * b).data(), equalsHVector(a));
}

TEST_F(HMatrixTest, TestFromRotationAndTranslation) {
  HMatrix rotation{
    {2.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 2.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 1.0f}};
  HVector3 translation{4.0f, -4.0f, 8.0f};
  HMatrix matrix = HMatrixGen::fromRotationAndTranslation(rotation, translation);
  HMatrix matrixGroundTruth{
    {2.0f, 0.0f, 0.0f, 4.0f},
    {0.0f, 2.0f, 0.0f, -4.0f},
    {0.0f, 0.0f, 1.0f, 8.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
    {0.5f, 0.0f, 0.0f, -2.0f},
    {0.0f, 0.5f, 0.0f, 2.0f},
    {0.0f, 0.0f, 1.0f, -8.0f},
    {0.0f, 0.0f, 0.0f, 1.0f}};
  EXPECT_THAT(matrix.data(), equalsMatrix(matrixGroundTruth));
}

}  // namespace c8
