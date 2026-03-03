// Copyright (c) 2024 Niantic, Inc.
// Original Author: Nicholas Butko (nbutko@nianticlabs.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "bezier.h",
  };
  deps = {
    "//c8:c8-log",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xc7bf8cba);

#include <cmath>
#include <complex>

#include "c8/c8-log.h"
#include "c8/geometry/bezier.h"

namespace c8 {

AnimationCurve BezierAnimation::DEFAULT_CURVE = {{{0.25f, 0.1f}, {0.25f, 1.0f}}};
AnimationCurve BezierAnimation::EASE_IN_CURVE = {{{0.4f, 0.0f}, {1.0f, 1.0f}}};
AnimationCurve BezierAnimation::EASE_IN_EASE_OUT_CURVE = {{{0.42f, 0.0f}, {0.58f, 1.0f}}};
AnimationCurve BezierAnimation::EASE_OUT_CURVE = {{{0.0f, 0.0f}, {0.58f, 1.0f}}};
AnimationCurve BezierAnimation::LINEAR_CURVE = {{{0.0f, 0.0f}, {1.0f, 1.0f}}};
AnimationCurve BezierAnimation::STRONG_EASE_CURVE = {{{0.0f, 0.0f}, {0.05f, 1.0f}}};

BezierAnimation::BezierAnimation(const AnimationCurve &curve)
    : curve_(curve),
      b_(curve[0].time),
      c_(curve[1].time),
      x0_(-3 * c_),
      x1_(-6 * b_ - x0_),
      x2_(3 * b_ + x0_ + 1),
      x3_(1.0 / x2_),
      x4_((1.0 / 3.0) * x1_ * x3_),
      x6_(std::pow(x2_, -2)),
      x7_(b_ * x1_ * x6_),
      x8_(std::pow(x1_, 3)),
      x9_(std::pow(x2_, -3)),
      x10_(-9 * b_ * x3_ + std::pow(x1_, 2) * x6_) {}

float BezierAnimation::at(float time) const {

  // See https://en.wikipedia.org/wiki/B%C3%A9zier_curve
  // Solution from sympy:
  //     from sympy.printing import cxxcode
  //     original_rhs = 3 * (1-t)**2 * t * b + 3 * (1 - t) * t**2 * c + t**3
  //     rhs = collect(expand(original_rhs), t)
  //     solutions = solve(Eq(v, rhs), t)
  //     sub_expressions, rewritten_expressions = cse(solutions)
  //     for var_name, expr in sub_expressions:
  //       print('auto', var_name, '=', cxxcode(expr),  ';')
  //     print('auto tc0 = {};'.format(cxxcode(rewritten_expressions[0])))
  //     print('auto tc1 = {};'.format(cxxcode(rewritten_expressions[1])))
  //     print('auto tc2 = {};'.format(cxxcode(rewritten_expressions[2])))

  // Solve for the curve parameter t at a give time.
  auto x5 = time * x3_;

  auto z0 = -27.0 / 2.0 * x5;
  auto z1 = -27.0 / 2.0 * x7_ + x8_ * x9_;

  auto z2 = -27 * x5 - 27 * x7_ + 2 * x8_ * x9_;
  auto z3 = std::pow(z2, 2);
  auto z4 = -4 * std::pow(x10_, 3);
  auto z5 = (1.0 / 2.0) * std::sqrt(std::complex(z4 + z3));

  auto x11 = std::pow(z0 + z1 + z5, 1.0 / 3.0);
  auto x12 = (1.0 / 3.0) * x11;
  auto x13 = (1.0 / 3.0) * x10_ / x11;
  auto x14 = (1.0 / 2.0) * std::sqrt(3) * std::complex(0.0, 1.0);
  auto x15 = -x14 - 1.0 / 2.0;
  auto x16 = x14 - 1.0 / 2.0;

  // Three solutions.
  auto tc0 = -x12 - x13 - x4_;
  auto tc1 = -x12 * x15 - x13 / x15 - x4_;
  auto tc2 = -x12 * x16 - x13 / x16 - x4_;

  // Pick between the solutions:
  // - Negative solutions are invalid.
  // - Solutions greater than 1 are invalid.
  // - Solutions with non-trivial imaginary parts are invalid.
  // Below we look for the least invalid solution.
  auto t0 = tc0.real();
  auto t1 = tc1.real();
  auto t2 = tc2.real();

  auto t0Neg = 0.0f - t0;
  auto t0Pos = t0 - 1.0f;
  auto t0Imag = std::abs(tc0.imag());
  auto t0Cost = std::max(std::max(t0Neg, t0Pos), t0Imag);

  auto t1Neg = 0.0f - t1;
  auto t1Pos = t1 - 1.0f;
  auto t1Imag = std::abs(tc1.imag());
  auto t1Cost = std::max(std::max(t1Neg, t1Pos), t1Imag);

  auto t2Neg = 0.0f - t2;
  auto t2Pos = t2 - 1.0f;
  auto t2Imag = std::abs(tc2.imag());
  auto t2Cost = std::max(std::max(t2Neg, t2Pos), t2Imag);

  auto t = t0;
  auto tCost = t0Cost;
  if (tCost > t1Cost) {
    t = t1;
    tCost = t1Cost;
  }
  if (tCost > t2Cost) {
    t = t2;
    tCost = t2Cost;
  }

  // Evaluate the curve at the given time.
  float u = 1.0 - t;
  float tt = t * t;
  float uu = u * u;
  float ttt = tt * t;

  return 3 * uu * t * curve_[0].value + 3 * u * tt * curve_[1].value + ttt;
}

}  // namespace c8
