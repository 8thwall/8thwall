#ifndef C8_PIXELS_RENDER_SHADERS_OCCLUSION_FRAG_
#define C8_PIXELS_RENDER_SHADERS_OCCLUSION_FRAG_

uniform sampler2D depthSampler;
uniform float hasDepthTexture;
uniform float occludeTol;

in vec4 projPos;
in float fragDepth;
in float outSize; // Set to pointSize / mvz, only used for gl_point pointclouds

float occlusion() {
  float visibility = 1.f;
  if (hasDepthTexture != 0.f) {
    // Depth texture is flipped vertically
    // +y in device coords is up, and down in normalized texture coords, so flip it
    vec2 depthUv = (projPos.xy / projPos.w);
    depthUv.y = -depthUv.y;
    depthUv = depthUv * 0.5 + 0.5;

    float depth = texture(depthSampler, depthUv).r;

    // Fade out as fragment fails depth test instead of just discarding
    visibility = clamp(1.f - (fragDepth - depth) / occludeTol, 0.f, 1.f);
  }
  return visibility;
}

float occlusionPointCloud() {
  float visibility = 1.f;
  if (hasDepthTexture != 0.0f) {
    // Soft occlusion if there is a depth texture
    // Fade out as it fails the depth test instead of discarding the fragment
    // Get current fragment's depth texture uv coord
    vec2 depthUv = (projPos.xy / projPos.w);
    vec2 depthTexelSize = 1.f / vec2(textureSize(depthSampler, 0));
    vec2 offset = (gl_PointCoord - vec2(0.5f)) * outSize / 2.f;
    depthUv += vec2(offset.x, -offset.y); // Per-fragment occlusion
    // Depth texture is flipped vertically
    depthUv.y = -depthUv.y; // +y in device coords is up, and down in texture coords, so flip it
    depthUv = depthUv * 0.5f + 0.5f; // Go from [-1, 1] clip space to [0, 1] texture coords
    float depth = texture(depthSampler, depthUv).r;
    visibility = clamp(1.f - (fragDepth - depth) / occludeTol, 0.f, 1.f);
  }
  return visibility;
}

#endif
