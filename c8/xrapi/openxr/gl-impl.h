#pragma once

#include "c8/command/command-buffer.h"
#include "c8/pixels/opengl/egl.h"
#include "c8/pixels/opengl/eglext.h"
#include "c8/pixels/opengl/gl.h"
#include "c8/pixels/opengl/glext.h"

namespace c8 {
// Use a 32kb Single Writer Single Producer CommandBuffer for OpenGL commands.
using GlBuffer = CommandBuffer<32 * 1024, CommandBufferType::SPSC>;

GlBuffer *getXrCommandBuffer();

void setXrCommandBuffer(GlBuffer *glBuffer);

void checkGLErrorBuffer(GlBuffer *glBuffer, const char *msg);

}  // namespace c8
