#version 320 es

// Vertex shader for rendering splats with data provided as an RGBA32UI texture.

#extension GL_GOOGLE_include_directive : enable

#include "c8/pixels/render/shaders/splat-vert.glsl"

in vec4 position;

uniform highp usampler2D splatIdsTexture;
uniform highp usampler2D positionColorTexture;
uniform highp usampler2D rotationScaleTexture;
uniform sampler2D shColorTexture;

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
  ivec2 idTexelCoords = ivec2(splatId % width, splatId / width);
  uint instanceId = texelFetch(splatIdsTexture, idTexelCoords, 0).r;

  if (instanceId == uint(0xFFFFFFFF)) {
    gl_Position = BEHIND_CAM;
    return;
  }

  int splatsPerRow = textureSize(positionColorTexture, 0).x;
  int col = (int(instanceId) % splatsPerRow);
  int row = int(instanceId) / splatsPerRow;
  ivec2 dataTexelCoords = ivec2(col, row);

  uvec4 positionColor = texelFetch(positionColorTexture, dataTexelCoords, 0);
  uvec4 scaleRotation = texelFetch(rotationScaleTexture, dataTexelCoords, 0);
  vec4 instanceColor = 255.0 * texelFetch(shColorTexture, dataTexelCoords, 0);

  vec4 instancePosition = unpackPosition(positionColor);
  float coeffs[6] = rssrCoeffs(scaleRotation);

  projectSplat(
    // Inputs
    mv,
    p,
    raysToPix,
    vec4(position.xy, 0.0, 1.0),
    instancePosition,
    instanceColor,
    coeffs,
    1.0,  // RUF
    antialiased,
    // Outputs
    gl_Position,
    splatColor,  // instanceColor already has SH applied, but may have its alpha adjusted.
    splatEccentricity,
    maxSqEccentricity);
}
