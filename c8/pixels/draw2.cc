// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "draw2.h",
  };
  deps = {
    ":embedded-drawing-font",
    ":pixel-transforms",
    ":pixels",
    "//c8:color",
    "//c8:color-maps",
    "//c8:hmatrix",
    "//c8:hpoint",
    "//c8:string-view",
    "//c8:vector",
    "//c8/geometry:mesh",
    "//c8/geometry:mesh-types",
    "@org_freetype_freetype2//:freetype2",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x69106930);

#include <algorithm>
#include <iostream>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <freetype/ftglyph.h>

#include "c8/color-maps.h"
#include "c8/pixels/draw2.h"
#include "c8/pixels/embedded-drawing-font.h"
#include "c8/pixels/pixel-transforms.h"
#include "c8/string-view.h"
#include "c8/geometry/mesh.h"
#include "c8/geometry/mesh-types.h"
#include "c8/stats/scope-timer.h"

namespace c8 {

namespace {

std::tuple<FT_Library, FT_Face> getFreetypeLibraryAndFace(float fontSizePts) {
  static FT_Library library = nullptr;
  static FT_Face face = nullptr;
  static int fontSize = 0;

  // Freetype expects font sizes in units of 1/64 points.
  int desiredFontSize = static_cast<int>(std::round(fontSizePts * 64.0f));

  if (library == nullptr) {
    auto error = FT_Init_FreeType(&library);
    if (error) {
      C8_THROW("Failed to initialize FreeType");
    }
  }

  if (face == nullptr) {
    auto error = FT_New_Memory_Face(
      library, embeddedNunitoRegularTtfData, embeddedNunitoRegularTtfSize, 0, &face);
    if (error) {
      C8_THROW("Unable to load font");
    }
  }

  if (desiredFontSize != fontSize) {
    auto error = FT_Set_Char_Size(
      face,
      0,               /* char_width in 1/64th of points  */
      desiredFontSize, /* char_height in 1/64th of points */
      72,              /* horizontal device resolution    */
      72);             /* vertical device resolution      */
    if (error) {
      C8_THROW("Error setting character size");
    }
    fontSize = desiredFontSize;
  }

  return {library, face};
}

void renderCharacter(
  RGBA8888PlanePixels dest, Color color, const FT_Bitmap *bitmap, int xOffset, int yOffset) {
  for (int y = 0; y < bitmap->rows; ++y) {
    for (int x = 0; x < bitmap->width; ++x) {
      uint8_t in = bitmap->buffer[x + y * bitmap->pitch];
      if (in == 0u) {
        // Fast skip for pixels without characters.
        continue;
      }

      int xx = x + xOffset;
      int yy = y + yOffset;
      if (xx < 0 || yy < 0 || xx >= dest.cols() || yy >= dest.rows()) {
        // Don't draw pixels outside the canvas.
        continue;
      }

      uint8_t *p = dest.pixels() + (xx << 2) + yy * dest.rowBytes();
      if (in == 255u) {
        // Set pure color pixels to their color.
        p[0] = color.r();
        p[1] = color.g();
        p[2] = color.b();
        p[3] = color.a();
      } else {
        // Set mixed pixels to their gamma-aware blend.
        float alpha = in / 255.0;
        Color c = mixSRGB(color, Color(p[0], p[1], p[2], p[3]), alpha);
        p[0] = c.r();
        p[1] = c.g();
        p[2] = c.b();
        p[3] = c.a();
      }
    }
  }
}

void renderText(
  StringView text, HPoint2 pt, float fontSize, Color color, RGBA8888PlanePixels dest) {

  auto [library, face] = getFreetypeLibraryAndFace(fontSize);

  // Convert x,y from c++ float to 26.6 fixed point float, and flip-Y in the process.
  FT_Pos pen_x = static_cast<FT_Pos>(std::round(pt.x() * 64.0f));
  FT_Pos pen_y = (dest.rows() << 6) - static_cast<FT_Pos>(std::round(pt.y() * 64.0f));

  for (int n = 0; n < text.size(); ++n) {
    // Load the glyph.
    auto error = FT_Load_Char(face, text[n], FT_LOAD_DEFAULT);
    if (error) {
      // Ignore errors.
      continue;
    }

    FT_Glyph glyph;
    error = FT_Get_Glyph(face->glyph, &glyph);
    if (error) {
      // Ignore errors.
      continue;
    }

    // Translate glyph.
    FT_Vector delta;
    // Equivalent to x - floor(x), yielding the fixed point fraction.
    delta.x = pen_x - (pen_x & -64);
    delta.y = pen_y - (pen_y & -64);
    if (glyph->format != FT_GLYPH_FORMAT_BITMAP) {
      error = FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, &delta, 1);
    }

    if (error) {
      // Ignore errors.
      continue;
    }

    // Rasterize the glyph.
    FT_BitmapGlyph bitmapGlyph = (FT_BitmapGlyph)glyph;
    renderCharacter(
      dest,
      color,
      &bitmapGlyph->bitmap,
      (pen_x >> 6) + bitmapGlyph->left,
      (dest.rows() - (pen_y >> 6)) - bitmapGlyph->top);

    // Increment the pen position. The Glyph advance member is in 16.16 fixed
    // point, so we shift it into 26.6 fixed point.
    pen_x += glyph->advance.x >> 10;
    pen_y += glyph->advance.y >> 10;

    FT_Done_Glyph(glyph);
  }
}

// Get the bounding box for a rendered string. Returns
// { HPoint2 topLeft, HPoint2, bottomRight}. The box height is sufficient to
// contain any possible character in the glyph, and the width is the distance
// from the left-most pixel position to the right-most position.
std::tuple<HPoint2, HPoint2> calculateTextWidth(StringView text, HPoint2 pt, float fontSize) {
  auto [library, face] = getFreetypeLibraryAndFace(fontSize);

  // Convert x,y from c++ float to 26.6 fixed point float, and flip-Y in the process.
  FT_Pos pen_x = static_cast<FT_Pos>(std::round(pt.x() * 64.0f));
  FT_Pos pen_y = static_cast<FT_Pos>(std::round(pt.y() * 64.0f));

  FT_Pos minX = pen_x;
  FT_Pos maxX = pen_x;
  const FT_Pos minY = pen_y - face->bbox.yMax;
  const FT_Pos maxY = pen_y - face->bbox.yMin;

  for (int n = 0; n < text.size(); ++n) {
    // Load the glyph.
    auto error = FT_Load_Char(face, text[n], FT_LOAD_DEFAULT);
    if (error) {
      // Ignore errors.
      continue;
    }

    FT_Glyph glyph;
    error = FT_Get_Glyph(face->glyph, &glyph);
    if (error) {
      // Ignore errors.
      continue;
    }

    // Translate glyph.
    FT_Vector delta;
    // Equivalent to x - floor(x), yielding the fixed point fraction.
    delta.x = pen_x - (pen_x & -64);
    FT_Glyph_Transform(glyph, 0, &delta);

    FT_BBox bbox;
    FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_GRIDFIT, &bbox);

    minX = std::min(minX, pen_x + bbox.xMin);
    maxX = std::max(maxX, pen_x + bbox.xMax);

    // Increment the pen position. The Glyph advance member is in 16.16 fixed
    // point, so we shift it into 26.6 fixed point.
    pen_x += glyph->advance.x >> 10;

    FT_Done_Glyph(glyph);
  }
  return {{static_cast<float>(minX) / 64.0f, static_cast<float>(minY) / 64.0f},
          {static_cast<float>(maxX) / 64.0f, static_cast<float>(maxY) / 64.0f}};
}

}  // namespace

