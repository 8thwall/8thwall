// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Scott Pollack (scott@8thwall.com)
//

#pragma once

// for M_PI symbol
#ifdef _WIN32
#define _USE_MATH_DEFINES
#endif
#include <math.h>

#include <cmath>

#include "c8/c8-log.h"
#include "c8/geometry/parameterized-geometry.h"
#include "c8/geometry/pixel-pinhole-camera-model.h"
#include "c8/hpoint.h"
#include "ceres/ceres.h"
#include "ceres/rotation.h"
#include "reality/engine/geometry/observed-point.h"

// Ceres is leaking its logging https://github.com/ceres-solver/ceres-solver/issues/323
// We undef these to avoid

namespace c8 {

class ReprojectionResidual {
public:
  ReprojectionResidual(ObservedPoint pt);
  template <typename T>
  inline T huber(const T x) const;
  template <typename T>
  bool operator()(const T *const camera, const T *const point, T *residuals) const;

  float u_;
  float v_;
  float weight_;
};

class SnavelyReprojectionResidual {
public:
  // Scale the error value allow us to optimize with a smaller parameter tolerance (~1E-8)
  // without this, we need to optimize at the tolerance of 1E-16 to get a good fit
  // Since 500 is around the focal length in pixel, this puts the parameter tolerance
  // around the tolerance if we were to optimize in pixel space. Experiment has shown that
  // optimizing in pixel space range results in better fit for synthetic data.
  SnavelyReprojectionResidual(ObservedPoint pt, float scale = 500.0f)
      : scale_(scale), u_(pt.position.x() * scale), v_(pt.position.y() * scale) {}

  template <typename T>
  bool operator()(const T *const camera, const T *const point, T *residuals) const;
  float scale_;
  float u_;
  float v_;
};

class CurvyTargetReprojectionResidual {
public:
  // We scale the error using the residualScale value to allow us to optimize with a smaller
  // parameter tolerance (~1E-8).  Experiments have shown that 1e6 provides the smallest residual.
  CurvyTargetReprojectionResidual(
    HPoint2 targetRay,
    HPoint2 liftedSearchRay,
    CurvyImageGeometry geom,
    const c8_PixelPinholeCameraModel &intrinsics,
    float residualScale);
  template <typename T>
  bool operator()(const T *const camera, T *residuals) const;
  static constexpr float i2pi_ = 1.0f / (2.0f * M_PI);
  float curvyRadiusSquared_;
  float curvyRadius_;
  float intrinsicsInvShiftX_;
  float intrinsicsInvShiftY_;
  float residualScale_;
  float scaleX_;
  float scaledXi2pi_;
  float scaleY_;
  float searchRayX_;
  float searchRayY_;
  float shift_;
  float shiftedScaleX_;
  float targetRayX_;
  float targetRayY_;
};

class CameraMovement {
public:
  CameraMovement(double *c, double weight = 0.07, double rotationWeight = 10.0)
      : pw_(weight), rw_(weight * rotationWeight) {
    for (int i = 0; i < BundleDesc::N_CAMERA_PARAMS; ++i) {
      c_[i] = c[i];
    }
  }
  template <typename T>
  bool operator()(const T *const camera, T *residuals) const;

  double c_[BundleDesc::N_CAMERA_PARAMS];
  double pw_;
  double rw_;
};

class PositionTarget {
public:
  PositionTarget(std::array<double, 3> v) : v_(v) {}
  template <typename T>
  bool operator()(const T *const camera, T *residuals) const;

  std::array<double, 3> v_;
};

class RotationTarget {
public:
  RotationTarget(std::array<double, 3> v) : v_(v) {}
  template <typename T>
  bool operator()(const T *const camera, T *residuals) const;

  std::array<double, 3> v_;
};

// Drive rotation or translation to zero.
class ZeroTarget {
public:
  // Index offset into the residual to start checking for zeros.  Use '0' for rotation and '3' for
  // translation.
  ZeroTarget(int offset) : offset_(offset) {}
  template <typename T>
  bool operator()(const T *const camera, T *residuals) const;

