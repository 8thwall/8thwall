// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "color.h",
  };
  deps = {
    ":color-maps",
  };
}
cc_end(0x41a50b58);

#include <cmath>

#include "c8/color.h"

namespace c8 {

namespace {
static inline uint8_t clip(float x) { return x < 0 ? 0 : (x > 255 ? 255 : x); }
}  // namespace

const Color Color::PURPLE(118, 17, 183);
const Color Color::LIGHT_PURPLE(173, 80, 255);
const Color Color::BLACK(45, 46, 67);
const Color Color::TRUE_BLACK(0, 0, 0);
const Color Color::WHITE(255, 255, 255);

const Color Color::DUSTY_VIOLET(217, 208, 227);
const Color Color::EGGSHELL(242, 241, 243);
const Color Color::MOONLIGHT(249, 249, 250);

const Color Color::GRAY_01(235, 235, 241);
const Color Color::GRAY_02(213, 215, 228);
const Color Color::GRAY_03(181, 184, 208);
const Color Color::GRAY_04(128, 131, 162);
const Color Color::GRAY_05(70, 71, 102);

const Color Color::BLUEBERRY(87, 191, 255);
const Color Color::CHERRY(221, 0, 101);
const Color Color::MANGO(255, 200, 40);
const Color Color::MINT(0, 237, 175);

const Color Color::PURPLE_POP(200, 109, 215);

const Color Color::CANARY(252, 238, 33);
const Color Color::OFF_WHITE(243, 242, 244);
const Color Color::PURPLE_GRAY(218, 209, 228);
const Color Color::PUMICE_GRAY(68, 168, 186);
const Color Color::STONE_GRAY(129, 129, 153);
const Color Color::DARK_GRAY(70, 71, 102);

const Color Color::GREEN(25, 132, 73);
const Color Color::MATCHA(158, 183, 17);
const Color Color::PINK(234, 45, 181);
const Color Color::YELLOW(234, 204, 45);
const Color Color::VIBRANT_BLUE(20, 8, 196);
const Color Color::VIBRANT_PINK(196, 8, 143);
const Color Color::VIBRANT_YELLOW(195, 138, 38);
const Color Color::DULL_BLUE(75, 9, 206);
const Color Color::DULL_PINK(183, 9, 206);
const Color Color::DULL_PURPLE(81, 40, 69);
const Color Color::DULL_YELLOW(192, 185, 66);
const Color Color::DARK_GREEN(5, 106, 50);
const Color Color::DARK_MATCHA(90, 106, 0);
const Color Color::DARK_PURPLE(73, 21, 106);

const Color Color::ORANGE(255, 69, 50);
const Color Color::BURNT_ORANGE(203, 96, 21);
const Color Color::BLUE(109, 158, 235);
const Color Color::RED(204, 65, 37);
const Color Color::DARK_BLUE(29, 11, 226);
const Color Color::DARK_RED(151, 26, 0);
const Color Color::DARK_YELLOW(212, 148, 21);
const Color Color::CLEAR(0, 0, 0, 0);

Color Color::blend(Color a, float aWt, Color b, float bWt) {
  float wt = aWt + bWt;
  float aw = aWt / wt;
  float bw = bWt / wt;
  uint8_t r_ = clip(aw * a.r() + (bw * b.r()));
  uint8_t g_ = clip(aw * a.g() + (bw * b.g()));
  uint8_t b_ = clip(aw * a.b() + (bw * b.b()));
  uint8_t a_ = clip(aw * a.a() + (bw * b.a()));
  return Color(r_, g_, b_, a_);
}

Color Color::gradient(Color a, Color b, float alpha) {
  alpha = std::min(std::max(alpha, 0.0f), 1.0f);
  return blend(a, 1.0f - alpha, b, alpha);
}

Color mixSRGB(Color a, Color b, float alpha) {
  constexpr auto toLinear = [](uint8_t val) -> float {
    // sRGB -> Linear sRGB.
    return val < 11u ? val / 3294.6f : std::pow((val + 14.025f) / 269.025f, 2.4f);
  };
  constexpr auto toGamma = [](float val) -> uint8_t {
    // Linear sRGB -> sRGB.
    return static_cast<uint8_t>(std::round(
      val <= 0.0031308f ? 3294.6f * val : 269.025f * std::pow(val, 1.0f / 2.4f) - 14.025f));
  };

  float beta = 1.0 - alpha;
  return Color(
    toGamma(alpha * toLinear(a.r()) + beta * toLinear(b.r())),
    toGamma(alpha * toLinear(a.g()) + beta * toLinear(b.g())),
    toGamma(alpha * toLinear(a.b()) + beta * toLinear(b.b())),
    alpha * a.a() + beta * b.a());
}

std::ostream &operator<<(std::ostream &out, const Color &c) {
  return out << "(" << static_cast<int>(c.r()) << ", " << static_cast<int>(c.g()) << ", "
             << static_cast<int>(c.b()) << ", " << static_cast<int>(c.a()) << ")";
}

}  // namespace c8
