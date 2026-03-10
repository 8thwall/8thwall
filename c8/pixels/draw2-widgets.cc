// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "draw2-widgets.h",
  };
  deps = {
    ":draw2",
    ":pixels",
    "//c8:color",
    "//c8:hmatrix",
    "//c8:hpoint",
    "//c8/geometry:egomotion",
    "//c8/geometry:parameterized-geometry",
    "//c8/pixels:gr8-pyramid",
    "//reality/engine/features:frame-point",
    "//reality/engine/features:image-point",
    "//reality/engine/geometry:image-warp",
    "//reality/engine/imagedetection:target-point",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xb5efea1d);

#include "c8/geometry/egomotion.h"
#include "c8/pixels/draw2-widgets.h"
#include "c8/pixels/draw2.h"
#include "c8/pixels/pixel-transforms.h"
#include "reality/engine/geometry/image-warp.h"

namespace c8 {

void drawFeatures(const ImagePoints &feats, RGBA8888PlanePixels dest, bool useGravityAngle) {
  for (const auto &f : feats) {
    auto pt = f.location().pt;
    float s = 1.0f;

    Color color;
    int n = f.location().roi < 0 ? f.location().scale : f.location().roi;
    if (n == 0) {
      color = Color::LIGHT_PURPLE;
    } else if (n == 1) {
      color = Color::MANGO;
    } else if (n == 2) {
      color = Color::MINT;
    } else if (n == 3) {
      color = Color::CHERRY;
    }

    float angle = useGravityAngle ? f.location().gravityAngle : f.location().angle;
    float xoff = std::cos(angle * M_PI / 180.0f) * 7;
    float yoff = std::sin(angle * M_PI / 180.0f) * 7;

    drawLine({pt.x * s, pt.y * s}, {pt.x * s + xoff, pt.y * s + yoff}, 1, color, dest);
    drawPoint({pt.x * s, pt.y * s}, 2.0, color, dest);
  }
}

void drawFeatures(const FrameWithPoints &feats, RGBA8888PlanePixels dest, bool useGravityAngle) {
  for (const auto &f : feats.points()) {
    Color color;
    // Color HIRES_SCAN (which have faceId -1 -> -4) differently than other ROIs
    if (feats.roi().source != ImageRoi::Source::HIRES_SCAN && feats.roi().faceId < 0) {
      if (f.scale() == 0) {
        color = Color::LIGHT_PURPLE;
      } else if (f.scale() == 1) {
        color = Color::MANGO;
      } else if (f.scale() == 2) {
        color = Color::MINT;
      } else if (f.scale() == 3) {
        color = Color::CHERRY;
      }
    } else {
      int hiResId = std::abs(feats.roi().faceId % 4);
      if (hiResId == 0) {
        color = Color::PINK;
      } else if (hiResId == 1) {
        color = Color::VIBRANT_YELLOW;
      } else if (hiResId == 2) {
        color = Color::MATCHA;
      } else if (hiResId == 3) {
        color = Color::DULL_BLUE;
      }
    }
    if (useGravityAngle) {
      color = Color::YELLOW;
    }

    auto pt = feats.distort(f.position());
    float s = 1.0f;
    float angle = useGravityAngle ? f.gravityAngle() : f.angle();
    float xoff = std::cos(angle * M_PI / 180.0f) * 7;
    float yoff = std::sin(angle * M_PI / 180.0f) * 7;

    // draw the feature point's angle as a mini-trail
    drawLine({pt.x() * s, pt.y() * s}, {pt.x() * s + xoff, pt.y() * s + yoff}, 1, color, dest);
    drawPoint({pt.x() * s, pt.y() * s}, 2.0, color, dest);
  }
}

void drawFeatures(
  const TargetWithPoints &feats,
  RGBA8888PlanePixels dest,
  bool useGravityAngle,
  bool rotateFeatures,
  int imageWidth) {
  for (const auto &f : feats.points()) {
    int n = f.scale();
    auto pt = feats.distort(f.position());
    float s = 1.0f;

    Color color;
    if (n == 0) {
      color = Color::LIGHT_PURPLE;
    } else if (n == 1) {
      color = Color::MANGO;
    } else if (n == 2) {
      color = Color::MINT;
    } else if (n == 3) {
      color = Color::CHERRY;
    }

    float x;
    float y;
    float angle = useGravityAngle ? f.gravityAngle() : f.angle();
    if (rotateFeatures) {
      x = imageWidth - 1 - pt.y() * s;
      y = pt.x() * s;
      angle += 90.0f;
    } else {
      x = pt.x() * s;
      y = pt.y() * s;
    }
    float xoff = std::cos(angle * M_PI / 180.0f) * 7;
    float yoff = std::sin(angle * M_PI / 180.0f) * 7;

    drawLine({x, y}, {x + xoff, y + yoff}, 1, color, dest);
    drawPoint({x, y}, 2.0, color, dest);
  }
}

void drawFeatureGravityDirections(
  const HMatrix &extrinsic,
  const HMatrix &intrinsics,
  const FrameWithPoints &feats,
  float len,
  Color color,
  RGBA8888PlanePixels dest) {

  // Gravity direction in 3D space
  const Vector<HPoint3> axes{
    HPoint3(0.0f, 0.0f, 0.0f),
    HPoint3(0.0f, 1.0f, 0.0f),
  };

  for (const auto &f : feats.points()) {
    auto pt = feats.distort(f.position());
    // Project the gravity direction into the image space
    HPoint3 origin = HPoint3(0.0f, 0.0f, 0.0f);
    auto anchorPose = HMatrixGen::translation(origin.x(), origin.y(), origin.z());

    auto axesImage3 = (intrinsics * extrinsic.inv() * anchorPose) * axes;

    float xOffset = axesImage3[0].flatten().x() - axesImage3[1].flatten().x();
    float yOffset = axesImage3[0].flatten().y() - axesImage3[1].flatten().y();
    drawLine({pt.x(), pt.y()}, {pt.x() + xOffset * len, pt.y() + yOffset * len}, 1, color, dest);
  }
}

void drawPyramidChannel(Gr8Pyramid pyr, int channel, RGBA8888PlanePixels dest) {
  drawImageChannel(pyr.data, channel, dest);
}

void textBox(
  const Vector<String> &lines,
  HPoint2 topLeft,
  int width,
  RGBA8888PlanePixels dest,
  Color textColor,
  Color bgColor) {
  int height = lines.size() * 20 + 15;
  drawRectangle(topLeft, {topLeft.x() + width, topLeft.y() + height}, bgColor, dest);
  auto cm = crop(dest, height, width - 5, topLeft.y(), topLeft.x());
  for (int i = 0; i < lines.size(); ++i) {
    putText(lines[i], {5.0f, 25 + i * 20.0f}, textColor, cm);
  }
}

void drawImageFrustum(
  const c8_PixelPinholeCameraModel &targetK,
  const c8_PixelPinholeCameraModel &intrinsics,
  const HMatrix &extrinsic,
  const HMatrix &imPose,
  float scale,
  Color color,
  RGBA8888PlanePixels cp) {

  auto K = HMatrixGen::intrinsic(intrinsics);

  // Axis in image
  drawAxis(imPose.inv() * extrinsic, K, HPoint3(0.0f, 0.0f, scale), 0.3f, cp);
  // drawAxis(lastLocatedPose_.inv() * extrinsic, K, HPoint3(0.0f, 0.0f, 0.0f), 0.3f, cp);

  auto imCorners = flatten<2>(K * extrinsic.inv() * imPose * cameraFrameCorners(targetK, scale));
  auto imOrigin3 = HPoint3(imPose(0, 3), imPose(1, 3), imPose(2, 3));
  auto imOrigin = (K * extrinsic.inv() * imOrigin3).flatten();
  for (auto &ic : imCorners) {
    if (std::isinf(ic.x()) || std::isinf(ic.y())) {
      continue;
    }
    imCorners.push_back(ic);
    imCorners.push_back(imOrigin);
  }
  drawShape(imCorners, 5, color, cp);
  drawPoints(imCorners, 5, 5, Color::GREEN, cp);
}

void drawCameraFrustum(
  c8_PixelPinholeCameraModel intrinsics,
  const HMatrix &extrinsic,
  c8_PixelPinholeCameraModel worldIntrinsics,
  const HMatrix &world,
  Color color,
  RGBA8888PlanePixels dest) {
  auto K = HMatrixGen::intrinsic(worldIntrinsics);
  auto P = K * world.inv();
  auto origin3 = P * HPoint3(extrinsic(0, 3), extrinsic(1, 3), extrinsic(2, 3));
  if (origin3.z() < 0) {
    return;
  }
  auto origin = origin3.flatten();
  auto frustum3 = (P * extrinsic) * cameraFrameCorners(intrinsics, 0.2f);
  auto frustum = flatten<2>(frustum3);
  for (auto &ic : frustum3) {
    if (ic.z() < 0 || std::isinf(ic.x()) || std::isinf(ic.y())) {
      continue;
    }
    frustum.push_back(ic.flatten());
    frustum.push_back(origin);
  }
  drawShape(frustum, 5, color, dest);
  for (int i = 1; i < 10; ++i) {
    drawPoint(origin, i, color, dest);
  }
}

void drawFloor(
  c8_PixelPinholeCameraModel intrinsics,
  const HMatrix &extrinsic,
  HPoint3 origin,
  float scale,
  int xpts,
  int ypts,
  Color color,
  RGBA8888PlanePixels dest) {
  auto P = HMatrixGen::intrinsic(intrinsics) * extrinsic.inv();
  for (int i = -xpts; i <= xpts; ++i) {
    for (int j = -ypts; j <= ypts; ++j) {
      auto x = i * scale + origin.x();
      auto x1 = (i - .5f) * scale + origin.x();
      auto x2 = x1 + scale;
      auto z = j * scale + origin.z();
      auto z1 = (j - .5f) * scale + origin.z();
      auto z2 = z1 + scale;
      auto y = origin.y();
      auto xp1 = P * HPoint3{x1, y, z};
      auto xp2 = P * HPoint3{x2, y, z};
      auto zp1 = P * HPoint3{x, y, z1};
      auto zp2 = P * HPoint3{x, y, z2};
      if (xp1.z() > 0 && xp2.z() > 0) {
        drawLine(xp1.flatten(), xp2.flatten(), 1, color, dest);
      }
      if (zp1.z() > 0 && zp2.z() > 0) {
        drawLine(zp1.flatten(), zp2.flatten(), 1, color, dest);
      }
    }
  }
}

Vector<Color> pixelColors(ConstRGBA8888PlanePixels im, const Vector<HPoint2> &pixels) {
  Vector<Color> out;
  std::transform(pixels.begin(), pixels.end(), std::back_inserter(out), [im](HPoint2 p) -> Color {
    int r = std::round(p.y());
    int c = std::round(p.x());
    const auto *pix = im.pixels() + r * im.rowBytes() + 4 * c;
    return {pix[0], pix[1], pix[2]};
  });
  return out;
}

void drawPointCloud(
  c8_PixelPinholeCameraModel k,
  const HMatrix &cam,
  const Vector<HPoint3> &pts,
  const Vector<Color> &colors,
  RGBA8888PlanePixels out) {
  auto pixPts = flatten<2>((HMatrixGen::intrinsic(k) * cam.inv()) * pts);

  for (int i = 0; i < pixPts.size(); ++i) {
    drawPoint(pixPts[i], 0, colors[i], out);
  }
}

void drawPointCloudWithBackCulling(
  c8_PixelPinholeCameraModel k,
  const HMatrix &cam,
  const Vector<HPoint3> &pts,
  const Vector<HVector3> &normals,
  const Vector<Color> &colors,
  RGBA8888PlanePixels out) {
  auto rayPts3 = cam.inv() * pts;
  auto rayPts = flatten<2>(rayPts3);
  auto pixPts = flatten<2>(HMatrixGen::intrinsic(k) * rayPts3);
  auto transformedNormals = cam.inv() * normals;

  for (int i = 0; i < pixPts.size(); ++i) {
    if (transformedNormals[i].dot({rayPts[i].x(), rayPts[i].y(), 1.0f}) < 0) {
      drawPoint(pixPts[i], 0, colors[i], out);
    }
  }
}

void drawCurvyImage(
  c8_PixelPinholeCameraModel k,
  const HMatrix &cam,
  CurvyImageGeometry im,
  RGBA8888PlanePixels out) {
  drawCurvyImageAtScale(k, cam, im, 1.0f, out);
}

Vector<HPoint3> curvyModelPoints(CurvyImageGeometry im, float scale) {
  // draw from back to front
  const int numSegments = 16;
  Vector<HPoint3> worldCorners;

  // We extend the height of the curvy to the full curvy instead of only the
  // cropped one which we use for tracking

  // draw top
  for (int i = 0; i <= numSegments; i++) {
    const float angle = 2 * M_PI * i / numSegments;
    worldCorners.emplace_back(
      -im.radius * std::sin(angle) * scale,
      (im.height * 0.5f
       + im.activationRegion.top / (im.activationRegion.bottom - im.activationRegion.top))
        * scale,
      im.radius * std::cos(angle) * scale);
  }

  // draw bottom in reverse
  for (int i = numSegments; i >= 0; i--) {
    const float angle = 2 * M_PI * i / numSegments;
    worldCorners.emplace_back(
      -im.radiusBottom * std::sin(angle) * scale,
      (-im.height * 0.5f
       - (1.f - im.activationRegion.bottom)
         / (im.activationRegion.bottom - im.activationRegion.top))
        * scale,
      im.radiusBottom * std::cos(angle) * scale);
  }

  return worldCorners;
}

void drawCurvyImageAtScale(
  c8_PixelPinholeCameraModel k,
  const HMatrix &cam,
  CurvyImageGeometry im,
  float scale,
  RGBA8888PlanePixels out) {
  // draw from back to front
  Vector<HPoint3> worldCorners = curvyModelPoints(im, scale);

  Vector<HPoint3> ptsInCamSpace = cam.inv() * worldCorners;
  auto pix = flatten<2>(HMatrixGen::intrinsic(k) * ptsInCamSpace);
  // color get lighter with distance
  Vector<Color> colors;
  std::transform(
    ptsInCamSpace.begin(),
    ptsInCamSpace.end(),
    std::back_inserter(colors),
    [](HPoint3 pt) -> Color {
      const float alpha =
        1.f - std::sqrt(pt.x() * pt.x() + pt.y() * pt.y() + pt.z() * pt.z()) / 2.0;
      return Color::blend(Color::WHITE, alpha, Color::BLACK, 1 - alpha);
    });

  drawShape(pix, 1, colors, out);
}

void drawCubeImageAtScale(
  c8_PixelPinholeCameraModel k, const HMatrix &cam, float scale, Color c, RGBA8888PlanePixels out) {
  scale = scale / 2;
  Vector<HPoint3> worldCorners = {
    {-scale, -scale, -scale},
    {-scale, -scale, scale},
    {-scale, scale, scale},
    {scale, scale, scale},
    {scale, scale, -scale},
    {-scale, scale, -scale},
    {-scale, -scale, -scale},
    {scale, -scale, -scale},
    {scale, -scale, scale},
    {-scale, -scale, scale},
    {-scale, scale, scale},
    {-scale, scale, -scale},
    {scale, scale, -scale},
    {scale, -scale, -scale},
    {scale, -scale, scale},
    {scale, scale, scale}};
  auto pix = flatten<2>(HMatrixGen::intrinsic(k) * cam.inv() * worldCorners);
  drawShape(pix, 1, c, out);
}

}  // namespace c8
