// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include <memory>

#include "c8/hvector.h"
#include "c8/pixels/render/object8.h"
#include "c8/vector.h"

namespace c8 {

struct HitTestResult {
  // The object that was hit.
  const Object8 *target = nullptr;

  // Element of the object that was hit (e.g. triangle if mesh, or point if point cloud).
  int32_t elementIndex = 0;

  // Distance of the hit object in clip space, used for sorting by distance.
  float clipSpaceZ = 0.0f;

  // Texture coordinate of the hit test result, needed for recursing to subscenes.
  HVector2 uv;

  // Whether this triangle that uses a subscene for its texture.
  bool rendersSubscene = false;
};

// Get a list of elements in all objects that render a pixel, sorted by distance from the camera.
// The same object may have multiple triangles or points intersect the pixel, and multiple objects
// may intersect the same pixel. The caller will usually just want element 0 (the closest hit to the
// camera), but in some cases special logic may be applied as post processing, for example to ignore
// results that render subscenes.
//
// The hit test query is defined in pixel space.
Vector<HitTestResult> hitTestPixel(const Scene &scene, HVector2 pixel);

// Get a list of elements in all objects that render a pixel, sorted by distance from the camera.
// The same object may have multiple triangles or points intersect the pixel, and multiple objects
// may intersect the same pixel. The caller will usually just want element 0 (the closest hit to the
// camera), but in some cases special logic may be applied as post processing, for example to ignore
// results that render subscenes.
//
// The hit test query is defined in clip space.
Vector<HitTestResult> hitTestClipSpace(const Scene &scene, HVector2 clipCoordinates);

// Hit test a single renderable object, without recursing to children. The specified point should be
// in clip space, i.e. -1 to 1 NDC coordinates. The outputs are unsorted.
Vector<HitTestResult> hitTestRenderable(const Renderable &r, const Camera &camera, HVector2 pt);

}  // namespace c8
