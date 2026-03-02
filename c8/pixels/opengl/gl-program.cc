// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Christoph Bartschat (christoph@8thwall.com)

#include "c8/pixels/opengl/gl-program.h"

#include "bzl/inliner/rules2.h"
#include "c8/c8-log.h"
#include "c8/map.h"
#include "c8/pixels/opengl/gl-constants.h"
#include "c8/string.h"
#include "c8/vector.h"

namespace c8 {

namespace {
const char *shaderTypeToString(GLenum shaderType) {
  switch (shaderType) {
    case GL_VERTEX_SHADER:
      return "Vertex";
    case GL_FRAGMENT_SHADER:
      return "Fragment";
    default:
      return "Unknown";
  }
}

GLuint compileShader(GLenum shaderType, const char *shaderCode) {
  GLuint shader = glCreateShader(shaderType);
  glShaderSource(shader, 1, &shaderCode, nullptr);
  glCompileShader(shader);

  GLint isCompiled = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
  if (isCompiled == GL_FALSE) {
    GLint maxLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
    Vector<GLchar> errorLog(maxLength);
    glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);
    C8Log("[gl-program] %s shader compilation failed:\n%s", shaderTypeToString(shaderType),
          &errorLog[0]);
    glDeleteShader(shader);
    return 0;  // 0 indicates failure
  }
  return shader;
}

bool linkProgram(GLuint program, GLuint vertexShader, GLuint fragmentShader) {
  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);
  glLinkProgram(program);

  GLint isLinked = 0;
  glGetProgramiv(program, GL_LINK_STATUS, &isLinked);

  bool linkedSuccessfully = (isLinked != GL_FALSE);
  if (!linkedSuccessfully) {
    GLint maxLength = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
    Vector<GLchar> errorLog(maxLength);
    glGetProgramInfoLog(program, maxLength, &maxLength, &errorLog[0]);
    C8Log("[gl-program] Program link failed:\n%s", &errorLog[0]);
    glDeleteProgram(program);
  }
  // Cleanup in both cases
  glDetachShader(program, vertexShader);
  glDetachShader(program, fragmentShader);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  return linkedSuccessfully;
}

}  // namespace

String nameForStructArray(const std::string &name, int index, const std::string &property) {
  return format("%s[%d].%s", name.c_str(), index, property.c_str());
}

String nameForStruct(const std::string &name, const std::string &property) {
  return format("%s.%s", name.c_str(), property.c_str());
}

String nameForArray(const std::string &name, int index) {
  return format("%s[%d]", name.c_str(), index);
}

GlProgramObject::GlProgramObject() noexcept : program_(glCreateProgram()) {}

GlProgramObject::~GlProgramObject() noexcept {
  if (program_) {
    glDeleteProgram(program_);
  }
}

GlProgramObject::GlProgramObject(GlProgramObject &&rhs) noexcept
    : program_(std::move(rhs.program_)), uniformMap_(std::move(rhs.uniformMap_)) {
  rhs.program_ = 0;
}

GlProgramObject &GlProgramObject::operator=(GlProgramObject &&rhs) noexcept {
  if (program_) {
    glDeleteProgram(program_);
  }
  program_ = std::move(rhs.program_);
  rhs.program_ = 0;
  return *this;
}

bool GlProgramObject::initialize(const char *vertexShaderCode, const char *fragmentShaderCode,
                                 const Vector<std::pair<String, GlVertexAttrib>> &vertexAttribs,
                                 const Vector<Uniform> &uniforms) {
  GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderCode);
  GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderCode);

  for (auto &kv : vertexAttribs) {
    // Bind all vertex attribute locations.
    glBindAttribLocation(program_, static_cast<GLuint>(kv.second), kv.first.c_str());
  }

  bool linkedSuccessfully = linkProgram(program_, vertexShader, fragmentShader);
  if (!linkedSuccessfully) {
    return false;
  }

  for (const auto &u : uniforms) {
    // Set all the uniform locations.
    setUniformLocation(u);
  }
  return true;
}

bool GlProgramObject::initialize(const char *vertexShaderCode, const char *fragmentShaderCode,
                                 const Vector<String> &vertexAttribs,
                                 const Vector<Uniform> &uniforms) {
  GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderCode);
  GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderCode);

  for (int i = 0; i < vertexAttribs.size(); i++) {
    // Bind all vertex attribute locations.
    glBindAttribLocation(program_, static_cast<GLuint>(i), vertexAttribs[i].c_str());
  }

  bool linkedSuccessfully = linkProgram(program_, vertexShader, fragmentShader);
  if (!linkedSuccessfully) {
    return false;
  }

  for (const auto &u : uniforms) {
    // Set all the uniform locations.
    setUniformLocation(u);
  }
  return true;
}

