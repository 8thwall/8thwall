#version 320 es

precision mediump float;

// clip space coordinate that is associated with the source camera
in vec4 sourceClipPos;

uniform sampler2D colorSampler;

out vec4 fragColor;

void main() {
  vec3 clipPos = sourceClipPos.xyz / sourceClipPos.w;
  if (clipPos.x < -1.0f || clipPos.x > 1.0f ||
      clipPos.y < -1.0f || clipPos.y > 1.0f) {
    fragColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
  } else {
    vec2 ratio = vec2(0.5f, -0.5f);
    vec2 offset = vec2(0.5f, 0.5f);
    vec2 uv = clipPos.xy * ratio + offset;
    fragColor = texture(colorSampler, uv);
  }
}
