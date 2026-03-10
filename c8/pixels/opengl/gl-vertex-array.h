// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Erik Murphy-Chutorian (mc@8thwall.com)
//
// A provides object semantics for OpenGL buffer objects, such as Pixel Buffers
// Objects and Vertex Buffer Objects. It provides memory management for buffer names, and methods
// for binding and memory mapping.

#pragma once

#include <cstddef>

#include "c8/map.h"
#include "c8/pixels/opengl/gl-buffer-object.h"
#include "c8/pixels/opengl/gl-constants.h"
#include "c8/pixels/opengl/gl.h"
#include "c8/string.h"
#include "c8/vector.h"

namespace c8 {

class GlVertexArray {
public:
  // Generates the vertex array name, but doesn't create (bind) the vertex array.
  GlVertexArray() noexcept;

  // Create a GlVertexArray object which does not represent a vertex array.
  GlVertexArray(std::nullptr_t) : vao_(0), indexBuffer_(nullptr) {}

  // Move constructor and assignment.
  GlVertexArray(GlVertexArray &&) noexcept;
  GlVertexArray &operator=(GlVertexArray &&) noexcept;

  // Disallow copying.
  GlVertexArray(const GlVertexArray &) = delete;
  GlVertexArray &operator=(const GlVertexArray &) = delete;

  // Destroys the vertex array and deletes the vertex array name.
  ~GlVertexArray() noexcept;

  // Create and attach a new index buffer object to the vertex array. If this attribute is already
  // set, the values will be updated, allocating new bufferdata underneath if needed.
  //
  // primitive - type of primitive: GL_POINTS, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES,
  // GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN,
  //             GL_TRIANGLES, GL_QUAD_STRIP, GL_QUADS, and GL_POLYGON.
  // type - type of the indices. Must be one of GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, or
  // GL_UNSIGNED_INT. bufferSize - size in bytes of the entire buffer passed into bufferData. Must
  // be divisible by the size of type. bufferData - pointer to data that will be copied into the
  // buffer or nullptr if no data is to be
  //              copied.
  // usage - buffer object usage string, e.g. GL_STATIC_DRAW, GL_DYNAMIC_DRAW.
  void setIndexBuffer(
    GLenum primitive, GLenum type, GLsizeiptr bufferSize, const GLvoid *bufferData, GLenum usage);

  // Initialize the vertex array object with the provided attributes. This will set up the vertex
  // attribute map for the vertex array object. This should be called before any calls to
  // addVertexBuffer.
  // Initializing more than GL_MAX_VERTEX_ATTRIBS attributes will print a warning and truncate the
  // list.
  void initialize(const Vector<String> &attributes) noexcept;

  // Create and attach a new vertex buffer object to the vertex array at the provided vertex
  // attribute which will be enabled after this call. If this attribute is already set, the values
  // will be updated, allocating new bufferdata underneath if needed. After this call, the vertex
  // array will be unbound.
  //
  // vertexAttrib - specify the Vertex attribute type (POSITION, NORMAL, TEXCOORD0, TEXCOORD1,
  //                TEXCOORD2, TEXCOORD3, TANGENT, COLOR);
  // numChannels - number of coordinates in the vertex attribute, (1, 2, 3, 4, or GL_BGRA).
  // type - data type in each array component, e.g. GL_BYTE, GL_UNSIGNED_BYTE, GL_FLOAT.
  // normalized -  specifies whether fixed-point data values should be normalized (GL_TRUE) or
  //               converted directly as fixed-point values (GL_FALSE) when they are accessed.
  // stride - byte offset between consecutive generic vertex attributes. Use 0 to mean
  //          tightly-packed.
  // bufferSize - size in bytes of the entire buffer passed into bufferData.
  // bufferData - pointer to data that will be copied into the buffer or nullptr if no data is to
  //              be copied.
  // usage - buffer object usage string, e.g. GL_STATIC_DRAW, GL_DYNAMIC_DRAW.
  void addVertexBuffer(
    GlVertexAttrib vertexAttrib,
    GLint numChannels,
    GLenum type,
    GLboolean normalized,
    GLsizei stride,
    GLsizeiptr bufferSize,
    const GLvoid *bufferData,
    GLenum usage) noexcept;

  // Same as above, but uses a string attribute name instead of a GlVertexAttrib enum.
  // Make sure to call initialize() before calling this function.
  // This function will print a warning if the attribute is not found in the vertex attribute map,
  // and will not add the buffer.
  void addVertexBuffer(
    const String &attribute,
    GLint numChannels,
    GLenum type,
    GLboolean normalized,
    GLsizei stride,
    GLsizeiptr bufferSize,
    const GLvoid *bufferData,
    GLenum usage) noexcept;

  // Sets the divisor for the specified vertex attribute.  This is used for instanced rendering.
  // By default, the divisor is 0, which means that the attribute is used for every instance.  If
  // the divisor is non-zero, then the attribute is used for n consecutive instances.  Typically you
  // would call this to set the divisor to 1, which means each instance gets a different value
  // for this attribute.
  void setDivisor(GlVertexAttrib vertexAttrib, int divisor);

  // Same as above, but uses a string attribute name instead of a GlVertexAttrib enum.
  void setDivisor(String vertexAttrib, int divisor);

  // Add an interleaved buffer to the vertex array object.
  // Client supplied id is used to identify the interleaved buffer.
  void setInterleavedBuffer(
    const int id, GLsizeiptr bufferSize, const GLvoid *bufferData, GLenum usage);

  // Set the interleaved attribute for the interleaved buffer.
  // Client supplied id is used to identify the interleaved buffer.
  // Make sure to call initialize() before calling this function.
  void setInterleavedAttribute(
    const int id,
    const String &vertexAttrib,
    GLint numChannels,
    GLenum type,
    GLboolean normalized,
    GLsizei stride,
    GLsizeiptr offset);

  // Bind the buffer object.
  void bind() const noexcept;

  // Unbind the buffer object.
  void unbind() const noexcept;

  // Unbind the buffer objects (VBOs) associated with this GLVertexArray (VAO)
  void unbindBuffers() const noexcept;

  // Unbinds the VAO but keeps the associated VBOs still attached to the GL state.  You most likely
  // shouldn't need to call this function unless you are doing an extreme optimization where we
  // want to avoid an unecessary vbo.unbind().  Our goal is to maintain a pristine GL state, so
  // when we unbind the VAO we should automatically unbind its VBOs.
  void unbindFastWithDirtyState() const noexcept;

  // Draw the vertex array object.
  void drawElements() const noexcept;

  // Draw the vertex array object using instancing.
  void drawElementsInstanced(int count) const noexcept;

  // Accessors
  GLenum primitive() const { return primitive_; }
  GLenum indexType() const { return indexType_; }
  GLsizei vertexCount() const { return vertexCount_; }

private:
  GLuint vao_;
  GlBufferObject indexBuffer_;
  TreeMap<GlVertexAttrib, GlBufferObject> vertexBuffers_;
  TreeMap<int, GlBufferObject> interleavedBuffers_;
  TreeMap<String, GlVertexAttrib> vertexAttribMap_;
  GLenum primitive_;
  GLenum indexType_;
  GLsizei vertexCount_;
};

}  // namespace c8
