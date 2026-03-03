#version 320 es

precision mediump float;

in vec4 vertColor;

uniform float opacity;

out vec4 fragColor;

void main() {
  fragColor = vertColor * vec4(1.0, 1.0, 1.0, opacity);
}
