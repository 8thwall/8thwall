// Copyright (c) 2018 8th Wall, Inc.
// Original Author: Nicholas Butko (nb@8thwall.com)

#pragma once

#include "c8/pixels/opengl/gl-version.h"
#include "reality/engine/faces/face-roi-shader.h"

#define FRAGMENT_SHADER_PRECISION "precision mediump float;\n"

/**************** VERTEX_SHADER_CODE *******************/

#if C8_OPENGL_VERSION_3
#define VERTEX_SHADER_DEFS \
  R"(
in vec3 position;
in vec2 uv;
out vec2 texUv;
)"
#else
#define VERTEX_SHADER_DEFS \
  R"(
attribute vec3 position;
attribute vec2 uv;
varying vec2 texUv;
)"
#endif

#define VERTEX_SHADER_BODY \
  R"(
void main() {
  gl_Position = vec4(position, 1.0);
  texUv = uv;
}
)"

/**************** ROTATE_SHADER_CODE *******************/

#if C8_OPENGL_VERSION_3
#define ROTATE_SHADER_DEFS \
  R"(
in vec3 position;
in vec2 uv;
out vec2 texUv;
uniform int rotation;
uniform int flipY;
uniform mat4 mvp;
)"
#else  // OpenGL 2
#define ROTATE_SHADER_DEFS \
  R"(
attribute vec3 position;
attribute vec2 uv;
varying vec2 texUv;
uniform int rotation;
uniform int flipY;
uniform mat4 mvp;
)"
#endif

#define ROTATE_SHADER_BODY \
  R"(
void main() {
  gl_Position = mvp * vec4(position, 1.0);
  vec2 fixUv = uv;
  if (flipY > 0) {
    fixUv.y = 1.0 - fixUv.y;
  }
  if (rotation == -90 || rotation == 270) {
    texUv = vec2(1.0 - fixUv.y, fixUv.x);
  } else if (rotation == 180 || rotation == -180) {
    texUv = vec2(1.0 - fixUv.x, 1.0 - fixUv.y);
  } else if (rotation == -270 || rotation == 90) {
    texUv = vec2(fixUv.y, 1.0 - fixUv.x);
  } else {
    texUv = fixUv;
  }
}
)"

/**************** STAGE_1_SHADER_CODE *******************/

#if C8_OPENGL_VERSION_3
#define STAGE_1_SHADER_DEFS \
  R"(
in vec2 texUv;
uniform sampler2D sampler;
uniform int clear;
vec3 texValueRGB() {
  if (clear == 0) {
    return texture(sampler, texUv).rgb;
  } else {
    return vec3(0.0, 0.0, 0.0);
  }
}
)"
#else  // OpenGL 2
#define STAGE_1_SHADER_DEFS \
  R"(
varying vec2 texUv;
uniform sampler2D sampler;
uniform int clear;
vec3 texValueRGB() {
  if (clear == 0) {
    return texture2D(sampler, texUv).rgb;
  } else {
    return vec3(0.0, 0.0, 0.0);
  }
}
)"
#endif

#define STAGE_1_SHADER_BODY \
  R"(
vec4 main_() {
  vec4 color;
  vec3 rgb = texValueRGB();
  color.r = rgb.r;
  color.g = rgb.g;
  color.b = rgb.b;
  color.a = 1.0;
  return color;
}
)"

/******* generic main for fragment shaders */

#if C8_OPENGL_VERSION_3
#define FRAGMENT_SHADER_MAIN \
  R"(
out vec4 fragColor;
void main() {
  fragColor = main_();
}
)"
#else  // OpenGL 2
#define FRAGMENT_SHADER_MAIN \
  R"(
void main() {
  gl_FragColor = main_();
}
)"
#endif

namespace c8 {

char const *FaceRoiShader::VERTEX_SHADER_CODE =
  C8_GLSL_VERSION_LINE VERTEX_SHADER_DEFS VERTEX_SHADER_BODY;

char const *FaceRoiShader::ROTATE_SHADER_CODE =
  C8_GLSL_VERSION_LINE ROTATE_SHADER_DEFS ROTATE_SHADER_BODY;

char const *FaceRoiShader::STAGE_1_SHADER_CODE = C8_GLSL_VERSION_LINE FRAGMENT_SHADER_PRECISION
  STAGE_1_SHADER_DEFS STAGE_1_SHADER_BODY FRAGMENT_SHADER_MAIN;

}  // namespace c8
