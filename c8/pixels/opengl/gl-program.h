// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Christoph Bartschat (christoph@8thwall.com)
#pragma once

#include <initializer_list>
#include <utility>

#include "c8/map.h"
#include "c8/pixels/opengl/gl-constants.h"
#include "c8/pixels/opengl/gl.h"
#include "c8/pixels/render/uniform.h"
#include "c8/string.h"
#include "c8/string/format.h"
#include "c8/vector.h"

namespace c8 {

// Class for managing a GPU program.
class GlProgramObject {
public:
  // Create the program id, but don't create the program.
  GlProgramObject() noexcept;

  // Create a GlProgramObject that does not represent a program.
  GlProgramObject(std::nullptr_t) : program_(0) {}

  // Delete the program id, which will unlink, deallocate and destroy the program.
  ~GlProgramObject() noexcept;

  // Move constructor and assignment.
  GlProgramObject(GlProgramObject &&rhs) noexcept;
  GlProgramObject &operator=(GlProgramObject &&rhs) noexcept;

  // Disallow copying.
  GlProgramObject(const GlProgramObject &) = delete;
  GlProgramObject &operator=(const GlProgramObject &) = delete;

  // Initialize the GL program, providing the vertex attribute variables and a list of uniform
  // variables.
  //
  // vertexShaderCode - The vertex shader GLSL code.
  // fragmentShaderCode - The fragment shader GLSL code.
  // vertexAttribs - list of {attribute, GlVertexAttrib} pairs,
  //                 e.g., {{"position", GlVertexAttrib::SLOT_0}, {"uv",
  //                 GlVertexAttrib::SLOT_2}}
  // uniforms - list of {uniformName, location*} pairs. location will be set to the uniform
  //            location.
  bool initialize(
    const char *vertexShaderCode,
    const char *fragmentShaderCode,
    const Vector<std::pair<String, GlVertexAttrib>> &vertexAttribs,
    const Vector<Uniform> &uniforms = Vector<Uniform>());

  // Same as above, but vertexAttribs is a list of attribute names.
  // Slotted attributes are assigned to slots starting from 0 to N.
  bool initialize(
    const char *vertexShaderCode,
    const char *fragmentShaderCode,
    const Vector<String> &vertexAttribs,
    const Vector<Uniform> &uniforms = Vector<Uniform>());

  void setUniformLocation(const Uniform &u);

  // Accessors
  GLuint id() const { return program_; }
  GLint location(const String &name) const;
  // Location for an array of structs.
  GLint location(const String &name, int index, const std::string &property) const;
  // Location for a struct.
  GLint location(const String &name, const std::string &property) const;
  // Location for an array.
  GLint location(const String &name, int index) const;

private:
  GLuint program_;
  TreeMap<String, GLuint> uniformMap_;
};

// This struct is harder to use than the class version above. Erik would like to deprecate and
// remove this class in favor of GlProgramObject.
struct GlProgram {
  GLuint program = 0;
  TreeMap<String, GLuint> locationMap;

  void initialize(
    char const *vertexShaderCode,
    char const *fragmentShaderCode,
    Vector<String> attributes,
    Vector<String> uniforms);
  GLint location(const String &name) const;
  void cleanup();
};

}  // namespace c8
