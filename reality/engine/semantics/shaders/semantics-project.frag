#version 320 es

precision mediump float;

in vec2 texUv;
// clip space coordinate that is associated with the source camera
in vec4 sourceClipPos;

uniform sampler2D colorSampler;
uniform sampler2D semSampler;

out vec4 fragColor;

void main() {
  vec4 prevCol = texture(colorSampler, texUv);
  vec3 clipPos = sourceClipPos.xyz / sourceClipPos.w;
  if (clipPos.x < -1.0f || clipPos.x > 1.0f ||
      clipPos.y < -1.0f || clipPos.y > 1.0f) {
    fragColor = prevCol;
  } else {
    vec2 ratio = vec2(0.5f, -0.5f);
    vec2 offset = vec2(0.5f, 0.5f);
    vec2 uv = clipPos.xy * ratio + offset;
    vec4 sem = texture(semSampler, uv);
    // TODO(yuyan): Figure out why semi-transparent alpha channel cause flickering.
    // When the intermediate textures have semi-transparent pixels, frames are flickering
    // in aframe-renderer.
    if (prevCol.b == 0.0f) {
      // first time the pixel is seen
      fragColor = sem;
      fragColor.b = max(0.1f, sem.b);
    } else {
      // blend with previous predictions
      float v = max(sem.r, 0.9 * prevCol.r);
      fragColor = vec4(v, v, 1.0f, 1.0f);
    }
  }
}