void drawPoints3(const Vector<HPoint3> &pts, Color color, RGBA8888PlanePixels dest) {
  for (auto pt : pts) {
    drawPoint3(pt, color, dest);
  }
}

void drawPoint3(HPoint3 pt, Color color, RGBA8888PlanePixels dest) {
  if (pt.z() < 1e-2) {
    return;
  }
  float r = 5.0 / pt.z();
  r = std::max(std::min(r, 10.0f), 1.0f);
  drawPoint(pt.flatten(), r, Color::BLACK, dest);
  drawPoint(pt.flatten(), r - 0.5f, color, dest);
}

void drawPoints(Vector<HPoint2> pts, float radius, Color color, RGBA8888PlanePixels dest) {
  for (const auto &pt : pts) {
    drawPoint(pt, radius, color, dest);
  }
}

void drawPoints(
  Vector<HPoint2> pts, float radius, int thickness, Color color, RGBA8888PlanePixels dest) {
  for (const auto &pt : pts) {
    drawPoint(pt, radius, thickness, color, dest);
  }
}

void drawLine(HPoint2 fromPt, HPoint2 toPt, int w, Color color, RGBA8888PlanePixels dest) {
  // Inspired from but quite different from an anti-aliased line algorithm from Computer
  // Graphics, July 1991. Additions include gamma-correction, varying line
  // widths, and support for correct rendering of subpixel point locations.

  // Do our math in 26.6 fixed point.
  using Int26p6 = int32_t;
  constexpr auto floor26p6 = [](Int26p6 val) { return val & -64; };
  constexpr auto round26p6 = [=](Int26p6 val) { return floor26p6(val + 32); };
  constexpr auto fract = [=](Int26p6 val) { return val - floor26p6(val); };
  constexpr auto ifract = [=](Int26p6 val) { return 64 - fract(val); };

  Int26p6 width = w << 6;

  Int26p6 x0 = std::lround(fromPt.x() * 64.0f);
  Int26p6 y0 = std::lround(fromPt.y() * 64.0f);
  Int26p6 x1 = std::lround(toPt.x() * 64.0f);
  Int26p6 y1 = std::lround(toPt.y() * 64.0f);

  // Get a line that goes left to right, mostly horizontally.
  bool absSlopeGreaterThanOne = std::abs(y1 - y0) > std::abs(x1 - x0);
  Int26p6 corrRows = dest.rows() << 6;
  Int26p6 corrCols = dest.cols() << 6;

  if (absSlopeGreaterThanOne) {
    std::swap(x0, y0);
    std::swap(x1, y1);
    std::swap(corrRows, corrCols);
  }
  if (x0 > x1) {
    std::swap(x0, x1);
    std::swap(y0, y1);
  }

  const Int26p6 dx = x1 - x0;
  const Int26p6 dy = y1 - y0;
  const float slope =
    dx == 0.0f ? 1.0f : static_cast<float>(dy) / dx;  // dx == 0 iff y0 == y1 (the line is a point).

  // If needed, truncate the inputs to the (possibly rotated) image boundary.
  if (x0 > corrCols) {
    return;  // Leftward point is too right, we are out of the image.
  }
  if (x0 < 0) {  // Starting to the left of the image, truncate to image left.
    y0 += (0 - x0) * slope;
    x0 = 0;
  }
  if (y0 < 0) {  // Starting above the image, truncate to image top.
    if ((y0 + (corrCols - x0) * slope) < 0) {
      return;  // The whole line is above the image.
    }
    x0 += (0 - y0) / slope;
    y0 = 0;
  }
  if (y0 > corrRows) {  // Starting below the image, truncate to image bottom.
    if ((y0 + (corrCols - x0) * slope) > corrRows) {
      return;  // The whole line is below the image.
    }
    x0 += (corrRows - y0) / slope;
    y0 = corrRows;
  }
  if (x0 > x1) {
    return;  // Line segment is too short to intersect the image.
  }
  if (y1 > corrRows) {  // Ending below the image, truncate to image bottom.
    x1 = x0 + static_cast<Int26p6>((corrRows - y0) / slope);
    y1 = corrRows;
  }
  if (y1 < 0) {  // Ending above the image, truncate to image top.
    x1 = x0 + static_cast<Int26p6>((0 - y0) / slope);
    y1 = 0;
  }
  if (x1 > corrCols) {  // Ending to the right of the image, truncate to the right.
    y1 = y0 + static_cast<Int26p6>((corrCols - x0) * slope);
    x1 = corrCols;
  }

  auto drawPixel = [color, dest](Int26p6 xx, Int26p6 yy, Int26p6 aa) {
    const int x = xx >> 6;
    const int y = yy >> 6;
    if (aa <= 0 || x < 0 || y < 0 || x >= dest.cols() || y >= dest.rows()) {
      return;
    }
    uint8_t *p = dest.pixels() + (x << 2) + y * dest.rowBytes();
    if (aa == 64) {
      p[0] = color.r();
      p[1] = color.g();
      p[2] = color.b();
      p[3] = color.a();
    } else {
      Color c = mixSRGB(color, Color(p[0], p[1], p[2], p[3]), static_cast<float>(aa) / 64.0f);
      p[0] = c.r();
      p[1] = c.g();
      p[2] = c.b();
      p[3] = c.a();
    }
  };

  // Draw one point.
  Int26p6 xend = round26p6(x0 - 32);
  Int26p6 yend = (y0 - 32) + static_cast<Int26p6>(slope * (xend - (x0 - 32)));
  Int26p6 xgap = ifract(x0);
  const Int26p6 xp1 = xend;
  const Int26p6 yp1 = floor26p6(yend);
  const Int26p6 maybeShift = (ifract(yend) > 32) ? 0 : -64;
  const Int26p6 halfWidth = floor26p6((width + maybeShift) / 2);

  // TODO(mc): Currently even widths render as thick as width+1. Figure out
  // how to divide their width into the outside segments.

  if (absSlopeGreaterThanOne) {
    drawPixel(yp1 - halfWidth, xp1, (ifract(yend) * xgap) >> 6);
    for (Int26p6 i = 64; i < width; i += 64) {
      drawPixel(yp1 + i - halfWidth, xp1, xgap);
    }
    drawPixel(yp1 + width - halfWidth, xp1, (fract(yend) * xgap) >> 6);
  } else {
    drawPixel(xp1, yp1 - halfWidth, (ifract(yend) * xgap) >> 6);
    for (Int26p6 i = 64; i < width; i += 64) {
      drawPixel(xp1, yp1 + i - halfWidth, xgap);
    }
    drawPixel(xp1, yp1 + width - halfWidth, (fract(yend) * xgap) >> 6);
  }
  Int26p6 curY = yend + static_cast<Int26p6>(64.0f * slope);

  // Draw other point.
  xend = round26p6(x1 - 32);
  yend = (y1 - 32) + static_cast<Int26p6>(slope * (xend - (x1 - 32)));
  xgap = fract(x1);
  const Int26p6 xp2 = xend;
  const Int26p6 yp2 = floor26p6(yend);
  if (absSlopeGreaterThanOne) {
    drawPixel(yp2 - halfWidth, xp2, (ifract(yend) * xgap) >> 6);
    for (Int26p6 i = 64; i < width; i += 64) {
      drawPixel(yp2 + i - halfWidth, xp2, xgap);
    }
    drawPixel(yp2 + width - halfWidth, xp2, (fract(yend) * xgap) >> 6);
  } else {
    drawPixel(xp2, yp2 - halfWidth, (ifract(yend) * xgap) >> 6);
    for (Int26p6 i = 64; i < width; i += 64) {
      drawPixel(xp2, yp2 + i - halfWidth, xgap);
    }
    drawPixel(xp2, yp2 + width - halfWidth, (fract(yend) * xgap) >> 6);
  }

  // Draw between the points.
  if (absSlopeGreaterThanOne) {
    for (Int26p6 x = xp1 + 64; x < xp2; x += 64) {
      drawPixel(floor26p6(curY) - halfWidth, x, ifract(curY));
      for (Int26p6 i = 64; i < width; i += 64) {
        drawPixel(floor26p6(curY) + i - halfWidth, x, 64);
      }
      drawPixel(floor26p6(curY) + width - halfWidth, x, fract(curY));
      curY += 64.0f * slope;
    }
  } else {
    for (Int26p6 x = xp1 + 64; x < xp2; x += 64) {
      drawPixel(x, floor26p6(curY) - halfWidth, ifract(curY));
      for (Int26p6 i = 64; i < width; i += 64) {
        drawPixel(x, floor26p6(curY) + i - halfWidth, 64);
      }
      drawPixel(x, floor26p6(curY) + width - halfWidth, fract(curY));
      curY += 64.0f * slope;
    }
  }
}