  int offset_;
};

// Penalizes high-frequency translational noise in camera movement.
class PoseEstimationTranslationStability {
public:
  // c1 and c2 are the translations of the past two camera extrinsics in world space.
  // Order is c1 -> c2 -> current extrinsic.
  PoseEstimationTranslationStability(
    std::array<double, 3> c1InWorld, std::array<double, 3> c2InWorld)
      : c2InWorld_(c2InWorld) {
    v1_ = {c2InWorld[0] - c1InWorld[0], c2InWorld[1] - c1InWorld[1], c2InWorld[2] - c1InWorld[2]};
  }

  // The residual is the weighted camera's acceleration.
  template <typename T>
  bool operator()(const T *const camera, T *residual) const;

private:
  // Velocity from c1 -> c2.
  std::array<double, 3> v1_;
  std::array<double, 3> c2InWorld_;
};

// Estimates the inverse location of a camera that took a photo of an image target, relative to the
// camera that originally photographed the imaged target.
class InversePoseEstimationImageTarget {
public:
  InversePoseEstimationImageTarget(double x, double y, double u, double v, double w)
      : x_(x), y_(y), u_(u), v_(v), w_(w) {}
  virtual ~InversePoseEstimationImageTarget() {}

  template <typename T>
  bool operator()(const T *const camera, T *residual) const;

private:
  double x_, y_, u_, v_, w_;
};

// Estimates the transform that needs to be applied to the points of a reference model to align them
// with a set of observed points.
class Full3dPoseEstimation {
public:
  // (xm, ym, zm) is a model point's 3d coordinates.
  // (xo, yo, zo) is an observed point's 3d coordinates.
  Full3dPoseEstimation(double xm, double ym, double zm, double xo, double yo, double zo)
      : xm_(xm), ym_(ym), zm_(zm), xo_(xo), yo_(yo), zo_(zo) {}
  virtual ~Full3dPoseEstimation() {}

  // The residual for a camera is the square distance of the model points to the observed points
  // when the camera transformation is applied to the observed points.
  template <typename T>
  bool operator()(const T *const camera, T *residual) const;

private:
  double xm_, ym_, zm_, xo_, yo_, zo_;
};

// Estimates the transform and scale that needs to be applied to the points of a reference model to
// align them with a set of observed points.
class Full3dWithScalePoseEstimation {
public:
  // (xm, ym, zm) is a model point's 3d coordinates.
  // (xo, yo, zo) is an observed point's 3d coordinates.
  Full3dWithScalePoseEstimation(double xm, double ym, double zm, double xo, double yo, double zo)
      : xm_(xm), ym_(ym), zm_(zm), xo_(xo), yo_(yo), zo_(zo) {}
  virtual ~Full3dWithScalePoseEstimation() {}

  // The residual for a camera is the square distance of the model points to the observed points
  // when the camera transformation is applied to the observed points.
  template <typename T>
  bool operator()(const T *const camera, T *residual) const;

private:
  double xm_, ym_, zm_, xo_, yo_, zo_;
};

// Estimates the scale that needs to be applied to the points of a reference model to align them
// with a set of observed points.
class Scale3dPoseEstimation {
public:
  // (xm, ym, zm) is a model point's 3d coordinates.
  // (xo, yo, zo) is an observed point's 3d coordinates.
  Scale3dPoseEstimation(double xm, double ym, double zm, double xo, double yo, double zo)
      : xm_(xm), ym_(ym), zm_(zm), xo_(xo), yo_(yo), zo_(zo) {}
  virtual ~Scale3dPoseEstimation() {}

