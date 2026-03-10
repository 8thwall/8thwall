// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_test {
  size = "small";
  deps = {
    ":gr8-feature-shader",
    "//c8:c8-log",
    "//c8/pixels:gl-pixels",
    "//c8/pixels/opengl:gl-framebuffer",
    "//c8/pixels/opengl:gl-quad",
    "//c8/pixels/opengl:gl-texture",
    "//c8/pixels/opengl:offscreen-gl-context",
    "//c8/pixels:pixel-buffer",
    "//c8/pixels:pixel-transforms",
    "@com_google_googletest//:gtest_main",
  };
}
cc_end(0xdd008f74);

#include <queue>
#include <vector>

#include "c8/c8-log.h"
#include "c8/pixels/gl-pixels.h"
#include "c8/pixels/opengl/gl-framebuffer.h"
#include "c8/pixels/opengl/gl-quad.h"
#include "c8/pixels/opengl/gl-texture.h"
#include "c8/pixels/opengl/offscreen-gl-context.h"
#include "c8/pixels/pixel-buffer.h"
#include "c8/pixels/pixel-transforms.h"
#include "gtest/gtest.h"
#include "reality/engine/features/gr8-feature-shader.h"
#include "c8/stats/scope-timer.h"

namespace c8 {

class Gr8FeatureShaderTest : public ::testing::Test {};

// Expensive direct access method to pixels.
uint8_t *pix(RGBA8888PlanePixels im, int r, int c) {
  return im.pixels() + r * im.rowBytes() + (c << 2);
}

int clip(int val, int min, int max) { return val < min ? min : (val >= max ? max - 1 : val); }

RGBA8888PlanePixelBuffer drawImageToImage(
  ConstRGBA8888PlanePixels in,
  Gr8FeatureShader *shaders,
  const GlProgramObject *shader) {
  auto width = in.cols();
  auto height = in.rows();
  auto src = readImageToNearestTexture(in);

  auto dest = makeNearestRGBA8888Framebuffer(width, height);

  GlSubRect rect{0, 0, width, height, width, height};
  auto quad = makeVertexArrayRect(rect, rect);

  src.bind();
  shaders->bindAndSetParams(shader);
  dest.bind();
  quad.bind();
  glFrontFace(GL_CCW);
  glViewport(0, 0, width, height);
  glUniform2f(shader->location("scale"), 1.0 / width, 1.0 / height);
  quad.drawElements();

  RGBA8888PlanePixelBuffer output(height, width);
  dest.tex().bind();
  glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, output.pixels().pixels());

  EXPECT_EQ(glGetError(), GL_NO_ERROR);
  return output;
}

TEST_F(Gr8FeatureShaderTest, TestShader2Fast) {
  // Create an offscreen gl context.
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();
  ScopeTimer rt("test");
  Gr8FeatureShader shaders;
  shaders.initialize();

  // Load the source pixels into a gl texture.
  RGBA8888PlanePixelBuffer input(128, 128);
  auto in = input.pixels();
  fill(0, 0, 0, 0, &in);

  RGBA8888PlanePixelBuffer expected(in.rows(), in.cols());
  auto exp = expected.pixels();
  fill(0, 0, 0, 0, &exp);

  // Draw a 7x7 box in around pixel (64, 96)
  for (int r = -3; r < 4; ++r) {
    for (int c = -3; c < 4; ++c) {
      pix(in, r + 64, c + 96)[0] = 100;
      pix(exp, r + 64, c + 96)[0] = 100;
      // We expect the points around the corner points of each box corner will be fast points.
      if ((std::abs(r) + std::abs(c)) >= 4) {
        pix(exp, r + 64, c + 96)[2] = 100;
      }
    }
  }

  // The output g values need to be row-filtered versions of the input r values.
  for (int r = 0; r < in.rows(); ++r) {
    for (int c = 0; c < in.cols(); ++c) {
      std::array<float, 7> vals{};
      for (int co = -3; co <= 3; ++co) {
        vals[co + 3] = pix(in, r, clip(c + co, 0, in.cols()))[0];
      }
      float ev = 0.070159f * vals[0] + 0.131075f * vals[1] + 0.190713f * vals[2]
        + 0.216106f * vals[3] + 0.190713f * vals[4] + 0.131075f * vals[5] + 0.070159f * vals[6];
      pix(exp, r, c)[1] = static_cast<uint8_t>(std::round(ev));
    }
  }

  // Run the shader and get the output.
  auto outBuf = drawImageToImage(input.pixels(), &shaders, shaders.shader2());
  auto out = outBuf.pixels();

  // Compare expected output to actual.
  for (int r = 0; r < out.rows(); ++r) {
    for (int c = 0; c < out.cols(); ++c) {
      const auto *op = pix(out, r, c);
      const auto *ep = pix(exp, r, c);
      EXPECT_EQ(op[0], ep[0]) << "Error at pixel (" << r << ", " << c << ").r";
      EXPECT_EQ(op[1], ep[1]) << "Error at pixel (" << r << ", " << c << ").g";
      EXPECT_EQ(op[2], ep[2]) << "Error at pixel (" << r << ", " << c << ").b";
      EXPECT_EQ(op[3], ep[3]) << "Error at pixel (" << r << ", " << c << ").a";
    }
  }
}

