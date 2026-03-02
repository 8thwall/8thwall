// Copyright (c) 2023 Niantic, Inc.
// Original Author: Lynn Dang (lynndang@nianticlabs.com)
//
// Operations for multiclass.

#pragma once

#include "c8/pixels/pixels.h"
#include "c8/string.h"
#include "c8/vector.h"

namespace c8 {

void maxMultiClassMap(
  const Vector<FloatPixels> semanticsRes, int numClasses, RGBA8888PlanePixels &outPix);

// Compute the mask for certain semantics class.
// If the semantics possibility of 'classId' is greater than 'threshold',
// assign the mask color & alpha to this pixel;
// otherwise, the pixel is assigned to background color and alpha.
//
// @param semanticsRes the semantics results
// @param outPix result mask pixels
// @param classId desired semantics class ID
// @param threshold the semantics possibility threshold
// @param maskColor pixel color if it is valid
// @param maskAlpha pixel alpha if it is valid
// @param backgroundColor pixel color if it is background
// @param backgroundAlpha pixel alpha if it is background
// @return number of mask pixels that pass the threshold
int multiClassBinaryMap(
  const Vector<FloatPixels> semanticsRes,
  RGBA8888PlanePixels &outPix,
  int classId,
  float threshold = 0.4,
  Color maskColor = Color::WHITE,
  uint8_t maskAlpha = 255,
  Color backgroundColor = Color::TRUE_BLACK,
  uint8_t backgroundAlpha = 0);

// clang-format off
const Vector<String> SEMANTICS_CLASS_STRINGS = {
    "Sky",
    "Ground",
    "Natural Ground",
    "Artificial Ground",
    "Water",
    "Person",
    "Building",
    "Flower",
    "Foliage",
    "Tree Trunk",
    "Pet",
    "Sand",
    "Grass",
    "TV",
    "Dirt",
    "Vehicle",
    "Road",
    "Food",
    "Loungeable",
    "Snow",
    "Max Classes"};
// clang-format on

// The 20 different classifications available for the semantics.
enum class SemanticsClassifications {
  SKY,
  GROUND,
  NATURAL_GROUND,
  ARTIFICIAL_GROUND,
  WATER,
  PERSON,
  BUILDING,
  FLOWER,
  FOLIAGE,
  TREE_TRUNK,
  PET,
  SAND,
  GRASS,
  TV,
  DIRT,
  VEHICLE,
  ROAD,
  FOOD,
  LOUNGEABLE,
  SNOW,
};

}  // namespace c8
