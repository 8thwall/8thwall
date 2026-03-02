// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Christoph Bartschat (christoph@8thwall.com)

#pragma once

#ifdef __APPLE__
#include "TargetConditionals.h"
#endif

#if JAVASCRIPT || __APPLE__ && TARGET_OS_MAC

#include <mutex>
#include <queue>
#include "c8/map.h"
#include "c8/pixels/opengl/gl.h"
#include "c8/pixels/pipeline/pipeline-texture.h"
#include "c8/vector.h"

namespace c8 {

class GlTexturePipeline {
public:
  void initialize(int width, int height, int delay, int numTextures);
  void cleanup();

  /**
   * Returns the ID of a texture ready to be written to.
   *
   * If the provided width / height does not match the initialized width / height,
   * the texture will be resized accordingly.
   */
  GLuint getReady(int width, int height);

  /**
   * Marks ready texture as filled with data, and marks it as being processed.
   */
  void markTextureFilled();

  /**
   * Notifies pipeline of a completed processing stage. Moves pipeline forward by one step.
   */
  void markStageFinished();

  /**
   * Notifies pipeline that a texture is no longer needed and can be recycled.
   */
  void release(GLuint textureId);

private:
  Vector<PipelineTexture> textures_;
  /*
  std::mutex pipelineMutex_;
  std::queue<PipelineTexture *> ready_;
  std::queue<PipelineTexture *> processing_;
  PipelineTexture *processed_;
  TreeMap<GLuint, PipelineTexture *> frozen_;
  bool filledLast = false;
  */
  int next_ = 0;
};

}  // namespace c8

#endif
