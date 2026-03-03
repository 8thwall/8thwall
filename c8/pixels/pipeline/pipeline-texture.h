// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Alvin Portillo (alvin@8thwall.com)
//
// A resizable texture used by GlTexturePipeline. This is assumed to have
// a GL_RGBA format type.

#pragma once

#include "c8/pixels/opengl/gl-texture.h"

namespace c8 {

struct PipelineTexture {
  GlTexture glTexture;  // TODO(nb): Switch this to GlTexture2D.
  int width = 0;
  int height = 0;

  void initialize();
  void resize(int width, int height);
  void cleanup();

  // NOTE(nb): Everything from here below can be removed after switching to GlTexture2D
  PipelineTexture() noexcept = default;
  PipelineTexture(PipelineTexture &&) noexcept = default;
  PipelineTexture &operator=(PipelineTexture &&) noexcept = default;
  ~PipelineTexture() { cleanup(); } // TODO(nb): use auto-cleaning types (GlTexture2D).
  PipelineTexture(const PipelineTexture &) = delete;
  PipelineTexture &operator=(const PipelineTexture &) = delete;
};

}  // namespace c8
