// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Christoph Bartschat (christoph@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "gl-texture-pipeline.h",
  };
  deps = {
    ":pipeline-texture",
    "//c8:c8-log",
    "//c8:map",
    "//c8/pixels/opengl:gl-texture",
    "//c8:vector",
    "//c8:exceptions",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x4eb10d0f);

#ifdef __APPLE__
#include "TargetConditionals.h"
#endif

#if JAVASCRIPT || __APPLE__ && TARGET_OS_MAC

#include <mutex>
#include "c8/c8-log.h"
#include "c8/exceptions.h"
#include "c8/pixels/pipeline/gl-texture-pipeline.h"

using namespace c8;

void GlTexturePipeline::initialize(int width, int height, int delay, int numTextures) {
  // std::lock_guard<std::mutex> lock(pipelineMutex_);

  // Check for invalid arguments:
  //  - delay can not be negative.
  //  - numTextures must be at least delay + 3 for ready/processing, processed, and frozen.
  if (numTextures < 0) {
    C8Log("[gl-texture-pipeline] %s", "numTextures can not be negative.");
    //C8_THROW_INVALID_ARGUMENT("Delay can not be negative.");
  }

  /*
  if (delay < 0) {
    C8Log("[gl-texture-pipeline] %s", "Delay can not be negative.");
    //C8_THROW_INVALID_ARGUMENT("Delay can not be negative.");
  } else if (numTextures < delay + 3) {
    C8Log("[gl-texture-pipeline] %s", "Insufficient number of textures");
    //C8_THROW_INVALID_ARGUMENT("Insufficient number of textures");
  }
  */

  // Initialize all textures
  textures_.resize(numTextures);

  for (int i = 0; i < numTextures; i++) {
    textures_[i].initialize();
    textures_[i].resize(width, height);
    // ready_.push(&textures_[i]);
  }

  /*
  for (int i = 0; i < delay; i++) {
    processing_.push(nullptr);
  }

  processed_ = nullptr;
  */
}

void GlTexturePipeline::cleanup() {
  // Clean up all textures
  std::for_each(textures_.begin(), textures_.end(), [](PipelineTexture &t) { t.cleanup(); });
}

GLuint GlTexturePipeline::getReady(int width, int height) {
  // std::lock_guard<std::mutex> lock(pipelineMutex_);
  if (next_ >= textures_.size()) {
    next_ = 0;
  }
  auto &texture = textures_[next_];
  next_++;

  if (texture.height != height || texture.width != width) {
    texture.resize(width, height);
  }

  return texture.glTexture.texture;

  /*
  if (ready_.size()) {
    auto texture = ready_.front();
    if (texture->height != height || texture->width != width) {
      texture->resize(width, height);
    }
    return texture->glTexture.texture;
  }
  // Enforcing rendered/processed order prevents this
  C8Log("[gl-texture-pipeline] %s", "No textures ready");
  return 0;
  */
  //C8_THROW("No textures ready");
}

void GlTexturePipeline::markTextureFilled() {
  /*
  std::lock_guard<std::mutex> lock(pipelineMutex_);
  if (filledLast) {
    C8Log("[gl-texture-pipeline] %s", "Out of order mark rendered: duplicate filled");
    return;
    //C8_THROW("Out of order mark rendered");
  }
  if (ready_.size()) {
    processing_.push(ready_.front());
    ready_.pop();  // pop front
    filledLast = true;
  } else {
    C8Log("[gl-texture-pipeline] %s", "No ready texture");
    //C8_THROW("No ready texture");
  }
  */
}

void GlTexturePipeline::markStageFinished() {
  /*
  std::lock_guard<std::mutex> lock(pipelineMutex_);
  if (!filledLast) {
    C8Log("[gl-texture-pipeline] %s", "Out of order mark processed: duplicate finished");
    return;
    //C8_THROW("Out of order mark processed");
  }
  if (processing_.size()) {
    if (processed_ != nullptr) {
      frozen_[processed_->glTexture.texture] = processed_;
    }
    processed_ = processing_.front();
    processing_.pop();  // pop front
    filledLast = false;
  } else {
    C8Log("[gl-texture-pipeline] %s", "No units currently processing");
    //C8_THROW("No units currently processing");
  }
  */
}

void GlTexturePipeline::release(GLuint textureId) {
  /*
  std::lock_guard<std::mutex> lock(pipelineMutex_);

  if (frozen_.find(textureId) == frozen_.end()) {
    C8Log("[gl-texture-pipeline] Can't release unreserved texture (%d)", textureId);
    return;
    //C8_THROW("Can't release unreserved texture");
  }
  ready_.push(frozen_[textureId]);
  frozen_.erase(frozen_.find(textureId));
  */
}

#endif
