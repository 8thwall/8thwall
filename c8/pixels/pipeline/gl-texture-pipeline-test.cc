// Copyright (c) 2017 8th Wall, Inc.
// Original Author: Christoph Bartschat (christoph@8thwall.com)
//
// Unit tests for Image Graphics Pipeline

#ifdef __APPLE__
#include "TargetConditionals.h"
#endif

#if JAVASCRIPT || __APPLE__ && TARGET_OS_MAC

#include <system_error>
#include "c8/pixels/opengl/gl.h"
#include "c8/pixels/opengl/offscreen-gl-context.h"
#include "c8/pixels/pipeline/gl-texture-pipeline.h"
#include "gtest/gtest.h"

namespace c8 {

class GlTexturePipelineTest : public ::testing::Test {};

TEST_F(GlTexturePipelineTest, TEST_EMPTY_THROW) {
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();
  GlTexturePipeline pl;
}

TEST_F(GlTexturePipelineTest, TEST_OUT_OF_ORDER_RENDERED) {
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();
  GlTexturePipeline pl;
  pl.initialize(10, 10, 0, 3);
  pl.markTextureFilled();
}

TEST_F(GlTexturePipelineTest, TEST_OUT_OF_ORDER_PROCESSED) {
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();
  GlTexturePipeline pl;
  pl.initialize(10, 10, 0, 3);
  pl.markTextureFilled();
  pl.markStageFinished();
}

TEST_F(GlTexturePipelineTest, TEST_EARLY_PROCESSED) {
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();
  GlTexturePipeline pl;
  pl.initialize(10, 10, 0, 3);
}

TEST_F(GlTexturePipelineTest, TEST_NO_DELAY) {
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();
  GlTexturePipeline pl;
  pl.initialize(10, 10, 0, 3);
  GLint readyTexture = pl.getReady(10, 10);
  pl.markTextureFilled();
  pl.markStageFinished();
}

TEST_F(GlTexturePipelineTest, TEST_ONE_DELAY) {
  OffscreenGlContext ctx = OffscreenGlContext::createRGBA8888Context();
  GlTexturePipeline pl;
  pl.initialize(10, 10, 1, 4);
  GLint readyTexture = pl.getReady(10, 10);
  pl.markTextureFilled();
  pl.markStageFinished();
  pl.markTextureFilled();
  pl.markStageFinished();
}

}  // namespace c8

#endif
