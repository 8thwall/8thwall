// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)
#pragma once

#include <algorithm>
#include <cmath>

#include "c8/hvector.h"
#include "c8/vector.h"

namespace c8 {

// A linear bin divides a range from [min, max) into num equal size bins. Calling binNum on an input
// figures out which bin the input is in, truncating to 0 if the input is less than min, and
// numBins - 1 if the input is more than max.
//
// For example, if min=10, max=330, and numBins is 32, then each bin has an effective size of 10.
// * Values in [-inf, 19] will have a bin of 0.
// * Values in [20, 29] will have a bin of 1.
// * Values in [30, 39] will have a bin of 2.
// * etc.
// * Values in [310, 319] will have a bin of 30.
// * Values in [320, inf] will have a bin of 31.
struct LinearBin {
public:
  LinearBin() = default;
  LinearBin(size_t num, float min, float max)
      : n_(num - 1),
        min_(min),
        size_((max - min) > 0 ? (max - min) / num : 0),
        isize_(size_ > 0 ? 1.0f / size_ : 0) {}

  size_t clip(int bin) const { return std::min(static_cast<size_t>(std::max(0, bin)), n_); }
  size_t binNum(float v) const { return clip(static_cast<int>((v - min_) * isize_)); }
  float binSize() const { return size_; }

private:
  size_t n_ = 0;
  float min_ = 0.0f;
  float size_ = 0.0f;
  float isize_ = 0.0f;
};

// A FameBin divides the bounds of an image (in ray space) into nx column bins and ny row bins.
// For example, a 640x480 image with focal length 625 has bounds [[-.512, .512), [-.384, .384)].
// If nx = 20, then each column bin has width .0384. If ny = 30, then each row bin has height .0341.
// Bins are numbered in row-major order. In the example above,
// * The point [-.384, -.512,] would have bin 0.
// * The point [.384, -.512] would have bin number 19.
// * The point [-.384, .512] would have bin number 580.
// * The point [.384, .512] would have bin number 599.
class FrameBin {
public:
  FrameBin() = default;
  FrameBin(float minX, float minY, float maxX, float maxY, size_t nx, size_t ny) {
    nx_ = nx;
    x_ = LinearBin{nx, minX, maxX};
    y_ = LinearBin{ny, minY, maxY};
  }

  size_t binNumPt(float x, float y) const { return binNum(xBin(x), yBin(y)); }

  // Compute the bin number given a known row bin number (y) and a known col
  // bin number (x).
  size_t binNum(size_t x, size_t y) const { return y * nx_ + x; }

  // Compute the x-dimension bin number of a point.
  size_t xBin(float v) const { return x_.binNum(v); }
  size_t xClip(int v) const { return x_.clip(v); }
  float xSize() const { return x_.binSize(); }

  // Compute the y-dimension bin number of a point.
  size_t yBin(float v) const { return y_.binNum(v); }
  size_t yClip(int v) const { return y_.clip(v); }
  float ySize() const { return y_.binSize(); }

private:
  LinearBin x_;
  LinearBin y_;
  size_t nx_ = 1;
};

// A bin map is a data structure for querying frame points near a specified point. Calling
// BinMap::reset on a frame builds a queryable map of points. Subsequently, calling
// getPointsInRadiusEager returns pointers to points near the query point. This method is
// eager in the sense that all points within the radius are returned, but some points outside the
// radius are returned as well. A further test is needed to check that the points are actually
// within the desired radius.
struct BinMap {
public:
  BinMap(int nx, int ny) : map_(nx * ny), nx_(nx), ny_(ny){};

