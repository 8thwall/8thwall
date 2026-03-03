
// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Paris Morgan (paris@8thwall.com)
//
// Control a virtual camera.

#pragma once

#include "c8/hmatrix.h"

namespace c8 {

bool updateViewCameraPosition(int key, const HMatrix &current, HMatrix *next);

}  // namespace c8
