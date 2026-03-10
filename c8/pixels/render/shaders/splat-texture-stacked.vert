#version 320 es

// Vertex shader for rendering splats with data provided as an RGBA32UI texture.

#extension GL_GOOGLE_include_directive : enable

#include "c8/pixels/render/shaders/splat-vert.glsl"

in vec4 position;

uniform highp usampler2D splatDataTexture;
uniform highp usampler2D splatIdsTexture;
uniform mat4 mv;
uniform mat4 p;
uniform float raysToPix;
uniform float antialiased;  // 0.0 means don't use mip filter.
uniform float stackedInstanceCount; // Number of splats per mesh instance

out vec4 splatColor;
out vec2 splatEccentricity;
out float maxSqEccentricity;

void main(void) {  
  int stackedId = gl_VertexID / 4;
  int splatId = gl_InstanceID * int(stackedInstanceCount) + stackedId;

  int width = textureSize(splatIdsTexture, 0).x;
  ivec2 texelCoords = ivec2(splatId % width, splatId / width);
  uint splatInstanceId = texelFetch(splatIdsTexture, texelCoords, 0).r;

  if (splatInstanceId == uint(0xFFFFFFFF)) {
    gl_Position = BEHIND_CAM;
    return;
  }

  uvec4[5] instanceAttributes;

  unpackAndProjectSplat(
    // Inputs
    mv,
    p,
    raysToPix,
    vec4(position.xy, 0.0, 1.0),
    instanceAttributes,
    splatInstanceId,
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
