#version 320 es

precision lowp float;

in vec4 splatColor;
in vec2 splatEccentricity;
in float maxSqEccentricity;

out vec4 fragColor;

void main() {
  float sqEccentricity = dot(splatEccentricity, splatEccentricity);
  // If the pixel value contribution would be less than 1, discard.
  if (sqEccentricity > maxSqEccentricity) discard;
  fragColor = vec4(splatColor.rgb, exp(-0.5 * sqEccentricity) * splatColor.a);
}