void drawRectangle(HPoint2 topLeft, HPoint2 bottomRight, Color color, RGBA8888PlanePixels dest) {
  int tp = std::floor(topLeft.y());
  int lp = std::floor(topLeft.x());
  int bp = std::floor(bottomRight.y());
  int rp = std::floor(bottomRight.x());

  float ta = 1.0f - (topLeft.y() - tp);
  float la = 1.0f - (topLeft.x() - lp);
  float ba = bottomRight.y() - bp;
  float ra = bottomRight.x() - bp;

  auto drawAliasedPixel = [color, dest](int x, int y, float a) {
    if (a <= 0.0f || x < 0 || y < 0 || x >= dest.cols() || y >= dest.rows()) {
      return;
    }
    uint8_t *p = dest.pixels() + (x << 2) + y * dest.rowBytes();
    Color c = mixSRGB(color, Color(p[0], p[1], p[2], p[3]), a);
    p[0] = c.r();
    p[1] = c.g();
    p[2] = c.b();
    p[3] = c.a();
  };

  // Draw top-left corner.
  drawAliasedPixel(lp, tp, la * ta);
  // Draw top-right corner.
  drawAliasedPixel(rp, tp, ra * ta);
  // Draw bottom-left corner.
  drawAliasedPixel(lp, bp, la * ba);
  // Draw bottom-right corner.
  drawAliasedPixel(rp, bp, ra * ba);

  int xBegin = std::max(lp + 1, 0);
  int yBegin = std::max(tp + 1, 0);
  int xEnd = std::min(rp, dest.cols());
  int yEnd = std::min(bp, dest.rows());

  // Draw top and bottom lines.
  for (int x = xBegin; x < xEnd; ++x) {
    drawAliasedPixel(x, tp, ta);
    drawAliasedPixel(x, bp, ba);
  }

  // Draw left and right lines.
  for (int y = yBegin; y < yEnd; ++y) {
    drawAliasedPixel(lp, y, la);
    drawAliasedPixel(rp, y, ra);
  }

  // Fill interior.
  uint8_t *destRowStart = dest.pixels() + (xBegin << 2) + yBegin * dest.rowBytes();
  for (int y = yBegin; y < yEnd; ++y) {
    uint8_t *destPix = destRowStart;
    for (int x = xBegin; x < xEnd; ++x) {
      destPix[0] = color.r();
      destPix[1] = color.g();
      destPix[2] = color.b();
      destPix[3] = color.a();
      destPix += 4;
    }
    destRowStart += dest.rowBytes();
  }
}