  template <typename T>
  bool operator()(const T *const scale, T *residual) const;

private:
  double xm_, ym_, zm_, xo_, yo_, zo_;
};

class Scale1dPoseEstimation {
public:
  Scale1dPoseEstimation(double vio, double imu) : tracker_(vio), imu_(imu) {}
  virtual ~Scale1dPoseEstimation() {}

  template <typename T>
  bool operator()(const T *const scale, T *residual) const;

private:
  double tracker_, imu_;
};

// Estimates the translation and y-axis rotation for a camera with a known direction of gravity.
class YawOnlyPoseEstimation {
public:
  YawOnlyPoseEstimation(
    double qw, double qx, double qy, double qz, double x, double y, double z, double u, double v)
      : qw_(qw), qx_(qx), qy_(qy), qz_(qz), x_(x), y_(y), z_(z), u_(u), v_(v) {}
  virtual ~YawOnlyPoseEstimation() {}

  template <typename T>
  bool operator()(const T *const fourParams, T *residual) const;

private:
  double qw_, qx_, qy_, qz_, x_, y_, z_, u_, v_;
};

/// TEMPLATED IMPLEMENTATION
template <typename T>
bool CameraMovement::operator()(const T *const camera, T *residuals) const {
  auto rw = T(rw_);
  auto pw = T(pw_);
  residuals[0] = rw * (camera[0] - c_[0]);
  residuals[1] = rw * (camera[1] - c_[1]);
  residuals[2] = rw * (camera[2] - c_[2]);
  residuals[3] = pw * (camera[3] - c_[3]);
  residuals[4] = pw * (camera[4] - c_[4]);
  residuals[5] = pw * (camera[5] - c_[5]);
  return true;
}

template <typename T>
bool PositionTarget::operator()(const T *const v, T *residuals) const {
  // Get camera translation in world space, from view space.
  // We have view = (TR)^-1 = R^-1 * T^-1, or (R^-1)^-1 * view = T^-1.

  // Invert of angle-axis rotation is negative of its magnitude
  T rotationInvert[3] = {
    -v[0],
    -v[1],
    -v[2],
  };
  T cameraInWorld[3];
  // cameraInWorld = R^-1 * T;
  ceres::AngleAxisRotatePoint(rotationInvert, v + 3, cameraInWorld);

  // We want cameraInWorld^-1 to be as close to v_ as possible
  // The original residual = -cameraInWorld - v_
  // Here we flip the sign with no change to the min val.
  residuals[0] = cameraInWorld[0] + v_[0];
  residuals[1] = cameraInWorld[1] + v_[1];
  residuals[2] = cameraInWorld[2] + v_[2];
  return true;
}

template <typename T>
bool RotationTarget::operator()(const T *const v, T *residuals) const {
  // We can solve for rotation in view space since it is trivially transformable to world space.
  residuals[0] = (v[0] - v_[0]);
  residuals[1] = (v[1] - v_[1]);
  residuals[2] = (v[2] - v_[2]);
  return true;
}

template <typename T>
bool ZeroTarget::operator()(const T *const v, T *residuals) const {
  // We can solve for rotation in view space since it is trivially transformable to world space.
  residuals[0] = v[0 + offset_];
  residuals[1] = v[1 + offset_];
  residuals[2] = v[2 + offset_];
  return true;
}

template <typename T>
inline T ReprojectionResidual::huber(const T x) const {
  static const T one = T(1);
  static const T delta = T(0.005);
  static const T idelta = T(1.0 / 0.005);
  static const T epsilon = T(1e-10);
  // ceres implicitly squares the residuals
  const T XOverDelta = x * idelta;
  return epsilon /* offset to prevent divide by 0 */
    + delta * sqrt(sqrt(one + XOverDelta * XOverDelta) - one);
}

template <typename T>
bool ReprojectionResidual::operator()(
  const T *const camera, const T *const point, T *residuals) const {
  T pt[3];
  ceres::AngleAxisRotatePoint(camera, point, pt);
  pt[0] += camera[3];
  pt[1] += camera[4];
  pt[2] += camera[5];

  // Do not move visible points behind the camera
  static const T zero = T(0);
  const T weight = pt[2] < zero ? T(100 * weight_) : T(weight_);

  residuals[0] = huber((pt[0] / pt[2] - T(u_)) * weight);
  residuals[1] = huber((pt[1] / pt[2] - T(v_)) * weight);
  return true;
}

template <typename T>
bool SnavelyReprojectionResidual::operator()(
  const T *const camera, const T *const point, T *residuals) const {
  T pt[3];
  ceres::AngleAxisRotatePoint(camera, point, pt);
  pt[0] += camera[3];
  pt[1] += camera[4];
  pt[2] += camera[5];

  residuals[0] = T(scale_) * pt[0] / pt[2] - T(u_);
  residuals[1] = T(scale_) * pt[1] / pt[2] - T(v_);
  return true;
}

template <typename T>
bool CurvyTargetReprojectionResidual::operator()(const T *const camera, T *residuals) const {
  static const T one = T(1.0);
  // the values computed based on the camera extrinsics can be cached across the many points until
  // the camera extrinsic changes.
  static T p[6];
  static T radiusXSq;
  static T radiusZSq;
  static T twoXZ;
  static T w[3];
  static T sintheta;
  static T costheta;
  static T theta2;
  static T theta;
  static T wCosTheta[3];

  static bool first = true;
  const int needsParamUpdate = first + (p[0] != camera[0]) + (p[1] != camera[1])
    + (p[2] != camera[2]) + (p[3] != camera[3]) + (p[4] != camera[4]) + (p[5] != camera[5]);
  if (needsParamUpdate) {
    first = false;
    p[0] = camera[0];
    p[1] = camera[1];
    p[2] = camera[2];
    p[3] = camera[3];
    p[4] = camera[4];
    p[5] = camera[5];

    radiusXSq = (T(curvyRadiusSquared_) - p[3] * p[3]);
    radiusZSq = (T(curvyRadiusSquared_) - p[5] * p[5]);
    twoXZ = 2.0 * p[3] * p[5];

    theta2 = ceres::DotProduct(p, p);
    theta = ceres::sqrt(theta2);
    w[0] = p[0] / theta;
    w[1] = p[1] / theta;
    w[2] = p[2] / theta;
    costheta = ceres::cos(theta);
    sintheta = ceres::sin(theta);

    wCosTheta[0] = w[0] * (T(1.0) - costheta);
    wCosTheta[1] = w[1] * (T(1.0) - costheta);
    wCosTheta[2] = w[2] * (T(1.0) - costheta);
  }

  T extrudedSearchRay[3] = {T(searchRayX_), T(searchRayY_), T(1.0f)};
  T rotatedPoint[3];

  // Rotate the cylinder so that it is straight up in world space.  We have inlined the ceres
  // function ceres::AngleAxisRotatePoint()
  if (theta2 > 0.0) {
    T w_cross_pt[3];
    ceres::CrossProduct(w, extrudedSearchRay, w_cross_pt);
    const T w_dot_pt = ceres::DotProduct(w, extrudedSearchRay);
    for (int i = 0; i < 3; ++i) {
      rotatedPoint[i] =
        extrudedSearchRay[i] * costheta + w_cross_pt[i] * sintheta + wCosTheta[i] * w_dot_pt;
    }
  } else {
    T w_cross_pt[3];
    ceres::CrossProduct(p, extrudedSearchRay, w_cross_pt);
    for (int i = 0; i < 3; ++i) {
      rotatedPoint[i] = extrudedSearchRay[i] + w_cross_pt[i];
    }
  }

  const T a = rotatedPoint[0] / rotatedPoint[2];
  const T b = rotatedPoint[1] / rotatedPoint[2];
  const T c = rotatedPoint[2] / rotatedPoint[2];
  const T invc = one / c;

  // Convert the upright points in world space to local space
  const T a2 = a * a;
  const T ac = a * c;
  const T c2 = c * c;

  const T A = ac * p[3];
  const T B = a2 * p[5];
  const T C = a2 + c2;
  const T invC = one / C;
  const T Dsq = c2 * (c2 * radiusXSq + ac * twoXZ + a2 * (radiusZSq));

  if (Dsq < 0.0 || C < 1e-6) {
    residuals[0] = T(1000);
    residuals[1] = T(1000);
    return true;
  }

  const T D = ceres::sqrt(Dsq);
  const T pz = (B - A - D) * invC;
  const T nz = (B - A + D) * invC;

  const T pt = pz - p[5] * invc;
  const T px = p[3] + a * pt;
  const T py = p[4] + b * pt;

  const T nt = nz - p[5] * invc;
  const T nx = p[3] + a * nt;
  const T ny = p[4] + b * nt;

  // Pick the solution closer to the camera.
  const T delta1[3] = {px - p[3], py - p[4], pz - p[5]};
  const T delta2[3] = {nx - p[3], ny - p[4], nz - p[5]};
  const T d1 = ceres::DotProduct(delta1, delta1);
  const T d2 = ceres::DotProduct(delta2, delta2);

  // The cylinder is now in local space.  Convert it into ray space.  This collapses the
  // intermediate step of converting into pixel space.
  T rayLocation[2];
  if (d1 < d2) {
    rayLocation[0] = ceres::atan2(px, -pz) * T(scaledXi2pi_) + T(shiftedScaleX_);
    rayLocation[1] = (py - T(0.5)) * -T(scaleY_) + T(intrinsicsInvShiftY_);
  } else {
    rayLocation[0] = ceres::atan2(nx, -nz) * T(scaledXi2pi_) + T(shiftedScaleX_);
    rayLocation[1] = (ny - T(0.5)) * -T(scaleY_) + T(intrinsicsInvShiftY_);
  }

  // use the scale to boost the signal so we're not working with extremely small numbers
  residuals[0] = T(residualScale_) * (rayLocation[0] - T(targetRayX_));
  residuals[1] = T(residualScale_) * (rayLocation[1] - T(targetRayY_));

  return true;
}

template <typename T>
void inv33(const T *m, T *im) {
  const auto ae = m[0] * m[4];
  const auto af = m[0] * m[5];
  const auto ah = m[0] * m[7];
  const auto ai = m[0] * m[8];

  const auto bd = m[1] * m[3];
  const auto bf = m[1] * m[5];
  const auto bg = m[1] * m[6];
  const auto bi = m[1] * m[8];

  const auto cd = m[2] * m[3];
  const auto ce = m[2] * m[4];
  const auto cg = m[2] * m[6];
  const auto ch = m[2] * m[7];

  const auto dh = m[3] * m[7];
  const auto di = m[3] * m[8];

  const auto eg = m[4] * m[6];
  const auto ei = m[4] * m[8];

  const auto fg = m[5] * m[6];
  const auto fh = m[5] * m[7];

  const auto r00 = ei - fh;
  const auto r10 = fg - di;
  const auto r20 = dh - eg;

  const auto id = 1.0 / (m[0] * r00 + m[1] * r10 + m[2] * r20);

  im[0] = id * r00;
  im[1] = id * (ch - bi);
  im[2] = id * (bf - ce);
  im[3] = id * r10;
  im[4] = id * (ai - cg);
  im[5] = id * (cd - af);
  im[6] = id * r20;
  im[7] = id * (bg - ah);
  im[8] = id * (ae - bd);
}

template <typename T>
bool PoseEstimationTranslationStability::operator()(const T *const camera, T *residuals) const {
  // Get camera translation in world space, from egocentric space.
  T camRotInWorld[9];
  ceres::AngleAxisToRotationMatrix(camera, camRotInWorld);
  T cameraInWorld[3];
  cameraInWorld[0] =
    -(camRotInWorld[0] * camera[3] + camRotInWorld[1] * camera[4] + camRotInWorld[2] * camera[5]);
  cameraInWorld[1] =
    -(camRotInWorld[3] * camera[3] + camRotInWorld[4] * camera[4] + camRotInWorld[5] * camera[5]);
  cameraInWorld[2] =
    -(camRotInWorld[6] * camera[3] + camRotInWorld[7] * camera[4] + camRotInWorld[8] * camera[5]);

  // Calculate acceleration. Order is c1 -> c2 -> camera.
  T v2[3];
  v2[0] = cameraInWorld[0] - T(c2InWorld_[0]);
  v2[1] = cameraInWorld[1] - T(c2InWorld_[1]);
  v2[2] = cameraInWorld[2] - T(c2InWorld_[2]);

  T a[3];
  a[0] = v2[0] - T(v1_[0]);
  a[1] = v2[1] - T(v1_[1]);
  a[2] = v2[2] - T(v1_[2]);

  // Weight is tuned to produce a smooth path while not preventing changes in direction.
  residuals[0] = a[0] * 0.1;
  residuals[1] = a[1] * 0.1;
  residuals[2] = a[2] * 0.1;
  return true;
}

template <typename T>
bool InversePoseEstimationImageTarget::operator()(const T *const c, T *residual) const {
  // When a new set of parameters is available, precompute the transform matrix and jets for those
  // parameters, and cache that computation for evaluation across the whole residual block.
  static T p[6];
  static T m[9];
  static bool first = true;
  const int needsParamUpdate = first + (c[0] != p[0]) + (c[1] != p[1]) + (c[2] != p[2])
    + (c[3] != p[3]) + (c[4] != p[4]) + (c[5] != p[5]);
  if (needsParamUpdate) {
    first = false;
    p[0] = c[0];
    p[1] = c[1];
    p[2] = c[2];
    p[3] = c[3];
    p[4] = c[4];
    p[5] = c[5];

    T H[9];
    // Compute the homography matrix induced by this camera extrinsic
    // H = R + T * n^T / d
    // where, H: homography matrix
    // .      R: rotation from the reference view
    // .      T: translation from the reference view
    // .      n^T: normal vector of the plane, transposed
    // .      d: distance between the camera frame and the plane along the plane normal
    // For derivation of this equation: see Multiple View Geometry, Hartley, Zisserman, 2nd edition
    // Result 13.1 and 13.2 on pages 325-327.
    // For auxiliary materials, see
    //   * https://cseweb.ucsd.edu/classes/sp04/cse252b/notes/lec04/lec4.pdf section 4.2
    //   * https://docs.opencv.org/master/d9/dab/tutorial_homography.html demo 3
    //
    // For our case, the image target is distance 1 in front of us facing backward.
    // i.e. (n^T / d) = (0, 0, 1)
    // Hence H = [R1 R2 R3] + [0 0 T]
    // where R1, R2, R3 are the column vectors of R
    // and T is the translation vector
    //     0 is the zero vector
    ceres::AngleAxisToRotationMatrix(c, ceres::RowMajorAdapter3x3(H));

    H[2] += c[3];
    H[5] += c[4];
    H[8] += c[5];

    // This is quite heavy but it gives much better pose results
    inv33(H, m);
  }
  // We compute the homography induced by our camera extrinsic
  // Take the inverse of that and minimize the ray distance

  const auto x = m[0] * x_ + m[1] * y_ + m[2];
  const auto y = m[3] * x_ + m[4] * y_ + m[5];
  const auto iz = 1.0 / (m[6] * x_ + m[7] * y_ + m[8]);

  // TODO(nb): factor w_ into precomputed u_, v_, and m_.
  residual[0] = w_ * (u_ - x * iz);
  residual[1] = w_ * (v_ - y * iz);

  return true;
}

template <typename T>
bool Full3dPoseEstimation::operator()(const T *const cam, T *residual) const {
  // When a new set of parameters is available, precompute the transform matrix and jets for those
  // parameters, and cache that computation for evaluation across the whole residual block.
  // NOTE(nb): in this cost function, only the rotation matrix needs significant precomputation,
  // so only it is checked for updates.
  static T params[3];
  static T rotation[9];
  static bool first = true;
  const int needsParamUpdate =
    first + (cam[0] != params[0]) + (cam[1] != params[1]) + (cam[2] != params[2]);
  if (needsParamUpdate) {
    first = false;
    params[0] = cam[0];
    params[1] = cam[1];
    params[2] = cam[2];

    ceres::AngleAxisToRotationMatrix(cam, ceres::RowMajorAdapter3x3(rotation));
  }

  // Rotate and translate the model point based on the camera params.
  const auto x = rotation[0] * xm_ + rotation[1] * ym_ + rotation[2] * zm_ + cam[3];
  const auto y = rotation[3] * xm_ + rotation[4] * ym_ + rotation[5] * zm_ + cam[4];
  const auto z = rotation[6] * xm_ + rotation[7] * ym_ + rotation[8] * zm_ + cam[5];

  // TODO(nb): Add point weights if needed.
  residual[0] = xo_ - x;
  residual[1] = yo_ - y;
  residual[2] = zo_ - z;

  return true;
}

template <typename T>
bool Full3dWithScalePoseEstimation::operator()(const T *const cam, T *residual) const {
  // When a new set of parameters is available, precompute the transform matrix and jets for those
  // parameters, and cache that computation for evaluation across the whole residual block.
  // NOTE(nb): in this cost function, only the rotation matrix needs significant precomputation,
  // so only it is checked for updates.
  static T params[3];
  static T rotation[9];
  static bool first = true;
  const int needsParamUpdate =
    first + (cam[0] != params[0]) + (cam[1] != params[1]) + (cam[2] != params[2]);
  if (needsParamUpdate) {
    first = false;
    params[0] = cam[0];
    params[1] = cam[1];
    params[2] = cam[2];

    ceres::AngleAxisToRotationMatrix(cam, ceres::RowMajorAdapter3x3(rotation));
  }

  const auto x =
    rotation[0] * cam[6] * xm_ + rotation[1] * cam[6] * ym_ + rotation[2] * cam[6] * zm_ + cam[3];
  const auto y =
    rotation[3] * cam[6] * xm_ + rotation[4] * cam[6] * ym_ + rotation[5] * cam[6] * zm_ + cam[4];
  const auto z =
    rotation[6] * cam[6] * xm_ + rotation[7] * cam[6] * ym_ + rotation[8] * cam[6] * zm_ + cam[5];

  // TODO(nb): Add point weights if needed.
  residual[0] = xo_ - x;
  residual[1] = yo_ - y;
  residual[2] = zo_ - z;

  return true;
}

template <typename T>
bool Scale3dPoseEstimation::operator()(const T *const scale, T *residual) const {
  residual[0] = scale[0] * xo_ - xm_;
  residual[1] = scale[0] * yo_ - ym_;
  residual[2] = scale[0] * zo_ - zm_;

  return true;
}

template <typename T>
bool Scale1dPoseEstimation::operator()(const T *const params, T *residual) const {
  residual[0] = params[0] * tracker_ - imu_ + params[1];

  return true;
}

template <typename T>
bool YawOnlyPoseEstimation::operator()(const T *const c, T *residual) const {
  // When a new set of parameters is available, precompute the transform matrix and jets for those
  // parameters, and cache that computation for evaluation across the whole residual block.
  static T p[4];
  static T r[9];
  static T t[3];
  static bool first = true;
  const int needsParamUpdate =
    first + (c[0] != p[0]) + (c[1] != p[1]) + (c[2] != p[2]) + (c[3] != p[3]);
  if (needsParamUpdate) {
    first = false;
    p[0] = c[0];
    p[1] = c[1];
    p[2] = c[2];
    p[3] = c[3];

    //      |  cos(yr) 0 sin(yr) 0 |
    // RY = |        0 1       0 0 |
    //      | -sin(yr) 0 cos(yr) 0 |
    //      |        0 0       0 1 |
    //
    //      |  cos(yr) 0 sin(yr) 0 |   | q00 q01 q02 0 |
    // R  = |        0 1       0 0 | * | q10 q11 q12 0 |
    //      | -sin(yr) 0 cos(yr) 0 |   | q20 q21 q22 0 |
    //      |        0 0       0 1 |   |   0   0   0 1 |
    //
    //      |( cy q00 + sy q20) ( cy q01 + sy q21) ( cy q02 + sy q22)|
    // R  = |              q10                q11                q12 |
    //      |(-sy q00 + cy q20) (-sy q01 + cy q21) (-sy q02 + cy q22)|
    //
    // invert position: (TR)^-1 = R^-1*T^-1 = R' * (T^-1)
    // | r00 r10 r20 0 |   | 1 0 0 -tx |
    // | r01 r11 r21 0 | * | 0 1 0 -ty |
    // | r02 r12 r22 0 |   | 0 0 1 -tz |
    // |   0   0   0 1 |   | 0 0 0   1 |
    //
    // | r00 r10 r20 -(r00 * tx + r10 * ty + r20 * tz) |
    // | r01 r11 r21 -(r01 * tx + r11 * ty + r21 * tz) |
    // | r02 r12 r22 -(r02 * tx + r12 * ty + r22 * tz) |
    // |   0   0   0                                 1 |
    //
    // x = (r00 * x_ + r10 * y_ + r20 * z_ - (r00 * tx + r10 * ty + r20 * tz)
    // y = (r01 * x_ + r11 * y_ + r21 * z_ - (r01 * tx + r11 * ty + r21 * tz)
    // z = (r01 * x_ + r11 * y_ + r21 * z_ - (r02 * tx + r12 * ty + r22 * tz)
    // u = x / z
    // v = y / z
    T q[4];
    q[0] = T(qw_);
    q[1] = T(qx_);
    q[2] = T(qy_);
    q[3] = T(qz_);
    T qr[9];

    // Compute the rotation matrix of the input quaternion that specifies the pose.
    // This assumes the input quaternion is pre-normalied. The output is row-major.
    ceres::QuaternionToScaledRotation(q, qr);

    // Compute the rotated orientation after applying a y-axis rotation. The inverted pose rotation
    // is the transpose of this matrix.
    const T cy = cos(p[3]);
    const T sy = sin(p[3]);
    r[0] = cy * qr[0] + sy * qr[6];
    r[1] = cy * qr[1] + sy * qr[7];
    r[2] = cy * qr[2] + sy * qr[8];
    r[3] = qr[3];
    r[4] = qr[4];
    r[5] = qr[5];
    r[6] = -sy * qr[0] + cy * qr[6];
    r[7] = -sy * qr[1] + cy * qr[7];
    r[8] = -sy * qr[2] + cy * qr[8];

    // Compute the translational components of the inverted pose.
    t[0] = -(r[0] * p[0] + r[3] * p[1] + r[6] * p[2]);
    t[1] = -(r[1] * p[0] + r[4] * p[1] + r[7] * p[2]);
    t[2] = -(r[2] * p[0] + r[5] * p[1] + r[8] * p[2]);
  }

  const auto x = r[0] * x_ + r[3] * y_ + r[6] * z_ + t[0];
  const auto y = r[1] * x_ + r[4] * y_ + r[7] * z_ + t[1];
  const auto iz = 1.0 / (r[2] * x_ + r[5] * y_ + r[8] * z_ + t[2]);

  residual[0] = u_ - x * iz;
  residual[1] = v_ - y * iz;

  return true;
}

}  // namespace c8
