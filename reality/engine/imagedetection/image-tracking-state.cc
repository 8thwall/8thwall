// Copyright (c) 2019 8th Wall, Inc.
// Original Author: Scott Pollack (scott@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"image-tracking-state.h"};
  deps = {
    ":location",
    ":tracked-image",
    "//c8:hmatrix",
    "//c8:c8-log",
    "//c8:hpoint",
    "//c8:vector",
    "//c8/geometry:egomotion",
    "//reality/engine/geometry:image-warp",
    "//reality/engine/geometry:pose-pnp",
    "//reality/engine/features:frame-point",
    "//reality/engine/imagedetection:detection-image",
  };
}
cc_end(0x2791d6b5);

#include "c8/geometry/egomotion.h"
#include "reality/engine/geometry/image-warp.h"
#include "reality/engine/imagedetection/image-tracking-state.h"
#include "reality/engine/imagedetection/location.h"

namespace c8 {

const uint8_t ImageTrackingState::DESCRIPTOR_DISTANCE_THRESHOLD_LOCAL_ORB = 64;
const uint8_t ImageTrackingState::DESCRIPTOR_DISTANCE_THRESHOLD_LOCAL_GORB = 24;

bool ImageTrackingState::AGGRESSIVELY_FREE_MEMORY = true;

float ImageTrackingState::computeScale(
  const HMatrix &aFirst, const HMatrix &aSecond, const HMatrix &bFirst, const HMatrix &bSecond) {
  auto deltaA = egomotion(aFirst, aSecond);
  auto deltaB = egomotion(bFirst, bSecond);
  auto n = translation(deltaA).l2Norm();
  auto d = translation(deltaB).l2Norm();
  return n > 3e-2 && d >= 1e-4 ? n / d : -1.0f;
}

const FrameWithPoints &ImageTrackingState::featsForTrackedImage(
  const String &name,
  const FrameWithPoints &pyramidOnlyFeats,
  const Vector<FrameWithPoints> &roiFeats) {
  for (const auto &f : roiFeats) {
    const auto &roi = f.roi();
    if (
      (roi.source == ImageRoi::Source::IMAGE_TARGET
       || roi.source == ImageRoi::Source::CURVY_IMAGE_TARGET)
      && roi.name == name && !f.points().empty()) {
      return f;
    }
  }
  return pyramidOnlyFeats;
}

void ImageTrackingState::nextTargetViewPose(
  const FrameWithPoints &globalTrackingFeats,
  const FrameWithPoints &localTrackingFeats,
  DetectionImage &im,
  HMatrix *nextPose,
  bool *found,
  RandomNumbers *random) {
  sc_->localMatchPoseInliers_.clear();
  sc_->globalMatchPoseInliers_.clear();
  sc_->globalMatches_.clear();
  sc_->localMatches_.clear();
  sc_->featsRayInTargetView_.clear();
  Vector<float> weights;

  auto hasRoi = localTrackingFeats.roi().source == ImageRoi::Source::IMAGE_TARGET
    || localTrackingFeats.roi().source == ImageRoi::Source::CURVY_IMAGE_TARGET;
  if (localMode() || hasRoi) {
    *nextPose = trackedImage_.pose;
    {
      ScopeTimer t("local-match");
      if (im.getType() == DetectionImageType::CURVY) {
        sc_->featsRayInTargetView_ =
          projectSearchRayToTargetRay(im, localTrackingFeats, *nextPose, im.getGeometry());
      } else {
        sc_->featsRayInTargetView_ = projectToTargetSpace(im, localTrackingFeats, *nextPose);
      }
      uint8_t descriptorDistanceThreshold = 0;
      if (im.framePoints().featureType() == DescriptorType::ORB) {
        descriptorDistanceThreshold = DESCRIPTOR_DISTANCE_THRESHOLD_LOCAL_ORB;
      } else {
        descriptorDistanceThreshold = DESCRIPTOR_DISTANCE_THRESHOLD_LOCAL_GORB;
      }
      im.localMatcher().useScaleFilter(true);
      im.localMatcher().setDescriptorThreshold(descriptorDistanceThreshold);
      im.localMatcher().setRoiScale(hasRoi);
      im.localMatcher().setRadius(0.1f);  // 11.4 degrees search radius
      im.localMatcher().findMatches(sc_->featsRayInTargetView_, &sc_->localMatches_);
    }

    // solve for the pose in local mode
    // TODO(dat): Can we remove repeats e.g. im.localMatcher in both of these cases?
    if (im.getType() == DetectionImageType::CURVY) {
      // NOTE(dat): Should we add a local world pts variable in the scratch space?
      getPointsOnGeometry(
        sc_->localMatches_, im.framePoints().pixels(), im.getGeometry(), &sc_->worldPts_);
      getMatchedCamRays(sc_->localMatches_, localTrackingFeats, &sc_->camRays_);
      constexpr float minInlierFraction = 0.67f;
      if (solvePnP(
            sc_->worldPts_,
            sc_->camRays_,
            {10, 20, 500e-6f, 1e-2f, 10.0f, minInlierFraction},
            nextPose,
            &sc_->localMatchPoseInliers_,
            &sc_->scratch_)) {
        // found this image in local search
        *found = true;

        // Boost the pose estimate it the rotational motion is big (similar to the planar case)
        auto estRot = rotationMat(egomotion(trackedImage_.pose, *nextPose));
        auto projInf = (estRot * HPoint3(0.0f, 0.0f, 1.0f)).flatten();
        auto projDist2 = projInf.x() * projInf.x() + projInf.y() * projInf.y();
        if (projDist2 > 5e-6f) {
          sc_->featsRayInTargetView_ =
            projectSearchRayToTargetRay(im, localTrackingFeats, *nextPose, im.getGeometry());

          im.localMatcher().useScaleFilter(true);
          im.localMatcher().setDescriptorThreshold(64);
          im.localMatcher().setRoiScale(hasRoi);
          im.localMatcher().setRadius(0.02f);  // 2.3 degrees search radius
          im.localMatcher().findMatches(sc_->featsRayInTargetView_, &sc_->localMatches_);
          getPointsOnGeometry(
            sc_->localMatches_, im.framePoints().pixels(), im.getGeometry(), &sc_->worldPts_);
          getMatchedCamRays(sc_->localMatches_, localTrackingFeats, &sc_->camRays_);

          HMatrix boostedPose = *nextPose;
          if (solvePnP(
                sc_->worldPts_,
                sc_->camRays_,
                {10, 20, 500e-6f, 1e-2f, 1.0f, minInlierFraction},
                &boostedPose,
                &sc_->localMatchPoseInliers_,
                &sc_->scratch_)) {
            *nextPose = boostedPose;
          }
        }

        return;
      }
    } else {
      getMatchedRays(
        sc_->localMatches_, im, localTrackingFeats, &sc_->imTargetRays_, &sc_->camRays_, &weights);
      // Planar image target
      if (solveImageTargetHomography(
            sc_->imTargetRays_,
            sc_->camRays_,
            weights,
            {10, 20, 500e-6f, 1e-2f, 10.0f},
            nextPose,
            &sc_->localMatchPoseInliers_,
            &sc_->scratch_)) {
        // If there was a non-trivial rotational motion, Boost the pose estimate by considering only
        // highly geometrically consistent matches near the solved pose, and solve for camera motion
        // with a reduced motion constraint.
        auto estRot = rotationMat(egomotion(trackedImage_.pose, *nextPose));
        auto projInf = (estRot * HPoint3(0.0f, 0.0f, 1.0f)).flatten();
        auto projDist2 = projInf.x() * projInf.x() + projInf.y() * projInf.y();
        if (projDist2 > 5e-6f) {
          sc_->featsRayInTargetView_ = projectToTargetSpace(im, localTrackingFeats, *nextPose);
          im.localMatcher().useScaleFilter(true);
          im.localMatcher().setDescriptorThreshold(64);
          im.localMatcher().setRoiScale(hasRoi);
          im.localMatcher().setRadius(0.02f);  // 2.3 degrees search radius
          im.localMatcher().findMatches(sc_->featsRayInTargetView_, &sc_->localMatches_);
          getMatchedRays(
            sc_->localMatches_,
            im,
            localTrackingFeats,
            &sc_->imTargetRays_,
            &sc_->camRays_,
            &weights);
          HMatrix boostedPose = *nextPose;
          if (solveImageTargetHomography(
                sc_->imTargetRays_,
                sc_->camRays_,
                weights,
                {10, 20, 500e-6f, 1e-2f, 1.0f},
                &boostedPose,
                &sc_->localMatchPoseInliers_,
                &sc_->scratch_)) {
            *nextPose = boostedPose;
          }
        }
        *found = true;
        return;
      }
    }
  }  // done local-match

  {
    ScopeTimer t("global-match");
    im.globalMatcher().match(globalTrackingFeats, &sc_->globalMatches_);

    if (im.getType() == DetectionImageType::CURVY) {
      getPointsOnGeometry(
        sc_->globalMatches_, im.framePoints().pixels(), im.getGeometry(), &sc_->worldPts_);
      getMatchedCamRays(sc_->globalMatches_, globalTrackingFeats, &sc_->camRays_);
    } else {
      // default to planar
      getPointsAndRays(
        sc_->globalMatches_, im, globalTrackingFeats, &sc_->worldPts_, &sc_->camRays_);
    }
  }

  if (im.framePoints().featureType() == DescriptorType::ORB) {
    if (robustPnP(
          sc_->worldPts_,
          sc_->camRays_,
          HMatrixGen::i(),
          {},
          nextPose,
          &sc_->globalMatchPoseInliers_,
          random,
          &sc_->scratch_)) {
      *found = true;
      return;
    }
  } else {
    HPoint2 lowerLeft;
    HPoint2 upperRight;
    frameBounds(im.framePoints().intrinsic(), &lowerLeft, &upperRight);
    float confidence = 0.0f;
    // TODO(Yuling): might need further tuning of the params.
    RobustP2PParams params{.isImageTracking = true};
    params.ransac.maxNumIteration = 200;
    params.ransac.minNumIteration = 10;
    params.ransac.initialInlierGuess = 0.3f;
    params.ransac.inlierThreshold = 1e-3f;

    if (robustP2P(
          sc_->worldPts_,
          sc_->camRays_,
          lowerLeft,
          upperRight,
          HMatrixGen::i(),
          params,
          nextPose,
          &confidence,
          &sc_->globalMatchPoseInliers_,
          random,
          &sc_->p2pScratch_)) {
      *found = true;
      return;
    }
  }

  *nextPose = trackedImage_.pose;
  *found = false;
}

void ImageTrackingState::track(
  const FrameWithPoints &pyramidOnlyFeats,
  const Vector<FrameWithPoints> &roiFeats,
  DetectionImage &im,
  RandomNumbers *random,
  const HMatrix &tamExtrinsic) {
  bool found = false;
  const auto &localTrackingFeats =
    featsForTrackedImage(trackedImage_.name, pyramidOnlyFeats, roiFeats);

  auto globalTrackingFeats = pyramidOnlyFeats.clone();
  for (const auto &f : roiFeats) {
    if (f.roi().source == ImageRoi::Source::HIRES_SCAN) {
      globalTrackingFeats.addAll(f);
    }
  }

  HMatrix nextPose = HMatrixGen::i();
  nextTargetViewPose(globalTrackingFeats, localTrackingFeats, im, &nextPose, &found, random);

  angleFromLastPose_ = std::acos(
    (rotationMat(egomotion(trackedImage_.pose, nextPose)) * HPoint3{0.0f, 0.0f, 1.0f}).z());

  if (!AGGRESSIVELY_FREE_MEMORY) {
    sc_->featsRayInTargetViewPostFit_ = (im.getType() == DetectionImageType::CURVY)
      ? projectSearchRayToTargetRay(im, localTrackingFeats, nextPose, im.getGeometry())
      : projectToTargetSpace(im, localTrackingFeats, nextPose);
  }

  auto status = found
    ? (localMode() ? TrackedImage::Status::TRACKED : TrackedImage::Status::LOCATED)
    : TrackedImage::Status::NOT_FOUND;

  if (found || trackedImage_.everSeen()) {
    updateTrackingState(
      status, im, nextPose, pyramidOnlyFeats, pyramidOnlyFeats.intrinsic(), tamExtrinsic);
  }

  if (AGGRESSIVELY_FREE_MEMORY) {
    sc_.reset(new ImageTrackingScratchSpace());
  }
}

void ImageTrackingState::updateTrackingState(
  TrackedImage::Status status,
  DetectionImage &im,
  const HMatrix &nextPose,
  const FrameWithPoints &pyramidOnlyFeats,
  c8_PixelPinholeCameraModel camK,
  const HMatrix &tamExtrinsic) {
  auto targetK = im.framePoints().intrinsic();
  localMode_ = static_cast<int>(status) > 1;

  // Compute a cheap scale guess based on the the camera motion and target motion if a target was
  // found in consecutive frames.
  // TODO(nb): This could likely be improved with the following strategy:
  // * look back 5 frames
  // * only update if the extrinsic baseline is greater than 3e-2.
  auto lastFrameWasLocalMode = trackedImage_.isUsable();
  if (lastFrameWasLocalMode && localMode_) {
    auto guessedScale = computeScale(lastCamPos_, tamExtrinsic, trackedImage_.pose, nextPose);
    if (guessedScale > 0) {
      scaleGuess_ = scaleGuess_ * .8 + guessedScale * .2;
    }
  }

  auto worldPose = trackedImage_.worldPose;
  if (localMode_) {
    if (!worldInit_) {
      // World pose is only valid if we are using VIO.
      // Without VIO we initialize the worldPose for initial placement only.
      worldPose = tamExtrinsic * scaleTranslation(scaleGuess_, nextPose.inv());
    }
    worldInit_ = true;
  }

  auto newPose = nextPose;
  if (!localMode_) {
    // If not tracking, update the target-relative pose based on camera position.
    auto lastcampose =
      updateWorldPosition(lastCamPos_, scaleTranslation(scaleGuess_, nextPose.inv()));
    auto camposedelta = egomotion(tamExtrinsic, lastcampose);
    newPose = scaleTranslation(1.0f / scaleGuess_, camposedelta).inv();
  }

  lastCamPos_ = tamExtrinsic;

  int32_t lastSeen =
    localMode_ ? 0 : (trackedImage_.lastSeen < 0 ? -1 : trackedImage_.lastSeen + 1);
  int32_t firstSeen = localMode_ ? trackedImage_.firstSeen + 1 : 0;
  trackedImage_ = TrackedImage{
    status,
    trackedImage_.index,
    trackedImage_.name,
    newPose,
    worldPose,
    scaleGuess_,
    lastSeen,
    firstSeen,
    targetK,
    camK};
}

c8::String ImageTrackingState::toString() const noexcept {
  return c8::format(
    "(name: %s, localMode %d, scaleGuess %f)", trackedImage_.name.c_str(), localMode_, scaleGuess_);
}

}  // namespace c8
