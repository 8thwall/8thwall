// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Alvin Portillo (alvin@8thwall.com)

#pragma once

#include "c8/quaternion.h"
#include "c8/vector.h"
#include "capnp/message.h"
#include "reality/engine/api/reality.capnp.h"
#include "reality/engine/api/response/features.capnp.h"
#include "reality/engine/tracking/tracker.h"

namespace c8 {

class HitTestPerformer {
public:
  // Default constructor.
  HitTestPerformer() = default;

  // Default move constructors.
  HitTestPerformer(HitTestPerformer &&) = default;
  HitTestPerformer &operator=(HitTestPerformer &&) = default;

  // Disallow copying.
  HitTestPerformer(const HitTestPerformer &) = delete;
  HitTestPerformer &operator=(const HitTestPerformer &) = delete;

  // Main method to execute a request.
  void performHitTest(
    ResponseCamera::Reader camera,
    ResponseFeatureSet::Reader featurePointSet,
    ResponseSurfaces::Reader surfaces,
    const XrHitTestRequest::Reader request,
    XrHitTestResponse::Builder *response) const;

  static void rotatePointsToFrontOfCamera(
    const Vector<HPoint3> &pts,
    const HMatrix &extrinsic,
    Vector<HPoint3> *rotPts,
    float *d,
    HMatrix *r);

private:
  void getSurfacePoints(
    const HMatrix &extrinsic,
    const HMatrix &intrinsic,
    ResponseSurfaces::Reader surfaceResponse,
    XrHitTestRequest::Reader request,
    uint32_t hitTestResultsMask,
    Vector<HPoint3> *detectedSurfaceResults,
    Vector<HPoint3> *estimatedSurfaceResults) const;
};

}  // namespace c8