void drawShape(Vector<HPoint2> pts, int width, Color color, RGBA8888PlanePixels dest) {
  for (int i = 0; i < pts.size(); ++i) {
    drawPoint(pts[i], width / 2.0f, color, dest);
    drawLine(pts[i], pts[(i + 1) % pts.size()], width, color, dest);
  }
}

void drawShape(Vector<HPoint2> pts, int width, Vector<Color> color, RGBA8888PlanePixels dest) {
  for (int i = 0; i < pts.size(); ++i) {
    drawPoint(pts[i], width / 2.0f, color[i], dest);
    drawLine(pts[i], pts[(i + 1) % pts.size()], width, color[i], dest);
  }
}

void drawPoint(HPoint2 pt, float radius, Color color, RGBA8888PlanePixels dest) {
  // Find a square boundary that will contain all the pixels in which we might
  // need to draw.
  constexpr float HP = 0.5f;
  const int beginX = std::max(0, static_cast<int>(std::floor(pt.x() - radius - HP)));
  const int beginY = std::max(0, static_cast<int>(std::floor(pt.y() - radius - HP)));
  const int endX = std::min(dest.cols(), static_cast<int>(std::ceil(pt.x() + radius + HP) + 1));
  const int endY = std::min(dest.rows(), static_cast<int>(std::ceil(pt.y() + radius + HP) + 1));

  uint8_t *row = dest.pixels() + beginY * dest.rowBytes();

  for (int y = beginY; y < endY; ++y) {
    for (int x = beginX; x < endX; ++x) {
      // Compute the distance between the point and the center of the pixel.
      const float dx = pt.x() - (x + HP);
      const float dy = pt.y() - (y + HP);
      const float d = std::sqrt(dx * dx + dy * dy);

      if (d < radius - 0.5f) {
        uint8_t *p = row + (x << 2);
        p[0] = color.r();
        p[1] = color.g();
        p[2] = color.b();
        p[3] = color.a();
      } else if (d < radius + 0.5f) {
        // Compute a weight for the pixel, depending on the approximate fraction
        // of a pixel that is covered by the circle.
        const float a = radius - d + 0.5f;

        // Draw the pixel as a weighted sum of the point color and the pixel color.
        uint8_t *p = row + (x << 2);
        Color c = mixSRGB(color, Color(p[0], p[1], p[2], p[3]), a);
        p[0] = c.r();
        p[1] = c.g();
        p[2] = c.b();
        p[3] = c.a();
      }
    }
    row += dest.rowBytes();
  }
}