  template <typename TypeWithXAndY>
  void reset(
    float minX,
    float minY,
    float maxX,
    float maxY,
    const Vector<TypeWithXAndY> &points,
    const Vector<size_t> *indices = nullptr) {
    bin_ = FrameBin(minX, minY, maxX, maxY, nx_, ny_);
    for (auto &v : map_) {
      v.clear();
    }
    if (indices) {
      for (auto i : *indices) {
        const auto &pt = points[i];
        size_t b = bin_.binNumPt(pt.x(), pt.y());
        map_[b].push_back(i);
      }
    } else {
      for (size_t i = 0; i < points.size(); i++) {
        const auto &pt = points[i];
        size_t b = bin_.binNumPt(pt.x(), pt.y());
        map_[b].push_back(i);
      }
    }
    minx_ = minX;
    miny_ = minY;
    maxx_ = maxX;
    maxy_ = maxY;
  }

  // Find grid cells that may contain points that are in the specified radius of the given point.
  void binsForPoint(HVector2 pt, float radius, Vector<size_t> *bins) const {
    bins->clear();
    size_t xmin = bin_.xBin(pt.x() - radius);
    size_t xmax = bin_.xBin(pt.x() + radius);
    size_t ymin = bin_.yBin(pt.y() - radius);
    size_t ymax = bin_.yBin(pt.y() + radius);
    for (size_t y = ymin; y <= ymax; ++y) {
      for (size_t x = xmin; x <= xmax; ++x) {
        bins->push_back(bin_.binNum(x, y));
      }
    }
  }

