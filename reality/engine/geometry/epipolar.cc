// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "epipolar.h",
  };
  deps = {
    "//c8:hmatrix",
    "//c8:map",
    "//c8:vector",
    "//c8/geometry:egomotion",
    "//c8/geometry:homography",
    "//c8/geometry:vectors",
    "//reality/engine/features:feature-scales",
  };
}
cc_end(0x0668a651);

#include "c8/c8-log.h"
#include "c8/geometry/egomotion.h"
#include "c8/geometry/homography.h"
#include "c8/geometry/vectors.h"
#include "reality/engine/features/feature-scales.h"
#include "reality/engine/geometry/epipolar.h"

namespace c8 {

namespace {
constexpr float sign(float x) { return x < 0 ? -1.0f : 1.0f; }
}  // namespace

HMatrix essentialMatrixForCameras(const HMatrix &camPos1, const HMatrix &camPos2) {
  auto cam = egomotion(camPos1, camPos2);
  auto proj = cam.inv();
  auto rd = rotationMat(proj);
  auto td = translation(proj);
  auto tx = HMatrixGen::cross(td.x(), td.y(), td.z());
  return tx * rd;
}

HVector3 groundPointTriangulationPrework(const HMatrix &extrinsic) {
  // Get the equation for the plane at y=-1 in the extrinsic camera view. The normal equation is
  // parameterized by the normal n normalized for a point p on the plane. Here (rx, ry) is the
  // position of the point in ray space.
  //
  // The plane equation is
  //   nx(x - px) + ny(y - py) + nz(z - pz) = 0
  //
  // We will be solving for z along a ray from the origin, giving
  //   nx(z * rx - px) + ny(z * ry - py) + nz(z - pz) = 0
  //
  // Collecting terms and solving for z gives
  //   z = n.dot(p) / n.dot({rx, ry, 1})
  //
  // We can scale n arbitrarily and have this ratio hold, so if we scale n to n' as
  //   n' = n / n.dot(p) then n.dot(p) = 1
  //
  // and the solution becomes
  //   z = 1 / n'.dot({rx, ry, 1})
  auto groundPlaneNormal = asVector(rotationMat(extrinsic).inv() * HPoint3{0.0f, 1.0f, 0.0f});
  auto p = asVector(extrinsic.inv() * HPoint3{0.0f, -1.0f, 0.0f});

  // If scale is close to zero here, it's because the camera is on the plane of the ground.
  // This leads to NaN in scaledNormal below. Setting this to a value close to zero
  // keeps it at a value extremely close to the camera plane and code below will handle it
  // correctly.
  auto scale = groundPlaneNormal.dot(p);
  if (std::abs(scale) < 1e-10f) {
    scale = 1e-10f * sign(scale);
  }

  // Return the scaled ground plane normal.
  return (1.0f / scale) * groundPlaneNormal;
}

EpipolarDepthImagePrework epipolarDepthImagePrework(
  const HMatrix &wordsCam, const HMatrix &dictionaryCam) {
  auto projectToDictionary = egomotion(dictionaryCam, wordsCam);

  // auto projectToDictionary = wordsExtrinsicInDictionarySpace.inv();
  auto rotateToDictionary = rotationMat(projectToDictionary);

  // Compute the dot product of the camera forward vectors to determine whether the cameras may
  // share some field of view.
  auto viewOverlap =
    asVector(rotateToDictionary * HPoint3{0.0f, 0.0f, 1.0f}).dot(HVector3{0.0f, 0.0f, 1.0f});

  // Get the equation for the plane at nearDist from the dictionary camera in the words camera
  // view. The normal equation is parameterized by the normal n normalized for a point p on the
  // plane.
  //
  // The plane equation is
  //   nx(x - px) + ny(y - py) + nz(z - pz) = 0
  //
  // We will be solving for z along a ray from the origin, giving
  //   nx(z * rx - px) + ny(z * ry - py) + nz(z - pz) = 0
  //
  // Collecting terms and solving for z gives
  //   z = n.dot(p) / n.dot({rx, ry, 1})
  //
  // We can scale n arbitrarily and have this ratio hold, so if we scale n to n' as
  //   n' = n / n.dot(p) then n.dot(p) = 1
  //
  // and the solution becomes
  //   z = 1 / n'.dot({rx, ry, 1})
  float nearDist = 0.4f;
  auto dictionaryClipPlaneNormal = asVector(rotateToDictionary.inv() * HPoint3{0.0f, 0.0f, 1.0f});
  auto p = asVector(projectToDictionary.inv() * HPoint3{0.0f, 0.0f, nearDist});

  // If scale is close to zero here, it's because nearDist in the dictionary is on the plane of the
  // words camera. This leads to NaN in scaledNormal below. Setting this to a value close to zero
  // keeps it at a value extremely close to the camera plane and code below will handle it
  // correctly.
  auto scale = dictionaryClipPlaneNormal.dot(p);
  if (std::abs(scale) < 1e-10f) {
    scale = 1e-10f * sign(scale);
  }

  auto scaledNormal = (1.0f / scale) * dictionaryClipPlaneNormal;

  return {
    nearDist,
    viewOverlap,
    projectToDictionary,
    rotateToDictionary,
    scaledNormal,                                  // dictionaryClipPlaneNormal
    isPureCameraRotation(wordsCam, dictionaryCam)  // isPureRotation
  };
}

EpipolarDepthPointPrework epipolarDepthPointPrework(
  const EpipolarDepthImagePrework &w, HPoint2 wordsRay) {
  // If there is insufficient view overlap between the cameras, return an error point.
  // The current threshold of 0.5 means that the facing directions can be up to 60 degrees apart,
  // which is very generous.
  HVector2 errPt{-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max()};
  std::pair<HVector2, HVector2> lineSegment{errPt, errPt};
  if (w.viewOverlap >= 0.5f) {
    // The far ray (point at infinity) is the point rotated into the dictionary reference frame
    // ignoring translation.
    auto farRay = asVector((w.rotateToDictionary * wordsRay.extrude()).flatten());

    // Extract out parameters of the plane equation for convenience and to avoid calling hpoint
    // dividing accessors multiple times.
    auto r = HVector3{wordsRay.x(), wordsRay.y(), 1.0f};  // ray from camera

    // Solve for the z component of the ray intersection with the plane.
    float dictionaryZ = 1.0f / w.dictionaryClipPlaneNormal.dot(r);

    // Pick the near point from the camera that is in front.
    float nearZ = std::max(dictionaryZ, w.nearDist);

    // Construct the near point and project it to the dictionary view.
    auto nearRay =
      asVector((w.projectToDictionary * HPoint3{r.x() * nearZ, r.y() * nearZ, nearZ}).flatten());

    // Return far and near.
    lineSegment = {farRay, nearRay};
  }

  auto segmentVector = lineSegment.second - lineSegment.first;
  auto segmentSqLen = segmentVector.dot(segmentVector);
  bool isPoint = segmentSqLen < 1e-10f;
  float norm = isPoint ? 1.0f : 1.0f / segmentSqLen;
  auto normSegmentVector = norm * segmentVector;

  const auto &m = w.projectToDictionary.data();
  auto x = wordsRay.x();
  auto y = wordsRay.y();
  HVector3 rotatedWordsPt = {
    m[0] * x + m[4] * y + m[8],
    m[1] * x + m[5] * y + m[9],
    m[2] * x + m[6] * y + m[10],
  };
  auto t0 = m[12];
  auto t1 = m[13];
  auto t2 = m[14];
  auto r0 = rotatedWordsPt.x();
  auto r1 = rotatedWordsPt.y();
  auto r2 = rotatedWordsPt.z();

  // @see depthOfEpipolarPoint
  HVector2 dzScale = {
    t2 * r0 - t0 * r2,
    t2 * r1 - t1 * r2,
  };

  return {
    lineSegment,
    segmentVector,
    isPoint,
    normSegmentVector,
    rotatedWordsPt,
    dzScale,
  };
}

HVector2 closestLineSegmentPoint(const EpipolarDepthPointPrework &w, HVector2 pt) {
  // If the line segment has zero length, just return the first point.
  if (w.isPoint) {
    return w.lineSegment.first;
  }
  auto alpha = (pt - w.lineSegment.first).dot(w.normSegmentVector);
  if (alpha <= 0) {
    return w.lineSegment.first;
  }
  if (alpha >= 1) {
    return w.lineSegment.second;
  }
  return w.lineSegment.first + alpha * w.segmentVector;
}

// Compute the depth of a point in a words frame from the position along the epipolar line in a
// dictionary frame.
//
// Given a matched pair of rays, [rx, ry] (words frame) and [r'x, r'y] (dictionary frame), and a
// matrix [R|t] that projects [rx, ry] to [r'x, r'y], we have:
//
//   |r'x| = |((R00rx + R01ry + R02)z + t0) / ((R20rx + R21ry + R22)z + t2)|
//   |r'y| = |((R10rx + R11ry + R12)z + t1) / ((R20rx + R21ry + R22)z + t2)|
//
// Defining
//       | R00rx + R01ry + R02 |   | R0 |
//   R = | R10rx + R11ry + R12 | = | R1 |
//       | R20rx + R21ry + R22 |   | R2 |
//
// The above simplifies to
//   |r'x| = |(R0z + t0) / (R2z + t2)|
//   |r'y| = |(R1z + t1) / (R2z + t2)|
//
// Solving for z gives two equivalent solutions:
//   z = (r'x t2 - t0) / (R0 - r'x R2)
//     or
//   z = (r'y t2 - t1) / (R1 - r'y R2)
//
// To compute certainty, we compute the Jacobian [d r'x / dz, d r'y / dz], i.e. the change in depth
// induced by sliding along the epipolar line segment.
//
// Differentiating the above gives:
//   | dx/dz | = | (t2 R0 - t0 R2) / (R0 - r'x R2)^2 |
//   | dy/dz | = | (t2 R1 - t1 R2) / (R1 - r'y R2)^2 |
//
// Since a high derivative means the estimate of z is unstable, it has an inverse relationship with
// certainty.
EpipolarDepthResult depthOfEpipolarPoint(
  const EpipolarDepthImagePrework &iw,
  const EpipolarDepthPointPrework &pw,
  HVector2 dictionaryPt,
  uint8_t scale) {
  if (iw.isPureRotation) {
    return {
      1.0f,  // depth
      0.0f,  // certainty
      EpipolarDepthResult::Status::PURE_ROTATION,
    };
  }
  const auto &m = iw.projectToDictionary.data();
  auto t0 = m[12];
  auto t1 = m[13];
  auto t2 = m[14];
  auto r0 = pw.rotatedWordsPoint.x();
  auto r1 = pw.rotatedWordsPoint.y();
  auto r2 = pw.rotatedWordsPoint.z();

  auto pt = closestLineSegmentPoint(pw, dictionaryPt);
  auto xDen = r0 - pt.x() * r2;
  auto yDen = r1 - pt.y() * r2;

  auto xDen2 = xDen * xDen;
  auto yDen2 = yDen * yDen;

  // More prominent x movement, compute depth from x motion.
  if (xDen2 > yDen2) {
    if (xDen2 < 1e-10f) {
      return {
        1.0f,  // depth
        0.0f,  // certainty
        EpipolarDepthResult::Status::FOCUS_OF_EXPANSION,
      };
    }
    auto depth = (pt.x() * t2 - t0) / xDen;

    // Sometimes this can return -inf, so the abs here corrects for precision.
    if (std::abs(depth) > 1e10f) {
      return {
        1.0f,  // depth
        0.0f,  // certainty
        EpipolarDepthResult::Status::TOO_FAR_AWAY,
      };
    }

    // Jacobian vector: [d r'x/dz, d r'y/dz]
    HVector2 dz = {
      pw.dzScale.x() / xDen2,
      yDen2 > 1e-10f ? pw.dzScale.y() / yDen2 : 0.0f,
    };
    // Correct jacobian for differential pixel/ray spread at different scales.
    auto certainty = INV_PATCH_SCALE_SQ[scale] / dz.dot(dz);
    // If the certainty is too high, it's probably due to numeric instability and something went
    // wrong.
    if (certainty > 1e10f || certainty < 1e-10f) {
      return {
        1.0f,  // depth
        0.0f,  // certainty
        EpipolarDepthResult::Status::TOO_UNCERTAIN,
      };
    }
    return {
      depth,
      certainty,
      EpipolarDepthResult::Status::OK,
    };
  }

  // More prominent y movement, compute depth from y motion.
  if (yDen2 < 1e-10f) {
    return {
      1.0f,  // depth
      0.0f,  // certainty
      EpipolarDepthResult::Status::FOCUS_OF_EXPANSION,
    };
  }
  auto depth = (pt.y() * t2 - t1) / yDen;

  // Sometimes this can return -inf, so the abs here corrects for precision.
  if (std::abs(depth) > 1e10f) {
    return {
      1.0f,  // depth
      0.0f,  // certainty
      EpipolarDepthResult::Status::TOO_FAR_AWAY,
    };
  }

  // Jacobian vector: [d r'x/dz, d r'y/dz]
  HVector2 dz = {
    xDen2 > 1e-10f ? pw.dzScale.x() / xDen2 : 0.0f,
    pw.dzScale.y() / yDen2,
  };
  // Correct jacobian for differential pixel/ray spread at different scales.
  auto certainty = INV_PATCH_SCALE_SQ[scale] / dz.dot(dz);

  // If the certainty is too high, it's probably due to numeric instability and something went
  // wrong.
  if (certainty > 1e10f || certainty < 1e-10f) {
    return {
      1.0f,  // depth
      0.0f,  // certainty
      EpipolarDepthResult::Status::TOO_UNCERTAIN,
    };
  }
  return {
    depth,
    certainty,
    EpipolarDepthResult::Status::OK,
  };
}

}  // namespace c8
