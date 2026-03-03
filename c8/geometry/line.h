// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Dat Chu (dat@8thwall.com)
//
// Handling lines and checking for their properties: e.g. perperdicularity

#pragma once

#include <string>
#include <vector>
#include "c8/hpoint.h"
#include "c8/hvector.h"

namespace c8 {
HVector2 line(HPoint2 from, HPoint2 to);
HVector3 line(HPoint3 from, HPoint3 to);
bool pointOnLineBetween(HPoint3 bottomPt, HPoint3 midPt, HPoint3 topPt);
bool linesPerpendicular(HVector3 line1, HVector3 line2);

// clock-wise angle between vector AB and vector AC. Output [0, 2 * M_PI)
// This is also known as directed angle
float angleBetweenABAndAC(HPoint2 A, HPoint2 B, HPoint2 C);

// rotate pixel point coordinate as if the image has been rotated
// height and width are pre-rotated
HPoint2 rotateCW(const HPoint2 &pt, int height);
HPoint2 rotateCCW(const HPoint2 &pt, int width);
}