void drawPoint(HPoint2 pt, float radius, int thickness, Color color, RGBA8888PlanePixels dest) {
  drawPoint(pt, radius, color, dest);
}

void heatMap(uint8_t val, uint8_t *r, uint8_t *g, uint8_t *b) {
  const auto &c = HOT_RGB_256[val];
  *r = c[0];
  *g = c[1];
  *b = c[2];
}

Color heatMap(uint8_t val) {
  return Color::hot(val / 255.0f);
}

void drawCompass(const HMatrix &extrinsic, const HMatrix &intrinsic, RGBA8888PlanePixels dest) {
  Vector<HPoint3> compassPoints;
  Vector<Color> colors;
  for (int x = -7; x <= 7; ++x) {
    for (int z = -7; z <= 7; ++z) {
      compassPoints.push_back(HPoint3(static_cast<float>(x), -1.0f, static_cast<float>(z)));
      if (x < 0 && z < 0) {
        // bottom left
        colors.push_back(Color::GREEN);
      } else if (x == 0 && z < 0) {
        // back
        colors.push_back(Color::DARK_RED);
      } else if (x > 0 && z < 0) {
        // bottom right
        colors.push_back(Color::BLUE);
      } else if (x < 0 && z == 0) {
        // left
        colors.push_back(Color::DARK_BLUE);
      } else if (x > 0 && z == 0) {
        // right
        colors.push_back(Color::DARK_YELLOW);
      } else if (x < 0 && z > 0) {
        // top left
        colors.push_back(Color::YELLOW);
      } else if (x > 0 && z > 0) {
        // top right
        colors.push_back(Color::RED);
      } else {
        // front
        colors.push_back(Color::PURPLE);
      }
    }
  }

  Vector<HPoint3> ptsInCameraFrame = extrinsic.inv() * compassPoints;
  Vector<HPoint2> ptsInCamera = flatten<2>(intrinsic * ptsInCameraFrame);

  for (int i = 0; i < compassPoints.size(); ++i) {
    if (ptsInCameraFrame[i].z() < 0) {
      continue;
    }
    if (ptsInCamera[i].x() < 0 || ptsInCamera[i].x() > dest.cols()) {
      continue;
    }
    if (ptsInCamera[i].y() < 0 || ptsInCamera[i].y() > dest.rows()) {
      continue;
    }

    drawPoint(ptsInCamera[i], 6, Color(200, 200, 200), dest);
    drawPoint(ptsInCamera[i], 5, Color(55, 55, 55), dest);
    drawPoint(ptsInCamera[i], 4, colors[i], dest);
    drawPoint(ptsInCamera[i], 3, colors[i], dest);
    drawPoint(ptsInCamera[i], 2, colors[i], dest);
    drawPoint(ptsInCamera[i], 1, colors[i], dest);
  }
}

