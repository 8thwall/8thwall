// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":egomotion",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0x465d2250);

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "c8/geometry/egomotion.h"

using testing::Eq;
using testing::Pointwise;

namespace c8 {

MATCHER_P(AreWithin, eps, "") { return fabs(testing::get<0>(arg) - testing::get<1>(arg)) < eps; }

decltype(auto) equalsMatrix(const HMatrix &matrix) {
  return Pointwise(AreWithin(0.0001), matrix.data());
}

class EgomotionTest : public ::testing::Test {};

TEST_F(EgomotionTest, TestRotation) {
  auto cam = HMatrixGen::translation(7.0, 5.0, 16.0) * HMatrixGen::zDegrees(30.0)
    * HMatrixGen::yDegrees(180.0);

  auto expected = HMatrixGen::zDegrees(30.0) * HMatrixGen::yDegrees(180.0);

  EXPECT_THAT(rotationMat(cam).data(), equalsMatrix(expected));
}

TEST_F(EgomotionTest, TestRotationQ) {
  auto cam = HMatrixGen::translation(7.0, 5.0, 16.0) * HMatrixGen::zDegrees(30.0)
    * HMatrixGen::yDegrees(180.0);

  auto expected = HMatrixGen::zDegrees(30.0) * HMatrixGen::yDegrees(180.0);
  auto eq = Quaternion::fromHMatrix(expected);

  auto q = rotation(cam);

  EXPECT_NEAR(q.w(), eq.w(), 0.0001);
  EXPECT_NEAR(q.x(), eq.x(), 0.0001);
  EXPECT_NEAR(q.y(), eq.y(), 0.0001);
  EXPECT_NEAR(q.z(), eq.z(), 0.0001);
}

TEST_F(EgomotionTest, TestTranslation) {
  auto cam = HMatrixGen::translation(7.0, 5.0, 16.0) * HMatrixGen::zDegrees(30.0)
    * HMatrixGen::yDegrees(180.0);

  auto expected = HMatrixGen::translation(7.0, 5.0, 16.0);

  EXPECT_THAT(translationMat(cam).data(), equalsMatrix(expected));
}

TEST_F(EgomotionTest, TestTranslationV) {
  auto cam = HMatrixGen::translation(7.0, 5.0, 16.0) * HMatrixGen::zDegrees(30.0)
    * HMatrixGen::yDegrees(180.0);

  auto ev = HVector3(7.0f, 5.0f, 16.0f);

  auto v = translation(cam);

  EXPECT_NEAR(v.x(), ev.x(), 0.0001);
  EXPECT_NEAR(v.y(), ev.y(), 0.0001);
  EXPECT_NEAR(v.z(), ev.z(), 0.0001);
}

TEST_F(EgomotionTest, TestTRSDecomp) {
  auto tv = HVector3{7.0f, 5.0f, 16.0f};
  auto t = HMatrixGen::translation(tv.x(), tv.y(), tv.z());
  auto rq = Quaternion::fromEulerAngleDegrees(0.0f, 180.0f, 30.0f);
  auto r = rq.toRotationMat();
  auto sv = HVector3{2.0f, 3.0f, 5.0f};
  auto s = HMatrixGen::scale(sv.x(), sv.y(), sv.z());

  auto cam = t * r * s;

  EXPECT_THAT(trsScaleMat(cam).data(), equalsMatrix(s));
  EXPECT_THAT(trsRotationMat(cam).data(), equalsMatrix(r));
  EXPECT_THAT(trsTranslationMat(cam).data(), equalsMatrix(t));

  auto trsS = trsScale(cam);
  EXPECT_NEAR(trsS.x(), sv.x(), 0.0001);
  EXPECT_NEAR(trsS.y(), sv.y(), 0.0001);
  EXPECT_NEAR(trsS.z(), sv.z(), 0.0001);

  auto trsR = trsRotation(cam);
  EXPECT_NEAR(trsR.w(), rq.w(), 0.0001);
  EXPECT_NEAR(trsR.x(), rq.x(), 0.0001);
  EXPECT_NEAR(trsR.y(), rq.y(), 0.0001);
  EXPECT_NEAR(trsR.z(), rq.z(), 0.0001);

  auto trsT = trsTranslation(cam);
  EXPECT_NEAR(trsT.x(), tv.x(), 0.0001);
  EXPECT_NEAR(trsT.y(), tv.y(), 0.0001);
  EXPECT_NEAR(trsT.z(), tv.z(), 0.0001);
}

TEST_F(EgomotionTest, TestTRSDecompNegativeSign) {
  auto tv = HVector3{7.0f, 5.0f, 16.0f};
  auto t = HMatrixGen::translation(tv.x(), tv.y(), tv.z());
  auto rq = Quaternion::fromEulerAngleDegrees(0.0f, 180.0f, 30.0f);
  auto r = rq.toRotationMat();
  auto sv = HVector3{2.0f, -3.0f, 5.0f};
  auto s = HMatrixGen::scale(sv.x(), sv.y(), sv.z());

  auto cam = t * r * s;

  // We can't detect which sign was flipped, so the return value always has x flipped. In this case
  // to compensate, the rotation also needs to be rotated 180 degrees about z.
  auto esv = HVector3{-2.0f, 3.0f, 5.0f};
  auto es = HMatrixGen::scale(esv.x(), esv.y(), esv.z());
  auto erq = rq.times(Quaternion::zDegrees(180.0f));
  auto er = erq.toRotationMat();

  EXPECT_THAT(trsScaleMat(cam).data(), equalsMatrix(es));
  EXPECT_THAT(trsRotationMat(cam).data(), equalsMatrix(er));
  EXPECT_THAT(trsTranslationMat(cam).data(), equalsMatrix(t));

  auto trsS = trsScale(cam);
  EXPECT_NEAR(trsS.x(), esv.x(), 0.0001);
  EXPECT_NEAR(trsS.y(), esv.y(), 0.0001);
  EXPECT_NEAR(trsS.z(), esv.z(), 0.0001);

  auto trsR = trsRotation(cam);
  EXPECT_NEAR(trsR.w(), erq.w(), 0.0001);
  EXPECT_NEAR(trsR.x(), erq.x(), 0.0001);
  EXPECT_NEAR(trsR.y(), erq.y(), 0.0001);
  EXPECT_NEAR(trsR.z(), erq.z(), 0.0001);

  auto trsT = trsTranslation(cam);
  EXPECT_NEAR(trsT.x(), tv.x(), 0.0001);
  EXPECT_NEAR(trsT.y(), tv.y(), 0.0001);
  EXPECT_NEAR(trsT.z(), tv.z(), 0.0001);
}

TEST_F(EgomotionTest, TestCameraMotion) {
  auto cam = HMatrixGen::translation(7.0, 5.0, 16.0) * HMatrixGen::zDegrees(30.0)
    * HMatrixGen::yDegrees(180.0);

  auto t = translationMat(cam);
  auto r = rotationMat(cam);

  EXPECT_THAT(cameraMotion(t, r).data(), equalsMatrix(cam));
}

TEST_F(EgomotionTest, TestCameraMotionQV) {
  auto cam = HMatrixGen::translation(7.0, 5.0, 16.0) * HMatrixGen::zDegrees(30.0)
    * HMatrixGen::yDegrees(180.0);

  auto t = translation(cam);
  auto r = rotation(cam);

  EXPECT_THAT(cameraMotion(t, r).data(), equalsMatrix(cam));
}

TEST_F(EgomotionTest, TestCameraMotionPoint) {
  auto cam = HMatrixGen::translation(7.0, 5.0, 16.0) * HMatrixGen::zDegrees(30.0)
    * HMatrixGen::yDegrees(180.0);

  auto t = HPoint3(7.0f, 5.0f, 16.0f);
  auto r = rotation(cam);

  EXPECT_THAT(cameraMotion(t, r).data(), equalsMatrix(cam));
}

TEST_F(EgomotionTest, TestEgomotion) {
  auto cam1 = HMatrixGen::translation(7.0, 5.0, 16.0) * HMatrixGen::zDegrees(0.0)
    * HMatrixGen::yDegrees(180.0);
  auto cam2 = HMatrixGen::translation(5.0, 6.0, 19.0) * HMatrixGen::zDegrees(-30.0)
    * HMatrixGen::yDegrees(180.0);

  auto expectedDelta = HMatrixGen::translation(2, 1, -3) * HMatrixGen::zDegrees(30.0);

  EXPECT_THAT(egomotion(cam1, cam2).data(), equalsMatrix(expectedDelta));
}

TEST_F(EgomotionTest, TestEgomotionQ) {
  auto cam1 = Quaternion::fromHMatrix(HMatrixGen::yDegrees(180.0));
  auto cam2 = Quaternion::fromHMatrix(HMatrixGen::zDegrees(-30.0) * HMatrixGen::yDegrees(180.0));

  auto expectedDelta = Quaternion::fromHMatrix(HMatrixGen::zDegrees(30.0));

  auto delta = egomotion(cam1, cam2);

  EXPECT_NEAR(delta.w(), expectedDelta.w(), 0.0001);
  EXPECT_NEAR(delta.x(), expectedDelta.x(), 0.0001);
  EXPECT_NEAR(delta.y(), expectedDelta.y(), 0.0001);
  EXPECT_NEAR(delta.z(), expectedDelta.z(), 0.0001);
}

TEST_F(EgomotionTest, TestUpdateWorldPosition) {
  auto cam1 = HMatrixGen::translation(7.0, 5.0, 16.0) * HMatrixGen::zDegrees(0.0)
    * HMatrixGen::yDegrees(180.0);
  auto cam2 = HMatrixGen::translation(5.0, 6.0, 19.0) * HMatrixGen::zDegrees(-30.0)
    * HMatrixGen::yDegrees(180.0);

  auto egodelta = HMatrixGen::translation(2, 1, -3) * HMatrixGen::zDegrees(30.0);

  EXPECT_THAT(updateWorldPosition(cam1, egodelta).data(), equalsMatrix(cam2));
}

TEST_F(EgomotionTest, TestUpdateWorldPositionQ) {
  auto cam1 = Quaternion::fromHMatrix(HMatrixGen::yDegrees(180.0));
  auto cam2 = Quaternion::fromHMatrix(HMatrixGen::zDegrees(-30.0) * HMatrixGen::yDegrees(180.0));

  auto egodelta = Quaternion::fromHMatrix(HMatrixGen::zDegrees(30.0));

  auto c2 = updateWorldPosition(cam1, egodelta);

  EXPECT_NEAR(c2.w(), cam2.w(), 0.0001);
  EXPECT_NEAR(c2.x(), cam2.x(), 0.0001);
  EXPECT_NEAR(c2.y(), cam2.y(), 0.0001);
  EXPECT_NEAR(c2.z(), cam2.z(), 0.0001);
}

TEST_F(EgomotionTest, TestUpdateWorldDelta) {
  auto cam1 = HMatrixGen::translation(7.0, 5.0, 16.0) * HMatrixGen::zDegrees(0.0)
    * HMatrixGen::yDegrees(180.0);

  auto cam2 = HMatrixGen::translation(5.0, 6.0, 19.0) * HMatrixGen::zDegrees(-30.0)
    * HMatrixGen::yDegrees(180.0);

  auto egodelta = HMatrixGen::translation(2, 1, -3) * HMatrixGen::zDegrees(30.0);

  EXPECT_THAT((worldDelta(cam1, egodelta) * cam1).data(), equalsMatrix(cam2));
}

TEST_F(EgomotionTest, TestUpdateWorldDeltaQ) {
  auto cam1 = Quaternion::fromHMatrix(HMatrixGen::yDegrees(180.0));
  auto cam2 = Quaternion::fromHMatrix(HMatrixGen::zDegrees(-30.0) * HMatrixGen::yDegrees(180.0));

  auto egodelta = Quaternion::fromHMatrix(HMatrixGen::zDegrees(30.0));

  auto c2 = worldDelta(cam1, egodelta).times(cam1);

  EXPECT_NEAR(c2.w(), cam2.w(), 0.0001);
  EXPECT_NEAR(c2.x(), cam2.x(), 0.0001);
  EXPECT_NEAR(c2.y(), cam2.y(), 0.0001);
  EXPECT_NEAR(c2.z(), cam2.z(), 0.0001);
}

TEST_F(EgomotionTest, TestEqualMatrices) {
  auto cam = HMatrixGen::translation(7.0, 5.0, 16.0) * HMatrixGen::zDegrees(17.0)
    * HMatrixGen::yDegrees(-32.0);

  EXPECT_TRUE(equalMatrices(cam, cam));
  EXPECT_FALSE(equalMatrices(cam, HMatrixGen::i()));
  EXPECT_FALSE(equalMatrices(cam, cam.inv()));
  EXPECT_TRUE(isIdentity(cam * cam.inv()));
}

}  // namespace c8
