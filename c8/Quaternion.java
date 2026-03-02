// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// Immutable class for representing Quaternions and Quaternion operations.

package com.the8thwall.c8;

public final class Quaternion {
  private final float w;
  private final float x;
  private final float y;
  private final float z;

  public Quaternion(float w, float x, float y, float z) {
    this.w = w;
    this.x = x;
    this.y = y;
    this.z = z;
  }

  public Quaternion(float[] q) {
    this.w = q[0];
    this.x = q[1];
    this.y = q[2];
    this.z = q[3];
  }

  // Accessors.
  public float w() {
    return w;
  }
  public float x() {
    return x;
  }
  public float y() {
    return y;
  }
  public float z() {
    return z;
  }

  public Quaternion conjugate() {
    return new Quaternion(w, -x, -y, -z);
  }

  public float squaredNorm() {
    return w * w + x * x + y * y + z * z;
  }

  public float norm() {
    return (float) Math.sqrt(squaredNorm());
  }

  public Quaternion normalize() {
    float n = norm();
    return new Quaternion(w / n, x / n, y / n, z / n);
  }

  public Quaternion inverse() {
    float sn = squaredNorm();
    return new Quaternion(w / sn, -x / sn, -y / sn, -z / sn);
  }

  public Quaternion plus(Quaternion rhs) {
    return new Quaternion(w + rhs.w, x + rhs.x, y + rhs.y, z + rhs.z);
  }

  public Quaternion times(Quaternion rhs) {
    return new Quaternion(
      w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z,
      w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y,
      w * rhs.y - x * rhs.z + y * rhs.w + z * rhs.x,
      w * rhs.z + x * rhs.y - y * rhs.x + z * rhs.w);
  }

  public void toRotationMat(float[] out) {
    double wx = w * x;
    double wy = w * y;
    double wz = w * z;
    double xx = x * x;
    double xy = x * y;
    double xz = x * z;
    double yy = y * y;
    double yz = y * z;
    double zz = z * z;

    float mm00 = (float)(1.0 - 2.0 * (yy + zz));
    float mm01 = (float)(2.0000000 * (xy - wz));
    float mm02 = (float)(2.0000000 * (xz + wy));

    float mm10 = (float)(2.0000000 * (xy + wz));
    float mm11 = (float)(1.0 - 2.0 * (xx + zz));
    float mm12 = (float)(2.0000000 * (yz - wx));

    float mm20 = (float)(2.0000000 * (xz - wy));
    float mm21 = (float)(2.0000000 * (yz + wx));
    float mm22 = (float)(1.0 - 2.0 * (xx + yy));

    float[][] rmat = {{mm00, mm01, mm02, 0.0f},   // Row 0
                      {mm10, mm11, mm12, 0.0f},   // Row 1
                      {mm20, mm21, mm22, 0.0f},   // Row 2
                      {0.0f, 0.0f, 0.0f, 1.0f}};  // Row 3

    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < 4; ++j) {
        out[i + 4 * j] = rmat[i][j];
      }
    }
  }

}