void drawAxis(
  const HMatrix &extrinsic,
  const HMatrix &intrinsic,
  HPoint3 origin,
  float len,
  RGBA8888PlanePixels dest) {
  auto t = HMatrixGen::translation(origin.x(), origin.y(), origin.z());
  drawAnchor(extrinsic, intrinsic, t, len, dest);
}

void drawAxis(
  const HMatrix &extrinsic,
  const HMatrix &intrinsic,
  HPoint3 origin,
  float len,
  const std::array<Color, 3> &axisColors,
  RGBA8888PlanePixels dest) {
  auto t = HMatrixGen::translation(origin.x(), origin.y(), origin.z());
  drawAnchor(extrinsic, intrinsic, t, len, axisColors, dest);
}

void drawAnchor(
  const HMatrix &extrinsic,
  const HMatrix &intrinsic,
  const HMatrix &anchorPose,
  float len,
  RGBA8888PlanePixels dest) {
  drawAnchor(extrinsic, intrinsic, anchorPose, len, {Color::CHERRY, Color::MANGO, Color::MINT}, dest);
}

void drawAnchor(
  const HMatrix &extrinsic,
  const HMatrix &intrinsic,
  const HMatrix &anchorPose,
  float len,
  const std::array<Color, 3> &axisColors,
  RGBA8888PlanePixels dest) {
  Vector<HPoint3> axes{
    HPoint3(0.0f, 0.0f, 0.0f),
    HPoint3(1.0f, 0.0f, 0.0f),
    HPoint3(0.0f, 1.0f, 0.0f),
    HPoint3(0.0f, 0.0f, 1.0f),
  };

  auto axesImage3 =
    (intrinsic * extrinsic.inv() * anchorPose * HMatrixGen::scale(len, len, len)) * axes;

  if (axesImage3[0].x() > 0 && axesImage3[0].y() > 0 && axesImage3[0].z() > 0) {
    for (int i = 1; i <= 3; ++i) {
      drawLine(axesImage3[0].flatten(), axesImage3[i].flatten(), 3, axisColors[i - 1], dest);
    }
  }
}

