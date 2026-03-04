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

// TODO remove splatDataTexture since it isn't used in this shader
uniform highp usampler2D splatDataTexture;
uniform float raysToPix;
uniform float antialiased;  // 0.0 means don't use mip filter.
uniform float maxInstanceIdForFragment; // 0.0 means no culling. TODO(dat): Remove in production.

in uvec4 positionColor;
in uvec4 scaleRotation;
in uvec4 shRG;
in uvec4 shB;


// default vertex attributes provided by BufferGeometry
in vec3 position;
in vec3 normal;
in vec2 uv;

out vec4 splatColor;
out vec2 splatEccentricity;
out float maxSqEccentricity;

void main(void) {
  uvec4[5] instanceAttributes;
  instanceAttributes[0] = positionColor;
  instanceAttributes[1] = scaleRotation;
  instanceAttributes[2] = shRG;
  instanceAttributes[3] = shB;
  instanceAttributes[4] = uvec4(0, 0, 0, 0);

  unpackAndProjectSplat(
    // Inputs
    modelViewMatrix,
    projectionMatrix,
    raysToPix,
    vec4(position.xy, 0, 1),
    instanceAttributes,
    uint(0), // instance id doesn't matter for this shader
    splatDataTexture,
    1.0, // sortTexture true
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
