#version 320 es
#extension GL_GOOGLE_include_directive: enable

precision mediump float;

#include "bzl/examples/glsl/shared.glsl"

uniform sampler2D camTex;
in vec2 texUV;
out vec4 fragmentColor;

void main () {
  vec4 orig = texture(camTex, texUV);
  fragmentColor = strengthenRed(orig);
}
