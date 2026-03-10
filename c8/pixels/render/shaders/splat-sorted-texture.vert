#version 320 es

// Vertex shader for rendering splats with data provided as an RGBA32UI texture.

#extension GL_GOOGLE_include_directive : enable

#include "c8/pixels/render/shaders/splat-vert.glsl"

in vec4 position;

in uvec4 positionColor;
in uvec4 scaleRotation;
in uvec4 shRG;
in uvec4 shB;

uniform highp usampler2D splatDataTexture; // Not used in this shader.
uniform mat4 mv;
uniform mat4 p;
uniform float raysToPix;
uniform float antialiased;  // 0.0 means don't use mip filter.

out vec4 splatColor;
out vec2 splatEccentricity;
out float maxSqEccentricity;

void main(void) {
  uvec4[5] instanceAttributes;

  instanceAttributes[0] = positionColor;
  instanceAttributes[1] = scaleRotation;
  instanceAttributes[2] = shRG;
  instanceAttributes[3] = shB;
  instanceAttributes[4] = uvec4(0u, 0u, 0u, 0u);

  unpackAndProjectSplat(
    // Inputs
    mv,
    p,
    raysToPix,
    position,
    instanceAttributes,
    uint(0),  // Fake instanceId
    splatDataTexture,
    1.0,  // sortTexture = true
    1.0,  // RUF
    antialiased,
    // Outputs
    gl_Position,
    splatColor,
    splatEccentricity,
    maxSqEccentricity);
}
