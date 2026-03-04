#version 320 es

precision mediump float;

in vec2 dist;
uniform float elliptical;

out vec4 fragColor;

const float ALPHA = 0.9;
const float GRAY = 0.25 / ALPHA;

void main() {
  if (elliptical == 0.0) {
    // square
    if (max(abs(dist.x), abs(dist.y)) <= 1.0) {
      discard;
    }
  } else {
    // elliptical
    if (dot(dist, dist) <= 1.0) {
      discard;
    }
  }

  fragColor = vec4(GRAY, GRAY, GRAY, ALPHA);
}
