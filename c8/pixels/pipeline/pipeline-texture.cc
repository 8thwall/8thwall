// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Alvin Portillo (alvin@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "pipeline-texture.h",
  };
  deps = {
    "//c8/pixels/opengl:gl-texture",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0xd445132d);

#include "c8/pixels/pipeline/pipeline-texture.h"

namespace c8 {

void PipelineTexture::initialize() {
  glTexture.initialize();
}

void PipelineTexture::resize(int width, int height) {
  GLint restoreTexture = 0;
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &restoreTexture);
  glBindTexture(GL_TEXTURE_2D, glTexture.texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  glBindTexture(GL_TEXTURE_2D, restoreTexture);

  this->width = width;
  this->height = height;
}

void PipelineTexture::cleanup() { glTexture.cleanup(); }

}  // namespace c8
