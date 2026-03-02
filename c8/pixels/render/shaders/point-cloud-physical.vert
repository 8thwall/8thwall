#version 320 es
#extension GL_GOOGLE_include_directive : enable

#include "c8/pixels/render/shaders/geometry.glsl"
#include "c8/pixels/render/shaders/occlusion-vert.glsl"
#include "c8/pixels/render/shaders/physical-lighting-vert.glsl"

in vec4 position;
in vec4 color;
in vec4 instancePosition;
in vec4 instanceRotation;
in vec4 instanceScale;

// Renderer uniforms.
uniform float raysToPix;
uniform mat4 mv;
uniform mat4 mvp;

// Material uniforms.
uniform float pointSize;

out vec4 vertColor;

void main() {
  // Compute physical lighting vertex params without normal or normalMatrix. A point represents a
  // whole sphere, which always has some point facing the viewer.  The fragment shader will compute
  // the normal analytically at each point in the fragment.
  mat4 emptyMat;
  vec4 emptyVec;
  physicalLighting(position, emptyVec, mv, emptyMat);

  vec4 mvPosition = mv * position;
  float mvz = mvPosition.z / mvPosition.w;
  if (raysToPix == 1.0) {
    mvz = 1.0;
  }
  vec4 instancePos = trs(instancePosition, instanceRotation, instanceScale) * position;
  gl_Position = mvp * instancePos;
  float ptSize = pointSize / mvz;
  gl_PointSize = ptSize * raysToPix;
  occlusionPointCloud(mvp, mv, instancePos, ptSize);
  float colorScale = 1.0 / 255.0;
  vertColor = color * colorScale;
}
