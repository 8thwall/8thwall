// Copyright (c) 2019 8th Wall, Inc.
// Original Author: Scott Pollack (scott@8thwall.com)

#pragma once

#include "c8/c8-log.h"
#include "c8/geometry/egomotion.h"
#include "c8/hmatrix.h"
#include "c8/vector.h"
#include "c8/quaternion.h"
#include "reality/engine/imagedetection/tracked-image.h"

namespace c8 {

struct RigidTransform {
  bool valid = false;
  HVector3 t;
  Quaternion r;
  float s;

  const HMatrix pose() const { return cameraMotion(t, r); }
};

class ImageTargetsRigidAssembly {
  public:
    ImageTargetsRigidAssembly() = default;

    // Default move
    ImageTargetsRigidAssembly(ImageTargetsRigidAssembly &&) = default;
    ImageTargetsRigidAssembly &operator=(ImageTargetsRigidAssembly &&) = default;

    // Disallow copying
    ImageTargetsRigidAssembly(const ImageTargetsRigidAssembly &) = delete;
    ImageTargetsRigidAssembly &operator=(const ImageTargetsRigidAssembly &) = delete;

    // Observe a collection of TrackedImages
    void observe(const Vector<TrackedImage> &images);

    // Returns true if the current rigid assembly camera position was estimated confidently.
    const bool valid() const { return isValid_; }

    // Check if a particular image index has a valid pose in the assembly
    const bool valid(size_t index) const {
      return !isValid_ ? isValid_ : imageTargets_.size() < index ? false : imageTargets_.at(index).valid;
    }

    // returns the local transform of the requested image index
    const HMatrix transform(size_t index) const {
      if (index > imageTargets_.size()) {
        return HMatrixGen::i();
      }
      return imageTargets_.at(index).pose();
    }

    // Rigid assemby camera pose
    const HMatrix pose() const { return origin_; }

    // returns the camera pose of the requested image index
    const HMatrix pose(size_t index) const {
      return egomotion(transform(index), origin_);
    }

    // Rigid assembly world pose
    const HMatrix worldPose() const {
      return world_ * scaleTranslation(scale_, origin_);
    }

    // returns the world pose of the requested image index
    const HMatrix worldPose(size_t index) const {
      return world_ * scaleTranslation(scale_, transform(index));
    }

    // The world scale (i.e., for frustum display)
    const float scale() const { return scale_; }

private:
  bool isValid_ = false;
  int relativeOriginIndex_ = -1;

  // scale factor to reach world coordinates
  float scale_ = 1.0f;
  // World pose origin
  HMatrix world_ = HMatrixGen::i();
  // during baking the world_ and scale_ fields are set to the values from the originating image.
  bool baked_ = false;

  // Camera pose origin
  HMatrix origin_ = HMatrixGen::i();
  Vector<RigidTransform> imageTargets_;

  HMatrix init(const TrackedImage &ti) {
    relativeOriginIndex_ = ti.index;
    origin_ = ti.pose;
    imageTargets_.clear();
    world_ = ti.worldPose;
    scale_ = ti.scale;
    baked_ = false;
    return origin_.inv();
  }

  RigidTransform& getTransform(size_t index) {
    while (imageTargets_.size() < index + 1) {
      imageTargets_.emplace_back();
    }
    return imageTargets_.at(index);
  }

  void setTransform(size_t index, const HMatrix &m, float scale) {
    RigidTransform &t = getTransform(index);
    t.valid = true;
    t.t = translation(m);
    t.r = rotation(m);
    t.s = scale;
  }
};

}
