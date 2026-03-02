// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Scott Pollack (scott@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "bundle-residual.h",
  };
  deps = {
    ":observed-point",
    "//c8:hmatrix",
    "//c8:hpoint",
    "//c8:vector",
    "//c8:c8-log",
    "//c8/geometry:parameterized-geometry",
    "//c8/geometry:pixel-pinhole-camera-model",
    "//c8/stats:scope-timer",
    "@ceres//:ceres",
  };
  copts = {
    "-Wno-unused-private-field",
  };
}
cc_end(0xba3ef855);

#include "ceres/ceres.h"
#include "c8/geometry/parameterized-geometry.h"
#include "reality/engine/geometry/bundle-residual.h"

namespace c8 {
namespace {
constexpr double SMALL_FLOAT = 1e-12;
}  // namespace

double computeObservedPointWeight(ObservedPoint pt) {
  // Larger scales weighted less
  const auto scaleWeight = 1.0f / (1.0f + pt.scale / 2.0f);

  // larger descriptor distances weighted less
  const auto distWeight =
    pt.descriptorDist > 1000.0f ? 1.0f : 1.0f / (1.0f + pt.descriptorDist / 100.0f);

  return scaleWeight * distWeight * (pt.weight > SMALL_FLOAT ? pt.weight : 1.0f);
}

// We are only fitting the camera extrinsics, but we could also
// fit for the camera intrinsics by including those values.
ReprojectionResidual::ReprojectionResidual(ObservedPoint pt)
    : u_(pt.position.x()), v_(pt.position.y()) {
  weight_ = computeObservedPointWeight(pt);
}

CurvyTargetReprojectionResidual::CurvyTargetReprojectionResidual(
  const HPoint2 targetRay,
  const HPoint2 liftedSearchRay,
  const CurvyImageGeometry geom,
  const c8_PixelPinholeCameraModel &k,
  float residualScale)
    : curvyRadiusSquared_(geom.radius * geom.radius),
      intrinsicsInvShiftX_(-k.centerPointX / k.focalLengthHorizontal),
      intrinsicsInvShiftY_(-k.centerPointY / (-k.focalLengthVertical)),
      residualScale_(residualScale),
      scaleX_(
        ((geom.srcCols - 1) / (geom.activationRegion.right - geom.activationRegion.left))
        / k.focalLengthHorizontal),
      scaledXi2pi_(i2pi_ * scaleX_),
      scaleY_((geom.srcRows - 1) / (-k.focalLengthVertical)),
      searchRayX_(liftedSearchRay.x()),
      searchRayY_(liftedSearchRay.y()),
      shift_(0.5f - geom.activationRegion.left),
      shiftedScaleX_(shift_ * scaleX_ + intrinsicsInvShiftX_),
      targetRayX_(targetRay.x()),
      targetRayY_(targetRay.y()) {}

double sigmoidF(const double x) {
  // https://timvieira.github.io/blog/post/2014/02/11/exp-normalize-trick/
  // This implementation is more numerically stable
  if (x >= 0) {
    const double z = exp(-x);
    return 1.0 / (1 + z);
  } else {
    // if x is less than zero then z will be small, denom can't zero because it's 1+z.
    const double z = exp(x);
    return z / (1 + z);
  }
}

}  // namespace c8