  // Find grid cells that may contain points that are in the specified radius of the given line
  // segment.
  void binsOnLine(std::pair<HVector2, HVector2> line, float radius, Vector<size_t> *bins) const {
    auto lineDelta = line.second - line.first;
    auto lineSqLen = lineDelta.dot(lineDelta);
    // Line segment is a point.
    if (lineSqLen < 1e-10f) {
      binsForPoint(line.first, radius, bins);
      return;
    }

    bins->clear();

    // We will either follow the line from left to right or from bottom to top, depending on whether
    // the line passes through more vertical bins or more horizontal bins.
    auto horizontalGrid = bin_.ySize() <= 0.0f;
    auto binAspect = horizontalGrid ? 0 : bin_.xSize() / bin_.ySize();
    auto horizontalSearch =
      horizontalGrid || std::abs(lineDelta.x()) >= std::abs(binAspect * lineDelta.y());

    if (horizontalSearch) {
      // Search left to right.
      if (line.first.x() > line.second.x()) {
        std::swap(line.first, line.second);
        lineDelta = line.second - line.first;
      }

      // Find the horizontal slope of the line we are searching on.
      auto searchSlope = lineDelta.y() / lineDelta.x();

      // Increase the length of the line by `radius` in the x dimension. This may be somewhat larger
      // than we need but it avoids computing a square root. We may want to investigate whether
      // achieving a tighter bound here is worthwhile.
      auto searchOffset = HVector2{radius, searchSlope * radius};
      auto searchStart = line.first - searchOffset;
      auto searchEnd = line.second + searchOffset;

      // Truncate the x-search dimension to the left and right of the target image.
      if (searchStart.x() < minx_) {
        auto delta = minx_ - searchStart.x();
        searchStart = searchStart + HVector2{delta, searchSlope * delta};
      }
      if (searchEnd.x() > maxx_) {
        auto delta = searchEnd.x() - maxx_;
        searchEnd = searchEnd - HVector2{delta, searchSlope * delta};
      }

      // Find the range of x-bins we need to check.
      auto xmin = bin_.xBin(searchStart.x());
      auto xmax = bin_.xBin(searchEnd.x());

      // Find how much the line changes in x and y for each x-bin.
      auto searchInc = bin_.xSize() * searchSlope;

      // Find the height of the intersection of the line with the boundaries of a grid-cell
      // (or the adjusted ends of the line) on the close and far side.
      auto binNear = searchStart.y();
      auto binFarDelta = (minx_ + (xmin + 1) * bin_.xSize()) - searchStart.x();
      auto binFar = binNear + searchSlope * binFarDelta;

      // Check the direction we're sloping.
      const bool slopingUp = searchSlope >= 0.0f;

      // For each x bin, find the left-edge and right-edge y+radius values, and add all y-bins
      // within the radius.
      for (auto x = xmin; x <= xmax; ++x) {
        // Find the min and max bin coordinates.
        auto binCoord = slopingUp ? std::pair<float, float>{binNear - radius, binFar + radius}
                                  : std::pair<float, float>{binFar - radius, binNear + radius};

        // If the values are outside of the image, we don't need to add any bins.
        if (binCoord.first <= maxy_ && binCoord.second >= miny_) {
          // Add a vertical band of bins for this x bin.
          auto ymin = bin_.yBin(binCoord.first);
          auto ymax = bin_.yBin(binCoord.second);
          for (auto y = ymin; y <= ymax; ++y) {
            bins->push_back(bin_.binNum(x, y));
          }
        }

        // Compute the vertical line intersections for the next bin.
        binNear = binFar;
        binFar = slopingUp ? std::min(binFar + searchInc, searchEnd.y())
                           : std::max(binFar + searchInc, searchEnd.y());
      }
    } else {
      // Search bottom to top.
      if (line.first.y() > line.second.y()) {
        std::swap(line.first, line.second);
        lineDelta = line.second - line.first;
      }
      // Find the vertical slope of the line we are searching on.
      auto searchSlope = lineDelta.x() / lineDelta.y();

      // Increase the length of the line by `radius` in the y dimension. This may be somewhat larger
      // than we need but it avoids computing a square root. We may want to investigate whether
      // achieving a tighter bound here is worthwhile.
      auto searchOffset = HVector2{searchSlope * radius, radius};
      auto searchStart = line.first - searchOffset;
      auto searchEnd = line.second + searchOffset;

      // Truncate the y-search dimension to the bottom and top of the target image.
      if (searchStart.y() < miny_) {
        auto delta = miny_ - searchStart.y();
        searchStart = searchStart + HVector2{searchSlope * delta, delta};
      }
      if (searchEnd.y() > maxy_) {
        auto delta = searchEnd.y() - maxy_;
        searchEnd = searchEnd - HVector2{searchSlope * delta, delta};
      }

      // Find the range of y-bins we need to check.
      auto ymin = bin_.yBin(searchStart.y());
      auto ymax = bin_.yBin(searchEnd.y());

      // Find how much the line changes in x for each y-bin.
      auto searchInc = bin_.ySize() * searchSlope;

      // Find the height of the intersection of the line with the boundaries of a grid-cell
      // (or the adjusted ends of the line) on the close and far side.
      auto binNear = searchStart.x();
      auto binFarDelta = (miny_ + (ymin + 1) * bin_.ySize()) - searchStart.y();
      auto binFar = binNear + searchSlope * binFarDelta;

      // Check the direction we're sloping.
      const bool slopingUp = searchSlope >= 0.0f;

      // For each y bin, find the bottom-edge and top-edge x+radius values, and add all x-bins
      // within the radius.
      for (auto y = ymin; y <= ymax; ++y) {
        // Find the min and max bin coordinates.
        auto binCoord = slopingUp ? std::pair<float, float>{binNear - radius, binFar + radius}
                                  : std::pair<float, float>{binFar - radius, binNear + radius};

        // If the values are outside of the image, we don't need to add any bins.
        if (binCoord.first <= maxx_ && binCoord.second >= minx_) {
          // Add a horizontal band of bins for this y bin.
          auto xmin = bin_.xBin(binCoord.first);
          auto xmax = bin_.xBin(binCoord.second);
          for (auto x = xmin; x <= xmax; ++x) {
            bins->push_back(bin_.binNum(x, y));
          }
        }

        // Compute the horizontal line intersections for the next bin.
        binNear = binFar;
        binFar = slopingUp ? std::min(binFar + searchInc, searchEnd.x())
                           : std::max(binFar + searchInc, searchEnd.x());
      }
    }
  }

