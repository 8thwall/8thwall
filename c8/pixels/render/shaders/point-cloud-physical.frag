#version 320 es
#extension GL_GOOGLE_include_directive: enable

precision mediump float;

#include "c8/pixels/render/shaders/occlusion-frag.glsl"
#include "c8/pixels/render/shaders/physical-lighting-frag.glsl"

in vec4 vertColor;

uniform float opacity;

out vec4 fragColor;

void main() {
  if (length(gl_PointCoord - vec2(0.5)) > 0.5) {  //outside of circle radius
    discard;
  }

  float visibility = occlusionPointCloud();
  // Compute normal for x/y coordinates on a sphere.
  float normx = 2.0 * gl_PointCoord.x - 1.0;
  float normy = 1.0 - 2.0 * gl_PointCoord.y;
  float normz = sqrt(normx * normx + normy * normy) - 1.0;

  fragColor =
    physicalLightForNormal(vec3(normx, normy, normz)) * vertColor * vec4(1.0, 1.0, 1.0, opacity);
  fragColor.a = fragColor.a * visibility;
}
