// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Paris Morgan (paris@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "camera-controls.h",
  };
  deps = {
    "//c8:hmatrix",
    "//c8/geometry:egomotion",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xd3230eee);

#include "c8/geometry/egomotion.h"
#include "c8/pixels/camera-controls.h"

namespace c8 {

bool updateViewCameraPosition(int key, const HMatrix &current, HMatrix *next) {
  auto egoDelta = HMatrixGen::i();
  *next = current;
  switch (key) {
    case 'd':
      egoDelta = HMatrixGen::translation(0.2f, 0.0f, 0.0f);
      break;
    case 'a':
      egoDelta = HMatrixGen::translation(-0.2f, 0.0f, 0.0f);
      break;
    case 's':
      egoDelta = HMatrixGen::translation(0.0f, 0.0f, -0.2f);
      break;
    case 'w':
      egoDelta = HMatrixGen::translation(0.0f, 0.0f, 0.2f);
      break;
    case 'e':
      egoDelta = HMatrixGen::translation(0.0f, 0.1f, 0.0f);
      break;
    case 'q':
      egoDelta = HMatrixGen::translation(0.0f, -0.1f, 0.0f);
      break;

    case 'l':
      egoDelta = HMatrixGen::rotationD(0.0f, 2.5f, 0.0f);
      break;
    case 'j':
      egoDelta = HMatrixGen::rotationD(0.0f, -2.5f, 0.0f);
      break;
    case 'i':
      egoDelta = HMatrixGen::rotationD(2.0f, 0.0f, 0.0f);
      break;
    case 'k':
      egoDelta = HMatrixGen::rotationD(-2.0f, 0.0f, 0.0f);
      break;
    case 'o':
      egoDelta = HMatrixGen::rotationD(0.0f, 0.0f, 2.0f);
      break;
    case 'u':
      egoDelta = HMatrixGen::rotationD(0.0f, 0.0f, -2.0f);
      break;

    default:
      return false;
  }
  *next = updateWorldPosition(current, egoDelta);
  return true;
}
}  // namespace c8