void GlProgramObject::setUniformLocation(const Uniform &u) {
  if (u.properties.size() > 0 && u.size > 0) {
    // Uniform is a struct array.
    for (const auto &p : u.properties) {
      for (int i = 0; i < u.size; ++i) {
        auto n = nameForStructArray(u.name, i, p);
        uniformMap_[n] = glGetUniformLocation(program_, n.c_str());
      }
    }
  } else if (u.properties.size() > 0) {
    // Uniform is a struct.
    for (const auto &p : u.properties) {
      auto n = nameForStruct(u.name, p);
      uniformMap_[n] = glGetUniformLocation(program_, n.c_str());
    }
  } else if (u.size > 0) {
    // Uniform is an array.
    for (int i = 0; i < u.size; ++i) {
      auto n = nameForArray(u.name, i);
      uniformMap_[n] = glGetUniformLocation(program_, n.c_str());
    }
  } else {
    // Uniform is a primitive.
    uniformMap_[u.name] = glGetUniformLocation(program_, u.name.c_str());
  }
}

GLint GlProgramObject::location(const String &name) const {
  auto loc = uniformMap_.find(name);
  if (loc == uniformMap_.end()) {
    return -1;
  }
  return loc->second;
}

GLint GlProgramObject::location(const std::string &name, int index,
                                const std::string &property) const {
  return location(nameForStructArray(name, index, property));
}

GLint GlProgramObject::location(const std::string &name, const std::string &property) const {
  return location(nameForStruct(name, property));
}

GLint GlProgramObject::location(const std::string &name, int index) const {
  return location(nameForArray(name, index));
}

// GlProgram

void GlProgram::initialize(char const *vertexShaderCode, char const *fragmentShaderCode,
                           Vector<String> attributes, Vector<String> uniforms) {
  // Compile shaders into program
  GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderCode);
  GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderCode);

  program = glCreateProgram();

  // TODO(mc): This is a hack for now. We should update GlProgram to be an class and use the
  // VectorAttrib constants here instead of the location map.
  for (const String &attribute : attributes) {
    if (attribute == "position") {
      glBindAttribLocation(program, static_cast<GLuint>(GlVertexAttrib::SLOT_0), "position");
    } else if (attribute == "uv") {
      glBindAttribLocation(program, static_cast<GLuint>(GlVertexAttrib::SLOT_2), "uv");
    } else if (attribute == "normal") {
      glBindAttribLocation(program, static_cast<GLuint>(GlVertexAttrib::SLOT_1), "normal");
    } else if (attribute == "color") {
      glBindAttribLocation(program, static_cast<GLuint>(GlVertexAttrib::SLOT_7), "color");
    } else if (attribute == "tangent") {
      glBindAttribLocation(program, static_cast<GLuint>(GlVertexAttrib::SLOT_6), "tangent");
    } else if (attribute == "instancePosition") {
      glBindAttribLocation(program, static_cast<GLuint>(GlVertexAttrib::SLOT_9),
                           "instancePosition");
    } else if (attribute == "instanceRotation") {
      glBindAttribLocation(program, static_cast<GLuint>(GlVertexAttrib::SLOT_8),
                           "instanceRotation");
    } else if (attribute == "instanceScale") {
      glBindAttribLocation(program, static_cast<GLuint>(GlVertexAttrib::SLOT_10), "instanceScale");
    } else if (attribute == "instanceColor") {
      glBindAttribLocation(program, static_cast<GLuint>(GlVertexAttrib::SLOT_11), "instanceColor");
    }
  }

  bool linkedSuccessfully = linkProgram(program, vertexShader, fragmentShader);
  if (!linkedSuccessfully) {
    return;
  }

  locationMap.clear();
  for (int i = 0; i < attributes.size(); i++) {
    auto loc = glGetAttribLocation(program, attributes[i].c_str());
    locationMap.insert({attributes[i], loc});
  }
  for (int i = 0; i < uniforms.size(); i++) {
    auto loc = glGetUniformLocation(program, uniforms[i].c_str());
    locationMap.insert({uniforms[i], loc});
  }
}

GLint GlProgram::location(const String &name) const {
  auto loc = locationMap.find(name);
  if (loc == locationMap.end()) {
    return -1;
  }
  return loc->second;
}

void GlProgram::cleanup() { glDeleteProgram(program); }

}  // namespace c8