TEST_F(Gr8FeatureShaderTest, TestShader3Nms) {
  // Create an offscreen gl context.
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();
  ScopeTimer rt("test");
  Gr8FeatureShader shaders;
  shaders.initialize();

  // Load the source pixels into a gl texture.
  RGBA8888PlanePixelBuffer input(1024, 1024);
  auto in = input.pixels();
  fill(0, 0, 0, 0, &in);

  RGBA8888PlanePixelBuffer expected(in.rows(), in.cols());
  auto exp = expected.pixels();
  fill(0, 0, 0, 0, &exp);

  // Fill the r values with arbitrary data; this should get echoed verbatim to the output.
  for (int r = 0; r < in.rows(); ++r) {
    for (int c = 0; c < in.cols(); ++c) {
      pix(in, r, c)[0] = c % 256;
      pix(in, r, c)[1] = r % 256;
      pix(exp, r, c)[0] = pix(in, r, c)[0];
      pix(exp, r, c)[3] = 0x77;
    }
  }

  // The output g values need to be column filtered versions of the input g values.
  for (int r = 0; r < in.rows(); ++r) {
    for (int c = 0; c < in.cols(); ++c) {
      std::array<float, 7> vals{};
      for (int ro = -3; ro <= 3; ++ro) {
        vals[ro + 3] = pix(in, clip(r + ro, 0, in.rows()), c)[1];
      }
      float ev = 0.070159f * vals[0] + 0.131075f * vals[1] + 0.190713f * vals[2]
        + 0.216106f * vals[3] + 0.190713f * vals[4] + 0.131075f * vals[5] + 0.070159f * vals[6];
      pix(exp, r, c)[1] = static_cast<uint8_t>(std::round(ev));
    }
  }

  // Draw a ramp from 0->255->0 in row 10. There should be one max at column 255.
  {
    int r = 10;
    for (int c = 0; c < 256; ++c) {
      pix(in, r, c)[2] = c;
    }
    for (int c = 256; c < 510; ++c) {
      pix(in, r, c)[2] = 510 - c;
    }

    pix(exp, r, 255)[2] = 255;
  }

  // Draw a ramp from 0->127->0 in column 600. There should be one max at row 127.
  {
    int c = 600;
    for (int r = 0; r < 128; ++r) {
      pix(in, r, c)[2] = r;
    }
    for (int r = 128; r < 254; ++r) {
      pix(in, r, c)[2] = 254 - r;
    }

    pix(exp, 127, c)[2] = 127;
  }

  // Draw a ramp from 0->255->252->255->0 in column 610. There should be two maxes at 255 and 261.
  {
    int c = 610;
    for (int r = 0; r < 256; ++r) {
      pix(in, r, c)[2] = r;
    }
    for (int r = 256; r < 259; ++r) {
      pix(in, r, c)[2] = 510 - r;
    }
    for (int r = 259; r < 262; ++r) {
      pix(in, r, c)[2] = r - 6;
    }
    for (int r = 262; r < 517; ++r) {
      pix(in, r, c)[2] = 516 - r;
    }

    pix(exp, 255, c)[2] = 255;
    pix(exp, 261, c)[2] = 255;
  }

  // Run the shader and get the output.
  auto outBuf = drawImageToImage(input.pixels(), &shaders, shaders.shader3());
  auto out = outBuf.pixels();

  // Compare expected output to actual.
  for (int r = 0; r < out.rows(); ++r) {
    for (int c = 0; c < out.cols(); ++c) {
      const auto *op = pix(out, r, c);
      const auto *ep = pix(exp, r, c);
      EXPECT_EQ(op[0], ep[0]) << "Error at pixel (" << r << ", " << c << ").r";
      EXPECT_EQ(op[1], ep[1]) << "Error at pixel (" << r << ", " << c << ").g";
      EXPECT_EQ(op[2], ep[2]) << "Error at pixel (" << r << ", " << c << ").b";
      EXPECT_EQ(op[3], ep[3]) << "Error at pixel (" << r << ", " << c << ").a";
    }
  }
}

}  // namespace c8
