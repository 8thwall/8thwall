// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nathan Waters (nathan@8thwall.com)
#pragma once

#include "c8/pixels/opengl/gl-texture.h"
#include "c8/pixels/pixels.h"

namespace c8 {

GlTexture2D readImageToLinearTexture(ConstRGBA8888PlanePixels p);
GlTexture2D readImageToNearestTexture(ConstRGBA8888PlanePixels p);

}  // namespace c8
