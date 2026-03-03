// Copyright (c) 2020 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {"point-regression.h"};
  deps = {
    "//c8:hmatrix",
    "//c8:hvector",
  };
}
cc_end(0xd3629d7c);

#include "reality/engine/geometry/point-regression.h"

namespace c8 {

namespace {

// Computes (X' * X + e * I) under the interpretation that the data vector is an Nx3 matrix X.
// e * I is the ridge term in ridge regression that allows for the returned matrix to be invertible
// even if X'X is singular by effectively preferring linear regression coefficients that are close
// to zero.
//
// By construction, the returned matrix is symmetric.
HMatrix xtx33(const Vector<HVector3> &data, float e) {
  float xxs = 0.0f;
  float xys = 0.0f;
  float xzs = 0.0f;
  float yys = 0.0f;
  float yzs = 0.0f;
  float zzs = 0.0f;
  for (const auto &v : data) {
    float x = v.x();
    float y = v.y();
    float z = v.z();
    xxs += x * x;
    xys += x * y;
    xzs += x * z;
    yys += y * y;
    yzs += y * z;
    zzs += z * z;
  }
  return HMatrix{
    {xxs + e, xys, xzs, 0.0f},
    {xys, yys + e, yzs, 0.0f},
    {xzs, yzs, zzs + e, 0.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
  };
}

// Computes (X' * Y) under the interpretation that aData and bData are Nx3 matrices.
HMatrix xty33(const Vector<HVector3> &aData, const Vector<HVector3> &bData) {
  float xxs = 0.0f;
  float xys = 0.0f;
  float xzs = 0.0f;
  float yxs = 0.0f;
  float yys = 0.0f;
  float yzs = 0.0f;
  float zxs = 0.0f;
  float zys = 0.0f;
  float zzs = 0.0f;
  for (int i = 0; i < aData.size(); ++i) {
    float ax = aData[i].x();
    float ay = aData[i].y();
    float az = aData[i].z();
    float bx = bData[i].x();
    float by = bData[i].y();
    float bz = bData[i].z();
    xxs += ax * bx;
    xys += ax * by;
    xzs += ax * bz;
    yxs += ay * bx;
    yys += ay * by;
    yzs += ay * bz;
    zxs += az * bx;
    zys += az * by;
    zzs += az * bz;
  }
  return HMatrix{
    {xxs, yxs, zxs, 0.0f},
    {xys, yys, zys, 0.0f},
    {xzs, yzs, zzs, 0.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
  };
}
}  // namespace

HMatrix fitLinear33(const Vector<HVector3> &obs, const Vector<HVector3> &pred, float reg) {
  return xty33(xtx33(obs, reg).inv() * obs, pred);
}

float residual(const HMatrix &fit, const Vector<HVector3> &obs, const Vector<HVector3> &pred) {
  auto y = fit * obs;
  float r = 0.0f;
  for (int i = 0; i < pred.size(); ++i) {
    float dx = y[i].x() - pred[i].x();
    float dy = y[i].y() - pred[i].y();
    float dz = y[i].z() - pred[i].z();
    r += dx * dx + dy * dy + dz * dz;
  }
  return r;
}

}  // namespace c8
