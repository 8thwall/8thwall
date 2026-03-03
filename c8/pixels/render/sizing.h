// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Dat Chu (dat@8thwall.com)

#pragma once

namespace c8 {
struct Rect {
  int x;
  int y;
  int width;
  int height;
};

/** SceneSize are multiple of camera widths and heights.
 * This class helps you get subregion of a scene that aligns with the camera width x camera height
 */
class SceneSize {
public:
  static constexpr int CAM_WIDTH = 480;
  static constexpr int CAM_HEIGHT = 640;

  SceneSize(int numWidths, int numHeights)
      : w_(CAM_WIDTH * numWidths), h_(CAM_HEIGHT * numHeights) {}

  // get the full scene size
  Rect operator()() const { return {0, 0, w_, h_}; }
  // get an arbitrary sub-region of the scene
  Rect operator()(int colIdx, int rowIdx, int numWidths, int numHeights) const;
  // Get a span of columns for row 0
  Rect row0(int row0StartIdx, int numWidths) const;
  // Get a span of columns for row 1
  Rect row1(int row1StartIdx, int numWidths) const;

private:
  int w_;
  int h_;
};
}
