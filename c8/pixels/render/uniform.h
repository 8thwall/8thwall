// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Paris Morgan (paris@8thwall.com)
#pragma once

#include "c8/string.h"
#include "c8/vector.h"

namespace c8 {

// Holds information about a uniform. Accepts: Primitive, struct, array, or array of structs.
struct Uniform {
  String name;
  // Empty if the uniform is a primitive, else holds struct properties.
  Vector<String> properties;
  // 0 if the uniform is a primitive or struct, >0 if it's an array.
  int size = 0;
};

}  // namespace c8
