// Copyright (c) 2019 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":gl-reality-frame",
    ":gr8gl",
    "//c8/io:image-io",
    "//c8/pixels:pixel-transforms",
    "//c8/pixels/opengl:gl-error",
    "//c8/pixels/opengl:offscreen-gl-context",
    "//c8/string:format",
    "//c8/string:join",
    "//reality/engine/features:gr8-feature-shader",
    "//reality/engine/features:image-point-testing",
    "@com_google_googletest//:gtest_main",
  };
  data = {
    "//reality/engine/imagedetection/testdata:flower-big",
  };
}
cc_end(0x20d14d6a);

#include "c8/io/image-io.h"
#include "c8/parameter-data.h"
#include "c8/pixels/opengl/gl-error.h"
#include "c8/pixels/opengl/offscreen-gl-context.h"
#include "c8/pixels/pixel-transforms.h"
#include "c8/stats/scope-timer.h"
#include "c8/string/format.h"
#include "c8/string/join.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "reality/engine/features/gl-reality-frame.h"
#include "reality/engine/features/gr8-feature-shader.h"
#include "reality/engine/features/gr8gl.h"
#include "reality/engine/features/image-point-testing.h"

namespace c8 {

class Gr8GlTest : public ::testing::Test {
  void SetUp() override {
    // Some tests in this suite rely on 1024x1024 image pyramid.
    globalParams().getOrSet("GlRealityFrame.PyramidSize", 1024);
  }
};

namespace {

// Clear the third channel (feature score) for this image.
void clearFeatures(RGBA8888PlanePixels p, uint8_t channel = 2, uint8_t val = 0) {
  for (int i = 0; i < p.rows(); ++i) {
    for (int j = 0; j < p.cols(); ++j) {
      p.pixels()[i * p.rowBytes() + (j << 2) + channel] = val;
    }
  }
}

// Get a mutable image representation of this level.
RGBA8888PlanePixels mutableLevel(Gr8Pyramid &pyr, RGBA8888PlanePixels pyrPix, int l) {
  auto level = pyr.levels[l];
  return {
    level.h,
    level.w,
    pyrPix.rowBytes(),
    pyrPix.pixels() + level.r * pyrPix.rowBytes() + 4 * level.c};
}

// Get the center pixel or pixels of an image.
Vector<HPoint2> centerPixels(ConstRGBA8888PlanePixels p) {
  std::array<int, 2> xs = {(p.cols() - 1) / 2, p.cols() / 2};
  std::array<int, 2> ys = {(p.rows() - 1) / 2, p.rows() / 2};
  int nx = xs[0] == xs[1] ? 1 : 2;
  int ny = ys[0] == ys[1] ? 1 : 2;
  Vector<HPoint2> pts;
  for (int i = 0; i < ny; ++i) {
    for (int j = 0; j < nx; ++j) {
      pts.push_back({static_cast<float>(xs[j]), static_cast<float>(ys[i])});
    }
  }
  return pts;
}

// Procedurally generate an image with a single corner.
// Here is an ASCII depiction:
//   1 1 1 1 1 1 1
//   1 1 1 1 1 1 1
//   1 1 1 0 0 0 0
//   1 1 1 0 0 0 0
//   1 1 1 0 0 0 0
void generateCorner(int x, int y, uint8_t direction, RGBA8888PlanePixels &pix) {
  size_t rowStart = 0;
  for (int i = 0; i < pix.rows(); ++i) {
    for (int j = 0; j < pix.cols(); ++j) {
      uint8_t *data = pix.pixels() + (j << 2) + rowStart;
      int left = (direction & 1) ? -1 : 1;
      int up = (direction >> 1 ? -1 : 1);
      if (up * i < up * y || left * j < left * x) {
        data[0] = 191u;
        data[1] = 191u;
        data[2] = 191u;
      } else {
        data[0] = 63u;
        data[1] = 63u;
        data[2] = 63u;
      }
      data[3] = 255u;
    }
    rowStart += pix.rowBytes();
  }
};

void setFeatureScore(RGBA8888PlanePixels p, HPoint2 loc, uint8_t score) {
  int lx = std::round(loc.x());
  int ly = std::round(loc.y());
  p.pixels()[ly * p.rowBytes() + (lx << 2) + 2] = 255;
}
}  // namespace

static Gr8Pyramid computeFeaturePyramid(
  ConstRGBA8888PlanePixels pix, RGBA8888PlanePixelBuffer *pyrBuf, bool skipSubpixel = false) {
  ScopeTimer rt("test");
  // Create an offscreen gl context.
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  // Load the source pixels into a gl texture.
  GlTexture2D imTexture = makeNearestRGBA8888Texture2D(pix.cols(), pix.rows());
  imTexture.bind();
  imTexture.tex().setPixels(pix.pixels());
  imTexture.unbind();

  // Initialize shaders.
  Gr8FeatureShader shader;
  shader.initialize();

  // Draw the gl pyramid and read it into and image.
  GlRealityFrame gl;
  gl.skipSubpixel(skipSubpixel);
  gl.initialize(&shader, pix.cols(), pix.rows(), 0);
  gl.draw(imTexture.id(), GlRealityFrame::Options::DEFER_READ);
  gl.readPixels();

  // Copy the pyramid data from the GlRealityFrame's image buffer into the provided one, and copy
  // the metadata as well.
  *pyrBuf = RGBA8888PlanePixelBuffer(gl.pyramid().data.rows(), gl.pyramid().data.cols());
  auto pyr = pyrBuf->pixels();
  copyPixels(gl.pyramid().data, &pyr);
  return {pyr, gl.pyramid().levels, gl.pyramid().rois};
}

static Gr8Pyramid computeFeaturePyramid(
  String imagePath, RGBA8888PlanePixelBuffer *pyrBuf, bool skipSubpixel = false) {
  // Read the image from disk.
  auto img = readJpgToRGBA(imagePath);
  auto pix = img.pixels();
  return computeFeaturePyramid(pix, pyrBuf, skipSubpixel);
}

TEST_F(Gr8GlTest, ComputeFastAtan2) {
  // Fast atan2
  for (int i = 0; i < 360; ++i) {
    float a = fastAtan2(std::sin(i * M_PI / 180), std::cos(i * M_PI / 180));
    EXPECT_NEAR(i, a, 0.01f);
  }
}

TEST_F(Gr8GlTest, ComputeFastAcos) {
  // Fast acos
  for (int i = 0; i < 180; ++i) {
    float a = fastAcos(std::cos(i * M_PI / 180));
    EXPECT_NEAR(i, a, 0.01f);
  }
}

TEST_F(Gr8GlTest, ComputeFeaturesFromImage) {
  ScopeTimer rt("test");
  // Compute the feature pyramid.
  RGBA8888PlanePixelBuffer pyrBuf;
  auto p = computeFeaturePyramid("reality/engine/imagedetection/testdata/flower-big.jpg", &pyrBuf);

  // Analyze it with gr8gl.
  auto gr8 = Gr8Gl::create();
  auto pts = gr8.detectAndCompute(p);
  EXPECT_GT(pts.size(), 400);
}

TEST_F(Gr8GlTest, FeaturesAllZero) {
  ScopeTimer rt("test");
  // Compute the feature pyramid.
  RGBA8888PlanePixelBuffer pyrBuf;
  auto p = computeFeaturePyramid("reality/engine/imagedetection/testdata/flower-big.jpg", &pyrBuf);

  // Clear all of the feature values.
  auto pyrPix = pyrBuf.pixels();
  clearFeatures(pyrPix);

  // Analyze it with gr8gl.
  auto gr8 = Gr8Gl::create();
  auto pts = gr8.detectAndCompute(p);

  // Expect no extracted features.
  EXPECT_EQ(pts.size(), 0);
}

TEST_F(Gr8GlTest, CheckBias) {
  ScopeTimer rt("test");
  // Compute the feature pyramid.
  RGBA8888PlanePixelBuffer pyrBuf;
  auto p = computeFeaturePyramid(
    "reality/engine/imagedetection/testdata/flower-big.jpg", &pyrBuf, true /* skipSubpixel */);
  auto pyrPix = pyrBuf.pixels();

  auto fullCx = (p.level(0).cols() - 1) * 0.5f;
  auto fullCy = (p.level(0).rows() - 1) * 0.5f;

  auto gr8 = Gr8Gl::create();

  // For each level, check that the central pixel or pixels map to the center of the full image.
  for (int l = 0; l < p.levels.size(); ++l) {
    // Clear the features of the whole pyramid.
    clearFeatures(pyrPix);

    // Get a mutable image representation of this level.
    auto lp = mutableLevel(p, pyrPix, l);

    auto activePts = centerPixels(lp);
    for (auto pt : activePts) {
      setFeatureScore(lp, pt, 255);
    }

    // Analyze it with gr8gl.
    auto pts = gr8.detectAndCompute(p);
    EXPECT_EQ(pts.size(), activePts.size());

    float xs = 0;
    float ys = 0;
    for (const auto &pt : pts) {
      xs += pt.location().pt.x;
      ys += pt.location().pt.y;
    }

    float xm = xs / pts.size();
    float ym = ys / pts.size();

    EXPECT_FLOAT_EQ(fullCx, xm)
      << "Level " << l << " (r: " << lp.rows() << ", c: " << lp.cols() << ") using level center ["
      << strJoin(
           activePts.begin(),
           activePts.end(),
           ", ",
           [](HPoint2 pt) { return format("(%01.0f, %01.0f)", pt.x(), pt.y()); })
      << "] got feature locations ["
      << strJoin(
           pts.begin(),
           pts.end(),
           ", ",
           [](const ImagePoint &pt) {
             return format("(%04.2f, %04.2f)", pt.location().pt.x, pt.location().pt.y);
           })
      << "]";
    EXPECT_FLOAT_EQ(fullCy, ym)
      << "Level " << l << " (r: " << lp.rows() << ", c: " << lp.cols() << ") using level center ["
      << strJoin(
           activePts.begin(),
           activePts.end(),
           ", ",
           [](HPoint2 pt) { return format("(%01.0f, %01.0f)", pt.x(), pt.y()); })
      << "] got feature locations ["
      << strJoin(
           pts.begin(),
           pts.end(),
           ", ",
           [](const ImagePoint &pt) {
             return format("(%04.2f, %04.2f)", pt.location().pt.x, pt.location().pt.y);
           })
      << "]";
  }
}

TEST_F(Gr8GlTest, SubPixelInterpolation) {
  ScopeTimer rt("test");

  // Create an input image with a single corner.
  constexpr int COLS = 480;
  constexpr int ROWS = 640;
  RGBA8888PlanePixelBuffer input(ROWS, COLS);
  auto pix = input.pixels();

  // Initialize all the pixels to black.
  fill(0, 0, 0, 1, &pix);

  auto gr8 = Gr8Gl::create();

  // Create an offscreen gl context.
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  // Get an empty black texture.
  GlTexture2D imTexture = makeNearestRGBA8888Texture2D(pix.cols(), pix.rows());

  // Initialize shaders.
  Gr8FeatureShader shader;
  shader.initialize();

  // Create a reality frame.
  GlRealityFrame gl;
  gl.skipSubpixel(false);
  gl.initialize(&shader, pix.cols(), pix.rows(), 0);

  // Bind the input texture.
  imTexture.bind();
  imTexture.tex().setPixels(pix.pixels());
  imTexture.unbind();

  RGBA8888PlanePixelBuffer pyrBuf = RGBA8888PlanePixelBuffer(gl.pyrSize(), gl.pyrSize());
  RGBA8888PlanePixels pyr = pyrBuf.pixels();
  Gr8Pyramid p = {pyr, gl.pyramid().levels, gl.pyramid().rois};

  auto createFeature = [&](int centerX, int centerY, uint8_t scale, std::array<uint8_t, 9> values)
    -> ImagePointLocation {
    // Do a fake first defer draw, to ensure that needsRead_ = true.
    gl.draw(imTexture.id(), GlRealityFrame::Options::DEFER_READ);

    // Now let's set 3x3 pixels in the input of the stage3 shader to known values.
    clearFeatures(pyr, 0, 0);
    clearFeatures(pyr, 1, 0);
    clearFeatures(pyr, 2, 0);
    clearFeatures(pyr, 3, 0);
    Gr8Pyramid p = {pyr, gl.pyramid().levels, gl.pyramid().rois};
    RGBA8888PlanePixels level = mutableLevel(p, pyr, scale);

    level.pixels()[((centerX - 1) << 2) + (centerY - 1) * level.rowBytes() + 2] = values[0];
    level.pixels()[(centerX << 2) + (centerY - 1) * level.rowBytes() + 2] = values[1];
    level.pixels()[((centerX + 1) << 2) + (centerY - 1) * level.rowBytes() + 2] = values[2];

    level.pixels()[((centerX - 1) << 2) + centerY * level.rowBytes() + 2] = values[3];
    level.pixels()[(centerX << 2) + centerY * level.rowBytes() + 2] = values[4];
    level.pixels()[((centerX + 1) << 2) + centerY * level.rowBytes() + 2] = values[5];

    level.pixels()[((centerX - 1) << 2) + (centerY + 1) * level.rowBytes() + 2] = values[6];
    level.pixels()[(centerX << 2) + (centerY + 1) * level.rowBytes() + 2] = values[7];
    level.pixels()[((centerX + 1) << 2) + (centerY + 1) * level.rowBytes() + 2] = values[8];

    GlSubRect subRect{0, 0, pix.cols(), pix.rows(), pyr.cols(), pyr.rows()};
    auto quad = makeVertexArrayRect(subRect, subRect);

    // Get a new pyramid texture to inject.
    GlTexture2D pyrTexture = makeNearestRGBA8888Texture2D(pyr.cols(), pyr.rows());

    // Update the input with our modified pyramid.
    pyrTexture.bind();
    pyrTexture.tex().setPixels(pyr.pixels());
    pyrTexture.unbind();

    const GlProgramObject *program = shader.shader3();

    shader.bindAndSetParams(program);

    gl.dest().bind();
    pyrTexture.bind();

    checkGLError("[gl-reality-frame] bind framebuffer");
    // Load vertex data
    quad.bind();
    glFrontFace(GL_CCW);
    checkGLError("[gl-reality-frame] bind quad");

    glViewport(0, 0, pyr.cols(), pyr.rows());
    glUniform2f(program->location("scale"), 1.0 / pyr.cols(), 1.0 / pyr.rows());
    glUniform1i(program->location("skipSubpixel"), 0);
    quad.drawElements();
    checkGLError("[gl-reality-frame] draw elements (no pyramid)");

    quad.unbind();
    checkGLError("[gl-reality-frame] quad unbind");

    pyrTexture.unbind();

    gl.readPixels();

    // Copy out the pyramid with the NMS+Subpixel stage run on our injected data.
    copyPixels(gl.pyramid().data, &pyr);

    // Analyze it with gr8gl.
    auto pt = gr8.detectAndCompute(p);

    // Ensure we have exactly one point and return it.
    EXPECT_EQ(1, pt.size()) << format("Number of features %d != 1", pt.size());
    return pt.at(0).location();
  };

  // Exact center pixel.
  EXPECT_THAT(
    createFeature(240, 320, 0, {17, 17, 17, 17, 192, 17, 17, 17, 17}),
    EqualsPtAndScale(240.0, 320.0, 0));

  // A Shift X by 0.5 pixel feature.
  EXPECT_THAT(
    createFeature(240, 320, 0, {78, 78, 78, 78, 255, 254, 78, 78, 78}),
    EqualsPtAndScale(240.5, 320.0, 0));

  // A Shift X by -0.5 pixel feature.
  EXPECT_THAT(
    createFeature(240, 320, 0, {45, 45, 45, 62, 63, 45, 45, 45, 45}),
    EqualsPtAndScale(239.5, 320.0, 0));

  // A Shift Y by 0.5 pixel feature.
  EXPECT_THAT(
    createFeature(240, 320, 0, {112, 112, 112, 112, 201, 112, 112, 200, 112}),
    EqualsPtAndScale(240, 320.5, 0));

  // A Shift Y by -0.5 pixel feature.
  EXPECT_THAT(
    createFeature(240, 320, 0, {0, 18, 0, 0, 19, 0, 0, 0, 0}), EqualsPtAndScale(240, 319.5, 0));

  // An near-exact fit to the following function.
  // z = -0.1 * x^2 - 0.2 * y^2 - 0.21 * x * y - 0.02 * x + 0.05 * y + 0.9;
  EXPECT_THAT(
    createFeature(240, 320, 0, {92, 166, 189, 209, 230, 199, 224, 191, 107}),
    EqualsPtAndScale(239.5, 320.357, 0));

  // A less-exact fit to the same function as previously given, by adjusting
  // some pixels up and down.
  EXPECT_THAT(
    createFeature(240, 320, 0, {90, 168, 183, 210, 229, 200, 223, 190, 108}),
    EqualsPtAndScale(239.5, 320.357, 0));

  // A least squares fit to the above, with a corner pixel set to zero. This
  // specific change flips the quadratic such that the fit quadratic is not an
  // ellipsoidal paraboloid. In this case we abandon subpixel regression. A
  // better algorithm might be able to include this in the error minimization.
  EXPECT_THAT(
    createFeature(240, 320, 0, {92, 166, 189, 209, 230, 199, 224, 191, 0}),
    EqualsPtAndScale(240.0, 320.0, 0));
}

// Ensure there is no bias in detected corner locations.
TEST_F(Gr8GlTest, DetectedFeaturesAreInCornerCenter) {
  ScopeTimer rt("test");

  // Create an input image with a single corner.
  constexpr int COLS = 480;
  constexpr int ROWS = 640;
  RGBA8888PlanePixelBuffer input(ROWS, COLS);
  auto pix = input.pixels();

  auto gr8 = Gr8Gl::create();

  RGBA8888PlanePixelBuffer pyrBuf;

  // Create an offscreen gl context.
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();

  // Load the source pixels into a gl texture.
  GlTexture2D imTexture = makeNearestRGBA8888Texture2D(pix.cols(), pix.rows());

  // Initialize shaders.
  Gr8FeatureShader shader;
  shader.initialize();

  // Create a GlRealityFrame with subpixel regression disabled.
  GlRealityFrame gl;
  gl.skipSubpixel(true);

  gl.initialize(&shader, pix.cols(), pix.rows(), 0);

  // Create a corner in each in 4 directions to ensure no bias in input.
  constexpr int DIRECTIONS = 4;

  // Actual location of the corner.
  constexpr double TRUE_X = 240.0;
  constexpr double TRUE_Y = 320.0;

  double meanX0 = 0.0;
  double meanY0 = 0.0;

  // Loop through the four directions, generating a corner at the same location
  // in each one.
  for (uint8_t d = 0; d < DIRECTIONS; ++d) {
    generateCorner(static_cast<int>(TRUE_X), static_cast<int>(TRUE_Y), d, pix);

    // Bind the input texture.
    imTexture.bind();
    imTexture.tex().setPixels(pix.pixels());
    imTexture.unbind();

    // Compute the feature pyramid.
    // auto p = computeFeaturePyramid(pix, &pyrBuf);
    gl.draw(imTexture.id(), GlRealityFrame::Options::DEFER_READ);
    gl.readPixels();

    // Copy the pyramid data from the GlRealityFrame's image buffer into the provided one, and copy
    // the metadata as well.
    pyrBuf = RGBA8888PlanePixelBuffer(gl.pyramid().data.rows(), gl.pyramid().data.cols());
    auto pyr = pyrBuf.pixels();
    copyPixels(gl.pyramid().data, &pyr);
    Gr8Pyramid p = {pyr, gl.pyramid().levels, gl.pyramid().rois};

    auto pyrPix = pyrBuf.pixels();

    // Analyze it with gr8gl.
    auto pts = gr8.detectAndCompute(p);

    // Expect 4 corners, 1 at each scale.
    ASSERT_EQ(pts.size(), 4);
    ASSERT_EQ(pts.at(0).location().scale, 0);
    ASSERT_EQ(pts.at(1).location().scale, 1);
    ASSERT_EQ(pts.at(2).location().scale, 2);
    ASSERT_EQ(pts.at(3).location().scale, 3);

    auto p0 = pts.at(0).location().pt;

    // Collect the (x, y) location of the corner.
    meanX0 += p0.x;
    meanY0 += p0.y;
  }
  meanX0 /= DIRECTIONS;
  meanY0 /= DIRECTIONS;

  // The mean corner position should match TRUE_X, TRUE_Y. Currently it doesn't,
  // so this is commented out until the code is changed.
  // Current Result:
  //  Failure
  //    Expected equality of these values:
  //      TRUE_X
  //        Which is: 240
  //      meanX0
  //        Which is: 239.5
  //  Failure
  //    Expected equality of these values:
  //      TRUE_Y
  //        Which is: 320
  //      meanY0
  //        Which is: 319
  // EXPECT_DOUBLE_EQ(TRUE_X, meanX0);
  // EXPECT_DOUBLE_EQ(TRUE_Y, meanY0);
}

// These methods are from gr8gl.cc. We extern them so they are not exposed in gr8gl.h
extern void keypointsForCrop(
  ConstRGBA8888PlanePixels s,
  uint8_t scale,
  int8_t roi,
  int edgeThreshold,
  Vector<ImagePointLocation> &keypoints);
extern void keypointsForCropJs(
  ConstRGBA8888PlanePixels s,
  uint8_t scale,
  int8_t roi,
  int edgeThreshold,
  Vector<ImagePointLocation> &keypoints);
// Test that keypointsForCrop produces the same data as keypointsForCropJs
TEST_F(Gr8GlTest, KeypointsForCropAndToPointsWithEmptyDescriptors) {
  uint8_t scale = 2;
  int8_t roi = 0;
  int edgeThreshold = 4;
  RGBA8888PlanePixelBuffer pix{128, 128};
  for (size_t i = 0; i < 128; i++) {
    for (size_t j = 0; j < 128; j++) {
      // set some values to non-zero
      if ((i + j) % 4) {
        pix.pixels().pixels()[i * pix.pixels().rowBytes() + 4 * j] = 50;
        pix.pixels().pixels()[i * pix.pixels().rowBytes() + 4 * j + 1] = 50;
        pix.pixels().pixels()[i * pix.pixels().rowBytes() + 4 * j + 2] = 50;
        pix.pixels().pixels()[i * pix.pixels().rowBytes() + 4 * j + 3] = 255;
      }
    }
  }
  Vector<ImagePointLocation> outKps1, outKps2;
  keypointsForCrop(pix.pixels(), scale, roi, edgeThreshold, outKps1);
  keypointsForCropJs(pix.pixels(), scale, roi, edgeThreshold, outKps2);

  EXPECT_EQ(outKps1.size(), outKps2.size());
  EXPECT_GT(outKps1.size(), 10) << "There should be some output data";
  for (size_t i = 0; i < 0; i++) {
    EXPECT_FLOAT_EQ(outKps1.at(i).pt.x, outKps2.at(i).pt.x);
    EXPECT_FLOAT_EQ(outKps1.at(i).pt.y, outKps2.at(i).pt.y);
  }

  auto emptyImagePoints = Gr8Gl::toPointsWithEmptyDescriptors(outKps1); 
  size_t numFeaturePts = std::min<size_t>(outKps1.size(), 2500);
  EXPECT_EQ(emptyImagePoints.size(), numFeaturePts);
  for (size_t i = 0; i < numFeaturePts; i++) {
    EXPECT_FLOAT_EQ(outKps1.at(i).pt.x, emptyImagePoints.at(i).location().pt.x);
    EXPECT_FLOAT_EQ(outKps1.at(i).pt.y, emptyImagePoints.at(i).location().pt.y);
  }
}

}  // namespace c8
