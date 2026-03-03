#version 320 es

// Vertex shader for rendering splats with data provided as an RGBA32UI texture.

#extension GL_GOOGLE_include_directive : enable

#include "c8/pixels/render/shaders/splat-vert.glsl"

in vec4 position;
in uint instanceId;

uniform highp usampler2D splatDataTexture;
uniform mat4 mv;
uniform mat4 p;
uniform float raysToPix;
uniform float antialiased;  // 0.0 means don't use mip filter.

out vec4 splatColor;
out vec2 splatEccentricity;
out float maxSqEccentricity;

void main(void) {
  uvec4[5] instanceAttributes;

  unpackAndProjectSplat(
    // Inputs
    mv,
    p,
    raysToPix,
    position,
    instanceAttributes,
    instanceId,
    splatDataTexture,
    0.0,  // sortTexture = false
    1.0,  // RUF
    antialiased,
    // Outputs
    gl_Position,
    splatColor,
    splatEccentricity,
    maxSqEccentricity);
}
