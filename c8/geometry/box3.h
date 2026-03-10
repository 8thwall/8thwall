// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include <array>

#include "c8/hmatrix.h"
#include "c8/hpoint.h"
#include "c8/hvector.h"
#include "c8/vector.h"

namespace c8 {

struct Box3 {
  HPoint3 min;
  HPoint3 max;

  static Box3 from(const Vector<HPoint3> &pts);  // Get a box containing all points.

  // Construct a box given the center point as well as the width, height, and depth.
  static Box3 from(const HPoint3 &center, const HVector3 &size);

  bool intersects(const Box3 &b) const;  // Check if two boxes overlap.
  bool contains(const HPoint3 &pt) const;  // Check if box contains point
  Box3 merge(const Box3 &b) const;  // Get the box that contains all points of this and another box.
  HPoint3 center() const;           // Get the center point of this box.
  Vector<HPoint3> corners() const;  // Get the eight corners of this box.
  bool operator==(const Box3 &b) const;  // Check if two boxes are the same.
  bool operator!=(const Box3 &b) const;  // Check if two boxes are different.

  // Get a box that contains the points of this box with a transform applied. This may be larger
  // than the original box.
  Box3 transform(const HMatrix &m) const;

  // Get x,y,z dimension (length in each axis)
  HVector3 dimensions() const;

  // Split a box into octants of equal size
  // child:  0 1 2 3 4 5 6 7
  // x:      - - - - + + + +
  // y:      - - + + - - + +
  // z:      - + - + - + - +
  // i.e. x is the most significant bit, then y then z in the index
  std::array<Box3, 8> splitOctants() const;

  // Print this bounding box.
  String toString() const;
};

}  // namespace c8
