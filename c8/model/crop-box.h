// Copyright (c) 2024 Niantic, Inc.
// Original Author: Nicholas Butko (nbutko@nianticlabs.com)

#pragma once

#include "c8/geometry/box3.h"
#include "c8/pixels/render/object8.h"
#include "c8/pixels/render/renderer.h"

namespace c8 {

// Creates a box of lines and points for corners to indicate the bounds used for cropping.
class CropBox : public Group {
public:
  CropBox(Box3 bounds, const Scene *scene, Renderer *renderer);
  enum class Side { TOP, LEFT, RIGHT, FRONT };
  // Makes the crop box visible from the given side with the camera lookng from the outside.
  // This is how the cropbox will be rendered when viewing from the front. .'s are not being
  // rendered.
  //                TOP
  //            ...........
  //           ..        ..
  //          . .       . .
  //         .  .      .  .
  // LEFT   +---------+   .    RIGHT
  //        |   ......|....
  //        |  .      |  .
  //        | .       | .
  //        |.        |.
  //        +---------+
  //           FRONT
  //            ^
  //            |
  //          Camera
  void viewFrom(Side side);

  // Update the crop box to the new bounds.
  void update(Box3 bounds);

  // Update the crop box to the new bounds based on changing the cornerIndex to xClip, yClip.
  // This is view side dependent and locks movement along the depth axis of the facing side.
  void update(float xClip, float yClip, int32_t cornerIndex);

  // Get the updated crop box bounds.
  Box3 bounds() const;

  // Return the index of the corner at positionClip, or -1 if no corner was hit.
  int hitTestClipSpace(HVector2 positionClip);

  // Update the crop box visualization based on the current scene.
  void tick(double frameTimeMillis);

  // Whether to render the crop region as an ellipse (as opposed to a box).
  void setElliptical(bool elliptical) { elliptical_ = elliptical; }
  bool elliptical() const { return elliptical_; }

private:
  const Scene *scene_;
  Box3 bounds_;
  Side side_ = Side::FRONT;
  bool elliptical_ = false;

  Renderable *dragHandles_ = nullptr;
  Renderable *cropPoints_ = nullptr;
  Renderable *cropBorders_ = nullptr;
  Renderable *cropCircle_ = nullptr;

  void updateCropBox();
};

}  // namespace c8
