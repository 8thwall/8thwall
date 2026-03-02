// Copyright (c) 2021 8th Wall, Inc.
// Original Author: Dat Chu (dat@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "bundle-residual-analytic.h",
  };
  deps = {
    ":observed-point",
    "//c8:c8-log",
    "@ceres//:ceres",
  };
  copts = {
    "-Wno-unused-private-field",
  };
}
cc_end(0xf3e507b0);

#include "ceres/ceres.h"
#include "reality/engine/geometry/bundle-residual-analytic.h"

namespace c8 {
namespace {
constexpr double SMALL_FLOAT = 1e-12;
constexpr double SMALL_DOUBLE = 1e-40;
double computeObservedPointWeight(const ObservedPoint &pt) {
  // Larger scales weighted less
  const auto scaleWeight = 1.0f / (1.0f + pt.scale / 2.0f);

  // larger descriptor distances weighted less
  const auto distWeight =
    pt.descriptorDist > 1000.0f ? 1.0f : 1.0f / (1.0f + pt.descriptorDist / 100.0f);

  return scaleWeight * distWeight * (pt.weight > SMALL_FLOAT ? pt.weight : 1.0f);
}
}  // namespace

bool PositionTargetAnalytic::Evaluate(
  double const *const *parameters, double *residuals, double **jacobians) const {
  const double rx = parameters[0][0];
  const double ry = parameters[0][1];
  const double rz = parameters[0][2];
  const double tx = parameters[0][3];
  const double ty = parameters[0][4];
  const double tz = parameters[0][5];
  double theta2 = rx * rx + ry * ry + rz * rz;
  double vx = v_[0];
  double vy = v_[1];
  double vz = v_[2];

  // special case when theta2 is close to zero
  if (theta2 < 1e-6) {
    residuals[0] = -ry * tz + rz * ty + tx + vx;
    residuals[1] = rx * tz - rz * tx + ty + vy;
    residuals[2] = -rx * ty + ry * tx + tz + vz;

    if (jacobians == NULL) {
      return true;
    }
    double *jacobian = jacobians[0];
    if (jacobian == NULL) {
      return true;
    }

    jacobian[0] = 0;
    jacobian[1] = -tz;
    jacobian[2] = ty;
    jacobian[3] = 1;
    jacobian[4] = rz;
    jacobian[5] = -ry;
    jacobian[6] = tz;
    jacobian[7] = 0;
    jacobian[8] = -tx;
    jacobian[9] = -rz;
    jacobian[10] = 1;
    jacobian[11] = rx;
    jacobian[12] = -ty;
    jacobian[13] = tx;
    jacobian[14] = 0;
    jacobian[15] = ry;
    jacobian[16] = -rx;
    jacobian[17] = 1;
    return true;
  }

  // Compute normally
  const double x0 = std::pow(rx, 2);
  const double x1 = std::pow(ry, 2);
  const double x2 = std::pow(rz, 2);
  const double x3 = x0 + x1 + x2;
  const double x4 = std::pow(x3, 3.0 / 2.0);
  const double x5 = 1.0 / x4;
  const double x6 = std::sqrt(x3);
  const double x7 = std::cos(x6);
  const double x8 = tx * x7 + vx;
  const double x9 = x4 * x8;
  const double x10 = ry * tz;
  const double x11 = rz * ty;
  const double x12 = x10 - x11;
  const double x13 = std::sin(x6);
  const double x14 = x13 * x3;
  const double x15 = x12 * x14;
  const double x16 = rx * tx;
  const double x17 = ry * ty;
  const double x18 = rz * tz;
  const double x19 = x16 + x17 + x18;
  const double x20 = x7 - 1.0;
  const double x21 = x19 * x20;
  const double x22 = x21 * x6;
  const double x23 = rx * x22;
  const double x24 = ty * x7 + vy;
  const double x25 = rx * tz;
  const double x26 = rz * tx;
  const double x27 = x25 - x26;
  const double x28 = -ry * x22 + x14 * x27 + x24 * x4;
  const double x29 = tz * x7 + vz;
  const double x30 = x29 * x4;
  const double x31 = rx * ty;
  const double x32 = ry * tx;
  const double x33 = x31 - x32;
  const double x34 = x14 * x33;
  const double x35 = rz * x22;

  residuals[0] = x5 * (-x15 - x23 + x9);
  residuals[1] = x28 * x5;
  residuals[2] = x5 * (x30 - x34 - x35);

  if (jacobians == NULL) {
    return true;
  }
  double *jacobian = jacobians[0];
  if (jacobian == NULL) {
    return true;
  }

  const double x36 = std::pow(x3, -9.0 / 2.0);
  const double x37 = 3 * rx;
  const double x38 = std::pow(x3, 2);
  const double x39 = x38 * (x15 + x23 - x9);
  const double x40 = std::pow(x3, 5.0 / 2.0);
  const double x41 = x0 * x19;
  const double x42 = rx * x12;
  const double x43 = 2 * x13;
  const double x44 = x20 * x6;
  const double x45 = x6 * x8;
  const double x46 = x6 * x7;
  const double x47 = -x22;
  const double x48 = 3 * ry;
  const double x49 = rx * ry;
  const double x50 = -x21 * x49;
  const double x51 = ry * x12;
  const double x52 = tz * x14;
  const double x53 = x13 * x19;
  const double x54 = x49 * x53;
  const double x55 = 3 * rz;
  const double x56 = rx * rz;
  const double x57 = -x21 * x56;
  const double x58 = rz * x12;
  const double x59 = ty * x14;
  const double x60 = x53 * x56;
  const double x61 = x1 * x7;
  const double x62 = x2 * x7;
  const double x63 = 1.0 / x3;
  const double x64 = rz * x14;
  const double x65 = x44 * x49;
  const double x66 = ry * x14;
  const double x67 = x44 * x56;
  const double x68 = x28 * x38;
  const double x69 = rx * x27;
  const double x70 = x24 * x6;
  const double x71 = ry * x27;
  const double x72 = ry * rz;
  const double x73 = -x21 * x72;
  const double x74 = rz * x27;
  const double x75 = tx * x14;
  const double x76 = x53 * x72;
  const double x77 = x0 * x7;
  const double x78 = rx * x14;
  const double x79 = x44 * x72;
  const double x80 = x38 * (-x30 + x34 + x35);
  const double x81 = rx * x33;
  const double x82 = x29 * x6;
  const double x83 = ry * x33;
  const double x84 = rz * x33;

  jacobian[0] = x36
    * (x37 * x39
       + x40
         * (-x20 * x41 + x6 * (x13 * x41 - x14 * x16 - x16 * x44 + x37 * x45 - x42 * x43 - x42 * x46 + x47)));
  jacobian[1] = x36
    * (x39 * x48
       + x40
         * (x50 + x6 * (-x14 * x32 - x31 * x44 - x43 * x51 + x45 * x48 - x46 * x51 - x52 + x54)));
  jacobian[2] = x36
    * (x39 * x55
       + x40
         * (x57 + x6 * (-x14 * x26 - x25 * x44 - x43 * x58 + x45 * x55 - x46 * x58 + x59 + x60)));
  jacobian[3] = x63 * (x0 + x61 + x62);
  jacobian[4] = x5 * (x64 - x65);
  jacobian[5] = -x5 * (x66 + x67);
  jacobian[6] = x36
    * (-x37 * x68
       + x40
         * (x50 + x6 * (-x14 * x31 - x32 * x44 + x37 * x70 + x43 * x69 + x46 * x69 + x52 + x54)));
  jacobian[7] = x36
    * (x40
         * (-x1 * x21 + x6 * (x1 * x53 - x14 * x17 - x17 * x44 + x43 * x71 + x46 * x71 + x47 + x48 * x70))
       - x48 * x68);
  jacobian[8] = x36
    * (x40 * (x6 * (-x10 * x44 - x11 * x14 + x43 * x74 + x46 * x74 + x55 * x70 - x75 + x76) + x73)
       - x55 * x68);
  jacobian[9] = -x5 * (x64 + x65);
  jacobian[10] = x63 * (x1 + x62 + x77);
  jacobian[11] = x5 * (x78 - x79);
  jacobian[12] = x36
    * (x37 * x80
       + x40
         * (x57 + x6 * (-x14 * x25 - x26 * x44 + x37 * x82 - x43 * x81 - x46 * x81 - x59 + x60)));
  jacobian[13] = x36
    * (x40 * (x6 * (-x10 * x14 - x11 * x44 - x43 * x83 - x46 * x83 + x48 * x82 + x75 + x76) + x73)
       + x48 * x80);
  jacobian[14] = x36
    * (x40
         * (-x2 * x21 + x6 * (-x14 * x18 - x18 * x44 + x2 * x53 - x43 * x84 - x46 * x84 + x47 + x55 * x82))
       + x55 * x80);
  jacobian[15] = x5 * (x66 - x67);
  jacobian[16] = -x5 * (x78 + x79);
  jacobian[17] = x63 * (x2 + x61 + x77);
  return true;
}

bool TriangulatePointMultiviewAnalytic::Evaluate(
  double const *const *parameters, double *residuals, double **jacobians) const {
  const double ptx = parameters[0][0];
  const double pty = parameters[0][1];
  const double ptz = parameters[0][2];

  double proj0 = projection_[0];
  double proj1 = projection_[1];
  double proj2 = projection_[2];
  double proj3 = projection_[3];
  double proj4 = projection_[4];
  double proj5 = projection_[5];
  double proj6 = projection_[6];
  double proj7 = projection_[7];
  double proj8 = projection_[8];
  double proj9 = projection_[9];
  double proj10 = projection_[10];
  double proj11 = projection_[11];

  double rayx = ray_[0];
  double rayy = ray_[1];

  const double x0 = proj10 * ptz + proj11 + proj8 * ptx + proj9 * pty;
  const double x1 = 1.0 / x0;
  const double x2 = proj0 * ptx + proj1 * pty + proj2 * ptz + proj3 - rayx * x0;
  const double x3 = proj4 * ptx + proj5 * pty + proj6 * ptz + proj7 - rayy * x0;
  const double x4 = std::pow(x0, -2);

  residuals[0] = x1 * x2;
  residuals[1] = x1 * x3;

  if (jacobians == NULL) {
    return true;
  }
  double *jacobian = jacobians[0];
  if (jacobian == NULL) {
    return true;
  }

  jacobian[0] = x4 * (-proj8 * x2 + x0 * (proj0 - proj8 * rayx));
  jacobian[1] = x4 * (-proj9 * x2 + x0 * (proj1 - proj9 * rayx));
  jacobian[2] = x4 * (-proj10 * x2 + x0 * (-proj10 * rayx + proj2));
  jacobian[3] = x4 * (-proj8 * x3 + x0 * (proj4 - proj8 * rayy));
  jacobian[4] = x4 * (-proj9 * x3 + x0 * (proj5 - proj9 * rayy));
  jacobian[5] = x4 * (-proj10 * x3 + x0 * (-proj10 * rayy + proj6));
  return true;
}

// TODO(dat): Memoize so we don't have to recompute the homography's invert
// which is VERY heavy
bool InversePoseEstimationImageTargetAnalytic::Evaluate(
  double const *const *c, double *residuals, double **jacobians) const {
  static bool first = true;
  static double rx, ry, rz, tx, ty, tz, theta2;
  const int needsParamUpdate = first + (c[0][0] != rx) + (c[0][1] != ry) + (c[0][2] != rz)
    + (c[0][3] != tx) + (c[0][4] != ty) + (c[0][5] != tz);

  first = false;
  rx = c[0][0];
  ry = c[0][1];
  rz = c[0][2];
  tx = c[0][3];
  ty = c[0][4];
  tz = c[0][5];
  theta2 = rx * rx + ry * ry + rz * rz;

  if (theta2 < 1e-6) {
    double x = x_;
    double y = y_;
    double u = u_;
    double v = v_;
    double w = w_;
    const double x0 = rz * ty;
    const double x1 = std::pow(rz, 2);
    const double x2 = rx * rz;
    const double x3 = ry * rz;
    const double x4 = -x3;
    const double x5 = x1 + x * (ry + x2) - y * (rx + x4) + 1;
    const double x6 = tz + 1;
    const double x7 = std::pow(rx, 2) - rx * ty + x6;
    const double x8 = x7 * x;
    const double x9 = rx * ry;
    const double x10 = rx * tx;
    const double x11 = rz * tz;
    const double x12 = rz + x11;
    const double x13 = y * (x10 + x12 + x9);
    const double x14 = ry + tx;
    const double x15 = w / x5;
    const double x16 = rz * tx;
    const double x17 = std::pow(ry, 2) + ry * tx;
    const double x18 = y * (x17 + x6);
    const double x19 = ry * ty;
    const double x20 = x * (x12 + x19 - x9);
    const double x21 = -ry - tx - x0 + x13 + x2 + x8;
    const double x22 = -ty;
    const double x23 = 2 * rx + x22;
    const double x24 = x16 + x23;
    const double x25 = rz * x - y;
    const double x26 = tx * x2 + ty * x3 + tz * x1 + x1 + x17 + x7;
    const double x27 = -x24 * x5 + x25 * x26;
    const double x28 = w / (x26 * std::pow(x5, 2));
    const double x29 = rx * y;
    const double x30 = 2 * ry + tx;
    const double x31 = x0 + x30;
    const double x32 = rz * y + x;
    const double x33 = x26 * x32 - x31 * x5;
    const double x34 = rx + x22;
    const double x35 = 2 * rz;
    const double x36 = x10 + 2 * x11 + x19 + x35;
    const double x37 = rx * x;
    const double x38 = ry * y;
    const double x39 = x26 * (x35 + x37 + x38) - x36 * x5;
    const double x40 = 1 - x29;
    const double x41 = ry * x;
    const double x42 = w / (x1 + x2 * x + x3 * y + x40 + x41);
    const double x43 = x41 + 1;
    const double x44 = x16 + x18 - x20 + x3 + x34;
    residuals[0] = x15 * (u * x5 + x0 - x13 + x14 - x2 - x8);
    residuals[1] = x15 * (-rx + ty + v * x5 - x16 - x18 + x20 + x4);

    if (jacobians == NULL) {
      return true;
    }
    double *jacobian = jacobians[0];
    if (jacobian == NULL) {
      return true;
    }

    jacobian[0] = x28 * (x21 * x27 - x5 * (-x21 * x24 + x26 * (rz + x14 * y + x23 * x)));
    jacobian[1] = -x28 * (-x21 * x33 + x5 * (-x21 * x31 + x26 * (x29 - 1)));
    jacobian[2] = -x28 * (-x21 * x39 + x5 * (-x21 * x36 + x26 * (x34 + x6 * y)));
    jacobian[3] = x40 * x42;
    jacobian[4] = x42 * (rz + x37);
    jacobian[5] = -x32 * x42;
    jacobian[6] = -x28 * (-x27 * x44 + x5 * (-x24 * x44 + x26 * x43));
    jacobian[7] = x28 * (x33 * x44 - x5 * (x26 * (rz + x30 * y + x34 * x) - x31 * x44));
    jacobian[8] = -x28 * (-x39 * x44 + x5 * (x26 * (x14 - x6 * x) - x36 * x44));
    jacobian[9] = -x42 * (rz + x38);
    jacobian[10] = x42 * x43;
    jacobian[11] = x25 * x42;

    return true;
  }

  // clang-format off
  static double   x0,   x1,   x2,   x3,   x4,   x5,   x6,   x7,   x8,   x9,
                 x10,  x11,  x12,  x13,  x14,  x15,  x16,  x17,  x18,  x19,
                 x20,  x21,  x22,  x23,  x24,  x25,  x26,  x27,  x28,  x29,
                 x30,  x31,  x32,  x33,  x34,  x35,  x36,  x37,  x38,  x39,
                 x40,  x41,  x42,  x43,  x44,  x45,  x46,  x47,  x48,  x49,
                 x50,  x51,  x52,        x54,  x55,  x56,  x57,  x58,  x59,
                 x60,  x61,  x62,  x63,  x64,  x65,  x66,  x67            ,
                       x71,  x72,  x73,  x74,  x75,  x76,  x77,  x78,  x79,
                 x80,  x81,  x82,  x83,  x84,  x85,  x86,  x87,  x88,  x89,
                 x90,  x91,  x92,  x93,  x94,  x95,  x96,  x97,  x98,  x99,
                x100, x101, x102, x103, x104, x105,       x107, x108, x109,
                x110, x111, x112, x113, x114, x115, x116, x117, x118, x119,
                x120, x121, x122, x123, x124, x125, x126, x127, x128, x129,
                      x131, x132, x133, x134, x135, x136, x137, x138, x139,
                x140, x141, x142, x143, x144, x145, x146, x147, x148, x149,
                x150, x151, x152, x153, x154, x155, x156, x157, x158, x159,
                x160, x161, x162, x163, x164, x165, x166, x167, x168, x169,
                x170, x171, x172, x173, x174, x175, x176, x177, x178, x179,
                x180, x181, x182, x183, x184, x185, x186, x187, x188, x189,
                x190, x191, x192, x193, x194, x195, x196, x197, x198, x199,
                x200, x201, x202, x203, x204, x205, x206, x207, x208, x209,
                x210, x211, x212, x213, x214, x215, x216, x217, x218, x219,
                x220, x221, x222, x223, x224, x225, x226, x227, x228, x229,
                x230, x231, x232, x233, x234, x235, x236, x237, x238, x239,
                x240, x241, x242, x243, x244, x245, x246, x247, x248, x249,
                x250,             x253, x254, x255, x256, x257, x258, x259,
                x260,                   x264, x265, x266, x267, x268, x269,
                x270, x271, x272, x273, x274, x275, x276, x277, x278, x279,
                x280, x281, x282, x283, x284, x285, x286, x287, x288, x289,
                x290, x291, x292, x293, x294, x295, x296, x297, x298, x299,
                x300, x301, x302, x303, x304, x305, x306, x307, x308, x309,
                x310, x311, x312, x313, x314, x315,             x318, x319,
                x320, x321, x322, x323, x324,       x326, x327, x328, x329,
                x330, x331, x332, x333, x334, x335, x336, x337, x338, x339,
                x340, x341, x342, x343, x344, x345, x346, x347, x348, x349,
                x350, x351, x352, x353, x354, x355, x356, x357, x358      ,
                      x361, x362,       x364, x365, x366, x367            ,
                      x371, x372, x373,                   x377, x378, x379,
                                  x383, x384,       x386, x387, x388, x389;

  static double  j0,  j1,  j2,  j3,  j4,  j5,  j6,  j7,  j8,  j9,
                j10, j11, j12, j13, j14, j15, j16, j17, j18, j19,
                j20, j21, j22, j23, j24, j25, j26, j27, j28, j29,
                j30, j31;
  // clang-format on
  if (needsParamUpdate) {
    x0 = std::pow(rx, 2);
    x1 = std::pow(ry, 2);
    x2 = std::pow(rz, 2);
    x3 = std::sqrt(x0 + x1 + x2);
    x4 = std::cos(x3);
    x5 = x0 * x3;
    x6 = x4 * x5;
    x7 = x1 * x3;
    x8 = x4 * x7;
    x9 = std::pow(x4, 2);
    x10 = x2 * x3;

    x11 = x10 * x9;
    x12 = std::sin(x3);
    x13 = std::pow(x12, 2);
    x14 = x10 * x13;
    x15 = x11 + x14;
    x16 = x15 + x6 + x8;
    x17 = rz * x3;
    x18 = x17 * x9;
    x19 = rx * x18;

    x20 = x13 * x17;
    x21 = rx * x20;
    x22 = ry * x3;
    x23 = ty * x22;
    x24 = rz * x23;
    x25 = x17 * x4;
    x26 = rx * x25;
    x27 = tx * x26;
    x28 = x24 * x4;
    x29 = std::pow(rx, 3);

    x30 = x12 * x29;
    x31 = x1 * x12;
    x32 = rx * x31;
    x33 = ty * x32;
    x34 = x12 * x2;
    x35 = rx * x34;
    x36 = ty * x35;
    x37 = -ty * x30 + tz * x6 + x13 * x5 - x33 - x36 + x5 * x9;
    x38 = std::pow(ry, 3);
    x39 = x12 * x38;

    x40 = x0 * x12;
    x41 = ry * x40;
    x42 = tx * x41;
    x43 = ry * x34;
    x44 = tx * x43;
    x45 = x42 + x44;
    x46 = tx * x39 + tz * x8 + x13 * x7 + x45 + x7 * x9;
    x47 = tx * x19 + tx * x21 + tz * x11 + tz * x14 + x13 * x24 + x15 + x24 * x9 - x27 - x28 + x37
      + x46;
    x48 = 1.0 / x47;
    x49 = -x26;

    x50 = x39 + x41 + x43;
    x51 = x19 + x21;
    x52 = x49 + x50 + x51;
    x54 = x22 * x4;
    x55 = rz * x54;
    x56 = -x55;
    x57 = -x35;
    x58 = -x30;
    x59 = -x32;

    x60 = x58 + x59;
    x61 = x57 + x60;
    x62 = x22 * x9;
    x63 = rz * x62;
    x64 = x13 * x22;
    x65 = rz * x64;
    x66 = x63 + x65;
    x67 = x56 + x61 + x66;

    x71 = x10 * x4;
    x72 = tz * x71;
    x73 = x71 + x8;
    x74 = tz * x7 - x24 + x28 + x37 + x72 + x73;
    x75 = std::pow(rz, 3);
    x76 = x12 * x75;
    x77 = ty * x76;
    x78 = rx * x23;
    x79 = rz * x40;

    x80 = ty * x79;
    x81 = rz * x31;
    x82 = ty * x81;
    x83 = -x82;
    x84 = -x43;
    x85 = -x39;
    x86 = -x41;
    x87 = x85 + x86;
    x88 = x84 + x87;
    x89 = x49 + x88;

    x90 = -tx * x6 - tx * x7 - tx * x71 - x4 * x78 + x51 - x77 + x78 - x80 + x83 + x89;
    x91 = tz * x76;
    x92 = tx * x32;
    x93 = tx * x22;
    x94 = x76 + x79 + x81;
    x95 = tx * x35;
    x96 = tx * x30 + x95;
    x97 = tz * x79;
    x98 = tz * x81;
    x99 = x97 + x98;

    x100 = rx * x54;
    x101 = -x100;
    x102 = tz * x22;
    x103 = rx * x102;
    x104 = rx * x62 + rx * x64 + x101 + x103 * x4 - x103;
    x105 = rz * x93 - tx * x55 + x104 + x91 + x92 + x94 + x96 + x99;
    x107 = tx * x17;
    x108 = x6 + x71;
    x109 = -rx * x107 + tz * x5 + x108 + x27 + x46 + x72;

    x110 = tx * x81;
    x111 = x30 + x32 + x35;
    x112 = x111 + x56;
    x113 = tx * x79;
    x114 = tx * x76 + x113;
    x115 = rx * x93 - tx * x100 - ty * x5 - ty * x71 - ty * x8 + x110 + x112 + x114 + x66;
    x116 = ty * x39;
    x117 = ty * x17;
    x118 = ty * x41;
    x119 = -x76;

    x120 = -x79;
    x121 = -x81;
    x122 = x120 + x121;
    x123 = x119 + x122;
    x124 = -x91;
    x125 = x124 - x98;
    x126 = ty * x43;
    x127 = -x126;
    x128 = x127 - x97;
    x129 = rx * x117 - ty * x26 + x104 - x116 - x118 + x123 + x125 + x128;

    x131 = 1.0 / x3;
    x132 = x131 * x4;
    x133 = rx * x1;
    x134 = x132 * x133;
    x135 = x131 * x29;
    x136 = x135 * x9;
    x137 = x13 * x135;
    x138 = tz * x30;
    x139 = 3 * x40;

    x140 = ty * x139;
    x141 = rx * x3;
    x142 = x141 * x9;
    x143 = 2 * x142;
    x144 = x13 * x141;
    x145 = 2 * x144;
    x146 = x131 * x133;
    x147 = tz * x146;
    x148 = x135 * x4;
    x149 = tz * x148;

    x150 = std::pow(rx, 4) * x132;
    x151 = ty * x150;
    x152 = tz * x141;
    x153 = x152 * x4;
    x154 = 2 * x153;
    x155 = rx * ry;
    x156 = rz * x155;
    x157 = ty * x131;
    x158 = x156 * x157;
    x159 = rx * x2;

    x160 = x132 * x159;
    x161 = tz * x160 - tz * x35 + x160 + x57;
    x162 = ty * x31;
    x163 = x0 * x132;
    x164 = x163 * x2;
    x165 = ty * x164;
    x166 = -x162 - x165;
    x167 = ty * x34;
    x168 = x1 * x163;
    x169 = ty * x168;

    x170 = -x167 - x169;
    x171 = x12 * x156;
    x172 = ty * x171;
    x173 = x132 * x156;
    x174 = ty * x173;
    x175 = -x172 + x174;
    x176 = x0 * x131;
    x177 = ry * ty;
    x178 = x176 * x177;
    x179 = tx * x131;

    x180 = x133 * x179;
    x181 = x23 * x4;
    x182 = tx * x148;
    x183 = x141 * x4;
    x184 = 2 * x183;
    x185 = tx * x160;
    x186 = rz * x163;
    x187 = rz * x176;
    x188 = x187 * x9;
    x189 = x13 * x187;

    x190 = x188 + x189;
    x191 = x18 + x20 - x25;
    x192 = -x186 + x190 + x191 + x79;
    x193 = 2 * x12;
    x194 = x155 * x193;
    x195 = x132 * x38;
    x196 = rx * x195;
    x197 = ry * x148;
    x198 = x132 * x2;
    x199 = x155 * x198;

    x200 = -x194 - x196 - x197 - x199;
    x201 = rz * x193;
    x202 = rx * x201;
    x203 = ty * x202;
    x204 = x132 * x75;
    x205 = rx * x204;
    x206 = ty * x205;
    x207 = rz * x148;
    x208 = ty * x207;
    x209 = rz * x134;

    x210 = ty * x209;
    x211 = -x203 - x206 - x208 - x210;
    x212 = tz * x202;
    x213 = tz * x205;
    x214 = rz * x149;
    x215 = tz * x209;
    x216 = ry * x176;
    x217 = x216 * x9;
    x218 = x13 * x216;
    x219 = tz * x41;

    x220 = ry * x163;
    x221 = tz * x220;
    x222 = x217 + x218 - x219 + x221;
    x223 = tx * x34;
    x224 = tx * x168;
    x225 = x223 + x224;
    x226 = tx * x31;
    x227 = x156 * x179;
    x228 = tx * x171;
    x229 = tx * x173;

    x230 = tx * x164 + x202 + x205 + x207 + x209 + x226 + x227 + x228 - x229;
    x231 = x102 * x4;
    x232 = tz * x216;
    x233 = -x54 + x62 + x64;
    x234 = -x102 - x220 + x231 - x232 + x233 + x41;
    x235 = std::pow(x47, -2);
    x236 = tz * x32;
    x237 = x146 * x9;
    x238 = x13 * x146;
    x239 = x131 * x159;
    x240 = x239 * x9;

    x241 = x13 * x239;
    x242 = tx * x194;
    x243 = tx * x196;
    x244 = tz * x134;
    x245 = tx * x197;
    x246 = tx * x199;
    x247 = tx * x25;
    x248 = tx * x186;
    x249 = -x113 + x247 + x248;

    x250 = x235
      * (-tx * x18 - tx * x188 - tx * x189 - tx * x20 - tz * x240 - tz * x241 - x13 * x158 - x136
         - x137 + x138 + x140 - x143 - x145 - x149 + x151 - x154 - x158 * x9 + x162 + x165 + x167
         + x169 + x175 + x236 - x237 - x238 - x240 - x241 - x242 - x243 - x244 - x245 - x246
         + x249);
    x253 = x240 + x241;
    x254 = x148 + x184;
    x255 = x131 * x9;
    x256 = x13 * x131;
    x257 = x156 * x255 + x156 * x256 + x171 - x173;
    x258 = -x168 + x257 - x34;
    x259 = -x164 - x31;

    x260 = x194 + x196 + x197 + x199;
    x264 = x131 * x38;
    x265 = rz * x1;
    x266 = x157 * x265;
    x267 = x195 + 2 * x54;
    x268 = ty * x25;
    x269 = x132 * x265;

    x270 = ty * x269;
    x271 = x268 + x270 + x83;
    x272 = ry * x198;
    x273 = tz * x272 - tz * x43 + x272 + x84;
    x274 = ty * x194;
    x275 = ty * x196;
    x276 = x148 * x177;
    x277 = ty * x199;
    x278 = x222 - x274 - x275 - x276 - x277;
    x279 = 3 * x31;

    x280 = std::pow(ry, 4) * x132;
    x281 = -tx * x272;
    x282 = -tx * x220 - ty * x134 + x33;
    x283 = ty * x141;
    x284 = x1 * x198;
    x285 = rz * x195;
    x286 = -ty * x285 - x177 * x186 - x177 * x201 - x177 * x204 - x283 * x4 + x283 - x284 - x40;
    x287 = x179 * x265;
    x288 = ry * x201;
    x289 = tz * x288;

    x290 = ry * x204;
    x291 = tz * x290;
    x292 = tz * x285;
    x293 = ry * x186;
    x294 = tz * x293;
    x295 = x285 + x288 + x290 + x293;
    x296 = -x236 + x237 + x238 + x244;
    x297 = x242 + x243 + x245 + x246 + x296;
    x298 = x142 + x144 - x183;
    x299 = -x134 - x147 - x152 + x153 + x298 + x32;

    x300 = tz * x39;
    x301 = tx * x40;
    x302 = x264 * x9;
    x303 = x13 * x264;
    x304 = 3 * x226;
    x305 = 2 * x62;
    x306 = 2 * x64;
    x307 = ry * x2;
    x308 = x255 * x307;
    x309 = x256 * x307;
    x310 = tx * x280;
    x311 = tz * x195;
    x312 = 2 * x231;
    x313 = tx * x284;
    x314 = -x228 + x229;
    x315 = x235
      * (-ty * x18 - ty * x20 - tz * x308 - tz * x309 - x13 * x227 - x13 * x266 - x217 - x218 + x219
         - x221 - x223 - x224 - x227 * x9 - x266 * x9 + x271 + x274 + x275 + x276 + x277 + x300
         - x301 - x302 - x303 - x304 - x305 - x306 - x308 - x309 - x310 - x311 - x312 - x313
         + x314);
    x318 = x308 + x309;
    x319 = x168 + x257 + x34;
    x320 = x284 + x40;
    x321 = x255 * x265;
    x322 = x256 * x265;
    x323 = x321 + x322;
    x324 = x191 - x269 + x323 + x81;
    x326 = tz * x131;
    x327 = x177 * x2;
    x328 = tz * x186;
    x329 = x177 * x198;
    x330 = x181 + x329;
    x331 = 2 * x25;
    x332 = tz * x204 + tz * x331 + x119 + x204 + x331;
    x333 = std::pow(rz, 4) * x132;
    x334 = -x160 + x253 + x298 + x35;
    x335 = -ty * x284 - ty * x40 + x158 + x172 - x174 - x285 - x288 - x290 - x293;
    x336 = 3 * x34;
    x337 = tz * x40;
    x338 = tz * x31;
    x339 = tz * x336;
    x340 = tz * x333;
    x341 = tz * x164;
    x342 = tz * x284;
    x343 = rz * x182 + tx * x202 + tx * x205 + tx * x209 - tx * x54 + x164 + x31 + x93;
    x344 = -tz * x171 + tz * x173 - x156 * x326 + x257;
    x345 = x131 * x75;
    x346 = x345 * x9;
    x347 = x13 * x345;
    x348 = 2 * x18;
    x349 = 2 * x20;
    x350 = tx * x288;
    x351 = x159 * x179;
    x352 = tx * x290;
    x353 = tx * x285;
    x354 = tz * x269;
    x355 = ry * x248;
    x356 = tx * x183;
    x357 = x185 + x356 - x95;
    x358 = x235
      * (-tx * x142 - tx * x144 - tz * x346 - tz * x347 - tz * x348 - tz * x349 + x127 - x13 * x23
         - x13 * x351 - x188 - x189 + x203 + x206 + x208 + x210 - x23 * x9 - x255 * x327
         - x256 * x327 - x321 - x322 - x328 + x330 - x346 - x347 - x348 - x349 - x350 - x351 * x9
         - x352 - x353 - x354 - x355 + x357 + x99);
    x361 = x233 - x272 + x318 + x43;
    x362 = -x202 - x205 - x207 - x209;
    x364 = -x6;
    x365 = -x71;
    x366 = rz * x22;
    x367 = x235 * (-x19 - x21 + x26 + x88);
    x371 = rx * x22;
    x372 = x101 + x371;
    x373 = x235 * (x111 + x55 - x63 - x65);
    x377 = x100 - x371;
    x378 = -x8;
    x379 = x235 * (-x11 - x14 + x364 + x378);
    x383 = -ty * x160 + x36;
    x384 = ty * x187;
    x386 = x301 + x313;
    x387 = tx * x141;
    x388 = x350 + x352 + x353 + x355;
    x389 = rx * x17;

    j0 = x250 * x90
      + x48
        * (-tx * x184 + x118 - x163 * x177 + x178 - x180 - x181 - x182 - x185 + x192 + x200 + x211 + x23 + x96);
    j1 = x134 + x136 + x137 - x138 - x140 + x143 + x145 + x147 + x149 - x151 + x154 - x158 + x161
      + x166 + x170 + x175 + x59;
    j2 = tx * x139 + tx * x150 + x212 + x213 + x214 + x215 + x222 + x225 + x230 + x234;
    j3 = x315 * x90
      + x48 * (-tx * x264 + ty * x146 + x258 - x279 - x280 + x281 + x282 + x286 + x45 - 2 * x93);
    j4 = tz * x264 + 2 * x102 - x117 - x266 + x267 + x271 + x273 + x278 + x85;
    j5 = -tx * x269 + x107 + x110 - x247 + x287 + x289 + x291 + x292 + x294 + x295 + x297 + x299;
    j6 = x358 * x90
      + x48
        * (-tx * x204 - ty * x333 + x114 + x166 - 3 * x167 - 2 * x247 - x248 - x287 + x334 + x335);
    j7 = x121 + x124 + x128 - x131 * x327 + x190 + x211 - x23 + x265 * x326 + x269 + x328 + x330
      + x332;
    j8 = x179 * x307 + x281 + x320 + x333 + x336 + x337 + x338 + x339 + x340 + x341 + x342 + x343
      + x344 + x44;
    j9 = x367 * x90 + x48 * (x364 + x365 - x7);
    j10 = x112 + x366;
    j11 = x373 * x90 + x48 * (x123 + x372);
    j12 = -x366 + x55 + x61;
    j13 = x379 * x90;
    j14 = x108 + x7;
    j15 = x377 + x94;
    j16 = x115 * x250
      + x48 * (tx * x216 - ty * x135 + x139 + x150 + x282 - 2 * x283 + x319 + x343 + x383 + x42);
    j17 = -ty * x186 + x117 - x212 - x213 - x214 - x215 + x234 - x268 + x278 + x362 + x384 + x80;
    j18 = -tx * x187 + tz * x135 - x107 + 2 * x152 + x161 + x249 + x254 + x297 + x58;
    j19 = x115 * x315
      + x48
        * (-tx * x134 - ty * x195 + x116 + x126 - x178 + x180 - 2 * x181 + x260 + x324 - x329 - x356 + x387 + x388 + x92);
    j20 = -ty * x280 - 3 * x162 + x170 - x289 - x291 - x292 - x294 + x296 + x299 + x335;
    j21 = x220 + x225 - x227 + x232 + x273 - x300 + x302 + x303 + x304 + x305 + x306 + x310 + x311
      + x312 + x314 + x386 + x86;
    j22 = x115 * x358
      + x48
        * (tx * x333 - ty * x204 + 3 * x223 + x230 - 2 * x268 - x270 + x361 - x384 + x386 + x77 + x82);
    j23 =
      ty * x239 + x259 + x286 - x333 - x336 - x337 - x338 - x339 - x340 - x341 - x342 + x344 + x383;
    j24 = tz * x187 + x120 + x125 + x186 + x323 + x332 - x351 + x354 + x357 - x387 + x388;
    j25 = x115 * x367 + x48 * (x372 + x94);
    j26 = x26 - x389 + x50;
    j27 = x115 * x373 + x48 * (x365 + x378 - x5);
    j28 = x389 + x89;
    j29 = x115 * x379;
    j30 = x123 + x377;
    j31 = x5 + x73;
  }

  double x = x_;
  double y = y_;
  double u = u_;
  double v = v_;
  double w = w_;

  const double x53 = x48 * x;
  const double x68 = x48 * y;
  const double x69 = x16 * x48 + x52 * x53 + x67 * x68;
  const double x70 = 1.0 / x69;
  const double x106 = x105 * x68 + x48 * x90 + x53 * x74;
  const double x130 = x109 * x68 + x115 * x48 + x129 * x53;
  const double x251 = x250 * x;
  const double x252 = x250 * y;
  const double x261 = -x16 * x250 - x251 * x52 - x252 * x67 - x48 * (x134 + x253 + x254 + x60)
    - x53 * (x192 + x260) - x68 * (-x139 - x150 + x258 + x259);
  const double x262 = std::pow(x69, -2);
  const double x263 = x106 * x262;
  const double x316 = x315 * x;
  const double x317 = x315 * y;
  const double x325 = -x16 * x315 - x316 * x52 - x317 * x67 - x48 * (x220 + x267 + x318 + x87)
    - x53 * (x279 + x280 + x319 + x320) - x68 * (x200 + x324);
  const double x359 = x358 * x;
  const double x360 = x358 * y;
  const double x363 = -x16 * x358 - x359 * x52 - x360 * x67
    - x48 * (x122 + x186 + x269 + x346 + x347 + x348 + x349) - x53 * (x295 + x334)
    - x68 * (x361 + x362);
  const double x368 = x367 * x;
  const double x369 = x367 * y;
  const double x370 = -x16 * x367 - x368 * x52 - x369 * x67;
  const double x374 = x373 * x;
  const double x375 = x373 * y;
  const double x376 = -x16 * x373 - x374 * x52 - x375 * x67;
  const double x380 = x379 * x;
  const double x381 = x379 * y;
  const double x382 = -x16 * x379 - x380 * x52 - x381 * x67;
  const double x385 = x130 * x262;

  residuals[0] = w * (u - x106 * x70);
  residuals[1] = w * (v - x130 * x70);

  if (jacobians == NULL) {
    return true;
  }
  double *jacobian = jacobians[0];
  if (jacobian == NULL) {
    return true;
  }

  jacobian[0] = w * (-x261 * x263 - x70 * (x105 * x252 + x251 * x74 + j0 + x53 * j1 + x68 * j2));
  jacobian[1] = w * (-x263 * x325 - x70 * (x105 * x317 + x316 * x74 + j3 + x53 * j4 + x68 * j5));
  jacobian[2] = w * (-x263 * x363 - x70 * (x105 * x360 + x359 * x74 + j6 + x53 * j7 + x68 * j8));
  jacobian[3] = w * (-x263 * x370 - x70 * (x105 * x369 + j9 + x368 * x74 + x68 * j10));
  jacobian[4] = w * (-x263 * x376 - x70 * (x105 * x375 + j11 + x374 * x74 + x53 * j12));
  jacobian[5] = w * (-x263 * x382 - x70 * (x105 * x381 + j13 + x380 * x74 + x53 * j14 + x68 * j15));
  jacobian[6] =
    w * (-x261 * x385 - x70 * (x109 * x252 + j16 + x129 * x251 + x53 * j17 + x68 * j18));
  jacobian[7] =
    w * (-x325 * x385 - x70 * (x109 * x317 + j19 + x129 * x316 + x53 * j20 + x68 * j21));
  jacobian[8] =
    w * (-x363 * x385 - x70 * (x109 * x360 + j22 + x129 * x359 + x53 * j23 + x68 * j24));
  jacobian[9] = w * (-x370 * x385 - x70 * (x109 * x369 + j25 + x129 * x368 + x68 * j26));
  jacobian[10] = w * (-x376 * x385 - x70 * (x109 * x375 + j27 + x129 * x374 + x53 * j28));
  jacobian[11] =
    w * (-x382 * x385 - x70 * (x109 * x381 + j29 + x129 * x380 + x53 * j30 + x68 * j31));

  return true;
}

PoseEstimationAnalytic::PoseEstimationAnalytic(
  double x, double y, double z, const ObservedPoint &pt, double scale)
    : x_(x), y_(y), z_(z), u_(pt.position.x()), v_(pt.position.y()), scale_(scale) {
  w_ = computeObservedPointWeight(pt);
}

// See implementation in analytical-residual/pose-estimation.py
// There are some improvements that can be done in that script that hasn't been done
bool PoseEstimationAnalytic::Evaluate(
  double const *const *c, double *residuals, double **jacobians) const {
  static bool first = true;
  static double rx, ry, rz, tx, ty, tz, theta2;
  const int needsParamUpdate = first + (c[0][0] != rx) + (c[0][1] != ry) + (c[0][2] != rz)
    + (c[0][3] != tx) + (c[0][4] != ty) + (c[0][5] != tz);

  first = false;
  rx = c[0][0];
  ry = c[0][1];
  rz = c[0][2];
  tx = c[0][3];
  ty = c[0][4];
  tz = c[0][5];
  theta2 = rx * rx + ry * ry + rz * rz;
  if (theta2 < 1e-6) {
    const double x = x_;
    const double y = y_;
    const double z = z_;
    const double u = u_;
    const double v = v_;
    const double w = w_;
    const double x0 = std::pow(w, 2);
    const double x1 = rx * y - ry * x + tz + z;
    const double x2 = ((x1 < 0) ? (10000 * x0) : (x0));
    const double x3 = ry * z - rz * y + tx + x;
    const double x4 = scale_ / x1;
    const double x5 = -scale_ * u + x3 * x4;
    const double x6 = std::sqrt(x2 * std::pow(x5, 2) + 2.5000000000000001e-5);
    const double x7 = std::sqrt(x6 - 0.0050000000000000001);
    const double x8 = -rx * z + rz * x + ty + y;
    const double x9 = -scale_ * v + x4 * x8;
    const double x10 = std::sqrt(x2 * std::pow(x9, 2) + 2.5000000000000001e-5);
    const double x11 = std::sqrt(x10 - 0.0050000000000000001);
    const double x12 = x5 / (x6 * x7);
    const double x13 = scale_ * std::pow(x1, -2);
    const double x14 = 0.035355339059327376 * x2;
    const double x15 = x13 * x14;
    const double x16 = x12 * x15 * x3;
    const double x17 = 2 * x4 * z;
    const double x18 = 2 * x13;
    const double x19 = 0.017677669529663688 * x2;
    const double x20 = x14 * x4;
    const double x21 = x12 * x20;
    const double x22 = x9 / (x10 * x11);
    const double x23 = x15 * x22 * x8;
    const double x24 = x20 * x22;
    residuals[0] = 0.070710678118654752 * x7 + 1.0e-10;
    residuals[1] = 0.070710678118654752 * x11 + 1.0e-10;

    if (jacobians == NULL) {
      return true;
    }
    double *jacobian = jacobians[0];
    if (jacobian == NULL) {
      return true;
    }

    if ((x6 * x7) <= SMALL_DOUBLE) {
      jacobian[0] = 0;
      jacobian[1] = 0;
      jacobian[2] = 0;
      jacobian[3] = 0;
      jacobian[5] = 0;
    } else {
      jacobian[0] = -x16 * y;
      jacobian[1] = x12 * x19 * (x17 + x18 * x3 * x);
      jacobian[2] = -x21 * y;
      jacobian[3] = x21;
      jacobian[5] = -x16;
    }

    jacobian[4] = 0;
    jacobian[9] = 0;

    if ((x10 * x11) <= SMALL_DOUBLE) {
      jacobian[6] = 0;
      jacobian[7] = 0;
      jacobian[8] = 0;
      jacobian[10] = 0;
      jacobian[11] = 0;
    } else {
      jacobian[6] = x19 * x22 * (-x17 - x18 * x8 * y);
      jacobian[7] = x23 * x;
      jacobian[8] = x24 * x;
      jacobian[10] = x24;
      jacobian[11] = -x23;
    }
    return true;
  }

  static double x1, x2, x3, x4, x5, x6, x7, x8, x19, x20, x39, x40, x45, x46, x48, x67, x68, x70,
    x73, x78, x79;
  if (needsParamUpdate) {
    x1 = std::pow(rx, 2);
    x2 = std::pow(ry, 2);
    x3 = std::pow(rz, 2);
    x4 = x1 + x2 + x3;
    x5 = std::sqrt(x4);
    x6 = std::cos(x5);
    x7 = std::sin(x5);
    x8 = 1.0 / x5;
    x19 = 1.0 - x6;
    x20 = x19 * x8;
    x39 = std::pow(x4, -3.0 / 2.0);
    x40 = rx * x39;
    x45 = x6 * x8;
    x46 = rx * x45;
    x48 = x1 * x39;
    x67 = ry * rz;
    x68 = x39 * x67;
    x70 = x2 * x39;
    x73 = ry * x45;
    x78 = rz * x45;
    x79 = x3 * x39;
  }

  const double x = x_;
  const double y = y_;
  const double z = z_;
  const double u = u_;
  const double v = v_;
  const double w = w_;

  const double x0 = std::pow(w, 2);
  const double x9 = x8 * y;
  const double x10 = rx * x9;
  const double x11 = x8 * x;
  const double x12 = ry * x11;
  const double x13 = x10 - x12;
  const double x14 = rx * x11;
  const double x15 = ry * x9;
  const double x16 = x8 * z;
  const double x17 = rz * x16;
  const double x18 = x14 + x15 + x17;
  const double x21 = x18 * x20;
  const double x22 = rz * x21 + tz + x13 * x7 + x6 * z;
  const double x23 = ((x22 < 0) ? (10000 * x0) : (x0));
  const double x24 = ry * x16;
  const double x25 = rz * x9;
  const double x26 = x24 - x25;
  const double x27 = rx * x21 + tx + x26 * x7 + x6 * x;
  const double x28 = scale_ / x22;
  const double x29 = -scale_ * u + x27 * x28;
  const double x30 = std::sqrt(x23 * std::pow(x29, 2) + 2.5000000000000001e-5);
  const double x31 = std::sqrt(x30 - 0.0050000000000000001);
  const double x32 = rz * x11;
  const double x33 = rx * x16;
  const double x34 = x32 - x33;
  const double x35 = ry * x21 + ty + x34 * x7 + x6 * y;
  const double x36 = -scale_ * v + x28 * x35;
  const double x37 = std::sqrt(x23 * std::pow(x36, 2) + 2.5000000000000001e-5);
  const double x38 = std::sqrt(x37 - 0.0050000000000000001);
  const double x41 = x40 * y;
  const double x42 = rz * x41;
  const double x43 = x40 * z;
  const double x44 = ry * x43;
  const double x47 = x18 * x7 / x4;
  const double x49 = x18 * x19;
  const double x50 = -ry * x41;
  const double x51 = rz * x43;
  const double x52 = x20 * (x11 - x48 * x + x50 - x51);
  const double x53 = 2 * x28;
  const double x54 = x40 * x;
  const double x55 = ry * x54;
  const double x56 = rx * x47;
  const double x57 = rz * x56;
  const double x58 = x40 * x49;
  const double x59 = rz * x58;
  const double x60 = -rz * x52 - x13 * x46 + x33 * x7 - x57 + x59 - x7 * (-x48 * y + x55 + x9);
  const double x61 = scale_ * std::pow(x22, -2);
  const double x62 = 2 * x61;
  const double x63 = x27 * x62;
  const double x64 = 0.017677669529663688 * x23;
  const double x65 = x29 / (x30 * x31);
  const double x66 = x64 * x65;
  const double x69 = x68 * y;
  const double x71 = -x68 * z;
  const double x72 = x20 * (-x55 - x70 * y + x71 + x9);
  const double x74 = ry * x56 - ry * x58;
  const double x75 = x47 * x67;
  const double x76 = x49 * x68;
  const double x77 = -rz * x72 - x13 * x73 + x24 * x7 - x7 * (-x11 + x50 + x70 * x) - x75 + x76;
  const double x80 = -rz * x54;
  const double x81 = x20 * (x16 - x69 - x79 * z + x80);
  const double x82 = x68 * x;
  const double x83 =
    -rz * x81 - x13 * x78 + x17 * x7 - x21 - x3 * x47 + x49 * x79 - x7 * (-x42 + x82);
  const double x84 = 0.035355339059327376 * x23;
  const double x85 = x28 * x84;
  const double x86 = x61 * x84;
  const double x87 = x35 * x62;
  const double x88 = x36 / (x37 * x38);
  const double x89 = x64 * x88;

  residuals[0] = 0.070710678118654752 * x31 + 1.0e-10;
  residuals[1] = 0.070710678118654752 * x38 + 1.0e-10;

  if (jacobians == NULL) {
    return true;
  }
  double *jacobian = jacobians[0];
  if (jacobian == NULL) {
    return true;
  }

  jacobian[0] = x66
    * (x53 * (rx * x52 + x1 * x47 - x14 * x7 + x21 + x26 * x46 - x48 * x49 + x7 * (x42 - x44))
       + x60 * x63);
  jacobian[1] =
    x66 * (x53 * (rx * x72 - x12 * x7 + x26 * x73 + x7 * (x16 + x69 - x70 * z) + x74) + x63 * x77);
  jacobian[2] = x66
    * (x53 * (rx * x81 + x26 * x78 - x32 * x7 + x57 - x59 + x7 * (x71 + x79 * y - x9)) + x63 * x83);
  jacobian[3] = x65 * x85;
  jacobian[4] = 0;
  jacobian[5] = -x27 * x65 * x86;
  jacobian[6] =
    x89 * (x53 * (ry * x52 - x10 * x7 + x34 * x46 + x7 * (-x16 + x48 * z + x80) + x74) + x60 * x87);
  jacobian[7] = x89
    * (x53 * (ry * x72 - x15 * x7 + x2 * x47 + x21 + x34 * x73 - x49 * x70 + x7 * (x44 - x82))
       + x77 * x87);
  jacobian[8] = x89
    * (x53 * (ry * x81 - x25 * x7 + x34 * x78 + x7 * (x11 + x51 - x79 * x) + x75 - x76)
       + x83 * x87);
  jacobian[9] = 0;
  jacobian[10] = x85 * x88;
  jacobian[11] = -x35 * x86 * x88;
  return true;
}

}  // namespace c8