void putText(
  const String &text, HPoint2 pt, Color color, RGBA8888PlanePixels dest, bool dropShadow) {
  if (dropShadow) {
    renderText(text, {pt.x() + 1, pt.y() + 1}, 20.0f, {0, 0, 0}, dest);
  }
  renderText(text, pt, 20.0f, color, dest);
}

void putText(const String &text, HPoint2 pt, Color color, Color bgColor, RGBA8888PlanePixels dest) {
  putText(text, pt, color, bgColor, 20.0f, dest);
}

void putText(
  const String &text,
  HPoint2 pt,
  Color color,
  Color bgColor,
  float fontSize,
  RGBA8888PlanePixels dest) {
  auto [tl, br] = calculateTextWidth(text, pt, fontSize);
  drawRectangle(tl, br, bgColor, dest);
  renderText(text, pt, fontSize, color, dest);
}

void fill(Color color, RGBA8888PlanePixels dest) {
  ScopeTimer t("fill-color");
  fill(color.r(), color.g(), color.b(), color.a(), &dest);
}

void drawImageChannel(ConstRGBA8888PlanePixels pp, int channel, RGBA8888PlanePixels dest) {
  auto cp = dest;

  fill(Color::BLACK, dest);

  const auto *ps = pp.pixels();
  auto *cs = cp.pixels();
  auto pr = pp.rowBytes();
  auto cr = cp.rowBytes();
  auto pc = pp.cols() * 4;
  auto pse = ps + pp.rows() * pr;

  while (ps < pse) {
    const auto *s = ps;
    auto *d = cs;
    const auto *e = s + pc;
    while (s < e) {
      heatMap(s[channel], d, d + 1, d + 2);
      d[3] = 255;
      s += 4;
      d += 4;
    }
    ps += pr;
    cs += cr;
  }
}

void drawImageChannelGray(ConstRGBA8888PlanePixels pp, int channel, RGBA8888PlanePixels dest) {
  auto cp = dest;

  fill(Color::BLACK, dest);

  const auto *ps = pp.pixels();
  auto *cs = cp.pixels();
  auto pr = pp.rowBytes();
  auto cr = cp.rowBytes();
  auto pc = pp.cols() * 4;
  auto pse = ps + pp.rows() * pr;

  while (ps < pse) {
    const auto *s = ps;
    auto *d = cs;
    const auto *e = s + pc;
    while (s < e) {
      auto v = s[channel];
      d[0] = v;
      d[1] = v;
      d[2] = v;
      d[3] = 255;
      s += 4;
      d += 4;
    }
    ps += pr;
    cs += cr;
  }
}

