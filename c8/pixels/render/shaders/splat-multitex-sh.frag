#version 320 es
#extension GL_GOOGLE_include_directive : enable

precision highp float;

#include "c8/pixels/render/shaders/splat-vert.glsl"

in vec2 texUv;

uniform mat4 mv;
uniform highp usampler2D positionColorTexture;
uniform highp usampler2D shRGTexture;
uniform highp usampler2D shBTexture;

out vec4 fragColor;

void main() {
  ivec2 texEl = ivec2(texUv * vec2(textureSize(positionColorTexture, 0)));
  uvec4 positionColorRaw = texelFetch(positionColorTexture, texEl, 0);
  uvec4 shRG = texelFetch(shRGTexture, texEl, 0);
  uvec4 shB = texelFetch(shBTexture, texEl, 0);

  vec4 splatPos = unpackPosition(positionColorRaw);
  vec4 baseColor = unpackBaseColor(positionColorRaw);
  vec3 sh1[3] = unpackSh1(shRG, shB);
  vec3 sh2[5] = unpackSh2(shRG, shB);
  vec3 sh3[7] = unpackSh3(shRG, shB);

  // inverse(mv)[3].xyz is the camera position in model space, should probably just pass this into
  // the shader.
  fragColor = (1.0 / 255.0) * baseColor
    + vec4(shOffset(inverse(mv)[3].xyz, splatPos.xyz, sh1, sh2, sh3), 0.0);
}