  // public for inlining.
  Vector<Vector<size_t>> map_;  // Map from bin number to index into points.
  FrameBin bin_;

private:
  int nx_ = 1;
  int ny_ = 1;
  float minx_ = 0.0f;
  float miny_ = 0.0f;
  float maxx_ = 0.0f;
  float maxy_ = 0.0f;
};

struct BinMap3d {
public:
  BinMap3d(int nx, int ny, int nz) : map_(nx * ny * nz), nx_(nx), ny_(ny), nz_(nz){};

  template <typename TypeWithXAndYAndZ>
  void reset(
    float minX,
    float minY,
    float minZ,
    float maxX,
    float maxY,
    float maxZ,
    const Vector<TypeWithXAndYAndZ> &points) {
    minx_ = minX;
    miny_ = minY;
    minz_ = minZ;
    maxx_ = maxX;
    maxy_ = maxY;
    maxz_ = maxZ;
    xSize_ = (maxX - minX) / nx_;
    ySize_ = (maxY - minY) / ny_;
    zSize_ = (maxZ - minZ) / nz_;

    for (auto &v : map_) {
      v.clear();
    }
    for (size_t i = 0; i < points.size(); i++) {
      const auto &pt = points[i];
      size_t b = binNumPt(pt.x(), pt.y(), pt.z());
      map_[b].push_back(i);
    }
  }

  // Find grid cells that may contain points that are in the specified radius of the given point.
  template <typename TypeWithXAndYAndZ>
  void binsForPoint(TypeWithXAndYAndZ pt, float radius, Vector<size_t> *bins) const {
    bins->clear();
    size_t xmin = xBin(pt.x() - radius);
    size_t xmax = xBin(pt.x() + radius);
    size_t ymin = yBin(pt.y() - radius);
    size_t ymax = yBin(pt.y() + radius);
    size_t zmin = zBin(pt.z() - radius);
    size_t zmax = zBin(pt.z() + radius);

    for (size_t z = zmin; z <= zmax; ++z) {
      for (size_t y = ymin; y <= ymax; ++y) {
        for (size_t x = xmin; x <= xmax; ++x) {
          auto bin = binNum(x, y, z);
          if (!map_[bin].empty()) {
            bins->push_back(bin);
          }
        }
      }
    }
  }

  // Compute the bin number given a known row bin number (y), col bin number (x), and depth bin
  // number (z).
  size_t binNum(size_t x, size_t y, size_t z) const { return (z * ny_ + y) * nx_ + x; }

  // Compute the bin number of a point in the x, y, and z dimensions.
  size_t binNumPt(float x, float y, float z) const { return binNum(xBin(x), yBin(y), zBin(z)); }

  // Compute the x-dimension bin number of a point.
  size_t xBin(float v) const { return clip(static_cast<int>((v - minx_) / xSize_), nx_); }

  // Compute the y-dimension bin number of a point.
  size_t yBin(float v) const { return clip(static_cast<int>((v - miny_) / ySize_), ny_); }

  // Compute the z-dimension bin number of a point.
  size_t zBin(float v) const { return clip(static_cast<int>((v - minz_) / zSize_), nz_); }

  // Clip the bin index to be within the range.
  size_t clip(int v, size_t max) const {
    return std::min(std::max(v, 0), static_cast<int>(max) - 1);
  }
  Vector<Vector<size_t>> map_;  // Map from bin number to index into points.

private:
  int nx_ = 1;
  int ny_ = 1;
  int nz_ = 1;
  float minx_ = 0.0f;
  float miny_ = 0.0f;
  float minz_ = 0.0f;
  float maxx_ = 0.0f;
  float maxy_ = 0.0f;
  float maxz_ = 0.0f;
  float xSize_ = 0.0f;
  float ySize_ = 0.0f;
  float zSize_ = 0.0f;
};

}  // namespace c8
