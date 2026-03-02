// Copyright (c) 2024 Niantic, Inc.
// Original Author: Nicholas Butko (nbutko@nianticlabs.com)

#pragma once

#include "c8/geometry/splat.h"
#include "c8/pixels/render/renderer.h"

namespace c8 {

// For points at a farther distance than radius, bake them into a skybox and remove them from the
// splat. Each face of the skybox is a square of size faceResolution x faceResolution.
SplatAttributes bakeSplatSkybox(const SplatAttributes &splat, float radius, int faceResolution);

// Merges the faces of the skybox into a single RGBA8888 image. The output is a 3x4 image where the
// faces fill only a portion of the image.
RGBA8888PlanePixelBuffer mergeSkybox(SplatSkybox skybox);

}  // namespace c8
