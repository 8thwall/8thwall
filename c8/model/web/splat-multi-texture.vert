#version 320 es

#extension GL_GOOGLE_include_directive : enable

#include "../../pixels/render/shaders/splat-vert.glsl"

// THREEJS built-in uniforms
// = object.matrixWorld
uniform mat4 modelMatrix;
// = camera.matrixWorldInverse * object.matrixWorld
uniform mat4 modelViewMatrix;
// = camera.projectionMatrix
uniform mat4 projectionMatrix;
// = camera.matrixWorldInverse
uniform mat4 viewMatrix;
// = inverse transpose of modelViewMatrix
uniform mat3 normalMatrix;
// = camera position in world space
uniform vec3 cameraPosition;

uniform highp usampler2D orderedSplatIdsTexture;
uniform highp usampler2D positionColorTexture;
uniform highp usampler2D rotationScaleTexture;
uniform highp usampler2D shRGTexture;
uniform highp usampler2D shBTexture;
uniform float raysToPix;
uniform float antialiased;  // 0.0 means don't use mip filter.
uniform float maxInstanceIdForFragment; // 0.0 means no culling. TODO(dat): Remove in production.
uniform float numSplatsPerInstance; // Number of splats per mesh instance

// default vertex attributes provided by BufferGeometry
in vec3 position;
in vec3 normal;
in vec2 uv;

out vec4 splatColor;
out vec2 splatEccentricity;
out float maxSqEccentricity;

void main(void) {

  int splatId = int(gl_InstanceID) * int(numSplatsPerInstance) + int(position.z);
  if (splatId == 0xFFFFFFFF) {
    gl_Position = BEHIND_CAM;
    return;
  }

  int width = textureSize(orderedSplatIdsTexture, 0).x;
  ivec2 texelCoords = ivec2(splatId % width, splatId / width);
  uint splatInstanceId = texelFetch(orderedSplatIdsTexture, texelCoords, 0).r;

  unpackAndProjectSplatMultiTex(
    // Inputs
    modelViewMatrix,
    projectionMatrix,
    raysToPix,
    vec4(position.xy, 0, 1),
    splatInstanceId,
    positionColorTexture,
    rotationScaleTexture,
    shRGTexture,
    shBTexture,
    -1.0,  // RUF to RUB
    antialiased,
    // Outputs
    gl_Position,
    splatColor,
    splatEccentricity,
    maxSqEccentricity);

  // Put vertex outside of clip space if it's above our max instance id.
  // To be used to debug fragment shader performance.
  // For Kate Sessions, only processing 10% gets us 30fps while 100% gets us 24fps.
  if (maxInstanceIdForFragment > 0.0 && float(gl_InstanceID) > maxInstanceIdForFragment) {
    gl_Position = BEHIND_CAM;
  }
}
