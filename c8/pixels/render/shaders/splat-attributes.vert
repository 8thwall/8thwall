#version 320 es

// Vertex shader for rendering splats with data provided as instanced attributes.

#extension GL_GOOGLE_include_directive : enable

#include "c8/pixels/render/shaders/splat-vert.glsl"

in vec4 position;
in vec4 instancePosition;
in vec4 instanceRotation;
in vec4 instanceScale;
in vec4 instanceColor;

uniform mat4 mv;
uniform mat4 p;
uniform float raysToPix;

out vec4 splatColor;
out vec2 splatEccentricity;
out float maxSqEccentricity;

float[6] rssrCoeffs(in vec3 rot, in vec3 scale) {
  float x = rot.x;
  float y = rot.y;
  float z = rot.z;
  float w = sqrt(1.0 - dot(rot, rot));

  // R * S * S.T * R.T = [c0 c1 c2,
  //                      c1 c3 c4,
  //                      c2 c4 c5]
  // where
  // c0 = s0r00 * s0r00 + s1r01 * s1r01 + s2r02 * s2r02
  // c1 = s0r00 * s0r10 + s1r01 * s1r11 + s2r02 * s2r12
  // c2 = s0r00 * s0r20 + s1r01 * s1r21 + s2r02 * s2r22
  // c3 = s0r10 * s0r10 + s1r11 * s1r11 + s2r12 * s2r12
  // c4 = s0r10 * s0r20 + s1r11 * s1r21 + s2r12 * s2r22
  // c5 = s0r20 * s0r20 + s1r21 * s1r21 + s2r22 * s2r22
  float s0r00 = scale.x * (1.0 - 2.0 * (y * y + z * z));
  float s1r01 = scale.y * (2.0 * (x * y - w * z));
  float s2r02 = scale.z * (2.0 * (x * z + w * y));
  float s0r10 = scale.x * (2.0 * (x * y + w * z));
  float s1r11 = scale.y * (1.0 - 2.0 * (x * x + z * z));
  float s2r12 = scale.z * (2.0 * (y * z - w * x));
  float s0r20 = scale.x * (2.0 * (x * z - w * y));
  float s1r21 = scale.y * (2.0 * (y * z + w * x));
  float s2r22 = scale.z * (1.0 - 2.0 * (x * x + y * y));
  return float[6](
    s0r00 * s0r00 + s1r01 * s1r01 + s2r02 * s2r02,
    s0r00 * s0r10 + s1r01 * s1r11 + s2r02 * s2r12,
    s0r00 * s0r20 + s1r01 * s1r21 + s2r02 * s2r22,
    s0r10 * s0r10 + s1r11 * s1r11 + s2r12 * s2r12,
    s0r10 * s0r20 + s1r11 * s1r21 + s2r12 * s2r22,
    s0r20 * s0r20 + s1r21 * s1r21 + s2r22 * s2r22);
}

void main(void) {
  float ruf = 1.0; // Right Up Front
  float antialiased = 0.0;
  float coeffs[6] = rssrCoeffs(instanceRotation.xyz, instanceScale.xyz);

  projectSplat(
    // Inputs
    mv,
    p,
    raysToPix,
    position,
    instancePosition,
    instanceColor,
    coeffs,
    ruf,
    antialiased,
    // Outputs
    gl_Position,
    splatColor,
    splatEccentricity,
    maxSqEccentricity);
}
