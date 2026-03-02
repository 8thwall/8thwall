// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Dat Chu (dat@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "draw-figure.h",
  };
  deps = {
    ":draw2",
    ":pixels",
    "//c8:color",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x1109cee3)

#include "c8/pixels/draw-figure.h"
#include "c8/pixels/draw2.h"

namespace c8 {
const float FONT_SIZE = 10.f;

Vector<float> getEqualPointsInRange(float min, float max, int numPoints) {
  Vector<float> points;
  float gap = (max - min) / numPoints;
  for (int i = 0; i < numPoints; i++) {
    points.push_back(min + i * gap);
  }
  return points;
}

// Assuming linear scale
Vector<float> getXValLocations(
  float offset, float length, float minVal, float maxVal, const Vector<float> &vals) {
  Vector<float> locations;
  locations.reserve(vals.size());
  float scaledLength = length / (maxVal - minVal);
  for (auto &val : vals) {
    float loc = offset + (val - minVal) * scaledLength;
    locations.push_back(loc);
  }
  return locations;
}

Vector<float> getYValLocations(
  float offset, float length, float totalLength, float minVal, float maxVal, const Vector<float> &vals) {
  Vector<float> locations;
  locations.reserve(vals.size());
  float scaledLength = length / (maxVal - minVal);
  for (auto &val : vals) {
    float loc = offset + (val - minVal) * scaledLength;
    loc = totalLength - loc;
    locations.push_back(loc);
  }
  return locations;
}

void Figure::legend() {
  isDrawLegend_ = true;
}

void Figure::draw(RGBA8888PlanePixels out) {
  fill(Color::OFF_WHITE, out);

  // draw the legend
  if (isDrawLegend_) {
    int numSeries = data_.size();
    drawArea_.offsetY_ = 25 + (numSeries / 5) * 10;

    float xOffset = 0.f;
    float yOffset = 2.f * FONT_SIZE;
    for (size_t i = 0; i < data_.size(); i++) {
      auto color = colors_[i];
      auto name = names_[i];
      auto textChar = name.size();
      putText(
        name,
        {drawArea_.offsetX_ + xOffset, yOffset},
        Color::OFF_WHITE,
        color,
        FONT_SIZE,
        out);
      xOffset += (textChar + 1) * FONT_SIZE / 2 + 10;

      // return to the next line if overshoot
      if ((drawArea_.offsetX_ + xOffset) > out.cols()) {
        xOffset = 0;
        yOffset += yOffset;
      }
    }
  }

  // find the drawing area inside of the output
  int width = out.cols() - drawArea_.offsetX_ - drawArea_.paddingX_;
  int height = out.rows() - drawArea_.offsetY_ - drawArea_.paddingY_;

  // draw the axes
  drawLine(
    {drawArea_.offsetX_, drawArea_.offsetY_},
    {drawArea_.offsetX_, drawArea_.offsetY_ + height},
    2,
    Color::BLUE,
    out);
  drawLine(
    {drawArea_.offsetX_, drawArea_.offsetY_ + height},
    {drawArea_.offsetX_ + width, drawArea_.offsetY_ + height},
    2,
    Color::BLUE,
    out);

  // draw the major ticks
  auto yTicks = getEqualPointsInRange(minY_, maxY_, numTicks_);
  Vector<float> yTickLocations = getYValLocations(drawArea_.paddingY_, height, out.rows(), minY_, maxY_, yTicks);
  for (int i = 0; i < yTicks.size(); i++) {
    auto yTick = yTicks[i];
    auto y = yTickLocations[i];
    drawLine({drawArea_.offsetX_ - 3, y}, {drawArea_.offsetX_ + 3, y}, 2, Color::BLUE, out);
    putText(
      format("%4.3f", yTick),
      {0.f, y + FONT_SIZE / 2},
      Color::BLUE,
      Color::OFF_WHITE,
      FONT_SIZE,
      out);
  }

  // draw the y=0 line
  float yZero = getYValLocations(drawArea_.paddingY_, height, out.rows(), minY_, maxY_, {0.f})[0];
  drawLine(
    {drawArea_.offsetX_, yZero}, {drawArea_.offsetX_ + width, yZero}, 2, Color::GRAY_04, out);

  // draw the minor ticks

  // draw the actual data
  Vector<float> xLocations = getXValLocations(drawArea_.offsetX_, width, minX_, maxX_, xPoints_);
  for (int i = 0; i < data_.size(); i++) {
    auto &currentSeries = data_[i];
    // TODO(dat): support marker style (dot, cross)
    // we currently only support line
    Vector<float> yLocations =
      getYValLocations(drawArea_.paddingY_, height, out.rows(), minY_, maxY_, currentSeries);

    float lastY = 0;
    float lastX = 0;
    for (int j = 0; j < currentSeries.size(); j++) {
      if (j == 0) {
        // the first datum cannot be drawn as a line
        continue;
      }
      float thisX = xLocations[j];
      float prevX = xLocations[j - 1];
      float thisY = yLocations[j];
      float prevY = yLocations[j - 1];
      if (std::isnan(thisY) || std::isnan(prevY)) {
        continue;
      }

      drawLine({prevX, prevY}, {thisX, thisY}, 1, colors_[i], out);
      lastX = thisX;
      lastY = thisY;
    }

    // draw the text of the series on the right side
     putText(
        names_[i],
        {lastX + 2, lastY},
        colors_[i],
        Color::OFF_WHITE,
        FONT_SIZE,
        out);
  }
}

}  // namespace c8
