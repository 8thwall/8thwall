#version 320 es

precision mediump float;

in vec2 texUv;

uniform sampler2D colorSampler;
uniform float opacity;

out vec4 fragColor;

void main() {
  fragColor = texture(colorSampler, texUv) * vec4(1.0, 1.0, 1.0, opacity);
}