void drawVertexNormals(
  const Vector<HPoint3> &vertices,
  const Vector<MeshIndices> &indices,
  const float normalScalingFactor,
  const HMatrix &cam,
  const HMatrix &cameraMotion,
  Color color,
  int lineWidth,
  RGBA8888PlanePixels &dest) {
  Vector<HVector3> vertexNormals;
  computeVertexNormals(vertices, indices, &vertexNormals);

  auto projectedPoints = flatten<2>((cam * cameraMotion.inv() * vertices));

  const auto normalsScalingFactor =
    HMatrixGen::scale(normalScalingFactor, normalScalingFactor, normalScalingFactor);

  Vector<HPoint3> normalHead;
  normalHead.resize(vertices.size());
  for (int i = 0; i < vertices.size(); i++) {
    auto resizedVertexNormal = normalsScalingFactor * vertexNormals[i];
    normalHead[i] = HPoint3(
      vertices[i].x() + resizedVertexNormal.x(),
      vertices[i].y() + resizedVertexNormal.y(),
      vertices[i].z() + resizedVertexNormal.z());
  }
  auto projectedNormalPoints = flatten<2>((cam * cameraMotion.inv() * normalHead));

  for (int i = 0; i < vertices.size(); i++) {
    drawLine(projectedPoints[i], projectedNormalPoints[i], lineWidth, color, dest);
  }
}

void drawFaceNormals(
  const Vector<HPoint3> &vertices,
  const Vector<MeshIndices> &indices,
  const float normalScalingFactor,
  const HMatrix &cam,
  const HMatrix &cameraMotion,
  Color color,
  int lineWidth,
  RGBA8888PlanePixels &dest) {
  Vector<HVector3> faceNormals;
  computeFaceNormals(vertices, indices, &faceNormals);

  Vector<HPoint3> faceCenter;
  faceCenter.resize(indices.size());
  Vector<HPoint3> faceNormalHead;
  faceNormalHead.resize(indices.size());

  const auto normalsScalingFactor =
    HMatrixGen::scale(normalScalingFactor, normalScalingFactor, normalScalingFactor);

  for (int i = 0; i < indices.size(); i++) {
    // calculate centroid of three points
    auto face = indices[i];
    Vector<HPoint3> facePoints = {
      vertices[face.a],
      vertices[face.b],
      vertices[face.c],
    };
    auto faceCentroid = computeCentroid(facePoints);
    faceCenter[i] = faceCentroid;

    auto resizedFaceNormal = normalsScalingFactor * faceNormals[i];
    faceNormalHead[i] = HPoint3(
      faceCentroid.x() + resizedFaceNormal.x(),
      faceCentroid.y() + resizedFaceNormal.y(),
      faceCentroid.z() + resizedFaceNormal.z());
  }

  auto projectedFaceCenter = flatten<2>((cam * cameraMotion.inv() * faceCenter));
  auto projectedFaceNormalHead = flatten<2>((cam * cameraMotion.inv() * faceNormalHead));

  for (int i = 0; i < indices.size(); i++) {
    drawLine(projectedFaceCenter[i], projectedFaceNormalHead[i], lineWidth, color, dest);
  }
}

void drawMesh(
  const Vector<HPoint2> &projectedPoints,
  const Vector<MeshIndices> &indices,
  Color color,
  int lineWidth,
  RGBA8888PlanePixels &dest) {
  for (auto triangle : indices) {
    auto pointA = projectedPoints[triangle.a];
    auto pointB = projectedPoints[triangle.b];
    auto pointC = projectedPoints[triangle.c];

    drawLine(pointA, pointB, lineWidth, color, dest);
    drawLine(pointB, pointC, lineWidth, color, dest);
    drawLine(pointC, pointA, lineWidth, color, dest);
  }
}

void drawMeshAndNormals(
  const Vector<HPoint3> &vertices,
  const Vector<MeshIndices> &indices,
  const float normalScalingFactor,
  const HMatrix &cam,
  const HMatrix &cameraMotion,
  const int lineWidth,
  RGBA8888PlanePixels &dest) {

  auto projectedPoints = flatten<2>((cam * cameraMotion.inv() * vertices));

  drawVertexNormals(
    vertices,
    indices,
    normalScalingFactor,
    cam,
    cameraMotion,
    Color(0, 255, 0),
    lineWidth,
    dest);

  drawMesh(projectedPoints, indices, Color(255, 255, 0), lineWidth, dest);

  // Draw the circles detected
  drawPoints(projectedPoints, 3, Color(255, 0, 0), dest);
}

}  // namespace c8
