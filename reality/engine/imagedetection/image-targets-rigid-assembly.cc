// Copyright (c) 2019 8th Wall, Inc.
// Original Author: Scott Pollack (scott@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"image-targets-rigid-assembly.h"};
  deps = {
    ":tracked-image",
    "//c8:hmatrix",
    "//c8:hpoint",
    "//c8:vector",
    "//c8:c8-log",
    "//c8/geometry:egomotion",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xe1787712);

#include "reality/engine/imagedetection/image-targets-rigid-assembly.h"

namespace c8 {

// Must see the originating image for at least this many frames before considered baked.
static int constexpr BAKING_MIN_FRAMECOUNT = 10;
// A scale change less than this means the world scale and position are baked.
static float constexpr BAKING_SCALE_THRESHOLD = 1e-6;
// Note that after baking if the world slides then the object would still slide with the world.
// TODO(scott) prevent object sliding when the world does -or- close the loop on the tracker.

void ImageTargetsRigidAssembly::observe(const Vector<TrackedImage> &images) {

  if (images.empty() || !images.front().isUsable()) {
    // No usable images
    isValid_ = false;
    return;
  }
  isValid_ = true;

  auto origin = origin_.inv();
  if (relativeOriginIndex_ < 0) {
    // First image is the new origin
    origin = init(images.front());
  } else {
    // Update the origin pose
    auto found = std::find_if(images.begin(), images.end(), [&](const auto &ti) {
      return ti.isUsable() && ti.index == relativeOriginIndex_;
    });
    if (found != images.end()) {
      origin = found->pose.inv();
      // Update world scale and position during bake-in
      if (
        !baked_ && found->lastSeen > BAKING_MIN_FRAMECOUNT
        && std::abs(scale_ - found->scale) < BAKING_SCALE_THRESHOLD) {
        baked_ = true;
      }
      if (!baked_) {
        scale_ = found->scale;
        world_ = found->worldPose;
      }
    } else {
      // Use the longest lived located image to update the origin pose.
      auto &ti = images.front();
      RigidTransform &prev = getTransform(ti.index);
      if (prev.valid) {
        auto prevPose = egomotion(prev.pose(), origin_);
        auto angleFromLastPose =
          std::acos((rotationMat(egomotion(prevPose, ti.pose)) * HPoint3{0.0f, 0.0f, 1.0f}).z());
        if (
          isnan(angleFromLastPose) || angleFromLastPose < 0.15f
          || ti.firstSeen <= BAKING_MIN_FRAMECOUNT) {
          origin = (prev.pose() * ti.pose).inv();
        }
      } else {
        // Use this image as new origin
        origin = init(ti);
      }
    }
  }

  // auto deltaO = egomotion(origin_.inv(), origin);
  // auto normO = translation(deltaO).l2Norm();

  // Update relativePose for all usable images
  for (auto &ti : images) {
    auto &prev = getTransform(ti.index);
    if (!ti.isUsable()) {
      prev.valid = false;
      continue;
    }
    // auto prevPose = egomotion(origin_, prev.pose());

    // TODO(scott) Update relative scale
    auto scale = prev.s;
    auto relativePose = egomotion(origin, ti.pose.inv());
    auto angleFromLastPose = std::acos(
      (rotationMat(egomotion(prev.pose(), relativePose)) * HPoint3{0.0f, 0.0f, 1.0f}).z());
    if (
      !prev.valid || isnan(angleFromLastPose) || angleFromLastPose < 0.15f
      || ti.firstSeen <= BAKING_MIN_FRAMECOUNT) {
      setTransform(ti.index, relativePose, scale);
    }
  }

  origin_ = origin.inv();
}

}  // namespace c8

// TODO(scott) relative scale work in progress
// if (prev.valid) {
//       auto initScale = prev.s < 1e-4;
//       auto prevPose = egomotion(origin_, prev.pose());
//       // auto prevPose = egomotion(scaleTranslation(initScale ? 1.0f : prev.s, prev.pose()),
//       origin_).inv(); auto deltaT = egomotion(prevPose, ti.pose.inv()); auto normT =
//       translation(deltaT).l2Norm(); if (std::min(normT, normO) >= 1e-4) {
//         auto s = normT / normO;
//         // fast initialization of scale
//         scale = initScale ? s : (prev.s * 0.8f + s * 0.2f);
//         // C8Log("Computed scale %d %f => %f / %f = %f\tw=%s p=%s %s => %s %s",
//         //   ti.index, prev.s, normT, normO, scale,
//         //   translation(origin_).toString().c_str(),
//         //   translation(prev.pose()).toString().c_str(),
//         //   translation(prevPose).toString().c_str(),
//         //   translation(ti.pose).toString().c_str(), prev.valid ?"V":"X");
//       }
//       // auto corr = (deltaO * scale);

//       // C8Log("delta %s %f %s || %s",
//       //   translation(deltaO).toString().c_str(), scale,
//       translation(relativePose).toString().c_str(),
//       //   translation(corr).toString().c_str());

//       // // egomotion(origin, ti.pose.inv());

//       // // Apply correction to relativePose
//       // auto motion = scaleTranslation(1.0f / scale, egomotion(prevPose, ti.pose.inv()));

//       // C8Log("motion was %s : %s %s | %s", translation(motion).toString().c_str(),
//       //   translation(origin).toString().c_str(),
//       //   translation(ti.pose).toString().c_str(),
//       //   translation(relativePose).toString().c_str()
//       // );
//       // auto last = updateWorldPosition(origin_.inv(), scaleTranslation(scale, ti.pose.inv()));
//       // auto delta = egomotion(origin, last);
//       // relativePose = scaleTranslation(1.0f / scale, delta).inv();
//     }
