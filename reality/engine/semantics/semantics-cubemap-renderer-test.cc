// Copyright (c) 2022 8th Wall, Inc.
// Original Author: Yuyan Song (yuyansong@nianticlabs.com)
//
// Tests the semantic sky cubemap renderer.

#include "bzl/inliner/rules2.h"

cc_test {
  deps = {
    "//c8/io:image-io",
    "//c8/io:file-io",
    "//c8/pixels:draw2",
    "//c8/pixels:gl-pixels",
    "//c8/pixels:gpu-pixels-resizer",
    "//c8/pixels/opengl:offscreen-gl-context",
    "//reality/engine/deepnets:multiclass-operations",
    "//reality/engine/semantics:semantics-classifier",
    "//reality/engine/semantics:semantics-cubemap-renderer",
    "@com_google_googletest//:gtest_main",
  };

  testonly = 1;
}
cc_end(0x91b3b324);

#include "c8/io/file-io.h"
#include "c8/io/image-io.h"
#include "c8/pixels/draw2.h"
#include "c8/pixels/gl-pixels.h"
#include "c8/pixels/gpu-pixels-resizer.h"
#include "c8/pixels/opengl/offscreen-gl-context.h"
#include "c8/stats/scope-timer.h"
#include "gtest/gtest.h"
#include "reality/engine/semantics/semantics-cubemap-renderer.h"

namespace c8 {

class SemanticsCubemapRendererTest : public ::testing::Test {};

TEST_F(SemanticsCubemapRendererTest, SemanticsCubemap) {
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();
  // Generate semantics and corresponding class map from the correct dimension.
  ScopeTimer t("test");

  SemanticsCubemapRenderer semanticsCubemapRenderer;
  RGBA8888PlanePixelBuffer buffer(10, 10);
  RGBA8888PlanePixels pix = buffer.pixels();
  c8::fill(Color::CHERRY, pix);
  semanticsCubemapRenderer.updateSrcTexture(buffer.pixels());
  EXPECT_NE(semanticsCubemapRenderer.maskTex().id(), 0);
}

}  // namespace c8
