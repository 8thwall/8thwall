// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include <array>
#include <iostream>

#include "c8/color-maps.h"

namespace c8 {

class Color {
public:
  // 8th Wall Style Guide colors
  static const Color PURPLE;
  static const Color LIGHT_PURPLE;
  static const Color BLACK;
  static const Color TRUE_BLACK;
  static const Color WHITE;

  static const Color DUSTY_VIOLET;
  static const Color EGGSHELL;
  static const Color MOONLIGHT;

  static const Color GRAY_01;
  static const Color GRAY_02;
  static const Color GRAY_03;
  static const Color GRAY_04;
  static const Color GRAY_05;

  static const Color BLUEBERRY;
  static const Color CHERRY;
  static const Color MANGO;
  static const Color MINT;

  static const Color PURPLE_POP;

  // Legacy style 8th Wall Style Guide Colors
  static const Color CANARY;
  static const Color OFF_WHITE;
  static const Color PURPLE_GRAY;
  static const Color PUMICE_GRAY;
  static const Color STONE_GRAY;
  static const Color DARK_GRAY;

  // color.adobe.com
  static const Color GREEN;
  static const Color MATCHA;
  static const Color PINK;
  static const Color YELLOW;
  static const Color VIBRANT_BLUE;
  static const Color VIBRANT_PINK;
  static const Color VIBRANT_YELLOW;
  static const Color DULL_BLUE;
  static const Color DULL_PINK;
  static const Color DULL_PURPLE;
  static const Color DULL_YELLOW;
  static const Color DARK_GREEN;
  static const Color DARK_MATCHA;
  static const Color DARK_PURPLE;

  // others
  static const Color ORANGE;
  static const Color BURNT_ORANGE;
  static const Color BLUE;
  static const Color RED;
  static const Color DARK_BLUE;
  static const Color DARK_RED;
  static const Color DARK_YELLOW;
  static const Color CLEAR;

  // Style guide gradients
  static const Color PURPLE_GRADIENT(float alpha) { return gradient(LIGHT_PURPLE, PURPLE, alpha); }
  static const Color BLACK_GRADIENT(float alpha) { return gradient(GRAY_05, BLACK, alpha); }
  static const Color GRAY_GRADIENT(float alpha) { return gradient(GRAY_04, GRAY_05, alpha); }
  static const Color PINK_GRAY_GRADIENT(float alpha) {
    return gradient(DUSTY_VIOLET, GRAY_02, alpha);
  }
  static const Color PALE_PINK_GRADIENT(float alpha) {
    return gradient(EGGSHELL, DUSTY_VIOLET, alpha);
  }
  static const Color POP_GRADIENT(float alpha) { return gradient(PURPLE_POP, PURPLE, alpha); }

  // Default constructor is 8th Wall Purple
  constexpr Color() : rgba_{{118, 117, 183, 255}} {}
  constexpr Color(uint8_t r, uint8_t g, uint8_t b) : rgba_{{r, g, b, 255}} {}
  constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : rgba_{{r, g, b, a}} {}

  constexpr uint8_t r() const { return rgba_[0]; }
  constexpr uint8_t g() const { return rgba_[1]; }
  constexpr uint8_t b() const { return rgba_[2]; }
  constexpr uint8_t a() const { return rgba_[3]; }

  constexpr Color alpha(uint8_t a) const { return Color(r(), g(), b(), a); }

  static Color gradient(Color a, Color b, float alpha);
  static Color blend(Color a, float aWt, Color b, float bWt);

  // Get a color from a color map by mapping a value in 0 to 1.
  static Color fromColorMap(const uint8_t (&map)[256][3], float value) {
    auto &c = map[static_cast<size_t>(value * 255)];
    return {c[0], c[1], c[2]};
  }

  // Get a color from the hot color map, which preserves luminance of gray scale values but adds
  // color (from black->red->yellow->white) in order to improve visual contrast.
  static Color hot(float value) { return fromColorMap(HOT_RGB_256, value); }

  // Get a color from the turbo color map, which is good at highlighting small differences in values
  // but doesn't show off order of magintude well.
  static Color turbo(float value) { return fromColorMap(TURBO_RGB_256, value); }

  // Get a color from the viridis color map, which is good at highlighting order of magnitude but
  // doesn't distinguish small values well. From purple -> blue -> green -> yellow.
  static Color viridis(float value) { return fromColorMap(VIRIDIS_RGB_256, value); }

  // Default move and copy constructors.
  Color(Color &&) = default;
  Color &operator=(Color &&) = default;
  Color(const Color &) = default;
  Color &operator=(const Color &) = default;

private:
  std::array<uint8_t, 4> rgba_;
};

// Gamma-aware mixing of two colors with an sRGB colorspace. This converts the
// color to linear-sRGB, applies a mixing alpha * a + (1 - alpha) * b, then
// converts back to gamma-expanded sRGB.
Color mixSRGB(Color a, Color b, float alpha);

inline bool operator==(const Color &l, const Color &r) {
  return l.r() == r.r() && l.g() == r.g() && l.b() == r.b() && l.a() == r.a();
}

inline bool operator!=(const Color &l, const Color &r) {
  return l.r() != r.r() || l.g() != r.g() || l.b() != r.b() || l.a() != r.a();
}

std::ostream &operator<<(std::ostream &out, const Color &c);

}  // namespace c8
