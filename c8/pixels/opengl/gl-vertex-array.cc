// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)

#include "bzl/inliner/rules2.h"

cc_library {
  hdrs = {
    "gl-vertex-array.h",
  };
  deps = {
    ":client-gl",
    ":gl-headers",
    ":gl-buffer-object",
    "//c8:log",
    "//c8:exceptions",
    "//c8:map",
  };
  visibility = {
    "//visibility:public",
  };
}
cc_end(0x1364c358);

#include "c8/c8-log.h"
#include "c8/exceptions.h"
#include "c8/pixels/opengl/client-gl.h"
#include "c8/pixels/opengl/gl-version.h"
#include "c8/pixels/opengl/gl-vertex-array.h"

#if C8_OPENGL_VERSION_2
#include <GL/gl.h>
#include <GL/glext.h>

#include "c8/pixels/opengl/glext.h"
#endif

namespace c8 {

namespace {

constexpr bool ENABLE_WARNINGS = false;

template <class... Args>
void LogWarning(const char *fmt, Args &&...args) {
  if (ENABLE_WARNINGS) {
    C8Log(fmt, std::forward<Args>(args)...);
  }
}

#if C8_OPENGL_VERSION_2
static PFNGLGENVERTEXARRAYSOESPROC glGenVertexArrays = nullptr;
static PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArrays = nullptr;
static PFNGLBINDVERTEXARRAYOESPROC glBindVertexArray = nullptr;
static PFNGLVERTEXATTRIBDIVISORANGLEPROC glVertexAttribDivisor = nullptr;
static PFNGLDRAWELEMENTSINSTANCEDANGLEPROC glDrawElementsInstanced = nullptr;
static PFNGLVERTEXATTRIBIPOINTEREXTPROC glVertexAttribIPointer = nullptr;
#endif

void initExtensions() {
#if C8_OPENGL_VERSION_2
  // Magic static to prevent initialization order issues with extension functions.
  static bool isInit = false;
  if (!isInit) {
    glGenVertexArrays = (PFNGLGENVERTEXARRAYSOESPROC)clientGlGetProcAddress("glGenVertexArraysOES");
    glDeleteVertexArrays =
      (PFNGLDELETEVERTEXARRAYSOESPROC)clientGlGetProcAddress("glDeleteVertexArraysOES");
    glBindVertexArray = (PFNGLBINDVERTEXARRAYOESPROC)clientGlGetProcAddress("glBindVertexArrayOES");
    glVertexAttribDivisor =
      (PFNGLVERTEXATTRIBDIVISORANGLEPROC)clientGlGetProcAddress("glVertexAttribDivisorANGLE");
    glDrawElementsInstanced =
      (PFNGLDRAWELEMENTSINSTANCEDANGLEPROC)clientGlGetProcAddress("glDrawElementsInstancedANGLE");
    glVertexAttribIPointer =
      (PFNGLVERTEXATTRIBIPOINTEREXTPROC)clientGlGetProcAddress("glVertexAttribIPointerEXT");
  }
#endif
}

int getMaxVertexAttribs() {
  // Using hardcoded value in practice for consistency, platforms must support at least 16 vertex
  // attributes. This is how to query the maximum number of vertex attributes:
  //
  // GLint max;
  // glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max);

  return 16;
}
}  // namespace

GlVertexArray::GlVertexArray() noexcept
    : vao_(0),
      indexBuffer_(),
      vertexBuffers_(),
      interleavedBuffers_(),
      vertexAttribMap_(),
      primitive_(),
      indexType_(),
      vertexCount_(0) {
  initExtensions();
  glGenVertexArrays(1, &vao_);
}

GlVertexArray::GlVertexArray(GlVertexArray &&rhs) noexcept
    : vao_(std::move(rhs.vao_)),
      indexBuffer_(std::move(rhs.indexBuffer_)),
      vertexBuffers_(std::move(rhs.vertexBuffers_)),
      interleavedBuffers_(std::move(rhs.interleavedBuffers_)),
      vertexAttribMap_(std::move(rhs.vertexAttribMap_)),
      primitive_(std::move(rhs.primitive_)),
      indexType_(std::move(rhs.indexType_)),
      vertexCount_(std::move(rhs.vertexCount_)) {
  // Must set the vao_ back to zero to prevent destructing the moved object.
  rhs.vao_ = 0;
}
GlVertexArray &GlVertexArray::operator=(GlVertexArray &&rhs) noexcept {
  if (vao_) {
    glDeleteVertexArrays(1, &vao_);
  }
  vao_ = std::move(rhs.vao_);
  indexBuffer_ = std::move(rhs.indexBuffer_);
  vertexBuffers_ = std::move(rhs.vertexBuffers_);
  interleavedBuffers_ = std::move(rhs.interleavedBuffers_);
  vertexAttribMap_ = std::move(rhs.vertexAttribMap_);
  primitive_ = std::move(rhs.primitive_);
  indexType_ = std::move(rhs.indexType_);
  vertexCount_ = std::move(rhs.vertexCount_);

  // Must set the vao_ back to zero to prevent destructing the moved object.
  rhs.vao_ = 0;

  return *this;
}

GlVertexArray::~GlVertexArray() noexcept {
  if (vao_) {
    glDeleteVertexArrays(1, &vao_);
  }
  // the associated VBOs will be deleted automatically since they are member variables and the
  // GlBufferObject destructors unbinds them.
}

void GlVertexArray::setIndexBuffer(
  GLenum primitive, GLenum type, GLsizeiptr bufferSize, const GLvoid *bufferData, GLenum usage) {
  bind();

  primitive_ = primitive;
  indexType_ = type;

  GLsizei elementSize;
  switch (indexType_) {
    case GL_UNSIGNED_BYTE:
      elementSize = 1;
      break;
    case GL_UNSIGNED_SHORT:
      elementSize = 2;
      break;
    case GL_UNSIGNED_INT:
      elementSize = 4;
      break;
    default:
      C8_THROW_INVALID_ARGUMENT(
        "setIndexBuffer type must be GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, or GL_UNSIGNED_INT");
  }

  vertexCount_ = bufferSize / elementSize;

  indexBuffer_.set(GL_ELEMENT_ARRAY_BUFFER, bufferSize, bufferData, usage);
  indexBuffer_.bind();

  unbind();
}

void GlVertexArray::initialize(const std::vector<String> &attributes) noexcept {
  GLuint newVertexAttrib = 0;
  static int maxVertexAttribs = getMaxVertexAttribs();

  for (const auto &attribute : attributes) {
    if (newVertexAttrib >= maxVertexAttribs) {
      LogWarning("WARNING: Too many attributes for vertex array");
      break;
    }
    vertexAttribMap_[attribute] = static_cast<GlVertexAttrib>(newVertexAttrib++);
  }
}

void GlVertexArray::addVertexBuffer(
  GlVertexAttrib vertexAttrib,
  GLint numChannels,
  GLenum type,
  GLboolean normalized,
  GLsizei stride,
  GLsizeiptr bufferSize,
  const GLvoid *bufferData,
  GLenum usage) noexcept {
  bind();

  auto &vb = vertexBuffers_[vertexAttrib];

  vb.set(GL_ARRAY_BUFFER, bufferSize, bufferData, usage);
  vb.bind();

  if (type == GL_INT || type == GL_UNSIGNED_INT) {
    glVertexAttribIPointer(static_cast<GLuint>(vertexAttrib), numChannels, type, stride, 0);
  } else {
    glVertexAttribPointer(
      static_cast<GLuint>(vertexAttrib), numChannels, type, normalized, stride, 0);
  }
  glEnableVertexAttribArray(static_cast<GLuint>(vertexAttrib));

  unbind();
}

void GlVertexArray::addVertexBuffer(
  const String &attribute,
  GLint numChannels,
  GLenum type,
  GLboolean normalized,
  GLsizei stride,
  GLsizeiptr bufferSize,
  const GLvoid *bufferData,
  GLenum usage) noexcept {
  auto it = vertexAttribMap_.find(attribute);
  if (it == vertexAttribMap_.end()) {
    LogWarning("WARNING: Attribute `%s` not found in vertex attribute map", attribute.c_str());
    return;
  }

  addVertexBuffer(it->second, numChannels, type, normalized, stride, bufferSize, bufferData, usage);
}

void GlVertexArray::setDivisor(GlVertexAttrib vertexAttrib, int divisor) {
  bind();
  glVertexAttribDivisor(static_cast<GLuint>(vertexAttrib), divisor);
  unbind();
}

void GlVertexArray::setDivisor(String vertexAttrib, int divisor) {
  auto it = vertexAttribMap_.find(vertexAttrib);
  if (it == vertexAttribMap_.end()) {
    LogWarning("WARNING: Attribute `%s` not found in vertex attribute map", vertexAttrib.c_str());
    return;
  }

  setDivisor(it->second, divisor);
}

void GlVertexArray::setInterleavedBuffer(
  const int id, GLsizeiptr bufferSize, const GLvoid *bufferData, GLenum usage) {
  bind();
  auto &vb = interleavedBuffers_[id];

  vb.set(GL_ARRAY_BUFFER, bufferSize, bufferData, usage);
  vb.bind();
  unbind();
}

void GlVertexArray::setInterleavedAttribute(
  const int id,
  const String &vertexAttrib,
  GLint numChannels,
  GLenum type,
  GLboolean normalized,
  GLsizei stride,
  GLsizeiptr offset) {
  auto it = vertexAttribMap_.find(vertexAttrib);
  if (it == vertexAttribMap_.end()) {
    LogWarning("WARNING: Attribute `%s` not found in vertex attribute map", vertexAttrib.c_str());
    return;
  }

  if (!interleavedBuffers_.contains(id)) {
    LogWarning("WARNING: Interleaved buffer `%d` not found in interleaved buffer map", id);
    return;
  }

  bind();

  auto &vb = interleavedBuffers_[id];
  vb.bind();

  if (type == GL_INT || type == GL_UNSIGNED_INT) {
    glVertexAttribIPointer(
      static_cast<GLuint>(it->second), numChannels, type, stride, reinterpret_cast<void *>(offset));
  } else {
    glVertexAttribPointer(
      static_cast<GLuint>(it->second),
      numChannels,
      type,
      normalized,
      stride,
      reinterpret_cast<void *>(offset));
  }
  glEnableVertexAttribArray(static_cast<GLuint>(it->second));

  unbind();
}

void GlVertexArray::bind() const noexcept { glBindVertexArray(vao_); }

void GlVertexArray::unbind() const noexcept {
  glBindVertexArray(0);

  // also unbind the associated VBOs to maintain a clean state
  unbindBuffers();
}

void GlVertexArray::unbindBuffers() const noexcept {
  for (const auto &entry : vertexBuffers_) {
    const auto &vbo = entry.second;
    vbo.unbind();
  }

  for (const auto &entry : interleavedBuffers_) {
    const auto &vbo = entry.second;
    vbo.unbind();
  }
}

void GlVertexArray::unbindFastWithDirtyState() const noexcept { glBindVertexArray(0); }

void GlVertexArray::drawElements() const noexcept {
  glDrawElements(primitive_, vertexCount_, indexType_, 0);
}

void GlVertexArray::drawElementsInstanced(int count) const noexcept {
  glDrawElementsInstanced(primitive_, vertexCount_, indexType_, 0, count);
}

}  // namespace c8
