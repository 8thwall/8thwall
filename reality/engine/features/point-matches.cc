// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)
#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"point-matches.h"};
  deps = {
    ":frame-point",
    "//c8:vector",
  };
}
cc_end(0x47a1c822);

#include "reality/engine/features/point-matches.h"

namespace c8 {

void filterMatches(
  const Vector<PointMatch> &in,
  const FrameWithPoints &first,
  const FrameWithPoints &second,
  int hammingDistThresh,
  float gaussFilterThresh,
  Vector<PointMatch> *out) {
  out->clear();

  if (in.empty()) {
    return;
  }

  int numMatches = 0;
  double xsum = 0.0;
  double ysum = 0.0;
  double xx = 0.0;
  double xy = 0.0;
  double yy = 0.0;

  // Filter using the hamming distance between the features.
  for (auto match : in) {
    if (match.descriptorDist > hammingDistThresh) {
      continue;
    }

    auto lastpt = first.points()[match.wordsIdx].position();
    auto thispt = second.points()[match.dictionaryIdx].position();

    auto xd = thispt.x() - lastpt.x();
    auto yd = thispt.y() - lastpt.y();

    xsum += xd;
    ysum += yd;
    xx += xd * xd;
    xy += xd * yd;
    yy += yd * yd;

    numMatches++;
  }

  if (numMatches == 0) {
    return;
  }

  // Apply a gaussian filter.
  auto mx = xsum / numMatches;
  auto my = ysum / numMatches;

  auto cxx = xx / numMatches - mx * mx;
  auto cxy = xy / numMatches - mx * my;
  auto cyy = yy / numMatches - my * my;

  auto sx = std::sqrt(cxx);
  auto sy = std::sqrt(cyy);
  auto r = cxy / (sx * sy);

  for (auto match : in) {
    if (match.descriptorDist > hammingDistThresh) {
      continue;
    }
    auto lastpt = first.points()[match.wordsIdx].position();
    auto thispt = second.points()[match.dictionaryIdx].position();

    auto xd = thispt.x() - lastpt.x();
    auto yd = thispt.y() - lastpt.y();

    auto xdm = xd - mx;
    auto ydm = yd - my;

    auto sqdist = xdm * xdm / cxx + ydm * ydm / cyy - 2 * r * xdm * ydm / (sx * sy);

    if (sqdist > gaussFilterThresh) {  // A higher value gives false match.
      continue;
    }
    out->push_back(match);
  }
}

}  // namespace c8
