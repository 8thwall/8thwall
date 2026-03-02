// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Paris Morgan (paris@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"spline.h"};
  deps = {
    "//c8:hpoint",
    "//c8:hvector",
    "//c8/string:format",
    "//c8:c8-log",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x0f8f8238);

#include <iostream>

#include "c8/c8-log.h"
#include "c8/geometry/spline.h"
namespace c8 {

Vector<Cubic> spline(const Vector<float> &xs, const Vector<float> &ys) {
  if (xs.empty() || ys.empty()) {
    return {};
  }

  int n = xs.size() - 1;
  Vector<float> a;
  a.insert(a.begin(), ys.begin(), ys.end());

  Vector<float> b(n);
  Vector<float> d(n);

  Vector<float> h;
  h.reserve(n);
  for (int i = 0; i < n; ++i) {
    h.push_back(xs[i + 1] - xs[i]);
  }

  Vector<float> alpha = {0.f};
  for (int i = 1; i < n; ++i) {
    alpha.push_back(3 * (a[i + 1] - a[i]) / h[i] - 3.f * (a[i] - a[i - 1]) / h[i - 1]);
  }

  Vector<float> c(n + 1);
  Vector<float> l(n + 1);
  Vector<float> mu(n + 1);
  Vector<float> z(n + 1);
  l[0] = 1.f;
  mu[0] = 0.f;
  z[0] = 0.f;

  for (int i = 1; i < n; ++i) {
    l[i] = 2.f * (xs[i + 1] - xs[i - 1]) - h[i - 1] * mu[i - 1];
    mu[i] = h[i] / l[i];
    z[i] = (alpha[i] - h[i - 1] * z[i - 1]) / l[i];
  }

  l[n] = 1.f;
  z[n] = 0.f;
  c[n] = 0.f;

  for (int j = n - 1; j >= 0; --j) {
    c[j] = z[j] - mu[j] * c[j + 1];
    b[j] = (a[j + 1] - a[j]) / h[j] - h[j] * (c[j + 1] + 2.f * c[j]) / 3.f;
    d[j] = (c[j + 1] - c[j]) / 3.f / h[j];
  }

  Vector<Cubic> res;
  res.reserve(n);
  for (int i = 0; i < n; ++i) {
    res.push_back({a[i], b[i], c[i], d[i], xs[i]});
  }
  return res;
}

int splineBin(const Vector<float> &xs, float val) {
  auto it = std::upper_bound(xs.begin(), xs.end(), val);
  if (it == xs.begin()) {
    return 0;
  }
  if (it == xs.end()) {
    return xs.size() - 1;
  }
  return it - xs.begin() - 1;
}
}  // namespace c8
