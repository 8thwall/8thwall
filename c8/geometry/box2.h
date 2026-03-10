// Copyright (c) 2022 8th Wall, LLC
// Original Author: Yuyan Song (yuyansong@nianticlabs.com)
//
// Handling 2D boxes and related calculations

#pragma once

#include <cfloat>

namespace c8 {

////////////////////////////////////////// 2D Box /////////////////////////////////////////

struct Box2 {
  float x = 0.0f;
  float y = 0.0f;
  float w = 0.0f;
  float h = 0.0f;
};

bool doBoxesOverlap(const Box2 &bboxA, const Box2 &bboxB);
float intersectionOverUnion(const Box2 &bboxA, const Box2 &bboxB);

}  // namespace c8
