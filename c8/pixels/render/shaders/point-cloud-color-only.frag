#version 320 es

precision mediump float;

in vec4 vertColor;

uniform float opacity;

out vec4 fragColor;

void main() {
  if (length(gl_PointCoord - vec2(0.5)) > 0.5) {  //outside of circle radius
    discard;
  }
  fragColor = vertColor * vec4(1.0, 1.0, 1.0, opacity);
}
