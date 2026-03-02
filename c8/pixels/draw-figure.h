
// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Dat Chu (dat@8thwall.com)
//
// Draw simple 2d line charts in a figure

#pragma once

#include <limits>
#include <algorithm>

#include "c8/color.h"
#include "c8/pixels/pixels.h"
#include "c8/string.h"
#include "c8/vector.h"


struct DrawArea {
  float offsetX_ = 60;
  float offsetY_ = 25;
  float paddingX_ = 25;
  float paddingY_ = 25;
};

namespace c8 {
// Draw 2D figure of multiple series.
//
// Typical usage. Make sure d0 is sized correctly to the area you want output.
//
// Figure fig;
// fig.xAxis(timeSeries_);
// fig.line(xSeries_, Color::RED);
// fig.line(ySeries_, Color::GREEN);
// fig.line(zSeries_, Color::BLUE);
// fig.legend();
// fig.draw(d0);
//
class Figure {
public:
  Figure(float numTicks = 20.f) : numTicks_(numTicks) {}
  template<class InputIt>
  void xAxis(InputIt first, InputIt last);
  // each series should have at least N points where N is the length of the vector passed into
  // xAxis()
  template<class InputIt>
  void line(InputIt first, InputIt last, Color color, const String &name);
  void legend();
  void draw(RGBA8888PlanePixels out);

private:
  float numTicks_;
  float minX_ = std::numeric_limits<float>::max();
  float maxX_ = std::numeric_limits<float>::min();
  float minY_ = std::numeric_limits<float>::max();
  float maxY_ = std::numeric_limits<float>::min();
  Vector<Vector<float>> data_;
  Vector<float> xPoints_;
  Vector<Color> colors_;
  Vector<String> names_;

  DrawArea drawArea_;
  bool isDrawLegend_ = false;
};


template<class InputIt>
void Figure::xAxis(InputIt first, InputIt last) {
  std::copy(first, last, std::back_inserter(xPoints_));
  for (InputIt it = first; it != last; it++) {
    float x = *it;
    if (minX_ > x) {
      minX_ = x;
    }
    if (maxX_ < x) {
      maxX_ = x;
    }
  }
}

template<class InputIt>
void Figure::line(InputIt first, InputIt last, Color color, const String &name) {
  Vector<float> data;
  std::copy(first, last, std::back_inserter(data));
  data_.push_back(data);
  colors_.push_back(color);
  names_.push_back(name);

  float minY = std::numeric_limits<float>::max();
  float maxY = std::numeric_limits<float>::min();
  for (InputIt it = first; it != last; it++) {
    float pt = *it;
    if (minY > pt) {
      minY = pt;
    }
    if (maxY < pt) {
      maxY = pt;
    }
  }

  minY_ = minY_ < minY ? minY_ : minY;
  maxY_ = maxY_ > maxY ? maxY_ : maxY;
}


}  // namespace c8
