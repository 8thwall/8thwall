#version 320 es
#extension GL_GOOGLE_include_directive : enable

in vec4 position;

uniform vec4 bounds;

out vec2 dist;

void main() {
  vec2 center = bounds.xy;
  vec2 radius = bounds.zw;
  dist = (position.xy - center) / radius;
  gl_Position = position;
}
