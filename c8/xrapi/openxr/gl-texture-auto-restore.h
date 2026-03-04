#pragma once

#include "c8/c8-log.h"
#include "c8/pixels/opengl/gl.h"

#define DEBUG_GL_SAFE_TEXTURE_BIND false

#if DEBUG_GL_SAFE_TEXTURE_BIND
#include <string>
#endif

namespace c8 {

/**
 * This class is needed to maintain the original texture binding when binding a new texture.
 *
 * Specifically, headless-gl might expect a different texture to be bound.
 * This class only handles 2D textures for now.
 *
 * Example usage:
 * {
 *   TextureAutoRestore textureAutoRestore;
 *   glBindTexture(GL_TEXTURE_2D, newTexture);
 *   ... work with newTexture
 * }  // original texture binding will be restored
 *
 * Note: There are OpenXR APIs that will call into OpenGL and do work on textures.
 */
template <typename CommandBuffer>
class TextureAutoRestore {
public:
#if DEBUG_GL_SAFE_TEXTURE_BIND
  TextureAutoRestore(CommandBuffer *cb, const char *callerFunction = __builtin_FUNCTION())
      : cb_(cb), callerFunction_(callerFunction) {
#else
  TextureAutoRestore(CommandBuffer *cb) : cb_(cb) {
#endif
    cb_->runSyncCommand(glGetIntegerv, GL_TEXTURE_BINDING_2D, &originalTexture_);

#if DEBUG_GL_SAFE_TEXTURE_BIND
    cb_->queueCommand(
      C8Log,
      "TextureAutoRestore: Saving current texture binding: %d, called from: %s",
      originalTexture_,
      callerFunction_.c_str());
#endif
  }

  ~TextureAutoRestore() {
#if DEBUG_GL_SAFE_TEXTURE_BIND
    cb_->queueCommand(
      C8Log, "TextureAutoRestore: Restoring texture binding to %d", originalTexture_);
#endif
    cb_->queueCommand(glBindTexture, GL_TEXTURE_2D, originalTexture_);
  }

private:
  CommandBuffer *cb_;
  GLint originalTexture_;
#if DEBUG_GL_SAFE_TEXTURE_BIND
  std::string callerFunction_;
#endif
};

}  // namespace c8
