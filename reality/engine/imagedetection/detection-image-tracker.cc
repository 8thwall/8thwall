// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"detection-image-tracker.h"};
  deps = {
    ":detection-image",
    ":location",
    ":tracked-image",
    ":image-tracking-state",
    "//c8:hmatrix",
    "//c8:hpoint",
    "//c8:random-numbers",
    "//c8:vector",
    "//c8/geometry:egomotion",
    "//reality/engine/features:frame-point",
    "//reality/engine/geometry:image-warp",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x95316561);

#include "c8/geometry/egomotion.h"
#include "reality/engine/geometry/image-warp.h"
#include "reality/engine/imagedetection/detection-image-tracker.h"
#include "reality/engine/imagedetection/location.h"

namespace {
class IdGenerator {
public:
  IdGenerator(std::mt19937 *rng) : rng_(rng) {};
  virtual uint32_t next() { return uniformDist_(*rng_); }

private:
  std::mt19937 *rng_;
  std::uniform_int_distribution<uint32_t> uniformDist_;
};
}  // namespace

namespace c8 {

static int constexpr MAX_NUM_TRACKED_IMAGES = 4;
static int constexpr MAX_GLOBALLY_TRACKED_PER_FRAME = 1;

// Number of lost frames to continue considering image tracked
static int constexpr MAX_UNTRACKED_FRAMES = 20;

// For test
TrackedImage DetectionImageTracker::track(
  const FrameWithPoints &feats,
  const Vector<FrameWithPoints> &roiFeats,
  DetectionImageMap &imageTargets,
  RandomNumbers *random) {
  std::mt19937 rng_;
  IdGenerator idGenerator_(&rng_);
  auto res = track(feats, roiFeats, imageTargets, random, HMatrixGen::i());
  return res.size() ? res[0] : TrackedImage{};
}

Vector<LocatedImage> DetectionImageTracker::locate(
  const FrameWithPoints &feats,
  const Vector<FrameWithPoints> &roiFeats,
  const HMatrix &tamExtrinsic,
  DetectionImageMap &imageTargets,
  RandomNumbers *random) {
  DetectionImagePtrMap t;
  for (auto it = imageTargets.begin(); it != imageTargets.end(); it++) {
    t.insert(std::make_pair(it->first, &it->second));
  }

  return locate(feats, roiFeats, tamExtrinsic, t, random);
}

Vector<LocatedImage> DetectionImageTracker::locate(
  const FrameWithPoints &feats,
  const Vector<FrameWithPoints> &roiFeats,
  const HMatrix &tamExtrinsic,
  const DetectionImagePtrMap &imageTargets,
  RandomNumbers *random) {
  Vector<LocatedImage> located;
  ScopeTimer t("image-locate");
  auto trackedImages = track(feats, roiFeats, imageTargets, random, tamExtrinsic);
  for (auto res : trackedImages) {
    located.push_back(locate(res, imageTargets, tamExtrinsic));
  }
  return located;
}

Vector<TrackedImage> DetectionImageTracker::track(
  const FrameWithPoints &feats,
  const Vector<FrameWithPoints> &roiFeats,
  DetectionImageMap &imageTargets,
  RandomNumbers *random,
  const HMatrix &tamExtrinsic) {
  DetectionImagePtrMap t;
  for (auto it = imageTargets.begin(); it != imageTargets.end(); it++) {
    t.insert(std::make_pair(it->first, &it->second));
  }

  return track(feats, roiFeats, t, random, tamExtrinsic);
}

Vector<TrackedImage> DetectionImageTracker::track(
  const FrameWithPoints &feats,
  const Vector<FrameWithPoints> &roiFeats,
  const DetectionImagePtrMap &imageTargets,
  RandomNumbers *random,
  const HMatrix &tamExtrinsic) {
  if (disabled_) {
    return {};
  }

  // Add new targets to images_
  bool resetRoundRobin = false;
  for (const auto &entry : imageTargets) {
    if (images_.find(entry.first) == images_.end()) {
      images_.insert(std::make_pair(entry.first, ImageTrackingState(entry.first)));
      resetRoundRobin = true;
    }
  }
  // Remove unloaded targets from images_
  for (auto it = images_.cbegin(); it != images_.cend(); /* no increment */) {
    if (imageTargets.find(it->first) == imageTargets.end()) {
      it = images_.erase(it);
      resetRoundRobin = true;
    } else {
      ++it;
    }
  }

  if (resetRoundRobin) {
    roundRobinIterator_ = images_.begin();
  }

  // On each frame, we want to track all tracked images, and also use a round-robin scheme to search
  // for MAX_GLOBALLY_TRACKED_PER_FRAME untracked images.
  int i = 0;
  int globallyTracked = 0;
  // The tracking iterator specifies the target we are examining for whether to attempt to track it.
  auto trackingIt = roundRobinIterator_;
  while (i < images_.size()) {
    auto name = trackingIt->first;
    auto &it = trackingIt->second;
    bool localTrack = it.everSeen() && (it.lastSeen() <= MAX_UNTRACKED_FRAMES);
    bool globalTrack = !localTrack && (globallyTracked < MAX_GLOBALLY_TRACKED_PER_FRAME);
    bool incrementRoundRobin = globalTrack || (localTrack && name == roundRobinIterator_->first);
    if (localTrack || globalTrack) {
      String tag = localTrack ? "track-local" : "track-global";
      ScopeTimer t(tag);

      it.track(feats, roiFeats, *imageTargets.at(name), random, tamExtrinsic);

      if (globalTrack) {
        ++globallyTracked;
      }

      // The round robin iterator specifies the next target we would like to track.
      if (incrementRoundRobin) {
        roundRobinIterator_++;
        if (roundRobinIterator_ == images_.end()) {
          roundRobinIterator_ = images_.begin();
        }
      }
    }

    // Move the tracking iterator forward. It will loop around all the targets testing if we should
    // track them.
    trackingIt++;
    if (trackingIt == images_.end()) {
      trackingIt = images_.begin();
    }
    i++;
  }

  // Return the 4 most recent TrackedImages
  Vector<TrackedImage> tis;
  for (auto it = images_.begin(); it != images_.end(); ++it) {
    if (!it->second.everSeen()) {
      continue;
    }
    tis.push_back(it->second.trackedImage());
  }

  // Sort by: lastSeen ASC, firstSeen DESC, name ASC
  std::sort(tis.begin(), tis.end(), [](const auto &a, const auto &b) {
    if (a.lastSeen != b.lastSeen) {
      return a.lastSeen < b.lastSeen;
    }
    if (a.firstSeen != b.firstSeen) {
      return a.firstSeen > b.firstSeen;
    }
    return a.name < b.name;
  });

  // If we aren't tracking any images, then set the front as the first target in our map.
  front_ = tis.empty() ? images_.begin()->first : tis.front().name;

  Vector<TrackedImage> t;
  for (auto &ti : tis) {
    if (ti.lastSeen <= MAX_UNTRACKED_FRAMES) {
      t.push_back(ti);
    }
    if (t.size() >= MAX_NUM_TRACKED_IMAGES) {
      break;
    }
  }

  return t;
}

LocatedImage DetectionImageTracker::locate(
  const TrackedImage &res, const DetectionImageMap &imageTargets, const HMatrix &camPos) {
  DetectionImagePtrMap t;
  for (auto it = imageTargets.begin(); it != imageTargets.end(); it++) {
    t.insert(std::make_pair(it->first, const_cast<DetectionImage *>(&it->second)));
  }

  return locate(res, t, camPos);
}

// For test
LocatedImage DetectionImageTracker::locate(
  const TrackedImage &t,
  const DetectionImageMap &imageTargets,
  const HMatrix &camPos,
  float scale) {
  TrackedImage t2(t);
  t2.scale = scale;
  return locate(t2, imageTargets, camPos);
}

LocatedImage DetectionImageTracker::locate(
  const TrackedImage &res, const DetectionImagePtrMap &imageTargets, const HMatrix &camPos) {
  const auto &t = *imageTargets.at(res.name);
  ImageRoi roi;
  auto pose = res.isUsable()
    ? updateWorldPosition(camPos, scaleTranslation(res.scale, res.pose.inv()))
    : res.worldPose;
  if (t.getType() == DetectionImageType::CURVY) {
    // Uncomment this section to use the new cylindrical ROI using a shader
    auto geom = t.getGeometry();
    // Increase the radius we use for ROI calculations so there is a margin around the image target
    // in the ROI.
    geom.radius += SHADER_ROI_RADIUS_INCREASE;
    geom.radiusBottom += SHADER_ROI_RADIUS_INCREASE;
    roi = {
      ImageRoi::Source::CURVY_IMAGE_TARGET,
      res.index,
      res.name,
      HMatrixGen::i(),
      geom,
      HMatrixGen::intrinsic(res.camK),
      res.pose};
  } else {
    roi = {
      ImageRoi::Source::IMAGE_TARGET,
      res.index,
      res.name,
      glImageTargetWarp(res.targetK, res.camK, res.pose, t.getPlanarGeometry())};
  }
  return locate(pose, t, res.scale, roi, res);
}

/**
 * @param scale how far away in the z axis is a point on the object we are locating. For planes,
 *              this scales to where we think the plane such that it is at 1.0f. For curvy it is
 *              always zero since our model space for the curvy is at the origin.
 */
LocatedImage DetectionImageTracker::locate(
  const HMatrix &pose,
  const DetectionImage &t,
  float scale,
  const ImageRoi &roi,
  const TrackedImage &targetSpace) {
  float originZ = t.getType() == DetectionImageType::CURVY ? 0.0f : scale;
  HPoint3 origin{0.0f, 0.0f, originZ};
  if (t.getType() == DetectionImageType::CURVY) {
    // move the origin back to where the full curvy origin should be
    origin =
      HMatrixGen::scale(scale, scale, scale) * t.getFullLabelToCroppedLabelPose().inv() * origin;
  }
  HPoint3 impos = pose * origin;
  auto imrot = Quaternion::fromHMatrix(pose);

  float width = 1.0;
  float height = 1.0;
  if (t.getType() != DetectionImageType::CURVY) {
    HPoint2 llb;
    HPoint2 urb;
    t.framePoints().frameBounds(&llb, &urb);
    width = urb.x() - llb.x();
    height = urb.y() - llb.y();
  }
  return {cameraMotion(impos, imrot), width * scale, height * scale, roi, targetSpace};
}

}  // namespace c8
